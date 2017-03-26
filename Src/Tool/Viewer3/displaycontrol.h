//
// 2017-03-25, jjuiddong
//
#pragma once


class cDisplayControl
{
public:
	cDisplayControl();
	virtual ~cDisplayControl();

	void Update(const float deltaSeconds);
	void Render(graphic::cRenderer &renderer, const float deltaSeconds);
	void Clear();


public:
	bool m_wireFrame = false;
	bool m_backfaceCulling = true;
};
