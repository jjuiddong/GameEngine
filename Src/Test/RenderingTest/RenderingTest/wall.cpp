
#include "stdafx.h"
#include "wall.h"

using namespace graphic;
using namespace warehouse;


cWall::cWall()
	: m_isPillar(true)
	, m_shadowEpsilon(0.01f)
{
}

cWall::~cWall()
{
	Clear();
}


bool cWall::Create(cRenderer &renderer, const Vector3 &pos1, const Vector3 &pos2, const float height, 
	const float thickness, const bool isPillar)
	// isPillar = true
{
	Clear();

	const Vector3 v = (pos2 - pos1).Normal();
	const Vector3 back = Vector3(0,1,0).CrossProduct(v).Normal();
	m_wall.SetCube(renderer, pos1, pos2 + back*thickness + Vector3(0,height,0));
	SetShader(cResourceManager::Get()->LoadShader(renderer, "wall.fx"));
	m_boundingBox.SetBoundingBox(pos1, pos2 + back*thickness + Vector3(0, height, 0));
	CalcBoundingSphere();

	// Create Pillar
	if (isPillar)
	{
		const float pillarSize = thickness + (thickness * 0.2f);
		const Vector3 start = pos1 - (back * thickness * 0.1f);
		const float unit = 5.f;
		const int pillarCnt = (int) ( sqrtf(pos1.LengthRoughly(pos2)) / unit);

		for (int i = 0; i < pillarCnt; ++i)
		{
			Vector3 p1 = start + v*(i*unit);
			Vector3 p2 = start + v*(i*unit) + v*pillarSize + back*pillarSize;
			p2.y += height + 0.1f;

			cCube3 *pillar = new cCube3();
			pillar->SetCube(renderer, p1, p2);
			pillar->m_mtrl.InitGray();
			m_pillars.push_back(pillar);
		}
	}

	return true;
}


bool cWall::Update(graphic::cRenderer &renderer, const float deltaSeconds)
{
	// nothing~
	return true;
}


void cWall::RenderShader(graphic::cRenderer &renderer, const Matrix44 &tm)
// tm = Matrix44::Identity
{
	RET(!m_shader);

	m_shader->SetFloat("g_shadowEpsilon", m_shadowEpsilon);
	m_wall.RenderShader(renderer, *m_shader, tm);

	for (auto &p : m_pillars)
		p->RenderShader(renderer, *m_shader, tm);	
}


//void cWall::Render(cRenderer &renderer, const Matrix44 &tm)
//// tm = Matrix44::Identity
//{
//	m_wall.Render(renderer, tm);
//	for (auto &p : m_pillars)
//		p->Render(renderer, tm);;
//}

//
//void cWall::RenderShadow(graphic::cRenderer &renderer, cShader &shader, const Matrix44 &tm)
//// tm = Matrix44::Identity
//{
//	shader.SetMatrix("g_mWorld", tm);
//
//	int passCount = shader.Begin();
//	for (int i = 0; i < passCount; ++i)
//	{
//		shader.BeginPass(i);
//		shader.CommitChanges();
//		m_shadow.Render(renderer);
//		shader.EndPass();
//	}
//	shader.End();
//}


void cWall::Clear()
{
	for (auto &p : m_pillars)
		delete p;
	m_pillars.clear();
}
