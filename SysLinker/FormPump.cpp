// FormPump.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormPump.h"
#include "DataPump.h"
#include "DlgChildPump.h"
#include "DataFacp.h"
#include "ManagerPump.h"
#include "RelayTableData.h"
#include "DlgTreePane.h"
#include "DataLinked.h"
#include "../Common/Control/DTreeCtrl.h"
#include "DataPattern.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "DataDevice.h"
#include "DataEmBc.h"
#include "DataPS.h"
#include "DataPump.h"
#include "DataFacpRelay.h"

// CFormPump

IMPLEMENT_DYNCREATE(CFormPump, CView)

CFormPump::CFormPump()
	//: CView(IDD_FORMPUMP)
{
	m_pDlgRightTopPumpInfo = nullptr;
	m_pRefFasSysData = nullptr;
	m_pDlgLeftTopTreePane = nullptr;
	m_pCurrentData = nullptr;
	m_pChangeData = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
}

CFormPump::~CFormPump()
{
	RemoveAllData();
}
// 
// void CFormPump::DoDataExchange(CDataExchange* pDX)
// {
// 	CView::DoDataExchange(pDX);
// }
void CFormPump::OnDraw(CDC* /*pDC*/)
{
	//CMDIDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
}
BEGIN_MESSAGE_MAP(CFormPump, CView)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(UWM_CHILDPANE_TREESELCHANGE, CFormPump::OnChildPaneTreeSelChanged)
	ON_MESSAGE(UWM_CHILDPANE_SELDATACHANGE, CFormPump::OnChildPaneSelDataChanged)
	ON_TVN_DROPED_ITEM(IDC_PATTERNVIEW_TREE, OnTvnPatternDropedItem)
	ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE, OnTvnOutputDropedItem)
	ON_TVN_DROPED_ITEM(IDC_EMERGENCYVIEW_TREE, OnTvnEmergencyDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PUMPVIEW_TREE, OnTvnPumpDropedItem)
	ON_TVN_DROPED_ITEM(IDC_CONTACTFACP_TREE, OnTvnContactDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PSWITCHVIEW_TREE, OnTvnPSwitchDropedItem)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PTNLK_MENU_DELPTN, &CFormPump::OnPtnlkMenuDelptn)
	ON_COMMAND(ID_PTNLK_MENU_VIEWPTN, &CFormPump::OnPtnlkMenuViewptn)
	ON_COMMAND(ID_RLYLK_MENU_DELRLY, &CFormPump::OnRlylkMenuDelrly)
	ON_COMMAND(ID_RLYLK_MENU_VIEWRELAY, &CFormPump::OnRlylkMenuViewrelay)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CFormPump 진단입니다.
LRESULT CFormPump::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	if (wp == FORM_PRJ_OPEN || wp == FORM_PRJ_NEW)
	{
		InitForm();
	}
	return 1;
}
#ifdef _DEBUG
void CFormPump::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormPump::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormPump 메시지 처리기입니다.


void CFormPump::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//ResizeParentToFit();
	InitSplitter();
	m_pChangeData = new CDataPump;
#ifndef ENGLISH_MODE
	m_ctrlPatternList.InsertColumn(0, L"제어 접점", LVCFMT_LEFT, 150);
	m_ctrlPatternList.InsertColumn(1, L"접점 종류", LVCFMT_LEFT, 150);
#else
	m_ctrlPatternList.InsertColumn(0, L"CONTROL RELAY CONTACT", LVCFMT_LEFT, 150);
	m_ctrlPatternList.InsertColumn(1, L"RELAY CONTACT TYPE", LVCFMT_LEFT, 150);
#endif
	m_ctrlPatternList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

