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

// SysLinkerView.cpp : CSysLinkerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "SysLinker.h"
#endif

#include "SysLinkerDoc.h"
#include "SysLinkerView.h"
#include "../Common/Control/DTreeCtrl.h"

#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataEmBc.h"
#include "DataPump.h"
#include "DataPS.h"
#include "DataDevice.h"
#include "DataSystem.h"
#include "DataLocation.h"
#include "DataPattern.h"
#include "DataEquip.h"
#include "RelayTableData.h"
#include "DataLinked.h"
#include "MainFrm.h"

#include "DlgInfoFacp.h"
#include "DlgInfoChn.h"
#include "DlgInfoUnit.h"
#include "DlgInfoRelay.h"
#include "DlgInfoEmpty.h"

#include "DlgViewRelay.h"
#include "DataFacpRelay.h"

#include "../Common/Utils/YAdoDatabase.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// SplitterCtrlCustom1.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// 
void DrawGradient(CDC *pDC, CRect const *rc, bool horz, COLORREF clrTop, COLORREF clrBottom)
{
	GRADIENT_RECT gRect = { 0,1 };
	TRIVERTEX vert[2] =
	{ { rc->left,rc->top,(COLOR16)(GetRValue(clrTop) << 8),(COLOR16)(GetGValue(clrTop) << 8),(COLOR16)(GetBValue(clrTop) << 8),0 },
	{ rc->right,rc->bottom,(COLOR16)(GetRValue(clrBottom) << 8),(COLOR16)(GetGValue(clrBottom) << 8),(COLOR16)(GetBValue(clrBottom) << 8),0 }
	};
	::GradientFill(pDC->m_hDC, vert, 2, &gRect, 1, (horz == true ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V));
}
// 
void SplitterCtrlCustom1::DrawSplitter(CDC *pDC, bool horz, int /*idx*/, CRect const *pRect)
{
	DrawGradient(pDC, pRect, horz == false, RGB(245, 245, 245), RGB(160, 165, 170));
}
/////////////////////////////////////////////////////////////////////////////
//
void SplitterCtrlCustom1::DrawDragRect(CDC *pDC, bool /*horz*/, bool firstTime, CRect const *pRectOld, CRect const *pRectNew)
{
	pDC->DrawDragRect(pRectNew, CSize(pRectNew->Width(), pRectNew->Height()),
		(firstTime == true ? NULL : pRectOld), CSize(pRectOld->Width(), pRectOld->Height()));
}
/////////////////////////////////////////////////////////////////////////////
// 
void SplitterCtrlCustom1::DrawBorder(CDC *pDC, CRect const *pRect)
{
	pDC->Draw3dRect(pRect, RGB(77, 115, 61), RGB(77, 115, 61));
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// SplitterCtrlCustom2.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// 
int SplitterCtrlCustom2::GetBorderWidth(ISplitterCtrlRecalc * /*base*/) const
{
	return 2;
}
// 
int SplitterCtrlCustom2::GetSplitterWidth(ISplitterCtrlRecalc *base) const
{
	return base->GetSplitterWidth(NULL);
}
// 
int SplitterCtrlCustom2::GetSplitterHeight(ISplitterCtrlRecalc *base) const
{
	return base->GetSplitterHeight(NULL);
}
/////////////////////////////////////////////////////////////////////////////
// 
void SplitterCtrlCustom2::DrawSplitter(CDC *pDC, bool /*horz*/, int /*idx*/, CRect const *pRect)
{
	pDC->DrawEdge((RECT *)pRect, EDGE_RAISED, BF_RECT);
}
/////////////////////////////////////////////////////////////////////////////
//
void SplitterCtrlCustom2::DrawDragRect(CDC *pDC, bool horz, bool firstTime, CRect const *pRectOld, CRect const *pRectNew)
{
	CRect rcOld = *pRectOld, rcNew = *pRectNew;
	if (horz == true)
	{
		rcOld.DeflateRect(0, 1);
		rcNew.DeflateRect(0, 1);
	}
	else
	{
		rcOld.DeflateRect(1, 0);
		rcNew.DeflateRect(1, 0);
	}
	CBrush brush(HS_DIAGCROSS, RGB(0, 0, 0));
	pDC->DrawDragRect(&rcNew, CSize(rcNew.Width(), rcNew.Height()),
		(firstTime == true ? NULL : &rcOld), CSize(rcOld.Width(), rcOld.Height()),
		&brush, &brush);
}
/////////////////////////////////////////////////////////////////////////////
// 
void SplitterCtrlCustom2::DrawBorder(CDC *pDC, CRect const *pRect)
{
	pDC->DrawEdge((RECT *)pRect, EDGE_SUNKEN, BF_RECT);
}

// CSysLinkerView

IMPLEMENT_DYNCREATE(CSysLinkerView, CFormView)

BEGIN_MESSAGE_MAP(CSysLinkerView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_TVN_DROPED_ITEM(IDC_PATTERNVIEW_TREE, OnTvnPatternDropedItem)
	ON_TVN_DROPED_ITEM(IDC_EMERGENCYVIEW_TREE, OnTvnEmergencyDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PUMPVIEW_TREE, OnTvnPumpDropedItem)
	ON_TVN_DROPED_ITEM(IDC_CONTACTFACP_TREE, OnTvnContactDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PSWITCHVIEW_TREE, OnTvnPSwitchDropedItem)
	ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE, OnTvnOutputDropedItem)
	ON_COMMAND(ID_PTNLK_MENU_DELPTN, &CSysLinkerView::OnPtnlkMenuDelptn)
	ON_COMMAND(ID_PTNLK_MENU_VIEWPTN, &CSysLinkerView::OnPtnlkMenuViewptn)
	ON_COMMAND(ID_RLYLK_MENU_DELRLY, &CSysLinkerView::OnRlylkMenuDelrly)
	ON_COMMAND(ID_RLYLK_MENU_VIEWRELAY, &CSysLinkerView::OnRlylkMenuViewrelay)
	ON_COMMAND(ID_RLYLK_MENU_MOVERELAY, &CSysLinkerView::OnRlylkMenuMoverelay)
	ON_UPDATE_COMMAND_UI(ID_PTNLK_MENU_DELPTN, &CSysLinkerView::OnUpdatePtnlkMenuDelptn)
	ON_UPDATE_COMMAND_UI(ID_PTNLK_MENU_VIEWPTN, &CSysLinkerView::OnUpdatePtnlkMenuViewptn)
	ON_UPDATE_COMMAND_UI(ID_RLYLK_MENU_DELRLY, &CSysLinkerView::OnUpdateRlylkMenuDelrly)
	ON_UPDATE_COMMAND_UI(ID_RLYLK_MENU_VIEWRELAY, &CSysLinkerView::OnUpdateRlylkMenuViewrelay)
	ON_UPDATE_COMMAND_UI(ID_RLYLK_MENU_MOVERELAY, &CSysLinkerView::OnUpdateRlylkMenuMoverelay)
	ON_NOTIFY(NM_RCLICK, IDC_RELAY_LIST, OnRClickRelayList)
	ON_NOTIFY(NM_RCLICK, IDC_PATTERN_LIST, OnRClickPatternList)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT , OnBCMAllDataInit)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_PATTERN_LIST, OnNMDblclkPatternList)
	ON_NOTIFY(NM_DBLCLK, IDC_RELAY_LIST, OnNMDblclkRelayList)
	ON_BN_CLICKED(IDC_BTN_ADD, &CSysLinkerView::OnBnClickedBtnAdd)
	ON_COMMAND(IDC_ADD_FACP, &CSysLinkerView::OnAddFacp)
	ON_COMMAND(IDC_ADD_RELAY, &CSysLinkerView::OnAddRelay)
	ON_COMMAND(IDC_ADD_UNIT, &CSysLinkerView::OnAddUnit)
	ON_COMMAND(IDC_ADD_CHN, &CSysLinkerView::OnAddChn)
	//ON_BN_CLICKED(IDC_BTN_SAVE, &CSysLinkerView::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CSysLinkerView::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_SAVE2, &CSysLinkerView::OnBnClickedBtnSave2)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CSysLinkerView::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, &CSysLinkerView::OnBnClickedButtonPaste)
END_MESSAGE_MAP()

// CSysLinkerView 생성/소멸

#ifndef ENGLISH_MODE
CSysLinkerView::CSysLinkerView()
	: CFormView(IDD_SYSLINKER_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pRefFasSysData = nullptr;
	m_pDevice = nullptr;
	for (int i = 0; i < D_MAX_DATA_PAGE; i++)
		m_pPage[i] = nullptr;

	m_pRefCurData = nullptr;
	m_pNewData = nullptr;
	m_pNewData = new ST_TREEITEM;
	m_pNewData->pData = new CDataSystem;
	m_nAction = DATA_SAVE;

	m_nAddType = -1;
	m_pRefPtrSelectedItems = nullptr;

	m_pDlgManualCopy = nullptr;			//20250617 GBM - 수동 연동데이터 일괄 편집 기능
}
#else
CSysLinkerView::CSysLinkerView()
	: CFormView(IDD_SYSLINKER_FORM_EN)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pRefFasSysData = nullptr;
	m_pDevice = nullptr;
	for (int i = 0; i < D_MAX_DATA_PAGE; i++)
		m_pPage[i] = nullptr;

	m_pRefCurData = nullptr;
	m_pNewData = nullptr;
	m_pNewData = new ST_TREEITEM;
	m_pNewData->pData = new CDataSystem;
	m_nAction = DATA_SAVE;

	m_nAddType = -1;
	m_pRefPtrSelectedItems = nullptr;
}
#endif

CSysLinkerView::~CSysLinkerView()
{
	for (int i = 0; i < D_MAX_DATA_PAGE; i++)
	{
		if (m_pPage[i] == nullptr)
			continue;
		delete m_pPage[i];
		m_pPage[i] = nullptr;
	}

	if (m_pNewData != nullptr)
	{
		if (m_pNewData->pData)
		{
			delete (CDataSystem*)m_pNewData->pData;
			m_pNewData->pData = nullptr;
		}
		delete m_pNewData;
		m_pNewData = nullptr;
	}
}

void CSysLinkerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_DEVICE, m_stDevName);
	DDX_Control(pDX, IDC_ST_FRAME, m_stFrame);
	DDX_Control(pDX, IDC_BTN_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BTN_SAVE2, m_btnSave);
	DDX_Control(pDX, IDC_BTN_DEL, m_btnDel);
}

BOOL CSysLinkerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CSysLinkerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
#ifndef ENGLISH_MODE
	GetParent()->SetWindowTextW(_T("연동데이터 편집"));
#else
	GetParent()->SetWindowTextW(_T("EDIT SITE LOGIC DATA"));
#endif
	GetParent()->SendMessage(UWM_CHILDFRAME_CLOSEENABLE , 0 , 0);
#ifndef ENGLISH_MODE
	AfxGetMainWnd()->SetWindowText(L"연동데이터 편집");
#else
	AfxGetMainWnd()->SetWindowText(L"EDIT SITE LOGIC DATA");
#endif

	ResizeParentToFit();
	InitSplitter();

	m_stDevName.SetFont(L"맑은 고딕");
	m_stDevName.SetHeight(18);
#ifndef ENGLISH_MODE
	m_stDevName.SetText(L"현재 디바이스");
#else
	m_stDevName.SetText(L"CURRENT DEVICE");
#endif
	m_stDevName.SetAlignment(DT_CENTER | DT_SINGLELINE);

#ifndef ENGLISH_MODE
	m_ctrlPatternList.InsertColumn(0, L"제어 접점" ,LVCFMT_LEFT, 150);
	m_ctrlPatternList.InsertColumn(1, L"접점 종류", LVCFMT_LEFT, 150);
#else
	m_ctrlPatternList.InsertColumn(0, L"CONTROL RELAY CONTACT", LVCFMT_LEFT, 150);
	m_ctrlPatternList.InsertColumn(1, L"RELAY CONTACT TYPE", LVCFMT_LEFT, 150);
#endif

	m_ctrlPatternList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

#ifndef ENGLISH_MODE
	m_ctrlRelayList.InsertColumn(0, L"순서", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(1, L"출력이름", LVCFMT_LEFT, 300);
	m_ctrlRelayList.InsertColumn(2, L"입력타입", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(3, L"출력타입", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(4, L"설비명", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(5, L"설비번호", LVCFMT_LEFT, 50);
	m_ctrlRelayList.InsertColumn(6, L"출력설명", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(7, L"위치", LVCFMT_LEFT, 150);
	m_ctrlRelayList.InsertColumn(8, L"연동타입", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(9, L"출력종류", LVCFMT_LEFT, 80);
#else
	m_ctrlRelayList.InsertColumn(0, L"ORDER", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(1, L"OUTPUT NAME", LVCFMT_LEFT, 300);
	m_ctrlRelayList.InsertColumn(2, L"INPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(3, L"OUTPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(4, L"EQUIPMENT NAME", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(5, L"EQUIPMENT NO.", LVCFMT_LEFT, 50);
	m_ctrlRelayList.InsertColumn(6, L"OUTPUT DESCRIPTION", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(7, L"LOCATION", LVCFMT_LEFT, 150);
	m_ctrlRelayList.InsertColumn(8, L"INTERLOCK TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(9, L"OUTPUT CATEGORY", LVCFMT_LEFT, 80);
#endif

	m_ctrlRelayList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CreateAllPage();


	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRelayList);

		m_pRefFasSysData =theApp.GetRelayTableData();
	}
}

void CSysLinkerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSysLinkerView::OnContextMenu(CWnd*  pWnd , CPoint point)
{
// #ifndef SHARED_HANDLERS
// 	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
// #endif
// 
	CMenu menu ,* pContext;
	UINT nID;
	UINT nFlag = 0;
	int nIdx = -1;
	CPoint ptHit;
	CPoint pt = point;
	ptHit = pt;
	CWnd * pHitWnd = WindowFromPoint(pt);
	//ScreenToClient(&ptHit);
	if (point == CPoint(-1, -1))
		return;

	if (!(pHitWnd == (CWnd*)&m_ctrlPatternList || pHitWnd == (CWnd*)&m_ctrlRelayList))
		return;
	if (pHitWnd == (CWnd*)&m_ctrlPatternList)
	{
#ifndef ENGLISH_MODE
		nID = IDR_POPUP_PTNLINK;
#else
		nID = IDR_POPUP_PTNLINK_EN;
#endif
		m_ctrlPatternList.ScreenToClient(&ptHit);
		nIdx = m_ctrlPatternList.HitTest(ptHit,nullptr);
	}
	else
	{
#ifndef ENGLISH_MODE
		nID = IDR_POPUP_RELAYLINK;
#else
		nID = IDR_POPUP_RELAYLINK_EN;
#endif
		m_ctrlRelayList.ScreenToClient(&ptHit);
		nIdx = m_ctrlRelayList.HitTest(ptHit, nullptr);
	}
	if (nIdx < 0)
		return;
	menu.LoadMenu(nID);
	pContext = menu.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
}


// CSysLinkerView 진단

#ifdef _DEBUG
void CSysLinkerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSysLinkerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSysLinkerDoc* CSysLinkerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSysLinkerDoc)));
	return (CSysLinkerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSysLinkerView 메시지 처리기

int CSysLinkerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (!m_wndSplitter.Create(this, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), IDC_SP_MAIN))
		return -1;

	if (!m_ctrlPatternList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 800, 100), &m_wndSplitter, IDC_PATTERN_LIST))
		return -1;
	// 	if (!m_ctrlHaAgentList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 0, 0), &m_SpTop, IDC_LIST_HAAGENT))
	// 		return -1;

	if (!m_ctrlRelayList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 0, 0), &m_wndSplitter, IDC_RELAY_LIST))
		return  -1;
	return 0;
}

