#pragma once
#include "afxwin.h"


// CDlgLoadRom ��ȭ �����Դϴ�.
class CRelayTableData;
class CDlgLoadRom : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLoadRom)

public:
	CDlgLoadRom(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgLoadRom();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_LOADROM };
#else
	enum { IDD = IDD_DLG_LOADROM_EN};
#endif
#endif
protected:
	CString m_strPrjName;
	CString m_strCreateDate;
	CString m_strDBName;
	CString m_strMaker;
	CString m_strPrjPath;
	CString m_strPrjVersion;
	CString m_strSiteName;
	CString m_strSiteManager;
	CString m_strSitePhone;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	SIMPLE_FUNC_IMPL(CString, PrjName, m_strPrjName);
	SIMPLE_FUNC_IMPL(CRelayTableData *, RefRelayTable, m_pRefRelayTable);

	CListBox			m_lbRomList;
	CString				m_strRomPath;
	CPtrList			m_ptrList;

	CRelayTableData		*	m_pRefRelayTable;

	BOOL				m_bFoundRvFile;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnBrowser();
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
