// FormAutoMake.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormAutoMake.h"

#include "DataAutoPattern.h"
#include "DataAutoDevice.h"
#include "DataAutoLogic.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataDevice.h"
#include "DataEquip.h"
#include "DataLocation.h"
#include "DataPattern.h"
#include "ManagerAutoLogic.h"
#include "ManagerPattern.h"
#include "ManagerEmergency.h"
#include "DataLinked.h"
#include "RelayTableData.h"
#include "DataEmBc.h"
#include "DataNewAutoLink.h"
#include "DataNewAutoPtn.h"
#include "DataAutoMake.h"
#include "../Common/Utils/YAdoDatabase.h"

#include "./XMakeLink/XMakeLink_Def.h"
#include "./XMakeLink/XDataDev.h"
#include "./XMakeLink/XMakeLink.h"
#include "./XMakeLink/XDataEm.h"
#include "./XMakeLink/XPatternMst.h"
#include "./XMakeLink/XDataLink.h"
#include "./XMakeLink/XPatternMst.h"
#include <algorithm>

#include "MainFrm.h"
// CFormAutoMake

IMPLEMENT_DYNCREATE(CFormAutoMake, CFormView)

#ifndef ENGLISH_MODE
CFormAutoMake::CFormAutoMake()
	: CFormView(IDD_FORMAUTOMAKE)
{
	m_pRefFasSysData = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
	m_pMakeLink = nullptr;

}
#else
CFormAutoMake::CFormAutoMake()
	: CFormView(IDD_FORMAUTOMAKE_EN)
{
	m_pRefFasSysData = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
	m_pMakeLink = nullptr;
}
#endif

CFormAutoMake::~CFormAutoMake()
{
	if(m_pMakeLink)
	{
		delete m_pMakeLink;
		m_pMakeLink = nullptr;
	}
}

void CFormAutoMake::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICE_TREE, m_ctrlTree);
	DDX_Control(pDX, IDC_RELAY_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_PROG, m_ctrlProg);
}

BEGIN_MESSAGE_MAP(CFormAutoMake, CFormView)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_BTN_MAKE, &CFormAutoMake::OnBnClickedBtnMake)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFormAutoMake::OnBnClickedBtnSave)
	ON_MESSAGE(CSWM_PROGRESS_STEP, OnMakeProgress)
	ON_BN_CLICKED(IDC_BTN_STOP, &CFormAutoMake::OnBnClickedBtnStop)
	ON_NOTIFY(TVN_SELCHANGED, IDC_DEVICE_TREE, &CFormAutoMake::OnTvnSelchangedDeviceTree)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_MESSAGE(UWM_ERRORCHECK_NOTIFY,&CFormAutoMake::OnErrorCheckEnd)

END_MESSAGE_MAP()


// CFormAutoMake 진단입니다.

#ifdef _DEBUG
void CFormAutoMake::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormAutoMake::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormAutoMake 메시지 처리기입니다.

DWORD CFormAutoMake::Thread_MakeProc(LPVOID lpData)
{
	CFormAutoMake	*me;
	int nRet = 0; 
	me = (CFormAutoMake *)lpData;
	if ((CFormAutoMake *)NULL == me)
	{
		me->SendMessage(CSWM_PROGRESS_STEP, 0, 0);
		return 0;
	}

	try
	{
		//20240822 GBM start - 시간 측정
		LARGE_INTEGER startTime, endTime;
		QueryPerformanceCounter(&startTime);

		nRet = me->GenerateAutoLinkData_XMake();

		QueryPerformanceCounter(&endTime);
		float duringTime;
		duringTime = CCommonFunc::GetPreciseDeltaTime(startTime, endTime);
		Log::Trace("연동데이터 자동 생성에 걸린 시간 : %f", duringTime);
		//20240822 GBM end
	}
	catch (...)
	{
	}
	/*
	*  Thread가 종료 하였음을 설정.
	*/
	return 0;
}


DWORD CFormAutoMake::Thread_SaveProc(LPVOID lpData)
{
	CFormAutoMake	*me;
	int nRet = 0;
	me = (CFormAutoMake *)lpData;
	if((CFormAutoMake *)NULL == me)
	{
		me->SendMessage(CSWM_PROGRESS_STEP,0,0);
		return 0;
	}

	try
	{
		//20240822 GBM start - 시간 측정
		LARGE_INTEGER startTime, endTime;
		QueryPerformanceCounter(&startTime);

		nRet = me->ProcessSaveAutoLink_XMake();

		QueryPerformanceCounter(&endTime);
		float duringTime;
		duringTime = CCommonFunc::GetPreciseDeltaTime(startTime, endTime);
		Log::Trace("자동 생성된 연동데이터 적용에 걸린 시간 : %f", duringTime);
		//20240822 GBM end
	}
	catch(...)
	{
	}
	
	/*
	*  Thread가 종료 하였음을 설정.
	*/
	return 0;
}


void CFormAutoMake::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	CRect rc,rcMain;
	GetClientRect(&rc);
	rc.DeflateRect(4, 4, 4, 4);

	rcMain = CRect(4, 65, rc.Width() - 8, rc.Height() - 4);
	
	if (m_SpMain.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,            // the parent of the splitter pane
		&m_ctrlTree,    // left pane
		&m_ctrlList,    // right pane
		IDC_STATIC_SLIT_PANE, // this ID is used for saving/restoring splitter
							  // position and therefore it must be unique 
							  // within your application
		rc,        // dimensions of the splitter pane
		90,            // left constraint for splitter position
		110         // right constraint for splitter position
	) == false)
		return;

	m_ctrlProg.SetRange(0, 100);
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pRefFasSysData = theApp.GetRelayTableData();