int CSysLinkerView::InitSplitter()
{
	SetSplitterPos();
	m_wndSplitter.AddRow();
	m_wndSplitter.AddRow();
	m_wndSplitter.AddColumn();
	m_wndSplitter.SetWindow(0, 0, m_ctrlPatternList.m_hWnd);
	m_wndSplitter.SetWindow(1, 0, m_ctrlRelayList.m_hWnd);

	m_wndSplitter.SetRecalcManager(NULL);
	m_wndSplitter.SetDrawManager((SplitterCtrlCustom1 *)&m_wndSplitter);
	m_wndSplitter.SetResizeMode(SPLITTERCTRL_RESIZE_DYNAMIC);
	//m_wndSplitter.SetDraggingMode(SPLITTERCTRL_DRAGGING_DYNAMIC);
	m_wndSplitter.Update();
	return 0;
}


int CSysLinkerView::SetSplitterPos()
{
	if (m_wndSplitter.GetSafeHwnd() == nullptr)
		return 0;

	if (m_stDevName.GetSafeHwnd() == nullptr)
		return 0;

	CRect rc , rcDev , rcAdd,rcSave , rcDel,rcFrame,rcChild;
	GetClientRect(&rc);
	rcDev = rc;
	rcDev.bottom = 22;
	rcDev.top += 2;
	rcDev.left += 5;
	rcDev.right -= 5;
	m_stDevName.MoveWindow(&rcDev);
	
	rcFrame = rcDev;
	rcFrame.top = rc.top + 24;
	rcFrame.bottom = rcFrame.top + 120;
	m_stFrame.MoveWindow(&rcFrame);
	rcChild = rcFrame;
	rcChild.top = 0;
	rcChild.bottom = rcFrame.Height();
	rcChild.left = 0;
	rcChild.right = rcFrame.Width();

	rcChild.DeflateRect(2, 2, 3, 3);
	for (int i = 0; i < D_MAX_DATA_PAGE; i++)
	{
		if(m_pPage[i] && m_pPage[i]->GetSafeHwnd() != nullptr)
			m_pPage[i]->MoveWindow(&rcChild);
	}
	rcAdd = rcFrame;
	rcAdd.top = rc.top + 146;
	rcAdd.bottom = rcAdd.top + 20;
	rcAdd.left = rcDev.left;
	rcAdd.right = rcAdd.left + 60;
	m_btnAdd.MoveWindow(&rcAdd);

	rcSave = rcAdd;
	//rcSave.top = rc.top + 126;
	rcSave.left = rcAdd.right + 2;
	rcSave.right = rcSave.left + 60;
	m_btnSave.MoveWindow(&rcSave);

	rcDel = rcSave;
	rcDel.left = rcSave.right + 2;
	rcDel.right = rcDel.left + 60;
	m_btnDel.MoveWindow(&rcDel);


	rc.top = rc.top + 170;
	//rc.top = rc.top + 32;
	rc.DeflateRect(5, 0, 5, 5);
	m_wndSplitter.MoveWindow(&rc);
	CRect rcbtn;
	rcbtn.left = rc.right - 80;
	rcbtn.right = rcbtn.left + 80;
	rcbtn.top = rc.bottom + 2;
	rcbtn.bottom = rcbtn.top + 30;
	if(GetDlgItem(IDC_BTN_SAVE)->GetSafeHwnd())
		GetDlgItem(IDC_BTN_SAVE)->MoveWindow(&rcbtn);


	m_wndSplitter.Update();
// 	if (m_ctrlRelayList.GetSafeHwnd())
// 		m_ctrlRelayList.R();
// 
// 	if (m_ctrlPatternList.GetSafeHwnd())
// 		m_ctrlPatternList.RedrawWindow();
	return 0;
}


void CSysLinkerView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetSplitterPos();

}



void CSysLinkerView::OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;
	if (m_pRefFasSysData == nullptr)
		return; 
	BOOL bMulti = FALSE;
	if (m_pRefPtrSelectedItems == nullptr)
	{
		if (m_pDevice == nullptr)
			return;
		bMulti = FALSE;
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 다중 선택
		bMulti = TRUE;
	}

	m_ctrlRelayList.SetRedraw(FALSE);
	COLORREF crText = RGB(255,255,255),crBk = RGB(255,0,0);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataSystem * pSys;
	CDataDevice * pDev; 
	CDataLocBase * pLoc;
	CDataEquip *pEq; 
	CDataLinked	* pLink;
	CString str;
	int nInType, nOutType, nEqName, nCont;
	BOOL bAddFirst = FALSE;
 	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0 , nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		nInType = nOutType = nEqName = nCont = -1;
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue; 

		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue; 
		if (pData->nDataType != TTYPE_DEV_DEVICE)
			continue; 

		pSys = (CDataSystem *)pData->pData;
		if (pSys == nullptr || pSys->GetSysData() == nullptr)
			continue;

		pDev = (CDataDevice *)pSys->GetSysData();
		if (pDev == nullptr)
			continue; 

		nFind = FindItemLink(LK_TYPE_RELEAY, pDev->GetFacpID()
			, pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID());

		if (nFind >= 0)
			continue; 
	
		pLoc = pDev->GetDevInputLocation();
		nIdx = m_ctrlRelayList.GetItemCount();

		pEq = pDev->GetEqInput();
		if (pEq)
			nInType = (int)pEq->GetEquipID();
		pEq = pDev->GetEqName();
		if (pEq)
			nEqName = (int)pEq->GetEquipID();
		pEq = pDev->GetEqOutput();
		if (pEq)
			nOutType = (int)pEq->GetEquipID();
		pEq = pDev->GetEqOutContents();
		if (pEq)
			nCont = (int)pEq->GetEquipID();
		// [KHS 2020-5-18 11:25:25] 
		// 교차 감지기 일때
		// 출력 타입 - 프리액션 , 출력내용 - 밸브 일때 
		if (nOutType == OUTTYPE_PREACTION && nCont == OUTCONT_VALVE)
			bAddFirst = TRUE;
		else
			bAddFirst = FALSE;
		nIdx = m_ctrlRelayList.GetItemCount();
		m_ctrlRelayList.InsertItem(nIdx,L"",crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 1, pDev->GetOutputFullName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 2, pDev->GetInputTypeName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 3, pDev->GetOutputTypeName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 4, pDev->GetEquipName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 5, pDev->GetEqAddIndex(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 6, pDev->GetOutContentsName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 7, pDev->GetOutputLocationName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 8, g_szLogicTypeString[LOGIC_MANUAL],crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 9, L"",crText,crBk);

		if (bMulti)
		{
			pLink = InsertMultiInputLink(bAddFirst
				, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
				, LK_TYPE_RELEAY, LOGIC_MANUAL, 0
			);
		}
		else
		{
			pLink = new CDataLinked;
			pLink->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
				, LK_TYPE_RELEAY, LOGIC_MANUAL, 0
				, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
			);
			m_pRefFasSysData->InsertLinkedTable(m_pDevice, pLink);
			m_pDevice->AddLink(bAddFirst, pLink);
		}
		m_ctrlRelayList.SetItemData(nIdx, (DWORD_PTR)pLink);
	}
	ChangeRelayEditNumber();
	m_ctrlRelayList.SetRedraw();

}

void CSysLinkerView::OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	BOOL bMulti = FALSE;
	if (m_pRefPtrSelectedItems == nullptr)
	{
		if (m_pDevice == nullptr)
			return;
		bMulti = FALSE;
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 다중 선택
		bMulti = TRUE;
	}

	m_ctrlPatternList.SetRedraw(FALSE);
	COLORREF crText = RGB(255,255,255),crBk = RGB(255,0,0);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataPattern * pPtn; 
	CDataLinked	* pLink;
	CString str;
	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;
		
		if (pData->nDataType != TTYPE_DEV_PATTERN)
			continue; 

		pPtn = (CDataPattern *)pData->pData;
		if (pPtn == nullptr)
			continue;

		nFind = FindItemLink(LK_TYPE_PATTERN, (int)pPtn->GetPatternID());

		if(nFind >= 0)
		{
			// Yellow
// 			m_ctrlPatternList.InsertItem(0,pPtn->GetPatternName(),crText,crBk);
// 			m_ctrlPatternList.SetItemText(0,1,g_szPatternTypeString[pPtn->GetPatternType()],crText,crBk);
// 			m_ctrlPatternList.SetItemData(0,(DWORD_PTR)0);
//  
			continue;
		}

		m_ctrlPatternList.InsertItem(0, pPtn->GetPatternName(),crText,crBk);
		m_ctrlPatternList.SetItemText(0,1,g_szPatternTypeString[pPtn->GetPatternType()],crText,crBk);


		if (bMulti)
		{
			pLink = InsertMultiInputLink(FALSE
				, (int)pPtn->GetPatternID(), 0, 0, 0
				, LK_TYPE_PATTERN, LOGIC_MANUAL, 0
			);
		}
		else
		{
			pLink = new CDataLinked;
			pLink->SetLinkData((int)pPtn->GetPatternID(), 0, 0, 0
				, LK_TYPE_PATTERN, LOGIC_MANUAL, 0
				, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
			);
			m_pRefFasSysData->InsertLinkedTable(m_pDevice, pLink);
			m_pDevice->AddLink(FALSE, pLink);
		}
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);

		//m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}

void CSysLinkerView::OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	BOOL bMulti = FALSE;
	if (m_pRefPtrSelectedItems == nullptr)
	{
		if (m_pDevice == nullptr)
			return;
		bMulti = FALSE;
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 다중 선택
		bMulti = TRUE;
	}

	m_ctrlPatternList.SetRedraw(FALSE);
	COLORREF crText = RGB(255,255,255),crBk = RGB(255,0,0);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataEmBc * pItem;
	CDataLinked	* pLink;
	CString str, strType, strName;
	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;

		if (pData->nDataType != TTYPE_DEV_EMERGENCY)
			continue; 
		pItem = (CDataEmBc *)pData->pData;
		if (pItem == nullptr)
			continue;

		nFind = FindItemLink(LK_TYPE_EMERGENCY, pItem->GetEmID());

		if (nFind >= 0)
			continue;
		
		if (pItem->GetEmAddr() == L"")
			strName.Format(L"%s(A%d)", pItem->GetEmName(), pItem->GetEmID());
		else
			strName = pItem->GetEmName() + '(' + pItem->GetEmAddr() + ')';
		strType = g_szLinkTypeString[LK_TYPE_EMERGENCY];
#ifndef ENGLISH_MODE
		strType += L"- 이름(주소)";
#else
		strType += L"- NAME (PUBLIC ADDRESS)";
#endif

		m_ctrlPatternList.InsertItem(0,strName,crText,crBk);
		m_ctrlPatternList.SetItemText(0, 1, strType,crText,crBk);

		if (bMulti)
		{
			pLink = InsertMultiInputLink(FALSE
				, pItem->GetEmID(), 0, 0, 0
				, LK_TYPE_EMERGENCY, LOGIC_MANUAL, 0
			);
		}
		else
		{
			pLink = new CDataLinked;
			pLink->SetLinkData(pItem->GetEmID(), 0, 0, 0
				, LK_TYPE_EMERGENCY, LOGIC_MANUAL, 0
				, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
			);
			m_pRefFasSysData->InsertLinkedTable(m_pDevice, pLink);
			m_pDevice->AddLink(FALSE, pLink);
		}
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}

void CSysLinkerView::OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	BOOL bMulti = FALSE;
	if (m_pRefPtrSelectedItems == nullptr)
	{
		if (m_pDevice == nullptr)
			return;
		bMulti = FALSE;
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 다중 선택
		bMulti = TRUE;
	}

	m_ctrlPatternList.SetRedraw(FALSE);
	COLORREF crText = RGB(255,255,255),crBk = RGB(255,0,0);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataPump * pPm;
	CDataLinked	* pLink;
	CString str;
	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;
		if (pData->nDataType != TTYPE_DEV_PUMP)
			continue;

		pPm = (CDataPump *)pData->pData;
		if (pPm == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_PUMP, pPm->GetFacpID() , pPm->GetPumpID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0,pPm->GetPumpName(),crText,crBk);

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_PUMP],crText,crBk);

		if (bMulti)
		{
			pLink = InsertMultiInputLink(FALSE
				, pPm->GetFacpID(), pPm->GetPumpID(), 0, 0
				, LK_TYPE_PUMP, LOGIC_MANUAL, 0
			);
		}
		else
		{
			pLink = new CDataLinked;
			pLink->SetLinkData(pPm->GetFacpID(), pPm->GetPumpID(), 0, 0
				, LK_TYPE_PUMP, LOGIC_MANUAL, 0
				, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
			);
			m_pRefFasSysData->InsertLinkedTable(m_pDevice, pLink);
			m_pDevice->AddLink(FALSE, pLink);
		}
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}

void CSysLinkerView::OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	//COLORREF crText = RGB(255,255,255),crBk = RGB(255,0,0);

	BOOL bMulti = FALSE;
	if (m_pRefPtrSelectedItems == nullptr)
	{
		if (m_pDevice == nullptr)
			return;
		bMulti = FALSE;
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 다중 선택
		bMulti = TRUE;
	}

	m_ctrlPatternList.SetRedraw(FALSE);
	COLORREF crText = RGB(255,255,255),crBk = RGB(255,0,0);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataFacpRelay * pCont;
	CDataLinked	* pLink;
	CString str;
	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;
		if (pData->nDataType != TTYPE_DEV_CONTACTFACP)
			continue;

		pCont = (CDataFacpRelay *)pData->pData;
		if (pCont == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_FACP_RELAY, pCont->GetFacpID(), pCont->GetRelayID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0,pCont->GetFRelayName(),crText,crBk);

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_FACP_RELAY],crText,crBk);

		if (bMulti)
		{
			pLink = InsertMultiInputLink(FALSE
				, pCont->GetFacpID(), pCont->GetRelayID(), 0, 0
				, LK_TYPE_FACP_RELAY, LOGIC_MANUAL, 0
			);
		}
		else
		{
			pLink = new CDataLinked;
			pLink->SetLinkData(pCont->GetFacpID(), pCont->GetRelayID(), 0, 0
				, LK_TYPE_FACP_RELAY, LOGIC_MANUAL, 0
				, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
			);
			m_pRefFasSysData->InsertLinkedTable(m_pDevice, pLink);
			m_pDevice->AddLink(FALSE, pLink);
		}
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}


void CSysLinkerView::OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	//20250618 GBM start - PS는 출력으로 잡지 않음
#ifndef ENGLISH_MODE
	AfxMessageBox(_T("압력 스위치는 추가할 수 없습니다."));
#else
	AfxMessageBox(_T("A pressure switch cannot be added."));
#endif
	return;
	//20250618 GBM end

	*pResult = 0;
#if _USE_PSWITCH_
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	BOOL bMulti = FALSE;
	if (m_pRefPtrSelectedItems == nullptr)
	{
		if (m_pDevice == nullptr)
			return;
		bMulti = FALSE;
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 다중 선택
		bMulti = TRUE;
	}

	m_ctrlPatternList.SetRedraw(FALSE);
	COLORREF crText = RGB(255,255,255),crBk = RGB(255,0,0);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataPS * pPs;
	CDataLinked	* pLink;
	CString str;
	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;

		if (pData->nDataType != TTYPE_DEV_PSWITCH)
			continue;
		pPs = (CDataPS *)pData->pData;
		if (pPs == nullptr)
			continue;

		nFind = FindItemLink(LK_TYPE_G_FLAG, pPs->GetFacpID() , pPs->GetPSwitchID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0,pPs->GetPSwitchName(),crText,crBk);

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_G_FLAG],crText,crBk);

		if (bMulti)
		{
			pLink = InsertMultiInputLink(FALSE
				, pPs->GetFacpID(), pPs->GetPSwitchID(), 0, 0
				, LK_TYPE_G_FLAG, LOGIC_MANUAL, 0
			);
		}
		else
		{
			pLink = new CDataLinked;
			pLink->SetLinkData(pPs->GetFacpID(), pPs->GetPSwitchID(), 0, 0
				, LK_TYPE_G_FLAG, LOGIC_MANUAL, 0
				, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
			);
			m_pRefFasSysData->InsertLinkedTable(m_pDevice, pLink);
			m_pDevice->AddLink(FALSE, pLink);
		}
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

