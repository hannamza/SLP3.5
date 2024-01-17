#pragma once
#include "afxcmn.h"
#include "../common/Control/DTreeCtrl.h"


// CDlgChildRelayTree 대화 상자입니다.
class CRelayTableData;
class CDataSystem;
class CDlgChildRelayTree : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChildRelayTree)

public:
	CDlgChildRelayTree(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgChildRelayTree();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CHILD_RELAYTREE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CDTreeCtrl				m_ctrlRelay;
	CRelayTableData		*	m_pRefFasSysData;
	CFormView			*	m_pMainForm;
	CPtrList				m_ptrItemList;
public:
	BOOL CreateDlg(CWnd * pParent){ return Create(IDD_DLG_CHILD_RELAYTREE, pParent); }
	void SetMainWindow(CFormView * pMainForm) { m_pMainForm = pMainForm; }
	void RemoveAllData();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnCheckChangeItem(NMHDR *pNMHDR, LRESULT *pResult);

	int RefreshTree(int nType , int nFid , int nUid=-1 , int nCid = -1, int nRid = -1);
	HTREEITEM FindTreeItem(int nFindType, int nFid, int nUid, int nCid, int nRid, HTREEITEM hItem);
	int ReloadData();
};
