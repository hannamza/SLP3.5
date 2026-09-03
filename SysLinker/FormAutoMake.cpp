// FormAutoMake.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormAutoMake.h"

#include "CsvBulkWriter.h"

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
	m_spRefAutoLogic = nullptr;
}
#else
CFormAutoMake::CFormAutoMake()
	: CFormView(IDD_FORMAUTOMAKE_EN)
{
	m_pRefFasSysData = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
	m_pMakeLink = nullptr;
	m_spRefAutoLogic = nullptr;
}
#endif

CFormAutoMake::~CFormAutoMake()
{
	if(m_pMakeLink)
	{
		delete m_pMakeLink;
		m_pMakeLink = nullptr;
	}

	RemoveAllData();
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

		if (me->m_pRefFasSysData->m_bUseUILogic)
		{
			nRet = me->GenerateAutoLinkData_XMake();
		}
		else
		{
			nRet = me->GenerateAutoLinkData2();
		}

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

		if (me->m_pRefFasSysData->m_bUseUILogic)
		{
			nRet = me->ProcessSaveAutoLink_XMake();
		}
		else
		{
			nRet = me->ProcessSaveAutoLink();
		}

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

int CFormAutoMake::AutoMakeStart()
{
	// 전체 퍼센트는 계통의 개수
	// 
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pRefFasSysData == nullptr)
		m_pRefFasSysData = theApp.GetRelayTableData();

	if (m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"연동데이터 자동생성하는데 실패했습니다.(중계기일람표 가져오기 실패)");
#else
		AfxMessageBox(L"Failed to autogenerate the site logic data. (Failed to retrieve the module table)");
#endif
		return 0;
	}

	m_spRefAutoLogic = m_pRefFasSysData->GetAutoLogicManager();

	if (m_spRefAutoLogic == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"연동데이터 자동생성하는데 실패했습니다.(자동생성 로직 가져오기 실패)");
#else
		AfxMessageBox(L"Failed to autogenerate the site logic data. (Failed to retrieve the autogeneration logic)");
#endif
		return 0;
	}
	m_bStopFlag = FALSE;
	if (InitAutoSystemData() < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"연동데이터 자동생성하는데 실패했습니다.(데이터 초기화 실패)");
#else
		AfxMessageBox(L"Failed to autogenerate the site logic data. (Failed to initialize data)");
#endif
		return 0;
	}

	//20250827 GBM start - 기존에 프로세스 우선순위를 높이던 방식에서 스레드 우선순위를 높이는 방식으로 변경
#ifdef SLP4_MODE
	// Thread Start
	m_pMakeThread = AfxBeginThread((AFX_THREADPROC)Thread_MakeProc,
		(LPVOID)this, THREAD_PRIORITY_HIGHEST);
#else
	// Thread Start
	m_pMakeThread = AfxBeginThread((AFX_THREADPROC)Thread_MakeProc,
		(LPVOID)this);
#endif
	//20250827 GBM end

	return 1;
}