#endif
}

int CSysLinkerView::DisplayLinkData(CDataDevice* pDev)
{
	COLORREF crBk,crText;
	m_ctrlPatternList.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
	m_pDevice = pDev;
	if (pDev == nullptr)
	{
		m_stDevName.SetText(L"");
		return 0;
	}
	m_ctrlPatternList.SetRedraw(FALSE);
	m_ctrlRelayList.SetRedraw(FALSE);

	m_ctrlPatternList.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
	if (m_pRefFasSysData == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		m_ctrlPatternList.SetRedraw();
		return 0;
	}
	m_pDevice = pDev;

	m_stDevName.SetText(m_pDevice->GetInputFullName());

	CPtrList * pLList = nullptr;
	POSITION pos;
	CDataDevice * pData;
	CDataLinked * pLink;
	CDataPattern * pPtn;
	CDataFacpRelay * pContact;
	CDataPump * pPmp;
	CDataEmBc * pEm;

	CString strDeleteText;
	CString strKey;
	CString strName ,strType; 
	int nPIdx = 0, nRIdx = 0;
	pLList = m_pDevice->GetLinkedList();
	pos = pLList->GetHeadPosition();
	while (pos)
	{
		pLink = (CDataLinked *)pLList->GetNext(pos);
		if (pLink == nullptr)
			continue;
		if(pLink->GetLogicType() == LOGIC_MANUAL)
		{
			crBk = RGB(255,0,0);
			crText = RGB(255,255,255);
		}
		else
		{
			crBk = RGB(255,255,255);
			crText = RGB(0,0,0);
		}
		if (pLink->GetLinkType() == LK_TYPE_RELEAY)
		{
			pData = m_pRefFasSysData->GetDeviceByID(pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());
			if(pData == nullptr)
			{
				crBk = RGB(255,255,0);
				crText = RGB(0,0,0);
#ifndef ENGLISH_MODE
				strDeleteText.Format(L"%s회로" , TXT_DELETE_ITEM);
#else
				strDeleteText.Format(L"%sCIRCUIT", TXT_DELETE_ITEM);
#endif
				m_ctrlRelayList.InsertItem(nRIdx,strDeleteText,crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,1,L" - ",crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,2,L" - ",crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,3,L" - ",crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,4,L" - ",crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,5,L" - ",crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,6,L" - ",crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,7,L" - ",crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,8,g_szLogicTypeString[pLink->GetLogicType()],crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,9,L" - ",crText,crBk);
				m_ctrlRelayList.SetItemData(nRIdx,(DWORD_PTR)pLink);
				nRIdx++;
			}
			else
			{
				m_ctrlRelayList.InsertItem(nRIdx,pData->GetOutputFullName(),crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,1,pData->GetOutputFullName(),crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,2,pData->GetInputTypeName(),crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,3,pData->GetOutputTypeName(),crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,4,pData->GetEquipName(),crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,5,pData->GetEqAddIndex(),crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,6,pData->GetOutContentsName(),crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,7,pData->GetOutputLocationName(),crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,8,g_szLogicTypeString[pLink->GetLogicType()],crText,crBk);
				m_ctrlRelayList.SetItemText(nRIdx,9,L"",crText,crBk);
				m_ctrlRelayList.SetItemData(nRIdx,(DWORD_PTR)pLink);
				nRIdx++;
			}
			
		}
		else 
		{
			strName = L"";
			strType = L"";
			switch (pLink->GetLinkType())
			{
			case LK_TYPE_PATTERN:
				pPtn = m_pRefFasSysData->GetPattern(pLink->GetTgtFacp());
				if(pPtn == nullptr)
				{
					crBk = RGB(255,255,0);
					crText = RGB(0,0,0);
#ifndef ENGLISH_MODE
					strDeleteText.Format(L"%s패턴",TXT_DELETE_ITEM);
#else
					strDeleteText.Format(L"%sPATTERN", TXT_DELETE_ITEM);
#endif
					strName = strDeleteText;// pPtn->GetPatternName();
#ifndef ENGLISH_MODE
					strType = L"패턴종류 없음";//g_szPatternTypeString[pPtn->GetPatternType()];
#else
					strType = L"NO PATTERN TYPE";//g_szPatternTypeString[pPtn->GetPatternType()];
#endif
					//continue;
				}
				else
				{
					strName = pPtn->GetPatternName();
					strType = g_szPatternTypeString[pPtn->GetPatternType()];
				}
				
				break;
			case LK_TYPE_EMERGENCY:
				pEm = m_pRefFasSysData->GetEmergency(pLink->GetTgtFacp());
				if(pEm == nullptr)
				{
					crBk = RGB(255,255,0);
					crText = RGB(0,0,0);
#ifndef ENGLISH_MODE
					strDeleteText.Format(L"%s비상방송",TXT_DELETE_ITEM);
#else
					strDeleteText.Format(L"%sPUBLIC_ADDRESS", TXT_DELETE_ITEM);
#endif
					strName = strDeleteText;// pPtn->GetPatternName();
				}
				else
				{
					if(pEm->GetEmAddr() == L"")
						strName.Format(L"%s(A%d)",pEm->GetEmName(),pEm->GetEmID());
					else
						strName = pEm->GetEmName() + '(' + pEm->GetEmAddr() + ')';

				}
				strType = g_szLinkTypeString[LK_TYPE_EMERGENCY];
#ifndef ENGLISH_MODE
				strType += L"- 이름(주소)";
#else
				strType += L"- NAME (PUBLIC ADDRESS)";
#endif
				break;
			case LK_TYPE_PUMP:
				pPmp = m_pRefFasSysData->GetPump(pLink->GetTgtFacp(),pLink->GetTgtUnit());
				if(pPmp == nullptr)
				{
					crBk = RGB(255,255,0);
					crText = RGB(0,0,0);
#ifndef ENGLISH_MODE
					strDeleteText.Format(L"%s펌프",TXT_DELETE_ITEM);
#else
					strDeleteText.Format(L"%sPUMP", TXT_DELETE_ITEM);
#endif
					strName = strDeleteText;// pPtn->GetPatternName();
				}
				else
				{
					strName = pPmp->GetPumpName();
				}
				strType = g_szLinkTypeString[LK_TYPE_PUMP];
				break;
			case LK_TYPE_G_FLAG:
				break;
			case LK_TYPE_BELL:
				break;
			case LK_TYPE_XA_FLAG:
				break;
			case LK_TYPE_XB_FLAG:
				break;
			case LK_TYPE_FACP_RELAY:
				pContact = m_pRefFasSysData->GetContactFacp(pLink->GetTgtFacp(), pLink->GetTgtUnit());
				if (pContact == nullptr)
				{
					crBk = RGB(255,255,0);
					crText = RGB(0,0,0);
#ifndef ENGLISH_MODE
					strDeleteText.Format(L"%s수신기접점",TXT_DELETE_ITEM);
#else
					strDeleteText.Format(L"%sFACP_RELAY", TXT_DELETE_ITEM);
#endif
					strName = strDeleteText;// pPtn->GetPatternName();
				}
				else
				{
					strName = pContact->GetFRelayName();
				}
				strType = g_szLinkTypeString[pLink->GetLinkType()];
				break;
			}
			
			m_ctrlPatternList.InsertItem(nPIdx,strName,crText,crBk);
			m_ctrlPatternList.SetItemText(nPIdx, 1, strType,crText,crBk);
			m_ctrlPatternList.SetItemData(nPIdx, (DWORD_PTR)pLink);
			nPIdx++;
		}
	}
	ChangeRelayEditNumber();
	m_ctrlRelayList.SetRedraw();
	m_ctrlPatternList.SetRedraw();

	return 1;
}


void CSysLinkerView::SetSelectTreeItem(CPtrList * pPtrSelectItems, ST_TREEITEM* pItem)
{
	AddCancel();
	m_pRefPtrSelectedItems = pPtrSelectItems;
	if (pItem == nullptr)
	{
		m_pRefCurData = nullptr;
		ShowChildPage(-1); 
		for (int i = 0; i < D_MAX_DATA_PAGE; i++)
		{
			if (m_pPage[i] == nullptr)
				continue;
			m_pPage[i]->DisplayItem(m_pRefCurData, m_pNewData);
		}
		return;
	}

	if (m_pRefPtrSelectedItems !=  nullptr)
	{
		ShowChildPage(-1);
		DisplayLinkData(nullptr);
		return;
	}
	m_nAction = DATA_SAVE;

	
	if (pItem->nDataType >= D_MAX_DATA_PAGE)
	{
		ShowChildPage(-1);
		DisplayLinkData(nullptr);
		m_pRefCurData = nullptr;
		return;
	}
	m_pRefCurData = pItem;
	CopyTreeItem(m_pRefCurData,m_pNewData);
	m_pPage[m_pRefCurData->nDataType]->DisplayItem(m_pRefCurData, m_pNewData);
	m_pPage[m_pRefCurData->nDataType]->SetAddFlag(FALSE);
	ShowChildPage(pItem->nDataType);
	if (pItem->nDataType != TTYPE_DEV_DEVICE)
	{
		DisplayLinkData(nullptr);
	}
	else
	{
		CDataSystem * pData;
		pData = (CDataSystem*)pItem->pData;
		DisplayLinkData((CDataDevice*)pData->GetSysData());
	}
}


void CSysLinkerView::OnPtnlkMenuDelptn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nIdx , nRet=-1;
	BOOL bDeletedAllLink = FALSE;
	nIdx = m_ctrlPatternList.GetNextItem(-1, LVNI_SELECTED);
	if (nIdx < 0)
		return; 

#ifndef ENGLISH_MODE
	if (AfxMessageBox(L"접점정보를 삭제하시겠습니까?", MB_YESNO) == IDNO)
		return;
#else
	if (AfxMessageBox(L"Do you want to delete the relay contact information?", MB_YESNO) == IDNO)
		return;
#endif

	str = m_ctrlPatternList.GetItemText(nIdx,0);
	if(str.Find(TXT_DELETE_ITEM) >= 0)
	{
#ifndef ENGLISH_MODE
		int nRet = AfxMessageBox(L"정보가 삭제된 접점 입니다.\n"
			L"해당 접점을 사용하는 모든 입력에서 삭제 - (예)\n"
			L"현재 입력에서만 삭제 - (아니오)\n"
			L"삭제 작업을 취소 - (취소)\n"
			,MB_ICONQUESTION | MB_YESNOCANCEL
		);
#else
		int nRet = AfxMessageBox(L"The information of the relay contact has been deleted.\n"
			L"To delete from all inputs that use the relay contact - (YES)\n"
			L"To delete only from the current input - (NO)\n"
			L"To cancel the delete operation - (CANCEL)\n"
			, MB_ICONQUESTION | MB_YESNOCANCEL
		);
#endif
		if(nRet == IDCANCEL)
			return;
		else if(nRet == IDYES)
			bDeletedAllLink = TRUE;
	}

	
	CString strSql;
	CDataLinked * pLink;
	pLink = (CDataLinked*)m_ctrlPatternList.GetItemData(nIdx);
	if (pLink == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 접점에 대한 정보를 가져오는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete. Failed to retrieve information about the relay contact.");
#endif
		return; 
	}

	if (m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
#else
		AfxMessageBox(L"Failed to delete. Invalid project settings information.");
#endif
		return;
	}

	if(bDeletedAllLink)
	{
		nRet = m_pRefFasSysData->DeleteLinkFromAllInput(pLink);
		if(nRet == 0)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
			return;
		}
	}
	else
	{
		nRet = m_pRefFasSysData->DeleteLinkInfo(m_pDevice,pLink);
		if(nRet == 0)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
			return;
		}

	}
	m_ctrlPatternList.DeleteItem(nIdx);
#ifndef ENGLISH_MODE
	AfxMessageBox(L"연동된 접점 정보를 삭제하는데 성공했습니다.");
#else
	AfxMessageBox(L"연동된 접점 정보를 삭제하는데 성공했습니다.");
#endif
}


void CSysLinkerView::OnUpdatePtnlkMenuDelptn(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	
}


void CSysLinkerView::OnPtnlkMenuViewptn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nIdx, nRet = -1;
	FormViewStyle iView = FV_COUNT;
	int nItemType = TTYPE_DEV_PRJNAME;
	nIdx = m_ctrlPatternList.GetNextItem(-1, LVNI_SELECTED);
	if (nIdx < 0)
		return;

	CString strSql;
	CDataLinked * pLink;
	pLink = (CDataLinked*)m_ctrlPatternList.GetItemData(nIdx);
	if (pLink == nullptr)
	{
		return;
	}
	switch (pLink->GetLinkType())
	{
	case LK_TYPE_RELEAY:
		iView = FV_MAKELINKER;
		nItemType = TTYPE_DEV_LINKED_TARGETITEM;
		break;
	case LK_TYPE_PATTERN:
		iView = FV_MAKEPATTERN;
		nItemType = TTYPE_DEV_LINKED_TARGETITEM;
		break;
	case LK_TYPE_PUMP:
		iView = FV_PUMP;
		nItemType = TTYPE_DEV_PUMP;
		break;
	case LK_TYPE_EMERGENCY:
		iView = FV_EMERGENCY;
		nItemType = TTYPE_DEV_EMERGENCY;
		break;
	default:
		return;
	}
	theApp.ViewFormSelectItem(iView, nItemType, (DWORD_PTR)pLink);
}


