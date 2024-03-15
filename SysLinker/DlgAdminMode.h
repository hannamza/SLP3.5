#pragma once

// CDlgAdminMode 대화 상자입니다.

class CDlgAdminMode : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAdminMode)

public:
	CDlgAdminMode(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgAdminMode();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADMIN_MOD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strEditPassword;
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
