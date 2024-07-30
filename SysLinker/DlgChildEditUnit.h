#pragma once
#include "DlgChildBasePage.h"


// CDlgChildEditUnit ��ȭ �����Դϴ�.
class CDlgChildEditMain;
class CRelayTableData;

class CDlgChildEditUnit : public CDlgChildBasePage
{
	DECLARE_DYNAMIC(CDlgChildEditUnit)

public:
	CDlgChildEditUnit(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgChildEditUnit();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_CHILD_EDIT_UNIT };
#else
	enum { IDD = IDD_DLG_CHILD_EDIT_UNIT_EN };
#endif
#endif
public:
	CComboBox m_cmbFacp;
	CComboBox m_cmbType;
	CString m_strName;
	int m_nNum;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
#ifndef ENGLISH_MODE
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_UNIT, pParent); }
#else
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_UNIT_EN, pParent); }
#endif
	virtual void DisplayItem(ST_TREEITEM * pData, ST_TREEITEM * pNewData);
	virtual BOOL GetChangeData();
	virtual void PrjDataInit(int nInitType);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	void SetTypeCombo(int nType);
	void SetFacpCombo(int nFacp);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	void FillFacpCombo();
	afx_msg void OnEnChangeEdNum();
	afx_msg void OnCbnSelchangeCmbFacp();
};