void CFormAutoMake::OnBnClickedBtnMake()
{
	m_ctrlList.DeleteAllItems();
	m_ctrlTree.DeleteAllItems();
	RemoveAllData();

	if (m_pRefFasSysData->m_bUseUILogic)
	{
		//20260609 GBM start - 연동데이터 자동 생성 오류 검사 안함
#if 1
		AutoMakeStart_XMake();
#else
#ifndef ENGLISH_MODE
		if (AfxMessageBox(L"연동데이터 자동생성 시작전 오류검사를 진행하실려면\nYes : 오류검사 후 연동데이터 생성\nNo:오류검사없이 연동데이터 생성", MB_YESNO | MB_ICONQUESTION) != IDYES)
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
#endif
		//20260609 GBM end

	}
	else
	{
		AutoMakeStart();
	}
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

	if (m_pRefFasSysData->m_bUseUILogic)
	{
		DisplayList_XMake(pNMTreeView->itemNew.hItem);
	}
	else
	{
		DisplayList(pNMTreeView->itemNew.hItem);
	}
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
#ifndef ENGLISH_MODE
	str.Format(
		L"연동데이터 자동생성을 완료 했습니다.\n"
		L"연동데이터 자동생성 시간 : %.4f"
		, ((float)(m_dwEnd - m_dwStart) / (float)1000));
#else
	str.Format(
		L"Automatic generation of linked data has been completed.\n"
		L"Linked data automatic generation time : %.4f"
		, ((float)(m_dwEnd - m_dwStart) / (float)1000));
#endif

	if (nExceptionCnt > 0)
	{
		CString strException;
#ifndef ENGLISH_MODE
		strException.Format(L"\n연동 출력이 %d개가 넘는 회로가 %d개 있습니다.", nMaxLinkCount, nExceptionCnt);
#else
		strException.Format(L"\nThere are %d circuits with more than %d interlocked outputs.", nExceptionCnt, nMaxLinkCount);
#endif
		str += strException;
	}
	AfxMessageBox(str);
	Log::Trace("%s", CCommonFunc::WCharToChar(str.GetBuffer(0)));

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
	//if (SaveAutoLink_XMake() > 0)
	if (SaveAutoLink_XMake_BulkInsert() > 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"생성된 연동데이터 저장이 완료 되었습니다.\n프로그램이 재시작됩니다.",MB_OK | MB_ICONINFORMATION);
#else
		AfxMessageBox(L"Saving of the created linked data has been completed.\nThe program will restart.", MB_OK | MB_ICONINFORMATION);
#endif
		theApp.CloseProject();	// 사용자가 다시 현재 프로젝트를 열거라는 전제에서는 할 필요없지만 꼭 그렇다고 볼 수는 없으므로 프로젝트 DB 파일을 Detach하기 위해 실행
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
		Log::Trace("DELETE FROM TB_LINK_RELAY WHERE LG_TYPE=3 failed.");
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




// -----------------------------------------------------------------------------
// [2026/03/04] Bulk insert using CSV + staging table
//  - CSV contains 12 columns (ALL except ADD_DATE):
//    SRC_FACP,SRC_UNIT,SRC_CHN,SRC_RLY,LINK_TYPE,LG_TYPE,LG_ID,
//    TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY,ADD_USER
//  - BULK INSERT into dbo.TB_LINK_RELAY_STAGE
//  - INSERT INTO dbo.TB_LINK_RELAY(..., ADD_USER) SELECT ... (ADD_DATE uses DEFAULT GETDATE())
// -----------------------------------------------------------------------------

bool CFormAutoMake::EnsureDirectoryExistsA(const CString& dir)
{
    if(dir.IsEmpty())
        return false;

    DWORD attr = ::GetFileAttributes(dir);
    if(attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY))
        return true;

    // Create parents recursively.
    int pos = 0;
    while(true)
    {
        int next = dir.Find(L'\\', pos);
        CString part = (next < 0) ? dir : dir.Left(next);

        // Skip drive root like "C:"
        if(part.GetLength() >= 3 && part.Right(1) != L":")
            ::CreateDirectory(part, nullptr);

        if(next < 0) break;
        pos = next + 1;
    }

    attr = ::GetFileAttributes(dir);
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

CString CFormAutoMake::GetDefaultBulkCsvPathA()
{
    // Use ProgramData so SQL Server service account can usually read it.
	CString strPrjPath = g_stConfig.szPrjPath;
	CString dir;
	dir.Format(_T("%s\\%s\\%s"), strPrjPath, m_pRefFasSysData->GetPrjName(), F3_VERSIONTEMPFOLDER_NAME);

	// 폴더 없으면 빈 문자열 리턴
	if (!EnsureDirectoryExistsA(dir))
	{
		return _T("");
	}

	CString file = BULK_INSERT_CSV_FILE;
	return dir + _T("\\") + file;
}

bool CFormAutoMake::EnsureStageTableA(YAdoDatabase* pDb)
{
    // Stage table excludes ADD_DATE (datetime default in final table)
    CString sql =
        L"IF OBJECT_ID('dbo.TB_LINK_RELAY_STAGE','U') IS NULL "
        L"BEGIN "
        L"  CREATE TABLE dbo.TB_LINK_RELAY_STAGE ("
        L"    SRC_FACP  INT NOT NULL,"
        L"    SRC_UNIT  INT NOT NULL,"
        L"    SRC_CHN   INT NOT NULL,"
        L"    SRC_RLY   INT NOT NULL,"
        L"    LINK_TYPE SMALLINT NOT NULL,"
        L"    LG_TYPE   SMALLINT NOT NULL,"
        L"    LG_ID     INT NOT NULL,"
        L"    TGT_FACP  INT NOT NULL,"
        L"    TGT_UNIT  INT NOT NULL,"
        L"    TGT_CHN   INT NOT NULL,"
        L"    TGT_RLY   INT NOT NULL,"
        L"    ADD_USER  VARCHAR(50) NOT NULL"
        L"  );"
        L"END";

    if(!pDb->ExecuteSql(sql))
        return false;

    // Clean stage for this run
    if(!pDb->ExecuteSql(L"TRUNCATE TABLE dbo.TB_LINK_RELAY_STAGE"))
        return false;

    return true;
}

int CFormAutoMake::SaveIndividualAutoLink_XMake_Csv(CCsvBulkWriter& w, CXDataDev* pInputDev, BOOL /*bCross*/)
{
    for(auto it : pInputDev->m_MapLink)
    {
        CXDataLink* pLink = it.second;
        if(pLink == nullptr || pLink->GetFacpID() < 0 || pLink->GetUnitID() < 0 || pLink->GetChnID() < 0 || pLink->GetDeviceID() < 0)
            continue;

        w.AppendRow(
            pInputDev->GetFacpID(), pInputDev->GetUnitID(), pInputDev->GetChnID(), pInputDev->GetDeviceID(),
            LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLink->m_nLogicID,
            pLink->GetFacpID(), pLink->GetUnitID(), pLink->GetChnID(), pLink->GetDeviceID(),
            m_pRefFasSysData->GetCurrentUser()
        );
    }
    return 1;
}

int CFormAutoMake::SaveIndividualEmergency_XMake_Csv(CCsvBulkWriter& w, CXDataDev* pInputDev)
{
    POSITION pos;
    CPtrList* pList = &pInputDev->m_ptrEtcList;
    pos = pList->GetHeadPosition();
    while(pos)
    {
        CXDataLink* pLink = (CXDataLink*)pList->GetNext(pos);
        if(pLink == nullptr)
            continue;

        w.AppendRow(
            pInputDev->GetFacpID(), pInputDev->GetUnitID(), pInputDev->GetChnID(), pInputDev->GetDeviceID(),
            LK_TYPE_EMERGENCY, LOGIC_ALL_AUTO, pLink->m_nLogicID,
            pLink->GetEmID(), 0, 0, 0,
            m_pRefFasSysData->GetCurrentUser()
        );
    }
    return 1;
}

int CFormAutoMake::SaveIndividualPattern_XMake_Csv(CCsvBulkWriter& w, CXDataDev* pInputDev)
{
    POSITION pos;
    CPtrList* pList = &pInputDev->m_ptrPatternList;
    pos = pList->GetHeadPosition();
    while(pos)
    {
        CXPatternMst* pMst = (CXPatternMst*)pList->GetNext(pos);
        if(pMst == nullptr)
            continue;

        w.AppendRow(
            pInputDev->GetFacpID(), pInputDev->GetUnitID(), pInputDev->GetChnID(), pInputDev->GetDeviceID(),
            LK_TYPE_PATTERN, LOGIC_ALL_AUTO, D_NUM_AUTO_PTN_LOGIC_ID,
            pMst->m_nPatternID, 0, 0, 0,
            m_pRefFasSysData->GetCurrentUser()
        );
    }
    return 1;
}

int CFormAutoMake::SaveAutoLink_XMake_BulkInsert()
{
	//20260304 GBM start - 시간 측정
	LARGE_INTEGER startTime, endTime;
	QueryPerformanceCounter(&startTime);

    // Keep progress logic similar to SaveAutoLink_XMake, but insert via CSV + BULK.
    m_nAllCnt = g_stConfig.dwTimeOut;
    m_nAllCnt += g_stConfig.dwTimeOut;
    m_nAllCnt += (int)m_vtInputDev.size();

    if(m_pRefFasSysData == nullptr)
    {
        SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_ERROR);
        AfxMessageBox(L"Project is not loaded.");
        return 0;
    }

    int nIdx = 0, nRet = 0;
    int n1, n2;
    int nProgOffset = 0;
    BOOL bCross = FALSE;
    CString strSql;
    YAdoDatabase* pDb = m_pRefFasSysData->GetPrjDB();

    if(pDb->DropCleanBuffer() == FALSE)
    {
        AfxMessageBox(L"Database clean buffer failed.");
        return 0;
    }

    pDb->BeginTransaction();

    SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_TIMER_END);
    SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_TIMER_START);

    if(m_pRefFasSysData->TempFunc_DropIndex() <= 0)
    {
        SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_TIMER_END);
        pDb->RollbackTransaction();
        return 0;
    }

    SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_TIMER_END);
    nProgOffset = g_stConfig.dwTimeOut;

	// 기존 자동 연동데이터 지우는데 시간이 아무래도 단 건보다 오래 걸리므로 Timeout을 0(무한정)으로 주고 실행한 뒤 원래 값으로 원복
	long nOldTimeout = pDb->m_pCommand->CommandTimeout;
	pDb->m_pCommand->CommandTimeout = 0;

    // Delete existing auto rows
    strSql.Format(L"DELETE FROM TB_LINK_RELAY WHERE LG_TYPE=%d", LOGIC_ALL_AUTO);
    if(pDb->ExecuteSql(strSql) == FALSE)
    {
		GF_AddLog(pDb->GetLastErrorString());
		Log::Trace("DELETE FROM TB_LINK_RELAY WHERE LG_TYPE=3 failed.");
        SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
        pDb->RollbackTransaction();
        return 0;
    }

    // Stage table
    if(!EnsureStageTableA(pDb))
    {
        GF_AddLog(pDb->GetLastErrorString());
		Log::Trace("EnsureStageTableA failed.");
        SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
        pDb->RollbackTransaction();
        return 0;
    }

    // CSV create
    CString csvPath = GetDefaultBulkCsvPathA();
	if (csvPath.IsEmpty())
	{
		Log::Trace("GetDefaultBulkCsvPathA failed.");
		SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
		pDb->RollbackTransaction();
		return 0;
	}

    CCsvBulkWriter writer;
    if(!writer.Open(csvPath))
    {
		Log::Trace("CCsvBulkWriter Open failed.");
        SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
        pDb->RollbackTransaction();
        return 0;
    }
    writer.WriteHeaderOnce();

    // Fill CSV
    for(auto it : m_vtInputDev)
    {
        if(m_bStopFlag == TRUE)
        {
            writer.Close();
            pDb->RollbackTransaction();
            return -1;
        }

        CXDataDev* pInputDev = it.second;
        if(pInputDev == nullptr)
        {
            writer.Close();
			Log::Trace("pInputDev == nullptr");
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
            || n1 == INTYPE_CROSS18_A || n1 == INTYPE_CROSS19_B)
            bCross = TRUE;

        nRet = SaveIndividualAutoLink_XMake_Csv(writer, pInputDev, bCross);
        if(nRet <= 0) 
		{ 
			Log::Trace("SaveIndividualAutoLink_XMake_Csv failed.");
			writer.Close(); 
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR); 
			pDb->RollbackTransaction(); return 0; 
		}

        nRet = SaveIndividualEmergency_XMake_Csv(writer, pInputDev);
        if(nRet <= 0) 
		{ 
			Log::Trace("SaveIndividualEmergency_XMake_Csv failed.");
			writer.Close(); 
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR); 
			pDb->RollbackTransaction(); 
			return 0; 
		}

        nRet = SaveIndividualPattern_XMake_Csv(writer, pInputDev);
        if(nRet <= 0) 
		{ 
			Log::Trace("SaveIndividualPattern_XMake_Csv failed.");
			writer.Close(); 
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR); 
			pDb->RollbackTransaction(); 
			return 0; 
		}

        nIdx++;
        nProgOffset += 1;
        SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_STEP);
    }

    writer.Close();

    // BULK INSERT into stage
    CString bulkSql;
    bulkSql.Format(
        L"BULK INSERT dbo.TB_LINK_RELAY_STAGE FROM '%s' WITH (FIRSTROW=2,FIELDTERMINATOR=',',ROWTERMINATOR='0x0d0a',TABLOCK,BATCHSIZE=50000,CODEPAGE='65001');",
        writer.GetPath().GetString()
    );

    if(!pDb->ExecuteSql(bulkSql))
    {
        GF_AddLog(pDb->GetLastErrorString());
		Log::Trace("Bulk Insert failed.");
        SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
        pDb->RollbackTransaction();
        return 0;
    }

	// csv 파일 삭제
	DeleteFile(writer.GetPath());

    // Move into final table. ADD_DATE uses DEFAULT GETDATE() because we omit it.
    CString moveSql =
        L"INSERT INTO dbo.TB_LINK_RELAY("
        L"SRC_FACP,SRC_UNIT,SRC_CHN,SRC_RLY,"
        L"LINK_TYPE,LG_TYPE,LG_ID,"
        L"TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY,"
        L"ADD_USER)"
        L" SELECT "
        L"SRC_FACP,SRC_UNIT,SRC_CHN,SRC_RLY,"
        L"LINK_TYPE,LG_TYPE,LG_ID,"
        L"TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY,"
        L"ADD_USER"
        L" FROM dbo.TB_LINK_RELAY_STAGE;";

    if(!pDb->ExecuteSql(moveSql))
    {
        GF_AddLog(pDb->GetLastErrorString());
		Log::Trace("Copying data from Stage Table failed.");
        SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
        pDb->RollbackTransaction();
        return 0;
    }

    // Remove duplicates between manual/auto (existing logic)
    nRet = DeleteManualLink_XMake(pDb);
    if(nRet <= 0)
    {
		GF_AddLog(pDb->GetLastErrorString());
		Log::Trace("DeleteManualLink_XMake failed.");
        SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
        pDb->RollbackTransaction();
        return 0;
    }

	//DB Timeout 원복
	pDb->m_pCommand->CommandTimeout = nOldTimeout;

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
	//20260304 GBM end

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
			strSql.Empty();
			nCnt = 0;
		}
	}

	if(nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		strSql.Empty();
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

void CFormAutoMake::RemoveAllData()
{
	int nIdx = 0;
	CString strKey;
	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
	CMapAutoSystem::iterator it;
	CDataAutoMake * pNewAuto;

	for (it = m_MapIDAuto.begin(); it != m_MapIDAuto.end(); it++)
	{
		pNewAuto = it->second;
		if (pNewAuto == nullptr)
			continue;
		delete pNewAuto;
		pNewAuto = nullptr;
		nIdx++;
		//SendMessage(CSWM_PROGRESS_STEP, nIdx, PROG_RESULT_STEP);
	}
	m_MapIDAuto.clear();

	m_ptrSortedDevice.RemoveAll();
}

int CFormAutoMake::InitAutoSystemData()
{
	int nAll = 0, nRet = 0;
	nAll = MakeAutoSystemData();
	if (nAll < 0)
		return 0;
	m_nAllCnt = nAll;
	// 	nRet = MakeSortedDeviceByLocation();
	// 	if (nRet > 0)
	// 		return nAll;

	m_nAllCnt += g_stConfig.dwTimeOut; // 600초 - TB_TEMP_SAVED_LINK 데이터 만들기 (SP_GENERATE_LINK)
	m_nAllCnt += g_stConfig.dwTimeOut; // 600초 - TB_TEMP_USED_PTN 데이터 만들기 (SP_GENERATE_PTN_BY_SOURCE)
	m_nAllCnt += g_stConfig.dwTimeOut; // 600초 - TB_TEMP_SAVED_LINK에서 PATTERN에 사용된 회로지우기(SP_DELETE_TEMPLINK_PTNITEM)
	m_nTimePrgCnt = g_stConfig.dwTimeOut * 3;
	return nRet;
}

int CFormAutoMake::GenerateAutoLinkData2()
{
	BOOL bRet = FALSE;
	int nIdx = 0, nCnt, i, nValue, nAddCnt = 0;
	int nExceptionCnt = 0, nRelayLinkCnt = 0;
	CString strKey, strSql;
	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
	CDataSystem * pSrcSys = nullptr;
	CDataDevice * pSrcDev = nullptr;
	CDataAutoMake * pNewAuto = nullptr;
	CDataLinked *pLink = nullptr;
	YAdoDatabase * pDBUtil = nullptr;

	CString strSB, strST, strSS, strSF, strSR, strTB, strTT, strTS, strTF, strTR;
	int nUID = 0, nFID, nCID, nRID;
	int nlf, nlu, nlc, nlr;
	int nProgOffset = 0;
	int nTFid, nTUid, nTCid, nTRid;
	int nLinkType, nLgID, nLgType;
	int nInType, nOutType, nEqName, nCont;
	BOOL bCross, bFirst;
	pDBUtil = new YAdoDatabase;
	pDBUtil->MSSqlInitialize(g_stConfig.szDBPass, g_stConfig.szDBUser
		, m_pRefFasSysData->GetPrjName(), g_stConfig.szSource);

	if (pDBUtil->DBOpen() == FALSE)
	{
#ifndef ENGLISH_MODE
		GF_AddDebug(L"자동생성 오류 : 데이터베이스를 연결하는데 실패했습니다.");
		AfxMessageBox(L"데이터베이스 접속에 실패했습니다.");
#else
		GF_AddDebug(L"Autogeneration error: failed to connect to the database.");
		AfxMessageBox(L"Failed to connect to the database.");
#endif
		return 0;
	}


#if _DBG_MAKE_TIME_
	COleDateTime dtCur;
	DWORD_PTR dwStart, dwEnd, dwTemp;
	dtCur = COleDateTime::GetCurrentTime();
#ifndef ENGLISH_MODE
	GF_AddDebug(L"자동생성 시작: %s", dtCur.Format(L"%H:%M:%S"));
#else
	GF_AddDebug(L"Autogeneration started: %s", dtCur.Format(L"%H:%M:%S"));
#endif
	dwStart = GetTickCount();
	dwTemp = dwStart;
#endif
	SendMessage(CSWM_PROGRESS_STEP, 0, PROG_RESULT_STEP);

	if (pDBUtil->ReduceDatabase(m_pRefFasSysData->GetPrjName()) == FALSE)
	{
		SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
#ifndef ENGLISH_MODE
		GF_AddLog(L"데이터베이스를 초기화하는데 실패했습니다.");
		AfxMessageBox(L"데이터베이스를 초기화하는데 실패했습니다.");
#else
		GF_AddLog(L"Failed to initialize the database.");
		AfxMessageBox(L"Failed to initialize the database.");
#endif
		return 0;
	}

	bCross = bFirst = FALSE;
	nlf = nlu = nlc = nlr = -1;
	nInType = nOutType = nEqName = nCont = -1;
	nLinkType = nLgID = nLgType = nTFid = nTUid = nTCid = nTRid = -1;
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_START);

#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - SP_GENERATE_LINK 시작: %d", dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_GENERATE_LINK started: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif

	bRet = pDBUtil->ExecuteSqlTimeOut(L"SP_GENERATE_LINK", g_stConfig.dwTimeOut);
	if (bRet == FALSE)
	{
		SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"자동생성 실패 : SP_GENERATE_LINK을 실행하는데 실패했습니다.\r\n%s"
			, pDBUtil->GetLastErrorString());
#else
		strError.Format(L"Autogeneration failed: failed to execute SP_GENERATE_LINK.\r\n%s"
			, pDBUtil->GetLastErrorString());
#endif
		GF_AddLog(strError);
		AfxMessageBox(strError);
		pDBUtil->ReduceDatabase(m_pRefFasSysData->GetPrjName());
		return 0;
	}
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - SP_GENERATE_LINK 종료: %d", dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_GENERATE_LINK ended: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - SP_GENERATE_PTN_BY_SOURCE 시작: %d", dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_GENERATE_PTN_BY_SOURCE started: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif
	nProgOffset = g_stConfig.dwTimeOut;
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_END);
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_START);
	bRet = pDBUtil->ExecuteSqlTimeOut(L"SP_GENERATE_PTN_BY_SOURCE", g_stConfig.dwTimeOut);
	if (bRet == FALSE)
	{
		SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"자동생성 실패 : SP_GENERATE_PTN_BY_SOURCE을 실행하는데 실패했습니다.\r\n%s"
#else
		strError.Format(L"Autogeneration failed: failed to execute SP_GENERATE_PTN_BY_SOURCE.\r\n%s"
#endif
			, pDBUtil->GetLastErrorString());
		GF_AddLog(strError);
		AfxMessageBox(strError);
		pDBUtil->ReduceDatabase(m_pRefFasSysData->GetPrjName());
		return 0;
	}
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - SP_GENERATE_PTN_BY_SOURCE 종료: %d", dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_GENERATE_PTN_BY_SOURCE ended: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif

