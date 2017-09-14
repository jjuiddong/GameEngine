#pragma once


class cViewer : public framework::cGameMain2
{
public:
	cViewer();
	virtual ~cViewer();
	virtual bool OnInit() override;
	virtual void OnUpdate(const float deltaSeconds) override;
	virtual void OnRender(const float deltaSeconds) override;
	virtual void OnEventProc(const sf::Event &evt) override;


public:
	graphic::cDbgArrow m_dbgArrow;
	graphic::cDbgAxis m_axis;
	graphic::cMaterial m_mtrl;
	graphic::cGrid m_ground;
	graphic::cModel2 m_model;
	graphic::cModel2 m_model2;
};


extern graphic::cRenderTarget g_shadowMap;
extern graphic::cCamera g_lightCamera;
extern graphic::cTexture g_unBindTex;
extern cViewer *g_viewer;
