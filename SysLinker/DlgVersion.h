#pragma once
#include "afxcmn.h"


// CDlgVersion ��ȭ �����Դϴ�.

class CDlgVersion : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVersion)

public:
	CDlgVersion(CString strPrjName , CString strPrjPath, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgVersion();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_VERSION };
#else
	enum { IDD = IDD_DLG_VERSION_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlVersionList;
	afx_msg void OnBnClickedOk();

	WORD m_wMajor;
	WORD m_wMinor;

	WORD m_wLastMajor;
	WORD m_wLastMinor;

	CString		m_strPrjName;
	CString		m_strPrjPath;
	int FillVersionList(CString strPath);
	int SetVersionList(int nRow, CString strVerFile);
};