#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - SP_DELETE_TEMPLINK_PTNITEM 시작: %d", dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_DELETE_TEMPLINK_PTNITEM started: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif

	nProgOffset += g_stConfig.dwTimeOut;
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_END);
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_START);
	bRet = pDBUtil->ExecuteSqlTimeOut(L"SP_DELETE_TEMPLINK_PTNITEM", g_stConfig.dwTimeOut);
	if (bRet == FALSE)
	{
		SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"자동생성 실패 : SP_DELETE_TEMPLINK_PTNITEM을 실행하는데 실패했습니다.\r\n%s"
			, pDBUtil->GetLastErrorString());
#else
		strError.Format(L"Autogeneration failed: failed to execute SP_DELETE_TEMPLINK_PTNITEM.\r\n%s"
			, pDBUtil->GetLastErrorString());
#endif
		GF_AddLog(strError);
		AfxMessageBox(strError);
		pDBUtil->ReduceDatabase(m_pRefFasSysData->GetPrjName());
		return 0;
	}

#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - SP_DELETE_TEMPLINK_PTNITEM 종료: %d", dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_DELETE_TEMPLINK_PTNITEM ended: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif

	nProgOffset += g_stConfig.dwTimeOut;
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_END);

	pDBUtil->ReduceDatabase(m_pRefFasSysData->GetPrjName());

