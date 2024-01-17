#pragma once

#include "SplitAggregate.h"
#include "../Common/XList/XListCtrl.h"

// CFormPump 폼 뷰입니다.
class CDlgChildPump;
class CDlgTreePane;
class CDataPump;
class CRelayTableData;
class CDataPattern;
class CDataDevice;
class CDataLinked;
class CFormPump : public CView
{
	DECLARE_DYNCREATE(CFormPump)

protected:
	CFormPump();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormPump();
	CImageList			m_ImageTree;
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMPUMP };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	//CSplitAggregate		m_wndSplitter;
	SplitterCtrl m_SpMain;
	SplitterCtrl m_SpLeft;
	SplitterCtrl m_SpRight;
	CDlgTreePane		*	m_pDlgLeftTopTreePane;
	CDlgChildPump		*	m_pDlgRightTopPumpInfo;
	CXListCtrl				m_ctrlPatternList;
	CXListCtrl				m_ctrlRelayList;
	CRelayTableData		*	m_pRefFasSysData;
	CPtrList				m_ptrItemList;
	CDataPump			*	m_pCurrentData;
	CDataPump			*	m_pChangeData;
	CBrush					m_brBackground;
	COLORREF				m_crBack;
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	int InitSplitter();
	int SetSplitterPos();
	void RemoveAllData();
	//int FillPumpTree();
	LRESULT OnChildPaneTreeSelChanged(WPARAM wp, LPARAM lp);
	LRESULT OnChildPaneSelDataChanged(WPARAM wp, LPARAM lp);

	afx_msg void OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult);

	int InsertLink(CDataLinked * pLink);
	int DeleteLink(CDataLinked * pLink , YAdoDatabase * pDB);
	int InitForm();
	void InitList();
	int DataAdd(CDataPump * pData);
	int DataSave(CDataPump * pData);
	int DataDelete(CDataPump * pData);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPtnlkMenuDelptn();
	afx_msg void OnPtnlkMenuViewptn();
	afx_msg void OnRlylkMenuDelrly();
	afx_msg void OnRlylkMenuViewrelay();
	int FindItemLink(int nType, int nFacp, int nUnit = 0, int nChn = 0, int nRelay = 0);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


