#pragma once


// CTopPanel �������Դϴ�.
class CTabPanel;
class CTopPanel : public CMiniFrameWnd
{
public:
	CTopPanel();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CTopPanel();

	void Init();


protected:
	CTabPanel *m_tabPanel;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