#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - DataMake 시작: %d", dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - DataMake started: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif

	strSql = L"SELECT 2 AS LTYPE , SRC_FACP , SRC_UNIT , SRC_CHN, SRC_RLY , TGT_FACP,TGT_UNIT , TGT_CHN, TGT_RLY , LG_ID,INPUT_ID,OUTPUT_ID,OUTCONT_ID,EQNAME_ID ";
	strSql += L", SRC_BD_NAME, SRC_BTYPE_NAME, SRC_STAIR_NAME, SRC_FL_NAME, SRC_RM_NAME, TGT_BD_NAME, TGT_BTYPE_NAME, TGT_STAIR_NAME, TGT_FL_NAME, TGT_RM_NAME ";
	strSql += L"FROM TB_TEMP_SAVED_LINK A ";
	strSql += L"UNION ";
	strSql += L"SELECT 1 AS LTYPE , SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY, PT_ID AS TGT_FACP, 0 AS TGT_UNIT, 0 AS TGT_CHN, 0 AS TGT_RLY, 1000 AS LG_ID, 0 AS INPUT_ID, 0 AS OUTPUT_ID, 0 AS OUTCONT_ID, 0 AS EQNAME_ID ";
	strSql += L", '' AS SRC_BD_NAME, '' AS SRC_BTYPE_NAME, '' AS SRC_STAIR_NAME, '' AS SRC_FL_NAME, '' AS SRC_RM_NAME, '' AS TGT_BD_NAME, '' AS TGT_BTYPE_NAME, '' AS TGT_STAIR_NAME, '' AS TGT_FL_NAME, '' AS TGT_RM_NAME ";
	strSql += L"FROM TB_TEMP_USED_PTN B ";
	strSql += L"ORDER BY SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY, TGT_FACP, TGT_UNIT, TGT_CHN, TGT_RLY ";

	if (pDBUtil->OpenQuery(strSql) == false)
	{
		SendMessage(CSWM_PROGRESS_STEP, nIdx + nProgOffset, PROG_RESULT_ERROR);
	}
	else
	{
		nCnt = pDBUtil->GetRecordCount();
		for (i = 0; i < nCnt; i++)
		{
			if (m_bStopFlag)
				break;
			pDBUtil->GetFieldValue(L"SRC_FACP", nValue);
			nFID = nValue;
			pDBUtil->GetFieldValue(L"SRC_UNIT", nValue);
			nUID = nValue;
			pDBUtil->GetFieldValue(L"SRC_CHN", nValue);
			nCID = nValue;
			pDBUtil->GetFieldValue(L"SRC_RLY", nValue);
			nRID = nValue;
			pDBUtil->GetFieldValue(L"LTYPE", nValue);
			nLinkType = nValue;
			pDBUtil->GetFieldValue(L"TGT_FACP", nValue);
			nTFid = nValue;
			pDBUtil->GetFieldValue(L"TGT_UNIT", nValue);
			nTUid = nValue;
			pDBUtil->GetFieldValue(L"TGT_CHN", nValue);
			nTCid = nValue;
			pDBUtil->GetFieldValue(L"TGT_RLY", nValue);
			nTRid = nValue;
			pDBUtil->GetFieldValue(L"LG_ID", nValue);
			nLgID = nValue;
			pDBUtil->GetFieldValue(L"INPUT_ID", nValue);
			nInType = nValue;
			pDBUtil->GetFieldValue(L"OUTPUT_ID", nValue);
			nOutType = nValue;
			pDBUtil->GetFieldValue(L"OUTCONT_ID", nValue);
			nCont = nValue;
			pDBUtil->GetFieldValue(L"EQNAME_ID", nValue);
			nEqName = nValue;
			pDBUtil->GetFieldValue(L"SRC_BD_NAME", strSB);
			pDBUtil->GetFieldValue(L"SRC_BTYPE_NAME", strST);
			pDBUtil->GetFieldValue(L"SRC_STAIR_NAME", strSS);
			pDBUtil->GetFieldValue(L"SRC_FL_NAME", strSF);
			pDBUtil->GetFieldValue(L"SRC_RM_NAME", strSR);

			pDBUtil->GetFieldValue(L"TGT_BD_NAME", strTB);
			pDBUtil->GetFieldValue(L"TGT_BTYPE_NAME", strTT);
			pDBUtil->GetFieldValue(L"TGT_STAIR_NAME", strTS);
			pDBUtil->GetFieldValue(L"TGT_FL_NAME", strTF);
			pDBUtil->GetFieldValue(L"TGT_RM_NAME", strTR);

			if (nFID <= 0 || nUID <= 0 || nCID <= 0 || nRID <= 0)
			{
				pDBUtil->MoveNext();
				continue;;
			}

			if (nlf != nFID || nlu != nUID
				|| nlc != nCID || nlr != nRID)
			{
				if (nAddCnt > 0 && pNewAuto)
				{
					AddIndividaulEmergency(pNewAuto);
				}


				nAddCnt = 0;
				nlf = nFID;
				nlu = nUID;
				nlc = nCID;
				nlr = nRID;
				strKey = GF_GetIDSysDataKey(SE_RELAY, nFID, nUID, nCID, nRID);
				pNewAuto = m_MapIDAuto[strKey];

				if (pNewAuto == nullptr)
				{
#ifndef ENGLISH_MODE
					GF_AddDebug(L"Generation AutoLink Error : %s를 찾을 수 없습니다.", strKey);
#else
					GF_AddDebug(L"Generation AutoLink Error: %s was not found.", strKey);
#endif
					pDBUtil->MoveNext();
					continue;
				}
				pSrcSys = pNewAuto->GetSysData();
				if (pSrcSys == nullptr || pSrcSys->GetDataType() != SE_RELAY)
				{
#ifndef ENGLISH_MODE
					GF_AddDebug(L"Generation AutoLink Error : %s의 타입이 디바이스가 아닙니다.", strKey);
#else
					GF_AddDebug(L"Generation AutoLink Error: The type of %s is not a device.", strKey);
#endif
					pDBUtil->MoveNext();
					continue;
				}

				pSrcDev = (CDataDevice*)pSrcSys->GetSysData();

				if (nInType == INTYPE_CROSSA || nInType == INTYPE_CROSSB
					|| nInType == INTYPE_CROSS16_A || nInType == INTYPE_CROSS17_B
					|| nInType == INTYPE_CROSS18_A || nInType == INTYPE_CROSS19_B
					)
					bCross = TRUE;
				else
					bCross = FALSE;
#if _DBG_MAKE_TIME_
				dwStart = GetTickCount();
#endif
				SendMessage(CSWM_PROGRESS_STEP, nIdx + nProgOffset, PROG_RESULT_STEP);
				nIdx++;
				// 연동 출력 개수가 20개가 넘어가면 사용자가 알 수 있도록 Debug에 넣는다.
				if (nRelayLinkCnt > 20)
				{
					nExceptionCnt++;
#ifndef ENGLISH_MODE
					GF_AddLog(L"** 오류 예상됨 : %s[%s]의 출력 개수가 %d개 입니다.", pSrcDev->GetInputFullName(), pSrcDev->GetDevAddress(), nRelayLinkCnt);
#else
					GF_AddLog(L"** Error expected: The number of outputs for %s[%s] is %d.", pSrcDev->GetInputFullName(), pSrcDev->GetDevAddress(), nRelayLinkCnt);
#endif
				}

				nRelayLinkCnt = 0;
			}

			if (pSrcDev == nullptr)
			{
				pDBUtil->MoveNext();
				continue;
			}

			pLink = new CDataLinked;
			pLink->SetLinkData(nTFid, nTUid, nTCid, nTRid
				, nLinkType, LOGIC_ALL_AUTO, nLgID
				, nFID, nUID, nCID, nRID
			);

			bFirst = FALSE;
			if (bCross && nOutType == OUTTYPE_PREACTION && nCont == OUTCONT_VALVE)
			{
				// 교차 감지기 일때는 같은 실일때 동작한다.
				if (strSB == strTB && strST == strTT && strSS == strTS && strSF == strTF && strSR == strTR)
					bFirst = TRUE;
				else
					bFirst = FALSE;
			}
			pSrcDev->AddLink(bFirst, pLink);
			nRelayLinkCnt++; // 연동 출력 개수만 따로 카운트한다.
			nAddCnt++;
			pDBUtil->MoveNext();
		}

		if (nAddCnt > 0 && pNewAuto)
		{
			AddIndividaulEmergency(pNewAuto);
		}
	}

