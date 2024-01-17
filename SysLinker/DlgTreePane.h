#pragma once
#include "afxcmn.h"


// CDlgTreePane ��ȭ �����Դϴ�.
class CRelayTableData;
class CDlgTreePane : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTreePane)

public:
	CDlgTreePane(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgTreePane();
	CRelayTableData *		m_pRefFasSysData;
	CView			*		m_pMainForm;
	int						m_nPaneType; // 1: PSwitch , 2: Pump
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_TREEPANE };
#endif
	CTreeCtrl			m_ctrlTree;
	CImageList			m_ImageTree;
	CPtrList			m_ptrItemList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL CreateDlg(int nPaneType, CWnd * pParent);
	void RemoveAllData();
	int FillPSwitchTree();
	int FillPumpTree();
	void SetMainWindow(CView * pMainForm) { m_pMainForm = pMainForm; }
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	int InitDlg();
	
	
	HTREEITEM FindTreeItem(int nFindType, int nFacpID, int nPID, HTREEITEM hItem);
	//< nFindType : 0-�з½���ġ , 1-���� , 2-���ű�

	int AddTreeData(LPVOID pData);
	int ChangeTreeData(LPVOID pData);
	int DeleteTreeData(LPVOID pData);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMSetfocusTree(NMHDR *pNMHDR, LRESULT *pResult);
};
