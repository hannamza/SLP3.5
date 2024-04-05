// FormPSwitch.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormPSwitch.h"
#include "DlgChildPSwitch.h"
#include "DataPS.h"
#include "DataFacp.h"
#include "ManagerPS.h"
#include "RelayTableData.h"
#include "DlgTreePane.h"
#include "DataLinked.h"
#include "../Common/Control/DTreeCtrl.h"
#include "DataPattern.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "DataDevice.h"
#include "DataLinked.h"
#include "DataEmBc.h"
#include "DataPS.h"
#include "DataPump.h"
#include "DataFacpRelay.h"
// CFormPSwitch

IMPLEMENT_DYNCREATE(CFormPSwitch, CView)

CFormPSwitch::CFormPSwitch()
	//: CView(IDD_FORMPSWITCH)
{
	m_pDlgLeftBottomPSwitchInfo = nullptr;
	m_pRefFasSysData = nullptr;
	m_pDlgLeftTopTreePane = nullptr;
	m_pCurrentData = nullptr;
	m_pChangeData = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
}

CFormPSwitch::~CFormPSwitch()
{
	RemoveAllData();
	delete m_pChangeData;
	m_pChangeData = nullptr;
}
void CFormPSwitch::OnDraw(CDC* /*pDC*/)
{
	//CMDIDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
}
// void CFormPSwitch::DoDataExchange(CDataExchange* pDX)
// {
// 	CView::DoDataExchange(pDX);
// }

BEGIN_MESSAGE_MAP(CFormPSwitch, CView)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_MESSAGE(UWM_CHILDPANE_TREESELCHANGE, CFormPSwitch::OnChildPaneTreeSelChanged)
	ON_MESSAGE(UWM_CHILDPANE_SELDATACHANGE, CFormPSwitch::OnChildPaneSelDataChanged)
	ON_TVN_DROPED_ITEM(IDC_PATTERNVIEW_TREE, OnTvnPatternDropedItem)
	ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE, OnTvnOutputDropedItem)
	ON_TVN_DROPED_ITEM(IDC_EMERGENCYVIEW_TREE, OnTvnEmergencyDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PUMPVIEW_TREE, OnTvnPumpDropedItem)
	ON_TVN_DROPED_ITEM(IDC_CONTACTFACP_TREE, OnTvnContactDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PSWITCHVIEW_TREE, OnTvnPSwitchDropedItem)

	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PTNLK_MENU_DELPTN, &CFormPSwitch::OnPtnlkMenuDelptn)
	ON_COMMAND(ID_PTNLK_MENU_VIEWPTN, &CFormPSwitch::OnPtnlkMenuViewptn)
	ON_COMMAND(ID_RLYLK_MENU_DELRLY, &CFormPSwitch::OnRlylkMenuDelrly)
	ON_COMMAND(ID_RLYLK_MENU_VIEWRELAY, &CFormPSwitch::OnRlylkMenuViewrelay)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CFormPSwitch 진단입니다.
LRESULT CFormPSwitch::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	if (wp == FORM_PRJ_OPEN || wp == FORM_PRJ_NEW)
	{
		InitForm();
	}
	return 1;
}
#ifdef _DEBUG
void CFormPSwitch::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormPSwitch::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormPSwitch 메시지 처리기입니다.