#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - DataMake 종료: %d", dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - DataMake ended: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif


#if _DBG_MAKE_TIME_
	dtCur = COleDateTime::GetCurrentTime();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"자동생성 종료 : %s", dtCur.Format(L"%H:%M:%S"));
#else
	GF_AddDebug(L"Autogeneration ended: %s", dtCur.Format(L"%H:%M:%S"));
#endif

#endif

	SendMessage(CSWM_PROGRESS_STEP, nIdx + nProgOffset, PROG_RESULT_FINISH);
	DisplayAutoMake();
	pDBUtil->ReduceDatabase(m_pRefFasSysData->GetPrjName());

	pDBUtil->DBClose();
	delete pDBUtil;
	pDBUtil = nullptr;

	if (nExceptionCnt > 0)
	{
		//GF_AddLog(L"** 오류 예상됨 : %s[%s]의 출력 개수가 %d개 입니다.",pSrcDev->GetInputFullName(),pSrcDev->GetDevAddress(),nRelayLinkCnt);
		CString strMsg;
#ifndef ENGLISH_MODE
		strMsg.Format(L"연동 출력이 20개가 넘는 회로가 %d개 있습니다.\n로그 창을 확인하시기 바랍니다.", nExceptionCnt);
#else
		strMsg.Format(L"The number of circuits with 20 interlock outputs is %d.\nPlease check the log window.", nExceptionCnt);
#endif
		AfxMessageBox(strMsg, MB_OK | MB_ICONERROR);
	}
	return 1;
}

int CFormAutoMake::AddIndividaulEmergency(CDataAutoMake * pSourceAuto)
{
	CDataAutoLogic * pLogic;
	CDataEmBc * pEm;
	std::shared_ptr<CManagerEmergency>	spEmManager;

	CDataDevice * pSrcDev;
	CDataEquip * pEq;
	//CDataLocBase * pLoc;
	CDataSystem * pSys;
	POSITION pos, epos = nullptr;
	int nSrcIntype, nSrcName;
	BOOL bLevelMatch = FALSE;
	nSrcIntype = nSrcName = -1;

	if (pSourceAuto == nullptr)
		return 0;
	spEmManager = m_pRefFasSysData->GetEmergencyManager();
	if (spEmManager == nullptr)
		return 0;
	pSys = pSourceAuto->GetSysData();
	if (pSys == nullptr || pSys->GetSysData() == nullptr)
		return 0;
	if (pSys->GetDataType() != SE_RELAY)
		return 0;
	pSrcDev = (CDataDevice *)pSys->GetSysData();
	pEq = pSrcDev->GetEqInput();
	if (pEq != nullptr)
	{
		nSrcIntype = (int)pEq->GetEquipID();
	}

	// 설비명이 없으면 자동생성하지 않는다.
	pEq = pSrcDev->GetEqName();
	if (pEq != nullptr)
		nSrcName = (int)pEq->GetEquipID();


	pos = m_spRefAutoLogic->GetHeadPosition();
	while (pos)
	{
		pLogic = (CDataAutoLogic*)m_spRefAutoLogic->GetNext(pos);
		if (pLogic == nullptr)
			continue;
		// 동작 장비 : 설비명 과 입력 타입을 비교
		// 연동 출력 데이터 : 전체 While 돌면서 로직과 출력이 일치하는거
		if (nSrcName >= 0 && pLogic->GetEqName() != nSrcName)
			continue;
		if (nSrcIntype >= 0 && pLogic->GetInType() != nSrcIntype)
			continue;

		if (pLogic->GetUseEmergency() == 0)
			continue;

		epos = spEmManager->GetHeadPosition();
		while (epos)
		{
			pEm = spEmManager->GetNext(epos);
			if (pEm == nullptr)
				continue;
			bLevelMatch = CheckMatchEmergencyLoc(pSrcDev, pEm
				, pLogic->GetUseMatchBuild()
				, pLogic->GetUseMatchStair(), pLogic->GetUseMatchFloor());
			if (bLevelMatch)
				pSourceAuto->AddEmergencyLink(pEm, pSrcDev, pLogic->GetLgId());
		}
		//pEm = AddEmergency(pSrcDev, pSrcDev->GetDevInputLocation(), pLogic);


	}
	return 1;
}

int CFormAutoMake::DisplayAutoMake()
{
	int nType;
	CString strKey;
	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
	CMapAutoSystem::iterator it;
	CDataSystem * pData;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDevice = nullptr;
	CDataAutoMake * pNewAuto;
	HTREEITEM hFacp[D_MAX_FACP_COUNT] = { nullptr };
	HTREEITEM hUnit[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT] = { nullptr };
	HTREEITEM hChn[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT][5] = { nullptr };
	HTREEITEM hItem;
	int nF, nU, nC, nD;
	nF = nU = nC = nD = -1;
	for (it = m_MapIDAuto.begin(); it != m_MapIDAuto.end(); it++)
	{
		if (m_bStopFlag == TRUE)
			return -1;
		pNewAuto = it->second;
		if (pNewAuto == nullptr)
			continue;
		strKey = it->first;
		if (strKey == L"")
			continue;
		pData = pNewAuto->GetSysData();
		if (pData == nullptr)
			continue;
		nType = pData->GetDataType();
		pFacp = nullptr;
		pUnit = nullptr;
		pChn = nullptr;
		pDevice = nullptr;
		nF = nU = nC = nD = -1;
		switch (nType)
		{
		case SE_FACP:
			pFacp = (CDataFacp*)pData->GetSysData();
			nF = pFacp->GetFacpID();
			break;
		case SE_UNIT:
			pUnit = (CDataUnit*)pData->GetSysData();
			nF = pUnit->GetFacpID();
			nU = pUnit->GetUnitID();
			break;
		case SE_CHANNEL:
			pChn = (CDataChannel*)pData->GetSysData();
			nF = pChn->GetFacpID();
			nU = pChn->GetUnitID();
			nC = pChn->GetChnID();
			break;
		case SE_RELAY:
			pDevice = (CDataDevice *)pData->GetSysData();
			nF = pDevice->GetFacpID();
			nU = pDevice->GetUnitID();
			nC = pDevice->GetChnID();
			nD = pDevice->GetDeviceID();
			break;
		}
		if (nF > 0 && hFacp[nF] == nullptr)
		{
			// Facp 입력
			if (pFacp == nullptr)
			{
				pFacp = m_pRefFasSysData->GetFacpByID(nF);
			}
			if (pFacp != nullptr)
			{
				hFacp[nF]
					= m_ctrlTree.InsertItem(pFacp->GetFacpName(), TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
				m_ctrlTree.SetItemData(hFacp[nF], (DWORD_PTR)pNewAuto);
			}

		}
		if (nF > 0 && nU > 0
			&& hUnit[nF][nU] == nullptr)
		{
			// Unit 입력
			if (pUnit == nullptr)
			{
				pUnit = m_pRefFasSysData->GetUnitByID(nF, nU);
			}

			if (pUnit != nullptr && hFacp[nF] != nullptr)
			{
				hUnit[nF][nU]
					= m_ctrlTree.InsertItem(pUnit->GetUnitName()
						, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hFacp[nF]);
				m_ctrlTree.SetItemData(hUnit[nF][nU], (DWORD_PTR)pNewAuto);
			}

		}
		if (nF > 0 && nU > 0 && nC > 0
			&& hChn[nF][nU][nC] == nullptr)
		{
			// Facp 입력
			if (pChn == nullptr)
			{
				pChn = m_pRefFasSysData->GetChannelByID(nF, nU, nC);
			}

			if (pChn != nullptr &&hUnit[nF][nU] != nullptr)
			{
				hChn[nF][nU][nC] = m_ctrlTree.InsertItem(pChn->GetChnName()
					, TIMG_DEV_CHANNEL, TIMG_DEV_CHANNEL
					, hUnit[nF][nU]);
				m_ctrlTree.SetItemData(hChn[nF][nU][nC], (DWORD_PTR)pNewAuto);
			}

		}

		if (nD > 0 && pDevice != nullptr && hChn[nF][nU][nC] != nullptr)
		{
			hItem = m_ctrlTree.InsertItem(pDevice->GetInputFullName()
				, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE
				, hChn[nF][nU][nC]);
			m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pNewAuto);
		}
	}
#ifndef ENGLISH_MODE
	AfxMessageBox(L"자동생성을 완료했습니다.");
#else
	AfxMessageBox(L"You have completed autogeneration.");
#endif
	return 1;
}

