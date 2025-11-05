#pragma once
#include "SplitAggregate.h"
#include "../Common/XList/XListCtrl.h"


// PROJECT 내의 PUMP TYPE : ETC 내의 파일 이름 
#define	FN_PUMPTYPE_LIST				L"pumptypelist.csv"
#define	FN_DEFAULT_PUMPTYPE_LIST		L"DefaultPumpTypeList.csv"
#define	FN_PUMPTEMPLETE_LIST			L"pumptemplete.csv"
#define	FN_DEFAULT_PUMPTEMPLETE_LIST	L"Defaultpumptemplete.csv"
// CFormPumpEdit 폼 뷰입니다.
class CDlgPumpEditPane;
class CDataPump;
class CPumpTypeList;
class CPumpItem;

class CFormPumpEdit : public CView
{
	DECLARE_DYNCREATE(CFormPumpEdit)

protected:
	CFormPumpEdit();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormPumpEdit();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMPUMPEDIT };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	SplitterCtrl			m_SpMain;
	SplitterCtrl			m_SpBtm;
	CXListCtrl				m_ctrlPatternList;
	CXListCtrl				m_ctrlRelayList;

	CDlgPumpEditPane	*	m_pDlgTopPumpEditPane;

	CRelayTableData		*	m_pRefFasSysData;
	CPtrList				m_ptrItemList;
// 	CDataPump			*	m_pCurrentData;
// 	CDataPump			*	m_pChangeData;

	CPumpItem			*	m_pCurrentPump;

	CBrush					m_brBackground;
	COLORREF				m_crBack;

	CPumpTypeList		*	m_pPtrTypeList;
	CPtrList		*	m_pPtrTemplete;
public:
	ST_PUMPTYPE * FindPumpType(CString strTypeName);
	int LoadPumpTypeList();
	int LoadPumpTemplete();
	int SavePumpTypeList();
	void RemoveAllPumpTypeList();
	void RemoveAllPumpTemplete();
	int InitSplitter();
	int SetSplitterPos();
protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);


	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp,LPARAM lp);

	int InitForm();
	void InitList();
	int InsertLink(CDataLinked * pLink);
	int DeleteLink(CDataLinked * pLink,YAdoDatabase * pDB);
	int DataAdd(CPumpItem * pItem);
	int DataSave(CPumpItem * pItem);
	int DataDelete(CPumpItem * pItem);
	int DataMove(CPumpItem * pItem,int nMoveVectorIdx);
	int DataDeleteFacp(int nFacpID);

	int DataPumpAdd(CPumpItem * pItem ,YAdoDatabase * pDB);
	int DataPumpSave(CPumpItem * pItem,YAdoDatabase * pDB);
	int DataPumpDelete(CPumpItem * pItem,YAdoDatabase * pDB);
	int DataPumpMove(CPumpItem * pItem,YAdoDatabase * pDB,int nMoveVectorIdx);
	int DataPumpDeleteFacp(int nFacpID,YAdoDatabase * pDB);

	int DataPsAdd(CPumpItem * pItem,YAdoDatabase * pDB);
	int DataPsSave(CPumpItem * pItem,YAdoDatabase * pDB);
	int DataPsDelete(CPumpItem * pItem,YAdoDatabase * pDB);
	int DataPsMove(CPumpItem * pItem,YAdoDatabase * pDB,int nMoveVectorIdx);
	int DataPsDeleteFacp(int nFacpID,YAdoDatabase * pDB);

	// Relay에서 펌프를 사용하는 경우 해당 펌프의 아이디 변경
	int DataMovePumpInRelay(CDataPump * pPmp,YAdoDatabase * pDB,int nMoveVectorIdx,CPtrList * pPtrInList);

	// 입력 회로에서 출력으로 사용되는 펌프 삭제
	int DataDeletePumpInRelay(CDataPump * pPmp,YAdoDatabase * pDB,CPtrList * pPtrInList);
	// 펌프에서 출력으로 사용하는 회로 삭제
	int DataDeleteLinkInPump(CDataPump * pPmp,YAdoDatabase * pDB);
	int DataLinkMove(CPumpItem * pItem,YAdoDatabase * pDB,int nMoveVectorIdx);
	int DataLinkDeleteFacp(int nFacpID,YAdoDatabase * pDB);
	int DataDeleteFacpInRelay(int nFacpID,YAdoDatabase * pDB,CPtrList * pPtrInList);

	int FindItemLink(int nType,int nFacp,int nUnit = 0,int nChn = 0,int nRelay = 0);
	afx_msg void OnSize(UINT nType,int cx,int cy);

	LRESULT OnChildPaneSelectChanged(WPARAM wp,LPARAM lp);
	LRESULT OnChildPaneCurrentDataChanged(WPARAM wp,LPARAM lp);
	LRESULT OnProjectSavePumpSave(WPARAM wp,LPARAM lp);

	afx_msg void OnTvnPatternDropedItem(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnTvnOutputDropedItem(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnTvnEmergencyDropedItem(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnTvnPumpDropedItem(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnTvnPSwitchDropedItem(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnTvnContactDropedItem(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd,CPoint point);
	afx_msg void OnPtnlkMenuDelptn();
	afx_msg void OnPtnlkMenuViewptn();
	afx_msg void OnRlylkMenuDelrly();
	afx_msg void OnRlylkMenuViewrelay();
	afx_msg void OnRlylkMenuMoverelay();
};


