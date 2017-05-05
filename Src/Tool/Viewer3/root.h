#pragma once

#include "displaycontrol.h"
#include "modelcontrol.h"
#include "skeletoncontrol.h"
#include "animationcontrol.h"


class cRoot
{
public:
	cRoot();
	virtual ~cRoot();

	void Init(graphic::cRenderer &renderer);
	void Update(graphic::cRenderer &renderer, const float deltaSeconds);
	void Render(graphic::cRenderer &renderer, const float deltaSeconds);
	void PreRender(graphic::cRenderer &renderer, const float deltaSeconds);
	void LostDevice();
	void ResetDevice(graphic::cRenderer &renderer);
	void UpdateShader();
	void Clear();
	

protected:
	void RenderGeometry(graphic::cRenderer &renderer, const float deltaSeconds
		, const Matrix44 &tm=Matrix44::Identity
		, const bool isShadow=true);


public:
	graphic::cModel2 m_model;
	graphic::cCube3 m_ground;
	graphic::cWater m_water;
	graphic::cSkyBox2 m_skybox;

	cDisplayControl m_dispControl;
	cModelControl m_modelControl;
	cSkeletonControl m_skeletonControl;
	cAnimationControl m_aniControl;
};

extern cRoot g_root;
