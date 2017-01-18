#pragma once


// CTopPanel 프레임입니다.
class CTabPanel;
class CTopPanel : public CMiniFrameWnd
{
public:
	CTopPanel();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CTopPanel();

	void Init();


protected:
	CTabPanel *m_tabPanel;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


