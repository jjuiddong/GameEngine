#pragma once
#include "afxcmn.h"


// CAnimationController 대화 상자입니다.

class CAnimationController2 : public CDialogEx
												, public common::iObserver2
{
public:
	CAnimationController2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAnimationController2();
	enum { IDD = IDD_DIALOG_ANICONTROLLER };

	virtual void Update(int type) override;
	void Update(const float elapseT);


protected:
	void UpdateController();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CSliderCtrl m_Slider;
	virtual BOOL OnInitDialog();
	int m_StartFrame;
	int m_EndFrame;
	int m_CurrentFrame;
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnNMCustomdrawSliderFrame(NMHDR *pNMHDR, LRESULT *pResult);
};