#ifndef ENGLISH_MODE
	m_ctrlList.InsertColumn(0, _T("종류"), LVCFMT_LEFT, 40);
	m_ctrlList.InsertColumn(1, _T("출력/패턴이름"), LVCFMT_LEFT, 250);
	m_ctrlList.InsertColumn(2, _T("입력타입"), LVCFMT_LEFT, 70);
	m_ctrlList.InsertColumn(3, _T("설비명"), LVCFMT_LEFT, 70);
	m_ctrlList.InsertColumn(4, _T("출력타입"), LVCFMT_LEFT, 70);
	m_ctrlList.InsertColumn(5, _T("출력설명"), LVCFMT_LEFT, 70);
	m_ctrlList.InsertColumn(6, _T("출력주소"), LVCFMT_LEFT, 100);
	m_ctrlList.InsertColumn(7 , _T("건물"), LVCFMT_LEFT, 50);
	m_ctrlList.InsertColumn(8 , _T("건물종류"), LVCFMT_LEFT, 50);
	m_ctrlList.InsertColumn(9 , _T("계단"), LVCFMT_LEFT, 50);
	m_ctrlList.InsertColumn(10, _T("층"), LVCFMT_LEFT, 50);
	m_ctrlList.InsertColumn(11, _T("실"), LVCFMT_LEFT, 50);
#else
	m_ctrlList.InsertColumn(0, _T("TYPE"), LVCFMT_LEFT, 40);
	m_ctrlList.InsertColumn(1, _T("OUTPUT/PATTERN NAME"), LVCFMT_LEFT, 250);
	m_ctrlList.InsertColumn(2, _T("INPUT TYPE"), LVCFMT_LEFT, 70);
	m_ctrlList.InsertColumn(3, _T("EQUIPMENT NAME"), LVCFMT_LEFT, 70);
	m_ctrlList.InsertColumn(4, _T("OUTPUT TYPE"), LVCFMT_LEFT, 70);
	m_ctrlList.InsertColumn(5, _T("OUTPUT DESCRIPTION"), LVCFMT_LEFT, 70);
	m_ctrlList.InsertColumn(6, _T("OUTPUT ADDRESS"), LVCFMT_LEFT, 100);
	m_ctrlList.InsertColumn(7, _T("BUILDING"), LVCFMT_LEFT, 50);
	m_ctrlList.InsertColumn(8, _T("BUILDING TYPE"), LVCFMT_LEFT, 50);
	m_ctrlList.InsertColumn(9, _T("LINE"), LVCFMT_LEFT, 50);
	m_ctrlList.InsertColumn(10, _T("FLOOR"), LVCFMT_LEFT, 50);
	m_ctrlList.InsertColumn(11, _T("ROOM"), LVCFMT_LEFT, 50);
#endif

	m_ctrlList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_pRefFasSysData->CheckAndSetFacpAndUnitType();		// 이 클래스에서 m_nMaxLinkCount가 사용되기 때문에 미리 ROM 파일 버전을 체크
}

void CFormAutoMake::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CRect rc , rcCtrl;
	CRect rcLeft, rcRight;
	rc.left = 4;
	rc.top = 4;
	rc.right = cx - 4;
	rc.bottom = cy - 4;
	rcCtrl = rc;

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (GetDlgItem(IDC_ST_MESSAGE)->GetSafeHwnd())
	{
		rcCtrl.top = 33;
		rcCtrl.bottom = 48;
		GetDlgItem(IDC_ST_MESSAGE)->MoveWindow(rcCtrl);
	}

	if (m_ctrlProg.GetSafeHwnd())
	{
		rcCtrl.top = 52;
		rcCtrl.bottom = 62;
		m_ctrlProg.MoveWindow(rcCtrl);
	}

	rcRight = CRect(4, 65, cx - 4, cy - 4);

	if (m_SpMain.GetSafeHwnd())
	{
		m_SpMain.MoveWindow(rcRight);
	}
}

