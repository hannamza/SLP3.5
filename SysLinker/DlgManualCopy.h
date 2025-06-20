#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgManualCopy 대화 상자입니다.

class CDlgManualCopy : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgManualCopy)

public:
	CDlgManualCopy(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgManualCopy();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DIALOG_MANUAL_COPY };
#else
	enum { IDD = IDD_DIALOG_MANUAL_COPY_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// Valuable

	CEdit m_ctrlEditInput;
	CListCtrl m_ctrlListPattern;
	CListCtrl m_ctrlListRelay;

	// Method

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();

	void InitControls();
	void SetNewInfo();
	void SetInputCircuitInfo();
	void SetManualOutputInfo();
	void InsertManualOutputRelayList(MANUAL_COPY_INFO mci, int nIndex);
	void InsertManualOutputPatternList(MANUAL_COPY_INFO mci, int nIndex);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
