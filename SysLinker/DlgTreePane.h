#pragma once
#include "afxcmn.h"


// CDlgTreePane 대화 상자입니다.
class CRelayTableData;
class CDlgTreePane : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTreePane)

public:
	CDlgTreePane(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTreePane();
	CRelayTableData *		m_pRefFasSysData;
	CView			*		m_pMainForm;
	int						m_nPaneType; // 1: PSwitch , 2: Pump
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_TREEPANE };
#endif
	CTreeCtrl			m_ctrlTree;
	CImageList			m_ImageTree;
	CPtrList			m_ptrItemList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
	//< nFindType : 0-압력스위치 , 1-펌프 , 2-수신기

	int AddTreeData(LPVOID pData);
	int ChangeTreeData(LPVOID pData);
	int DeleteTreeData(LPVOID pData);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMSetfocusTree(NMHDR *pNMHDR, LRESULT *pResult);
};