void CFormAutoMake::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CFormAutoMake::OnBnClickedBtnMake()
{
	m_ctrlList.DeleteAllItems();
	m_ctrlTree.DeleteAllItems();
	
#ifndef ENGLISH_MODE
	if(AfxMessageBox(L"연동데이터 자동생성 시작전 오류검사를 진행하실려면\nYes : 오류검사 후 연동데이터 생성\nNo:오류검사없이 연동데이터 생성",MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		AutoMakeStart_XMake();
	}
	else
	{
		CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

		if(pMainWnd == nullptr)
		{
			return;
		}

		pMainWnd->StartErrorCheck(ERR_CHECK_MAKEAUTOLINK,this);
	}
#else
	if (AfxMessageBox(L"To proceed with error check before starting to autogenerate site logic data\nYes: Generate site logic data after error check\nNo : Generate site logic data without error checking", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		AutoMakeStart_XMake();
	}
	else
	{
		CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

		if (pMainWnd == nullptr)
		{
			return;
		}

		pMainWnd->StartErrorCheck(ERR_CHECK_MAKEAUTOLINK, this);
	}
#endif
	
}


void CFormAutoMake::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 	if (SaveAutoLink() > 0)
	// 		AfxMessageBox(L"생성된 연동데이터 저장이 완료 되었습니다." , MB_OK|MB_ICONINFORMATION);

	//20250827 GBM start - 기존에 프로세스 우선순위를 높이던 방식에서 스레드 우선순위를 높이는 방식으로 변경
#ifdef SLP4_MODE
	m_pMakeThread = AfxBeginThread((AFX_THREADPROC)Thread_SaveProc,
		(LPVOID)this,THREAD_PRIORITY_HIGHEST);
#else
	m_pMakeThread = AfxBeginThread((AFX_THREADPROC)Thread_SaveProc,
		(LPVOID)this);
#endif
	//20250827 GBM end
}

LRESULT CFormAutoMake::OnMakeProgress(WPARAM wp,LPARAM lp)
{
	CString str;
	int nP = 0;
	switch(lp)
	{
	case PROG_RESULT_CANCEL:
#ifndef ENGLISH_MODE
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"사용자가 취소했습니다.");
#else
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"User canceled.");
#endif
		m_ctrlProg.SetPos(100);
		break;
	case PROG_RESULT_ERROR:
#ifndef ENGLISH_MODE
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"생성 중 오류가 발생했습니다.");
#else
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"An error has occurred during creation.");
#endif
		m_ctrlProg.SetPos(100);
		KillTimer(TM_PROG_TIMER);
		break;
	case PROG_RESULT_TIMER_START:
		SetTimer(TM_PROG_TIMER,1000,nullptr);
		m_nCurTimerIdx = wp;
		nP = (int)(((float)wp / (float)m_nAllCnt) * 100);
#ifndef ENGLISH_MODE
		str.Format(L"[%d]%% 작업중(%d/%d) : 연동입력 개수(%d)",nP,wp,m_nAllCnt,m_nAllCnt - m_nTimePrgCnt);
#else
		str.Format(L"[%d]% in progress ([%d]/[%d]) interlocked input count ([%d])",nP,wp,m_nAllCnt,m_nAllCnt - m_nTimePrgCnt);
#endif
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlProg.SetPos(nP);
		break;
	case PROG_RESULT_TIMER_END:
		KillTimer(TM_PROG_TIMER);
		break;
	case PROG_RESULT_STEP:
		nP = (int)(((float)wp / (float)m_nAllCnt) * 100);
#ifndef ENGLISH_MODE
		str.Format(L"[%d]%% 작업중(%d/%d) :연동입력 개수(%d)",nP,wp,m_nAllCnt,m_nAllCnt - m_nTimePrgCnt);
#else
		str.Format(L"[%d]% in progress ([%d]/[%d]) interlocked input count ([%d])",nP,wp,m_nAllCnt,m_nAllCnt - m_nTimePrgCnt);
#endif
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlProg.SetPos(nP);
		break;
	case PROG_RESULT_FINISH:
#ifndef ENGLISH_MODE
		str.Format(L"%d개의 연동데이터를 생성했습니다.",m_nAllCnt - m_nTimePrgCnt);
#else
		str.Format(L"You have created [%d] piece(s) of site logic data.",m_nAllCnt - m_nTimePrgCnt);
#endif
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlProg.SetPos(100);
		KillTimer(TM_PROG_TIMER);
		//DisplayAutoMake();
		break;
	}
	return 1;
}

void CFormAutoMake::OnBnClickedBtnStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bStopFlag = TRUE;
	SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_CANCEL);
}


void CFormAutoMake::OnTvnSelchangedDeviceTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMTreeView->itemNew.hItem == nullptr)
		return; 
	DisplayList_XMake(pNMTreeView->itemNew.hItem);

}

BOOL CFormAutoMake::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, &m_brBackground);
	pDC->SetBkColor(m_crBack);
	pDC->SetBkMode(TRANSPARENT);
	return TRUE;
}


void CFormAutoMake::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (TM_PROG_TIMER == nIDEvent)
	{
		KillTimer(nIDEvent);
		int nP;
		CString str;
		m_nCurTimerIdx++;
		nP = (int)(((float)m_nCurTimerIdx / (float)m_nAllCnt) * 100);
#ifndef ENGLISH_MODE
		str.Format(L"[%d]%% 작업중(%d/%d) : 연동입력 개수(%d)", nP, m_nCurTimerIdx, m_nAllCnt, m_nAllCnt - m_nTimePrgCnt);
#else
		str.Format(L"[%d]% in progress (%d/%d) interlocked input count (%d)", nP, m_nCurTimerIdx, m_nAllCnt, m_nAllCnt - m_nTimePrgCnt);
#endif
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlProg.SetPos(nP);
		if(m_bStopFlag == FALSE)
			SetTimer(TM_PROG_TIMER, 1000, nullptr);
	}
	CFormView::OnTimer(nIDEvent);
}


