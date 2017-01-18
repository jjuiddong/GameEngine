
-- version 14
-- start frame, end frame 정확히 저장되게 수정함.



global Outfile -- 저장될 file
global g_MeshFilePath -- mesh file path
global g_AniFilePath -- animation file path
global g_fEpsilon = 0.0001
global g_nSampling = 1 -- 샘플링 간격 (default = 1)
global g_TextureOptimize = true
global g_WriteMesh = true
global g_WriteAni = false
global g_animationStartFrame = 0;
global g_animationEndFrame = 0;

global g_varSetMeshes = #()
global g_varSetBones = #()
global g_varSetMaterials = #()

-- UI
global g_progBar



-- [MAX quaterion]            [D3D quaternion]
-- x,y,z,w                    -x,-z,-y,w

-- [D3D matrix]               [MAX matrix]
-- _11 _12 _13 0              _11 _13 _12 0
-- _21 _22 _23 0       <->  _31 _33 _32 0
-- _31 _32 _33 0              _21 _23 _22 0
-- _41 _42 _43 1              _41 _43 _42 1

-- Basic type definitions
struct MATRIX_D3D
( 
	_11, _12, _13, _14,
	_21, _22, _23, _24,
	_31, _32, _33, _34,
	_41, _42, _43, _44
)
	

struct PHYSIQDATA
(
	vtxidx,
	varSetVtxWeight
)

struct WEIGHT
(
	BoneName,
	Weight
)

struct ATTRIBUTE2
(
	attribId,
	faceStart,
	faceCount,
	vertexStart,
	vertexCount	
)



fn ConvertToD3D_POS max_pos =
(
	local d3d_pos = [0,0,0]
	d3d_pos.x = max_pos.x
	d3d_pos.y = max_pos.z
	d3d_pos.z = max_pos.y
	
	d3d_pos
)


fn ConvertToD3D_INDEX max_index =
(
	local d3d_index = [0,0,0]
	d3d_index.x = max_index.x - 1
	d3d_index.y = max_index.z - 1
	d3d_index.z = max_index.y - 1

	d3d_index
)

-- MAX의 텍스쳐 좌표를 D3D의 uv좌표로 바꿔준다.
fn ConvertToD3D_UV max_pos =
(
	if max_pos == undefined then return undefined
	local d3d_uv = [0,0]

	d3d_uv.x = max_pos.x
	d3d_uv.y = 1.0 - max_pos.y

	d3d_uv
)

fn ConvertToD3D_QUAT max_quat =
(
	local	d3d_quat = quat 0 0 0 0

	d3d_quat.x = -max_quat.x
	d3d_quat.y = -max_quat.z
	d3d_quat.z = -max_quat.y
	d3d_quat.w =  max_quat.w
	
	d3d_quat
)

fn ConvertToD3D_SCALE max_scale =
(
	local d3d_scale = [0,0,0]

	d3d_scale.x = max_scale.x
	d3d_scale.y = max_scale.z
	d3d_scale.z = max_scale.y
	
	d3d_scale
)



-- MAX의 TM을 D3D의 TM으로 바꿔준다.
fn ConvertToD3D_TM max_tm =
(
	local	d3d_tm = MATRIX_D3D	0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

	d3d_tm._11 = max_tm.row1.x
	d3d_tm._12 = max_tm.row1.z
	d3d_tm._13 = max_tm.row1.y
	d3d_tm._14 = 0.0

	d3d_tm._21 = max_tm.row3.x
	d3d_tm._22 = max_tm.row3.z
	d3d_tm._23 = max_tm.row3.y
	d3d_tm._24 = 0.0

	d3d_tm._31 = max_tm.row2.x
	d3d_tm._32 = max_tm.row2.z
	d3d_tm._33 = max_tm.row2.y
	d3d_tm._34 = 0.0

	d3d_tm._41 = max_tm.row4.x
	d3d_tm._42 = max_tm.row4.z
	d3d_tm._43 = max_tm.row4.y
	d3d_tm._44 = 1.0

	d3d_tm
)


fn FindObject name =
(
	for o in objects do
	(
		if o.name == name then 
			return o
	)
	
	undefined
)


