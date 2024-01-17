#pragma once
#include "../Common/Control/SSplitter.h"
#include "../Common/XList/XListCtrl.h"
#include "afxcmn.h"

class CMapSystemData;


// CDlgOutputChange ��ȭ �����Դϴ�.
//////////////////////////////////////////////////////////////////////////
// Outputlist�� ���þȵȰ��� ������

class CDlgOutputChange : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOutputChange)

public:
	CDlgOutputChange(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgOutputChange();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGOUTPUTCHANGE };
#endif

public:
	CSSplitter	m_SpMain;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

	void SetSplitterPos();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CTreeCtrl			m_ctrlOutTree;
	CXListCtrl			m_ctrlRelayList;

	HTREEITEM			m_hItem[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT][4];
	HTREEITEM			m_hFacp[D_MAX_FACP_COUNT];
	void DisplayCompareResult(
		int nCompareType
		, CMapSystemData * pOldMap
		, CMapSystemData * pNewMap
		, CPtrList * pPtrUSINGDupList
		, CWnd				* pMainWnd
	);
	int DisplayList(HTREEITEM hItem);
	afx_msg void OnTvnSelchangedOutTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRelayListCheck(NMHDR *pNMHDR, LRESULT *pResult);
};
