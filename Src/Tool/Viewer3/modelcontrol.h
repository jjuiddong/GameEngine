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


protected:
	void RenderCollada(graphic::cRenderer &renderer, graphic::cColladaModel *model, const float deltaSeconds);
	void RenderXFile(graphic::cRenderer &renderer, graphic::cXFileMesh *model,	const float deltaSeconds);
};