-- 정의 : Biped나 IK사용시 키값을 추출하기 위해서 TM샘플링을 사용하고 있다.
--        이때, snapshot으로 임시 오브젝트를 생성하여 TM값을 샘플링(Sampling or Dump)한다.
fn DumpObjects obj =
(
	origbiped = #()
	snapset = #()

	format "DumpObjects Root : %...\n" obj.name
	a = "$'" + obj.name + "'"
	select (execute (a + "...*"))
	selectionSets ["OrigBiped"] = selection

	for i in selection do 
	(
	    append origbiped i
	    b = snapshot i name:(i.name as string + "_snap")
	    b.parent = undefined
	    b.transform = i.transform
	    b.position.track = bezier_position()
	    b.rotation.track = tcb_rotation()
	    append snapset b
	)
	selectionSets ["Snapset"] = Snapset

	for i in 1 to snapset.count do
	(
	    try
	    ( 
			snapset[i].parent = execute ("$'" + origbiped[i].parent.name + "_snap'")
			snapset[i].position.track = bezier_position()
	   	   	snapset[i].rotation.track = tcb_rotation()
	    )
	    catch()
	)

	animate on
	undo off
	for t = animationRange.start to animationRange.end by g_nSampling do at time t
	(
	    for i in 1 to Snapset.count do
	    (
	        snapset[i].transform = origbiped[i].transform
	    )
	)
	
	snapset
)


-- {0: Not Bone} {1: Bone}
fn IsBone obj = 
(
	if (classof obj == BoneGeometry) or 
		(classof obj == Biped_Object) or
		(classof obj == dummy)
	then
	(
		return 1
	)
	return 0
)


fn IsExportType obj = 
(
	if ((editable_mesh == (classof obj)) or 
		(editable_poly == (classof obj)) or
		(EditablePolyMesh  == (classof obj)) or
		(PolyMeshObject  == (classof obj)) or
		(Sphere == (classof obj)) or
		(Box == (classof obj)) or
		(Cone == (classof obj)) or
		(Cylinder == (classof obj)) or
		(GeoSphere == (classof obj)) or
		(Torus == (classof obj)) or
		(Pyramid == (classof obj)) or
		(Teapot == (classof obj)) or
		(Plane == (classof obj))
			) then
	(
		return 1
	)
	
	return 0	
)


fn GetBoneId name =
(
	for i=1 to g_varSetBones.count do
	(
		if name == g_varSetBones[i].name then
			return i as integer
	)
	return 0
)


fn GetMeshId name =
(
	for i=1 to g_varSetMeshes.count do
	(
		if name == g_varSetMeshes[i].name then
			return i as integer
	)
	return 0
)


fn GetObjectId obj = 
(
	if obj == undefined then 
		return 0

	id = 0

	if ( 1 == IsBone(obj) ) then
	(
		id = (GetBoneId obj.name)
	)
	else
	(
		id = (GetMeshId obj.name)
	)

	id
)


fn GetParentObjectId obj = 
(
	if obj.parent == undefined then 
		return 0

	if ( 1 == IsBone(obj) ) then
	(
		return (GetBoneId obj.parent.name)
	)
	else
	(
		return (GetMeshId obj.parent.name)
	)

	return 0
)



-- Hierarchy를 고려한 Local Transform Matrix(TM)를 리턴한다.
fn GetLocalTM obj =
(
	local matLocal = copy obj.transform
	if obj.parent != undefined then
	(
		matLocal = obj.transform * (inverse obj.parent.transform)
	)

	matLocal
)


fn CollectObject = 
(
	format "- CollectObject count = %\n" objects.count
	
	g_varSetMeshes = #()
	
	for i=1 to objects.count do
	(
		if 1 == (IsBone objects[i]) then
		(
			format "collect bone %\n" objects[i].name
			append g_varSetBones objects[i]
		)
		else if ( (targetCamera ==  (classof objects[i])) or
					(omniLight ==  (classof objects[i])) or
					(targetObject ==  (classof objects[i]))
				) then
		(
			-- dummy는 처리되지 않는다.
			format "ignore object %\n" objects[i].name
		)		
		else if 1 == (IsExportType objects[ i]) then
		(			
			format "collect mesh %\n" objects[i].name
			append g_varSetMeshes objects[i]
		)
		else
		(
			format "ignore object %\n" objects[i].name
		)
	)
		
)


fn GetMtrlId obj =
(
	if classOf obj.material == UndefinedClass then
	(
		return 0
	)
	
	if classOf obj.material == Standardmaterial then
	(
		for i=1 to g_varSetMaterials.count do
		(
			if obj.material == g_varSetMaterials[i] then
			(
				return (i as integer)
			)
		)
	)
	
	return 0
)


fn GetMtrlIdFromMaterial mtrl =
(
	if classOf mtrl == UndefinedClass then
	(
		return 0
	)
	
	for i=1 to g_varSetMaterials.count do
	(
		if mtrl == g_varSetMaterials[i] then
		(
			return (i as integer)
		)
	)
	
	return 0
)