void CSysLinkerView::OnUpdatePtnlkMenuViewptn(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CSysLinkerView::OnRlylkMenuDelrly()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nRet = 0 , nCnt ,i;
	nCnt = m_ctrlRelayList.GetSelectedCount();
	if (nCnt < 0)
		return; 

#ifndef ENGLISH_MODE
	if (AfxMessageBox(L"연동 출력을 삭제하시겠습니까?", MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;
#else
	if (AfxMessageBox(L"Do you want to delete the interlock output?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
#endif

	if (m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
#else
		AfxMessageBox(L"Failed to delete. Invalid project settings information. ");
#endif
		return;
	}

	BOOL bError = FALSE;
	CString strSql , strError= L"";
	CDataLinked * pLink;
	POSITION pos;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	pos = m_ctrlRelayList.GetFirstSelectedItemPosition();
	std::vector<int> vtSel;


	while (pos)
	{
		int nSelected = m_ctrlRelayList.GetNextSelectedItem(pos);
		vtSel.push_back(nSelected);
	}
	sort(vtSel.begin(), vtSel.end());

	pDB->BeginTransaction();

	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		pLink = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		if (pLink == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
#ifndef ENGLISH_MODE
			strError = L"삭제하는데 실패했습니다. 연동 출력에 대한 정보를 가져오는데 실패했습니다.";
#else
			strError = L"Failed to delete. Failed to retrieve information about the interlock output.";
#endif
			bError = TRUE;
			break;;
		}

		strSql.Format(L"SELECT * FROM TB_LINK_RELAY "
			L"WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
			L"AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
			, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
			, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn() , pLink->GetTgtDev()
		);

		if (pDB->OpenQuery(strSql) == FALSE)
		{
#ifndef ENGLISH_MODE
			strError = L"삭제하는데 실패했습니다. 데이터베이스에서 연동정보를 가져오는데 실패했습니다.";
#else
			strError = L"Failed to delete. Failed to retrieve the interlock information from the database.";
#endif
			bError = TRUE;
			break;;
		}

		nCnt = pDB->GetRecordCount();
		if (nCnt == 0)
			continue; 

		strSql.Format(L"DELETE TB_LINK_RELAY WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
			L" AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
			, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
			, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev()
		);

		if (pDB->ExecuteSql(strSql) == FALSE)
		{
#ifndef ENGLISH_MODE
			strError = L"삭제하는데 실패했습니다. 데이터베이스에서 연동정보를 삭제하는데 실패했습니다.";
#else
			strError = L"Failed to delete. Failed to delete the interlocked information from the database.";
#endif
			bError = TRUE;
			break;;
		}

		//m_pDevice->DeleteLink(pLink);
		//m_ctrlRelayList.DeleteItem(vtSel[i]);
	}

	if (bError == TRUE)
	{
		pDB->RollbackTransaction();
		AfxMessageBox(strError);
		return; 
	}

	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		pLink = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		if (pLink == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
#ifndef ENGLISH_MODE
			strError = L"삭제하는데 실패했습니다. 연동 출력에 대한 정보를 가져오는데 실패했습니다.";
#else
			strError = L"Failed to delete. Failed to retrieve information about the interlock output.";
#endif
			bError = TRUE;
			break;;
		}
		m_pDevice->DeleteLink(pLink);
		m_ctrlRelayList.DeleteItem(vtSel[i]);
	}
	pDB->CommitTransaction();
	ChangeRelayEditNumber();
#ifndef ENGLISH_MODE
	AfxMessageBox(L"연동 출력 정보를 삭제하는데 성공했습니다.");
#else
	AfxMessageBox(L"Successfully deleted the interlock output information.");
#endif
// 	
// 
// 
// 	if (AfxMessageBox(L"연동 출력을 삭제하시겠습니까?", MB_YESNO) == IDNO)
// 		return;
// 
// 	CString strSql;
// 	CDataLinked * pLink;
// 	pLink = (CDataLinked*)m_ctrlRelayList.GetItemData(nIdx);
// 	if (pLink == nullptr)
// 	{
// 		AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
// 		return;
// 	}
// 
// 	if (m_pRefFasSysData == nullptr)
// 	{
// 		AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
// 		return;
// 	}
// 
// 	nRet = m_pRefFasSysData->DeleteLinkInfo(m_pDevice,pLink);
// 	if (nRet == 0)
// 	{
// 		//AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
// 		return;
// 	}
// 	m_ctrlRelayList.DeleteItem(nIdx);
//	AfxMessageBox(L"연동 출력 정보를 삭제하는데 성공했습니다");
}


void CSysLinkerView::OnUpdateRlylkMenuDelrly(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CSysLinkerView::OnRlylkMenuViewrelay()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	CPoint ptCur;
// 	::GetCursorPos(&ptCur);
// 
// 	int nIdx = -1;
// 
// 	m_ctrlRelayList.ScreenToClient(&ptCur);
// 	nIdx = m_ctrlRelayList.HitTest(ptCur);
// 	if (nIdx < 0)
// 		return; 

	int nIdx;
	nIdx = m_ctrlRelayList.GetNextItem(-1, LVNI_SELECTED);
	if (nIdx < 0)
		return;
	CDataLinked * pLnk = (CDataLinked *)m_ctrlRelayList.GetItemData(nIdx);
	if (pLnk == nullptr)
		return; 
	CDataDevice * pDev; 
	pDev = m_pRefFasSysData->GetDeviceByID(pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
		, pLnk->GetTgtChn(), pLnk->GetTgtDev());
	if (pDev == nullptr)
		return; 
	CDlgViewRelay dlg(pDev);
	dlg.DoModal();
}


void CSysLinkerView::OnUpdateRlylkMenuViewrelay(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CSysLinkerView::OnRlylkMenuMoverelay()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int nIdx;
	nIdx = m_ctrlRelayList.GetNextItem(-1, LVNI_SELECTED);
	if (nIdx < 0)
		return;
	CDataLinked * pLnk = (CDataLinked *)m_ctrlRelayList.GetItemData(nIdx);
	if (pLnk == nullptr)
		return;

	theApp.ViewFormSelectItem(FV_MAKELINKER, TTYPE_DEV_LINKED_TARGETITEM, (DWORD_PTR)pLnk);
}


void CSysLinkerView::OnUpdateRlylkMenuMoverelay(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CSysLinkerView::OnRClickRelayList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	UNREFERENCED_PARAMETER(pNMItemActivate);

	CPoint ptCur;
	::GetCursorPos(&ptCur);

	int nIdx  = -1;

	m_ctrlRelayList.ScreenToClient(&ptCur);
	nIdx = m_ctrlRelayList.HitTest(ptCur);

	::GetCursorPos(&ptCur);
	CMenu MenuTemp;
	CMenu *pContextMenu = NULL;
#ifndef ENGLISH_MODE
	MenuTemp.LoadMenu(IDR_POPUP_RELAYLINK);
#else
	MenuTemp.LoadMenu(IDR_POPUP_RELAYLINK_EN);
#endif
	pContextMenu = MenuTemp.GetSubMenu(0);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN, ptCur.x, ptCur.y, ::AfxGetMainWnd());

	*pResult = 0;
}


void CSysLinkerView::OnRClickPatternList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	UNREFERENCED_PARAMETER(pNMItemActivate);

	CPoint ptCur;
	::GetCursorPos(&ptCur);

	int nIdx = -1;

	m_ctrlPatternList.ScreenToClient(&ptCur);
	nIdx = m_ctrlPatternList.HitTest(ptCur);

	
	::GetCursorPos(&ptCur);
	CMenu MenuTemp;
	CMenu *pContextMenu = NULL;
#ifndef ENGLISH_MODE
	MenuTemp.LoadMenu(IDR_POPUP_PTNLINK);
#else
	MenuTemp.LoadMenu(IDR_POPUP_PTNLINK_EN);
#endif
	pContextMenu = MenuTemp.GetSubMenu(0);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN, ptCur.x, ptCur.y, ::AfxGetMainWnd());
	*pResult = 0;
}

void CSysLinkerView::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRelayList);

		m_pRefFasSysData = nullptr;
	}
	CFormView::OnClose();
}


LRESULT CSysLinkerView::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	if (wp == FORM_PRJ_CLOSE )
	{
		if (m_ctrlRelayList.GetSafeHwnd())
			m_ctrlRelayList.DeleteAllItems();
		if (m_ctrlPatternList)
			m_ctrlPatternList.DeleteAllItems();
		DisplayLinkData(nullptr);
	}
	else if (wp == FORM_PRJ_NEW || wp == FORM_PRJ_OPEN)
	{
		m_pRefFasSysData = theApp.GetRelayTableData();
		for (int i = 0; i < D_MAX_DATA_PAGE; i++)
		{
			if (m_pPage[i] && m_pPage[i]->GetSafeHwnd() != nullptr)
				m_pPage[i]->PrjDataInit((int)wp);
		}
	}
	return 0; 
}
	

void CSysLinkerView::OnNMDblclkRelayList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSysLinkerView::OnNMDblclkPatternList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


int CSysLinkerView::FindItemLink(int nType , int nFacp , int nUnit , int nChn , int nRelay)
{
	int i, nCnt; 
	CDataLinked * pLink = nullptr;
	CXListCtrl * pList = nullptr; 
	if (LK_TYPE_RELEAY == nType)
	{
		pList = &m_ctrlRelayList;
	}
	else
	{
		pList = &m_ctrlPatternList;
	}

	nCnt = pList->GetItemCount();
	if (nCnt <= 0)
		return -1;

	for (i = 0; i < nCnt; i++)
	{
		pLink = (CDataLinked *)pList->GetItemData(i);
		if (pLink == nullptr)
			continue; 

		if (pLink->GetLinkType() != nType)
			continue; 

		if (pLink->GetTgtFacp() != nFacp)
			continue;
		if (nUnit > 0 && pLink->GetTgtUnit() != nUnit)
			continue;

		if (nChn > 0 && pLink->GetTgtChn() != nChn)
			continue;

		if (nRelay > 0 && pLink->GetTgtDev() != nRelay)
			continue;

		return i;
	}

	return -1;
}


void CSysLinkerView::ChangeRelayEditNumber()
{
	POSITION pos;
	CPtrList * pList;
	CDataLinked * pLnk;
	int nIdx = 0, nItem = -1;
	CString str;
	if (m_pRefPtrSelectedItems == nullptr)
	{
		pList = m_pDevice->GetLinkedList();
		if (pList == nullptr)
			return;
		pos = pList->GetHeadPosition();
		while (pos)
		{
			pLnk = (CDataLinked*)pList->GetNext(pos);
			if (pLnk == nullptr)
				continue;
			if (pLnk->GetLinkType() != LK_TYPE_RELEAY)
				continue;
			nItem = FindItemLink(pLnk->GetLinkType(), pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
				, pLnk->GetTgtChn(), pLnk->GetTgtDev());
			if (nItem < 0)
				continue;
			str.Format(L"%03d", nIdx + 1);
			m_ctrlRelayList.SetItemText(nItem, 0, str);
			nIdx++;
		}
	}
	else
	{
// 		POSITION pos;
// 		CDataDevice * pDev;
// 		CDataSystem * pSys;
// 		ST_TREEITEM * pItem;
// 		
// 		pos = m_pRefPtrSelectedItems->GetHeadPosition();
// 		while (pos)
// 		{
// 			pItem = (ST_TREEITEM *)m_pRefPtrSelectedItems->GetNext(pos);
// 			if (pItem == nullptr || pItem->pData == nullptr)
// 				continue;
// 			if (pItem->nDataType != TTYPE_DEV_DEVICE)
// 				continue;
// 			pSys = (CDataSystem*)pItem->pData;
// 			if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
// 				continue;
// 			ChangeMultiInputIndivisualLinkOrder((CDataDevice*)pSys->GetSysData());
// 		}
	}
	
}


void CSysLinkerView::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CRect rc;
	CMenu mn, *pContext;
	GetDlgItem(IDC_BTN_ADD)->GetWindowRect(&rc);
#ifndef ENGLISH_MODE
	mn.LoadMenu(IDR_ADD_MENU);
#else
	mn.LoadMenu(IDR_ADD_MENU_EN);
#endif
	pContext = mn.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, rc.left, rc.bottom, this);
}

// 
// void CSysLinkerView::OnBnClickedBtnSave()
// {
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	// 	int i = 0, nCnt;
// 	// 	nCnt = m_ctrlPatternList.GetItemCount();
// 	// 	
// 	
// }

void CSysLinkerView::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nRet = 0; 
	if (m_pRefCurData == nullptr)
	 	return;
	m_nAction = DATA_DEL;
	nRet = DataDelete();

	if (nRet <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete.");
#endif
	}
	else
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 성공했습니다.");
#else
		AfxMessageBox(L"Successfully deleted.");
#endif
	}
	AddCancel();
}


void CSysLinkerView::OnAddFacp()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	AddInit(TTYPE_DEV_FACP);
	ShowChildPage(m_nAddType);
	if (m_pNewData->nDataType == SE_FACP)
	{
		DeleteTreeItem(m_pNewData);
		CopyTreeItem(m_pRefCurData, m_pNewData);
	}
	else
	{
		DeleteTreeItem(m_pNewData);
		SetNewTreeItem(SE_FACP, m_pNewData);
	}
// 	DeleteTreeItem(m_pNewData);
// 	SetNewTreeItem(SE_FACP, m_pNewData);
	m_pPage[m_nAddType]->DisplayItem(nullptr, m_pNewData);
	m_pPage[m_nAddType]->SetAddFlag(TRUE);
	DisplayLinkData(nullptr);

}

void CSysLinkerView::OnAddUnit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	AddInit(TTYPE_DEV_UNIT);
	ShowChildPage(m_nAddType);
	if (m_pNewData->nDataType == SE_UNIT)
	{
		DeleteTreeItem(m_pNewData);
		CopyTreeItem(m_pRefCurData, m_pNewData);
		m_pNewData->hItem = nullptr;
		m_pNewData->hParent = nullptr;
	}
	else
	{
		DeleteTreeItem(m_pNewData);
		SetNewTreeItem(SE_UNIT, m_pNewData);
	}
//  	DeleteTreeItem(m_pNewData);
//  	SetNewTreeItem(SE_UNIT, m_pNewData);
 	m_pPage[m_nAddType]->DisplayItem(nullptr, m_pNewData);
 	m_pPage[m_nAddType]->SetAddFlag(TRUE);
	DisplayLinkData(nullptr);
}

void CSysLinkerView::OnAddChn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	AddInit(TTYPE_DEV_CHANNEL);
	ShowChildPage(m_nAddType);
	//m_bPreviewComplete = TRUE;
	if (m_pNewData->nDataType == SE_CHANNEL)
	{
		DeleteTreeItem(m_pNewData);
		CopyTreeItem(m_pRefCurData, m_pNewData);
		m_pNewData->hItem = nullptr;
		m_pNewData->hParent = nullptr;
	}
	else
	{
		DeleteTreeItem(m_pNewData);
		SetNewTreeItem(SE_CHANNEL, m_pNewData);
	}
	
	m_pPage[m_nAddType]->DisplayItem(nullptr, m_pNewData);
	m_pPage[m_nAddType]->SetAddFlag(TRUE);
	DisplayLinkData(nullptr);
}

void CSysLinkerView::OnAddRelay()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	AddInit(TTYPE_DEV_DEVICE);
	ShowChildPage(m_nAddType);
	//m_bPreviewComplete = TRUE;

	if (m_pNewData->nDataType == SE_RELAY)
	{
		DeleteTreeItem(m_pNewData);
		CopyTreeItem(m_pRefCurData, m_pNewData);
		m_pNewData->hItem = nullptr;
		m_pNewData->hParent = nullptr;
	}
	else
	{
		DeleteTreeItem(m_pNewData);
		SetNewTreeItem(SE_RELAY, m_pNewData);
	}
	m_pPage[m_nAddType]->DisplayItem(nullptr, m_pNewData);
	m_pPage[m_nAddType]->SetAddFlag(TRUE);
	DisplayLinkData(nullptr);

}

int CSysLinkerView::CreateAllPage()
{
	CRect rc;
	m_stFrame.GetClientRect(&rc);
	m_stFrame.ClientToScreen(&rc);
	ScreenToClient(&rc);

	if (m_pPage[TTYPE_DEV_PRJNAME] == nullptr)
		m_pPage[TTYPE_DEV_PRJNAME] = new CDlgInfoEmpty;
	m_pPage[TTYPE_DEV_PRJNAME]->CreateDlg(&m_stFrame);

	if (m_pPage[TTYPE_DEV_FACP] == nullptr)
		m_pPage[TTYPE_DEV_FACP] = new CDlgInfoFacp;
	m_pPage[TTYPE_DEV_FACP]->CreateDlg(&m_stFrame);

	if (m_pPage[TTYPE_DEV_UNIT] == nullptr)
		m_pPage[TTYPE_DEV_UNIT] = new CDlgInfoUnit;
	m_pPage[TTYPE_DEV_UNIT]->CreateDlg(&m_stFrame);

	if (m_pPage[TTYPE_DEV_CHANNEL] == nullptr)
		m_pPage[TTYPE_DEV_CHANNEL] = new CDlgInfoChn;
	m_pPage[TTYPE_DEV_CHANNEL]->CreateDlg(&m_stFrame);

	if (m_pPage[TTYPE_DEV_DEVICE] == nullptr)
		m_pPage[TTYPE_DEV_DEVICE] = new CDlgInfoRelay;
	m_pPage[TTYPE_DEV_DEVICE]->CreateDlg(&m_stFrame);

	int i = 0;
	for (i = 0; i < D_MAX_DATA_PAGE; i++)
	{
		if (m_pPage[i] == nullptr)
			continue;
		m_pPage[i]->SetMainWindow(this);
		//m_pPage[i]->SetEditDlg(this);
	}

	ShowChildPage(TTYPE_DEV_DEVICE);
	return 1;
}

void CSysLinkerView::ShowChildPage(int nType)
{
	int i = 0;
	m_nCurType = nType;
	for (i = 0; i < D_MAX_DATA_PAGE; i++)
	{
		if (m_pPage[i] == nullptr)
			continue;
		if (i == nType)
			m_pPage[i]->ShowWindow(SW_SHOW);
		else
			m_pPage[i]->ShowWindow(SW_HIDE);
	}
}



