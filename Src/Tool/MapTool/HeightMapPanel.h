#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "afxeditbrowsectrl.h"


// CHeightMapPanel ��ȭ �����Դϴ�.

class CHeightMapPanel : public CPanelBase
									, public common::iObserver2
{
public:
	CHeightMapPanel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CHeightMapPanel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_HEIGHTMAP };


public:
	virtual void Update(int type) override;
	void UpdateTerrainInfo();
	void UpdateHeightMapList();
	void UpdateTextureList();


private:
	Image *m_heightMap;
	Image *m_texture;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	CSliderCtrl m_heightSlider;
	float m_heightFactor;
	afx_msg void OnNMCustomdrawSliderHeightFactor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditHeightFactor();
	CSliderCtrl m_uvSlider;
	float m_uvFactor;
	afx_msg void OnNMCustomdrawSliderUvFactor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditUvFactor();
	afx_msg void OnBnClickedButtonRefresh();
	CFileTreeCtrl m_heightmapTree;
	CFileTreeCtrl m_textureTree;
	afx_msg void OnTvnSelchangedTreeHeightmap(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeTexture(NMHDR *pNMHDR, LRESULT *pResult);
	CMFCEditBrowseCtrl m_heightmapBrowser;
	CMFCEditBrowseCtrl m_textureBrowser;
	afx_msg void OnEnChangeMfceditbrowseHeightmap();
	afx_msg void OnEnChangeMfceditbrowseTexture();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
