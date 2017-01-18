#pragma once
#include "afxwin.h"


// CLightPanel 대화 상자입니다.

class CLightPanel : public CDialogEx
								, public common::iObserver2
{
public:
	CLightPanel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightPanel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LIGHT };

	virtual void Update(int type) override;


protected:
	CMFCPropertyGridCtrl m_modelProperty;
	CMFCPropertyGridCtrl m_lightProperty;


protected:
	void UpdateModelProperty(const graphic::cMaterial &mtrl);
	void UpdateLightProperty(const graphic::cLight &light);

	void ChangeMaterialValue(CMFCPropertyGridProperty *prop);
	void ChangeLightValue(CMFCPropertyGridProperty *prop);

	void AddPropertyColor4(CMFCPropertyGridProperty *group, CString name, Vector4 value);
	void AddPropertyVector3(CMFCPropertyGridProperty *group, CString name, Vector3 value,
		const float _min=0, const float _max=500, const int slice=10000);
	Vector4 GetPropertyVector4(CMFCPropertyGridProperty *group);
	Vector3 GetPropertyVector3(CMFCPropertyGridProperty *group);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSelchangeComboLight();
	LRESULT OnPropertyChanged (WPARAM wparam, LPARAM lparam); 
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CComboBox m_lightCombo;
	afx_msg void OnBnClickedCheckDirection();
	BOOL m_EditDirection;
};
