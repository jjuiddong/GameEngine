////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <SDKDDKVer.h>
#include <afxcontrolbars.h>
////////////////////////////////////////////////////////////////////////////////////
class CPropGridSlider;
class CPropSlider : public CSliderCtrl
{
public:
	CPropSlider(CPropGridSlider *parent_prop) :CSliderCtrl() {
		m_iPrevPos = -1; 
		m_pParentProp = parent_prop;
	}
	
protected:
	CPropGridSlider *m_pParentProp;
	int m_iPrevPos;
	void OnUpdateValue();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMReleasedcapture(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

class CPropGridSlider : public CMFCPropertyGridProperty
{
	// OnEdit -> CreateInPlaceEdit -> OnClickValue
	// OnPosSizeChanged -> OnDrawValue
public:

protected:
	CPropSlider *m_pSlider;
	float m_fMin, m_fMax;
	int m_iSliderStep;
	int m_iEditCtrlWidth;
	bool m_isSliderEdit;

public:
	CPropGridSlider(CString name, float val, CString tip, float min, float max, int step, int edit_ctrl_width=-2 ) :
		CMFCPropertyGridProperty(name, val, tip) {
		m_fMin = min;
		m_fMax = max;
		m_iSliderStep = step;
		m_pSlider = NULL;
		m_iEditCtrlWidth = edit_ctrl_width;
		m_isSliderEdit = false;
	}
	virtual ~CPropGridSlider() {
		if (m_pSlider) delete m_pSlider;
	}

	void OnSliderPosChanged();
	void SetSliderPos();


//virtuals:
	virtual BOOL OnEndEdit();
	virtual void OnDrawValue(CDC* pDC, CRect rect);
	virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);
	virtual void SetValue(const COleVariant& varValue) {
		CMFCPropertyGridProperty::SetValue(varValue);
		SetSliderPos();
	}
};

////////////////////////////////////////////////////////////////////////////////////