#ifndef ENGLISH_MODE
	m_ctrlRelayList.InsertColumn(0, L"출력이름", LVCFMT_LEFT, 300);
	m_ctrlRelayList.InsertColumn(1, L"입력타입", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(2, L"출력타입", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(3, L"설비명", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(4, L"설비번호", LVCFMT_LEFT, 50);
	m_ctrlRelayList.InsertColumn(5, L"출력설명", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(6, L"위치", LVCFMT_LEFT, 150);
	m_ctrlRelayList.InsertColumn(7, L"연동타입", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(8, L"출력종류", LVCFMT_LEFT, 80);
#else
	m_ctrlRelayList.InsertColumn(0, L"OUTPUT NAME", LVCFMT_LEFT, 300);
	m_ctrlRelayList.InsertColumn(1, L"INPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(2, L"OUTPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(3, L"EQUIPMENT NAME", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(4, L"EQUIPMENT NO.", LVCFMT_LEFT, 50);
	m_ctrlRelayList.InsertColumn(5, L"OUTPUT DESCRIPTION", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(6, L"LOCATION", LVCFMT_LEFT, 150);
	m_ctrlRelayList.InsertColumn(7, L"INTERLOCK TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(8, L"OUTPUT CATEGORY", LVCFMT_LEFT, 80);
#endif
	m_ctrlRelayList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRelayList);

		m_pRefFasSysData = theApp.GetRelayTableData();
		
	}
	if (m_pDlgLeftTopTreePane)
		m_pDlgLeftTopTreePane->SetMainWindow(this);
	if (m_pDlgRightTopPumpInfo)
	{
		m_pDlgRightTopPumpInfo->SetMainWindow(this);
		m_pDlgRightTopPumpInfo->SetChangeData(m_pChangeData);
	}
	theApp.SetFormViewInitComplete(FV_PUMP);

}

int CFormPump::InitSplitter()
{
	SetSplitterPos();
	SetSplitterPos();
	m_SpMain.AddRow();
	m_SpMain.AddColumn();
	m_SpMain.AddColumn();
	m_SpMain.SetWindow(0, 0, m_SpLeft.m_hWnd);
	m_SpMain.SetWindow(0, 1, m_SpRight.m_hWnd);
	m_SpMain.SetMinHeight(20);

	m_SpLeft.AddRow();
	m_SpLeft.AddRow();
	m_SpLeft.AddColumn();
	m_SpLeft.SetWindow(0, 0, m_pDlgLeftTopTreePane->GetSafeHwnd());
	m_SpLeft.SetWindow(1, 0, m_pDlgRightTopPumpInfo->GetSafeHwnd());
	m_SpLeft.SetMinHeight(20);


	m_SpRight.AddRow();
	m_SpRight.AddRow();
	m_SpRight.AddColumn();
	m_SpRight.SetWindow(0, 0, m_ctrlPatternList.m_hWnd);
	m_SpRight.SetWindow(1, 0, m_ctrlRelayList.m_hWnd);
	m_SpRight.SetMinHeight(20);

	m_SpMain.SetWidthForStatic(0, 350);

	m_SpLeft.SetHeightForStatic(0, 400);
	m_SpLeft.SetHeightForStatic(1, 400);

	m_SpRight.SetHeightForStatic(0, 400);
	m_SpRight.SetHeightForStatic(1, 400);

	m_SpMain.Update();
	return 0;
}


int CFormPump::SetSplitterPos()
{
	if (m_SpMain.GetSafeHwnd() == nullptr)
		return 0;

	CRect rc, rcDev;
	GetClientRect(&rc);
	rc.top = rc.top + 32;
	rc.DeflateRect(5, 0, 5, 5);
	m_SpMain.MoveWindow(&rc);
	m_SpLeft.Update();
	m_SpRight.Update();
	m_SpMain.Update();

	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.R();
	// 
	// 	if (m_ctrlPatternList.GetSafeHwnd())
	// 		m_ctrlPatternList.RedrawWindow();
	return 0;
}



BOOL CFormPump::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CView::PreCreateWindow(cs);
}


void CFormPump::OnClose()
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
	}
	CView::OnClose();
}


int CFormPump::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (m_SpMain.Create(this, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), AFX_IDW_PANE_FIRST) == false ||
		m_SpLeft.Create(&m_SpMain, WS_CHILD | WS_VISIBLE, CRect(0, 0, 600, 600), 200) == false ||
		m_SpRight.Create(&m_SpMain, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), 201) == false)
		return -1;

	if (m_pDlgLeftTopTreePane == nullptr)
		m_pDlgLeftTopTreePane = new CDlgTreePane;

	if (!m_pDlgLeftTopTreePane->CreateDlg(2, &m_SpLeft)) //2 : child pane - pump
		return -1;
	m_pDlgLeftTopTreePane->ShowWindow(SW_SHOW);

	if (!m_ctrlPatternList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 800, 100), &m_SpRight, IDC_PATTERN_LIST))
		return -1;

	if (!m_ctrlRelayList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 200, 200), &m_SpRight, IDC_RELAY_LIST))
		return  -1;


	if(m_pDlgRightTopPumpInfo == nullptr)
		m_pDlgRightTopPumpInfo = new CDlgChildPump;

	if (!m_pDlgRightTopPumpInfo->CreateDlg(&m_SpLeft))
		return -1;
	m_pDlgRightTopPumpInfo->ShowWindow(SW_SHOW);
	return 0;
}


void CFormPump::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetSplitterPos();
}


