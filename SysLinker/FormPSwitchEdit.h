#pragma once
#include "../Common/Control/SSplitter.h"
#include "../Common/XList/XListCtrl.h"
#include "afxcmn.h"


// CFormPSwitchEdit 폼 뷰입니다.
class CDlgChildPSwitch;
class CDlgTreePane;
class CDataPattern;
class CDataDevice;
class CDataPS;
class CDataLinked;
//class CFormPSwitchEdit : public CFormView
class CFormPSwitchEdit : public CView
{
	DECLARE_DYNCREATE(CFormPSwitchEdit)

protected:
	CFormPSwitchEdit();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormPSwitchEdit();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMPSWITCHEDIT };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	CSSplitter				m_SpMain; 
	CSSplitter				m_SpLeft; ///< 접점 리스트 , 중계기 리스트
	CSSplitter				m_SpRight;	///< 트리 , 정보

	CDlgChildPSwitch	*	m_pDlgLeftBottomPSwitchInfo;
	CDlgTreePane		*	m_pDlgLeftTopTreePane;
	CRelayTableData *		m_pRefFasSysData;
	CPtrList				m_ptrItemList;
	CDataPS				*	m_pCurrentData;
	CDataPS				*	m_pChangeData;
	CXListCtrl				m_ctrlPatternList;
	CXListCtrl				m_ctrlRelayList;

	CBrush					m_brBackground;
	COLORREF				m_crBack;
protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	int SetSplitterPos();
	
	CTreeCtrl m_ctrlTree;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


