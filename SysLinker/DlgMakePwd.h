#pragma once


// CDlgMakePwd ��ȭ �����Դϴ�.

class CDlgMakePwd : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMakePwd)

public:
	CDlgMakePwd(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMakePwd();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_PASSWORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnMake();
	UINT m_uInputNum;
	UINT m_uResult;
};
