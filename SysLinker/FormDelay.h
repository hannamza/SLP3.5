#pragma once

#include "Control/SplitterCtrl.h"
#include "XList/XListCtrl.h"

class CDlgChildDelay;
class CDlgDelayTreePane;
class CDlgChildDelayOutput;

#define IDC_DELAY_TIME_INPUT_LIST	2001
#define IDC_DELAY_TIME_OUTPUT_LIST	2002

// CFormDelay 폼 뷰입니다.

class CFormDelay : public CView
{
	DECLARE_DYNCREATE(CFormDelay)

protected:
	CFormDelay();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormDelay();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORM_DELAY };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	SplitterCtrl m_SpMain;
	SplitterCtrl m_SpLeft;
	SplitterCtrl m_SpRight;

	CDlgDelayTreePane* m_pDlgLeftTopDelayTreePane;
	CDlgChildDelay*	m_pDlgLeftBottomDelayInfo;
	CDlgChildDelayOutput* m_pDlgRightMiddleDelayOutput;
	CXListCtrl m_ctrlRightTopInputList;
	CXListCtrl m_ctrlRightBottomOutputList;
	CRelayTableData *		m_pRefFasSysData;

	CBrush					m_brBackground;
	COLORREF				m_crBack;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();

	int SetSplitterPos();
	int InitSplitter();
	void RemoveAllData();
	void SetInputCircuitList(unsigned short nGroupNum);
	void SetOutputCircuitList(unsigned short nGroupNum);
	void MoveSelectedOutputCircuit(BOOL bMoveUp);

	LRESULT OnDelayTreeSelChange(WPARAM wp, LPARAM lp);
	LRESULT OnAddGroup(WPARAM wp, LPARAM lp);
	LRESULT OnSaveGroup(WPARAM wp, LPARAM lp);
	LRESULT OnDelGroup(WPARAM wp, LPARAM lp);
	LRESULT OnAddDelayTime(WPARAM wp, LPARAM lp);

	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnTvnInputDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnPopupMenuDelInputCircuit();
	afx_msg void OnPopupMenuDelOutputCircuit();
	afx_msg void OnPopupMenuMoveOutputUp();
	afx_msg void OnPopupMenuMoveOutputDown();

};