void CFormPSwitch::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//ResizeParentToFit();
	InitSplitter();
	m_pChangeData = new CDataPS;
	m_ctrlRightTopPatternList.InsertColumn(0, L"제어 접점", LVCFMT_LEFT, 150);
	m_ctrlRightTopPatternList.InsertColumn(1, L"접점 종류", LVCFMT_LEFT, 150);
	m_ctrlRightTopPatternList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ctrlRightBottomRelayList.InsertColumn(0, L"출력이름", LVCFMT_LEFT, 300);
	m_ctrlRightBottomRelayList.InsertColumn(1, L"입력타입", LVCFMT_LEFT, 80);
	m_ctrlRightBottomRelayList.InsertColumn(2, L"출력타입", LVCFMT_LEFT, 80);
	m_ctrlRightBottomRelayList.InsertColumn(3, L"설비명", LVCFMT_LEFT, 80);
	m_ctrlRightBottomRelayList.InsertColumn(4, L"설비번호", LVCFMT_LEFT, 50);
	m_ctrlRightBottomRelayList.InsertColumn(5, L"출력설명", LVCFMT_LEFT, 80);
	m_ctrlRightBottomRelayList.InsertColumn(6, L"위치", LVCFMT_LEFT, 150);
	m_ctrlRightBottomRelayList.InsertColumn(7, L"연동타입", LVCFMT_LEFT, 80);
	m_ctrlRightBottomRelayList.InsertColumn(8, L"출력종류", LVCFMT_LEFT, 80);

// 	m_ctrlRightBottomRelayList.InsertColumn(0, L"감지기/중계기", LVCFMT_LEFT, 150);
// 	m_ctrlRightBottomRelayList.InsertColumn(1, L"주소", LVCFMT_LEFT, 150);
// 	m_ctrlRightBottomRelayList.InsertColumn(2, L"위치", LVCFMT_LEFT, 150);
// 	m_ctrlRightBottomRelayList.InsertColumn(3, L"로직타입", LVCFMT_LEFT, 150);
	m_ctrlRightBottomRelayList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
		
		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomRelayList);
		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);

		m_pRefFasSysData = theApp.GetRelayTableData();
	}

	if (m_pDlgLeftTopTreePane)
		m_pDlgLeftTopTreePane->SetMainWindow(this);
	if (m_pDlgLeftBottomPSwitchInfo)
	{
		m_pDlgLeftBottomPSwitchInfo->SetMainWindow(this);
		m_pDlgLeftBottomPSwitchInfo->SetChangeData(m_pChangeData);
	}
	theApp.SetFormViewInitComplete(FV_PSWITCH);

}


BOOL CFormPSwitch::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CView::PreCreateWindow(cs);
}

int CFormPSwitch::SetSplitterPos()
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


	// 	if (m_ctrlRightBottomRelayList.GetSafeHwnd())
	// 		m_ctrlRightBottomRelayList.R();
	// 
	// 	if (m_ctrlRightTopPatternList.GetSafeHwnd())
	// 		m_ctrlRightTopPatternList.RedrawWindow();
	return 0;
}

int CFormPSwitch::InitSplitter()
{
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
	m_SpLeft.SetWindow(1, 0, m_pDlgLeftBottomPSwitchInfo->GetSafeHwnd());
	m_SpLeft.SetMinHeight(20);


	m_SpRight.AddRow();
	m_SpRight.AddRow();
	m_SpRight.AddColumn();
	m_SpRight.SetWindow(0, 0, m_ctrlRightTopPatternList.m_hWnd);
	m_SpRight.SetWindow(1, 0, m_ctrlRightBottomRelayList.m_hWnd);
	m_SpRight.SetMinHeight(20);

	m_SpMain.SetWidthForStatic(0, 350);
	
	m_SpLeft.SetHeightForStatic(0, 400);
	m_SpLeft.SetHeightForStatic(1, 400);

	m_SpRight.SetHeightForStatic(0, 400);
	m_SpRight.SetHeightForStatic(1, 400);


	m_SpMain.Update();
	return 0;
}


