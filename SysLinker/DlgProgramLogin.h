#pragma once


// CDlgProgramLogin 대화 상자입니다.
class YAdoDatabase;
class CDlgProgramLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProgramLogin)

public:
	CDlgProgramLogin(YAdoDatabase * pMainDB , CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgProgramLogin();


	YAdoDatabase *		m_pRefMainDB;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_SLPLOGIN };
#else
	enum { IDD = IDD_DLG_SLPLOGIN_EN};
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnEnv();
	virtual BOOL OnInitDialog();
	CString m_strUser;
	CString m_strPwd;
	afx_msg void OnBnClickedOk();
};