void CFormPump::RemoveAllData()
{
	//20240321 GBM start - 메모리 누수 오류 수정
	if (m_pDlgLeftTopTreePane != nullptr)
	{	
		delete m_pDlgLeftTopTreePane;
		m_pDlgLeftTopTreePane = nullptr;
	}

	if (m_pDlgRightTopPumpInfo != nullptr)
	{
		delete m_pDlgRightTopPumpInfo;
		m_pDlgRightTopPumpInfo = nullptr;
	}

	if (m_pChangeData != nullptr)
	{
		delete m_pChangeData;
		m_pChangeData = nullptr;
	}

	//20240724 GBM start - m_pCurrentData는 RelayTableData 소멸자에서 일괄 정리되므로 여기서는 하지 않음
// 	if (m_pCurrentData != nullptr)
// 	{
// 		delete m_pCurrentData;
// 		m_pCurrentData = nullptr;
// 	}
	//20240321 GBM end
}
// 
// 
// int CFormPump::FillPumpTree()
// {
// 	m_ctrlLeftTopPump.DeleteAllItems();
// 	HTREEITEM hRoot = m_ctrlLeftTopPump.InsertItem(L"펌프 연동" ,0,0, TVI_ROOT);
// 	m_ctrlLeftTopPump.SetItemData(hRoot, 0);
// 	if (m_pRefFasSysData == nullptr)
// 		return 0; 
// 	std::shared_ptr<CManagerPump> spPump;
// 	std::map<int, CDataFacp*> * pMapFacp;
// 	std::map<int, CDataFacp*>::iterator it;
// 	CDataFacp * pFacp;
// 	CDataPump * pData;
// 	POSITION pos; 
// 	HTREEITEM hFacp[D_MAX_FACP_COUNT + 1] = { nullptr };
// 	HTREEITEM hItem;
// 	pMapFacp = m_pRefFasSysData->GetFacpMap();
// 	if (pMapFacp == nullptr)
// 		return 0; 
// 	for (it = pMapFacp->begin(); it != pMapFacp->end(); it++)
// 	{
// 		pFacp = it->second;
// 		if (pFacp == nullptr)
// 			continue;
// 		hFacp[pFacp->GetFacpID()] = m_ctrlLeftTopPump.InsertItem(pFacp->GetFacpName(), 1, 1, hRoot);
// 		m_ctrlLeftTopPump.SetItemData(hFacp[pFacp->GetFacpID()], 0);
// 	}
// 
// 	spPump = m_pRefFasSysData->GetPumpManager();
// 	if (spPump == nullptr)
// 		return 0;
// 	pos = spPump->GetHeadPosition();
// 	while (pos)
// 	{
// 		pData = spPump->GetNext(pos);
// 		if (pData == nullptr)
// 			continue; 
// 		hItem = m_ctrlLeftTopPump.InsertItem(pData->GetPumpName(), 2, 2, hFacp[pData->GetFacpID()]);
// 		m_ctrlLeftTopPump.SetItemData(hFacp[pData->GetFacpID()], (DWORD)pData);
// 	}
// 	
// 	return 1;
// }
void CFormPump::InitList()
{
	m_ctrlPatternList.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
	m_pCurrentData = nullptr;
}

