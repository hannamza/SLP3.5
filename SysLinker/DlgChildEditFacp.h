#pragma once
#include "afxwin.h"
#include "DlgChildBasePage.h"


// CDlgChildEditFacp ��ȭ �����Դϴ�.
class CDlgChildEditMain;
class CRelayTableData;

class CDlgChildEditFacp : public CDlgChildBasePage
{
	DECLARE_DYNAMIC(CDlgChildEditFacp)

public:
	CDlgChildEditFacp(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgChildEditFacp();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_CHILD_EDIT_FACP };
#else
	enum { IDD = IDD_DLG_CHILD_EDIT_FACP_EN };
#endif
#endif
public:
	CString m_strName;
	int m_nNum;
	CComboBox m_cmbType;
protected:	

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
#ifndef ENGLISH_MODE
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_FACP, pParent); }
#else
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_FACP_EN, pParent); }
#endif
	virtual void DisplayItem(ST_TREEITEM * pData, ST_TREEITEM * pNewData);
	virtual BOOL GetChangeData();
	virtual void PrjDataInit(int nInitType);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	void SetTypeCombo(int nType);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	afx_msg void OnCbnSelchangeCmbType();
	afx_msg void OnEnChangeEdNum();
};