LRESULT CFormAutoMake::OnErrorCheckEnd(WPARAM wp,LPARAM lp)
{
	switch(wp)
	{
	case ERR_CHECK_SIMPLE:
		break;
	case ERR_CHECK_CREATELINK:
// 		if(lp == 0)
// 			CreateFacpLink();
// 		else if(lp == -1)
// 			AfxMessageBox(L"사용자가 취소 했습니다.");
// 		else
// 			AfxMessageBox(L"오류가 발생하여 컴파일 할 수 없습니다.");

		break;
	case ERR_CHECK_MAKEAUTOLINK:
#ifndef ENGLISH_MODE
		if(lp == 0)
			AutoMakeStart_XMake();
		else if(lp == -1)
			AfxMessageBox(L"사용자가 취소했습니다.");
		else
			AfxMessageBox(L"오류가 발생하여 연동데이터 자동생성을 할 수 없습니다.");
#else
		if (lp == 0)
			AutoMakeStart_XMake();
		else if (lp == -1)
			AfxMessageBox(L"User canceled.");
		else
			AfxMessageBox(L"An error has occurred and the site logic data could not be autogenerated.");
#endif
		break;
	}
	return 0;
}


int CFormAutoMake::AutoMakeStart_XMake()
{
	// 전체 퍼센트는 계통의 개수
	// 
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_dwStart = GetTickCount();

	int nLogicCnt = 0;
	if(m_pMakeLink == nullptr)
		m_pMakeLink = new CXMakeLink;
	if(theApp.GetRelayTableData() == nullptr)
	{
		AfxMessageBox(L"프로젝트 정보가 없습니다.\n프로젝트를 열고 다시 시도해주십시요");
		return 0;
	}

	std::shared_ptr<CManagerAutoLogic> spList = theApp.GetRelayTableData()->GetAutoLogicManager();
	if(spList == nullptr || spList->GetCount() <= 0)
	{
		AfxMessageBox(L"프로젝트에 자동생성 로직 정보가 없습니다.\n프로젝트를 열고 다시 시도해주십시요");
		return 0;
	}
	nLogicCnt = m_pMakeLink->InitBasicLinkData(this);
	if(nLogicCnt == 0)
	{
		AfxMessageBox(L"연동데이터 자동생성하는데 실패 했습니다.(데이터 초기화 실패)");
		return 0;
	}
	m_nAllCnt = nLogicCnt;
	m_nTimePrgCnt = 0;
	m_bStopFlag = FALSE;
	m_pMakeThread = AfxBeginThread((AFX_THREADPROC)Thread_MakeProc_XMake,
		(LPVOID)this);
	return 1;
}

DWORD CFormAutoMake::Thread_MakeProc_XMake(LPVOID lpData)
{
	CFormAutoMake	*me;
	int nRet = 0;
	me = (CFormAutoMake *)lpData;
	if((CFormAutoMake *)NULL == me)
	{
		me->SendMessage(CSWM_PROGRESS_STEP,0,0);
		return 0;
	}

	try
	{
		nRet = me->GenerateAutoLinkData_XMake();
	}
	catch(...)
	{
	}
	return 0;
}

int CFormAutoMake::GenerateAutoLinkData_XMake()
{
	BOOL bRet = FALSE;
#if _DBG_MAKE_TIME_
	COleDateTime dtCur;
	DWORD_PTR dwStart,dwEnd,dwTemp;
	dtCur = COleDateTime::GetCurrentTime();
	GF_AddDebug(L"자동생성 시작: %s",dtCur.Format(L"%H:%M:%S"));
	dwStart = GetTickCount();
	dwTemp = dwStart;
#endif
	SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_STEP);

	m_vtInputDev.clear();

	bRet = m_pMakeLink->RunMakeLink(m_vtInputDev);
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();
	GF_AddDebug(L"   - RunMakeLink 종료: %d",dwEnd - dwTemp);
	dwTemp = dwEnd;
#endif


#if _DBG_MAKE_TIME_
	dtCur = COleDateTime::GetCurrentTime();
	GF_AddDebug(L"자동생성 종료 : %s",dtCur.Format(L"%H:%M:%S"));
#endif


	SendMessage(CSWM_PROGRESS_STEP,100,PROG_RESULT_FINISH);
	DisplayAutoMake_XMake();
	return 1;
}