LRESULT CFormPump::OnChildPaneTreeSelChanged(WPARAM wp, LPARAM lp)
{
// 	if (lp == 0)
// 		return 0;
	m_ctrlPatternList.SetRedraw(FALSE);
	m_ctrlRelayList.SetRedraw(FALSE);
	m_ctrlPatternList.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
	if (m_pDlgRightTopPumpInfo)
		m_pDlgRightTopPumpInfo->InitData();
	
	CDataPump * pData;
	//CDataPump * pPmp = nullptr;
	CDataFacpRelay * pContact = nullptr;
	CDataDevice * pDev;
	CDataPattern * pPtn;
	CPtrList * pLIst;
	POSITION pos;
	CDataLinked * pLink;
	ST_TREEPANE_DATA * pTData;
	int nPIdx = 0, nRIdx = 0;
	pTData = (ST_TREEPANE_DATA *)lp;
	if (pTData == nullptr || pTData->nType != TTYPE_DEV_PUMP)
	{
		m_ctrlPatternList.SetRedraw();
		m_ctrlRelayList.SetRedraw(); 
		return 0;
	}
	
	pData = (CDataPump *)pTData->pData;
	m_pCurrentData = (CDataPump *)pTData->pData;
	if (m_pCurrentData == nullptr)
	{
		m_ctrlPatternList.SetRedraw();
		m_ctrlRelayList.SetRedraw();
		return 0;
	}
	if (m_pDlgRightTopPumpInfo)
		m_pDlgRightTopPumpInfo->SetPumpInfo((CDataPump*)m_pCurrentData);// , m_pChangeData);
	pLIst = pData->GetLinkList();
	pos = pLIst->GetHeadPosition();
	while (pos)
	{
		pLink = (CDataLinked*)pLIst->GetNext(pos);
		if (pLink == nullptr)
			continue;
		switch (pLink->GetLinkType())
		{
		case LK_TYPE_PUMP:
// 			pPmp = m_pRefFasSysData->GetPump(pLink->GetTgtFacp(), pLink->GetTgtUnit());
// 			if (pPmp == nullptr)
// 				continue;
// 			m_ctrlPatternList.InsertItem(nPIdx, pPmp->GetPumpName());
// 
// 			m_ctrlPatternList.SetItemText(nPIdx, 1, g_szLinkTypeString[LK_TYPE_PUMP]);
// 			m_ctrlPatternList.SetItemData(nPIdx, (DWORD_PTR)pLink);
// 			nPIdx++;
			continue;
		case LK_TYPE_PATTERN:
			pPtn = m_pRefFasSysData->GetPattern(pLink->GetTgtFacp());
			if (pPtn == nullptr)
				continue;
			m_ctrlPatternList.InsertItem(nPIdx, pPtn->GetPatternName());

			m_ctrlPatternList.SetItemText(nPIdx, 1, g_szPatternTypeString[pPtn->GetPatternType()]);
			m_ctrlPatternList.SetItemData(nPIdx, (DWORD_PTR)pLink);
			nPIdx++;
			continue;
		case LK_TYPE_RELEAY:
			pDev = m_pRefFasSysData->GetDeviceByID(pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());
			if (pDev == nullptr)
				continue;
			m_ctrlRelayList.InsertItem(nRIdx, pDev->GetOutContentsFullName());
			m_ctrlRelayList.SetItemText(nRIdx, 1, pDev->GetInputTypeName());
			m_ctrlRelayList.SetItemText(nRIdx, 2, pDev->GetOutputTypeName());
			m_ctrlRelayList.SetItemText(nRIdx, 3, pDev->GetEquipName());
			m_ctrlRelayList.SetItemText(nRIdx, 4, pDev->GetEqAddIndex());
			m_ctrlRelayList.SetItemText(nRIdx, 5, pDev->GetOutContentsName());
			m_ctrlRelayList.SetItemText(nRIdx, 6, pDev->GetOutputLocationName());
			m_ctrlRelayList.SetItemText(nRIdx, 7, g_szLogicTypeString[pLink->GetLogicType()]);
#ifndef ENGLISH_MODE
			m_ctrlRelayList.SetItemText(nRIdx, 8, L"감지기/중계기");
#else
			m_ctrlRelayList.SetItemText(nRIdx, 8, L"DETECTOR/MODULE");
#endif
			m_ctrlRelayList.SetItemData(nRIdx, (DWORD_PTR)pLink);
			nRIdx++;
			continue;
		case LK_TYPE_FACP_RELAY:
			pContact = m_pRefFasSysData->GetContactFacp(pLink->GetTgtFacp(), pLink->GetTgtUnit());
			if (pContact == nullptr)
				continue;
			m_ctrlPatternList.InsertItem(nPIdx, pContact->GetFRelayName());

			m_ctrlPatternList.SetItemText(nPIdx, 1, g_szLinkTypeString[LK_TYPE_FACP_RELAY]);
			m_ctrlPatternList.SetItemData(nPIdx, (DWORD_PTR)pLink);
			nPIdx++;
			continue;
		}
// 		if (LK_TYPE_RELEAY != pLink->GetLinkType())
// 		{
// 			pPtn = m_pRefFasSysData->GetPattern(pLink->GetTgtFacp());
// 			if (pPtn == nullptr)
// 				continue;
// 			m_ctrlPatternList.InsertItem(0, pPtn->GetPatternName());
// 
// 			m_ctrlPatternList.SetItemText(0, 1, g_szPatternTypeString[pPtn->GetPatternType()]);
// 			m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
// 		}
// 		else
// 		{
// 			pDev = m_pRefFasSysData->GetDeviceByID(pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());
// 			if (pDev == nullptr)
// 				continue;
// 			
// 			m_ctrlRelayList.InsertItem(0, pDev->GetOutContentsFullName());
// 			m_ctrlRelayList.SetItemText(0, 1, pDev->GetInputTypeName());
// 			m_ctrlRelayList.SetItemText(0, 2, pDev->GetOutputTypeName());
// 			m_ctrlRelayList.SetItemText(0, 3, pDev->GetEquipName());
// 			m_ctrlRelayList.SetItemText(0, 4, pDev->GetEqAddIndex());
// 			m_ctrlRelayList.SetItemText(0, 5, pDev->GetOutContentsName());
// 			m_ctrlRelayList.SetItemText(0, 6, pDev->GetOutputLocationName());
// 			m_ctrlRelayList.SetItemText(0, 7, g_szLogicTypeString[pLink->GetLogicType()]);
// 			m_ctrlRelayList.SetItemText(0, 8, L"감지기/중계기");
// 			m_ctrlRelayList.SetItemData(0, (DWORD_PTR)pLink);
// 		}
	}
	m_ctrlPatternList.SetRedraw();
	m_ctrlRelayList.SetRedraw();
	return 1;
}

