#pragma once


// CDlgEnv 대화 상자입니다.

class CDlgEnv : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEnv)

public:
	CDlgEnv(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgEnv();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_ENV };
#else
	enum { IDD = IDD_DLG_ENV_EN };
#endif
#endif

	CString m_strAddr;
	CString m_strName;
	CString m_strUser;
	CString m_strPwd;

	UINT m_nSerialPort;
	CComboBox m_cmbBaudrate;
	CComboBox m_cmbStopBit;
	CComboBox m_cmbParityBit;
	CComboBox m_cmbDataBit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnBrowser();
	CString m_strPrjPath;
	int SetComboSelect(CComboBox* pCmb, int nData);
	UINT m_nDBPort;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
