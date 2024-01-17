#pragma once
#include "../Common/Control/SSplitter.h"
#include "../Common/XList/XListCtrl.h"
#include "afxcmn.h"

class CMapSystemData;


// CDlgOutputChange 대화 상자입니다.
//////////////////////////////////////////////////////////////////////////
// Outputlist에 선택안된것은 삭제됨

class CDlgOutputChange : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOutputChange)

public:
	CDlgOutputChange(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgOutputChange();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGOUTPUTCHANGE };
#endif

public:
	CSSplitter	m_SpMain;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