LRESULT CFormPump::OnChildPaneSelDataChanged(WPARAM wp, LPARAM lp)
{
	CDataPump * pData;
	if (lp == 0)
		return 0;
	pData = (CDataPump *)lp;
	switch (wp)
	{
	case DATA_ADD:
		DataAdd(pData);
		break;
	case DATA_SAVE:
		DataSave(pData);
		break;
	case DATA_DEL:
		DataDelete(pData);
		m_pCurrentData = nullptr;
		break;
	}

	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_DKP_PUMP_REFRESH, DATA_ALL, 0);
	return 1;
}

void CFormPump::OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;
	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	m_ctrlRelayList.SetRedraw(FALSE); 
	
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataSystem * pSys;
	CDataDevice * pDev;
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

		m_ctrlRelayList.InsertItem(0, pDev->GetOutContentsFullName());
		m_ctrlRelayList.SetItemText(0, 1, pDev->GetInputTypeName());
		m_ctrlRelayList.SetItemText(0, 2, pDev->GetOutputTypeName());
		m_ctrlRelayList.SetItemText(0, 3, pDev->GetEquipName());
		m_ctrlRelayList.SetItemText(0, 4, pDev->GetEqAddIndex());
		m_ctrlRelayList.SetItemText(0, 5, pDev->GetOutContentsName());
		m_ctrlRelayList.SetItemText(0, 6, pDev->GetOutputLocationName());
#ifndef ENGLISH_MODE
		m_ctrlRelayList.SetItemText(0, 7, L"수동");
		m_ctrlRelayList.SetItemText(0, 8, L"감지기/중계기");
#else
		m_ctrlRelayList.SetItemText(0, 7, L"MANUAL");
		m_ctrlRelayList.SetItemText(0, 8, L"DETECTOR/MODULE");
#endif

		pLink = new CDataLinked;
		pLink->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
			, LK_TYPE_RELEAY, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPumpID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlRelayList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();
	m_ctrlRelayList.SetRedraw();
}


void CFormPump::OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	m_ctrlRelayList.SetRedraw(FALSE);

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


		pPtn = (CDataPattern *)pData->pData;
		if (pPtn == nullptr)
			continue;

		nFind = FindItemLink(LK_TYPE_PATTERN, pPtn->GetPatternID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pPtn->GetPatternName());

		m_ctrlPatternList.SetItemText(0, 1, g_szPatternTypeString[pPtn->GetPatternType()]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pPtn->GetPatternID(), 0, 0, 0
			, LK_TYPE_PATTERN, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPumpID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();
	m_ctrlRelayList.SetRedraw();
}


void CFormPump::OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataEmBc * pItem;
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


		pItem = (CDataEmBc *)pData->pData;
		if (pItem == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_EMERGENCY, pItem->GetEmID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pItem->GetEmName());

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_EMERGENCY]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pItem->GetEmID(), 0, 0, 0
			, LK_TYPE_EMERGENCY, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPumpID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);

		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}


void CFormPump::OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
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


		pPm = (CDataPump *)pData->pData;
		if (pPm == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_PUMP, pPm->GetFacpID(), pPm->GetPumpID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pPm->GetPumpName());

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_PUMP]);


		pLink = new CDataLinked;
		pLink->SetLinkData(pPm->GetFacpID() , (int)pPm->GetPumpID(), 0, 0
			, LK_TYPE_PUMP, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPumpID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}

void CFormPump::OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
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


		pCont = (CDataFacpRelay *)pData->pData;
		if (pCont == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_FACP_RELAY, pCont->GetFacpID(), pCont->GetRelayID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pCont->GetFRelayName());

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_FACP_RELAY]);


		pLink = new CDataLinked;
		pLink->SetLinkData(pCont->GetFacpID(), (int)pCont->GetRelayID(), 0, 0
			, LK_TYPE_FACP_RELAY, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPumpID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}


