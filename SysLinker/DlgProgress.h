#pragma once
#include "afxcmn.h"


// CDlgProgress ��ȭ �����Դϴ�.

class CDlgProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgProgress();
	CProgressCtrl m_ctrlProg;

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_PROGRESS };
#else
	enum { IDD = IDD_DLG_PROGRESS_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	void InitProgress() { m_ctrlProg.SetRange(0, 100); }
	void SetPos(int nPos) { m_ctrlProg.SetPos(nPos); }
	void SetProgress(CString strText , int nPos);
};