fn GetMtrlCount obj = 
(
	format "material type = %\n" obj.material
	
	if classOf obj.material == UndefinedClass then
	(
		return 0
	)
	
	if classOf obj.material == Standardmaterial then
	(
		return 1	
	)
	else if classof obj.material == multimaterial then
	(
		return (obj.material.count as integer)
	)
	
	return 0	
)


fn CollectMaterial = 
(
	temp = standard()
	Append g_varSetMaterials temp

	for i=1 to objects.count do
	(
		if objects[i].material == undefined do continue

		if classof objects[i].material == standardmaterial then
		(
			Append g_varSetMaterials objects[i].material
		)

		if classof objects[i].material == multimaterial then
		(
			for j=1 to objects[i].material.count do
			(
				Append g_varSetMaterials objects[i].material[j]
			)
		)
	)

	varSetMaterials
)


fn WriteMaterial = 
(
	format "MATERIALGROUP = %\n" g_varSetMaterials.count to:Outfile

	for i=1 to g_varSetMaterials.count do
	(
		format "MATERIAL %\n" (i-1) to:Outfile

		format "DIFFUSE % % % 1\n"  (g_varSetMaterials[i].Diffuse.r/255) (g_varSetMaterials[i].Diffuse.g/255) (g_varSetMaterials[i].Diffuse.b/255) to:Outfile
		format "AMBIENT % % % 1\n"  (g_varSetMaterials[i].Ambient.r/255) (g_varSetMaterials[i].Ambient.g/255) (g_varSetMaterials[i].Ambient.b/255) to:Outfile
		format "SPECULAR % % % 1\n" (g_varSetMaterials[i].Specular.r/255) (g_varSetMaterials[i].Specular.g/255) (g_varSetMaterials[i].Specular.b/255) to:Outfile
		format "EMISSIVE % % % 0\n" (g_varSetMaterials[i].Diffuse.r/255) (g_varSetMaterials[i].Diffuse.g/255) (g_varSetMaterials[i].Diffuse.b/255) to:Outfile
		format "POWER %\n"  g_varSetMaterials[i].SpecularLevel to:Outfile

		-- Diffuse Map
		if g_varSetMaterials[i].DiffuseMap != undefined then
		(
			format "TEXTURE %\n" g_varSetMaterials[i].DiffuseMap.filename to:Outfile
		)
		else
		(
			format "TEXTURE  \n"  to:Outfile
		)
		
		-- SpecularMap
		if g_varSetMaterials[i].SpecularMap != undefined then
		(
			format "SPECULAR_TEXTURE %\n" g_varSetMaterials[i].SpecularMap.filename to:Outfile
		)
		else
		(
			format "SPECULAR_TEXTURE  \n"  to:Outfile
		)

		-- BumpMap		
		if g_varSetMaterials[i].BumpMap != undefined then
		(
			format "BUMP_TEXTURE %\n" g_varSetMaterials[i].BumpMap to:Outfile
		)
		else
		(
			format "BUMP_TEXTURE  \n"  to:Outfile
		)		
		
		
	)
)


fn CollectVertex obj = 
(
	varSetVertex= #()
	
	if (dummy == (classof obj)) then
	(
		return undefined
	)

		
	if (obj.numTVerts >= obj.numVerts) and g_TextureOptimize then
	(
		for i=1 to obj.numTVerts do
		(
			vnt = getvert obj 1
			append varSetVertex vnt
		)
		
		for i=1 to obj.faces.count do
		(
			local tvfaceidx = getTVFace obj i
			local faceidx = getFace obj i

			varSetVertex[ tvfaceidx[ 1]] = copy (getvert obj faceidx[ 1])
			varSetVertex[ tvfaceidx[ 2]] = copy (getvert obj faceidx[ 2])
			varSetVertex[ tvfaceidx[ 3]] = copy (getvert obj faceidx[ 3])
		)
	)
	else
	(
		for i=1 to obj.numverts do
		(
			v = getvert obj i
			append varSetVertex v
		)
	)
	
	varSetVertex
)


fn CollectNormals obj = 
(
	varSetNormal = #()	

	if (dummy == (classof obj)) then
	(
		return undefined
	)
	
	
	for i=1 to obj.numfaces do
	(
		local normal = normalize (getFaceNormal obj i)
		append varSetNormal normal
	)
	varSetNormal
)