int CFormAutoMake::DisplayList(HTREEITEM hItem)
{
	CDataAutoMake * pNewAuto;
	CDataSystem * pData;
	CDataDevice * pDev, *pTemp;
	CDataPattern * pPtn;
	CDataLinked * pDlk;
	CDataLocBase * pLoc;
	CDataEquip * pIn, *pName, *pCont, *pOut;
	POSITION pos;
#ifndef ENGLISH_MODE
	CString strType = L"패턴", str;
#else
	CString strType = L"PATTERN", str;
#endif
	CString strIn, strOut, strCont, strName, strEqName, strAddr;
	CString strB, strBType, strStair, strFloor, strRoom;
	int nIdx = 0;
	CPtrList * pList;
	m_ctrlList.SetRedraw(FALSE);
	m_ctrlList.DeleteAllItems();

	pIn = pName = pCont = pOut = nullptr;
	pNewAuto = (CDataAutoMake*)m_ctrlTree.GetItemData(hItem);
	if (pNewAuto == nullptr)
	{
		m_ctrlList.SetRedraw();
		return 0;
	}
	pData = pNewAuto->GetSysData();
	if (pData == nullptr || pData->GetSysData() == nullptr || pData->GetDataType() != SE_RELAY)
	{
		m_ctrlList.SetRedraw();
		return 0;
	}
	pDev = (CDataDevice *)pData->GetSysData();
	pIn = pDev->GetEqInput();
	pName = pDev->GetEqName();


	strAddr = strIn = strOut = strCont = strName = strEqName = L"";
	if (pIn)
		strIn = pIn->GetEquipName();
	if (pName)
		strEqName = pName->GetEquipName();

	// 
	// 	pList = pNewAuto->GetRefPatternList();
	// 	if (pList == nullptr)
	// 	{
	// 		m_ctrlList.SetRedraw();
	// 		return 0;
	// 	}
	// 	pos = pList->GetHeadPosition();
	// 	while (pos)
	// 	{
	// 		pAp = (CDataAutoPattern *)pList->GetNext(pos);
	// 		if (pAp == nullptr)
	// 			continue; 
	// 
	// 		pPtn = pAp->GetPattern();
	// 		if (pPtn == nullptr)
	// 			continue; 
	// 
	// 		m_ctrlList.InsertItem(nIdx, strType);					//_T("종류"), LVCFMT_LEFT, 80);
	// 		m_ctrlList.SetItemText(nIdx, 1, pPtn->GetPatternName());//_T("출력/패턴이름"), LVCFMT_LEFT, 200);
	// 		m_ctrlList.SetItemText(nIdx, 2, strIn);//_T("입력타입"), LVCFMT_LEFT, 150);
	// 		m_ctrlList.SetItemText(nIdx, 3, strName);//_T("설비명"), LVCFMT_LEFT, 150);
	// 		m_ctrlList.SetItemText(nIdx, 4, L"");//_T("출력타입"), LVCFMT_LEFT, 150);
	// 		m_ctrlList.SetItemText(nIdx, 5, L"");//_T("출력설명"), LVCFMT_LEFT, 150);
	// 		str.Format(L"%d" , pPtn->GetPatternID());
	// 		m_ctrlList.SetItemText(nIdx, 6, str);//_T("출력주소"), LVCFMT_LEFT, 150);
	// 		nIdx++;
	// 	}

#ifndef ENGLISH_MODE
	strType = L"출력";
#else
	strType = L"OUTPUT";
#endif

	pList = pDev->GetLinkedList();
	if (pList == nullptr)
	{
		m_ctrlList.SetRedraw();
		return 0;
	}

	pos = pList->GetHeadPosition();
	while (pos)
	{
		pDlk = (CDataLinked *)pList->GetNext(pos);
		if (pDlk == nullptr)
			continue;

		strAddr = strOut = strCont = strName = L"";
		strB = strBType = strStair = strFloor = strRoom = L"";
		if (pDlk->GetLinkType() == LK_TYPE_RELEAY)
		{
			pTemp = m_pRefFasSysData->GetDeviceByID(pDlk->GetTgtFacp(), pDlk->GetTgtUnit(), pDlk->GetTgtChn(), pDlk->GetTgtDev());
			if (pTemp == nullptr)
				continue;
			pCont = pTemp->GetEqOutContents();
			pOut = pTemp->GetEqOutput();
			if (pCont)
				strCont = pCont->GetEquipName();
			if (pOut)
				strOut = pOut->GetEquipName();

			pLoc = pTemp->GetDevOutputLocation();
			if (pLoc != nullptr)
			{
				strB = pLoc->GetBuildName();
				strBType = pLoc->GetBTypeName();
				strStair = pLoc->GetStairName();
				strFloor = pLoc->GetFloorName();
				strRoom = pLoc->GetRoomName();
			}
#ifndef ENGLISH_MODE
			strType = L"출력";
#else
			strType = L"OUTPUT";
#endif
			strName = pTemp->GetOutputFullName();
			strAddr.Format(L"%s", pTemp->GetDevAddress());
		}
		else if (pDlk->GetLinkType() == LK_TYPE_PATTERN)
		{
			pPtn = m_pRefFasSysData->GetPattern(pDlk->GetTgtFacp());
			if (pPtn == nullptr)
				continue;
#ifndef ENGLISH_MODE
			strType = L"패턴";
#else
			strType = L"PATTERN";
#endif
			strName = pPtn->GetPatternName();
			strAddr.Format(L"%d", pPtn->GetPatternID());
		}

		m_ctrlList.InsertItem(nIdx, strType);					//_T("종류"), LVCFMT_LEFT, 80);
		m_ctrlList.SetItemText(nIdx, 1, strName);//_T("출력/패턴이름"), LVCFMT_LEFT, 200);
		m_ctrlList.SetItemText(nIdx, 2, strIn);//_T("입력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 3, strEqName);//_T("설비명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 4, strOut);//_T("출력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 5, strCont);//_T("출력설명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 6, strAddr);
		m_ctrlList.SetItemText(nIdx, 7, strB);			//_T("건물"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 8, strBType);	  //_T("건물종류"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 9, strStair);	  //_T("계단"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 10, strFloor);	  //_T("층"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 11, strRoom);	  //_T("실"), LVCFMT_LEFT, 150);
		nIdx++;
	}

	// [KHS 2020-5-27 16:22:16] 
	// 비상방송 추가
	CDataEmBc * pEm;
	pList = pNewAuto->GetRefEtcLinkList();
	if (pList == nullptr)
	{
		m_ctrlList.SetRedraw();
		return 0;
	}
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pDlk = (CDataLinked *)pList->GetNext(pos);
		if (pDlk == nullptr)
			continue;
		// 비상방송
		pEm = m_pRefFasSysData->GetEmergency(pDlk->GetTgtFacp());
		if (pEm == nullptr)
			continue;
		strB = strBType = strStair = strFloor = strRoom = L"";

#ifndef ENGLISH_MODE
		m_ctrlList.InsertItem(nIdx, L"비상방송");					//_T("종류"), LVCFMT_LEFT, 80);
#else
		m_ctrlList.InsertItem(nIdx, L"PUBLIC ADDRESS");					//_T("종류"), LVCFMT_LEFT, 80);
#endif
		m_ctrlList.SetItemText(nIdx, 1, pEm->GetEmName());//_T("출력/패턴이름"), LVCFMT_LEFT, 200);
		m_ctrlList.SetItemText(nIdx, 2, strIn);//_T("입력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 3, strName);//_T("설비명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 4, L"");//_T("출력타입"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 5, L"");//_T("출력설명"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 6, pEm->GetEmAddr());
		m_ctrlList.SetItemText(nIdx, 7, pEm->GetBuildName());			//_T("건물"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 8, L"");	  //_T("건물종류"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 9, pEm->GetStairName());	  //_T("계단"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 10, pEm->GetFloorName());	  //_T("층"), LVCFMT_LEFT, 150);
		m_ctrlList.SetItemText(nIdx, 11, L"");	  //_T("실"), LVCFMT_LEFT, 150);
		nIdx++;
	}
	m_ctrlList.SetRedraw();
	return 1;
}

