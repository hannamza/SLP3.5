#pragma once
#include "afxeditbrowsectrl.h"
#include "afxwin.h"
#include "afxcmn.h"


// CPropPageDevice 대화 상자입니다.
class CRelayTableData;
class CPropPageDevice : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropPageDevice)

public:
	CPropPageDevice();   // 표준 생성자입니다.
	virtual ~CPropPageDevice();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROP_PAGE_DEVICETABLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_lstTable;
	CString m_strPath;

	CList<int> m_FacpNumList;

	CRelayTableData		*		m_pRefFasSysData; /// 중계기 일람표의 모든데이터

	CListCtrl m_ctrlListCtrl;
	int m_nListCtrlSelIndex;
	CString m_strEditPath;

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
