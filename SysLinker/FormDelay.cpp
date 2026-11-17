// FormDelay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "RelayTableData.h"
#include "resource.h"
#include "FormDelay.h"
#include "DlgChildDelay.h"
#include "DlgDelayTreePane.h"
#include "DlgChildDelayOutput.h"
#include "DataSystem.h"
#include "DataDevice.h"
#include "DataEquip.h"
#include "DataLocBase.h"

// CFormDelay

IMPLEMENT_DYNCREATE(CFormDelay, CView)

CFormDelay::CFormDelay()
	//: CView(IDD_FORM_DELAY)
{
	m_pDlgLeftBottomDelayInfo = nullptr;
	m_pDlgLeftTopDelayTreePane = nullptr;
	m_pDlgRightMiddleDelayOutput = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
	m_pRefFasSysData = nullptr;
}

CFormDelay::~CFormDelay()
{
	RemoveAllData();
}

void CFormDelay::DoDataExchange(CDataExchange* pDX)
{
	CView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormDelay, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_TVN_DROPED_ITEM(IDC_INPUTVIEW_TREE, OnTvnInputDropedItem)
	ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE, OnTvnOutputDropedItem)
	ON_MESSAGE(UWM_DELAYFORM_TREESELCHANGE, CFormDelay::OnDelayTreeSelChange)
	ON_MESSAGE(UWM_DELAYFORM_ADD_GROUP, CFormDelay::OnAddGroup)
	ON_MESSAGE(UWM_DELAYFORM_SAVE_GROUP, CFormDelay::OnSaveGroup)
	ON_MESSAGE(UWM_DELAYFORM_DEL_GROUP, CFormDelay::OnDelGroup)
	ON_MESSAGE(UWM_DELAYFORM_DELAY_TIME_ADD, CFormDelay::OnAddDelayTime)
	ON_COMMAND(ID_POPUP_MENU_DEL_INPUT, CFormDelay::OnPopupMenuDelInputCircuit)
	ON_COMMAND(ID_POPUP_MENU_DEL_OUTPUT, CFormDelay::OnPopupMenuDelOutputCircuit)
	ON_COMMAND(ID_POPUP_MENU_MOVE_UP, CFormDelay::OnPopupMenuMoveOutputUp)
	ON_COMMAND(ID_POPUP_MENU_MOVE_DOWN, CFormDelay::OnPopupMenuMoveOutputDown)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CFormDelay 진단입니다.

#ifdef _DEBUG
void CFormDelay::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormDelay::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormDelay 메시지 처리기입니다.

int CFormDelay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (m_SpMain.Create(this, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), AFX_IDW_PANE_FIRST) == false ||
		m_SpLeft.Create(&m_SpMain, WS_CHILD | WS_VISIBLE, CRect(0, 0, 800, 800), 200) == false ||
		m_SpRight.Create(&m_SpMain, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), 201) == false)
		return -1;

	if (!m_ctrlRightTopInputList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 600, 600), &m_SpRight, IDC_DELAY_TIME_INPUT_LIST))
		return -1;

	if (!m_ctrlRightBottomOutputList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT, CRect(0, 0, 600, 600), &m_SpRight, IDC_DELAY_TIME_OUTPUT_LIST))
		return -1;

	// 순서가 중요하므로 헤더 클릭에 의한 정렬 기능 비활성화
	m_ctrlRightBottomOutputList.m_bSortEnable = FALSE;

	if (m_pDlgLeftTopDelayTreePane == nullptr)
		m_pDlgLeftTopDelayTreePane = new CDlgDelayTreePane;

	if (!m_pDlgLeftTopDelayTreePane->CreateDlg(&m_SpLeft))
		return -1;
	m_pDlgLeftTopDelayTreePane->ShowWindow(SW_SHOW);
	m_pDlgLeftTopDelayTreePane->SetParentForMessage(this);

	if (m_pDlgLeftBottomDelayInfo == nullptr)
		m_pDlgLeftBottomDelayInfo = new CDlgChildDelay;

	if (!m_pDlgLeftBottomDelayInfo->CreateDlg(&m_SpLeft))
		return -1;
	m_pDlgLeftBottomDelayInfo->ShowWindow(SW_SHOW);
	m_pDlgLeftBottomDelayInfo->SetParentForMessage(this);

	if (m_pDlgRightMiddleDelayOutput == nullptr)
		m_pDlgRightMiddleDelayOutput = new CDlgChildDelayOutput;

	if (!m_pDlgRightMiddleDelayOutput->CreateDlg(&m_SpRight))
		return -1;
	m_pDlgRightMiddleDelayOutput->ShowWindow(SW_SHOW);
	m_pDlgRightMiddleDelayOutput->SetParentForMessage(this);

	theApp.SetFormViewInitComplete(FV_DELAY);

	return 0;
}