void CSysLinkerView::CopyTreeItem(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData)
{
	if (pCurData == nullptr || pCurData->pData == nullptr)
		return;
	if(pNewData && pNewData->pData != nullptr)
	{
		delete (CDataSystem*)pNewData->pData;
		pNewData->pData = nullptr;
	}
	CDataSystem * pSys = new CDataSystem;
	pNewData->hItem = pCurData->hItem;
	pNewData->hParent = pCurData->hParent;
	pNewData->nDataType = pCurData->nDataType;
	pSys->CopyData((CDataSystem*)pCurData->pData);
	pNewData->pData = pSys;
}

BOOL CSysLinkerView::ChangeTreeItem(ST_TREEITEM * pSourceData, ST_TREEITEM * pTargetData)
{
	if (pSourceData == nullptr || pSourceData->pData == nullptr)
		return FALSE;
	if (pTargetData == nullptr || pTargetData->pData == nullptr)
		return FALSE;
	pTargetData->hItem = pSourceData->hItem;
	pTargetData->hParent = pSourceData->hParent;
	pTargetData->nDataType = pSourceData->nDataType;
	((CDataSystem*)pTargetData->pData)->CopyData((CDataSystem*)pSourceData->pData);
	return TRUE;
}

void CSysLinkerView::DeleteTreeItem(ST_TREEITEM * pNewData)
{
// 	if (pNewData == nullptr || pNewData->pData == nullptr)
// 		return;
// 	CDataSystem * pSys = (CDataSystem*)pNewData->pData;
// 	pSys->DeleteData();
// 	delete pSys;
// 	memset((void*)pNewData, 0, sizeof(ST_TREEITEM));
}

void CSysLinkerView::SetNewTreeItem(int nType, ST_TREEITEM * pNewData)
{
	CDataSystem * pSys = new CDataSystem;
	pSys->SetNewSysData(nType);
	pNewData->hItem = nullptr;
	pNewData->hParent = nullptr;
	pNewData->nDataType = nType;
	pNewData->pData = pSys;
}

void CSysLinkerView::AddInit(int nAddType)
{
	m_nAction = DATA_ADD;
	m_nAddType = nAddType;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(FALSE);
}

void CSysLinkerView::AddCancel()
{
	m_nAction = DATA_SAVE;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(TRUE);
}


int CSysLinkerView::DataAdd()
{
	int nRet = 0;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	CDataSystem * pSys = nullptr;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	int nf, nu, nc, nr;
	nf = nu = nc = nr = -1;
	pDB->BeginTransaction();
	switch (m_nAddType)
	{
	case TTYPE_DEV_FACP:
		pSys = DataAddFacp(pDB);
		if (pSys != nullptr && pSys->GetSysData() != nullptr)
		{
			pFacp = (CDataFacp*)pSys->GetSysData();
			nf = pFacp->GetFacpID();
		}
		break;
	case TTYPE_DEV_UNIT:
		pSys = DataAddUnit(pDB);
		if (pSys != nullptr && pSys->GetSysData() != nullptr)
		{
			pUnit = (CDataUnit*)pSys->GetSysData();
			nf = pUnit->GetFacpID();
			nu = pUnit->GetUnitID();
		}
		break;
	case TTYPE_DEV_CHANNEL:
		pSys = DataAddChn(pDB);
		if (pSys != nullptr && pSys->GetSysData() != nullptr)
		{
			pChn = (CDataChannel*)pSys->GetSysData();
			nf = pChn->GetFacpID();
			nu = pChn->GetUnitID();
			nc = pChn->GetChnID();
		}
		break;
	case TTYPE_DEV_DEVICE:
		pSys = DataAddRelay(pDB);
		if (pSys != nullptr && pSys->GetSysData() != nullptr)
		{
			pDev = (CDataDevice*)pSys->GetSysData();
			nf = pDev->GetFacpID();
			nu = pDev->GetUnitID();
			nc = pDev->GetChnID();
			nr = pDev->GetDeviceID();
		}
		break;
	}

	if (pSys == nullptr)
	{
		pDB->RollbackTransaction();
		return 0;
	}
	pDB->CommitTransaction();

	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_ADDITEM, m_nAddType, (LPARAM)pSys);

	switch (m_nAddType)
	{
	case TTYPE_DEV_FACP:
		m_pPage[TTYPE_DEV_UNIT]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_CHANNEL]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_UNIT:
		m_pPage[TTYPE_DEV_CHANNEL]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_CHANNEL:
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_DEVICE:
		break;
	}
// 	if (AfxGetApp())
// 		((CSysLinkerApp*)AfxGetApp())->PostMessageAllView(UWM_VIEWFORM_ADDITEM, FORM_PRJ_REFESH, 0);
	return 1;
}


int CSysLinkerView::DataSave()
{
	int nRet = 0;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	pDB->BeginTransaction();
	switch (m_pNewData->nDataType)
	{
	case TTYPE_DEV_FACP:
		nRet = DataSaveFacp(pDB);
		break;
	case TTYPE_DEV_UNIT:
		nRet = DataSaveUnit(pDB);
		break;
	case TTYPE_DEV_CHANNEL:
		nRet = DataSaveChn(pDB);
		break;
	case TTYPE_DEV_DEVICE:
		nRet = DataSaveRelay(pDB);
		break;
	}
	if (nRet <= 0)
	{
		pDB->RollbackTransaction();
		return 0;
	}
	pDB->CommitTransaction();
	ChangeTreeItem(m_pNewData, m_pRefCurData);

	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_CHANGEITEM, m_pNewData->nDataType, (LPARAM)m_pNewData);
#ifdef _DEBUG
#else
	if (AfxGetApp())
		((CSysLinkerApp*)AfxGetApp())->PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
#endif

	switch (m_pNewData->nDataType)
	{
	case TTYPE_DEV_FACP:
		m_pPage[TTYPE_DEV_UNIT]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_CHANNEL]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_UNIT:
		m_pPage[TTYPE_DEV_CHANNEL]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_CHANNEL:
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_DEVICE:
		break;
	}
	return 1;
}


int CSysLinkerView::DataDelete()
{
	int nRet = 0;
	m_vtDeleteRelationFacp.clear();
	m_vtDeleteRelationUnit.clear();
	m_ptrDeleteRelationLowerItems.RemoveAll();

	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	pDB->BeginTransaction();
	nRet = MakeDeleteItem(pDB);
	if (nRet <= 0)
	{
		pDB->RollbackTransaction();
		return 0;
	}
	pDB->CommitTransaction();

	DeleteMemoryRelationLowerItems();
	//DeleteTreeItem(m_pNewData);
	DisplayLinkData(nullptr);
	switch (m_pRefCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		m_pPage[TTYPE_DEV_UNIT]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_CHANNEL]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_UNIT:
		m_pPage[TTYPE_DEV_CHANNEL]->ChangeDataControlUpdate();
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_CHANNEL:
		m_pPage[TTYPE_DEV_DEVICE]->ChangeDataControlUpdate();
		break;
	case TTYPE_DEV_DEVICE:
		break;
	}
	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_DELITEM, m_nAddType, (LPARAM)m_pNewData);
// 
// 	if (AfxGetApp())
// 		((CSysLinkerApp*)AfxGetApp())->PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);

	return 1;
}



int CSysLinkerView::DeleteMemoryRelationLowerItems()
{
	CDataSystem * pSys, *pTemp;
	CDataDevice * pDev;
	CDataPS *pPs;
	CDataPump *pPmp;
	CDataPattern * pPtn;
	int i = 0 , nFNum; 
	CString strKey;
	CMapSystemData * pMap = nullptr;
	ST_REMOVE_LINKED * pRm;
	std::map<CString, CDataUnit*> *	pMapUnitNum; //< KEY : facpnum.unitNum , Data : Reference
	std::map<int, CDataFacp*>			*pMapFacpNum; //< KEY : FcapNum  , Data : Reference
	std::shared_ptr<CManagerPS> spPs;
	std::shared_ptr<CManagerPump> spPump;
	std::shared_ptr<CManagerPattern> spPtn;
	/*
	1. 압력 스위치 삭제
	2. 펌프 삭제
	3. 패터 삭제
	4. Output 연동 삭제
	5. Unit Map 삭제
	6. Facp Map 삭제
	7. System 삭제
	*/

	// 1. 압력 스위치 삭제
	while (!m_ptrDeleteRelationPSwitchItems.IsEmpty())
	{
		pRm = (ST_REMOVE_LINKED*)m_ptrDeleteRelationPSwitchItems.RemoveHead();
		if (pRm == nullptr)
			continue;
		pPs = m_pRefFasSysData->GetPSwitch(pRm->nSrcFacp, pRm->nSrcChn);
		if (pPs == nullptr)
			continue; 
		pPs->DeleteLink(pRm->btLinkType, pRm->nTgtFacp, pRm->nTgtUnit, pRm->nTgtChn, pRm->nTgtDev);
	}
	
	// 2. 펌프 삭제
	while (!m_ptrDeleteRelationPumpItems.IsEmpty())
	{
		pRm = (ST_REMOVE_LINKED*)m_ptrDeleteRelationPumpItems.RemoveHead();
		if (pRm == nullptr)
			continue;
		pPmp = m_pRefFasSysData->GetPump(pRm->nSrcFacp, pRm->nSrcChn);
		if (pPmp == nullptr)
			continue;
		pPmp->DeleteLink(pRm->btLinkType, pRm->nTgtFacp, pRm->nTgtUnit, pRm->nTgtChn, pRm->nTgtDev);
	}

	// 3. 패턴 삭제
	while (!m_ptrDeleteRelationPatternItems.IsEmpty())
	{
		pRm = (ST_REMOVE_LINKED*)m_ptrDeleteRelationPatternItems.RemoveHead();
		if (pRm == nullptr)
			continue;
		pPtn = m_pRefFasSysData->GetPattern(pRm->nSrcFacp);
		if (pPtn == nullptr)
			continue;
		pPtn->DeleteItemByID(pRm->btLinkType, pRm->nTgtFacp, pRm->nTgtUnit, pRm->nTgtChn, pRm->nTgtDev);
	}

	// 4. Output 삭제
	pMap = m_pRefFasSysData->GetSystemData();
	if (pMap == nullptr)
		return 0;
	while (!m_ptrDeleteRelationOutputLinkItems.IsEmpty())
	{
		pRm = (ST_REMOVE_LINKED*)m_ptrDeleteRelationOutputLinkItems.RemoveHead();
		if (pRm == nullptr)
			continue;
		strKey = GF_GetIDSysDataKey(SE_RELAY, pRm->nSrcFacp, pRm->nSrcUnit, pRm->nSrcChn, pRm->nSrcDev);
		pTemp = (*pMap)[strKey];
		if (pTemp == nullptr || pTemp->GetSysData() == nullptr || pTemp->GetDataType() != SE_RELAY)
			continue;
		pDev = (CDataDevice*)pTemp->GetSysData();
		if (pDev == nullptr)
			continue;
		pDev->DeleteLinkByID(pRm->btLinkType, pRm->nTgtFacp, pRm->nTgtUnit, pRm->nTgtChn, pRm->nTgtDev);
	}

	// 5. 유닛 삭제
	pMapUnitNum = m_pRefFasSysData->GetUnitMap();
	for (i = 0; i < m_vtDeleteRelationUnit.size(); i++)
	{
		strKey = m_vtDeleteRelationUnit[i];
		pMapUnitNum->erase(strKey);
	}
	m_vtDeleteRelationUnit.clear();

	// 6. Facp 삭제
	pMapFacpNum = m_pRefFasSysData->GetFacpMap();
	for (i = 0; i < m_vtDeleteRelationFacp.size(); i++)
	{
		nFNum = m_vtDeleteRelationFacp[i];
		pMapFacpNum->erase(nFNum);
	}
	m_vtDeleteRelationFacp.clear();

	// 7. System Data삭제
	while (!m_ptrDeleteRelationLowerItems.IsEmpty())
	{
		pSys = (CDataSystem *)m_ptrDeleteRelationLowerItems.RemoveHead();
		if (pSys == nullptr || pSys->GetSysData() == nullptr)
			continue;
		m_pRefFasSysData->RemoveSysMap(pSys);
		delete pSys;
		pSys = nullptr;
	}
	return 1;
}

