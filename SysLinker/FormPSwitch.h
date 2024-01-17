#pragma once

#include "../Common/Control/SplitterCtrl.h"
#include "../Common/XList/XListCtrl.h"


// CFormPSwitch �� ���Դϴ�.
class CDlgChildPSwitch;
class CDlgTreePane;
class CDataPattern;
class CDataDevice;
class CDataPS;
class CDataLinked;	
class CFormPSwitch : public CView
{
	DECLARE_DYNCREATE(CFormPSwitch)

protected:
	CFormPSwitch();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormPSwitch();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMPSWITCH };
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
	//CTreeCtrl				m_ctrlLeftTopPSwitch;
	CDlgChildPSwitch	*	m_pDlgLeftBottomPSwitchInfo;
	CDlgTreePane		*	m_pDlgLeftTopTreePane;
	CXListCtrl				m_ctrlRightTopPatternList;
	CXListCtrl				m_ctrlRightBottomRelayList;
	CRelayTableData *		m_pRefFasSysData;
	CPtrList				m_ptrItemList;
	CDataPS				*	m_pCurrentData;
	CDataPS				*	m_pChangeData;

	CBrush					m_brBackground;
	COLORREF				m_crBack;
protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	int InitSplitter();
	int SetSplitterPos();
	void RemoveAllData();
	LRESULT OnChildPaneTreeSelChanged(WPARAM wp, LPARAM lp);
	LRESULT OnChildPaneSelDataChanged(WPARAM wp, LPARAM lp);

	afx_msg void OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult);

	int InsertLink(CDataLinked * pLink);
	int DeleteLink(CDataLinked * pLink, YAdoDatabase * pDB);
	int InitForm();
	void InitList();
	int DataAdd(CDataPS * pData);
	int DataSave(CDataPS * pData);
	int DataDelete(CDataPS * pData);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPtnlkMenuDelptn();
	afx_msg void OnPtnlkMenuViewptn();
	afx_msg void OnRlylkMenuDelrly();
	afx_msg void OnRlylkMenuViewrelay();
	int FindItemLink(int nType, int nFacp, int nUnit = 0, int nChn = 0, int nRelay = 0);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