void CFormDelay::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	InitSplitter();

	m_ctrlRightTopInputList.InsertColumn(0, L"입력 회로 번호", LVCFMT_LEFT, 150);
	m_ctrlRightTopInputList.InsertColumn(1, L"입력 타입", LVCFMT_LEFT, 150);
	m_ctrlRightTopInputList.InsertColumn(2, L"설비명", LVCFMT_LEFT, 150);
	m_ctrlRightTopInputList.InsertColumn(3, L"설비 번호", LVCFMT_LEFT, 80);
	m_ctrlRightTopInputList.InsertColumn(4, L"위치", LVCFMT_LEFT, 150);
	m_ctrlRightTopInputList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ctrlRightBottomOutputList.InsertColumn(0, L"순서", LVCFMT_LEFT, 70);
	m_ctrlRightBottomOutputList.InsertColumn(1, L"종류", LVCFMT_LEFT, 70);
	m_ctrlRightBottomOutputList.InsertColumn(2, L"번호 (DELAY TIME)", LVCFMT_LEFT, 130);
	m_ctrlRightBottomOutputList.InsertColumn(3, L"이름", LVCFMT_LEFT, 300);
	m_ctrlRightBottomOutputList.InsertColumn(4, L"출력 타입", LVCFMT_LEFT, 100);
	m_ctrlRightBottomOutputList.InsertColumn(5, L"출력 내용", LVCFMT_LEFT, 100);
	m_ctrlRightBottomOutputList.InsertColumn(6, L"설비 번호", LVCFMT_LEFT, 80);	
	m_ctrlRightBottomOutputList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_INVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopInputList);
		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomOutputList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomOutputList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomOutputList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomOutputList);
		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomOutputList);	
		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomOutputList);
	}
}

int CFormDelay::SetSplitterPos()
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

	return 0;
}

int CFormDelay::InitSplitter()
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
	m_SpLeft.SetWindow(0, 0, m_pDlgLeftTopDelayTreePane->GetSafeHwnd());
	m_SpLeft.SetWindow(1, 0, m_pDlgLeftBottomDelayInfo->GetSafeHwnd());
	m_SpLeft.SetMinHeight(20);


	m_SpRight.AddRow();
	m_SpRight.AddRow();
	m_SpRight.AddRow();
	m_SpRight.AddColumn();
	m_SpRight.SetWindow(0, 0, m_ctrlRightTopInputList.m_hWnd);
	m_SpRight.SetWindow(1, 0, m_pDlgRightMiddleDelayOutput->GetSafeHwnd());
	m_SpRight.SetWindow(2, 0, m_ctrlRightBottomOutputList.m_hWnd);
	m_SpRight.SetMinHeight(20);

	m_SpMain.SetWidthForStatic(0, 350);

	m_SpLeft.SetHeightForStatic(0, 500);
	m_SpLeft.SetHeightForStatic(1, 400);
	m_SpLeft.SetWidthForStatic(0, 800);

	m_SpRight.SetHeightForStatic(0, 420);
	m_SpRight.SetHeightForStatic(1, 80);
	m_SpRight.SetHeightForStatic(2, 400);
	m_SpRight.SetWidthForStatic(0, 800);

	m_SpMain.Update();
	return 0;
}


