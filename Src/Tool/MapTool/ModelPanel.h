#pragma once
#include "afxeditbrowsectrl.h"
#include "afxwin.h"
#include "afxcmn.h"


// CModelPanel 대화 상자입니다.

class CModelPanel : public CPanelBase
								, public iObserver2
{
public:
	CModelPanel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CModelPanel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MODEL };


public:
	virtual void Update(int type) override;
	void UpdateModelList(const string &directoryPath);
	void UpdatePlaceModelList();
	void UpdateModelInfo(const graphic::cModel &model, const bool updateList=true);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CListCtrl m_placeModelList;
	afx_msg void OnBnClickedButtonRefresh();
	CFileTreeCtrl m_modelTree;
	afx_msg void OnTvnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnModelmenuDeletemodel();
	afx_msg void OnLvnItemchangedListPlaceModel(NMHDR *pNMHDR, LRESULT *pResult);
	float m_PosX;
	float m_PosY;
	float m_PosZ;
	float m_RotX;
	float m_RotY;
	float m_RotZ;
	float m_ScaleX;
	float m_ScaleY;
	float m_ScaleZ;
	afx_msg void OnEnChangeEditModel(UINT id);
	CString m_modelName;
};