void CFormPump::OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
#if _USE_PSWITCH_
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
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


		pPs = (CDataPS *)pData->pData;
		if (pPs == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_G_FLAG, pPs->GetFacpID(), pPs->GetPSwitchID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pPs->GetPSwitchName());

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_G_FLAG]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pPs->GetFacpID(), pPs->GetPSwitchID(), 0, 0
			, LK_TYPE_G_FLAG, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPumpID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

#endif
}


int CFormPump::InsertLink(CDataLinked * pLink)
{
	if (pLink == nullptr)
		return 0;
	if (m_pCurrentData == nullptr)
		return 0;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB == nullptr)
	{
		return 0;
	}

	CString strSql;
	int nCnt = 0;
	int nSrcFacp, nSrcUnit;
	int nTgtFacp, nTgtUnit, nTgtChn, nTgtDev;
	nSrcFacp = nSrcUnit = -1;
	nTgtFacp = nTgtUnit = nTgtChn = nTgtDev = -1;
	// 패턴은 모두 SOURCE DEVICE를 저장한다.

	if (pLink->GetLogicType() != LOGIC_ALL_AUTO)
	{
		nSrcFacp = m_pCurrentData->GetFacpID();
		nSrcUnit = m_pCurrentData->GetPumpID();
	}

	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_PUMP "
		L"WHERE SRC_FACP=%d AND PMP_ID=%d  "
		L"AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, nSrcFacp, nSrcUnit
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
	);

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	if (nCnt != 0)
		return 1;
	pDB->RSClose();
	strSql.Format(L"INSERT INTO TB_LINK_PUMP(SRC_FACP , PMP_ID "
		L", LINK_TYPE "
		L", FACP_ID , UNIT_ID , CHN_ID, RLY_ID "
		L", ADD_USER) "
		L" VALUES(%d,%d"
		L", %d"
		L", %d,%d,%d,%d"
		L", '%s')"
		, nSrcFacp, nSrcUnit
		, pLink->GetLinkType()
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
		, m_pRefFasSysData->GetCurrentUser()
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}

int CFormPump::DeleteLink(CDataLinked * pLink, YAdoDatabase * pDB)
{
	if (pLink == nullptr)
		return 0;
	if (m_pCurrentData == nullptr)
		return 0;
	if (pDB == nullptr)
	{
		return 0;
	}

	CString strSql;
	int nCnt = 0;
	int nSrcFacp, nSrcUnit;
	int nTgtFacp, nTgtUnit, nTgtChn, nTgtDev;
	nSrcFacp = nSrcUnit = -1;
	nTgtFacp = nTgtUnit = nTgtChn = nTgtDev = -1;
	// 패턴은 모두 SOURCE DEVICE를 저장한다.

	//if (pLink->GetLogicType() != LOGIC_ALL_AUTO)
	{
		nSrcFacp = m_pCurrentData->GetFacpID();
		nSrcUnit = m_pCurrentData->GetPumpID();
	}

	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_PUMP "
		L"WHERE SRC_FACP=%d AND PMP_ID=%d  "
		L"AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, nSrcFacp, nSrcUnit
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
	);

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	pDB->RSClose();
	if (nCnt == 0)
		return 1;

	strSql.Format(L"DELETE FROM TB_LINK_PUMP "
		L"WHERE SRC_FACP=%d AND PMP_ID=%d  "
		L"AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, nSrcFacp, nSrcUnit
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}

int CFormPump::InitForm()
{
	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		//AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRelayList);

		m_pRefFasSysData = theApp.GetRelayTableData();
	}

	if (m_pDlgRightTopPumpInfo)
		m_pDlgRightTopPumpInfo->InitDlg();

	if (m_pDlgLeftTopTreePane)
		m_pDlgLeftTopTreePane->InitDlg();
	return 0;
}


