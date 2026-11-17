#pragma once
#include "resource.h"

// CDlgChildDelay 대화 상자입니다.

class CDlgChildDelay : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChildDelay)

public:
	CDlgChildDelay(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgChildDelay();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DELAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonDel();

	//variable
	CView* m_pParent;
	unsigned short m_nCurGroupNum;

	//method
	BOOL CreateDlg(CWnd * pParent);
	void SetParentForMessage(CView* pParent);
	void SetGroupInfo(unsigned short nGroupNum);
};