int CFormAutoMake::MakeAutoSystemData()
{
	int nCnt = 0;
	CString strKey, strIDKey;
	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
	CMapSystemData::iterator it;
	CDataSystem * pData;
	CDataDevice * pDevice = nullptr;
	CDataAutoMake * pNewAuto;

	for (it = pRefMap->begin(); it != pRefMap->end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		strKey = it->first;
		if (strKey == L"")
			continue;
		if (pData->GetSysData() == nullptr)// || pData->GetDataType() != SE_RELAY)
			continue;

		strIDKey = pData->GetIDSysKey();
		pNewAuto = m_MapIDAuto[strIDKey];
		if (pNewAuto == nullptr)
		{
			pNewAuto = new CDataAutoMake;
			m_MapIDAuto[strIDKey] = pNewAuto;
		}

		// pNewAuto 초기화되지 않았으면 초기화
		if (pNewAuto->GetIDKey() == L"")
		{
			pNewAuto->InitAutoData(pData);
		}
		nCnt++;
	}

	return nCnt;
}

BOOL CFormAutoMake::CheckMatchEmergencyLoc(CDataDevice * pSrcDev, CDataEmBc * pEm
	, BYTE btBuild, BYTE btStair, BYTE btLevel)
{
	CDataLocBase * pSrcLoc;
	CString strS[3] = { L"" };
	CString strT[3] = { L"" };
	CString strtemp1, strtemp2;
	BYTE btFlag[3] = { 0 };
	int i = 0;
	pSrcLoc = pSrcDev->GetDevInputLocation();
	if (pSrcLoc == nullptr)
		return FALSE;

	strS[0] = pSrcLoc->GetBuildName();
	strS[1] = pSrcLoc->GetStairName();
	strS[2] = pSrcLoc->GetFloorName();


	strT[0] = pEm->GetBuildName();
	strT[1] = pEm->GetStairName();
	strT[2] = pEm->GetFloorName();

	btFlag[0] = btBuild;
	btFlag[1] = btStair;
	btFlag[2] = btLevel;

	for (i = 0; i < 3; i++)
	{
		strtemp1 = strS[i];
		strtemp2 = strT[i];
		strtemp1.Remove(' ');
		strtemp2.Remove(' ');
		if (btFlag[i])
		{
			if (strtemp1.CompareNoCase(strtemp2) != 0)
				return FALSE;
		}
	}
	return TRUE;
}

int CFormAutoMake::ProcessSaveAutoLink()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_BTN_MAKE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);

#ifndef ENGLISH_MODE
	if (SaveAutoLink() > 0)
		AfxMessageBox(L"생성된 연동데이터 저장이 완료 되었습니다.", MB_OK | MB_ICONINFORMATION);
#else
	if (SaveAutoLink() > 0)
		AfxMessageBox(L"The generated site logic data has been saved.", MB_OK | MB_ICONINFORMATION);
#endif

	GetDlgItem(IDC_BTN_MAKE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	return 1;
}

int CFormAutoMake::SaveAutoLink()
{
	m_nAllCnt = g_stConfig.dwTimeOut;
	m_nAllCnt += g_stConfig.dwTimeOut;
	m_nAllCnt += m_MapIDAuto.size();

	if (m_pRefFasSysData == nullptr)
	{
		SendMessage(CSWM_PROGRESS_STEP, 0, PROG_RESULT_ERROR);
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트가 닫혀있는 상태입니다. 프로젝트를 열고 다시 시도해 주십시오.");
		AfxMessageBox(L"프로젝트가 닫혀있는 상태입니다. 프로젝트를 열고 다시 시도해주십시오.");
#else
		GF_AddLog(L"The project is closed. Please open the project and try again.");
		AfxMessageBox(L"The project is closed. Please open the project and try again.");
#endif
		return 0;
	}
	int nIdx = 0, nRet = 0;
	int n1, n2;
	int nProgOffset = 0;
	BOOL bCross = FALSE;
	CString strKey, strSql;
	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
	CMapAutoSystem::iterator it;
	CDataSystem * pData;
	CDataDevice * pDevice = nullptr, *pOriginalDev;
	CDataAutoMake * pNewAuto;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();

	if (pDb->DropCleanBuffer() == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"데이터베이스의 메모리 버퍼를 초기화하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to initialize the memory buffer of the database.");
#endif
		return 0;
	}
	pDb->BeginTransaction();

	SendMessage(CSWM_PROGRESS_STEP, 0, PROG_RESULT_TIMER_END);
	SendMessage(CSWM_PROGRESS_STEP, 0, PROG_RESULT_TIMER_START);
	//  [9/20/2022 KHS] Index 제거
	if (m_pRefFasSysData->TempFunc_DropIndex() <= 0)
	{

		SendMessage(CSWM_PROGRESS_STEP, 0, PROG_RESULT_TIMER_END);
		pDb->RollbackTransaction();
		//m_pNewRelayTable->SendProgStep(this,PROG_RESULT_ERROR,0,0);
		return 0;
	}
	SendMessage(CSWM_PROGRESS_STEP, 0, PROG_RESULT_TIMER_END);
	nProgOffset = g_stConfig.dwTimeOut;
	strSql.Format(L"DELETE FROM TB_LINK_RELAY WHERE LG_TYPE=%d", LOGIC_ALL_AUTO);
	if (pDb->ExecuteSql(strSql) == FALSE)
	{
		SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
#ifndef ENGLISH_MODE
		AfxMessageBox(L"전체 연동출력을 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the entire interlock output.");
#endif
		pDb->RollbackTransaction();
		return 0;
	}
	for (it = m_MapIDAuto.begin(); it != m_MapIDAuto.end(); it++)
	{
		if (m_bStopFlag == TRUE)
			return -1;
		pNewAuto = it->second;
		if (pNewAuto == nullptr)
			continue;
		strKey = it->first;
		if (strKey == L"")
			continue;
		pData = pNewAuto->GetSysData();
		if (pData == nullptr || pData->GetDataType() != SE_RELAY)
			continue;
		pOriginalDev = m_pRefFasSysData->GetDeviceByIDKey(strKey);
		if (pOriginalDev == nullptr)
		{
			SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}
		// [KHS 2020-8-19 16:55:01] 
		// 	pOriginalDev->RemoveAllLink(); --> pOriginalDev->RemoveAllLink(LOGIC_ALL_AUTO);
		pOriginalDev->RemoveLinkByType(LOGIC_ALL_AUTO);
		bCross = FALSE;
		n1 = n2 = 0;
		if (pOriginalDev->GetEqInput())
			n1 = pOriginalDev->GetEqInput()->GetEquipID();
		// 		if (pOriginalDev->GetEqName())
		// 			n2 = pOriginalDev->GetEqName()->GetEquipID();
		if (n1 == INTYPE_CROSSA || n1 == INTYPE_CROSSB
			|| n1 == INTYPE_CROSS16_A || n1 == INTYPE_CROSS17_B
			|| n1 == INTYPE_CROSS18_A || n1 == INTYPE_CROSS19_B
			)
			bCross = TRUE;
		nRet = SaveIndividualAutoLink(pDb, pNewAuto, pOriginalDev, bCross);
		if (nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}

		nRet = SaveIndividualEmergency(pDb, pNewAuto, pOriginalDev);
		if (nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}
		nRet = SaveIndividualPattern(pDb, pNewAuto, pOriginalDev);
		if (nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}

		nIdx++;
		nProgOffset += 1;
		SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_STEP);
		//SendMessage(CSWM_PROGRESS_STEP, nIdx, PROG_RESULT_STEP);
	}
	//SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_TIMER_END);
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_START);
	if (m_pRefFasSysData->TempFunc_CheckIndex() <= 0)
	{
		SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_ERROR);
		pDb->RollbackTransaction();
		//m_pRefFasSysData->SendProgStep(this,PROG_RESULT_ERROR,0,0);
		return 0;
	}
	nProgOffset += g_stConfig.dwTimeOut;
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_FINISH);
	pDb->CommitTransaction();
	//RemoveAllData();
	return 1;
}

