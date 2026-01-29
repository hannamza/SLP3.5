#pragma once
#include "ProgressBarDlg.h"

// CDlgWebLogin 대화 상자입니다.

class CDlgWebLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWebLogin)

public:
	CDlgWebLogin(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgWebLogin();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DIALOG_WEB_LOGIN };
#else
	enum { IDD = IDD_DIALOG_WEB_LOGIN_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strEditEmail;
	CString m_strEditPassword;

	HANDLE m_hThreadHandle;
	CString m_strErr;
	CProgressBarDlg* m_pProgressBarDlg;

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	bool CheckEmailFormat(const CString& email);
	CString MakeErrorMsg(int nErrorType, int nStatusCode);
};
