
#include "stdafx.h"
#include "GameApp.h"
#include <fstream>

INIT_FRAMEWORK(cGameApp);


// ���ؽ� ����ü
struct Vertex
{
	Vertex() {}
	Vertex(float x0, float y0, float z0) : p(Vector3(x0, y0, z0)), n(Vector3(0,0,0)) {}
	Vector3 p;
	Vector3 n;
	static const DWORD FVF;
};
//���ؽ� ����ü ����.
const DWORD Vertex::FVF  = D3DFVF_XYZ | D3DFVF_NORMAL;


cGameApp::cGameApp()
{
	m_windowName = L"GameApp";
	const RECT r = {0, 0, 800, 600};
	m_windowRect = r;
}

cGameApp::~cGameApp()
{
}


bool cGameApp::OnInit()
{
	graphic::cResourceManager::Get()->SetMediaDirectory("../media/");
	ReadModelFile("../media/vase.dat", m_vtxBuff, m_VtxSize, m_idxBuff, m_FaceSize);

	m_mtrl.InitRed();

	Vector4 color(1,1,1,1);
	m_light.Init( graphic::cLight::LIGHT_DIRECTIONAL, 
		color * 0.4f,
		color,
		color * 0.6f,
		Vector3(1,0,0));

	Matrix44 V;
	Vector3 dir = Vector3(0,0,0)-Vector3(0,0,-5);
	dir.Normalize();
	V.SetView(Vector3(0,0,-500), dir, Vector3(0,1,0));
	m_renderer.GetDevice()->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&V);

	const int WINSIZE_X = 1024;		//�ʱ� ������ ���� ũ��
	const int WINSIZE_Y = 768;	//�ʱ� ������ ���� ũ��

	Matrix44 proj;
	proj.SetProjection(D3DX_PI * 0.5f, (float)WINSIZE_X / (float) WINSIZE_Y, 1.f, 1000.0f) ;
	m_renderer.GetDevice()->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&proj) ;

	m_light.Bind(m_renderer, 0);

	m_renderer.GetDevice()->LightEnable (
		0, // Ȱ��ȭ/ ��Ȱ��ȭ �Ϸ��� ���� ����Ʈ ���� ���
		true); // true = Ȱ��ȭ �� false = ��Ȱ��ȭ

	return true;
}


void cGameApp::OnUpdate(const float elapseT)
{

}


void cGameApp::OnRender(const float elapseT)
{
	if (SUCCEEDED(m_renderer.GetDevice()->Clear( 
		0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_XRGB(255, 255, 255),
		1.0f, 0)))
	{
		m_renderer.GetDevice()->BeginScene();

		static float y = 0;
		y += elapseT;
		// ������ 2*PI �� �̸��� 0���� �ʱ�ȭ�Ѵ�.
		if (y >= 6.28f)
			y = 0;

		Matrix44 rx, ry, r;
		rx.SetRotationX(MATH_PI/4.f); 	// x������ 45�� ȸ����Ų��.
		ry.SetRotationY(y); // y������ ȸ��
		r = rx*ry;
		m_renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&r);

		m_mtrl.Bind(m_renderer);
		m_idxBuff.Bind(m_renderer);
		m_vtxBuff.Bind(m_renderer);
		m_renderer.GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_VtxSize, 0, m_FaceSize);

		m_renderer.GetDevice()->EndScene();
		m_renderer.GetDevice()->Present( NULL, NULL, NULL, NULL );
	}
}


void cGameApp::OnShutdown()
{

}


void cGameApp::OnMessageProc( UINT message, WPARAM wParam, LPARAM lParam)
{

}


bool cGameApp::ReadModelFile( const string &fileName, graphic::cVertexBuffer &vtxBuff, int &vtxSize,  
	graphic::cIndexBuffer &idxBuff, int &faceSize )
{
	using namespace std;
	ifstream fin(fileName.c_str());
	if (!fin.is_open())
		return false;

	string vtx, vtx_eq;
	int numVertices;
	fin >> vtx >> vtx_eq >> numVertices;

	if (numVertices <= 0)
		return  false;

	vtxSize = numVertices;

	// ���ؽ� ���� ����.
	if (!vtxBuff.Create(m_renderer, vtxSize, sizeof(Vertex), Vertex::FVF))
		return false;

	// ���ؽ� ���� �ʱ�ȭ.
	Vertex* vertices = (Vertex*)vtxBuff.Lock();
	float num1, num2, num3;
	for (int i = 0; i < numVertices; i++)
	{
		fin >> num1 >> num2 >> num3;
		vertices[i] = Vertex(num1, num2, num3);
	}
	vtxBuff.Unlock();


	string idx, idx_eq;
	int numIndices;
	fin >> idx >> idx_eq >> numIndices;

	if (numIndices <= 0)
		return false;

	faceSize = numIndices;

	idxBuff.Create(m_renderer, numIndices);

	WORD *indices = (WORD*)idxBuff.Lock();
	int num4, num5, num6;
	for (int i = 0; i < numIndices*3; i+=3)
	{
		fin >> num4 >> num5 >> num6;
		indices[ i] = num4;
		indices[ i+1] = num5;
		indices[ i+2] = num6;	
	}
	idxBuff.Unlock();

	ComputeNormals(vtxBuff, vtxSize, idxBuff, faceSize);
	return true;
}


void cGameApp::ComputeNormals(graphic::cVertexBuffer &vtxBuff, int vtxSize,  
	graphic::cIndexBuffer &idxBuff, int faceSize)
{
	Vertex* vertices = (Vertex*)vtxBuff.Lock();
	WORD *indices = (WORD*)idxBuff.Lock();

	for (int i=0; i < faceSize*3; i+=3)
	{
		Vector3 p1 = vertices[ indices[ i]].p;
		Vector3 p2 = vertices[ indices[ i+1]].p;
		Vector3 p3 = vertices[ indices[ i+2]].p;

		Vector3 v1 = p2 - p1;
		Vector3 v2 = p3 - p1;
		v1.Normalize();
		v2.Normalize();
		Vector3 n = v1.CrossProduct(v2);
		n.Normalize();

		if (vertices[ indices[ i]].n.IsEmpty())
		{
			vertices[ indices[ i]].n = n;
		}
		else
		{
			vertices[ indices[ i]].n += n;
			vertices[ indices[ i]].n /= 2.f;
		}

		if (vertices[ indices[ i+1]].n.IsEmpty())
		{
			vertices[ indices[ i+1]].n = n;
		}
		else
		{
			vertices[ indices[ i+1]].n += n;
			vertices[ indices[ i+1]].n /= 2.f;
		}

		if (vertices[ indices[ i+2]].n.IsEmpty())
		{
			vertices[ indices[ i+2]].n = n;
		}
		else
		{
			vertices[ indices[ i+2]].n += n;
			vertices[ indices[ i+2]].n /= 2.f;
		}
	}

	vtxBuff.Unlock();
	idxBuff.Unlock();
}
