#pragma once
#include "afxcmn.h"


// CProgressBarDlg 대화 상자입니다.

class CProgressBarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	CProgressBarDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CProgressBarDlg(CString strMsg = _T(""));
	virtual ~CProgressBarDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROGRESS_BAR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CProgressCtrl m_ctrlProgressBar;
	CString m_strMsg;
};
