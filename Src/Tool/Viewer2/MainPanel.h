#pragma once


// CMainPanel 프레임입니다.

class CMainPanelTab;
class CMainPanel : public CMiniFrameWnd
{
public:
	CMainPanel();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMainPanel();

	void Init();


protected:
	DECLARE_MESSAGE_MAP()
	CMainPanelTab *m_mainPanel;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