fn CollectIndex obj = 
(
	varSetIndex = #()
	
	if (dummy == (classof obj)) then
	(
		return undefined
	)

	
	if (obj.numTVerts >= obj.numVerts) and g_TextureOptimize then
	(
		print "obj.numTVerts >= obj.numVerts"
		
		for i=1 to obj.numfaces do
		(
			local v = copy (getTVFace obj i)
			append varSetIndex v
		)
	)
	else
	(
		for i=1 to obj.numfaces do
		(
			local v = copy (getface obj i)
			append varSetIndex v
		)
	)
	
	varSetIndex
)


fn CollectFaceMaterial obj = 
(
	varSetAttribute = #()
	
	mtrlCount = (GetMtrlCount obj)
	if (mtrlCount <= 1) then
	(
		return undefined
	)
	
	if (dummy == (classof obj)) then
	(
		return undefined
	)
	
	if (obj.numfaces == 0) then
	(
		return undefined
	)

	

	firstId = (getFaceMatID obj 1)
	faceStart = 1
	
	for i=1 to obj.numfaces do
	(
		id = (getFaceMatID obj i)
		
		if (id == firstId) then
		(
			-- nothing			
		)
		else
		(
			count = i - faceStart
			
			local attribute = ATTRIBUTE2 0 0 0 0 0
			attribute.attribId = firstId
			attribute.faceStart = faceStart
			attribute.faceCount = count
			attribute.vertexStart = 0
			attribute.vertexCount = 0
			
			
			firstId = id	
			faceStart = i
			
			append varSetAttribute attribute
		)

	)

	-- last
	count = obj.numfaces - faceStart
	
	local attribute = ATTRIBUTE2 0 0 0 0 0
	attribute.attribId = firstId
	attribute.faceStart = faceStart
	attribute.faceCount = count
	attribute.vertexStart = 0
	attribute.vertexCount = 0	
	
	append varSetAttribute attribute
	
	
	varSetAttribute
)


fn WriteVertex obj = 
(
	varSetVertice = CollectVertex obj
	
	if (varSetVertice == undefined) then
	(
		format "VERTEX = 0\n" to:Outfile
	)
	else
	(
		format "VERTEX = %\n" varSetVertice.count to:Outfile
		for i=1 to varSetVertice.count do
		(
			vtx = (ConvertToD3D_POS varSetVertice[ i]) 
			format "% % %\n" vtx.x vtx.y vtx.z to:Outfile
		)
	)
)


fn WriteNormal obj = 
(
	varSetNormal = CollectNormals obj

	if (varSetNormal == undefined) then
	(
		format "FACE-NORMAL = 0\n" to:Outfile
	)
	else
	(
		format "FACE-NORMAL = %\n" varSetNormal.count to:Outfile
		for i=1 to varSetNormal.count do
		(
			norm = (ConvertToD3D_POS varSetNormal[ i])
			format "% % %\n" norm.x norm.y norm.z to:Outfile
		)
	)
)


fn WriteIndex obj = 
(
	varSetIndex = CollectIndex obj
	
	if (varSetIndex == undefined) then
	(
		format "INDEX = 0\n" to:Outfile
	)
	else
	(
		format "INDEX = %\n" varSetIndex.count to:Outfile
		for i=1 to varSetIndex.count do
		(
			index = (ConvertToD3D_INDEX varSetIndex[ i])
			format "% % %\n" (index.x as integer) (index.y as integer) (index.z as integer) to:Outfile
		)
	)
)


fn WriteTVertTVFace obj = 
(
	format "TEXTURE-VERTEX = %\n" obj.numTVerts to:Outfile
	for i=1 to obj.numTVerts do
	(
		local tex = getTvert obj i
		local uv = (ConvertToD3D_UV tex)
		format "% %\n" uv.x  uv.y to:Outfile
	)
		
	format "TEXTURE-FACE = %\n" obj.faces.count to:Outfile	
	for i=1 to obj.faces.count do
	(
		local tvfaceidx = getTVFace obj i
		local index = (ConvertToD3D_INDEX tvfaceidx)
		format "% % %\n" (index.x as integer) (index.y as integer) (index.z as integer) to:Outfile
	)	
)


fn WriteMeshInfo obj = 
(
	format "MESH_NAME = %\n" obj.name to:Outfile
	
	mtrlCount = (GetMtrlCount obj)

	if (0 == mtrlCount) then
	(
		format "MATERIAL_COUNT = 1\n" to:Outfile		
		format "MATERIAL_ID = 0\n" to:Outfile
	)
	else if (1 == mtrlCount) then
	(
		format "MATERIAL_COUNT = %\n" mtrlCount to:Outfile		
		format "MATERIAL_ID = %\n" ((GetMtrlId obj)-1) to:Outfile
	)
	else
	(
		format "MATERIAL_COUNT = %\n" mtrlCount to:Outfile
		for i=1 to obj.material.count do
		(
			format "MATERIAL_ID = %\n" ((GetMtrlIdFromMaterial obj.material[ i])-1) to:Outfile
		)
	)

)


