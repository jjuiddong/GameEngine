#pragma once


// CMapView 뷰입니다.

class cTerrainCursor;
class CMapView : public CView
{
public:
	CMapView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMapView();

	bool Init();
	void Render();
	void Update(float elapseT);
	graphic::cModel *GetFocusModel();
	void SetFocusModel(graphic::cModel *model);


protected:
	bool m_dxInit;
	string m_filePath;
	common::Ray m_ray;

	graphic::cModel *m_focusModel; // refenrece

	graphic::cSphere m_lightSphere;
	graphic::cLine m_lightLine;

	CPoint m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;


public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
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
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


inline graphic::cModel* CMapView::GetFocusModel() { return m_focusModel; }
inline void CMapView::SetFocusModel(graphic::cModel *model) { m_focusModel = model; }
