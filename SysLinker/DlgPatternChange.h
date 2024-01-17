#pragma once
#include "../Common/Control/SSplitter.h"
#include "../Common/XList/XListCtrl.h"
class CMapSystemData;


// 1. 패턴의 개수 변화가 있는거
// 2. 패턴의 내용 변화가 있는거
// 3. 기존 패턴에 없는거 , 기존 패턴에 있는거 
// 2. 중복되는거 표시

// CDlgPatternChange 대화 상자입니다.
class CMapSystemData;
class CDlgPatternChange : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPatternChange)

public:
	CDlgPatternChange(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPatternChange();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGPATTERNCHANGE };
#endif
public:
	
	CTreeCtrl			m_ctrlPatternTree;
	CXListCtrl			m_ctrlRelayList;
	CSSplitter			m_SpMain;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
