//
// 2017-03-25, jjuiddong
//
#pragma once

class cSkeletonControl
{
public:
	cSkeletonControl();
	virtual ~cSkeletonControl();

	void Update(const float deltaSeconds);
	void Render(graphic::cRenderer &renderer, const float deltaSeconds);
	void Clear();
};
