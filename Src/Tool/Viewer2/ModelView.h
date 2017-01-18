#pragma once


// CModelView ∫‰¿‘¥œ¥Ÿ.
class CModelView : public CView
							, public common::iObserver2
{
public:
	CModelView();
	virtual ~CModelView();


public:
	void Init();
	void Update(const float elapseT);
	void ShowSkybox(bool show);
	void ShowTexture(const string &fileName);
	void Render();
	virtual void Update(int type) override;
	virtual void OnDraw(CDC* pDC) { }


protected:
	Matrix44 m_rotateTm;
	graphic::cSkyBox m_skybox;
	graphic::cShader m_shader;
	graphic::cGrid2 m_grid;
	graphic::cText m_msg;
	graphic::cSphere m_lightSphere;
	graphic::cLine m_lightLine;
	graphic::cSprite *m_sprite;

	LPD3DXSPRITE m_dxSprite;

	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
	CPoint m_curPos;

	bool m_showSkybox;


public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


inline void CModelView::ShowSkybox(bool show) { m_showSkybox = show; }
