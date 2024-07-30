// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// SysLinkerView.h : CSysLinkerView Ŭ������ �������̽�
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
protected: // serialization������ ��������ϴ�.
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

// Ư���Դϴ�.
public:
	CSysLinkerDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// �����Դϴ�.
public:
	virtual ~CSysLinkerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
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

	int						m_nCurType;// ����,���ű�,����,ȸ��
									   // ����,���ű�,���� üũ ������ �ش� ȸ�θ� Ÿ������ ����
	CDlgInfoBasePage	*	m_pPage[D_MAX_DATA_PAGE];
	ST_TREEITEM			*	m_pRefCurData; //< ���� ���õ� ������
	ST_TREEITEM			*	m_pNewData; 
	int						m_nAction; //< DATA_ADD , DATA_SAVE,DATA_DEL
	int						m_nAddType;// SE_NONE , SE_FACP .. 
	CPtrList				m_ptrDeleteRelationLowerItems; ///< ���� �� ����Ǵ� ���� �����۵� , 
																///< �޸𸮿��� �����ϱ� ���� �̰��� �����Ѵ�.
	
	CPtrList				m_ptrDeleteRelationOutputLinkItems; ///< ���� �� ����Ǵ� ����������
	
	CPtrList				m_ptrDeleteRelationPatternItems; ///< ���� �� ����Ǵ� ����������
	CPtrList				m_ptrDeleteRelationPumpItems; ///< ���� �� ����Ǵ� ����������
	CPtrList				m_ptrDeleteRelationPSwitchItems; ///< ���� �� ����Ǵ� ����������

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

#ifndef _DEBUG  // SysLinkerView.cpp�� ����� ����
inline CSysLinkerDoc* CSysLinkerView::GetDocument() const
   { return reinterpret_cast<CSysLinkerDoc*>(m_pDocument); }
#endif