int CFormAutoMake::DisplayAutoMake_XMake()
{
	HTREEITEM hFacp[D_MAX_FACP_COUNT] = { nullptr };
	HTREEITEM hUnit[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT] = { nullptr };
	HTREEITEM hChn[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT][5] = { nullptr };
	HTREEITEM hItem;

	CXDataDev * pDev;
	BOOL bCross = FALSE;
	int nF,nU,nC,nD;
	int nLastF,nLastU,nLastC,nLastD;
	int nExceptionCnt = 0;
	CDataFacp * pFacp = nullptr;
	CDataUnit * pUnit = nullptr;
	CDataChannel * pChn = nullptr;
	nF = nU = nC = nD = -1;
	nLastF = nLastU = nLastC = nLastD = -1;
	int nMaxLinkCount = CNewInfo::Instance()->m_nMaxLinkCount;

	m_ctrlTree.SetRedraw(FALSE);
	for(auto src : m_vtInputDev)
	{
		pDev = src.second;
		if(pDev == nullptr)
			continue;

		nF = pDev->GetFacpID();
		nU = pDev->GetUnitID();
		nC = pDev->GetChnID();
		nD = pDev->GetDeviceID();

		if(nF != nLastF)
		{
			pFacp = m_pRefFasSysData->GetFacpByID(nF);
			if(nF > 0 && hFacp[nF] == nullptr)
			{
				if(pFacp != nullptr)
				{
					hFacp[nF]
						= m_ctrlTree.InsertItem(pFacp->GetFacpName(),TIMG_DEV_FACP,TIMG_DEV_FACP,TVI_ROOT);
					m_ctrlTree.SetItemData(hFacp[nF],(DWORD_PTR)0);
				}
			}
		}

		if(nF > 0 && nU > 0
			&& hUnit[nF][nU] == nullptr)
		{
			// Unit 입력
			//if(pUnit == nullptr)
			{
				pUnit = m_pRefFasSysData->GetUnitByID(nF,nU);
			}

			if(pUnit != nullptr && hFacp[nF] != nullptr)
			{
				hUnit[nF][nU]
					= m_ctrlTree.InsertItem(pUnit->GetUnitName()
						,TIMG_DEV_UNIT,TIMG_DEV_UNIT,hFacp[nF]);
				//m_ctrlTree.SetItemData(hUnit[nF][nU],(DWORD_PTR)pUnit);
				m_ctrlTree.SetItemData(hUnit[nF][nU],(DWORD_PTR)0);
			}

		}
		if(nF > 0 && nU > 0 && nC > 0
			&& hChn[nF][nU][nC] == nullptr)
		{
			// Facp 입력
			//if(pChn == nullptr)
			{
				pChn = m_pRefFasSysData->GetChannelByID(nF,nU,nC);
			}

			if(pChn != nullptr &&hUnit[nF][nU] != nullptr)
			{
				hChn[nF][nU][nC] = m_ctrlTree.InsertItem(pChn->GetChnName()
					,TIMG_DEV_CHANNEL,TIMG_DEV_CHANNEL
					,hUnit[nF][nU]);
				//m_ctrlTree.SetItemData(hChn[nF][nU][nC],(DWORD_PTR)pChn);
				m_ctrlTree.SetItemData(hChn[nF][nU][nC],(DWORD_PTR)0);
			}

		}

		if(nD > 0 && pDev != nullptr && hChn[nF][nU][nC] != nullptr)
		{
			hItem = m_ctrlTree.InsertItem(pDev->GetInputFullName()
				,TIMG_DEV_DEVICE,TIMG_DEV_DEVICE
				,hChn[nF][nU][nC]);
			m_ctrlTree.SetItemData(hItem,(DWORD_PTR)pDev);

			if(pDev->m_MapLink.size() > nMaxLinkCount)
			{
				nExceptionCnt++;
				GF_AddLog(L"** 오류 예상됨 : %s의 출력 개수가 %d개 입니다.",pDev->GetInputFullName(),pDev->m_MapLink.size());
			}

			if(pDev->m_ptrPatternList.GetCount() + pDev->m_ptrEtcList.GetCount() > nMaxLinkCount)
			{
				nExceptionCnt++;
				GF_AddLog(L"** 오류 예상됨 : %s의 접점 개수가 %d개 입니다.",pDev->GetInputFullName()
					,pDev->m_ptrPatternList.GetCount() + pDev->m_ptrEtcList.GetCount());
			}

		}
	}

	m_dwEnd = GetTickCount();
	CString str;
	str.Format(
		L"연동데이터 자동생성을 완료 했습니다.\n"
		L"연동데이터 자동생성 시간 : %.4f\n"
		, nMaxLinkCount
		, nExceptionCnt
		, ((float)(m_dwEnd - m_dwStart) / (float)1000));

	if (nExceptionCnt > 0)
	{
		CString strException;
		strException.Format(L"연동 출력이 %d개가 넘는 회로가 %d개 있습니다\n", nMaxLinkCount, nExceptionCnt);
		str += strException;
	}
	AfxMessageBox(str);

	m_ctrlTree.SetRedraw();
	m_ctrlTree.RedrawWindow();
	return 1;
}


