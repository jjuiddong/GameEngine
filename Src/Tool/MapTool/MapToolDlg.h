
// MapToolDlg.h : ��� ����
//

#pragma once

class  CMapView;
class CTopPanel;

// CMapToolDlg ��ȭ ����
class CMapToolDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMapToolDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	virtual ~CMapToolDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MAPTOOL_DIALOG };

	void MainLoop();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;
	bool m_loop;
	CMapView *m_mapView;
	CTopPanel *m_mainPanel;
	// GDI plus
	ULONG_PTR m_gdiplusToken;
	GdiplusStartupInput m_gdiplusStartupInput; 



	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
