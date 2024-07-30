// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// SysLinkerView.h : CSysLinkerView 클래스의 인터페이스
//

#pragma once
#include "../Common/Control/SplitterCtrl.h"
#include "../Common/Control/ColorStatic.h"
#include "../Common/XList/XListCtrl.h"
#include "afxwin.h"
#include "afxcmn.h"

#define D_MAX_DATA_PAGE		5

class CRelayTableData;
class CDataDevice;
class CDataSystem;
/////////////////////////////////////////////////////////////////////////////
// 
class SplitterCtrlCustom1 : virtual public SplitterCtrlBase, public SplitterCtrlDraw
{
protected:	// SplitterCtrlDraw.
	virtual void DrawSplitter(CDC *pDC, bool horz, int idx, CRect const *pRect);
	virtual void DrawDragRect(CDC *pDC, bool horz, bool firstTime, CRect const *pRectOld, CRect const *pRectNew);
	virtual void DrawBorder(CDC *pDC, CRect const *pRect);
};
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// 
class SplitterCtrlCustom2 : virtual public SplitterCtrlBase, public ISplitterCtrlRecalc, public SplitterCtrlDraw
{
protected:	// ISplitterCtrlRecalc.
	virtual int GetSplitterWidth(ISplitterCtrlRecalc *base) const;
	virtual int GetSplitterHeight(ISplitterCtrlRecalc *base) const;
	virtual int GetBorderWidth(ISplitterCtrlRecalc *base) const;

protected:	// SplitterCtrlDraw.
	virtual void DrawSplitter(CDC *pDC, bool horz, int idx, CRect const *pRect);
	virtual void DrawDragRect(CDC *pDC, bool horz, bool firstTime, CRect const *pRectOld, CRect const *pRectNew);
	virtual void DrawBorder(CDC *pDC, CRect const *pRect);
};
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// 
class SplitterCtrlAggregate :
	public SplitterCtrl, public SplitterCtrlCustom1, public SplitterCtrlCustom2
{
};

class CDlgInfoBasePage;
class CDataLinked;
class CSysLinkerView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CSysLinkerView();
	DECLARE_DYNCREATE(CSysLinkerView)

public:
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum{ IDD = IDD_SYSLINKER_FORM };
#else
	enum{ IDD = IDD_SYSLINKER_FORM_EN };
#endif
#endif

// 특성입니다.
public:
	CSysLinkerDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현입니다.
