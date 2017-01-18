#pragma once
#include "afxwin.h"
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"


// CBrushPanel 대화 상자입니다.

class CBrushPanel : public CPanelBase
								, public common::iObserver2
{
public:
	CBrushPanel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBrushPanel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_BRUSH };


public:
	virtual void Update(int type) override;


protected:
	void UpdateTextureFiles( const string &directoryPath );
	void UpdateLayerList();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	Image *m_texture;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_TextureFiles;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	CMFCEditBrowseCtrl m_textureBrowser;
	afx_msg void OnChangeMfceditbrowseTexture();
	CListCtrl m_layerList;
	CSliderCtrl m_innerRSlider;
	CSliderCtrl m_outerRSlider;
	float m_innerRadius;
	float m_outerRadius;
	afx_msg void OnNMCustomdrawSliderInnerRadius(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderOuterRadius(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditInnerRadius();
	afx_msg void OnEnChangeEditOuterRadius2();
	BOOL m_IsEraseMode;
	afx_msg void OnBnClickedCheckErase();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnDeleteLayer();
	afx_msg void OnBnClickedButtonRefresh();
	CFileTreeCtrl m_brushTree;
	afx_msg void OnTvnSelchangedTreeBrush(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
