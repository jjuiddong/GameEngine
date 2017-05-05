//
// XFile Volume Shadow Test
//

#include "stdafx.h"
using namespace graphic;


class cViewer : public framework::cGameMain
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float elapseT) override;
	virtual void OnRender(const float elapseT) override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;


protected:
	bool GenerateShadowMesh();
	void RenderShadow();


private:
	cCube3 m_cube2;
	cCube3 m_cube3;
	ID3DXMesh *m_shadowMesh;
	cXFileMesh m_srcMesh;
	cShader m_shader;
	cShader m_terrainShader;
	bool m_isShowShadow;
	bool m_isPause;

	Matrix44 m_rotateTm;
	Matrix44 m_rotateTm2;
	POINT m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK(cViewer);


cViewer::cViewer()
	: m_shadowMesh(NULL)
	, m_isShowShadow(false)
	, m_isPause(false)
{
	m_windowName = L"Shadow Volume X";
	const RECT r = { 0, 0, 1024, 768 };
	m_windowRect = r;
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;
}

cViewer::~cViewer()
{
	SAFE_RELEASE(m_shadowMesh);
	graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	DragAcceptFiles(m_hWnd, TRUE);

	cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = 1024;		//초기 윈도우 가로 크기
	const int WINSIZE_Y = 768;	//초기 윈도우 세로 크기
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.f, 10000.0f);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	GetMainLight().SetPosition(Vector3(-30000, 30000, -30000));
	GetMainLight().SetDirection(Vector3(1, -1, 1).Normal());

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);

	if (!m_shader.Create(m_renderer, "../media/shader/hlsl_shadow.fx", "Shadow"))
		return false;
	if (!m_terrainShader.Create(m_renderer, "../media/shader/hlsl_shadow2.fx", "Shadow"))
		return false;

	m_cube2.InitCube(m_renderer);
	m_cube2.m_tm.SetTranslate(Vector3(3.5f, -2, 2));
	m_cube2.m_mtrl.InitBlue();

	m_cube3.SetCube(m_renderer, Vector3(-1000, -5, -1000), Vector3(1000, -4, 1000));
	m_cube3.m_mtrl.InitGray();
	m_cube3.m_tex = cResourceManager::Get()->LoadTexture(m_renderer, "whitetex.dds");
	return true;
}


struct sShadowVertex
{
	Vector3 Position;
	Vector3 Normal;
	const static D3DVERTEXELEMENT9 Decl[3];
};
const D3DVERTEXELEMENT9 sShadowVertex::Decl[3] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	D3DDECL_END()
};

struct CEdgeMapping
{
	int m_anOldEdge[2];  // vertex index of the original edge
	int m_aanNewEdge[2][2]; // vertex indexes of the new edge
							// First subscript = index of the new edge
							// Second subscript = index of the vertex for the edge

public:
	CEdgeMapping()
	{
		FillMemory(m_anOldEdge, sizeof(m_anOldEdge), -1);
		FillMemory(m_aanNewEdge, sizeof(m_aanNewEdge), -1);
	}
};

#define ADJACENCY_EPSILON 0.0001f


//--------------------------------------------------------------------------------------
// Takes an array of CEdgeMapping objects, then returns an index for the edge in the
// table if such entry exists, or returns an index at which a new entry for the edge
// can be written.
// nV1 and nV2 are the vertex indexes for the old edge.
// nCount is the number of elements in the array.
// The function returns -1 if an available entry cannot be found.  In reality,
// this should never happens as we should have allocated enough memory.
int FindEdgeInMappingTable(int nV1, int nV2, CEdgeMapping* pMapping, int nCount)
{
	for (int i = 0; i < nCount; ++i)
	{
		// If both vertex indexes of the old edge in mapping entry are -1, then
		// we have searched every valid entry without finding a match.  Return
		// this index as a newly created entry.
		if ((pMapping[i].m_anOldEdge[0] == -1 && pMapping[i].m_anOldEdge[1] == -1) ||

			// Or if we find a match, return the index.
			(pMapping[i].m_anOldEdge[1] == nV1 && pMapping[i].m_anOldEdge[0] == nV2))
		{
			return i;
		}
	}

	return -1;  // We should never reach this line
}


