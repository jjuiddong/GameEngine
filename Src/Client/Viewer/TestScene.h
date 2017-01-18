#pragma once


class cTestScene : public framework::cWindow
{
public:
	cTestScene(LPD3DXSPRITE sprite);
	virtual ~cTestScene();

	void Button1Click(framework::cEvent &event);
};
