#pragma once


// CDlgMakeAutoOnInput ��ȭ �����Դϴ�.

class CDlgMakeAutoOnInput : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMakeAutoOnInput)

public:
	CDlgMakeAutoOnInput(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMakeAutoOnInput();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MAKEAUTO_ONEINPUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