int CFormPSwitch::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (m_SpMain.Create(this, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), AFX_IDW_PANE_FIRST) == false ||
		m_SpLeft.Create(&m_SpMain, WS_CHILD | WS_VISIBLE, CRect(0, 0, 600, 600), 200) == false ||
		m_SpRight.Create(&m_SpMain, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), 201) == false)
		return -1;

	if (m_pDlgLeftTopTreePane == nullptr)
		m_pDlgLeftTopTreePane = new CDlgTreePane;

	if (!m_pDlgLeftTopTreePane->CreateDlg(1, &m_SpLeft)) //1 : child pane - presure switch
		return -1;
	m_pDlgLeftTopTreePane->ShowWindow(SW_SHOW);

	if (!m_ctrlRightTopPatternList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 600, 600), &m_SpRight, IDC_PATTERN_LIST))
		return -1;
	
	if (!m_ctrlRightBottomRelayList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 600, 600), &m_SpRight, IDC_RELAY_LIST))
		return  -1;

	if (m_pDlgLeftBottomPSwitchInfo == nullptr)
		m_pDlgLeftBottomPSwitchInfo = new CDlgChildPSwitch;

	if (!m_pDlgLeftBottomPSwitchInfo->CreateDlg(&m_SpLeft))
		return -1;
	m_pDlgLeftBottomPSwitchInfo->ShowWindow(SW_SHOW);
	return 0;
}

void CFormPSwitch::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRightBottomRelayList);

		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRightBottomRelayList);
	}
	CView::OnClose();
}


void CFormPSwitch::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	SetSplitterPos();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CFormPSwitch::RemoveAllData()
{
	//20240321 GBM start - 메모리 누수 오류 수정
	if (m_pDlgLeftTopTreePane != nullptr)
	{
		delete m_pDlgLeftTopTreePane;
		m_pDlgLeftTopTreePane = nullptr;
	}

	if (m_pDlgLeftBottomPSwitchInfo != nullptr)
	{
		delete m_pDlgLeftBottomPSwitchInfo;
		m_pDlgLeftBottomPSwitchInfo = nullptr;
	}

	// 펌프와는 달리 아래부분을 실행하면 종료 시 CRelayTableData::RemovePs()에서 프로그램 죽음
// 	if (m_pChangeData != nullptr)
// 	{
// 		delete m_pChangeData;
// 		m_pChangeData = nullptr;
// 	}
// 
// 	if (m_pCurrentData != nullptr)
// 	{
// 		delete m_pCurrentData;
// 		m_pCurrentData = nullptr;
// 	}
	//20240321 GBM end
}