int CFormPump::DataAdd(CDataPump * pData)
{
	// 현재 선택된 데이터와 같으면
// 	if (pData == m_pCurrentData)
// 		return 0;
	if (m_pRefFasSysData == nullptr)
		return 0; 
	std::shared_ptr<CManagerPump> spPump;
	spPump = m_pRefFasSysData->GetPumpManager();
	if (spPump == nullptr)
		return 0;

	CString strSql;
	strSql.Format(L"INSERT INTO TB_PUMP_MST(NET_ID,FACP_ID,PMP_ID,PMP_TYPE,PMP_NAME,PMP_LCD,PMP_PCB,ADD_USER) "
		L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s')"
		, pData->GetFacpID(), pData->GetPumpID(), pData->GetPumpType()
		, pData->GetPumpName(), pData->GetPumpLcd(), pData->GetPcb()
		, m_pRefFasSysData->GetCurrentUser()
	);

	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if (pDb == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
#else
		AfxMessageBox(L"The project database has not been connected.");
#endif
		return 0;
	}
	if (pDb->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 입력하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to enter the pump information into the project database.");
#endif
		return 0;
	}

	CDataPump * pNewData = new CDataPump;
	pNewData->CopyData(pData);
	if (m_pDlgLeftTopTreePane->AddTreeData(pNewData) <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 입력하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to enter the pump information into the project database.");
#endif
		return 0;
	}
	spPump->AddTail(pNewData);
	return 1;
}

int CFormPump::DataSave(CDataPump * pData)
{
// 	if (pData == m_pCurrentData)
// 		return 0;

	CString strSql;
	int nCnt;
	BOOL bAdd = FALSE;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if (pDb == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
#else
		AfxMessageBox(L"The project database has not been connected.");
#endif
		return 0;
	}

	strSql.Format(L"SELECT * FROM TB_PUMP_MST WHERE FACP_ID=%d AND PMP_ID=%d"
		, pData->GetFacpID(), pData->GetPumpID()
	);

	if (pDb->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 저장하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to save the pump information in the project database.");
#endif
		return 0;
	}
	nCnt = pDb->GetRecordCount();
	if (nCnt > 0)
	{
		bAdd = FALSE;
		strSql.Format(L"UPDATE TB_PUMP_MST SET PMP_TYPE=%d,PMP_NAME='%s',PMP_LCD='%s',PMP_PCB=%d "
			L" WHERE FACP_ID=%d AND PMP_ID=%d "
			, pData->GetPumpType(), pData->GetPumpName(), pData->GetPumpLcd(), pData->GetPcb()
			, pData->GetFacpID(), pData->GetPumpID()
		);
	}
	else
	{
		bAdd = TRUE;
		strSql.Format(L"INSERT INTO TB_PUMP_MST(NET_ID,FACP_ID,PMP_ID,PMP_TYPE,PMP_NAME,PMP_LCD,PMP_PCB,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s')"
			, pData->GetFacpID(), pData->GetPumpID(), pData->GetPumpType()
			, pData->GetPumpName(), pData->GetPumpLcd(), pData->GetPcb()
			, m_pRefFasSysData->GetCurrentUser()
		);
	}

	if (pDb->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 저장하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to save the pump information in the project database.");
#endif
		return 0;
	}

	if (bAdd)
	{
		CDataPump * pNewData = new CDataPump;
		pNewData->CopyData(pData);
		if (m_pDlgLeftTopTreePane->AddTreeData(pNewData) <= 0)	//20240402 GBM - ">" -> "<="로 변경
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 저장하는데 실패했습니다.");
#else
			AfxMessageBox(L"Failed to save the pump information in the project database.");
#endif
			return 0;
		}
		std::shared_ptr<CManagerPump> spManager = m_pRefFasSysData->GetPumpManager();
		if (spManager)
			spManager->AddTail(pNewData);
	}
	else
	{
		if (m_pDlgLeftTopTreePane->ChangeTreeData(pData) <= 0)
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 저장하는데 실패했습니다.");
#else
			AfxMessageBox(L"Failed to save the pump information in the project database.");
#endif
			return 0;
		}
	}
	return 1;
}

int CFormPump::DataDelete(CDataPump * pData)
{
// 	if (pData == m_pCurrentData)
// 		return 0;
	CString strSql;
	int nCnt;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if (pDb == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
#else
		AfxMessageBox(L"The project database has not been connected.");
#endif
		return 0;
	}

	strSql.Format(L"SELECT * FROM TB_PUMP_MST WHERE FACP_ID=%d AND PMP_ID=%d"
		, pData->GetFacpID(), pData->GetPumpID()
	);

	if (pDb->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}
	nCnt = pDb->GetRecordCount();
	if (nCnt <= 0)
	{
		return 1;
	}
	strSql.Format(L"DELETE FROM TB_PUMP_MST WHERE FACP_ID=%d AND PMP_ID=%d"
		, pData->GetFacpID(), pData->GetPumpID()
	);

	if (pDb->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}

	if (m_pDlgLeftTopTreePane->DeleteTreeData(pData) <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}

	std::shared_ptr<CManagerPump> spManager = m_pRefFasSysData->GetPumpManager();
	if (spManager == nullptr)
		return 0;
	spManager->RemoveData(pData);
	delete pData;
	pData = nullptr;
	return 1;
}



