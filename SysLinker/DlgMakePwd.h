#pragma once


// CDlgMakePwd 대화 상자입니다.

class CDlgMakePwd : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMakePwd)

public:
	CDlgMakePwd(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMakePwd();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_PASSWORD };
#else
	enum { IDD = IDD_DLG_PASSWORD_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnMake();
	UINT m_uInputNum;
	UINT m_uResult;
};