CDataSystem * CSysLinkerView::DataAddFacp(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return nullptr;
	if (m_pNewData->nDataType != TTYPE_DEV_FACP || m_pNewData->pData == nullptr)
		return nullptr;
	CString strSql;
	CDataSystem * pSys;
	CDataFacp *pNew;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_FACP || pSys->GetSysData() == nullptr)
		return nullptr;
	pNew = (CDataFacp*)pSys->GetSysData();

	// [KHS 2020-7-16 10:55:47] 
	// 같은 수신기 번호로 있는지 확인
	if (m_pRefFasSysData->GetFacpByNum(pNew->GetFacpNum()) != nullptr)
	{
		CString str;
#ifndef ENGLISH_MODE
		str.Format(L"수신기 번호 [%d]는 이미 있습니다.\n기존 수신기 정보를 바꾸시겠습니까?", pNew->GetFacpNum());
#else
		str.Format(L"The FACP number [%d] already exists.\nDo you want to change the existing FACP information ? ", pNew->GetFacpNum());
#endif
		if (AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION) != IDYES)
			return nullptr;
	}

	strSql.Format(L"SELECT * FROM TB_FACP WHERE FACP_ID=%d ", pNew->GetFacpID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return nullptr;

	if (pDB->GetRecordCount() > 0)
	{
		strSql.Format(L"UPDATE TB_FACP SET FACP_NAME='%s' , FACP_NUM=%d , FACP_TYPE=%d WHERE FACP_ID =%d"
			, pNew->GetFacpName(), pNew->GetFacpNum(), pNew->GetFacpType()
			, pNew->GetFacpID()
		);
	}
	else
	{
		strSql.Format(L"INSERT INTO TB_FACP(NET_ID,FACP_ID,FACP_NUM,FACP_TYPE,FACP_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetFacpNum(), pNew->GetFacpType(), pNew->GetFacpName()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return nullptr;
	pSys = m_pRefFasSysData->AddSystemFacpDataByNum(pNew->GetFacpNum(), pNew->GetFacpID(), pNew->GetFacpType(), pNew->GetFacpName());

	// 	m_pRefCurData->nDataType = TTYPE_DEV_FACP;
	// 	m_pRefCurData->pData = pSys;
	return pSys;

}


CDataSystem * CSysLinkerView::DataAddUnit(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return nullptr;
	if (m_pNewData->nDataType != TTYPE_DEV_UNIT || m_pNewData->pData == nullptr)
		return nullptr;
	CString strSql;
	CDataSystem * pSys;
	CDataUnit *pNew;
	int nFNum;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_UNIT || pSys->GetSysData() == nullptr)
		return nullptr;
	pNew = (CDataUnit*)pSys->GetSysData();

	// [KHS 2020-7-16 10:55:47] 
	// 같은 수신기 번호로 있는지 확인
	nFNum = m_pRefFasSysData->CvtFacpIDToNum(pNew->GetFacpID());
	if (m_pRefFasSysData->GetUnitByNum(nFNum,pNew->GetUnitNum()) != nullptr)
	{
		CString str;
#ifndef ENGLISH_MODE
		str.Format(L"수신기 번호[%d] 유닛번호[%d]는 이미 있습니다.\n기존 유닛 정보를 바꾸시겠습니까?"
			, nFNum, pNew->GetUnitNum());
#else
		str.Format(L"The FACP number [%d] unit number [%d] already exists.\nDo you want to change the existing unit information ?"
			, nFNum, pNew->GetUnitNum());
#endif
		if (AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION) != IDYES)
			return nullptr;
	}

	strSql.Format(L"SELECT * FROM TB_UNIT WHERE FACP_ID=%d AND UNIT_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return nullptr;

	if (pDB->GetRecordCount() > 0)
	{
		strSql.Format(L"UPDATE TB_UNIT SET UNIT_NAME='%s' , UNIT_NUM=%d ,UNIT_TYPE=%d "
			L" WHERE FACP_ID =%d AND UNIT_ID=%d "
			, pNew->GetUnitName(), pNew->GetUnitNum(), pNew->GetUnitType()
			, pNew->GetFacpID(), pNew->GetUnitID()
		);
	}
	else
	{
		strSql.Format(L"INSERT INTO TB_UNIT(NET_ID,FACP_ID,UNIT_ID,UNIT_NUM,UNIT_TYPE,UNIT_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetUnitNum(), pNew->GetUnitType()
			, pNew->GetUnitName()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return nullptr;

	pSys = m_pRefFasSysData->AddSystemUnitDataByID(pNew->GetFacpID(), pNew->GetUnitNum()
		, pNew->GetUnitID(), pNew->GetUnitType(), pNew->GetUnitName());

	// 	m_pRefCurData->nDataType = TTYPE_DEV_UNIT;
	// 	m_pRefCurData->pData = pSys;
	return pSys;
}

CDataSystem * CSysLinkerView::DataAddChn(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return nullptr;
	if (m_pNewData->nDataType != TTYPE_DEV_CHANNEL || m_pNewData->pData == nullptr)
		return nullptr;
	CString strSql;
	CDataSystem * pSys;
	CDataChannel *pNew;
	int nFNum, nUNum;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_CHANNEL || pSys->GetSysData() == nullptr)
		return nullptr;
	pNew = (CDataChannel*)pSys->GetSysData();

	// [KHS 2020-7-16 10:55:47] 
	// 같은 수신기 번호로 있는지 확인
	nFNum = m_pRefFasSysData->CvtFacpIDToNum(pNew->GetFacpID());
	nUNum = m_pRefFasSysData->CvtUnitIDToNum(pNew->GetFacpID(), pNew->GetUnitID());
	if (m_pRefFasSysData->GetChannelByNum(nFNum, nUNum, pNew->GetChnNum()) != nullptr)
	{
		CString str;
#ifndef ENGLISH_MODE
		str.Format(L"수신기 번호[%d] 유닛번호[%d] 계통번호[%d]는 이미 있습니다.\n기존 계통 정보를 바꾸시겠습니까?"
			, nFNum , nUNum, pNew->GetChnNum());
#else
		str.Format(L"The FACP number [%d] unit number [%d] loop number [%d] already exists.\nDo you want to change the existing loop information ?"
			, nFNum, nUNum, pNew->GetChnNum());
#endif
		if (AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION) != IDYES)
			return nullptr;
	}

	strSql.Format(L"SELECT * FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return nullptr;

	if (pDB->GetRecordCount() > 0)
	{
		strSql.Format(L"UPDATE TB_CHANNEL SET CHN_NAME='%s' , CHN_NUM=%d "
			L" WHERE FACP_ID =%d AND UNIT_ID=%d AND CHN_ID=%d "
			, pNew->GetChnName(), pNew->GetChnNum()
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID()
		);
	}
	else
	{
		strSql.Format(L"INSERT INTO TB_CHANNEL(NET_ID,FACP_ID,UNIT_ID,CHN_ID,CHN_NUM,CHN_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,"
			L"'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetChnNum()
			, pNew->GetChnName()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return nullptr;

	pSys = m_pRefFasSysData->AddSystemChannelDataByID(pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnNum()
		, pNew->GetChnNum(), pNew->GetChnName()
	);

	// 	m_pRefCurData->nDataType = TTYPE_DEV_CHANNEL;
	// 	m_pRefCurData->pData = pSys;
	return pSys;
}

CDataSystem * CSysLinkerView::DataAddRelay(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return nullptr;
	if (m_pNewData->nDataType != TTYPE_DEV_DEVICE || m_pNewData->pData == nullptr)
		return nullptr;

	CString strSql, strCmd, strWhere;
	CDataSystem * pSys;
	CDataDevice *pNew;
	CDataLocBase * pLoc;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	int nRelayIndex = 0;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		return nullptr;
	pNew = (CDataDevice*)pSys->GetSysData();
	nRelayIndex = m_pRefFasSysData->GetNewRelayIndex();
	pNew->SetRelayIndex(nRelayIndex);
	// [KHS 2020-7-16 10:55:47] 
	// 같은 수신기 번호로 있는지 확인
	;
	
	if (m_pRefFasSysData->GetDevice(pNew->GetDevKey()) != nullptr)
	{
		CString str;
#ifndef ENGLISH_MODE
		str.Format(L"수신기 번호[%d] 유닛번호[%d] 계통번호[%d] 회로번호[%d]는 이미 있습니다.\n기존 회로 정보를 바꾸시겠습니까?"
			, pNew->GetFacpNum(),pNew->GetUnitNum() , pNew->GetDeviceNum() , pNew->GetDeviceNum());
#else
		str.Format(L"The FACP number [%d] unit number [%d] loop number [%d] circuit number [%d] already exists.\nDo you want to replace the existing circuit information ? "
			, pNew->GetFacpNum(), pNew->GetUnitNum(), pNew->GetDeviceNum(), pNew->GetDeviceNum());
#endif
		if (AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION) != IDYES)
			return nullptr;
	}

	nInput = nOut = nContents = 0;
	nBuild = nBtype = nStair = nFloor = nRoom = 0;
	pInput = pNew->GetEqInput();
	pOut = pNew->GetEqOutput();
	pContents = pNew->GetEqOutContents();
	pEqName = pNew->GetEqName();
	if (pInput)
		nInput = (int)pInput->GetEquipID();
	if (pOut)
		nOut = (int)pOut->GetEquipID();
	if (pContents)
		nContents = (int)pContents->GetEquipID();
	if (pEqName)
		nEqID = (int)pEqName->GetEquipID();
	pLoc = pNew->GetDevInputLocation();
	if (pLoc)
	{
		nBuild = (int)pLoc->GetBuildID();
		nBtype = (int)pLoc->GetBTypeID();
		nStair = (int)pLoc->GetStairID();
		nFloor = (int)pLoc->GetFloorID();
		nRoom = (int)pLoc->GetRoomID();
	}

	strSql.Format(L"SELECT * FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
	);
	if (pDB->OpenQuery(strSql) == FALSE)
		return nullptr;

	if (pDB->GetRecordCount() > 0)
	{
		strCmd.Format(L"UPDATE TB_RELAY_LIST SET RLY_NUM=%d,MAP_KEY='%s',RPT_NUM=%d"
			L",INPUT_ID=%d,OUTPUT_ID=%d "
			L",BD_ID=%d,BTYPE_ID=%d,STAIR_ID=%d,FL_ID=%d,RM_ID=%d "
			L",OUTCONTENTS_ID=%d,EQ_ID=%d , EQ_ADD_IDX='%s' "
			, pNew->GetDeviceNum(), pNew->GetDevKey(), pNew->GetRepeatorNum()
			, nInput, nOut
			, nBuild, nBtype, nStair, nFloor, nRoom
			, nContents, nEqID, pNew->GetEqAddIndex()
		);
		strWhere.Format(L" WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
		);
		strSql = strCmd + strWhere;
	}
	else
	{
		strSql.Format(L"INSERT INTO TB_RELAY_LIST(NET_ID,RIDX,FACP_ID,UNIT_ID"
			L",CHN_ID,RLY_ID,RLY_NUM "
			L",MAP_KEY"
			L",RPT_NUM,INPUT_ID"
			L",OUTPUT_ID "
			L",BD_ID,BTYPE_ID"
			L",STAIR_ID,FL_ID"
			L",RM_ID,OUTCONTENTS_ID"
			L",EQ_ID , EQ_ADD_IDX , ADD_USER) "
			L" VALUES(1,%d,%d,%d"
			L",%d,%d,%d"
			L",'%s'"
			L",%d,%d"
			L",%d"
			L",%d,%d"
			L",%d,%d"
			L",%d,%d"
			L",%d,'%s','ADMIN') "
			, pNew->GetRelayIndex()
			, pNew->GetFacpID(), pNew->GetUnitID()
			, pNew->GetChnID(), pNew->GetDeviceID(), pNew->GetDeviceNum()
			, pNew->GetDevKey()
			, pNew->GetRepeatorNum(), nInput
			, nOut
			, nBuild, nBtype
			, nStair, nFloor
			, nRoom, nContents
			, nEqID, pNew->GetEqAddIndex()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return nullptr;

	pSys = m_pRefFasSysData->AddSystemDeviceDataByID(pNew->GetFacpID(), pNew->GetUnitID()
		, pNew->GetChnID(), pNew->GetDeviceID(), pNew->GetDeviceNum()
		, nBuild, nBtype, nStair, nFloor, nRoom
		, -1, -1, -1, -1, -1
		, nInput, nOut, nContents
		, nEqID, pNew->GetEqAddIndex()
		, pNew->GetDeviceNum() / 4
		, 0
		, pNew->GetDevKey()
		, nullptr, nullptr, nullptr
		, pNew->GetRelayIndex()
	);

	// 	m_pRefCurData->nDataType = TTYPE_DEV_DEVICE;
	// 	m_pRefCurData->pData = pSys;

	return pSys;
}

int CSysLinkerView::DataSaveFacp(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;
	if (m_pNewData->nDataType != TTYPE_DEV_FACP || m_pNewData->pData == nullptr)
		return 0;
	if (m_pRefCurData == nullptr || m_pRefCurData->nDataType != TTYPE_DEV_FACP || m_pRefCurData->pData == nullptr)
		return 0;

	CString strSql;
	CDataSystem * pSys;
	CDataFacp* pOld, *pNew;
	pSys = (CDataSystem*)m_pRefCurData->pData;
	if (pSys->GetDataType() != SE_FACP || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataFacp*)pSys->GetSysData();
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_FACP || pSys->GetSysData() == nullptr)
		return 0;
	pNew = (CDataFacp*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_FACP WHERE FACP_ID=%d ", pNew->GetFacpID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	if (pDB->GetRecordCount() <= 0)
	{
		strSql.Format(L"INSERT INTO TB_FACP(NET_ID,FACP_ID,FACP_NUM,FACP_TYPE,FACP_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetFacpNum(), pNew->GetFacpType(), pNew->GetFacpName()
		);
	}
	else
	{
		strSql.Format(L"UPDATE TB_FACP SET FACP_NAME='%s' , FACP_NUM=%d , FACP_TYPE=%d WHERE FACP_ID =%d"
			, pNew->GetFacpName(), pNew->GetFacpNum(), pNew->GetFacpType()
			, pOld->GetFacpID()
		);
	}


	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1;
}

int CSysLinkerView::DataSaveChn(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;
	if (m_pNewData->nDataType != TTYPE_DEV_CHANNEL || m_pNewData->pData == nullptr)
		return 0;
	if (m_pRefCurData == nullptr || m_pRefCurData->nDataType != TTYPE_DEV_CHANNEL || m_pRefCurData->pData == nullptr)
		return 0;

	CString strSql;
	CDataSystem * pSys;
	CDataChannel* pOld, *pNew;
	pSys = (CDataSystem*)m_pRefCurData->pData;
	if (pSys->GetDataType() != SE_CHANNEL || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataChannel*)pSys->GetSysData();
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_CHANNEL || pSys->GetSysData() == nullptr)
		return 0;
	pNew = (CDataChannel*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	if (pDB->GetRecordCount() <= 0)
	{
		strSql.Format(L"INSERT INTO TB_CHANNEL(NET_ID,FACP_ID,UNIT_ID,CHN_ID,CHN_NUM,CHN_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,"
			L"'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetChnNum()
			, pNew->GetChnName()
		);
	}
	else
	{
		strSql.Format(L"UPDATE TB_CHANNEL SET CHN_NAME='%s' , CHN_NUM=%d "
			L" WHERE FACP_ID =%d AND UNIT_ID=%d AND CHN_ID=%d "
			, pNew->GetChnName(), pNew->GetChnNum()
			, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1;
}

int CSysLinkerView::DataSaveUnit(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;
	if (m_pNewData->nDataType != TTYPE_DEV_UNIT || m_pNewData->pData == nullptr)
		return 0;
	if (m_pRefCurData == nullptr || m_pRefCurData->nDataType != TTYPE_DEV_UNIT || m_pRefCurData->pData == nullptr)
		return 0;

	CString strSql;
	CDataSystem * pSys;
	CDataUnit* pOld, *pNew;
	pSys = (CDataSystem*)m_pRefCurData->pData;
	if (pSys->GetDataType() != SE_UNIT || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataUnit*)pSys->GetSysData();
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_UNIT || pSys->GetSysData() == nullptr)
		return 0;
	pNew = (CDataUnit*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_UNIT WHERE FACP_ID=%d AND UNIT_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	if (pDB->GetRecordCount() <= 0)
	{
		strSql.Format(L"INSERT INTO TB_UNIT(NET_ID,FACP_ID,UNIT_ID,UNIT_NUM,UNIT_TYPE,UNIT_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetUnitNum(), pNew->GetUnitType()
			, pNew->GetUnitName()
		);
	}
	else
	{
		strSql.Format(L"UPDATE TB_UNIT SET UNIT_NAME='%s' , UNIT_NUM=%d ,UNIT_TYPE=%d "
			L" WHERE FACP_ID =%d AND UNIT_ID=%d "
			, pNew->GetUnitName(), pNew->GetUnitNum(), pNew->GetUnitType()
			, pOld->GetFacpID(), pOld->GetUnitID()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1;
}

int CSysLinkerView::DataSaveRelay(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;
	if (m_pNewData->nDataType != TTYPE_DEV_DEVICE || m_pNewData->pData == nullptr)
		return 0;
	if (m_pRefCurData == nullptr || m_pRefCurData->nDataType != TTYPE_DEV_DEVICE || m_pRefCurData->pData == nullptr)
		return 0;

	CString strSql, strCmd, strWhere;
	CDataSystem * pSys;
	CDataDevice* pOld, *pNew;
	CDataLocBase * pLoc;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	pSys = (CDataSystem*)m_pRefCurData->pData;
	if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataDevice*)pSys->GetSysData();
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		return 0;
	pNew = (CDataDevice*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
	);
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	if (pDB->GetRecordCount() <= 0)
		return 0;

	nInput = nOut = nContents = 0;
	nBuild = nBtype = nStair = nFloor = nRoom = 0;
	pInput = pNew->GetEqInput();
	pOut = pNew->GetEqOutput();
	pContents = pNew->GetEqOutContents();
	pEqName = pNew->GetEqName();
	if (pInput)
		nInput = (int)pInput->GetEquipID();
	if (pOut)
		nOut = (int)pOut->GetEquipID();
	if (pContents)
		nContents = (int)pContents->GetEquipID();
	if (pEqName)
		nEqID = (int)pEqName->GetEquipID();
	pLoc = pNew->GetDevInputLocation();
	if (pLoc)
	{
		nBuild = (int)pLoc->GetBuildID();
		nBtype = (int)pLoc->GetBTypeID();
		nStair = (int)pLoc->GetStairID();
		nFloor = (int)pLoc->GetFloorID();
		nRoom = (int)pLoc->GetRoomID();
	}

	strCmd.Format(L"UPDATE TB_RELAY_LIST SET RLY_NUM=%d,MAP_KEY='%s',RPT_NUM=%d"
		L",INPUT_ID=%d,OUTPUT_ID=%d "
		L",BD_ID=%d,BTYPE_ID=%d,STAIR_ID=%d,FL_ID=%d,RM_ID=%d "
		L",OUTCONTENTS_ID=%d,EQ_ID=%d , EQ_ADD_IDX='%s' "
		L",FACP_ID=%d,UNIT_ID=%d , CHN_ID=%d "
		, pNew->GetDeviceNum(), pNew->GetDevKey(), pNew->GetRepeatorNum()
		, nInput, nOut
		, nBuild, nBtype, nStair, nFloor, nRoom
		, nContents, nEqID, pNew->GetEqAddIndex()
		, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID()
	);
	strWhere.Format(L" WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID()
	);

	strSql = strCmd + strWhere;

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}

int CSysLinkerView::MakeDeleteItem(YAdoDatabase * pDB)
{
	/*
	1. 수신기 삭제
	2. 유닛 삭제
	3. 채널 삭제
	4. 입력 연동 삭제
	5. 출력 연동 삭제
	*/
	if (m_pRefFasSysData == nullptr)
		return 0;

	// device , chn , unit,facp
	if (m_pRefCurData == nullptr || m_pRefCurData->pData == nullptr)
		return 0;

	int nCnt, i;
	int nF, nU, nC, nR;
	//int nFNum, nUNum, nCNum;
	int nif, niu, nic, nir;
	int ntf, ntu, ntc, ntr, nlnkType ,nSrcFacp, nPid;
	CString strDelFacp, strDelUnit, strDelChn, strDelDev, strDelInputLink, strDelOutput, strDelPattern, strDelPump, strDelPSwich;
	CString strSelectFacp, strSelectUnit, strSelectChn, strSelectDev, strSelectInputLink, strSelectOutput, strSelectPattern, strSelectPump, strSelectPSwich;
	CString strKey;
	CDataSystem * pSys, *pTemp;
	CDataFacp* pFacp = nullptr;
	CDataUnit * pUnit = nullptr;
	CDataChannel * pChn = nullptr;
	CDataDevice *pDev = nullptr;

	ST_REMOVE_LINKED * pRm = nullptr;
	CMapSystemData * pMap = nullptr;

	strSelectFacp = strSelectUnit = strSelectChn = strSelectDev = strSelectInputLink = strSelectOutput = strSelectPattern = strSelectPump = strSelectPSwich = L"";
	strDelFacp = strDelUnit = strDelChn = strDelDev = strDelInputLink = strDelOutput = strDelPattern = strDelPump = strDelPSwich = L"";
	
	pSys = (CDataSystem*)m_pRefCurData->pData;
	if (pSys->GetDataType() != m_pRefCurData->nDataType || pSys->GetSysData() == nullptr)
		return 0;
	pMap = m_pRefFasSysData->GetSystemData();
	if (pMap == nullptr)
		return 0;

	nF = nU = nC = nR = -1;
	nif = niu = nic = nir = -1;
	ntf = ntu = ntc = ntr = nlnkType = nSrcFacp = nPid = -1;
	switch (m_pRefCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp*)pSys->GetSysData();
		nF = pFacp->GetFacpNum();
		nif = pFacp->GetFacpID();
		m_ptrDeleteRelationLowerItems.AddTail(pSys);
		m_vtDeleteRelationFacp.push_back(nF);

		strDelFacp.Format(L"DELETE FROM TB_FACP WHERE FACP_ID=%d ", pFacp->GetFacpID());
		strSelectFacp.Format(L"SELECT  * FROM TB_UNIT WHERE FACP_ID=%d ", pFacp->GetFacpID());
		strDelUnit.Format(L"DELETE FROM TB_UNIT WHERE FACP_ID=%d ", pFacp->GetFacpID());
		strSelectChn.Format(L"SELECT * FROM TB_CHANNEL WHERE FACP_ID=%d ", pFacp->GetFacpID());
		strDelChn.Format(L"DELETE FROM TB_CHANNEL WHERE FACP_ID=%d ", pFacp->GetFacpID());
		strSelectDev.Format(L"SELECT * FROM TB_RELAY_LIST WHERE FACP_ID=%d ", pFacp->GetFacpID());
		strDelDev.Format(L"DELETE FROM TB_RELAY_LIST WHERE FACP_ID=%d ", pFacp->GetFacpID());
		strDelInputLink.Format(L"DELETE FROM TB_LINK_RELAY WHERE SRC_FACP=%d ", pFacp->GetFacpID());
		strSelectOutput.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			, LK_TYPE_RELEAY, pFacp->GetFacpID());
		strDelOutput.Format(L"DELETE FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			, LK_TYPE_RELEAY, pFacp->GetFacpID());
		strSelectPattern.Format(L"SELECT * FROM TB_PATTERN_ITEM WHERE ITEM_TYPE=%d AND FACP_ID=%d "
			, LK_TYPE_RELEAY, pFacp->GetFacpID());
		strDelPattern.Format(L"DELETE FROM TB_PATTERN_ITEM WHERE ITEM_TYPE=%d AND FACP_ID=%d "
			, LK_TYPE_RELEAY, pFacp->GetFacpID());
		strSelectPump.Format(L"SELECT * FROM TB_LINK_PUMP WHERE LINK_TYPE=%d AND FACP_ID=%d "
			, LK_TYPE_RELEAY, pFacp->GetFacpID());
		strDelPump.Format(L"DELETE FROM TB_LINK_PUMP WHERE LINK_TYPE=%d AND FACP_ID=%d "
			, LK_TYPE_RELEAY, pFacp->GetFacpID());
		strSelectPSwich.Format(L"SELECT * FROM TB_LINK_PSWITCH WHERE LINK_TYPE=%d AND FACP_ID=%d "
			, LK_TYPE_RELEAY, pFacp->GetFacpID());
		strDelPSwich.Format(L"DELETE FROM TB_LINK_PSWITCH WHERE LINK_TYPE=%d AND FACP_ID=%d "
			, LK_TYPE_RELEAY, pFacp->GetFacpID());
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit*)pSys->GetSysData();
		nif = pUnit->GetFacpID();
		nU = pUnit->GetUnitNum();
		niu = pUnit->GetUnitID();
		m_ptrDeleteRelationLowerItems.AddTail(pSys);
		strKey.Format(L"%02d.%02d", nF, nU);
		m_vtDeleteRelationUnit.push_back(strKey);
		strDelUnit.Format(L"DELETE FROM TB_UNIT WHERE FACP_ID=%d AND UNIT_ID=%d "
			, pUnit->GetFacpID(), pUnit->GetUnitID());
		strSelectChn.Format(L"SELECT * FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d "
			, pUnit->GetFacpID(), pUnit->GetUnitID());
		strDelChn.Format(L"DELETE FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d "
			, pUnit->GetFacpID(), pUnit->GetUnitID());
		strSelectDev.Format(L"SELECT * FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d "
			, pUnit->GetFacpID(), pUnit->GetUnitID());
		strDelDev.Format(L"DELETE FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d "
			, pUnit->GetFacpID(), pUnit->GetUnitID());
		strDelInputLink.Format(L"DELETE FROM TB_LINK_RELAY WHERE SRC_FACP=%d AND SRC_UNIT=%d"
			, pUnit->GetFacpID(), pUnit->GetUnitID());
		strSelectOutput.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
			, LK_TYPE_RELEAY, pUnit->GetFacpID(), pUnit->GetUnitID());
		strDelOutput.Format(L"DELETE FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
			, LK_TYPE_RELEAY, pUnit->GetFacpID(), pUnit->GetUnitID());
		strSelectPattern.Format(L"SELECT * FROM TB_PATTERN_ITEM WHERE ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			, LK_TYPE_RELEAY, pUnit->GetFacpID(), pUnit->GetUnitID());
		strDelPattern.Format(L"DELETE FROM TB_PATTERN_ITEM WHERE ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			, LK_TYPE_RELEAY, pUnit->GetFacpID(), pUnit->GetUnitID());
		strSelectPump.Format(L"SELECT * FROM TB_LINK_PUMP WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			, LK_TYPE_RELEAY, pUnit->GetFacpID(), pUnit->GetUnitID());
		strDelPump.Format(L"DELETE FROM TB_LINK_PUMP WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			, LK_TYPE_RELEAY, pUnit->GetFacpID(), pUnit->GetUnitID());
		strSelectPSwich.Format(L"SELECT * FROM TB_LINK_PSWITCH WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			, LK_TYPE_RELEAY, pUnit->GetFacpID(), pUnit->GetUnitID());
		strDelPSwich.Format(L"DELETE FROM TB_LINK_PSWITCH WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			, LK_TYPE_RELEAY, pUnit->GetFacpID(), pUnit->GetUnitID());
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel*)pSys->GetSysData();
		nif = pChn->GetFacpID();
		niu = pChn->GetUnitID();
		nic = pChn->GetChnID();
		nC = pChn->GetChnNum();
		m_ptrDeleteRelationLowerItems.AddTail(pSys);

		strDelChn.Format(L"DELETE FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strSelectDev.Format(L"SELECT * FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strDelDev.Format(L"DELETE FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strDelInputLink.Format(L"DELETE FROM TB_LINK_RELAY WHERE SRC_FACP=%d AND SRC_UNIT=%d"
			L" AND SRC_CHN=%d "
			, pChn->GetFacpID(), pChn->GetUnitID(), pChn->GetChnID());
		strSelectOutput.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
			L" AND TGT_CHN=%d "
			, LK_TYPE_RELEAY, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strDelOutput.Format(L"DELETE FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
			L" AND TGT_CHN=%d "
			, LK_TYPE_RELEAY, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strSelectPattern.Format(L"SELECT * FROM TB_PATTERN_ITEM WHERE ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, LK_TYPE_RELEAY, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strDelPattern.Format(L"DELETE FROM TB_PATTERN_ITEM WHERE ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, LK_TYPE_RELEAY, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strSelectPump.Format(L"SELECT * FROM TB_LINK_PUMP WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, LK_TYPE_RELEAY, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strDelPump.Format(L"DELETE FROM TB_LINK_PUMP WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, LK_TYPE_RELEAY, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strSelectPSwich.Format(L"SELECT * FROM TB_LINK_PSWITCH WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, LK_TYPE_RELEAY, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		strDelPSwich.Format(L"DELETE FROM TB_LINK_PSWITCH WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d "
			, LK_TYPE_RELEAY, pChn->GetFacpID(), pChn->GetUnitID()
			, pChn->GetChnID()
		);
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice*)pSys->GetSysData();
		m_ptrDeleteRelationLowerItems.AddTail(pSys);
		strDelDev.Format(L"DELETE FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d AND RLY_ID=%d "
			, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strDelInputLink.Format(L"DELETE FROM TB_LINK_RELAY WHERE SRC_FACP=%d AND SRC_UNIT=%d"
			L" AND SRC_CHN=%d AND SRC_RLY=%d "
			, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strSelectOutput.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
			L" AND TGT_CHN=%d AND TGT_RLY=%d "
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strDelOutput.Format(L"DELETE FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
			L" AND TGT_CHN=%d AND TGT_RLY=%d "
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strSelectPattern.Format(L"SELECT * FROM TB_PATTERN_ITEM WHERE ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d AND RLY_ID=%d "
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strDelPattern.Format(L"DELETE FROM TB_PATTERN_ITEM WHERE ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d AND RLY_ID=%d "
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strSelectPump.Format(L"SELECT * FROM TB_LINK_PUMP WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d AND RLY_ID=%d "
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strDelPump.Format(L"DELETE FROM TB_LINK_PUMP WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d AND RLY_ID=%d "
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strSelectPSwich.Format(L"SELECT * FROM TB_LINK_PSWITCH WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d AND RLY_ID=%d "
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		strDelPSwich.Format(L"DELETE FROM TB_LINK_PSWITCH WHERE LINK_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d "
			L" AND CHN_ID=%d AND RLY_ID=%d "
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		break;
	default:
		return 0;
	}

	if (m_pRefCurData->nDataType == TTYPE_DEV_FACP)
	{
		if (pDB->ExecuteSql(strDelFacp) == FALSE)
			return 0;

		if (pDB->OpenQuery(strSelectUnit) == FALSE)
			return 0;

		nCnt = pDB->GetRecordCount();
		for (i = 0; i < nCnt; i++)
		{
			if (pDB->GetFieldValue(L"UNIT_NUM", nU) == FALSE)
				continue;
			if (nF < 0)
				continue; 
			strKey = GF_GetSysDataKey(SE_UNIT, nF, nU);
			pTemp = (*pMap)[strKey];
			if (pTemp && pTemp->GetDataType() == SE_UNIT)
			{
				m_ptrDeleteRelationLowerItems.AddTail(pTemp);
				strKey.Format(L"%02d.%02d", nF, nU);
				m_vtDeleteRelationUnit.push_back(strKey);
			}
		}
	}

	if (m_pRefCurData->nDataType == TTYPE_DEV_FACP || m_pRefCurData->nDataType == TTYPE_DEV_UNIT)
	{
		if (pDB->ExecuteSql(strDelUnit) == FALSE)
			return 0;
		if (pDB->OpenQuery(strSelectChn) == FALSE)
			return 0;
		nCnt = pDB->GetRecordCount();
		for (i = 0; i < nCnt; i++)
		{
			if (pDB->GetFieldValue(L"FACP_ID", nif) == FALSE)
				continue;
			if (pDB->GetFieldValue(L"UNIT_ID", niu) == FALSE)
				continue;
			if (pDB->GetFieldValue(L"CHN_ID", nic) == FALSE)
				continue;

			strKey = GF_GetIDSysDataKey(SE_CHANNEL, nif, niu, nic);
			pTemp = (*pMap)[strKey];
			if (pTemp)
				m_ptrDeleteRelationLowerItems.AddTail(pTemp);
		}
	}

	if (m_pRefCurData->nDataType == TTYPE_DEV_FACP || m_pRefCurData->nDataType == TTYPE_DEV_UNIT
		|| m_pRefCurData->nDataType == TTYPE_DEV_CHANNEL)
	{
		
		if (pDB->ExecuteSql(strDelChn) == FALSE)
			return 0;

		if (pDB->OpenQuery(strSelectDev) == FALSE)
			return 0;
		nCnt = pDB->GetRecordCount();
		for (i = 0; i < nCnt; i++)
		{
			if (pDB->GetFieldValue(L"FACP_ID", nif) == FALSE)
				continue;
			if (pDB->GetFieldValue(L"UNIT_ID", niu) == FALSE)
				continue;
			if (pDB->GetFieldValue(L"CHN_ID", nic) == FALSE)
				continue;
			if (pDB->GetFieldValue(L"RLY_ID", nir) == FALSE)
				continue;

			strKey = GF_GetIDSysDataKey(SE_RELAY, nif, niu, nic, nir);
			pTemp = (*pMap)[strKey];
			if (pTemp)
				m_ptrDeleteRelationLowerItems.AddTail(pTemp);
		}
		if (pDB->ExecuteSql(strDelDev) == FALSE)
			return 0;
	}

	if (m_pRefCurData->nDataType == TTYPE_DEV_FACP || m_pRefCurData->nDataType == TTYPE_DEV_UNIT
		|| m_pRefCurData->nDataType == TTYPE_DEV_CHANNEL || m_pRefCurData->nDataType == TTYPE_DEV_DEVICE)
	{
		if (pDB->ExecuteSql(strDelDev) == FALSE)
			return 0;
	}
	// 연동데이터 입력에 해당 항목 삭제 - 메모리에서는 삭제 할 필요 없음 , CDataSystem에서 먼저 삭제함
	// m_ptrDeleteRelationLowerItems삭제 시 연도데이터 삭제됨
	if (pDB->ExecuteSql(strDelInputLink) == FALSE)
		return 0;

	// 연동 출력 데이터 삭제
	if (pDB->OpenQuery(strSelectOutput) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (pDB->GetFieldValue(L"SRC_FACP", nif) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"SRC_UNIT", niu) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"SRC_CHN", nic) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"SRC_RLY", nir) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"LINK_TYPE", nlnkType) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"TGT_FACP", ntf) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"TGT_UNIT", ntu) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"TGT_CHN", ntc) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"TGT_RLY", ntr) == FALSE)
			continue;
		if (nif <= 0 || niu <= 0 || nic <= 0 || nir <= 0
			|| ntf <= 0 || ntu <= 0 || ntc <= 0 || ntr <= 0 || nlnkType <= 0
			)
			continue;

		pRm = new ST_REMOVE_LINKED;
		pRm->nSrcFacp = nif;
		pRm->nSrcUnit = niu;
		pRm->nSrcChn = nic;
		pRm->nSrcDev = nir;
		pRm->nTgtFacp = ntf;
		pRm->nTgtUnit = ntu;
		pRm->nTgtChn = ntc;
		pRm->nTgtDev = ntr;
		pRm->btLinkType = nlnkType;
		m_ptrDeleteRelationOutputLinkItems.AddTail(pRm);
	}
	if (pDB->ExecuteSql(strDelOutput) == FALSE)
		return 0;

	// Pattern 삭제
	if (pDB->OpenQuery(strSelectPattern) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (pDB->GetFieldValue(L"PT_ID", nPid) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"FACP_ID", nif) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"UNIT_ID", niu) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"CHN_ID", nic) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"RLY_ID", nir) == FALSE)
			continue;
		if (nif <= 0 || niu <= 0 || nic <= 0 || nir <= 0 || nPid <= 0
			)
			continue;

		pRm = new ST_REMOVE_LINKED;
		memset((void*)pRm, 0, sizeof(ST_REMOVE_LINKED));
		pRm->nSrcFacp = nPid;
		pRm->nTgtFacp = nif;
		pRm->nTgtUnit = niu;
		pRm->nTgtChn = nic;
		pRm->nTgtDev = nir;
		m_ptrDeleteRelationPatternItems.AddTail(pRm);
	}
	if (pDB->ExecuteSql(strDelPattern) == FALSE)
		return 0;

	// PUmp 삭제
	if (pDB->OpenQuery(strSelectPump) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (pDB->GetFieldValue(L"PMP_ID", nPid) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"FACP_ID", nif) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"UNIT_ID", niu) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"CHN_ID", nic) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"RLY_ID", nir) == FALSE)
			continue;
		if (nif <= 0 || niu <= 0 || nic <= 0 || nir <= 0 || nPid <= 0
			)
			continue;

		pRm = new ST_REMOVE_LINKED;
		memset((void*)pRm, 0, sizeof(ST_REMOVE_LINKED));
		pRm->nSrcFacp = nPid;
		pRm->nTgtFacp = nif;
		pRm->nTgtUnit = niu;
		pRm->nTgtChn = nic;
		pRm->nTgtDev = nir;
		m_ptrDeleteRelationPumpItems.AddTail(pRm);
	}
	if (pDB->ExecuteSql(strDelPump) == FALSE)
		return 0;

	// 압력 스위치
	if (pDB->OpenQuery(strSelectPSwich) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (pDB->GetFieldValue(L"PS_ID", nPid) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"SRC_FACP", nSrcFacp) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"FACP_ID", nif) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"UNIT_ID", niu) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"CHN_ID", nic) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"RLY_ID", nir) == FALSE)
			continue;
		if (nif <= 0 || niu <= 0 || nic <= 0 || nir <= 0 || nPid <= 0
			)
			continue;

		pRm = new ST_REMOVE_LINKED;
		memset((void*)pRm, 0, sizeof(ST_REMOVE_LINKED));
		pRm->nSrcFacp = nSrcFacp;
		pRm->nSrcUnit = nPid;
		pRm->nTgtFacp = nif;
		pRm->nTgtUnit = niu;
		pRm->nTgtChn = nic;
		pRm->nTgtDev = nir;
		m_ptrDeleteRelationPSwitchItems.AddTail(pRm);
	}
	if (pDB->ExecuteSql(strDelPSwich) == FALSE)
		return 0;
	return 1;
}


void CSysLinkerView::OnBnClickedBtnSave2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nRet = 0;
	CString strAction, strMessage;
	if (m_pPage[m_nCurType]->GetChangeData() == FALSE)
		return;

	strMessage = L"";

#ifndef ENGLISH_MODE
	if (m_nAction == DATA_ADD)
	{
		nRet = DataAdd();
		strAction = L"추가하는데 ";
	}
	else
	{
		nRet = DataSave();
		strAction = L"저장하는데 ";
	}

	strMessage = strAction;
	if (nRet <= 0)
	{
		strMessage += L"실패했습니다";
		AfxMessageBox(strMessage);
	}
	else
	{
		strMessage += L"성공했습니다";
		AfxMessageBox(strMessage);
		AddCancel();
	}
#else
	if (m_nAction == DATA_ADD)
	{
		nRet = DataAdd();
	}
	else
	{
		nRet = DataSave();
	}

	if (nRet <= 0)
	{
		if (m_nAction == DATA_ADD)
		{
			strMessage = L"Failed to add.";
		}
		else
		{
			strMessage = L"Failed to save.";
		}
	}
	else
	{
		if (m_nAction == DATA_ADD)
		{
			strMessage = L"Successfully added.";
		}
		else
		{
			strMessage += L"Successfully saved.";
			AddCancel();
		}
	}

	AfxMessageBox(strMessage);
#endif
		
}