fn WriteFaceMaterialId obj = 
(
	varSetAttribute = CollectFaceMaterial obj
	
	if (varSetAttribute == undefined) then
	(
		format "ATTRIBUTE_COUNT = 0\n" to:Outfile
	)
	else
	(
		format "ATTRIBUTE_COUNT = %\n" varSetAttribute.count to:Outfile
		for i=1 to varSetAttribute.count do
		(
			format "ATTRIBUTE_ID = %\n" (varSetAttribute[ i].attribId - 1) to:Outfile
			format "ATTRIBUTE_FACESTART = %\n" (varSetAttribute[ i].faceStart - 1) to:Outfile
			format "ATTRIBUTE_FACECOUNT = %\n" varSetAttribute[ i].faceCount to:Outfile
			format "ATTRIBUTE_VERTEXSTART = %\n" (varSetAttribute[ i].vertexStart - 1) to:Outfile
			format "ATTRIBUTE_VERTEXCOUNT = %\n" varSetAttribute[ i].vertexCount to:Outfile
		)
	)
	
)


fn WriteVtxNormTex obj = 
(
	WriteMeshInfo obj
	WriteVertex obj
	WriteIndex obj
	WriteNormal obj
	WriteTVertTVFace obj
	WriteFaceMaterialId obj
)


fn WriteAni obj = 
(
	postrack = #()
	rottrack = #()
	scaletrack = #()
	
	-- Collect Position Animation
	if obj.pos.isAnimated then
	(
		obj.pos.controller = tcb_position()
		local keys = obj.pos.controller.keys
		local oldkey

		start = 1
		for i=start to keys.count do
		(
			-- 전의 키와 같다면 넘어간다.
			if i > start then
			(
				if abs (oldkey.x - keys[i].value.x) < g_fEpsilon and
				   abs (oldkey.y - keys[i].value.y) < g_fEpsilon and
				   abs (oldkey.z - keys[i].value.z) < g_fEpsilon do 
					continue
			)
			oldkey = keys[i].value
			append postrack keys[i]
		)
	)
	
	
	-- Collect Rotation Animation
	if obj.rotation.isAnimated then
	(
		obj.rotation.controller = tcb_rotation()
		local keys = obj.rotation.controller.keys
		
		for i=1 to keys.count do
		(
			-- 단위 쿼터니온이면 넘어간다.
			local k = keys[i].value as quat
			if i > 1 then
			(
				if abs (1 - k.w) < g_fEpsilon and
				   abs (0 - k.x) < g_fEpsilon and
				   abs (0 - k.y) < g_fEpsilon and
				   abs (0 - k.z) < g_fEpsilon do continue
			)
			append rottrack keys[i]
		)
	)	
	
	
	-- Collect Scale Animation
	if obj.scale.isAnimated then
	(
		obj.scale.controller = tcb_scale()
		local keys = obj.scale.controller.keys
		local oldkey

		start = 1
		for i=start to keys.count do
		(
			-- 전의 키와 같다면 넘어간다.
			if i > start then
			(
				if abs (oldkey.x - keys[i].value.x) < g_fEpsilon and
				   abs (oldkey.y - keys[i].value.y) < g_fEpsilon and
				   abs (oldkey.z - keys[i].value.z) < g_fEpsilon do continue
			)
			oldkey = keys[i].value
			append scaletrack keys[i]
		)
	)
	
	
	
	local start, end
	
	if (g_animationStartFrame == 0) then
	(
		start = animationRange.start
	)
	else
	(
		start = g_animationStartFrame
	)
	
	if (g_animationEndFrame == 0) then
	(
		end = animationRange.end
	)
	else
	(
		start = g_animationEndFrame
	)
	
	
	format "ANI_RANGE = % %\n" start end to:Outfile	
	
	-- Write Position
	format "KEY_POS = %\n" postrack.count to:Outfile
	
	for i = 1 to postrack.count do
	(
		local t = postrack[i].time
		if t < 0 then 
			t = 0
		
		pos = (ConvertToD3D_POS postrack[i].value)
		format "FRAME_POS % % % % \n" t pos.x pos.y pos.z to:Outfile
	)
	
	

	-- Write Rotation
	rotcount = rottrack.count

	format "KEY_ROT = %\n" rotcount to:Outfile
	local multiquat = quat 0 0 0 1
	for i = 1 to rottrack.count do
	(
		-- 단위 쿼터니온이면 넘어간다.
		local k = rottrack[i].value as quat
		if i == 1 then k = (quat 0 0 0 1) 
		multiquat = k * multiquat

		local t = rottrack[i].time
		if t < 0 then t = 0
		q = ConvertToD3D_QUAT multiquat

		format "FRAME_ROT % % % % %\n" t q.x q.y q.z q.w to:Outfile
	)
	
	

	-- Write Scaling
	format "KEY_SCALE = %\n" scaletrack.count to:Outfile
	for i = 1 to scaletrack.count do
	(
		local t = scaletrack[i].time
		if t < 0 then t = 0
		scale = (ConvertToD3D_SCALE scaletrack[i].value)
		format "FRAME_SCALE % % % % \n" t scale.x scale.y scale.z to:Outfile
	)

)


