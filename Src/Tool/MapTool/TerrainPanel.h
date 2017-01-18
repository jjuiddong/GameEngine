#pragma once
#include "afxeditbrowsectrl.h"
#include "afxwin.h"
#include "afxcmn.h"


// CTerrainPanel 대화 상자입니다.

class CTerrainPanel : public CDialogEx
								, public common::iObserver2
{
public:
	CTerrainPanel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTerrainPanel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TERRAIN };


public:
	virtual void Update(int type) override;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonLoadTerrain();
	afx_msg void OnBnClickedButtonCrterrain();
	afx_msg void OnBnClickedButtonSaveTerrain();
	int m_colCellCount;
	int m_rowCellCount;
	float m_cellSize;
	CMFCEditBrowseCtrl m_textureBrowser;
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeMfceditbrowseTexture();
	CComboBox m_ComboEditMode;
	afx_msg void OnCbnSelchangeComboTerrainEdit();
	CSliderCtrl m_SliderRadius;
	float m_Radius;
	afx_msg void OnNMCustomdrawSliderOuterRadius(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditOuterRadius2();
	CSliderCtrl m_SliderSpeed;
	float m_Speed;
	afx_msg void OnNMCustomdrawSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditSpeed();
	afx_msg void OnBnClickedButtonSaveTerrainTexture();
	afx_msg void OnBnClickedCheckWater();
	BOOL m_checkWater;
	CSliderCtrl m_sliderUVFactor;
	float m_uvFactor;
	afx_msg void OnNMCustomdrawSliderUvFactor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditUvFactor();
};
