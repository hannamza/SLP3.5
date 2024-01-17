#pragma once
#include "afxcmn.h"
#include "../Common/Control/DTreeCtrl.h"


// CDlgChildChangeView ��ȭ �����Դϴ�.
class CRelayTableData;
class CDlgRETabPattern;
class CDlgRETabRelay;
class CDlgChildChangeView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChildChangeView)

public:
	CDlgChildChangeView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgChildChangeView();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CHILD_CHANGEVIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CRelayTableData		*	m_pRefFasSysData;
	CFormView			*	m_pMainForm;
	CDlgRETabPattern	*	m_pTabPattern;
	CDlgRETabRelay		*	m_pTabRelay;
	ST_TREEITEM			*	m_pRefCurData;
	ST_TREEITEM			*	m_pRefNewData;
// 	VT_HITEM			*	m_pvtRefCurData;
// 	VT_HITEM			*	m_pvtRefNewData;
public:
	BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_CHANGEVIEW, pParent); }
	void SetMainWindow(CFormView * pMainForm) { m_pMainForm = pMainForm; }
	void SetChangeContainer(CPtrList * pRefList, CMapChangeDevice * pRefMap);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CTabCtrl m_ctrlTab;
	afx_msg void OnTcnSelchangeEffectTab(NMHDR *pNMHDR, LRESULT *pResult);
	void SetTabPos();
	int TreeSelectChange(ST_TREEITEM * pCurItem, ST_TREEITEM * pNewItem);
	int EditDlgPreviewChange(int nDatatype , int nAction);

	void ReloadData();
};
