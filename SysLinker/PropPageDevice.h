#pragma once
#include "afxeditbrowsectrl.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "ProgressBarDlg.h"

// CPropPageDevice ��ȭ �����Դϴ�.
class CRelayTableData;
class CPropPageDevice : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropPageDevice)

public:
	CPropPageDevice();   // ǥ�� �������Դϴ�.
	virtual ~CPropPageDevice();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROP_PAGE_DEVICETABLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_lstTable;
	CString m_strPath;

	CList<int> m_FacpNumList;

	CRelayTableData		*		m_pRefFasSysData; /// �߰�� �϶�ǥ�� ��絥����

	CListCtrl m_ctrlListCtrl;
	int m_nListCtrlSelIndex;
	CString m_strEditPath;
	
	CProgressBarDlg* m_pProgressBarDlg;	//20240523 GBM - ������ ��� �� ���α׷����� �˾�
	HANDLE	m_hThreadHandle;	//20240523 GBM - ������ ��� �̺�Ʈ �ڵ�

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnDel();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnBrowser();


	virtual LRESULT OnWizardNext();
	int CheckDuplicate(int nValue);
	void SetFasSysData(CRelayTableData * pData) { m_pRefFasSysData = pData; }
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