// Direct3D9 Shadow Volume Sample
bool cViewer::GenerateShadowMesh()
{
	RETV(!m_srcMesh.m_mesh, false);

	HRESULT hr = S_OK;
	ID3DXMesh *pInputMesh = NULL;

	// Convert the input mesh to a format same as the output mesh using 32-bit index.
	hr = m_srcMesh.m_mesh->CloneMesh(D3DXMESH_32BIT, sShadowVertex::Decl, m_renderer.GetDevice(), &pInputMesh);
	if (FAILED(hr))
		return false;

	//--------------------------------------------------------------------------------------------//
	//https://www.gamedev.net/topic/146017-convertadjacencytopointreps/
	//-- - CD-- -
	//-- / || \--
	//- A - || -F -
	//--\ || / --
	//-- - BE-- -
	//Vertices: ABCDEF
	//Adjacency : -1-0--
	//Point - reps : 012215
	DWORD* pdwAdj = new DWORD[3 * pInputMesh->GetNumFaces()];
	DWORD* pdwPtRep = new DWORD[pInputMesh->GetNumVertices()];

	hr = pInputMesh->GenerateAdjacency(ADJACENCY_EPSILON, pdwAdj);
	if (FAILED(hr))
		return false;

	pInputMesh->ConvertAdjacencyToPointReps(pdwAdj, pdwPtRep);
	delete[] pdwAdj;

	sShadowVertex *pvtx = NULL;
	DWORD *pidx = NULL;

	pInputMesh->LockVertexBuffer(0, (LPVOID*)&pvtx);
	pInputMesh->LockIndexBuffer(0, (LPVOID*)&pidx);
	if (!pvtx || !pidx)
		return false;

	sShadowVertex* pNewVBData = NULL;
	DWORD* pdwNewIBData = NULL;
	ID3DXMesh* pNewMesh = NULL;

	// Maximum number of unique edges = Number of faces * 3
	const DWORD dwNumEdges = pInputMesh->GetNumFaces() * 3;
	CEdgeMapping* pMapping = new CEdgeMapping[dwNumEdges];
	if (!pMapping)
		goto cleanup;

	int nNumMaps = 0;  // Number of entries that exist in pMapping

					   // Create a new mesh
	hr = D3DXCreateMesh(pInputMesh->GetNumFaces() + dwNumEdges * 2,
		pInputMesh->GetNumFaces() * 3,
		D3DXMESH_32BIT,
		sShadowVertex::Decl,
		m_renderer.GetDevice(),
		&pNewMesh);

	if (FAILED(hr))
		goto cleanup;

	pNewMesh->LockVertexBuffer(0, (LPVOID*)&pNewVBData);
	pNewMesh->LockIndexBuffer(0, (LPVOID*)&pdwNewIBData);

	// nNextIndex is the array index in IB that the next vertex index value
	// will be store at.
	int nNextIndex = 0;

	if (!pNewVBData || !pdwNewIBData)
		goto cleanup;

	ZeroMemory(pNewVBData, pNewMesh->GetNumVertices() * pNewMesh->GetNumBytesPerVertex());
	ZeroMemory(pdwNewIBData, sizeof(DWORD) * pNewMesh->GetNumFaces() * 3);

	// pNextOutVertex is the location to write the next
	// vertex to.
	sShadowVertex* pNextOutVertex = pNewVBData;

	// Iterate through the faces.  For each face, output new
	// vertices and face in the new mesh, and write its edges
	// to the mapping table.

	for (UINT f = 0; f < pInputMesh->GetNumFaces(); ++f)
	{
		// Copy the vertex data for all 3 vertices
		CopyMemory(pNextOutVertex, pvtx + pidx[f * 3], sizeof(sShadowVertex));
		CopyMemory(pNextOutVertex + 1, pvtx + pidx[f * 3 + 1], sizeof(sShadowVertex));
		CopyMemory(pNextOutVertex + 2, pvtx + pidx[f * 3 + 2], sizeof(sShadowVertex));

		// Write out the face
		pdwNewIBData[nNextIndex++] = f * 3;
		pdwNewIBData[nNextIndex++] = f * 3 + 1;
		pdwNewIBData[nNextIndex++] = f * 3 + 2;

		// Compute the face normal and assign it to
		// the normals of the vertices.
		D3DXVECTOR3 v1, v2;  // v1 and v2 are the edge vectors of the face
		D3DXVECTOR3 vNormal;
		v1 = *(D3DXVECTOR3*)(pNextOutVertex + 1) - *(D3DXVECTOR3*)pNextOutVertex;
		v2 = *(D3DXVECTOR3*)(pNextOutVertex + 2) - *(D3DXVECTOR3*)(pNextOutVertex + 1);
		D3DXVec3Cross(&vNormal, &v1, &v2);
		D3DXVec3Normalize(&vNormal, &vNormal);

		pNextOutVertex->Normal = *(Vector3*)&vNormal;
		(pNextOutVertex + 1)->Normal = *(Vector3*)&vNormal;
		(pNextOutVertex + 2)->Normal = *(Vector3*)&vNormal;

		pNextOutVertex += 3;

		// Add the face's edges to the edge mapping table

		// Edge 1
		int nIndex;
		DWORD nVertIndex[3] =
		{
			pdwPtRep[pidx[f * 3]],
			pdwPtRep[pidx[f * 3 + 1]],
			pdwPtRep[pidx[f * 3 + 2]]
		};
		nIndex = FindEdgeInMappingTable(nVertIndex[0], nVertIndex[1], pMapping, dwNumEdges);

		// If error, we are not able to proceed, so abort.
		if (-1 == nIndex)
		{
			hr = E_INVALIDARG;
			goto cleanup;
		}

		if (pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1)
		{
			// No entry for this edge yet.  Initialize one.
			pMapping[nIndex].m_anOldEdge[0] = nVertIndex[0];
			pMapping[nIndex].m_anOldEdge[1] = nVertIndex[1];
			pMapping[nIndex].m_aanNewEdge[0][0] = f * 3;
			pMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 1;

			++nNumMaps;
		}
		else
		{
			// An entry is found for this edge.  Create
			// a quad and output it.
			assert(nNumMaps > 0);

			pMapping[nIndex].m_aanNewEdge[1][0] = f * 3;      // For clarity
			pMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 1;

			// First triangle
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

			// Second triangle
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

			// pMapping[nIndex] is no longer needed. Copy the last map entry
			// over and decrement the map count.

			pMapping[nIndex] = pMapping[nNumMaps - 1];
			FillMemory(&pMapping[nNumMaps - 1], sizeof(pMapping[nNumMaps - 1]), 0xFF);
			--nNumMaps;
		}

		// Edge 2
		nIndex = FindEdgeInMappingTable(nVertIndex[1], nVertIndex[2], pMapping, dwNumEdges);

		// If error, we are not able to proceed, so abort.
		if (-1 == nIndex)
		{
			hr = E_INVALIDARG;
			goto cleanup;
		}

		if (pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1)
		{
			pMapping[nIndex].m_anOldEdge[0] = nVertIndex[1];
			pMapping[nIndex].m_anOldEdge[1] = nVertIndex[2];
			pMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 1;
			pMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 2;

			++nNumMaps;
		}
		else
		{
			// An entry is found for this edge.  Create
			// a quad and output it.
			assert(nNumMaps > 0);

			pMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 1;
			pMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 2;

			// First triangle
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

			// Second triangle
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

			// pMapping[nIndex] is no longer needed. Copy the last map entry
			// over and decrement the map count.

			pMapping[nIndex] = pMapping[nNumMaps - 1];
			FillMemory(&pMapping[nNumMaps - 1], sizeof(pMapping[nNumMaps - 1]), 0xFF);
			--nNumMaps;
		}

		// Edge 3
		nIndex = FindEdgeInMappingTable(nVertIndex[2], nVertIndex[0], pMapping, dwNumEdges);

		// If error, we are not able to proceed, so abort.
		if (-1 == nIndex)
		{
			hr = E_INVALIDARG;
			goto cleanup;
		}

		if (pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1)
		{
			pMapping[nIndex].m_anOldEdge[0] = nVertIndex[2];
			pMapping[nIndex].m_anOldEdge[1] = nVertIndex[0];
			pMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 2;
			pMapping[nIndex].m_aanNewEdge[0][1] = f * 3;

			++nNumMaps;
		}
		else
		{
			// An entry is found for this edge.  Create
			// a quad and output it.
			assert(nNumMaps > 0);

			pMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 2;
			pMapping[nIndex].m_aanNewEdge[1][1] = f * 3;

			// First triangle
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

			// Second triangle
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

			// pMapping[nIndex] is no longer needed. Copy the last map entry
			// over and decrement the map count.

			pMapping[nIndex] = pMapping[nNumMaps - 1];
			FillMemory(&pMapping[nNumMaps - 1], sizeof(pMapping[nNumMaps - 1]), 0xFF);
			--nNumMaps;
		}
	}


	// Now the entries in the edge mapping table represent
	// non-shared edges.  What they mean is that the original
	// mesh has openings (holes), so we attempt to patch them.
	// First we need to recreate our mesh with a larger vertex
	// and index buffers so the patching geometry could fit.
	//DXUTTRACE(L"Faces to patch: %d\n", nNumMaps);

	// Create a mesh with large enough vertex and
	// index buffers.

	sShadowVertex* pPatchVBData = NULL;
	DWORD* pdwPatchIBData = NULL;

	ID3DXMesh* pPatchMesh = NULL;
	// Make enough room in IB for the face and up to 3 quads for each patching face
	hr = D3DXCreateMesh(nNextIndex / 3 + nNumMaps * 7,
		(pInputMesh->GetNumFaces() + nNumMaps) * 3,
		D3DXMESH_32BIT,
		sShadowVertex::Decl,
		m_renderer.GetDevice(),
		&pPatchMesh);

	if (FAILED(hr))
		goto cleanup;

	hr = pPatchMesh->LockVertexBuffer(0, (LPVOID*)&pPatchVBData);
	if (SUCCEEDED(hr))
		hr = pPatchMesh->LockIndexBuffer(0, (LPVOID*)&pdwPatchIBData);

	if (pPatchVBData && pdwPatchIBData)
	{
		ZeroMemory(pPatchVBData, sizeof(sShadowVertex) * (pInputMesh->GetNumFaces() + nNumMaps) * 3);
		ZeroMemory(pdwPatchIBData, sizeof(DWORD) * (nNextIndex + 3 * nNumMaps * 7));

		// Copy the data from one mesh to the other
		CopyMemory(pPatchVBData, pNewVBData, sizeof(sShadowVertex) * pInputMesh->GetNumFaces() * 3);
		CopyMemory(pdwPatchIBData, pdwNewIBData, sizeof(DWORD) * nNextIndex);
	}
	else
	{
		// Some serious error is preventing us from locking.
		// Abort and return error.

		pPatchMesh->Release();
		goto cleanup;
	}

	// Replace pNewMesh with the updated one.  Then the code
	// can continue working with the pNewMesh pointer.

	pNewMesh->UnlockVertexBuffer();
	pNewMesh->UnlockIndexBuffer();
	pNewVBData = pPatchVBData;
	pdwNewIBData = pdwPatchIBData;
	pNewMesh->Release();
	pNewMesh = pPatchMesh;


	// Now, we iterate through the edge mapping table and
	// for each shared edge, we generate a quad.
	// For each non-shared edge, we patch the opening
	// with new faces.

	// nNextVertex is the index of the next vertex.
	int nNextVertex = pInputMesh->GetNumFaces() * 3;

	for (int i = 0; i < nNumMaps; ++i)
	{
		if (pMapping[i].m_anOldEdge[0] != -1 &&
			pMapping[i].m_anOldEdge[1] != -1)
		{
			// If the 2nd new edge indexes is -1,
			// this edge is a non-shared one.
			// We patch the opening by creating new
			// faces.
			if (pMapping[i].m_aanNewEdge[1][0] == -1 ||  // must have only one new edge
				pMapping[i].m_aanNewEdge[1][1] == -1)
			{
				// Find another non-shared edge that
				// shares a vertex with the current edge.
				for (int i2 = i + 1; i2 < nNumMaps; ++i2)
				{
					if (pMapping[i2].m_anOldEdge[0] != -1 &&       // must have a valid old edge
						pMapping[i2].m_anOldEdge[1] != -1 &&
						(pMapping[i2].m_aanNewEdge[1][0] == -1 || // must have only one new edge
							pMapping[i2].m_aanNewEdge[1][1] == -1))
					{
						int nVertShared = 0;
						if (pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1])
							++nVertShared;
						if (pMapping[i2].m_anOldEdge[1] == pMapping[i].m_anOldEdge[0])
							++nVertShared;

						if (2 == nVertShared)
						{
							// These are the last two edges of this particular
							// opening. Mark this edge as shared so that a degenerate
							// quad can be created for it.

							pMapping[i2].m_aanNewEdge[1][0] = pMapping[i].m_aanNewEdge[0][0];
							pMapping[i2].m_aanNewEdge[1][1] = pMapping[i].m_aanNewEdge[0][1];
							break;
						}
						else if (1 == nVertShared)
						{
							// nBefore and nAfter tell us which edge comes before the other.
							int nBefore, nAfter;
							if (pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1])
							{
								nBefore = i;
								nAfter = i2;
							}
							else
							{
								nBefore = i2;
								nAfter = i;
							}

							// Found such an edge. Now create a face along with two
							// degenerate quads from these two edges.

							pNewVBData[nNextVertex] = pNewVBData[pMapping[nAfter].m_aanNewEdge[0][1]];
							pNewVBData[nNextVertex + 1] = pNewVBData[pMapping[nBefore].m_aanNewEdge[0][1]];
							pNewVBData[nNextVertex + 2] = pNewVBData[pMapping[nBefore].m_aanNewEdge[0][0]];
							// Recompute the normal
							Vector3 v1 = pNewVBData[nNextVertex + 1].Position - pNewVBData[nNextVertex].Position;
							Vector3 v2 = pNewVBData[nNextVertex + 2].Position - pNewVBData[nNextVertex + 1].Position;
							v1.Normalize();
							v2.Normalize();
							pNewVBData[nNextVertex].Normal = v1.CrossProduct(v2).Normal();
							pNewVBData[nNextVertex + 1].Normal = pNewVBData[nNextVertex +
								2].Normal = pNewVBData[nNextVertex].Normal;

							pdwNewIBData[nNextIndex] = nNextVertex;
							pdwNewIBData[nNextIndex + 1] = nNextVertex + 1;
							pdwNewIBData[nNextIndex + 2] = nNextVertex + 2;

							// 1st quad

							pdwNewIBData[nNextIndex + 3] = pMapping[nBefore].m_aanNewEdge[0][1];
							pdwNewIBData[nNextIndex + 4] = pMapping[nBefore].m_aanNewEdge[0][0];
							pdwNewIBData[nNextIndex + 5] = nNextVertex + 1;

							pdwNewIBData[nNextIndex + 6] = nNextVertex + 2;
							pdwNewIBData[nNextIndex + 7] = nNextVertex + 1;
							pdwNewIBData[nNextIndex + 8] = pMapping[nBefore].m_aanNewEdge[0][0];

							// 2nd quad

							pdwNewIBData[nNextIndex + 9] = pMapping[nAfter].m_aanNewEdge[0][1];
							pdwNewIBData[nNextIndex + 10] = pMapping[nAfter].m_aanNewEdge[0][0];
							pdwNewIBData[nNextIndex + 11] = nNextVertex;

							pdwNewIBData[nNextIndex + 12] = nNextVertex + 1;
							pdwNewIBData[nNextIndex + 13] = nNextVertex;
							pdwNewIBData[nNextIndex + 14] = pMapping[nAfter].m_aanNewEdge[0][0];

							// Modify mapping entry i2 to reflect the third edge
							// of the newly added face.

							if (pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1])
							{
								pMapping[i2].m_anOldEdge[0] = pMapping[i].m_anOldEdge[0];
							}
							else
							{
								pMapping[i2].m_anOldEdge[1] = pMapping[i].m_anOldEdge[1];
							}
							pMapping[i2].m_aanNewEdge[0][0] = nNextVertex + 2;
							pMapping[i2].m_aanNewEdge[0][1] = nNextVertex;

							// Update next vertex/index positions

							nNextVertex += 3;
							nNextIndex += 15;

							break;
						}
					}
				}
			}
			else
			{
				// This is a shared edge.  Create the degenerate quad.

				// First triangle
				pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][1];
				pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][0];
				pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][0];

				// Second triangle
				pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][1];
				pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][0];
				pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][0];
			}
		}
	}