int CFormAutoMake::DisplayList_XMake(HTREEITEM hItem)
{
	DWORD_PTR dwData;
	CXDataDev * pDev,*pTemp;
	CXDataEm * pEm;
	CDataLocBase * pLoc;
	CDataEquip * pIn,*pName,*pCont,*pOut;
	POSITION pos;
	CString strType = L"패턴",str;
	CString strIn,strOut,strCont,strName,strEqName,strAddr;
	CString strB,strBType,strStair,strFloor,strRoom;
	int nIdx = 0,nPtnCnt = 0;
	int nMaxLinkCount = CNewInfo::Instance()->m_nMaxLinkCount;

	m_ctrlList.SetRedraw(FALSE);
	m_ctrlList.DeleteAllItems();

	pIn = pName = pCont = pOut = nullptr;
	dwData = m_ctrlTree.GetItemData(hItem);
	if(dwData == 0)
	{
		m_ctrlList.SetRedraw();
		return 0;
	}

	pDev = (CXDataDev *)dwData;
	pIn = pDev->GetEqInput();
	pName = pDev->GetEqName();

	strAddr = strIn = strOut = strCont = strName = strEqName = L"";
	if(pIn)
		strIn = pIn->GetEquipName();
	if(pName)
		strEqName = pName->GetEquipName();


	strType = L"출력";

	CXPatternMst * pPtn;
	CXDataLink * pLink;
	POSITION pPos;
	std::vector<std::pair<DWORD,CXDataLink*>> linksort(pDev->m_MapLink.begin(),pDev->m_MapLink.end());
	std::sort(linksort.begin(),linksort.end(),
		[](std::pair<DWORD,CXDataLink*>& a,std::pair<DWORD,CXDataLink*>& b)
	{
		//key 기준 정렬
		return a.first < b.first;		// key 오름차순 정렬
	});

	for(auto out : linksort)
	{
		if(out.second == nullptr)
			continue;
		pLink = out.second;
		if(pLink->m_lpRefData == nullptr)
			continue;
		pTemp = (CXDataDev*)pLink->m_lpRefData;
		strAddr = strOut = strCont = strName = L"";
		strB = strBType = strStair = strFloor = strRoom = L"";

		pCont = pTemp->GetEqOutContents();
		pOut = pTemp->GetEqOutput();
		if(pCont)
			strCont = pCont->GetEquipName();
		if(pOut)
			strOut = pOut->GetEquipName();

		pLoc = pTemp->GetDevOutputLocation();
		if(pLoc != nullptr)
		{
			strB = pLoc->GetBuildName();
			strBType = pLoc->GetBTypeName();
			strStair = pLoc->GetStairName();
			strFloor = pLoc->GetFloorName();
			strRoom = pLoc->GetRoomName();
		}
		strType = L"출력";
		strName = pTemp->GetOutputFullName();
		strAddr.Format(L"%s",pTemp->GetDevAddress());
		//WriteXMakeLinkLog(L"Input:%s-Device:%s",pDev->GetDevKey(),pTemp->GetDevKey());
		m_ctrlList.InsertItem(nIdx,strType);					//_T("종류"), LVCFMT_LEFT, 80);
		m_ctrlList.SetItemText(nIdx,1,strName);//_T("출력/패턴이름"), LVCFMT_LEFT, 200);
		m_ctrlList.SetItemText(nIdx,2,strIn);//_T("입력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,3,strEqName);//_T("설비명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,4,strOut);//_T("출력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,5,strCont);//_T("출력설명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,6,strAddr);
		m_ctrlList.SetItemText(nIdx,7,strB);			//_T("건물"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,8,strBType);	  //_T("건물종류"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,9,strStair);	  //_T("계단"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,10,strFloor);	  //_T("층"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,11,strRoom);	  //_T("실"), LVCFMT_LEFT, 150);
		if(nIdx >= nMaxLinkCount)
			m_ctrlList.SetItemColors(nIdx,0,RGB(255,255,255),RGB(255,0,0));
		nIdx++;

	}
	pPos = pDev->m_ptrPatternList.GetHeadPosition();
	while(pPos)
	{
		pPtn = (CXPatternMst*)pDev->m_ptrPatternList.GetNext(pPos);
		if(pPtn == nullptr)
			continue;
		//WriteXMakeLinkLog(L"Input:%s-Pattern:%s",pDev->GetDevKey(),pPtn->m_strPatternName);
		strType = L"패턴";
		strName = pPtn->m_strPatternName;
		strAddr.Format(L"%d",pPtn->m_nPatternID);
		m_ctrlList.InsertItem(nIdx,strType);					//_T("종류"), LVCFMT_LEFT, 80);
		m_ctrlList.SetItemText(nIdx,1,strName);//_T("출력/패턴이름"), LVCFMT_LEFT, 200);
		m_ctrlList.SetItemText(nIdx,2,strIn);//_T("입력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,3,strEqName);//_T("설비명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,4,strOut);//_T("출력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,5,strCont);//_T("출력설명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,6,strAddr);
		m_ctrlList.SetItemText(nIdx,7,strB);			//_T("건물"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,8,strBType);	  //_T("건물종류"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,9,strStair);	  //_T("계단"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,10,strFloor);	  //_T("층"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,11,strRoom);	  //_T("실"), LVCFMT_LEFT, 150);
		if(nPtnCnt >= nMaxLinkCount)
			m_ctrlList.SetItemColors(nIdx,0,RGB(255,255,255),RGB(255,0,0));
		nIdx++;
		nPtnCnt ++;

	}


	pos = pDev->m_ptrEtcList.GetHeadPosition();
	while(pos)
	{
		pLink = (CXDataLink *)pDev->m_ptrEtcList.GetNext(pos);
		if(pLink == nullptr || pLink->m_nLinkType != LK_TYPE_EMERGENCY)
			continue;
		pEm = (CXDataEm *)pLink->m_lpRefData;
		if(pEm == nullptr)
			continue;
		strB = strBType = strStair = strFloor = strRoom = L"";

		m_ctrlList.InsertItem(nIdx,L"비상방송");					//_T("종류"), LVCFMT_LEFT, 80);
		m_ctrlList.SetItemText(nIdx,1,pEm->GetEmName());//_T("출력/패턴이름"), LVCFMT_LEFT, 200);
		m_ctrlList.SetItemText(nIdx,2,strIn);//_T("입력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,3,strName);//_T("설비명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,4,L"");//_T("출력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,5,L"");//_T("출력설명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,6,pEm->GetEmAddr());
		m_ctrlList.SetItemText(nIdx,7,pEm->GetBuildName());			//_T("건물"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,8,L"");	  //_T("건물종류"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,9,pEm->GetStairName());	  //_T("계단"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,10,pEm->GetFloorName());	  //_T("층"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx,11,L"");	  //_T("실"), LVCFMT_LEFT, 150);
		nIdx++;
	}
	m_ctrlList.SetRedraw();
	return 1;
}

