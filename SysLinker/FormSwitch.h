#pragma once



// CFormSwitch �� ���Դϴ�.

class CFormSwitch : public CFormView
{
	DECLARE_DYNCREATE(CFormSwitch)

protected:
	CFormSwitch();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormSwitch();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMSWITCH };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


