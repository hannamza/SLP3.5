#pragma once


// CDlgProgramLogin ��ȭ �����Դϴ�.
class YAdoDatabase;
class CDlgProgramLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProgramLogin)

public:
	CDlgProgramLogin(YAdoDatabase * pMainDB , CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgProgramLogin();


	YAdoDatabase *		m_pRefMainDB;
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_SLPLOGIN };
#else
	enum { IDD = IDD_DLG_SLPLOGIN_EN};
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnEnv();
	virtual BOOL OnInitDialog();
	CString m_strUser;
	CString m_strPwd;
	afx_msg void OnBnClickedOk();
};