cleanup:
	if (pNewVBData)
	{
		pNewMesh->UnlockVertexBuffer();
		pNewVBData = NULL;
	}
	if (pdwNewIBData)
	{
		pNewMesh->UnlockIndexBuffer();
		pdwNewIBData = NULL;
	}

	if (SUCCEEDED(hr))
	{
		// At this time, the output mesh may have an index buffer
		// bigger than what is actually needed, so we create yet
		// another mesh with the exact IB size that we need and
		// output it.  This mesh also uses 16-bit index if
		// 32-bit is not necessary.
		//DXUTTRACE(L"Shadow volume has %u vertices, %u faces.\n",
		//	(pInputMesh->GetNumFaces() + nNumMaps) * 3, nNextIndex / 3);

		bool bNeed32Bit = (pInputMesh->GetNumFaces() + nNumMaps) * 3 > 65535;
		ID3DXMesh* pFinalMesh;
		hr = D3DXCreateMesh(nNextIndex / 3,  // Exact number of faces
			(pInputMesh->GetNumFaces() + nNumMaps) * 3,
			D3DXMESH_WRITEONLY | (bNeed32Bit ? D3DXMESH_32BIT : 0),
			sShadowVertex::Decl,
			m_renderer.GetDevice(),
			&pFinalMesh);
		if (SUCCEEDED(hr))
		{
			pNewMesh->LockVertexBuffer(0, (LPVOID*)&pNewVBData);
			pNewMesh->LockIndexBuffer(0, (LPVOID*)&pdwNewIBData);

			sShadowVertex* pFinalVBData = NULL;
			WORD* pwFinalIBData = NULL;

			pFinalMesh->LockVertexBuffer(0, (LPVOID*)&pFinalVBData);
			pFinalMesh->LockIndexBuffer(0, (LPVOID*)&pwFinalIBData);

			if (pNewVBData && pdwNewIBData && pFinalVBData && pwFinalIBData)
			{
				CopyMemory(pFinalVBData, pNewVBData, sizeof(sShadowVertex) *
					(pInputMesh->GetNumFaces() + nNumMaps) * 3);

				if (bNeed32Bit)
					CopyMemory(pwFinalIBData, pdwNewIBData, sizeof(DWORD) * nNextIndex);
				else
				{
					for (int i = 0; i < nNextIndex; ++i)
						pwFinalIBData[i] = (WORD)pdwNewIBData[i];
				}
			}

			if (pNewVBData)
				pNewMesh->UnlockVertexBuffer();
			if (pdwNewIBData)
				pNewMesh->UnlockIndexBuffer();
			if (pFinalVBData)
				pFinalMesh->UnlockVertexBuffer();
			if (pwFinalIBData)
				pFinalMesh->UnlockIndexBuffer();

			// Release the old
			pNewMesh->Release();
			pNewMesh = pFinalMesh;
		}

		m_shadowMesh = pNewMesh;
	}
	else
		pNewMesh->Release();

	pInputMesh->UnlockIndexBuffer();
	pInputMesh->UnlockVertexBuffer();

	delete[] pdwPtRep;
	delete[] pMapping;
	pInputMesh->Release();

	return true;
}


