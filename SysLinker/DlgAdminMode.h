#pragma once

// CDlgAdminMode ��ȭ �����Դϴ�.

class CDlgAdminMode : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAdminMode)

public:
	CDlgAdminMode(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgAdminMode();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADMIN_MOD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strEditPassword;
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