LRESULT CFormPSwitch::OnChildPaneTreeSelChanged(WPARAM wp, LPARAM lp)
{
	//return 0;
// 	if (lp == 0)
// 		return 0; 
	m_ctrlRightTopPatternList.SetRedraw(FALSE);
	m_ctrlRightBottomRelayList.SetRedraw(FALSE);
	m_ctrlRightTopPatternList.DeleteAllItems();
	m_ctrlRightBottomRelayList.DeleteAllItems();
	
	if (m_pDlgLeftBottomPSwitchInfo)
		m_pDlgLeftBottomPSwitchInfo->InitData();

	CDataPS * pData;
	CDataDevice * pDev;
	CDataPattern * pPtn;
	CDataPump * pPmp = nullptr;
	CDataFacpRelay * pContact = nullptr;
	CPtrList * pLIst;
	POSITION pos;
	CDataLinked * pLink;
	ST_TREEPANE_DATA * pTData;
	int nPIdx = 0, nRIdx = 0; 
	pTData = (ST_TREEPANE_DATA *)lp;
	if (pTData == nullptr || pTData->nType != TTYPE_DEV_PSWITCH)
	{
		m_ctrlRightTopPatternList.SetRedraw();
		m_ctrlRightBottomRelayList.SetRedraw();
		return 0;
	}
	m_pCurrentData = (CDataPS*)pTData->pData;
	pData = (CDataPS*)pTData->pData;

	if (m_pCurrentData == nullptr)
	{
		m_ctrlRightTopPatternList.SetRedraw();
		m_ctrlRightBottomRelayList.SetRedraw();
		return 0;
	}
	if (m_pDlgLeftBottomPSwitchInfo)
		m_pDlgLeftBottomPSwitchInfo->SetPSwitch((CDataPS*)m_pCurrentData);
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
			pPmp = m_pRefFasSysData->GetPump(pLink->GetTgtFacp(), pLink->GetTgtUnit());
			if (pPmp == nullptr)
				continue;
			m_ctrlRightTopPatternList.InsertItem(nPIdx, pPmp->GetPumpName());

			m_ctrlRightTopPatternList.SetItemText(nPIdx, 1, g_szLinkTypeString[LK_TYPE_PUMP]);
			m_ctrlRightTopPatternList.SetItemData(nPIdx, (DWORD_PTR)pLink);
			nPIdx++;
			continue;
		case LK_TYPE_PATTERN:
			pPtn = m_pRefFasSysData->GetPattern(pLink->GetTgtFacp());
			if (pPtn == nullptr)
				continue;
			m_ctrlRightTopPatternList.InsertItem(nPIdx, pPtn->GetPatternName());

			m_ctrlRightTopPatternList.SetItemText(nPIdx, 1, g_szPatternTypeString[pPtn->GetPatternType()]);
			m_ctrlRightTopPatternList.SetItemData(nPIdx, (DWORD_PTR)pLink);
			nPIdx++;
			continue;
		case LK_TYPE_RELEAY:
			pDev = m_pRefFasSysData->GetDeviceByID(pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());
			if (pDev == nullptr)
				continue;
			m_ctrlRightBottomRelayList.InsertItem(nRIdx, pDev->GetOutContentsFullName());
			m_ctrlRightBottomRelayList.SetItemText(nRIdx, 1, pDev->GetInputTypeName());
			m_ctrlRightBottomRelayList.SetItemText(nRIdx, 2, pDev->GetOutputTypeName());
			m_ctrlRightBottomRelayList.SetItemText(nRIdx, 3, pDev->GetEquipName());
			m_ctrlRightBottomRelayList.SetItemText(nRIdx, 4, pDev->GetEqAddIndex());
			m_ctrlRightBottomRelayList.SetItemText(nRIdx, 5, pDev->GetOutContentsName());
			m_ctrlRightBottomRelayList.SetItemText(nRIdx, 6, pDev->GetOutputLocationName());
			m_ctrlRightBottomRelayList.SetItemText(nRIdx, 7, g_szLogicTypeString[pLink->GetLogicType()]);
			m_ctrlRightBottomRelayList.SetItemText(nRIdx, 8, L"감지기/중계기");
			m_ctrlRightBottomRelayList.SetItemData(nRIdx, (DWORD_PTR)pLink);
			nRIdx++;
			continue;
		case LK_TYPE_FACP_RELAY:
			pContact = m_pRefFasSysData->GetContactFacp(pLink->GetTgtFacp(),pLink->GetTgtUnit());
			if (pContact == nullptr)
				continue;
			m_ctrlRightTopPatternList.InsertItem(nPIdx, pContact->GetFRelayName());

			m_ctrlRightTopPatternList.SetItemText(nPIdx, 1, g_szLinkTypeString[LK_TYPE_FACP_RELAY]);
			m_ctrlRightTopPatternList.SetItemData(nPIdx, (DWORD_PTR)pLink);
			nPIdx++;
			continue;
		}
	}
	m_ctrlRightTopPatternList.SetRedraw();
	m_ctrlRightBottomRelayList.SetRedraw();
	return 1;
}

LRESULT CFormPSwitch::OnChildPaneSelDataChanged(WPARAM wp, LPARAM lp)
{
	CDataPS * pData;
	if (lp == 0)
		return 0; 
	pData = (CDataPS *)lp;
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
		AfxGetMainWnd()->SendMessage(UWM_DKP_PSWITCH_REFRESH, DATA_ALL, 0);
	return 1;
}