void cViewer::OnUpdate(const float elapseT)
{
	// keyboard
	const float vel = 10 * elapseT;
	if (GetAsyncKeyState('W'))
		GetMainCamera()->MoveFront(vel);
	else if (GetAsyncKeyState('A'))
		GetMainCamera()->MoveRight(-vel);
	else if (GetAsyncKeyState('D'))
		GetMainCamera()->MoveRight(vel);
	else if (GetAsyncKeyState('S'))
		GetMainCamera()->MoveFront(-vel);
	else if (GetAsyncKeyState('E'))
		GetMainCamera()->MoveUp(vel);
	else if (GetAsyncKeyState('C'))
		GetMainCamera()->MoveUp(-vel);

	GetMainCamera()->Update(elapseT);
}


void cViewer::RenderShadow()
{
	RET(!m_shadowMesh);

	GetMainCamera()->Bind(m_shader);
	GetMainLight().Bind(m_shader);

	// Ambient
	if (1)
	{
		m_shader.SetTechnique("Ambient");

		//m_srcMesh.m_tm = m_rotateTm;
		//m_srcMesh.SetShader(&m_shader);
		m_srcMesh.RenderShader(m_renderer, m_shader, m_rotateTm);

		m_cube3.RenderShader(m_renderer, m_shader);
	}


	m_renderer.GetDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 170, 170, 170), 1.0f, 0);

	// Shadow
	if (1)
	{
		if (m_isShowShadow)
			m_shader.SetTechnique("ShowShadow");
		else
			m_shader.SetTechnique("Shadow");

		m_shader.SetMatrix("g_mWorld", m_rotateTm);
		m_shader.SetVector("g_vLightView", GetMainLight().GetPosition() * GetMainCamera()->GetViewMatrix());
		m_shader.SetVector("g_vShadowColor", Vector4(0, 1, 0, 0.2f));
		m_shader.SetFloat("g_fFarClip", 10000.0f);

		const int passCount = m_shader.Begin();
		for (int i = 0; i < passCount; ++i)
		{
			m_shader.BeginPass(i);
			m_shader.CommitChanges();
			m_shadowMesh->DrawSubset(0);
			m_shader.EndPass();
		}
		m_shader.End();
	}


	// Scene
	if (1)
	{
		m_shader.SetTechnique("Scene");

		m_cube3.RenderShader(m_renderer, m_shader);
		//m_srcMesh.m_tm = m_rotateTm;
		//m_srcMesh.SetShader(&m_shader);
		m_srcMesh.RenderShader(m_renderer, m_shader, m_rotateTm);
	}
}


