#pragma once



// CFormUser 폼 뷰입니다.

class CFormUser : public CFormView
{
	DECLARE_DYNCREATE(CFormUser)

protected:
	CFormUser();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormUser();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMUSER };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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