int CFormAutoMake::ProcessSaveAutoLink_XMake()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_BTN_MAKE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	if(SaveAutoLink_XMake() > 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"생성된 연동데이터 저장이 완료 되었습니다.\n프로그램이 재시작됩니다.",MB_OK | MB_ICONINFORMATION);
#else
		AfxMessageBox(L"Saving of the created linked data has been completed.\nThe program will restart.", MB_OK | MB_ICONINFORMATION);
#endif
		//RegisterApplicationRestart(L"--restart",RESTART_NO_CRASH | RESTART_NO_HANG);
		//AfxGetMainWnd()->PostMessageW(WM_CLOSE);
		theApp.RequestRestart();
		return 0;
	}

	GetDlgItem(IDC_BTN_MAKE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	return 1;
}


int CFormAutoMake::SaveAutoLink_XMake()
{
	//20260224 GBM start - 시간 측정
	LARGE_INTEGER startTime, endTime;
	QueryPerformanceCounter(&startTime);

	m_nAllCnt = g_stConfig.dwTimeOut;
	m_nAllCnt += g_stConfig.dwTimeOut;
	m_nAllCnt += m_vtInputDev.size();

	if(m_pRefFasSysData == nullptr)
	{
		SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_ERROR);
		GF_AddLog(L"프로젝트가 닫혀있는 상태입니다. 프로젝트를 열고 다시 시도해주십시요.");
		AfxMessageBox(L"프로젝트가 닫혀있는 상태입니다. 프로젝트를 열고 다시 시도해주십시요");
		return 0;
	}
	int nIdx = 0,nRet = 0;
	int n1,n2;
	int nProgOffset = 0;
	BOOL bCross = FALSE;
	CString strKey,strSql;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();

	if(pDb->DropCleanBuffer() == FALSE)
	{
		AfxMessageBox(L"데이터베이스의 메모리 버퍼를 초기화 하는데 실패 했습니다.");
		return 0;
	}
	pDb->BeginTransaction();

	SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_TIMER_END);
	SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_TIMER_START);
	//  [9/20/2022 KHS] Index 제거
	if(m_pRefFasSysData->TempFunc_DropIndex() <= 0)
	{

		SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_TIMER_END);
		pDb->RollbackTransaction();
		//m_pNewRelayTable->SendProgStep(this,PROG_RESULT_ERROR,0,0);
		return 0;
	}
	SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_TIMER_END);
	nProgOffset = g_stConfig.dwTimeOut;
	strSql.Format(L"DELETE FROM TB_LINK_RELAY WHERE LG_TYPE=%d",LOGIC_ALL_AUTO);
	if(pDb->ExecuteSql(strSql) == FALSE)
	{
		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
		AfxMessageBox(L"전체 연동출력을 삭제하는데 실패 했습니다.");
		pDb->RollbackTransaction();
		return 0;
	}
	CXDataDev * pInputDev;
	for(auto it : m_vtInputDev)
	{
		if(m_bStopFlag == TRUE)
			return -1;

		pInputDev = it.second;
		if(pInputDev == nullptr)
		{
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}

		bCross = FALSE;
		n1 = n2 = 0;
		if(pInputDev->GetEqInput())
			n1 = pInputDev->GetEqInput()->GetEquipID();
		if(n1 == INTYPE_CROSSA || n1 == INTYPE_CROSSB
			|| n1 == INTYPE_CROSS16_A || n1 == INTYPE_CROSS17_B
			|| n1 == INTYPE_CROSS18_A || n1 == INTYPE_CROSS19_B
			)
			bCross = TRUE;
		nRet = SaveIndividualAutoLink_XMake(pDb,pInputDev,bCross);
		if(nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}

		nRet = SaveIndividualEmergency_XMake(pDb,pInputDev);
		if(nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}
		nRet = SaveIndividualPattern_XMake(pDb,pInputDev);
		if(nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}


		nIdx++;
		nProgOffset += 1;
		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_STEP);

	}

	nRet = DeleteManualLink_XMake(pDb);
	if(nRet <= 0)
	{
		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
		pDb->RollbackTransaction();
		return 0;
	}


	SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_TIMER_START);
	if(m_pRefFasSysData->TempFunc_CheckIndex() <= 0)
	{
		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
		pDb->RollbackTransaction();
		return 0;
	}


	nProgOffset += g_stConfig.dwTimeOut;
	SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_FINISH);
	pDb->CommitTransaction();

	QueryPerformanceCounter(&endTime);
	float duringTime;
	duringTime = CCommonFunc::GetPreciseDeltaTime(startTime, endTime);
	Log::Trace("자동 생성된 연동데이터 적용에 걸린 시간 : %f", duringTime);
	//20260226 GBM end

	return 1;
}


int CFormAutoMake::SaveIndividualAutoLink_XMake(YAdoDatabase * pDb,CXDataDev * pInputDev,BOOL bCross)
{
	CString strSql = L"",strtemp = L"";
	CXDataLink * pLink;
	int nCnt = 0;
	BOOL bFirst = FALSE;

	for(auto it : pInputDev->m_MapLink)
	{
		pLink = it.second;
		if(pLink == nullptr || pLink->GetFacpID() < 0 || pLink->GetUnitID() < 0 || pLink->GetChnID() < 0 || pLink->GetDeviceID() < 0)
			continue;

		strtemp.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
			L", LINK_TYPE,LG_TYPE , LG_ID "
			L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
			L", ADD_USER) "
			L" VALUES(%d,%d,%d,%d"
			L", %d,%d,%d"
			L", %d,%d,%d,%d"
			L", '%s') ;\n"
			,pInputDev->GetFacpID(),pInputDev->GetUnitID(),pInputDev->GetChnID(),pInputDev->GetDeviceID()
			,LK_TYPE_RELEAY,LOGIC_ALL_AUTO,pLink->m_nLogicID
			,pLink->GetFacpID(),pLink->GetUnitID(),pLink->GetChnID(),pLink->GetDeviceID()
			,m_pRefFasSysData->GetCurrentUser()
		);

		strSql += strtemp;
		nCnt ++;
		if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
		{
			pDb->ExecuteSql(strSql);
			nCnt = 0;
		}
	}

	if(nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	// 수동으로 넣은 항목이 있는지 확인

	return 1;
}