void cViewer::OnRender(const float elapseT)
{
	GetMainLight().Bind(m_renderer, 0);

	if (m_renderer.ClearScene())
	//m_renderer.GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 66, 75, 121), 1.0f, 0);
	{
		m_renderer.BeginScene();

		m_renderer.RenderGrid();
		m_renderer.RenderAxis();
		m_renderer.RenderFPS();

		RenderShadow();

		m_renderer.EndScene();
		m_renderer.Present();
	}
}


void cViewer::OnShutdown()
{
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DROPFILES:
	{
		HDROP hdrop = (HDROP)wParam;
		char filePath[MAX_PATH];
		const UINT size = DragQueryFileA(hdrop, 0, filePath, MAX_PATH);
		if (size == 0)
			return;// handle error...

		m_srcMesh.Create(m_renderer, filePath);
		GenerateShadowMesh();
	}
	break;

	case WM_MOUSEWHEEL:
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera()->GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera()->Zoom((zDelta<0) ? -zoomLen : zoomLen);
	}
	break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
		{
			static bool flag = false;
			m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;
		case VK_SPACE:
			m_isShowShadow = !m_isShowShadow;
			break;
		case 'P':
			m_isPause = !m_isPause;
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		SetCapture(m_hWnd);
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
	}
	break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		m_LButtonDown = false;
		break;

	case WM_RBUTTONDOWN:
	{
		SetCapture(m_hWnd);
		m_RButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);

		// unit 이동.
		Ray ray(m_curPos.x, m_curPos.y, 1024, 768,
			GetMainCamera()->GetProjectionMatrix(),
			GetMainCamera()->GetViewMatrix());
	}
	break;

	case WM_RBUTTONUP:
		m_RButtonDown = false;
		ReleaseCapture();
		break;

	case WM_MBUTTONDOWN:
		m_MButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
		break;

	case WM_MBUTTONUP:
		m_MButtonDown = false;
		break;

	case WM_MOUSEMOVE:
	{
		if (wParam & 0x10) // middle button down
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
		}

		if (m_LButtonDown)
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			Quaternion q1(graphic::GetMainCamera()->GetRight(), -y * 0.01f);
			Quaternion q2(graphic::GetMainCamera()->GetUpVector(), -x * 0.01f);

			if (GetAsyncKeyState('F'))
			{
				//m_cube2.m_tm *= (q2.GetMatrix() * q1.GetMatrix());
			}
			else
			{
				m_rotateTm *= (q2.GetMatrix() * q1.GetMatrix());
			}

		}
		else if (m_RButtonDown)
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			//if (GetAsyncKeyState('C'))
			{
				graphic::GetMainCamera()->Yaw2(x * 0.005f);
				graphic::GetMainCamera()->Pitch2(y * 0.005f);
			}
		}
		else if (m_MButtonDown)
		{
			const POINT point = { LOWORD(lParam), HIWORD(lParam) };
			const POINT pos = { point.x - m_curPos.x, point.y - m_curPos.y };
			m_curPos = point;

			const float len = graphic::GetMainCamera()->GetDistance();
			graphic::GetMainCamera()->MoveRight(-pos.x * len * 0.001f);
			graphic::GetMainCamera()->MoveUp(pos.y * len * 0.001f);
		}
		else
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
		}

	}
	break;
	}
}

