#pragma once
#include "resource.h"
#include "XList/XListCtrl.h"

// CDlgChildDelayOutput 대화 상자입니다.

class CDlgChildDelayOutput : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChildDelayOutput)

public:
	CDlgChildDelayOutput(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgChildDelayOutput();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DELAY_OUTPUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

	// variable
	CView* m_pParent;
	int m_nDelayTime;

	// mathod
	BOOL CreateDlg(CWnd * pParent);
	void SetParentForMessage(CView* pParent);
	afx_msg void OnBnClickedButtonAddDelayTime();
	virtual BOOL OnInitDialog();
};
