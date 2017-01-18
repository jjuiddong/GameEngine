#pragma once
#include "afxcmn.h"
#include "BoneTreeCtrl.h"


// CBoneDialog 대화 상자입니다.

class CBoneDialog : public CDialogEx
										, public common::iObserver2
{
public:
	CBoneDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBoneDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_BONE };


public:
	virtual void Update(int type) override;
	bool UpdateTree();


protected:
	void UpdateBoneNodeInfo(graphic::cBoneNode *boneNode);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CBoneTreeCtrl m_boneTree;
	afx_msg void OnSelchangedTreeBone(NMHDR *pNMHDR, LRESULT *pResult);
	CTreeCtrl m_nodeInfoTree;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int m_boneCount;
};


void InitBoneDialog(CWnd *parent);
void ShowBoneDialog();
void HideBoneDialog();
void DestroyBoneDialog();
