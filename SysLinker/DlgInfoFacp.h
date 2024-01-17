#pragma once
#include "DlgInfoBasePage.h"
class CRelayTableData;

// CDlgInfoFacp ��ȭ �����Դϴ�.

class CDlgInfoFacp : public CDlgInfoBasePage
{
	DECLARE_DYNAMIC(CDlgInfoFacp)

public:
	CDlgInfoFacp(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgInfoFacp();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INFO_EDIT_FACP };
#endif
public:
	CString m_strName;
	int m_nNum;
	CComboBox m_cmbType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_INFO_EDIT_FACP, pParent); }
	virtual void DisplayItem(ST_TREEITEM * pData, ST_TREEITEM * pNewData);
	virtual BOOL GetChangeData();
	virtual void PrjDataInit(int nInitType);
	virtual void ChangeDataControlUpdate();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	void SetTypeCombo(int nType);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	afx_msg void OnCbnSelchangeCmbType();
	afx_msg void OnEnChangeEdNum();
};
