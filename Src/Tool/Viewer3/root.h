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

	void Init();
	void Update(const float deltaSeconds);
	void Render(graphic::cRenderer &renderer, const float deltaSeconds);
	void Clear();
	

public:
	graphic::cModel2 m_model;

	cDisplayControl m_dispControl;
	cModelControl m_modelControl;
	cSkeletonControl m_skeletonControl;
	cAnimationControl m_aniControl;
};

extern cRoot g_root;