CDataLinked * CSysLinkerView::InsertMultiInputLink(BOOL bFirst
	, int nTgtFacp, int nTgtUnit, int nTgtChn, int nTgtDev
	, byte btLinkType, byte btLogicType, int nLogicID
)
{
	POSITION pos;
	CDataDevice * pDev;
	CDataSystem * pSys;
	CDataLinked * pLink = nullptr;
	ST_TREEITEM * pItem; 
	if (m_pRefPtrSelectedItems == nullptr)
		return 0; 

	pos = m_pRefPtrSelectedItems->GetHeadPosition();
	while (pos)
	{
		pItem = (ST_TREEITEM *)m_pRefPtrSelectedItems->GetNext(pos);
		if (pItem == nullptr || pItem->pData == nullptr)
			continue; 
		if (pItem->nDataType != TTYPE_DEV_DEVICE)
			continue;
		pSys = (CDataSystem*)pItem->pData;
		if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
			continue; 
		pDev = (CDataDevice*)pSys->GetSysData();

		// [KHS 2020-10-7 15:21:16] 
		// Device에 이미 있으면 넣지 않는다, 연동타입만 수동으로 변경
		pLink = pDev->GetLink(btLinkType, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev, FALSE);
		if (pLink == nullptr)
		{
			pLink = new CDataLinked;
			pLink->SetLinkData(nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
				, btLinkType, btLogicType, nLogicID
				, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
			);

			m_pRefFasSysData->InsertLinkedTable(pDev, pLink);
			pDev->AddLink(bFirst, pLink);
		}
		else
		{
			pLink->SetLinkData(nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
				, btLinkType, btLogicType, nLogicID
				, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
			);
			m_pRefFasSysData->ChangeLinkedTable(pDev, pLink);
		}
	}
	return pLink;
}

