#pragma once



// CFormFacp �� ���Դϴ�.

class CFormFacp : public CFormView
{
	DECLARE_DYNCREATE(CFormFacp)

protected:
	CFormFacp();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


