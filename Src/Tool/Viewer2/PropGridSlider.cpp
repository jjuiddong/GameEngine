////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PropGridSlider.h"
////////////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CPropSlider, CSliderCtrl)
	ON_NOTIFY_REFLECT(NM_RELEASEDCAPTURE, &CPropSlider::OnNMReleasedcapture)
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void CPropSlider::OnNMReleasedcapture(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnUpdateValue();
	*pResult = 0;
}

void CPropSlider::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	OnUpdateValue();
	CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CPropSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags == 1)
	{
		SetFocus();
		OnUpdateValue();
	}
	CSliderCtrl::OnMouseMove(nFlags, point);
}


BOOL CPropSlider::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	OnUpdateValue();
	return CSliderCtrl::OnMouseWheel(nFlags, zDelta, pt);
}


void CPropSlider::OnUpdateValue()
{
	int pos = GetPos();
	if (pos != m_iPrevPos) {
		m_iPrevPos = pos;
		m_pParentProp->OnSliderPosChanged();
		SetFocus();
	}
}

BOOL CPropSlider::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return TRUE;
	//return CSliderCtrl::OnSetCursor(pWnd, nHitTest, message);
}


//void CPropSlider::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	RedrawWindow();
//}


void CPropGridSlider::OnSliderPosChanged()
{
	int pos = m_pSlider->GetPos();
	float ratio = ((float)pos) / ((float)m_iSliderStep);
	float value = (m_fMax - m_fMin) * ratio + m_fMin;

	m_isSliderEdit = true;
	SetValue((float)value);
	m_pWndList->OnPropertyChanged(this);
	m_isSliderEdit = false;
}

void CPropGridSlider::SetSliderPos()
{
	COleVariant value = GetValue();
	float fval = value.fltVal;
	float ratio = (fval - m_fMin) / (m_fMax - m_fMin);
	int pos = (int)(ratio * ((float)m_iSliderStep));

	if (m_pSlider)
		m_pSlider->SetPos(pos);
}

BOOL CPropGridSlider::OnEndEdit() 
{
	if (m_isSliderEdit)
		return CMFCPropertyGridProperty::OnEndEdit();

	SetSliderPos();
	return CMFCPropertyGridProperty::OnEndEdit();
}

void CPropGridSlider::OnDrawValue(CDC* pDC, CRect rect)
{
	CRect rt = rect;
	int w = rect.Width();

	rect.right = rect.left 
		+ (m_iEditCtrlWidth < 0 ? w / (-m_iEditCtrlWidth) : m_iEditCtrlWidth);
	rt.left = rect.right;

	if (!m_pSlider) {
		m_pSlider = new CPropSlider(this);
		DWORD style = WS_VISIBLE | WS_CHILD | TBS_BOTH | TBS_NOTICKS;
		m_pSlider->Create(style, rt, m_pWndList, 5);
		m_pSlider->SetRange(0, m_iSliderStep);
		m_pSlider->ShowWindow(SW_SHOW);
		SetSliderPos();
	}
	
	m_pSlider->MoveWindow(&rt);
	CMFCPropertyGridProperty::OnDrawValue(pDC, rect);
}

CWnd* CPropGridSlider::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	int w = rectEdit.Width();
	rectEdit.right = rectEdit.left 
		+ (m_iEditCtrlWidth < 0 ? w / (-m_iEditCtrlWidth) : m_iEditCtrlWidth);

	return CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit, bDefaultFormat);
}