void CFormDelay::OnDraw(CDC* /*pDC*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void CFormDelay::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetSplitterPos();
}


BOOL CFormDelay::OnEraseBkgnd(CDC* pDC)
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

void CFormDelay::RemoveAllData()
{
	if (m_pDlgLeftTopDelayTreePane != nullptr)
	{
		delete m_pDlgLeftTopDelayTreePane;
		m_pDlgLeftTopDelayTreePane = nullptr;
	}

	if (m_pDlgLeftBottomDelayInfo != nullptr)
	{
		delete m_pDlgLeftBottomDelayInfo;
		m_pDlgLeftBottomDelayInfo = nullptr;
	}

	if (m_pDlgRightMiddleDelayOutput != nullptr)
	{
		delete m_pDlgRightMiddleDelayOutput;
		m_pDlgRightMiddleDelayOutput = nullptr;
	}
}

void CFormDelay::SetInputCircuitList(unsigned short nGroupNum)
{
	auto iter = CNewInfo::Instance()->m_gmi.giMap.find(nGroupNum);
	if (iter == CNewInfo::Instance()->m_gmi.giMap.end())
	{
		return;
	}
	GROUP_INFO* pGi = iter->second;

	CDataSystem* pSys;
	CDataDevice* pDev;
	CDataEquip* pInputType;
	CDataEquip* pEqName;
	
	m_ctrlRightTopInputList.DeleteAllItems();
	m_ctrlRightTopInputList.SetRedraw(FALSE);

	int nRow = 0;
	for (auto pInput : pGi->inputEquipVec)
	{
		// 회로 번호 정보로 회로 데이터 찾기
		CString strSysKey = _T("");
		strSysKey = GF_GetSysDataKey(SE_RELAY, pInput->facp, pInput->unit, pInput->chn, pInput->circuit);

		pSys = theApp.m_pFasSysData->m_MapSystemData[strSysKey];
		if (pSys == nullptr)
			return;

		pDev = (CDataDevice*)pSys->GetSysData();
		if (pDev == nullptr)
			return;

		// 회로 번호
		CString strCircuitNum = _T("");
		strCircuitNum.Format(_T("%02d%02d-%d%03d"), pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum());

		// 입력 타입
		CString strInputType = _T("");
		pInputType = pDev->GetEqInput();
		if (pInputType != nullptr)
		{
			strInputType = pInputType->GetEquipName();
		}
		
		// 설비 명
		CString strEqName = _T("");
		pEqName = pDev->GetEqName();
		if (pEqName != nullptr)
		{
			strEqName = pEqName->GetEquipName();
		}

		// 설비 번호
		CString strEqNum = _T("");
		strEqNum = pDev->GetEqAddIndex();

		// 위치
		CString strLocation = _T("");
		strLocation = pDev->GetInputLocationName();

		// 리스트에 삽입
		m_ctrlRightTopInputList.InsertItem(nRow, strCircuitNum);
		m_ctrlRightTopInputList.SetItemText(nRow, 1, strInputType);
		m_ctrlRightTopInputList.SetItemText(nRow, 2, strEqName);
		m_ctrlRightTopInputList.SetItemText(nRow, 3, strEqNum);
		m_ctrlRightTopInputList.SetItemText(nRow, 4, strLocation);

		nRow++;
	}

	m_ctrlRightTopInputList.SetRedraw();
}

void CFormDelay::SetOutputCircuitList(unsigned short nGroupNum)
{
	auto iter = CNewInfo::Instance()->m_gmi.giMap.find(nGroupNum);
	if (iter == CNewInfo::Instance()->m_gmi.giMap.end())
	{
		return;
	}
	GROUP_INFO* pGi = iter->second;

	CDataSystem* pSys;
	CDataDevice* pDev;
	CDataEquip* pOutputType;
	CDataEquip* pOutputContent;
	GROUP_EQUIPMENT_INFO* pOutputEquip;

	m_ctrlRightBottomOutputList.DeleteAllItems();
	m_ctrlRightBottomOutputList.SetRedraw(FALSE);

	int nRow = 0;

	//std::map은 키 값 기준으로 자동 정렬되어 있음
	for (auto& pOutputPair : pGi->outputEquipMap)
	{
		unsigned short nSeq = pOutputPair.first;
		pOutputEquip = pOutputPair.second;

		// 순서 번호
		CString strSeq = _T("");
		strSeq.Format(_T("%d"), nSeq);

		// 타입
		unsigned char nType = pOutputEquip->type;

		CString strSysKey = _T("");
		CString strType = _T("");
		CString strCircuitNum = _T("");
		CString strCircuitName = _T("");
		CString strOutputType = _T("");
		CString strOutputContent = _T("");
		CString strEqNum = _T("");
		CString strLocation = _T("");

		// 리스트 추가 시 ROW 문자 및 배경의 색상 기본 설정
		COLORREF crText = RGB(0, 0, 0);		// 문자 : 검정
		COLORREF crBk = RGB(255, 255, 255);	// 배경 : 햐양

		switch (nType)
		{		
			case DET_CIRCUIT:
			{
				// 회로 번호 정보로 회로 데이터 찾기	
				strSysKey = GF_GetSysDataKey(SE_RELAY, pOutputEquip->facp, pOutputEquip->unit, pOutputEquip->chn, pOutputEquip->circuit);

				pSys = theApp.m_pFasSysData->m_MapSystemData[strSysKey];
				if (pSys == nullptr)
				{
					m_ctrlRightBottomOutputList.SetRedraw();
					return;
				}

				pDev = (CDataDevice*)pSys->GetSysData();
				if (pDev == nullptr)
				{
					m_ctrlRightBottomOutputList.SetRedraw();
					return;
				}

				// 타입
#ifndef ENGLISH_MODE
				strType = _T("회로");
#else
				strType = _T("CIRCUIT");
#endif

				// 회로 번호
				strCircuitNum.Format(_T("%02d%02d-%d%03d"), pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum());

				// 회로 이름
				strCircuitName = pDev->GetOutputFullName();

				// 출력 타입
				pOutputType = pDev->GetEqInput();
				if (pOutputType != nullptr)
				{
					strOutputType = pOutputType->GetEquipName();
				}

				// 출력 내용		
				pOutputContent = pDev->GetEqName();
				if (pOutputContent != nullptr)
				{
					strOutputContent = pOutputContent->GetEquipName();
				}

				// 설비 번호			
				strEqNum = pDev->GetEqAddIndex();
			}
			break;

			case DET_DELAY:
			{
				// 타입
				strType = _T("DELAY");

				// 회로 번호 : 회로 번호에 delay time을 넣음, 추후 협의 필요
				strCircuitNum.Format(_T("%d"), pOutputEquip->circuit);

				// 딜레이는 빨간 색으로 구별
				crText = RGB(255, 255, 255);		// 문자 : 햐양
				crBk = RGB(255, 0, 0);	// 배경 : 빨강
			}
			break;

			default:
				break;
		}

		// 리스트에 삽입
		m_ctrlRightBottomOutputList.InsertItem(nRow, strSeq, crText, crBk);
		m_ctrlRightBottomOutputList.SetItemText(nRow, 1, strType, crText, crBk);
		m_ctrlRightBottomOutputList.SetItemText(nRow, 2, strCircuitNum, crText, crBk);
		m_ctrlRightBottomOutputList.SetItemText(nRow, 3, strCircuitName, crText, crBk);
		m_ctrlRightBottomOutputList.SetItemText(nRow, 4, strOutputType, crText, crBk);
		m_ctrlRightBottomOutputList.SetItemText(nRow, 5, strOutputContent, crText, crBk);
		m_ctrlRightBottomOutputList.SetItemText(nRow, 6, strEqNum, crText, crBk);
		
		nRow++;
	}

	m_ctrlRightBottomOutputList.SetRedraw();
}

void CFormDelay::OnTvnInputDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	//만약 tree가 선택된 상태가 아니라면 리턴
	if (m_pDlgLeftTopDelayTreePane->m_nCurGroupNum == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("선택된 그룹이 없습니다."));
#else
		AfxMessageBox(_T("No group is selected."));
#endif
		return;
	}

	m_ctrlRightTopInputList.SetRedraw(FALSE);

	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataSystem* pSys;
	CDataDevice* pDev;
	CDataEquip* pInputType;
	CDataEquip* pEqName;
	CDataLocBase* pLoc;
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

		// 입력 회로 데이터를 가공

		// 입력 회로 번호
		CString strCircuitNum = _T("");
		strCircuitNum.Format(_T("%02d%02d-%d%03d"), pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum());

		// 중복일 경우 추가하지 않음
		nIdx = m_ctrlRightTopInputList.GetItemCount();
		for (int nRow = 0; nRow < nIdx; nRow++)
		{
			CString strPreCircuitNum = _T("");
			strPreCircuitNum = m_ctrlRightTopInputList.GetItemText(nRow, 0);
			if (strPreCircuitNum.Compare(strCircuitNum) == 0)
			{
#ifndef ENGLISH_MODE
				AfxMessageBox(_T("이미 추가된 회로입니다."));
#else
				AfxMessageBox(_T("This circuit has already been added."));
#endif
				return;
			}
		}
	
		// 입력 타입
		CString strInputType = _T("");
		pInputType = pDev->GetEqInput();
		if (pInputType != nullptr)
		{
			strInputType = pInputType->GetEquipName();
		}		

		// 설비명
		CString strEqName = _T("");
		pEqName = pDev->GetEqName();
		if (pEqName != nullptr)
		{
			strEqName = pEqName->GetEquipName();
		}

		// 설비 번호
		CString strEqNum = _T("");
		strEqNum = pDev->GetEqAddIndex();

		// 위치
		CString strLocation = _T("");
		strLocation = pDev->GetInputLocationName();

		// 입력 회로 리스트에 추가
		m_ctrlRightTopInputList.InsertItem(nIdx, strCircuitNum);
		m_ctrlRightTopInputList.SetItemText(nIdx, 1, strInputType);
		m_ctrlRightTopInputList.SetItemText(nIdx, 2, strEqName);
		m_ctrlRightTopInputList.SetItemText(nIdx, 3, strEqNum);
		m_ctrlRightTopInputList.SetItemText(nIdx, 4, strLocation);

		// 입력 회로 메모리에 추가
		GROUP_INFO* pGi = CNewInfo::Instance()->m_gmi.giMap[m_pDlgLeftTopDelayTreePane->m_nCurGroupNum];
		GROUP_EQUIPMENT_INFO* pGiei = new GROUP_EQUIPMENT_INFO;
		memset(pGiei, 0, sizeof(GROUP_EQUIPMENT_INFO));
		pGiei->facp = pDev->GetFacpNum();
		pGiei->unit = pDev->GetUnitNum();
		pGiei->chn = pDev->GetChnNum();
		pGiei->circuit = pDev->GetDeviceNum();
		pGi->inputEquipVec.push_back(pGiei);
	}

	m_ctrlRightTopInputList.SetRedraw();
	m_ctrlRightTopInputList.Invalidate();
}

void CFormDelay::OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	//만약 tree가 선택된 상태가 아니라면 리턴
	if (m_pDlgLeftTopDelayTreePane->m_nCurGroupNum == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("선택된 그룹이 없습니다."));
#else
		AfxMessageBox(_T("No group is selected."));
#endif
		return;
	}

	m_ctrlRightTopInputList.SetRedraw(FALSE);

	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataSystem* pSys;
	CDataDevice* pDev;
	CDataEquip* pOutputType;
	CDataEquip* pOutputContent;
	CDataLocBase* pLoc;

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

		// 출력 회로 데이터를 가공

		// 순서 번호
		CString strSeq = _T("");
		nIdx = m_ctrlRightBottomOutputList.GetItemCount();
		strSeq.Format(_T("%d"), nIdx + 1);

		// 타입
#ifndef ENGLISH_MODE
		CString strType = _T("회로");
#else
		CString strType = _T("CIRCUIT");
#endif

		// 번호
		CString strCircuitNum = _T("");
		strCircuitNum.Format(_T("%02d%02d-%d%03d"), pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum());

		// 중복일 경우 추가하지 않음
		nIdx = m_ctrlRightBottomOutputList.GetItemCount();
		for (int nRow = 0; nRow < nIdx; nRow++)
		{
			// 타입이 회로인 경우만 체크 대상
			CString strTypeInList = _T("");
			strTypeInList = m_ctrlRightBottomOutputList.GetItemText(nRow, 1);
			if(strTypeInList.Compare(strType) != 0)
				continue;

			CString strPreCircuitNum = _T("");
			strPreCircuitNum = m_ctrlRightBottomOutputList.GetItemText(nRow, 2);
			if (strPreCircuitNum.Compare(strCircuitNum) == 0)
			{
#ifndef ENGLISH_MODE
				AfxMessageBox(_T("이미 추가된 회로입니다."));
#else
				AfxMessageBox(_T("This circuit has already been added."));
#endif
				return;
			}
		}
		// 이름
		CString strOutputCircuitName = _T("");
		strOutputCircuitName = pDev->GetOutputFullName();

		// 출력 타입
		CString strOutputType = _T("");
		pOutputType = pDev->GetEqOutput();
		if (pOutputType != nullptr)
		{
			strOutputType = pOutputType->GetEquipName();
		}

		// 출력 내용
		CString strOutputContent = _T("");
		pOutputContent = pDev->GetEqOutContents();
		if (pOutputContent != nullptr)
		{
			strOutputContent = pOutputContent->GetEquipName();
		}

		// 설비 번호
		CString strEqNum = pDev->GetEqAddIndex();

		// 출력 회로 리스트에 추가	
		m_ctrlRightBottomOutputList.InsertItem(nIdx, strSeq);
		m_ctrlRightBottomOutputList.SetItemText(nIdx, 1, strType);
		m_ctrlRightBottomOutputList.SetItemText(nIdx, 2, strCircuitNum);
		m_ctrlRightBottomOutputList.SetItemText(nIdx, 3, strOutputCircuitName);
		m_ctrlRightBottomOutputList.SetItemText(nIdx, 4, strOutputType);
		m_ctrlRightBottomOutputList.SetItemText(nIdx, 5, strOutputContent);
		m_ctrlRightBottomOutputList.SetItemText(nIdx, 6, strEqNum);	

		// 출력 회로 메모리에 추가
		GROUP_INFO* pGi = CNewInfo::Instance()->m_gmi.giMap[m_pDlgLeftTopDelayTreePane->m_nCurGroupNum];
		GROUP_EQUIPMENT_INFO* pOutputEquip = new GROUP_EQUIPMENT_INFO;
		memset(pOutputEquip, 0, sizeof(GROUP_EQUIPMENT_INFO));
		pOutputEquip->type = DET_CIRCUIT;
		pOutputEquip->facp = pDev->GetFacpNum();
		pOutputEquip->unit = pDev->GetUnitNum();
		pOutputEquip->chn = pDev->GetChnNum();
		pOutputEquip->circuit = pDev->GetDeviceNum();
		pGi->outputEquipMap[nIdx + 1] = pOutputEquip;
	}

	m_ctrlRightTopInputList.SetRedraw();
	m_ctrlRightTopInputList.Invalidate();
}

LRESULT CFormDelay::OnDelayTreeSelChange(WPARAM wp, LPARAM lp)
{
	// 그룹 번호 확인
	unsigned short nGroupNum = m_pDlgLeftTopDelayTreePane->m_nCurGroupNum;

	// 그룹 번호에 해당하는 데이터를 UI에 표시

	// CDlgChildDelay*	m_pDlgLeftBottomDelayInfo
	m_pDlgLeftBottomDelayInfo->SetGroupInfo(nGroupNum);

	// CXListCtrl m_ctrlRightTopInputList
	SetInputCircuitList(nGroupNum);

	// CXListCtrl m_ctrlRightBottomOutputList;
	SetOutputCircuitList(nGroupNum);

	return 1;
}

LRESULT CFormDelay::OnAddGroup(WPARAM wp, LPARAM lp)
{
	// 그룹 번호 확인
	unsigned short nGroupNum = m_pDlgLeftBottomDelayInfo->m_nCurGroupNum;

	// 트리에 새 아이템을 추가
	m_pDlgLeftTopDelayTreePane->AddNewGroupToTree(nGroupNum);

	return 1;
}

LRESULT CFormDelay::OnSaveGroup(WPARAM wp, LPARAM lp)
{
	// 그룹 번호 확인
	unsigned short nGroupNum = m_pDlgLeftBottomDelayInfo->m_nCurGroupNum;

	// 변경된 그룹 이름 적용
	m_pDlgLeftTopDelayTreePane->SaveGroupToTree(nGroupNum);

	return 1;
}

LRESULT CFormDelay::OnDelGroup(WPARAM wp, LPARAM lp)
{
	// 그룹 번호 확인
	unsigned short nGroupNum = m_pDlgLeftBottomDelayInfo->m_nCurGroupNum;

	// 삭제된 그룹 노드 적용
	m_pDlgLeftTopDelayTreePane->DelGroupFromTree(nGroupNum);

	// 그룹 상세 정보 초기화
	m_pDlgLeftBottomDelayInfo->SetGroupInfo(0);

	return 1;
}

LRESULT CFormDelay::OnAddDelayTime(WPARAM wp, LPARAM lp)
{
	// 그룹 번호 확인
	unsigned short nGroupNum = m_pDlgLeftBottomDelayInfo->m_nCurGroupNum;
	if (nGroupNum == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("선택된 그룹이 없습니다."));
#else
		AfxMessageBox(_T("No group is selected."));
#endif
		return 0;
	}

	// 현재 출력 회로 갯수 가져오기
	int nListCount = m_ctrlRightBottomOutputList.GetItemCount();

	// 딜레이 타임 가져오기
	unsigned short nDelayTime = m_pDlgRightMiddleDelayOutput->m_nDelayTime;

	// UI 적용
	CString strSeq = _T("");
	strSeq.Format(_T("%d"), nListCount + 1);

	CString strType = _T("DELAY");

	CString strDelayTime = _T("");
	strDelayTime.Format(_T("%d"), nDelayTime);

	// 딜레이일 경우 구분할 수 있도록 타입 컬럼의 색을 적용
	COLORREF crText = RGB(255, 255, 255), crBk = RGB(255, 0, 0);

	m_ctrlRightBottomOutputList.InsertItem(nListCount, strSeq, crText, crBk);				// row는 0베이스
	m_ctrlRightBottomOutputList.SetItemText(nListCount, 1, strType, crText, crBk);
	m_ctrlRightBottomOutputList.SetItemText(nListCount, 2, strDelayTime, crText, crBk);
	m_ctrlRightBottomOutputList.SetItemText(nListCount, 3, _T(""), crText, crBk);
	m_ctrlRightBottomOutputList.SetItemText(nListCount, 4, _T(""), crText, crBk);
	m_ctrlRightBottomOutputList.SetItemText(nListCount, 5, _T(""), crText, crBk);
	m_ctrlRightBottomOutputList.SetItemText(nListCount, 6, _T(""), crText, crBk);

	// 데이터 처리
	auto iter = CNewInfo::Instance()->m_gmi.giMap.find(nGroupNum);
	if (iter != CNewInfo::Instance()->m_gmi.giMap.end())
	{
		GROUP_INFO* pGi = iter->second;
		
		GROUP_EQUIPMENT_INFO* pGei = new GROUP_EQUIPMENT_INFO;
		memset(pGei, 0, sizeof(GROUP_EQUIPMENT_INFO));
		pGei->type = DET_DELAY;
		pGei->circuit = nDelayTime;		// 일단 circuit에 넣고 추후 협의 필요

		pGi->outputEquipMap[nListCount + 1] = pGei;
	}

	return 1;
}

void CFormDelay::OnContextMenu(CWnd* pWnd, CPoint point)
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

	if (!(pHitWnd == (CWnd*)&m_ctrlRightTopInputList || pHitWnd == (CWnd*)&m_ctrlRightBottomOutputList))
		return;
	if (pHitWnd == (CWnd*)&m_ctrlRightTopInputList)
	{
#ifndef ENGLISH_MODE
		nID = IDR_POPUP_GROUP_INPUT;
#else
		nID = IDR_POPUP_GROUP_INPUT;
#endif
		m_ctrlRightTopInputList.ScreenToClient(&ptHit);
		nIdx = m_ctrlRightTopInputList.HitTest(ptHit, nullptr);

		if (nIdx >= 0)
		{
			// 우클릭한 항목이 선택되어 있지 않은 경우
			if (!(m_ctrlRightTopInputList.GetItemState(nIdx, LVIS_SELECTED) & LVIS_SELECTED))
			{
				// 기존 선택 해제
				m_ctrlRightTopInputList.SetItemState(-1, 0, LVIS_SELECTED);

				// 우클릭한 항목 선택
				m_ctrlRightTopInputList.SetItemState(nIdx, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			}
		}
	}
	else
	{
#ifndef ENGLISH_MODE
		nID = IDR_POPUP_GROUP_OUTPUT;
#else
		nID = IDR_POPUP_GROUP_OUTPUT;
#endif
		m_ctrlRightBottomOutputList.ScreenToClient(&ptHit);
		nIdx = m_ctrlRightBottomOutputList.HitTest(ptHit, nullptr);

		if (nIdx >= 0)
		{
			// 우클릭한 항목이 선택되어 있지 않은 경우
			if (!(m_ctrlRightBottomOutputList.GetItemState(nIdx, LVIS_SELECTED) & LVIS_SELECTED))
			{
				// 기존 선택 해제
				m_ctrlRightBottomOutputList.SetItemState(-1, 0, LVIS_SELECTED);

				// 우클릭한 항목 선택
				m_ctrlRightBottomOutputList.SetItemState(nIdx, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			}
		}
	}
	if (nIdx < 0)
		return;
	menu.LoadMenu(nID);
	pContext = menu.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
}

void CFormDelay::OnPopupMenuDelInputCircuit()
{
	unsigned short nGroupNum = m_pDlgLeftTopDelayTreePane->m_nCurGroupNum;
	auto iter = CNewInfo::Instance()->m_gmi.giMap.find(nGroupNum);
	if (iter != CNewInfo::Instance()->m_gmi.giMap.end())
	{
		GROUP_INFO* pGi = iter->second;

		POSITION pos = m_ctrlRightTopInputList.GetFirstSelectedItemPosition();

		while (pos != NULL)
		{
			int nItem = m_ctrlRightTopInputList.GetNextSelectedItem(pos);
			CString strCircuitNum = _T("");
			strCircuitNum = m_ctrlRightTopInputList.GetItemText(nItem, 0);

			// 람다식을 이용해 name 멤버만 비교
			auto it = std::find_if(pGi->inputEquipVec.begin(), pGi->inputEquipVec.end(), [&strCircuitNum](GROUP_EQUIPMENT_INFO* gei) {
				CString strCircuitNumInVec = _T("");
				strCircuitNumInVec.Format(_T("%02d%02d-%d%03d"), gei->facp, gei->unit, gei->chn, gei->circuit);
				return strCircuitNumInVec.Compare(strCircuitNum) == 0;
			});
			if (it != pGi->inputEquipVec.end())
			{
				// 데이터 삭제
				delete *it;
				pGi->inputEquipVec.erase(it);

				// UI에서 삭제
				m_ctrlRightTopInputList.DeleteItem(nItem);
			}

			// 삭제 후 position을 다시 처음부터 갱신하거나 재설정
			pos = m_ctrlRightTopInputList.GetFirstSelectedItemPosition();
		}
	}
}

void CFormDelay::OnPopupMenuDelOutputCircuit()
{
	unsigned short nGroupNum = m_pDlgLeftTopDelayTreePane->m_nCurGroupNum;
	auto iter = CNewInfo::Instance()->m_gmi.giMap.find(nGroupNum);
	if (iter != CNewInfo::Instance()->m_gmi.giMap.end())
	{
		GROUP_INFO* pGi = iter->second;
		auto& outputMap = pGi->outputEquipMap;

		POSITION pos = m_ctrlRightBottomOutputList.GetFirstSelectedItemPosition();

		// 삭제할 순서번호 저장
		std::vector<unsigned short> vecDeleteKey;

		while (pos != NULL)
		{
			int nItem = m_ctrlRightBottomOutputList.GetNextSelectedItem(pos);
			CString strSeq = _T("");
			strSeq = m_ctrlRightBottomOutputList.GetItemText(nItem, 0);
			int nSeq = _wtoi(strSeq);
			
			vecDeleteKey.push_back(nSeq);
		}

		// 1. 선택된 항목을 Map에서 삭제
		for (auto nKey : vecDeleteKey)
		{
			auto it = outputMap.find(nKey);

			if (it != outputMap.end())
			{
				// 동적 할당된 데이터 해제
				delete it->second;

				// Map에서 제거
				outputMap.erase(it);
			}
		}

		// 2. 남아 있는 Map의 Key를 1부터 재정렬
		std::map<unsigned short, GROUP_EQUIPMENT_INFO*> newMap;

		unsigned short nNewKey = 1;

		for (auto& pair : outputMap)
		{
			newMap[nNewKey] = pair.second;
			++nNewKey;
		}

		// 기존 Map을 새로운 Map으로 교체
		outputMap.swap(newMap);

		// 3. 출력 리스트 갱신
		SetOutputCircuitList(nGroupNum);
	}
}

void CFormDelay::MoveSelectedOutputCircuit(BOOL bMoveUp)
{
	// 현재 선택된 그룹 번호
	unsigned short nGroupNum = m_pDlgLeftTopDelayTreePane->m_nCurGroupNum;

	if (nGroupNum == 0)
		return;

	// 그룹 정보 검색
	auto& giMap = CNewInfo::Instance()->m_gmi.giMap;
	auto iter = giMap.find(nGroupNum);

	if (iter == giMap.end() || iter->second == nullptr)
		return;

	GROUP_INFO* pGi = iter->second;
	auto& outputMap = pGi->outputEquipMap;

	int nCount = m_ctrlRightBottomOutputList.GetItemCount();

	if (nCount <= 1 || outputMap.size() != static_cast<size_t>(nCount))
		return;

	// 1. 현재 선택 상태 저장
	std::vector<bool> vecSelected(nCount, false);

	POSITION pos = m_ctrlRightBottomOutputList.GetFirstSelectedItemPosition();

	if (pos == NULL)
		return;

	while (pos != NULL)
	{
		int nItem = m_ctrlRightBottomOutputList.GetNextSelectedItem(pos);

		if (nItem >= 0 && nItem < nCount)
			vecSelected[nItem] = true;
	}

	// 2. Map Iterator를 순서대로 저장
	typedef std::map<unsigned short, GROUP_EQUIPMENT_INFO*>::iterator OUTPUT_ITER;

	std::vector<OUTPUT_ITER> vecIter;

	for (auto it = outputMap.begin(); it != outputMap.end(); ++it)
	{
		vecIter.push_back(it);
	}

	BOOL bMoved = FALSE;

	// 3. 이동 방향에 따라 데이터 교환
	if (bMoveUp)
	{
		// 위로 이동: 앞에서부터 검사
		for (int i = 1; i < nCount; ++i)
		{
			if (vecSelected[i] && !vecSelected[i - 1])
			{
				// Map 데이터 교환
				std::swap(vecIter[i]->second, vecIter[i - 1]->second);

				// 선택 상태도 함께 이동
				vecSelected[i - 1] = true;
				vecSelected[i] = false;

				bMoved = TRUE;
			}
		}
	}
	else
	{
		// 아래로 이동: 뒤에서부터 검사
		for (int i = nCount - 2; i >= 0; --i)
		{
			if (vecSelected[i] && !vecSelected[i + 1])
			{
				// Map 데이터 교환
				std::swap(vecIter[i]->second, vecIter[i + 1]->second);

				// 선택 상태도 함께 이동
				vecSelected[i + 1] = true;
				vecSelected[i] = false;

				bMoved = TRUE;
			}
		}
	}

	// 실제 이동한 항목이 없으면 종료
	if (!bMoved)
		return;

	// 4. 출력 리스트 다시 구성
	SetOutputCircuitList(nGroupNum);

	// 5. 이동한 항목의 선택 상태 복원
	int nFirstSelected = -1;

	for (int i = 0; i < nCount; ++i)
	{
		if (vecSelected[i])
		{
			m_ctrlRightBottomOutputList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

			if (nFirstSelected == -1)
				nFirstSelected = i;
		}
	}

	// 6. 선택된 항목에 포커스 설정
	if (nFirstSelected >= 0)
	{
		m_ctrlRightBottomOutputList.SetItemState(nFirstSelected, LVIS_FOCUSED, LVIS_FOCUSED);

		m_ctrlRightBottomOutputList.EnsureVisible(nFirstSelected, FALSE);
	}
}

void CFormDelay::OnPopupMenuMoveOutputUp()
{
	MoveSelectedOutputCircuit(TRUE);
}

void CFormDelay::OnPopupMenuMoveOutputDown()
{
	MoveSelectedOutputCircuit(FALSE);
}