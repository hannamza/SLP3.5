#pragma once
#include "resource.h"
#include "afxcmn.h"

// CDlgDelayTreePane 대화 상자입니다.

class CDlgDelayTreePane : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDelayTreePane)

public:
	CDlgDelayTreePane(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDelayTreePane();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DELAY_TREE_PANE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	// Variable
	CTreeCtrl			m_ctrlGroupTree;
	CImageList			m_ImageTree;
	CView*				m_pParent;
	CRelayTableData *	m_pRefFasSysData;
	HTREEITEM			m_treeItemRoot;
	unsigned short		m_nCurGroupNum;

	// method
	BOOL CreateDlg(CWnd * pParent);
	void SetParentForMessage(CView* pParent);
	void FillGroupTree();
	void SetTreeIcons();
	BOOL AddNewGroupToTree(unsigned short nGroupNum);
	BOOL SaveGroupToTree(unsigned short nGroupNum);
	BOOL DelGroupFromTree(unsigned short nGroupNum);
	HTREEITEM FindTreeItemByGroupNum(unsigned short nGroupNum);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeDelay(NMHDR *pNMHDR, LRESULT *pResult);
};
