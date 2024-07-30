#pragma once


// CDlgMakeAutoOnInput 대화 상자입니다.

class CDlgMakeAutoOnInput : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMakeAutoOnInput)

public:
	CDlgMakeAutoOnInput(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMakeAutoOnInput();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_MAKEAUTO_ONEINPUT };
#else
	enum { IDD = IDD_DLG_MAKEAUTO_ONEINPUT_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
