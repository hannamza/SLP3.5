#pragma once
#include "afxcmn.h"

#include "../Common/XList/XListCtrl.h"
#include "MapSystemData.h"

// CDlgInputChange ��ȭ �����Դϴ�.

class CDlgInputChange : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInputChange)

public:
	CDlgInputChange(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgInputChange();



// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGINPUTCHANGE };
#endif
public:
	CXListCtrl m_ctrlInputList;
	VT_RUPDATEITEM	*	m_pVtIns;
	VT_RUPDATEITEM	*	m_pVtOld;
	VT_RUPDATEITEM	*	m_pVtNew;
	CPtrList		*	m_pRefUSEDDupList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();


	void DisplayCompareResult(
		int nCompareType
		, CMapSystemData	* pOldMap
		, CMapSystemData	* pNewMap
		, VT_RUPDATEITEM	* pVtIns
		, VT_RUPDATEITEM	* pVtOld
		, VT_RUPDATEITEM	* pVtNew
		, CPtrList			* pRefUSEDDupList
		, CWnd				* pMainWnd
	);

	afx_msg void OnInputListCheck(NMHDR *pNMHDR, LRESULT *pResult);
	void SetOutputEnable(int nFacpNum, int nUnitNum, int nChnNum, int nDevNum, BOOL bEnable);
};