int CFormAutoMake::SaveIndividualAutoLink(YAdoDatabase * pDb, CDataAutoMake * pSourceAuto, CDataDevice *pOriginalDev, BOOL bCross)
{
	CString strSql = L"", strtemp = L"";
	POSITION pos;
	CPtrList * pList;
	int n1, n2, nCnt = 0;
	BOOL bFirst = FALSE;
	CDataLinked * pLnk, *pNewLnk = nullptr;
	CDataDevice *pNewDev, *pTempDev;
	CDataSystem * pNewSys;
	pNewSys = pSourceAuto->GetSysData();
	if (pNewSys == nullptr || pNewSys->GetSysData() == nullptr)
		return 0;
	pNewDev = (CDataDevice *)pNewSys->GetSysData();

	pList = pNewDev->GetLinkedList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pLnk = (CDataLinked *)pList->GetNext(pos);
		if (pLnk == nullptr)
			continue;
		if (pOriginalDev->FindLink(pLnk) == nullptr)
		{
			pNewLnk = new CDataLinked;
			pNewLnk->CopyData(pLnk);
			bFirst = FALSE;
			if (bCross)
			{
				pTempDev = m_pRefFasSysData->GetDeviceByID(pLnk->GetTgtFacp(), pLnk->GetTgtUnit(), pLnk->GetTgtChn(), pLnk->GetTgtDev());
				if (pTempDev != NULL)
				{
					n1 = n2 = 0;
					if (pTempDev->GetEqOutContents())
						n1 = pTempDev->GetEqOutContents()->GetEquipID();

					if (pTempDev->GetEqOutput())
						n2 = pTempDev->GetEqOutput()->GetEquipID();

					//[4/28/2021 KHS] 오류 수정 : 교차 감지기 일때 한번 bFirstAdd가 TRUE 이면 계속 TRUE					
					// 					if(n1 == OUTTYPE_PREACTION && n2 == OUTCONT_VALVE
					// 						&& (pOriginalDev->GetFacpID() == pTempDev->GetFacpID()
					// 							&& pOriginalDev->GetUnitID() == pTempDev->GetFacpID()
					// 							&& pOriginalDev->GetChnID() == pTempDev->GetChnID())
					// 						) // 같은계통
					// 					{
					// 						bFirst = TRUE;
					// 					}
					//[4/28/2021 KHS] 오류 수정 : 교차 감지기 일때 한번 bFirstAdd가 TRUE 이면 계속 TRUE
					//if(n1 == OUTTYPE_PREACTION && n2 == OUTCONT_VALVE && IsSameRoom(pOriginalDev,pTempDev))
					if (n1 == OUTCONT_VALVE && n2 == OUTTYPE_PREACTION  && IsSameRoom(pOriginalDev, pTempDev))
						bFirst = TRUE;
					else
						bFirst = FALSE;
					/////////////////////////////////////
				}
			}
			pOriginalDev->AddLink(bFirst, pNewLnk);
			strtemp.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
				L", LINK_TYPE,LG_TYPE , LG_ID "
				L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
				L", ADD_USER) "
				L" VALUES(%d,%d,%d,%d"
				L", %d,%d,%d"
				L", %d,%d,%d,%d"
				L", '%s') ;\n"
				, pLnk->GetSrcFacp(), pLnk->GetSrcUnit(), pLnk->GetSrcChn(), pLnk->GetSrcDev()
				, pLnk->GetLinkType(), pLnk->GetLogicType(), pLnk->GetLogicID()
				, pLnk->GetTgtFacp(), pLnk->GetTgtUnit(), pLnk->GetTgtChn(), pLnk->GetTgtDev()
				, m_pRefFasSysData->GetCurrentUser()
			);

			strSql += strtemp;
			nCnt++;
			if (strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				pDb->ExecuteSql(strSql);
				nCnt = 0;
			}
		}
	}

	if (nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	return 1;
}

BOOL CFormAutoMake::IsSameRoom(CDataDevice * pSrcDev, CDataDevice * pTargetDev)
{
	CDataLocBase * pSrcLoc, *pTgtLoc;
	CString strS[5] = { L"" };
	CString strT[5] = { L"" };
	CString strtemp1, strtemp2;
	BYTE btFlag[5] = { 0 };
	int i = 0;
	pSrcLoc = pSrcDev->GetDevInputLocation();
	if (pSrcLoc == nullptr)
		return FALSE;

	strS[0] = pSrcLoc->GetBuildName();
	strS[1] = pSrcLoc->GetBTypeName();
	strS[2] = pSrcLoc->GetStairName();
	strS[3] = pSrcLoc->GetFloorName();
	strS[4] = pSrcLoc->GetRoomName();

	pTgtLoc = pTargetDev->GetDevInputLocation();
	if (pTgtLoc == nullptr)
		return FALSE;

	strT[0] = pTgtLoc->GetBuildName();
	strT[1] = pTgtLoc->GetBTypeName();
	strT[2] = pTgtLoc->GetStairName();
	strT[3] = pTgtLoc->GetFloorName();
	strT[4] = pTgtLoc->GetRoomName();

	for (i = 0; i < 5; i++)
	{
		strtemp1 = strS[i];
		strtemp2 = strT[i];
		strtemp1.Remove(' ');
		strtemp2.Remove(' ');
		if (btFlag[i])
		{
			if (strtemp1.CompareNoCase(strtemp2) != 0)
				return FALSE;
		}
		// 		if (strS[i] != strT[i])
		// 			return FALSE;
	}
	return TRUE;
}

int CFormAutoMake::SaveIndividualEmergency(YAdoDatabase * pDb, CDataAutoMake * pSourceAuto, CDataDevice *pOriginalDev)
{
	CString strSql = L"", strtemp = L"";
	POSITION pos;
	CPtrList * pList;
	CDataLinked * pLnk, *pNewLnk = nullptr;
	CDataSystem * pNewSys;
	int nCnt = 0;
	if (pDb == nullptr || pSourceAuto == nullptr)
		return 0;
	pNewSys = pSourceAuto->GetSysData();
	if (pNewSys == nullptr || pNewSys->GetSysData() == nullptr)
		return 0;


	pList = pSourceAuto->GetRefEtcLinkList();
	//pList = pNewDev->GetLinkedList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pLnk = (CDataLinked *)pList->GetNext(pos);
		if (pLnk == nullptr)
			continue;
		if (pOriginalDev->FindLink(pLnk) == nullptr)
		{
			pNewLnk = new CDataLinked;
			pNewLnk->CopyData(pLnk);
			pOriginalDev->AddLink(FALSE, pNewLnk);
			strtemp.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
				L", LINK_TYPE,LG_TYPE , LG_ID "
				L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
				L", ADD_USER) "
				L" VALUES(%d,%d,%d,%d"
				L", %d,%d,%d"
				L", %d,%d,%d,%d"
				L", '%s') ;\n"
				, pLnk->GetSrcFacp(), pLnk->GetSrcUnit(), pLnk->GetSrcChn(), pLnk->GetSrcDev()
				, pLnk->GetLinkType(), pLnk->GetLogicType(), pLnk->GetLogicID()
				, pLnk->GetTgtFacp(), pLnk->GetTgtUnit(), pLnk->GetTgtChn(), pLnk->GetTgtDev()
				, m_pRefFasSysData->GetCurrentUser()
			);

			strSql += strtemp;
			nCnt++;
			if (strSql.GetLength() > 65535)
			{
				pDb->ExecuteSql(strSql);
				nCnt = 0;
			}
		}
	}

	if (nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	return 1;
}

int CFormAutoMake::SaveIndividualPattern(YAdoDatabase * pDb, CDataAutoMake * pSourceAuto, CDataDevice *pOriginalDev)
{
	CString strSql = L"", strtemp = L"";
	POSITION pos;
	CPtrList * pList;
	CDataAutoPattern * pAp;
	CDataSystem * pNewSys;
	CDataLinked *pNewLnk;
	int nCnt = 0;
	int nID = 0;
	pNewSys = pSourceAuto->GetSysData();
	if (pNewSys == nullptr || pNewSys->GetSysData() == nullptr)
		return 0;
	//pNewDev = (CDataDevice *)pNewSys->GetSysData();

	pList = pSourceAuto->GetRefPatternList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pAp = (CDataAutoPattern *)pList->GetNext(pos);
		if (pAp == nullptr)
			continue;
		nID = pAp->GetPtnID();
		if (pOriginalDev->FindLinkByID(LK_TYPE_PATTERN, nID, 0, 0, 0) == nullptr)
		{
			pNewLnk = new CDataLinked;
			pNewLnk->SetLinkData(pAp->GetPattern(), LOGIC_ALL_AUTO, D_NUM_AUTO_PTN_LOGIC_ID
				, pOriginalDev);

			pOriginalDev->AddLink(FALSE, pNewLnk);
			strtemp.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
				L", LINK_TYPE,LG_TYPE , LG_ID "
				L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
				L", ADD_USER) "
				L" VALUES(%d,%d,%d,%d"
				L", %d,%d,%d"
				L", %d,%d,%d,%d"
				L", '%s') ;\n"
				, pNewLnk->GetSrcFacp(), pNewLnk->GetSrcUnit(), pNewLnk->GetSrcChn(), pNewLnk->GetSrcDev()
				, pNewLnk->GetLinkType(), pNewLnk->GetLogicType(), pNewLnk->GetLogicID()
				, pNewLnk->GetTgtFacp(), pNewLnk->GetTgtUnit(), pNewLnk->GetTgtChn(), pNewLnk->GetTgtDev()
				, m_pRefFasSysData->GetCurrentUser()
			);
			strSql += strtemp;
			nCnt++;
			if (strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				pDb->ExecuteSql(strSql);
				nCnt = 0;
			}
			//pDb->ExecuteSql(strSql);
		}
	}

	if (nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	return 1;
}