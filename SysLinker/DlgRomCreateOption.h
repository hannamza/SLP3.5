#pragma once
#include "resource.h"

// CDlgRomCreateOption 대화 상자입니다.

class CDlgRomCreateOption : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRomCreateOption)

public:
	CDlgRomCreateOption(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgRomCreateOption();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DIALOG_ROM_CREATE_OPTION };
#else
	enum { IDD = IDD_DIALOG_ROM_CREATE_OPTION_EN };
#endif
#endif

public:
	BOOL m_bErrorCheck;
	BOOL m_bAdminMode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckErrorCheck();
	afx_msg void OnBnClickedCheckAdminMode();
};
