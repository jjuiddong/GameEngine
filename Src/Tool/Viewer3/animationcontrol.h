//
// 2017-03-25, jjuiddong
//
#pragma once


class cAnimationControl
{
public:
	cAnimationControl();
	virtual ~cAnimationControl();

	void Update(const float deltaSeconds);
	void Render(graphic::cRenderer &renderer, const float deltaSeconds);
	void Clear();


public:
};
