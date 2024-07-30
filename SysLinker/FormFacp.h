#pragma once



// CFormFacp 폼 뷰입니다.

class CFormFacp : public CFormView
{
	DECLARE_DYNCREATE(CFormFacp)

protected:
	CFormFacp();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormFacp();

public:
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_FORMFACP };
#else
	enum { IDD = IDD_FORMFACP_EN };
#endif
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
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