void CFormPSwitch::OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;
	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;
	m_ctrlRightBottomRelayList.SetRedraw(FALSE);

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
		m_ctrlRightBottomRelayList.InsertItem(0, pDev->GetOutContentsFullName());
		m_ctrlRightBottomRelayList.SetItemText(0, 1, pDev->GetInputTypeName());
		m_ctrlRightBottomRelayList.SetItemText(0, 2, pDev->GetOutputTypeName());
		m_ctrlRightBottomRelayList.SetItemText(0, 3, pDev->GetEquipName());
		m_ctrlRightBottomRelayList.SetItemText(0, 4, pDev->GetEqAddIndex());
		m_ctrlRightBottomRelayList.SetItemText(0, 5, pDev->GetOutContentsName());
		m_ctrlRightBottomRelayList.SetItemText(0, 6, pDev->GetOutputLocationName());
		m_ctrlRightBottomRelayList.SetItemText(0, 7, L"수동");
		m_ctrlRightBottomRelayList.SetItemText(0, 8, L"감지기/중계기");

		pLink = new CDataLinked;
		pLink->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
			, LK_TYPE_RELEAY, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPSwitchID(), -1, -1
		);

		if(InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlRightBottomRelayList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlRightBottomRelayList.SetRedraw();

}


void CFormPSwitch::OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlRightTopPatternList.SetRedraw(FALSE);
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

		m_ctrlRightTopPatternList.InsertItem(0, pPtn->GetPatternName());

		m_ctrlRightTopPatternList.SetItemText(0, 1, g_szPatternTypeString[pPtn->GetPatternType()]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pPtn->GetPatternID(), 0, 0, 0
			, LK_TYPE_PATTERN, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPSwitchID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlRightTopPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlRightTopPatternList.SetRedraw();

}

void CFormPSwitch::OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlRightTopPatternList.SetRedraw(FALSE);
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

		m_ctrlRightTopPatternList.InsertItem(0, pItem->GetEmName());

		m_ctrlRightTopPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_EMERGENCY]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pItem->GetEmID(), 0, 0, 0
			, LK_TYPE_EMERGENCY, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPSwitchID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);

		m_ctrlRightTopPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlRightTopPatternList.SetRedraw();

}


void CFormPSwitch::OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlRightTopPatternList.SetRedraw(FALSE);
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
		m_ctrlRightTopPatternList.InsertItem(0, pPm->GetPumpName());

		m_ctrlRightTopPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_PUMP]);


		pLink = new CDataLinked;
		pLink->SetLinkData(pPm->GetFacpID(), (int)pPm->GetPumpID(), 0, 0
			, LK_TYPE_PUMP, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPSwitchID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlRightTopPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlRightTopPatternList.SetRedraw();

}

void CFormPSwitch::OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentData == nullptr)
		return;

	m_ctrlRightTopPatternList.SetRedraw(FALSE);
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
		m_ctrlRightTopPatternList.InsertItem(0, pCont->GetFRelayName());

		m_ctrlRightTopPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_FACP_RELAY]);


		pLink = new CDataLinked;
		pLink->SetLinkData(pCont->GetFacpID(), (int)pCont->GetRelayID(), 0, 0
			, LK_TYPE_FACP_RELAY, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPSwitchID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlRightTopPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlRightTopPatternList.SetRedraw();

}


void CFormPSwitch::OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
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

	m_ctrlRightTopPatternList.SetRedraw(FALSE);
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

		m_ctrlRightTopPatternList.InsertItem(0, pPs->GetPSwitchName());

		m_ctrlRightTopPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_G_FLAG]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pPs->GetFacpID(), pPs->GetPSwitchID(), 0, 0
			, LK_TYPE_G_FLAG, LOGIC_MANUAL, 0
			, m_pCurrentData->GetFacpID(), m_pCurrentData->GetPSwitchID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			m_pCurrentData->AddLink(pLink);
		m_ctrlRightTopPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlRightTopPatternList.SetRedraw();

#endif
}

