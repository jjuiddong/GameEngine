//
// 2017-03-25, jjuiddong
//
#pragma once


class cModelControl
{
public:
	cModelControl();
	virtual ~cModelControl();

	void Update(const float deltaSeconds);
	void Render(graphic::cRenderer &renderer, const float deltaSeconds);
	void Clear();


public:
};