fn WriteBoneInfo obj = 
(

	format "BONE = %\n" ((GetObjectId obj)-1) to:Outfile
	format "BONE_NAME = %\n" obj.name to:Outfile
	format "PARENT_ID = %\n" ((GetParentObjectId obj)-1) to:Outfile

)


fn WriteBoneTM obj  = 
(
	format "LOCALTM \n" to:Outfile
	tm = (ConvertToD3D_TM (GetLocalTM obj) )
	format "MAT % % % %\n" tm._11 tm._12 tm._13 tm._14 to:Outfile
	format "MAT % % % %\n" tm._21 tm._22 tm._23 tm._24 to:Outfile
	format "MAT % % % %\n" tm._31 tm._32 tm._33 tm._34 to:Outfile
	format "MAT % % % %\n" tm._41 tm._42 tm._43 tm._44 to:Outfile

	format "WORLDTM \n" to:Outfile
	tm = (ConvertToD3D_TM (obj.transform) )
	format "MAT % % % %\n" tm._11 tm._12 tm._13 tm._14 to:Outfile
	format "MAT % % % %\n" tm._21 tm._22 tm._23 tm._24 to:Outfile
	format "MAT % % % %\n" tm._31 tm._32 tm._33 tm._34 to:Outfile
	format "MAT % % % %\n" tm._41 tm._42 tm._43 tm._44 to:Outfile
)



