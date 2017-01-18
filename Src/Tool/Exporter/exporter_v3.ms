
global Outfile -- 저장될 file
global g_FilePath
global g_fEpsilon = 0.0001

global g_varSetMeshes = #()


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



fn CollectObject = 
(
	g_varSetMeshes = #()

	for i=1 to objects.count do
	(
		append g_varSetMeshes objects[i]
	)
)


fn CollectVertex obj = 
(
	varSetVertex= #()
	for i=1 to obj.numverts do
	(
		v = getvert obj i
		append varSetVertex v
	)
	varSetVertex
)


fn CollectNormals obj = 
(
	varSetNormal = #()
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
	for i=1 to obj.numfaces do
	(
		v = copy (getface obj i)
		append varSetIndex v
	)
	varSetIndex
)


fn WriteVertex obj = 
(
	varSetVertice = CollectVertex obj

	format "VERTEX = %\n" varSetVertice.count to:Outfile
	for i=1 to varSetVertice.count do
	(
		vtx = (ConvertToD3D_POS varSetVertice[ i]) 
		format "% % %\n" vtx.x vtx.y vtx.z to:Outfile
	)
)


fn WriteNormal obj = 
(
	varSetNormal = CollectNormals obj

	format "FACE-NORMAL = %\n" varSetNormal.count to:Outfile
	for i=1 to varSetNormal.count do
	(
		norm = (ConvertToD3D_POS varSetNormal[ i])
		format "% % %\n" norm.x norm.y norm.z to:Outfile
	)
)


fn WriteIndex obj = 
(
	varSetIndex = CollectIndex obj

	format "INDEX = %\n" varSetIndex.count to:Outfile
	for i=1 to varSetIndex.count do
	(
		index = (ConvertToD3D_INDEX varSetIndex[ i])
		format "% % %\n" (index.x as integer) (index.y as integer) (index.z as integer) to:Outfile
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


fn WriteVtxNormTex obj = 
(
	WriteVertex obj
	WriteIndex obj
	WriteNormal obj
	WriteTVertTVFace obj
	format "TEXTURE_FILE = texture file name\n" to:Outfile
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
	
	
	
	-- Write Position
	format "ANI_RANGE = % %\n" animationRange.start animationRange.end to:Outfile
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


fn ExporterStart = 
(
	CollectObject()

	Outfile = createfile g_FilePath
	
	format "EXPORTER_V4\n" to:Outfile	

	for i=1 to g_varSetMeshes.count do
	(
		print g_varSetMeshes[i].name 
		
		-- vertex, index값은 EditMesh모드일때만 얻어올수 있어서 ConvertToMesh 함수를
		-- 사용했다. 문제는 EditMesh모드일때 VertexWeight값을 얻을 수 없어서,
		-- Vertex, index값을 얻은 후 undo명령어로 이전상태로 바꾼후 얻어온다.
		undo on
		(
			ConvertToMesh g_varSetMeshes[ i]
			WriteVtxNormTex g_varSetMeshes[ i]
		)
		max undo
		
		WriteAni g_varSetMeshes[ i]
	)

	close Outfile
)



rollout ui "Exporter" width:387 height:266
(
	button btnRun "DX Exporter 실행" pos:[101,108] width:177 height:30

	-- 파일명 입력창
	label lbl1 "파일명:" pos:[30,13] width:70 height:17
	edittext editFilePath "" pos:[100,10] width:250 height:17 enabled:true

	-- UI 창이 열릴때 호출됨
	on ui open do
	(
		editFilePath.text = "data.dat"
	)

	-- DirectX Export
	on btnRun pressed do
	(
		g_FilePath = editFilePath.text
		ExporterStart()
	)

)

MainUI = newRolloutFloater "Simple Exporter" 400 300
addrollout ui MainUI