public:
	virtual ~CSysLinkerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	SplitterCtrlAggregate m_wndSplitter;
	CColorStatic m_stDevName;

	CXListCtrl m_ctrlPatternList;
	CXListCtrl m_ctrlRelayList;
	CRelayTableData		*	m_pRefFasSysData;
	CDataDevice			*	m_pDevice;

	int						m_nCurType;// 유닛,수신기,계통,회로
									   // 유닛,수신기,계통 체크 했을때 해당 회로를 타입으로 설정
	CDlgInfoBasePage	*	m_pPage[D_MAX_DATA_PAGE];
	ST_TREEITEM			*	m_pRefCurData; //< 현재 선택된 아이템
	ST_TREEITEM			*	m_pNewData; 
	int						m_nAction; //< DATA_ADD , DATA_SAVE,DATA_DEL
	int						m_nAddType;// SE_NONE , SE_FACP .. 
	CPtrList				m_ptrDeleteRelationLowerItems; ///< 삭제 시 관계되는 하위 아이템들 , 
																///< 메모리에서 삭제하기 위해 이곳에 저장한다.
	
	CPtrList				m_ptrDeleteRelationOutputLinkItems; ///< 삭제 시 관계되는 연동데이터
	
	CPtrList				m_ptrDeleteRelationPatternItems; ///< 삭제 시 관계되는 연동데이터
	CPtrList				m_ptrDeleteRelationPumpItems; ///< 삭제 시 관계되는 연동데이터
	CPtrList				m_ptrDeleteRelationPSwitchItems; ///< 삭제 시 관계되는 연동데이터

	vector<CString>			m_vtDeleteRelationUnit;
	vector<int>				m_vtDeleteRelationFacp;
	CPtrList			*	m_pRefPtrSelectedItems;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int InitSplitter();
	int SetSplitterPos();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// Docking View Drop Event Process
	afx_msg void OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult);

	int DisplayLinkData(CDataDevice* pDev);
	void SetSelectTreeItem(CPtrList * pPtrSelectItems, ST_TREEITEM* pItem);
	afx_msg void OnPtnlkMenuDelptn();
	afx_msg void OnUpdatePtnlkMenuDelptn(CCmdUI *pCmdUI);
	afx_msg void OnPtnlkMenuViewptn();
	afx_msg void OnUpdatePtnlkMenuViewptn(CCmdUI *pCmdUI);
	afx_msg void OnRlylkMenuDelrly();
	afx_msg void OnUpdateRlylkMenuDelrly(CCmdUI *pCmdUI);
	afx_msg void OnRlylkMenuViewrelay();
	afx_msg void OnUpdateRlylkMenuViewrelay(CCmdUI *pCmdUI);
	afx_msg void OnRlylkMenuMoverelay();
	afx_msg void OnUpdateRlylkMenuMoverelay(CCmdUI *pCmdUI);
	afx_msg void OnRClickRelayList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRClickPatternList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkPatternList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkRelayList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp , LPARAM lp);

	afx_msg void OnClose();

	void SetRelayTableData(CRelayTableData * pData) { m_pRefFasSysData = pData; }
	int FindItemLink(int nType, int nFacp, int nUnit = 0, int nChn= 0, int nRelay = 0);
	void ChangeRelayEditNumber();
	CStatic m_stFrame;
	CButton m_btnAdd;
	CButton m_btnSave;
	CButton m_btnDel;
	afx_msg void OnBnClickedBtnAdd();
	//afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnAddFacp();
	afx_msg void OnAddRelay();
	afx_msg void OnAddUnit();
	afx_msg void OnAddChn();
	void ShowChildPage(int nType);
	int CreateAllPage();
	void CopyTreeItem(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData);
	BOOL ChangeTreeItem(ST_TREEITEM * pSourceData, ST_TREEITEM * pTargetData);
	void DeleteTreeItem(ST_TREEITEM * pNewData);
	void SetNewTreeItem(int nType, ST_TREEITEM * pNewData);
	void AddInit(int nAddType);
	void AddCancel();
	int DataAdd();
	int DataSave();
	int DataDelete();

	CDataSystem * DataAddFacp(YAdoDatabase * pDB);
	CDataSystem * DataAddUnit(YAdoDatabase * pDB);
	CDataSystem * DataAddChn(YAdoDatabase * pDB);
	CDataSystem * DataAddRelay(YAdoDatabase * pDB);
	int DataSaveFacp(YAdoDatabase * pDB);
	int DataSaveChn(YAdoDatabase * pDB);
	int DataSaveUnit(YAdoDatabase * pDB);
	int DataSaveRelay(YAdoDatabase * pDB);

	int MakeDeleteItem(YAdoDatabase * pDB);
	int DeleteMemoryRelationLowerItems();
	afx_msg void OnBnClickedBtnSave2();
	CDataLinked*  InsertMultiInputLink(BOOL bFirst
		, int nTgtFacp, int nTgtUnit, int nTgtChn, int nTgtDev
		, byte btLinkType, byte btLogicType, int nLogicID
	);
	int ChangeMultiInputIndivisualLinkOrder(CDataDevice* pDev);
};

#ifndef _DEBUG  // SysLinkerView.cpp의 디버그 버전
inline CSysLinkerDoc* CSysLinkerView::GetDocument() const
   { return reinterpret_cast<CSysLinkerDoc*>(m_pDocument); }
#endif