int CFormPSwitch::InsertLink(CDataLinked * pLink)
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
		nSrcUnit = m_pCurrentData->GetPSwitchID();
	}

	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_PSWITCH "
		L"WHERE SRC_FACP=%d AND PS_ID=%d  "
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
	strSql.Format(L"INSERT INTO TB_LINK_PSWITCH(SRC_FACP , PS_ID "
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


int CFormPSwitch::DeleteLink(CDataLinked * pLink , YAdoDatabase * pDB)
{
	if (pLink == nullptr)
		return 0;
	if (m_pCurrentData == nullptr)
		return 0;

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
		nSrcUnit = m_pCurrentData->GetPSwitchID();
	}

	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_PSWITCH "
		L"WHERE SRC_FACP=%d AND PS_ID=%d  "
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

	strSql.Format(L"DELETE FROM TB_LINK_PSWITCH "
		L"WHERE SRC_FACP=%d AND PS_ID=%d  "
		L"AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, nSrcFacp, nSrcUnit
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}

void CFormPSwitch::InitList()
{
	m_ctrlRightBottomRelayList.DeleteAllItems();
	m_ctrlRightTopPatternList.DeleteAllItems();
	m_pCurrentData = nullptr;
}

int CFormPSwitch::InitForm()
{
	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
		//AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomRelayList);


		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomRelayList);

		m_pRefFasSysData = theApp.GetRelayTableData();
	}

	if (m_pDlgLeftBottomPSwitchInfo)
		m_pDlgLeftBottomPSwitchInfo->InitDlg();

	if (m_pDlgLeftTopTreePane)
		m_pDlgLeftTopTreePane->InitDlg();
	return 0;
}


int CFormPSwitch::DataAdd(CDataPS * pData)
{
	if (pData == m_pCurrentData)
		return 0; 
	if (m_pRefFasSysData == nullptr)
		return 0;
	std::shared_ptr<CManagerPS> spPs;
	spPs = m_pRefFasSysData->GetPSwitchManager();
	if (spPs == nullptr)
		return 0;
	CString strSql;
	strSql.Format(L"INSERT INTO TB_PSWITCH_MST(NET_ID,FACP_ID,PS_ID,PS_TYPE,PS_NAME,PS_LCD,PS_PCB,ADD_USER) "
		L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s')"
		, pData->GetFacpID(), pData->GetPSwitchID() , pData->GetPSwitchType() 
		, pData->GetPSwitchName(), pData->GetPSwitchLcd(), pData->GetPcb()
		,m_pRefFasSysData->GetCurrentUser()
	);

	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if (pDb == nullptr)
	{
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
		return 0;
	}
	if (pDb->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
		return 0;
	}

	CDataPS * pNewData = new CDataPS;
	pNewData->CopyData(pData);
	if (m_pDlgLeftTopTreePane->AddTreeData(pNewData) <= 0)
	{
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
		return 0;
	}
	spPs->AddTail(pNewData);
	return 1;
}

