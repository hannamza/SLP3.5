#pragma once
#include "../Common/Control/SSplitter.h"
#include "../Common/XList/XListCtrl.h"
class CMapSystemData;


// 1. ������ ���� ��ȭ�� �ִ°�
// 2. ������ ���� ��ȭ�� �ִ°�
// 3. ���� ���Ͽ� ���°� , ���� ���Ͽ� �ִ°� 
// 2. �ߺ��Ǵ°� ǥ��

// CDlgPatternChange ��ȭ �����Դϴ�.
class CMapSystemData;
class CDlgPatternChange : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPatternChange)

public:
	CDlgPatternChange(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgPatternChange();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGPATTERNCHANGE };
#endif
public:
	
	CTreeCtrl			m_ctrlPatternTree;
	CXListCtrl			m_ctrlRelayList;
	CSSplitter			m_SpMain;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	void SetSplitterPos();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	void DisplayCompareResult(
		int nCompareType
		, CMapSystemData * pOldMap
		, CMapSystemData * pNewMap
		, CPtrList * pPatternList
		, CWnd				* pMainWnd
	);
	// CPtrList * pPatternList ; 
	int DisplayList(HTREEITEM hItem);
	afx_msg void OnTvnSelchangedPatternTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRelayListCheck(NMHDR *pNMHDR, LRESULT *pResult);
};
