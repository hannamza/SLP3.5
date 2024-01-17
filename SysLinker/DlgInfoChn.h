#pragma once

#include "DlgInfoBasePage.h"

// CDlgInfoChn ��ȭ �����Դϴ�.
class CRelayTableData;

class CDlgInfoChn : public CDlgInfoBasePage
{
	DECLARE_DYNAMIC(CDlgInfoChn)

public:
	CDlgInfoChn(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgInfoChn();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INFO_EDIT_CHN };
#endif
public:
	CComboBox m_cmbFacp;
	CComboBox m_cmbUnit;
	CString m_strName;
	int m_nNum;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_INFO_EDIT_CHN, pParent); }
	virtual void DisplayItem(ST_TREEITEM * pData, ST_TREEITEM * pNewData);
	virtual void PrjDataInit(int nInitType);
	virtual BOOL GetChangeData();
	virtual BOOL OnInitDialog();
	virtual void ChangeDataControlUpdate();
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