int CFormPSwitch::DataSave(CDataPS * pData)
{
// 	if (pData == m_pCurrentData)
// 		return 0;

	CString strSql;
	int nCnt;
	BOOL bAdd = FALSE;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if (pDb == nullptr)
	{
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
		return 0;
	}

	strSql.Format(L"SELECT * FROM TB_PSWITCH_MST WHERE FACP_ID=%d AND PS_ID=%d"
		, pData->GetFacpID(), pData->GetPSwitchID()
	);

	if (pDb->OpenQuery(strSql) == FALSE)
	{
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
		return 0;
	}
	nCnt = pDb->GetRecordCount();
	if (nCnt > 0)
	{
		bAdd = FALSE;
		strSql.Format(L"UPDATE TB_PSWITCH_MST SET PS_TYPE=%d,PS_NAME='%s',PS_LCD='%s',PS_PCB=%d "
			L" WHERE FACP_ID=%d AND PS_ID=%d "
			, pData->GetPSwitchType(), pData->GetPSwitchName(), pData->GetPSwitchLcd(), pData->GetPcb()
			, pData->GetFacpID(), pData->GetPSwitchID()
		);
	}
	else
	{
		bAdd = TRUE;
		strSql.Format(L"INSERT INTO TB_PSWITCH_MST(NET_ID,FACP_ID,PS_ID,PS_TYPE,PS_NAME,PS_LCD,PS_PCB,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s')"
			, pData->GetFacpID(), pData->GetPSwitchID(), pData->GetPSwitchType()
			, pData->GetPSwitchName(), pData->GetPSwitchLcd(), pData->GetPcb()
			, m_pRefFasSysData->GetCurrentUser()
		);
	}

	if (pDb->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
		return 0;
	}
	
	if (bAdd)
	{
		CDataPS * pNewData = new CDataPS;
		pNewData->CopyData(pData);
		if (m_pDlgLeftTopTreePane->AddTreeData(pNewData) <= 0)	//20240402 GBM - ">" -> "<="로 변경
		{
			AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
			return 0;
		}
		std::shared_ptr<CManagerPS> spManager = m_pRefFasSysData->GetPSwitchManager();
		if (spManager)
			spManager->AddTail(pNewData);
	}
	else
	{
		if (m_pDlgLeftTopTreePane->ChangeTreeData(pData) <= 0)
		{
			AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
			return 0;
		}
	}
// 	
// 	if (pDb->ExecuteSql(strSql) == FALSE)
// 	{
// 		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
// 		return 0;
// 	}
// 
// 	if (m_pDlgLeftTopTreePane->AddTreeData(pData) > 0)
// 	{
// 		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
// 		return 0;
// 	}

	return 1;
}

int CFormPSwitch::DataDelete(CDataPS * pData)
{
// 	if (pData == m_pCurrentData)
// 		return 0;
	CString strSql;
	int nCnt;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if (pDb == nullptr)
	{
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
		return 0;
	}

	strSql.Format(L"SELECT * FROM TB_PSWITCH_MST WHERE FACP_ID=%d AND PS_ID=%d"
		, pData->GetFacpID(), pData->GetPSwitchID()
	);

	if (pDb->OpenQuery(strSql) == FALSE)
	{
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
		return 0;
	}
	nCnt = pDb->GetRecordCount();
	if (nCnt <= 0)
	{
		return 1;
	}
	strSql.Format(L"DELETE FROM TB_PSWITCH_MST WHERE FACP_ID=%d AND PS_ID=%d"
		, pData->GetFacpID(), pData->GetPSwitchID()
	);

	if (pDb->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 삭제하는데 실패 했습니다.");
		return 0;
	}

	if (m_pDlgLeftTopTreePane->DeleteTreeData(pData) <= 0)
	{
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 입력하는데 실패 했습니다.");
		return 0;
	}

	std::shared_ptr<CManagerPS> spManager = m_pRefFasSysData->GetPSwitchManager();
	if (spManager == nullptr)
		return 0; 
	spManager->RemoveData(pData);
	delete pData;
	pData = nullptr;
	return 1;
}


void CFormPSwitch::OnContextMenu(CWnd* pWnd, CPoint point)
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

	if (!(pHitWnd == (CWnd*)&m_ctrlRightTopPatternList || pHitWnd == (CWnd*)&m_ctrlRightBottomRelayList))
		return;
	if (pHitWnd == (CWnd*)&m_ctrlRightTopPatternList)
	{
		nID = IDR_POPUP_PTNLINK;
		m_ctrlRightTopPatternList.ScreenToClient(&ptHit);
		nIdx = m_ctrlRightTopPatternList.HitTest(ptHit, nullptr);
	}
	else
	{
		nID = IDR_POPUP_RELAYLINK;
		m_ctrlRightBottomRelayList.ScreenToClient(&ptHit);
		nIdx = m_ctrlRightBottomRelayList.HitTest(ptHit, nullptr);
	}
	if (nIdx < 0)
		return;
	menu.LoadMenu(nID);
	pContext = menu.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
}