int CFormAutoMake::SaveIndividualEmergency_XMake(YAdoDatabase * pDb,CXDataDev * pInputDev)
{
	CString strSql = L"",strtemp = L"";
	POSITION pos;
	CPtrList * pList;
	CXDataLink * pLink;
	int nCnt = 0;
	pList = &pInputDev->m_ptrEtcList;
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pLink = (CXDataLink *)pList->GetNext(pos);
		if(pLink == nullptr)
			continue;
		strtemp.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
			L", LINK_TYPE,LG_TYPE , LG_ID "
			L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
			L", ADD_USER) "
			L" VALUES(%d,%d,%d,%d"
			L", %d,%d,%d"
			L", %d,%d,%d,%d"
			L", '%s') ;\n"
			,pInputDev->GetFacpID(),pInputDev->GetUnitID(),pInputDev->GetChnID(),pInputDev->GetDeviceID()
			,LK_TYPE_EMERGENCY,LOGIC_ALL_AUTO,pLink->m_nLogicID
			,pLink->GetEmID(),0,0,0
			,m_pRefFasSysData->GetCurrentUser()
		);

		strSql += strtemp;
		nCnt ++;
		if(strSql.GetLength() > 65535)
		{
			pDb->ExecuteSql(strSql);
			nCnt = 0;
		}
	}

	if(nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	return 1;
}

int CFormAutoMake::SaveIndividualPattern_XMake(YAdoDatabase * pDb,CXDataDev * pInputDev)
{
	CString strSql = L"",strtemp = L"";
	POSITION pos;
	CPtrList * pList;
	CXPatternMst * pMst;
	int nID = 0,nCnt = 0;
	pList = &pInputDev->m_ptrPatternList;
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pMst = (CXPatternMst *)pList->GetNext(pos);
		if(pMst == nullptr)
			continue;
		nID = pMst->m_nPatternID;
		strtemp.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
			L", LINK_TYPE,LG_TYPE , LG_ID "
			L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
			L", ADD_USER) "
			L" VALUES(%d,%d,%d,%d"
			L", %d,%d,%d"
			L", %d,%d,%d,%d"
			L", '%s') ;\n"
			,pInputDev->GetFacpID(),pInputDev->GetUnitID(),pInputDev->GetChnID(),pInputDev->GetDeviceID()
			,LK_TYPE_PATTERN,LOGIC_ALL_AUTO,D_NUM_AUTO_PTN_LOGIC_ID
			,pMst->m_nPatternID,0,0,0
			,m_pRefFasSysData->GetCurrentUser()
		);
		strSql += strtemp;
		nCnt ++;
		if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
		{
			pDb->ExecuteSql(strSql);
			nCnt = 0;
		}
	}

	if(nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	return 1;
}

int CFormAutoMake::DeleteManualLink_XMake(YAdoDatabase * pDb)
{
	CString strSql = L"";
	int nCnt = 0;
	BOOL bRet = FALSE;

	// 연동 데이터 중에 수동,자동이 동시에 들어간 중복 회로 삭제
	// 삭제 시 수동으로 들어간 항목 삭제
	strSql.Format(
		L"DELETE A"
		L" FROM TB_LINK_RELAY A JOIN"
		L" ("
		L" 	SELECT SRC_FACP,SRC_UNIT,SRC_CHN,SRC_RLY,TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY,LINK_TYPE,MAX(LG_ID) AS LGID"
		L" 	FROM TB_LINK_RELAY"
		L" 	GROUP BY SRC_FACP,SRC_UNIT,SRC_CHN,SRC_RLY,TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY,LINK_TYPE"
		L" 	HAVING COUNT(DISTINCT LG_TYPE) > 1"
		L" 	OR COUNT(DISTINCT LG_ID) > 1" //
		L" ) B ON A.SRC_FACP = B.SRC_FACP AND A.SRC_UNIT = B.SRC_UNIT"
		L" AND A.SRC_CHN = B.SRC_CHN AND A.SRC_RLY = B.SRC_RLY"
		L" AND	A.TGT_FACP = B.TGT_FACP AND A.TGT_UNIT = B.TGT_UNIT"
		L" AND	A.TGT_CHN = B.TGT_CHN AND A.TGT_RLY = B.TGT_RLY"
		L" AND A.LINK_TYPE = B.LINK_TYPE"
		L" WHERE (A.LG_TYPE = 1 OR A.LG_ID <> B.LGID)" // 로직 타입이 수동인거 삭제
	);
	bRet = pDb->ExecuteSql(strSql);
	if(bRet == FALSE)
		GF_AddLog(pDb->GetLastErrorString());
	return bRet;
}
