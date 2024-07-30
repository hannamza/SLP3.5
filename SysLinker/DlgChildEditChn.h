#pragma once
#include "afxwin.h"
#include "DlgChildBasePage.h"

// CDlgChildEditChn ��ȭ �����Դϴ�.
class CDlgChildEditMain;
class CRelayTableData;
class CDlgChildEditChn : public CDlgChildBasePage
{
	DECLARE_DYNAMIC(CDlgChildEditChn)

public:
	CDlgChildEditChn(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgChildEditChn();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_CHILD_EDIT_CHN };
#else
	enum { IDD = IDD_DLG_CHILD_EDIT_CHN_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbFacp;
	CComboBox m_cmbUnit;
	CString m_strName;
	int m_nNum;
	
public:
#ifndef ENGLISH_MODE
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_CHN, pParent); }
#else
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_CHN_EN, pParent); }
#endif
	virtual void DisplayItem(ST_TREEITEM * pData , ST_TREEITEM * pNewData);
	virtual void PrjDataInit(int nInitType);
	virtual BOOL GetChangeData();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	void SetUnitCombo(int nFacp, int nUnit);
	void SetFacpCombo(int nFacp);
	void FillFacpCombo();
	void FillUnitCombo(int nFacp);
	afx_msg void OnCbnSelchangeCmbFacp();
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	afx_msg void OnCbnSelchangeCmbUnit();
	afx_msg void OnEnChangeEdNum();
};
