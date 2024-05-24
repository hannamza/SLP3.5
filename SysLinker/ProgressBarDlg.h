#pragma once
#include "afxcmn.h"


// CProgressBarDlg ��ȭ �����Դϴ�.

class CProgressBarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	CProgressBarDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	CProgressBarDlg(CString strMsg = _T(""));
	virtual ~CProgressBarDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROGRESS_BAR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CProgressCtrl m_ctrlProgressBar;
	CString m_strMsg;
};