fn CollectVertexWeightFromSkin obj = 
(
	print " ---------- CollectVertexWeigthBySkin ----------- "

	unfreeze obj

	local sm
	max modify mode
	sm = obj.modifiers[#skin]
	if sm == undefined then return undefined
		
	modPanel.setCurrentObject sm
	if obj.numverts == 0 then return undefined
		
	local nBoneCount = SkinOps.GetNumberBones sm
	local nVertCount = SkinOps.GetNumberVertices sm
	if nVertCount == 0 then return undefined

	setVarPhysique = #()
	
	for i=1 to nVertCount do
	(
		local phys = PHYSIQDATA #()
		local varSW = #()

		local nVertWeightCount = SkinOps.GetVertexWeightCount sm i
		if 0 < nVertWeightCount then
		(
			for k=1 to nVertWeightCount do
			(
				local vw = WEIGHT 0 0
				nBoneID = (SkinOps.GetVertexWeightBoneID sm i k)
				vw.BoneName = (SkinOps.GetBoneName sm nBoneID 0)
				vw.Weight = (SkinOps.GetVertexWeight sm i k)

				if 0 == (GetBoneId vw.BoneName) then
				(
					continue
				)

				append varSW vw
			)

			phys.vtxidx = i
			phys.varSetVtxWeight = varSW
			append setVarPhysique phys
		)
		else
		(
			phys.vtxidx = i
			phys.varSetVtxWeight = undefined
			append setVarPhysique phys
		)
		
	)


	setVarRet = #()
	undo on
	(
		ConvertToMesh obj

		if (obj.numTVerts >= obj.numVerts) and g_TextureOptimize then
		(
			for i=1 to obj.numTVerts do
			(
				local phys = PHYSIQDATA #()
				append setVarRet phys
			)

			for i=1 to obj.faces.count do
			(
				local tvfaceidx = getTVFace obj i
				local faceidx = getFace obj i
				
				phys1 = copy (setVarPhysique[ faceidx[ 1]])
				phys1.vtxidx = tvfaceidx[ 1]
				setVarRet[ tvfaceidx[ 1]] = phys1

				phys2 = copy (setVarPhysique[ faceidx[ 2]])
				phys2.vtxidx = tvfaceidx[ 2]
				setVarRet[ tvfaceidx[ 2]] = phys2

				phys3 = copy (setVarPhysique[ faceidx[ 3]])
				phys3.vtxidx = tvfaceidx[ 3]
				setVarRet[ tvfaceidx[ 3]] = phys3
			)
		)
		else
		(
			setVarRet = setVarPhysique
		)
	)
	max undo	
	--setVarRet = setVarPhysique

	setVarRet		
)



fn WriteVertexWeightFromSkin obj = 
(
	print "---------- Write Vertex Weight ----------"
	format  "name = %\n"  obj.name
	format  "skin modifier = %\n" obj.modifiers[#skin]
	format  "physique modifier = %\n" obj.modifiers[#physique]	
	
	ret = undefined

	ret = (CollectVertexWeightFromSkin obj)

	if ret == undefined then
	(
		print "######## no weight ############# "
		format "VERTEXWEIGHT = 0\n" to:Outfile
		return undefined
	)

	format "VERTEXWEIGHT_COUNT = %\n" ret.count to:Outfile

	for i=1 to ret.count do
	(
		if ret[i].varSetVtxWeight == undefined then
		(
			--format "VTXWEIGHT_VERTEX_COUNT = % 0\n" ((ret[i].vtxidx-1) as integer) to:Outfile
			format "VTXWEIGHT_VERTEX_COUNT = 0 0\n" to:Outfile
		)
		else
		(
			format "VTXWEIGHT_VERTEX_COUNT = % %\n" ((ret[i].vtxidx-1) as integer) (ret[i].varSetVtxWeight.count) to:Outfile
			for j=1 to ret[i].varSetVtxWeight.count do
			(
				format "% %\n" ((GetBoneId ret[i].varSetVtxWeight[j].BoneName)-1) (ret[i].varSetVtxWeight[j].Weight) to:Outfile
			)
		)
	)	
)


fn ExporterStart = 
(
	CollectObject()
	CollectMaterial()

	
	if (g_WriteMesh == true) then
	(
		Outfile = createfile g_MeshFilePath
		
		format "EXPORTER_V13\n" to:Outfile
		
		format "MESH_EXPORT\n" to:Outfile
		
		
		-- Write Material
		print "---------- Write Material ----------"
		WriteMaterial()

		g_progBar.value = 0 -- 프로그레스바 초기화.
		currentProgress = 0 as float
		totalProgress = g_varSetMeshes.count  +  g_varSetBones.count;
		
		
		-- Write Geometric Mesh
		print "---------- Write Geometric ----------"
		format "GEOMOBJECT_COUNT = %\n" g_varSetMeshes.count to:Outfile
		for i=1 to g_varSetMeshes.count do
		(
			format "write mesh name=%, id=%\n" g_varSetMeshes[ i].name i
			
			-- vertex, index값은 EditMesh모드일때만 얻어올수 있어서 ConvertToMesh 함수를
			-- 사용했다. 문제는 EditMesh모드일때 VertexWeight값을 얻을 수 없어서,
			-- Vertex, index값을 얻은 후 undo명령어로 이전상태로 바꾼후 얻어온다.
			undo on
			(
				ConvertToMesh g_varSetMeshes[ i]
				WriteVtxNormTex g_varSetMeshes[ i]
			)
			max undo

			WriteVertexWeightFromSkin g_varSetMeshes[ i]
			
			
			-- 프로그레스바 이동.
			currentProgress = currentProgress + 1
			g_progBar.value = 100 * (currentProgress / totalProgress)
			
		)
		
		
		print "---------- Write Bone ----------"
		format "BONEOBJECT_COUNT = %\n" g_varSetBones.count to:Outfile
		for i=1 to g_varSetBones.count do
		(
			format "name=%, id=%\n" g_varSetBones[ i].name i
			-- vertex, index값은 EditMesh모드일때만 얻어올수 있어서 ConvertToMesh 함수를
			-- 사용했다. 문제는 EditMesh모드일때 VertexWeight값을 얻을 수 없어서,
			-- Vertex, index값을 얻은 후 undo명령어로 이전상태로 바꾼후 얻어온다.
			undo on
			(
				ConvertToMesh g_varSetBones[ i]
				WriteVertex g_varSetBones[ i]
				WriteIndex g_varSetBones[ i]
				WriteNormal g_varSetBones[ i]
			)
			max undo
			

			-- Write Bone Hierarchy
			WriteBoneInfo g_varSetBones[ i] 
			
			-- Write Bone localTM, worldTM
			WriteBoneTM g_varSetBones[ i] 
			

			-- 프로그레스바 이동.			
			currentProgress = currentProgress + 1
			g_progBar.value = 100 * (currentProgress / totalProgress)
		)
		
		close Outfile
	)
		
	
	-- Write Animation
	if (g_WriteAni == true) then
	(
		Outfile = createfile g_AniFilePath
		
		format "EXPORTER_V13\n" to:Outfile
		
		format "ANIMATION_EXPORT\n" to:Outfile
		
		
		-- 프로그레스바 초기화.
		g_progBar.value = 0 
		currentProgress = 0 as float
		
		
		if (g_varSetBones.count <= 0) then
		(
			print "---------- Write Mesh Animation ----------"
			format "ANIMATION_COUNT = %\n" g_varSetMeshes.count to:Outfile
		
			-- 프로그레스바 초기화.			
			totalProgress = g_varSetMeshes.count ;

			
			for i=1 to g_varSetMeshes.count do
			(
				WriteAni g_varSetMeshes[ i]
				

				-- 프로그레스바 이동.			
				currentProgress = currentProgress + 1
				g_progBar.value = 100 * (currentProgress / totalProgress)			
			)
		)
		else
		(
			print "---------- Write Bone Animation ----------"
			
			-- 프로그레스바 초기화.			
			totalProgress = g_varSetBones.count ;

			
			format "ANIMATION_COUNT = %\n" g_varSetBones.count to:Outfile
			for i=1 to g_varSetBones.count do
			(				
				-- snapshot
				DumpObjects g_varSetBones[ i] 
				snapobj = FindObject (g_varSetBones[ i].name + "_snap")
				if snapobj == undefined then 
					continue
				
				WriteAni snapobj
				
				
				-- 프로그레스바 이동.			
				currentProgress = currentProgress + 1
				g_progBar.value = 100 * (currentProgress / totalProgress)				
			)
		)

		close Outfile		
	)

	
	if $ != undefined then deselect $
	select $'*snap'
	delete selection	
)



rollout ui "Exporter" width:387 height:266
(
	button btnRun "DX Exporter 실행" pos:[101,220] width:177 height:30

	-- 메쉬 파일명 입력창
	checkbox chkMesh " " pos:[10,10] width:17 height:17 checked:true
	label lbl1 "파일명:" pos:[30,10] width:70 height:17
	edittext editFilePath "" pos:[100,10] width:250 height:17 enabled:true
	
	-- 텍스쳐 좌표 최적화.
	label lbl2 "Texture Optimize: " pos:[10,35] width:100 height:17
	checkbox chkTextureOptimize " " pos:[100,35] width:17 height:17 checked:true

	-- 애니메이션 파일명 입력창.
	checkbox chkAni " " pos:[10,60] width:17 height:17 checked:false
	label lbl3 "Ani 파일명:" pos:[30,60] width:70 height:17
	edittext editAniFilePath "" pos:[100,60] width:250 height:17 enabled:true
	
	-- 애니메이션 start/end range 입력창.
	label lbl4 "Start Frame:" pos:[30,85] width:70 height:17
	spinner startFrame "" pos:[100,85] width:70 height:17 enabled:true range:[0,1000000,0] type:#integer scale:1 value:0
	label lbl5 "End Frame:" pos:[200,85] width:70 height:17
	spinner endFrame "" pos:[270,85] width:70 height:17 enabled:true range:[0,1000000,0] type:#integer scale:1 value:0

	-- 샘플링 간격.
	spinner spnSamplingGap "" pos:[100,105] width:49 height:16 enabled:true range:[1,100,0] type:#integer scale:1 value:1
	label lbl39 "샘플링간격:" pos:[10,105] width:80 height:17
	
	-- 프로그래스바
	progressBar writeProgress "프로그레스바" pos:[10,190] width:360 height:20 value:0
	
	
	-- UI 창이 열릴때 호출됨
	on ui open do
	(
		editFilePath.text = "mesh.dat"
		editAniFilePath.text = "ani.ani"
	)
	
	on chkTextureOptimize changed state do
	(
		g_TextureOptimize = state
	)	

	on chkMesh changed state do
	(
		g_WriteMesh = state
	)
	
	on chkAni changed state do
	(
		g_WriteAni = state
	)	

	-- DirectX Export
	on btnRun pressed do
	(
		g_MeshFilePath = editFilePath.text
		g_AniFilePath = editAniFilePath.text
		
		g_animationStartFrame = startFrame.value;
		g_animationEndFrame = endFrame.value;
		
		if spnSamplingGap.value < 1 then 
			spnSamplingGap.value = 1
	
		g_nSampling = spnSamplingGap.value
		
		g_progBar = writeProgress
		
		ExporterStart()
	)

)

MainUI = newRolloutFloater "DirectX Exporter" 400 300
addrollout ui MainUI