void CFormPSwitch::OnPtnlkMenuDelptn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nIdx, nRet = -1;
	nIdx = m_ctrlRightTopPatternList.GetNextItem(-1, LVNI_SELECTED);
	if (nIdx < 0)
		return;

	if (AfxMessageBox(L"패턴정보를 삭제하시겠습니까?", MB_YESNO) == IDNO)
		return;

	CString strSql;
	CDataLinked * pLink;
	YAdoDatabase * pDB;
	pLink = (CDataLinked*)m_ctrlRightTopPatternList.GetItemData(nIdx);
	if (pLink == nullptr)
	{
		AfxMessageBox(L"삭제하는데 실패 했습니다. 패턴에 대한 정보를 가져오는데 실패 했습니다.");
		return;
	}

	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
		return;
	}

	pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB == nullptr)
	{
		AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트의 데이터베이스 정보가 잘못됐습니다.");
		return;
	}

	if (DeleteLink(pLink, pDB) == 0)
	{
		AfxMessageBox(L"연동된 패턴 정보를 삭제하는데 실패했습니다");
		return;
	}
	m_pCurrentData->DeleteLink(pLink);
	m_ctrlRightTopPatternList.DeleteItem(nIdx);
	AfxMessageBox(L"연동된 패턴 정보를 삭제하는데 성공했습니다");
}


void CFormPSwitch::OnPtnlkMenuViewptn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CFormPSwitch::OnRlylkMenuDelrly()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nRet = 0, nCnt, i;
	nCnt = m_ctrlRightBottomRelayList.GetSelectedCount();
	if (nCnt < 0)
		return;

	if (AfxMessageBox(L"연동 출력을 삭제하시겠습니까?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
		return;
	}

	BOOL bError = FALSE;
	CString strSql, strError = L"";
	CDataLinked * pLink;
	POSITION pos;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();

	if (pDB == nullptr)
	{
		AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트의 데이터베이스 정보가 잘못됐습니다.");
		return;
	}
	pos = m_ctrlRightBottomRelayList.GetFirstSelectedItemPosition();
	std::vector<int> vtSel;


	while (pos)
	{
		int nSelected = m_ctrlRightBottomRelayList.GetNextSelectedItem(pos);
		vtSel.push_back(nSelected);
	}

	pDB->BeginTransaction();

	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		pLink = (CDataLinked*)m_ctrlRightBottomRelayList.GetItemData(vtSel[i]);
		if (pLink == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
			strError = L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.";
			bError = TRUE;
			break;;
		}

		if (DeleteLink(pLink, pDB) == 0)
		{
			strError = L"삭제하는데 실패 했습니다. 데이터베이스에서 연동정보를 삭제하는데 실패했습니다.";
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
		pLink = (CDataLinked*)m_ctrlRightBottomRelayList.GetItemData(vtSel[i]);
		if (pLink == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
			strError = L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.";
			bError = TRUE;
			break;;
		}
		m_pCurrentData->DeleteLink(pLink);
		m_ctrlRightBottomRelayList.DeleteItem(vtSel[i]);
	}
	pDB->CommitTransaction();
	vtSel.clear();
	AfxMessageBox(L"연동 출력 정보를 삭제하는데 성공했습니다");
}


void CFormPSwitch::OnRlylkMenuViewrelay()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

int CFormPSwitch::FindItemLink(int nType, int nFacp, int nUnit, int nChn, int nRelay)
{
	int i, nCnt;
	CDataLinked * pLink = nullptr;
	CXListCtrl * pList = nullptr;
	if (LK_TYPE_RELEAY == nType)
	{
		pList = &m_ctrlRightBottomRelayList;
	}
	else
	{
		pList = &m_ctrlRightTopPatternList;
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


BOOL CFormPSwitch::OnEraseBkgnd(CDC* pDC)
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