int CSysLinkerView::ChangeMultiInputIndivisualLinkOrder(CDataDevice* pDev)
{
// 	POSITION pos;
// 	CPtrList * pList;
// 	CDataLinked * pLnk;
// 	int nIdx = 0, nItem = -1;
// 	CString str;
// 	{
// 		pList = pDev->GetLinkedList();
// 		if (pList == nullptr)
// 			return;
// 		pos = pList->GetHeadPosition();
// 		while (pos)
// 		{
// 			pLnk = (CDataLinked*)pList->GetNext(pos);
// 			if (pLnk == nullptr)
// 				continue;
// 
// 			nItem = FindItemLink(LK_TYPE_RELEAY, pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
// 				, pLnk->GetTgtChn(), pLnk->GetTgtDev());
// 			if (nItem < 0)
// 				continue;
// 			str.Format(L"%03d", nIdx + 1);
// 			m_ctrlRelayList.SetItemText(nItem, 0, str);
// 			nIdx++;
// 		}
// 	}
// 	pList = pDev->GetLinkedList();
// 	if (pList == nullptr)
// 		continue;
// 
// 	pos = pList->GetHeadPosition();
// 	while (pos)
// 	{
// 		pLnk = (CDataLinked*)pList->GetNext(pos);
// 		if (pLnk == nullptr)
// 			continue;
// 
// 
// 		nItem = FindItemLink(LK_TYPE_RELEAY, pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
// 			, pLnk->GetTgtChn(), pLnk->GetTgtDev());
// 		if (nItem < 0)
// 			continue;
// 		str.Format(L"%03d", nIdx + 1);
// 		m_ctrlRelayList.SetItemText(nItem, 0, str);
// 		nIdx++;
// 	}
// 

	return 0;
}


void CSysLinkerView::OnBnClickedButtonCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 현재 선택된 입력 회로가 있는지 여부
	CDataDevice* pDataDevice = nullptr;
	pDataDevice = CManualLinkManager::Instance()->GetCurrentInputCircuit();
	if (pDataDevice == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("선택된 입력 회로가 없습니다."));
#else
		AfxMessageBox(_T("There is no input circuit selected."));
#endif
		return;
	}

	//
	CManualLinkManager::Instance()->SetRelayTableData(m_pRefFasSysData);

	// 현재까지 진행된 연동데이터 중 수동 연동데이터의 리스트를 얻음
	CManualLinkManager::Instance()->SetManualLinkListOnly();

	// 리스트에 표시할 정보와 수동 붙여쓰기 할 때에 수동 연동을 넣을 정보를 이 시점에 가공해야 한다. CManualLinkManager에서 해야 할 듯
	CManualLinkManager::Instance()->MakeCopyData();

	// 현재 복사된 목록 다이이얼로그 띄움
	if (m_pDlgManualCopy == nullptr)
	{
		CDlgManualCopy* pDlg = new CDlgManualCopy;
		m_pDlgManualCopy = pDlg;
#ifndef ENGLISH_MODE
		pDlg->Create(IDD_DIALOG_MANUAL_COPY);
#else
		pDlg->Create(IDD_DIALOG_MANUAL_COPY_EN);
#endif
		pDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		CDlgManualCopy* pDlg = m_pDlgManualCopy;
		pDlg->ShowWindow(SW_SHOW);
		pDlg->SetNewInfo();
	}
}


void CSysLinkerView::OnBnClickedButtonPaste()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 현재 선택된 입력 회로가 있는지 여부
	CPtrList* pList = nullptr;
	pList = CManualLinkManager::Instance()->GetManualLinkListOnly();
	if (pList->GetCount() == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("수동 출력이 없습니다."));
#else
		AfxMessageBox(_T("There is no manual output."));
#endif
		return;
	}

	m_ctrlRelayList.SetRedraw(FALSE);
	m_ctrlPatternList.SetRedraw(FALSE);

	BOOL bCrossCondition = FALSE;

	// 현재 입력 회로 정보를 얻어옴
	CDataDevice* pCurrentInputCircuit = nullptr;
	pCurrentInputCircuit = CManualLinkManager::Instance()->GetCurrentInputCircuit();
	
	// 수동 출력 리스트 가져옴
	std::vector<MANUAL_COPY_INFO> mci;
	std::vector<MANUAL_COPY_INFO>::iterator iter;

	mci = CManualLinkManager::Instance()->GetManualLinkVector();
	iter = mci.begin();
	for (; iter != mci.end(); iter++)
	{
		// 리스트에 이미 있다면 추가하지 않음
		int nAlreadyExists = -1;
		nAlreadyExists = FindItemLink(iter->nLinkType, iter->nTgtFacp, iter->nTgtUnit, iter->nTgtChn, iter->nTgtCircuit);
		if(nAlreadyExists >= 0)
			continue;

		// 크로스 조건 만족 여부
		bCrossCondition = CheckCrossCondition(pCurrentInputCircuit, iter);

		// UI및 메모리에 적용
		InsertManualOutputToList(bCrossCondition, pCurrentInputCircuit, iter);
	}

	// 리스트 화면 갱신 -> 참조한 소스에 있어서 추가했으나 동작하지 않는 것으로 보임
	m_ctrlRelayList.SetRedraw();
	m_ctrlPatternList.SetRedraw();
}

BOOL CSysLinkerView::CheckCrossCondition(CDataDevice* pDataDevice, std::vector<MANUAL_COPY_INFO>::iterator& iter)
{
	POSITION pos;
	CDataEquip* pEq;
	CString strEqName;

	// 크로스 입력 타입 여부 판단
	BOOL bCrossInputType = FALSE;
	CDataEquip* pInputEq = nullptr;
	pInputEq = pDataDevice->GetEqInput();
	int nInputType = pInputEq->GetEquipID();

	if (nInputType == INTYPE_CROSSA || nInputType == INTYPE_CROSSB
		|| nInputType == INTYPE_CROSS16_A || nInputType == INTYPE_CROSS17_B
		|| nInputType == INTYPE_CROSS18_A || nInputType == INTYPE_CROSS19_B
		)
	{
		// 입력 회로가 Cross 조건 대상이면 출력에 대한 조건도 검사
		std::shared_ptr<CManagerEquip> pRefOutputEquipManager;
		pRefOutputEquipManager = m_pRefFasSysData->m_spRefOutputEquipManager;

		// 프리액션 여부
		int nOutputType = -1;
		pos = pRefOutputEquipManager->GetHeadPosition();
		while (pos)
		{
			pEq = pRefOutputEquipManager->GetNext(pos);
			strEqName = pEq->GetEquipName();

			if (iter->strOutputType.Compare(strEqName) == 0)
			{
				nOutputType = pEq->GetEquipID();
				break;
			}
		}

		if (nOutputType == OUTTYPE_PREACTION)
		{
			// 밸브 여부
			std::shared_ptr<CManagerEquip> pRefContentsEquipManager;
			pRefContentsEquipManager = m_pRefFasSysData->m_spRefContentsEquipManager;

			int nOutputContent = -1;
			pos = pRefContentsEquipManager->GetHeadPosition();
			while (pos)
			{
				pEq = pRefContentsEquipManager->GetNext(pos);
				strEqName = pEq->GetEquipName();

				if (iter->strOutputDecription.Compare(strEqName) == 0)
				{
					nOutputContent = pEq->GetEquipID();
					break;
				}
			}

			if (nOutputContent == OUTCONT_VALVE)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CSysLinkerView::InsertManualOutputToList(BOOL bCrossCondition, CDataDevice* pDataDevice, std::vector<MANUAL_COPY_INFO>::iterator& iter)
{
	int nListCount = -1;
	CString strIndex = _T("");
	COLORREF crText = RGB(255, 255, 255), crBk = RGB(255, 0, 0);

	// 메모리 및 DB에 적용
	CDataLinked* pDataLinked;
	pDataLinked = new CDataLinked;
	pDataLinked->SetLinkData(iter->nTgtFacp, iter->nTgtUnit, iter->nTgtChn, iter->nTgtCircuit, iter->nLinkType, iter->nLogicType, 0, pDataDevice->GetFacpID(), pDataDevice->GetUnitID(), pDataDevice->GetChnID(), pDataDevice->GetDeviceID());
	m_pRefFasSysData->InsertLinkedTable(pDataDevice, pDataLinked);
	pDataDevice->AddLink(bCrossCondition, pDataLinked);

	// 타입에 따른 분기
	if (iter->nLinkType == LK_TYPE_RELEAY)
	{
		nListCount = m_ctrlRelayList.GetItemCount();

		m_ctrlRelayList.InsertItem(nListCount, L"", crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 1, iter->strFullName, crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 2, iter->strInputType, crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 3, iter->strOutputType, crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 4, iter->strEquipmentName, crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 5, iter->strEquipmentNumber, crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 6, iter->strOutputDecription, crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 7, iter->strPosition, crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 8, g_szLogicTypeString[LOGIC_MANUAL], crText, crBk);
		m_ctrlRelayList.SetItemText(nListCount, 9, L"", crText, crBk);

		m_ctrlRelayList.SetItemData(nListCount, (DWORD_PTR)pDataLinked);

		// Cross 조건에 의해 연동된 출력이 맨 앞에 있을 경우 이를 보정해서 순번 숫자를 매기고 현재는 맨 아래있지만 다시 그려질 때는 순서대로 표시가 됨
		ChangeRelayEditNumber();	
	}
	else
	{
		//m_ctrlPatternList에서는 추가되면 가장 윗 칸에 추가됨
		m_ctrlPatternList.InsertItem(0, iter->strFullName, crText, crBk);
		m_ctrlPatternList.SetItemText(0, 1, iter->strContactType, crText, crBk);

		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pDataLinked);
	}
}