void CFormPump::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CMenu menu, *pContext;
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
		nIdx = m_ctrlPatternList.HitTest(ptHit, nullptr);
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


void CFormPump::OnPtnlkMenuDelptn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nIdx, nRet = -1;
	nIdx = m_ctrlPatternList.GetNextItem(-1, LVNI_SELECTED);
	if (nIdx < 0)
		return;

#ifndef ENGLISH_MODE
	if (AfxMessageBox(L"패턴정보를 삭제하시겠습니까?", MB_YESNO) == IDNO)
		return;
#else
	if (AfxMessageBox(L"Do you want to delete the pattern information?", MB_YESNO) == IDNO)
		return;
#endif

	CString strSql;
	CDataLinked * pLink;
	YAdoDatabase * pDB;
	pLink = (CDataLinked*)m_ctrlPatternList.GetItemData(nIdx);
	if (pLink == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 패턴에 대한 정보를 가져오는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete. Failed to retrieve information about the pattern.");
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

	pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 프로젝트의 데이터베이스 정보가 잘못됐습니다.");
#else
		AfxMessageBox(L"Failed to delete. The project has invalid database information.");
#endif
		return;
	}

	if (DeleteLink(pLink, pDB) == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"연동된 패턴 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete interlocked pattern information.");
#endif
		return;
	}
	m_ctrlPatternList.DeleteItem(nIdx);
	m_pCurrentData->DeleteLink(pLink);
#ifndef ENGLISH_MODE
	AfxMessageBox(L"연동된 패턴 정보를 삭제하는데 성공했습니다.");
#else
	AfxMessageBox(L"Successfully deleted the interlock pattern information.");
#endif
}


void CFormPump::OnPtnlkMenuViewptn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CFormPump::OnRlylkMenuDelrly()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nRet = 0, nCnt, i;
	nCnt = m_ctrlRelayList.GetSelectedCount();
	if (nCnt < 0)
		return;

#ifndef ENGLISH_MODE
	if (AfxMessageBox(L"연동 출력을 삭제하시겠습니까?", MB_YESNO | MB_ICONQUESTION) == IDNO)
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
		AfxMessageBox(L"Failed to delete. Invalid project settings information.");
#endif
		return;
	}

	BOOL bError = FALSE;
	CString strSql, strError = L"";
	CDataLinked * pLink;
	POSITION pos;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();

	if (pDB == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 프로젝트의 데이터베이스 정보가 잘못됐습니다.");
#else
		AfxMessageBox(L"Failed to delete. The project has invalid database information.");
#endif
		return;
	}
	pos = m_ctrlRelayList.GetFirstSelectedItemPosition();
	std::vector<int> vtSel;


	while (pos)
	{
		int nSelected = m_ctrlRelayList.GetNextSelectedItem(pos);
		vtSel.push_back(nSelected);
	}

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

		if (DeleteLink(pLink, pDB) == 0)
		{
#ifndef ENGLISH_MODE
			strError = L"삭제하는데 실패했습니다. 데이터베이스에서 연동정보를 삭제하는데 실패했습니다.";
#else
			strError = L"Failed to delete. Failed to delete the interlocked information from the database.";
#endif
			bError = TRUE;
			break;
		}
	}

	if (bError == TRUE)
	{
		pDB->RollbackTransaction();
		AfxMessageBox(strError);
		vtSel.clear();
		return;
	}

	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		pLink = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		if (pLink == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
			strError = L"삭제하는데 실패했습니다. 연동 출력에 대한 정보를 가져오는데 실패했습니다.";
			bError = TRUE;
			break;;
		}
		m_pCurrentData->DeleteLink(pLink);
		m_ctrlRelayList.DeleteItem(vtSel[i]);
	}
	pDB->CommitTransaction();
	vtSel.clear();
#ifndef ENGLISH_MODE
	AfxMessageBox(L"연동 출력 정보를 삭제하는데 성공했습니다.");
#else
	AfxMessageBox(L"Successfully deleted the interlock output information.");
#endif
}


void CFormPump::OnRlylkMenuViewrelay()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


int CFormPump::FindItemLink(int nType, int nFacp, int nUnit, int nChn, int nRelay)
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


BOOL CFormPump::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, &m_brBackground);
	pDC->SetBkColor(m_crBack);
	pDC->SetBkMode(TRANSPARENT);
	return TRUE;

	//return CView::OnEraseBkgnd(pDC);
}
