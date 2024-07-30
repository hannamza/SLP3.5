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

#include "MainFrm.h"
// CFormAutoMake

IMPLEMENT_DYNCREATE(CFormAutoMake, CFormView)

#ifndef ENGLISH_MODE
CFormAutoMake::CFormAutoMake()
	: CFormView(IDD_FORMAUTOMAKE)
{
	m_pRefFasSysData = nullptr;
	m_spRefAutoLogic = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
}
#else
CFormAutoMake::CFormAutoMake()
	: CFormView(IDD_FORMAUTOMAKE_EN)
{
	m_pRefFasSysData = nullptr;
	m_spRefAutoLogic = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
}
#endif

CFormAutoMake::~CFormAutoMake()
{
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
		//nRet = me->GenerateAutoLinkData();
		nRet = me->GenerateAutoLinkData2();
		//nRet = me->GenerateAutoLinkData3();
	}
	catch (...)
	{
	}
// 	if(nRet == 1)
// 		me->SendMessage(CSWM_PROGRESS_STEP, 100, PROG_RESULT_FINISH);
// 	else
// 		me->SendMessage(CSWM_PROGRESS_STEP, 100, PROG_RESULT_ERROR);
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
		//nRet = me->GenerateAutoLinkData();
		nRet = me->ProcessSaveAutoLink();
		//nRet = me->GenerateAutoLinkData3();
	}
	catch(...)
	{
	}
	// 	if(nRet == 1)
	// 		me->SendMessage(CSWM_PROGRESS_STEP, 100, PROG_RESULT_FINISH);
	// 	else
	// 		me->SendMessage(CSWM_PROGRESS_STEP, 100, PROG_RESULT_ERROR);
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

	// 	rcLeft = CRect(4, 65, cx / 4 - 4 - 2, cy - 4);
	// 	rcRight = CRect(rcLeft.right + 4, 65, cx - 4, cy - 4);
	// 
	// 	if (m_ctrlTree.GetSafeHwnd())
	// 	{
	// 		m_ctrlTree.MoveWindow(rcLeft);
	// 	}
	// 
	// 	if (m_ctrlList.GetSafeHwnd())
	// 	{
	// 		m_ctrlList.MoveWindow(rcRight);
	// 	}

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
	if(m_pRefFasSysData == nullptr)
		m_pRefFasSysData = theApp.GetRelayTableData();

	if(m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"연동데이터 자동생성하는데 실패했습니다.(중계기일람표 가져오기 실패)");
#else
		AfxMessageBox(L"Failed to autogenerate the site logic data. (Failed to retrieve the module table)");
#endif
		return 0;
	}

	m_spRefAutoLogic = m_pRefFasSysData->GetAutoLogicManager();

	if(m_spRefAutoLogic == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"연동데이터 자동생성하는데 실패했습니다.(자동생성 로직 가져오기 실패)");
#else
		AfxMessageBox(L"Failed to autogenerate the site logic data. (Failed to retrieve the autogeneration logic)");
#endif
		return 0;
	}
	m_bStopFlag = FALSE;
	if(InitAutoSystemData() < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"연동데이터 자동생성하는데 실패했습니다.(데이터 초기화 실패)");
#else
		AfxMessageBox(L"Failed to autogenerate the site logic data. (Failed to initialize data)");
#endif
		return 0;
	}

	// Thread Start
	m_pMakeThread = AfxBeginThread((AFX_THREADPROC)Thread_MakeProc,
		(LPVOID)this);
	return 1; 
}

void CFormAutoMake::OnBnClickedBtnMake()
{
	m_ctrlList.DeleteAllItems();
	m_ctrlTree.DeleteAllItems();
	RemoveAllData();

#ifndef ENGLISH_MODE
	if(AfxMessageBox(L"연동데이터 자동생성 시작전 오류검사를 진행하실려면\nYes : 오류검사 후 연동데이터 생성\nNo:오류검사없이 연동데이터 생성",MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		AutoMakeStart();
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
		AutoMakeStart();
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
	
// 	if (GenerateAutoLinkData() < 0)
// 	{
// 		AfxMessageBox(L"연동데이터 자동생성하는데 실패 했습니다.(생성로직 실패)");
// 		return;
// 	}

	// 1. Tree 입력 



}

/*
1. InitAutoSystem Data : 기존 데이터 삭제
	1.1. MakeAutoSystemData : 새롭게 Map 구성 --> 자동으로 만들어진 연동정보가 들어갈 Map
	1.2. MakeSortedDeviceByLocation : 위치로 정렬된 DeviceList 구성
2. GenerateAutoLinkData : 자동으로 구성하는 연동정보 생상
	4.1. While Loop : MakeAutoSystemData에서 만들어진 Map
	4.2. 로직별 AddIndividualAutoLink() 
	4.3. AddIndividualAutoLink
	4.4. Pattern 재구성 및 Relay 삭제
	
5. Pattern 재구성	
*/
int CFormAutoMake::InitAutoSystemData()
{
	int nAll = 0 , nRet = 0;
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

BOOL CFormAutoMake::CheckMatchLoc(CDataDevice * pSrcDev, CDataDevice * pTargetDev
	, CDataAutoLogic * pLogic)
{
	CDataLocBase * pSrcLoc , *pTgtLoc;
	CString strS[5] = { L"" };
	CString strT[5] = { L"" };
	BYTE btFlag[5] = { 0 };
	CString strtemp1, strtemp2;
	CString strTemp;
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
	if (pLogic->GetUseUnderBasic())
	{
		if (pSrcLoc->GetFloorNumber() < 0)
			return FALSE;

		if (pTgtLoc->GetFloorNumber() < 0)
			return FALSE;

	}

	strT[0] = pTgtLoc->GetBuildName();
	strT[1] = pTgtLoc->GetBTypeName();
	strT[2] = pTgtLoc->GetStairName();
	strT[3] = pTgtLoc->GetFloorName();
	strT[4] = pTgtLoc->GetRoomName();


	btFlag[0] = pLogic->GetUseMatchBuild();
	btFlag[1] = pLogic->GetUseMatchBType();
	btFlag[2] = pLogic->GetUseMatchStair();
	btFlag[3] = pLogic->GetUseMatchFloor();
	btFlag[4] = pLogic->GetUseMatchRoom();
	
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
	}
	return TRUE;
}

BOOL CFormAutoMake::CheckMatchLoc(CDataDevice * pSrcDev, CDataDevice * pTargetDev
	, BYTE btBuild, BYTE btBType, BYTE btStair, BYTE btLevel, BYTE btRoom )
{
	CDataLocBase * pSrcLoc, *pTgtLoc;
	CString strS[5] = { L"" };
	CString strT[5] = { L"" };
	BYTE btFlag[5] = { 0 };
	CString strtemp1, strtemp2;
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

	btFlag[0] = btBuild;
	btFlag[1] = btBType;
	btFlag[2] = btStair;
	btFlag[3] = btLevel;
	btFlag[4] = btRoom;

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
	}
	return TRUE;
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

BOOL CFormAutoMake::CheckEtcLoc(CDataDevice * pSrcDev, CDataDevice * pTargetDev
	, CDataAutoLogic * pLogic)
{
	CDataLocBase * pSrcLoc, *pTgtLoc;
	CString strS[5] = { L"" };
	CString strT[5] = { L"" };
	BYTE btFlag[5] = { 0 };
	CString strtemp1, strtemp2;
	int i = 0, nCnt = 2;
	int nSrcFlNum, nTgtFlNum;
	BOOL bRet = TRUE;
	nSrcFlNum = nTgtFlNum = N_FLOOR_NONE_NAME;
	pSrcLoc = pSrcDev->GetDevInputLocation();
	if (pSrcLoc == nullptr)
		return FALSE;

	pTgtLoc = pTargetDev->GetDevInputLocation();
	if (pTgtLoc == nullptr)
		return FALSE;
	
	nSrcFlNum = pSrcLoc->GetFloorNumber();
	nTgtFlNum = pTgtLoc->GetFloorNumber();

	strS[0] = pSrcLoc->GetBuildName();
	strT[0] = pTgtLoc->GetBuildName();

	
	strS[1] = pSrcLoc->GetBTypeName();
	strS[2] = pSrcLoc->GetStairName();

	strT[1] = pTgtLoc->GetBTypeName();
	strT[2] = pTgtLoc->GetStairName();

	btFlag[0] = pLogic->GetUseUnderClassBuild();
	btFlag[1] = pLogic->GetUseUnderClassBType();
	btFlag[2] = pLogic->GetUseUnderClassStair();

	btFlag[3] = pLogic->GetUseUnder1F();
	btFlag[4] = pLogic->GetUseUnderB1F();
	nCnt = 3;

	if (pLogic->GetUseUnderParking() == 1)
	{
#ifndef ENGLISH_MODE
		if (pSrcLoc->GetBTypeName() == L"주차장"
			|| pSrcLoc->GetBuildName() == L"주차장"
			|| pTgtLoc->GetBTypeName() == L"주차장"
			|| pTgtLoc->GetBuildName() == L"주차장")
		{
			strT[0] = strS[0];
			strT[1] = strS[1];
			strT[2] = strS[2];
		}
#else
		if (pSrcLoc->GetBTypeName() == L"PARKING LOT"
			|| pSrcLoc->GetBuildName() == L"PARKING LOT"
			|| pTgtLoc->GetBTypeName() == L"PARKING LOT"
			|| pTgtLoc->GetBuildName() == L"PARKING LOT")
		{
			strT[0] = strS[0];
			strT[1] = strS[1];
			strT[2] = strS[2];
		}
#endif
	}
	
	
	for (i = 0; i < nCnt; i++)
	{
		strtemp1 = strS[i];
		strtemp2 = strT[i];
		strtemp1.Remove(' ');
		strtemp2.Remove(' ');
		if (btFlag[i])
		{
			if (strtemp1.CompareNoCase(strtemp2) != 0)
			{
				bRet = FALSE;
				return FALSE;
			}
		}
	}
	if (nSrcFlNum < 0)
	{
		if (nTgtFlNum < 0)
		{

		}
		else if (nTgtFlNum == 1)
		{
			if (btFlag[4])
			{
				if (nSrcFlNum != -1) // 지하 1층화재 시
					return FALSE;
			}
			else // 2층 이상
				return FALSE;
		}
		else // TARGET 지상츠
			return FALSE;
	}
	else if (nSrcFlNum == 1)
	{
		if (btFlag[3])
		{
			if (nTgtFlNum < 0)
			{
				return TRUE;
			}
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;
	return bRet;
}

BOOL CFormAutoMake::CheckEtcLoc2(CDataDevice * pSrcDev, CDataDevice * pTargetDev
	, CDataAutoLogic * pLogic)
{
	CDataLocBase * pSrcLoc, *pTgtLoc;
	CString strS[5] = { L"" };
	CString strT[5] = { L"" };
	BYTE btFlag[5] = { 0 };
	CString strtemp1, strtemp2;
	int i = 0, nCnt = 2;
	int nSrcFlNum, nTgtFlNum;
	BOOL bRet = TRUE;
	nSrcFlNum = nTgtFlNum = N_FLOOR_NONE_NAME;
	pSrcLoc = pSrcDev->GetDevInputLocation();
	if (pSrcLoc == nullptr)
		return FALSE;

	pTgtLoc = pTargetDev->GetDevInputLocation();
	if (pTgtLoc == nullptr)
		return FALSE;

	// 	if (pTgtLoc->GetBTypeName() == L"주차장")
	// 		return FALSE;
	nSrcFlNum = pSrcLoc->GetFloorNumber();
	nTgtFlNum = pTgtLoc->GetFloorNumber();

	strS[0] = pSrcLoc->GetBuildName();
	strT[0] = pTgtLoc->GetBuildName();

	strS[1] = pSrcLoc->GetBTypeName();
	strS[2] = pSrcLoc->GetStairName();

	strT[1] = pTgtLoc->GetBTypeName();
	strT[2] = pTgtLoc->GetStairName();

	btFlag[0] = pLogic->GetUseUnderClassBuild();
	btFlag[1] = pLogic->GetUseUnderClassBType();
	btFlag[2] = pLogic->GetUseUnderClassStair();

	btFlag[3] = pLogic->GetUseUnder1F();
	btFlag[4] = pLogic->GetUseUnderB1F();

	if (pLogic->GetUseUnderParking() == 1)
	{
#ifndef ENGLISH_MODE
		if (pSrcLoc->GetBTypeName() == L"주차장"
			|| pSrcLoc->GetBuildName() == L"주차장"
			|| pTgtLoc->GetBTypeName() == L"주차장"
			|| pTgtLoc->GetBuildName() == L"주차장")
		{
			strT[0] = strS[0];
			strT[1] = strS[1];
			strT[2] = strS[2];
		}
#else
		if (pSrcLoc->GetBTypeName() == L"PARKING LOT"
			|| pSrcLoc->GetBuildName() == L"PARKING LOT"
			|| pTgtLoc->GetBTypeName() == L"PARKING LOT"
			|| pTgtLoc->GetBuildName() == L"PARKING LOT")
		{
			strT[0] = strS[0];
			strT[1] = strS[1];
			strT[2] = strS[2];
		}
#endif
	}

	nCnt = 3;
	
	for (i = 0; i < nCnt; i++)
	{
		strtemp1 = strS[i];
		strtemp2 = strT[i];
		strtemp1.Remove(' ');
		strtemp2.Remove(' ');
		if (btFlag[i])
		{
			if (strtemp1.CompareNoCase(strtemp2) != 0)
			{
				bRet = FALSE;
				return FALSE;
			}
		}
	}
	if (nSrcFlNum < 0)
	{
		if (nTgtFlNum < 0)
		{

		}
		else if (nTgtFlNum == 1)
		{
			if (btFlag[4])
			{
				if (nSrcFlNum != -1) // 지하 1층화재 시
					return FALSE;
			}
			else // 2층 이상
				return FALSE;
		}
		else // TARGET 지상츠
			return FALSE;
	}
	else if (nSrcFlNum == 1)
	{
		if (btFlag[3])
		{
			if (nTgtFlNum < 0)
			{
				return TRUE;
			}
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;
	return bRet;
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
/*
  Source : 동작되는 장비
	1. 자동생성 로직에서 입력/설비명과 같은 로직 찾기
	2. 전체 Device 중에 출력타입/출력설명이 같은 
*/
int CFormAutoMake::AddIndividualAutoLink(CDataAutoMake * pSourceAuto)
{

 	CDataAutoLogic * pLogic;
 	POSITION posDev , posLogic;
 	CDataDevice * pSrcDev, *pTargetDev;
 	CDataEquip * pEq;
	CDataLocBase * pLoc;
 	CDataSystem * pSys;
	CString strCont, strSrcStair, strTargetStair, strSrcBuild, strSrcBtype, strSrcFloor, strS_Room, strT_Room;
	CString strT_Build, strT_BType, strT_Floor;
 	int nSrcIntype, nSrcOuttype, nSrcName, nSrcCont, nSrcFlNum;
 	int nTargetIntype, nTargetOuttype, nTargetName, nTargetCont, nTargetFlNum;
 	//int nLi, nLo, nLn, nLc; // 마지막 값
 	int nLogicOut = 0 , nAddCnt = 0 ;
 	BOOL bAdded = FALSE;
	BOOL bFirstAdd = FALSE;
	BOOL bLocMatch = FALSE, bLevelMatch = FALSE , bParkingMatch = FALSE , bUnderMatch = FALSE;
	BOOL bCrossDetector = FALSE;
 	CString strDebug = L"";
 	nSrcIntype = nSrcOuttype = nSrcName = nSrcCont = 0;
 	nSrcFlNum = N_FLOOR_NONE_NAME; // 층의 기본값 1000
	strSrcBuild = strSrcBtype = strSrcFloor = strSrcStair = strS_Room = strT_Room = L""; // 층의 기본값 1000
	strT_Build = strT_BType = strT_Floor =  L""; // 층의 기본값 1000
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
		// 교차 감지기 인지 확인
		if (nSrcIntype == INTYPE_CROSSA || nSrcIntype == INTYPE_CROSSB
			|| nSrcIntype == INTYPE_CROSS16_A || nSrcIntype == INTYPE_CROSS17_B
			|| nSrcIntype == INTYPE_CROSS18_A || nSrcIntype == INTYPE_CROSS19_B
			)
			bCrossDetector = TRUE;
 	}
	
	
 	// 설비명이 없으면 자동생성하지 않는다.
 	if (pSrcDev->GetEqName() == nullptr)
 		return 0;
 	pEq = pSrcDev->GetEqName();
 	if (pEq != nullptr)
 		nSrcName = (int)pEq->GetEquipID();
 
 	pEq = pSrcDev->GetEqOutContents();
 	if (pEq != nullptr)
 		nSrcCont = (int)pEq->GetEquipID();
 
 	pEq = pSrcDev->GetEqOutput();
 	if (pEq != nullptr)
 		nSrcOuttype = (int)pEq->GetEquipID();
 
 	pLoc = pSrcDev->GetDevInputLocation();
 	if (pLoc != nullptr)
 	{
 		nSrcFlNum = pLoc->GetFloorNumber();
 		strSrcStair = pLoc->GetStairName();
 		strSrcBuild = pLoc->GetBuildName();
 		strSrcBtype = pLoc->GetBTypeName();
 		strSrcFloor = pLoc->GetFloorName();
 		strS_Room = pLoc->GetRoomName();
 	}
 	// 	if (nSrcFlNum < 0) // 지하층은 -1 부터 시작
 	// 	{
 	// 		AddUnderFloorAutoLink();
 	// 	}
 
 	// [KHS 2020-2-25 13:53:24] 
 	// 로직과 일치 여부 
 	posDev = m_ptrSortedDevice.GetHeadPosition();
 	while (posDev)
 	{
 		if (m_bStopFlag == TRUE)
 			return -1;
		bFirstAdd = FALSE;
 		pTargetDev = (CDataDevice *)m_ptrSortedDevice.GetNext(posDev);
 		if (pTargetDev == nullptr)
 			continue;
 		nTargetIntype = nTargetOuttype = nTargetName = nTargetCont = 0;
 		pEq = pTargetDev->GetEqInput();
 		if (pEq != nullptr)
 			nTargetIntype = (int)pEq->GetEquipID();
 		// 설비명이 없으면 자동생성하지 않는다.
 		pEq = pTargetDev->GetEqName();
 		if (pEq != nullptr)
 			nTargetName = (int)pEq->GetEquipID();
 
 		pEq = pTargetDev->GetEqOutContents();
 		if (pEq != nullptr)
 		{
 			nTargetCont = (int)pEq->GetEquipID();
 			strDebug = pEq->GetEquipName();
 		}
 
 		pEq = pTargetDev->GetEqOutput();
 		if (pEq != nullptr)
 			nTargetOuttype = (int)pEq->GetEquipID();
 
 		pLoc = pTargetDev->GetDevInputLocation();
 		if (pLoc == nullptr)
 			continue;
 
 		
 		// 지하층일 때 입력 로직을 달리 한다.
 		nTargetFlNum = pLoc->GetFloorNumber();
 		strTargetStair = pLoc->GetStairName();
 		strT_Room = pLoc->GetRoomName();
		strT_Build = pLoc->GetBuildName();
		strT_BType = pLoc->GetBTypeName();
		strT_Floor = pLoc->GetFloorName();
 
//  		if (strSrcBuild != pLoc->GetBuildName())
//  			continue;
//  
//  		if (strSrcBtype != pLoc->GetBTypeName())
//  			continue;
//  
//  		if (strSrcStair != strTargetStair)
//  			continue;
 
 		// [KHS 2020-5-15 17:25:05] 
 		// 최용찬과장 요청으로 위치정보가 입력도 위치정보가 없는 출력이랑 매칭 시킴
 // 		if (pLoc->GetFloorName() == L"-" || pLoc->GetFloorName() == L"")
 // 			continue; 
 
 
 		// 		if (nTargetFlNum < 0) // 지하층은 '-'
 		// 		{
 		// 			//AddUnderFloorAutoLink();
 		// 			continue;
 		// 		}
 
 		// [KHS 2020-5-18 11:25:25] 
 		// 교차 감지기 일때
 		// 출력 타입 - 프리액션 , 출력내용 - 밸브 일때 
		if (bCrossDetector)
		{
			if (nTargetOuttype == OUTTYPE_PREACTION && nTargetCont == OUTCONT_VALVE)
			{
				// 교차 감지기 일때는 같은 실일때 동작한다.
				if(IsSameRoom(pSrcDev,pTargetDev) == TRUE)
					bFirstAdd = TRUE;
				//[4/28/2021 KHS] 오류 수정 : 교차 감지기 일때 한번 bFirstAdd가 TRUE 이면 계속 TRUE
				// 추가
				else
					bFirstAdd = FALSE;
			}
			else
				bFirstAdd = FALSE;
			// 추가 끝
		}
 		
 		// m_spRefAutoLogic은 LG_EQNAME_ID, LG_INTYPE_ID, LG_OUTTYPE_ID, LG_OUTCONT_ID 순으로 정렬됨
 		// bCheck가 TRUE인 상태에서 ID가 틀린게 있으면 더이상 같은 로직이 있는지 체크 하지 않는다.
 		nAddCnt = 0; 
 		posLogic = m_spRefAutoLogic->GetHeadPosition();
 		while (posLogic)
 		{
 			bAdded = FALSE;
 			pLogic = (CDataAutoLogic*)m_spRefAutoLogic->GetNext(posLogic);
 			if (pLogic == nullptr)
 				continue;
 
 			// 동작 장비 : 설비명 과 입력 타입을 비교
 			// 연동 출력 데이터 : 전체 While 돌면서 로직과 출력이 일치하는거
 			if (pLogic->GetEqName() != nSrcName)
 				continue;
 			if (pLogic->GetInType() != nSrcIntype)
 				continue;
 			if (pLogic->GetOutType() != nTargetOuttype)
 				continue;
 			if (pLogic->GetOutContents() != nTargetCont)
 				continue;
			
			// 로직에서 설정된 위치정보와 같은지 확인
			bLocMatch = CheckMatchLoc(pSrcDev, pTargetDev, pLogic);
			
 			// [KHS 2020-6-11 09:41:25] 
 			// 전층 , +N층 , 해당층(AND 조건으로 실구분) , 출력회로
 			if (!pLogic->GetUseAllFloor())
 			{
				if (bLocMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
 			}
			else
			{
				pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
				bAdded = TRUE;
			}
			// Plus N층까지 연동
			if (bAdded == FALSE && nSrcFlNum < N_FLOOR_NONE_NAME && pLogic->GetUsePluseNFloor())
			{
				// 지하 층일때 무시
				if (nSrcFlNum > 0 && nSrcFlNum <= nTargetFlNum
					&& nSrcFlNum + pLogic->GetUsePluseNFloor() >= nTargetFlNum)
				{
					bLevelMatch = CheckMatchLoc(pSrcDev, pTargetDev
						, pLogic->GetUseMatchBuild(), pLogic->GetUseMatchBType()
						, pLogic->GetUseMatchStair(), 0, pLogic->GetUseMatchRoom());
					if (bLevelMatch)
					{
						pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
						bAdded = TRUE;
					}
				}
			}

			/// 출력회로
			if (bAdded == FALSE &&pLogic->GetUseOutput())
			{
				if (pSrcDev->GetFacpID() == pTargetDev->GetFacpID()
					&& pSrcDev->GetUnitID() == pTargetDev->GetUnitID()
					&& pSrcDev->GetChnID() == pTargetDev->GetChnID()
					&& pSrcDev->GetDeviceID() == pTargetDev->GetDeviceID()
					)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}


 			// [KHS 2020-6-11 10:24:05] 
 			// 1층 화재 시 지하 1층 연동
 			// 지하층 로직 
 			if (bAdded == FALSE && pLogic->GetUseUnderBasic())
 			{
				bUnderMatch = CheckEtcLoc(pSrcDev, pTargetDev, pLogic);
				if (bUnderMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
 			}
			
 			if (bAdded)
 			{
 				nAddCnt++;
 				break;
 			}
 		}
 
 		// Pattern 검색 - 모든 패턴 중에 있는지 검색
 		if (nAddCnt > 0)
 		{
 			MakeTempPatternData(pSourceAuto, pTargetDev);
 		}
 	}
	return 1;
}

int CFormAutoMake::AddIndividualAutoLink4(CDataAutoMake * pSourceAuto)
{

	CDataAutoLogic * pLogic;
	POSITION posDev, posLogic;
	CDataDevice * pSrcDev, *pTargetDev;
	CDataEquip * pEq;
	CDataLocBase * pLoc;
	CDataSystem * pSys;
	CString strCont, strSrcStair, strTargetStair, strSrcBuild, strSrcBtype, strSrcFloor, strS_Room, strT_Room;
	CString strT_Build, strT_BType, strT_Floor;
	int nSrcIntype, nSrcOuttype, nSrcName, nSrcCont, nSrcFlNum;
	int nTargetIntype, nTargetOuttype, nTargetName, nTargetCont, nTargetFlNum;
	//int nLi, nLo, nLn, nLc; // 마지막 값
	int nLogicOut = 0, nAddCnt = 0;
	BOOL bAdded = FALSE;
	BOOL bFirstAdd = FALSE;
	BOOL bLocMatch = FALSE, bLevelMatch = FALSE, bParkingMatch = FALSE, bUnderMatch = FALSE;
	BOOL bCrossDetector = FALSE;
	CString strDebug = L"";
	nSrcIntype = nSrcOuttype = nSrcName = nSrcCont = 0;
	nSrcFlNum = N_FLOOR_NONE_NAME; // 층의 기본값 1000
	strSrcBuild = strSrcBtype = strSrcFloor = strSrcStair = strS_Room = strT_Room = L""; // 층의 기본값 1000
	strT_Build = strT_BType = strT_Floor = L""; // 층의 기본값 1000
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
		// 교차 감지기 인지 확인
		if(nSrcIntype == INTYPE_CROSSA || nSrcIntype == INTYPE_CROSSB
			|| nSrcIntype == INTYPE_CROSS16_A || nSrcIntype == INTYPE_CROSS17_B
			|| nSrcIntype == INTYPE_CROSS18_A || nSrcIntype == INTYPE_CROSS19_B
			)
			bCrossDetector = TRUE;
	}


	// 설비명이 없으면 자동생성하지 않는다.
	if (pSrcDev->GetEqName() == nullptr)
		return 0;
	pEq = pSrcDev->GetEqName();
	if (pEq != nullptr)
		nSrcName = (int)pEq->GetEquipID();

	pEq = pSrcDev->GetEqOutContents();
	if (pEq != nullptr)
		nSrcCont = (int)pEq->GetEquipID();

	pEq = pSrcDev->GetEqOutput();
	if (pEq != nullptr)
		nSrcOuttype = (int)pEq->GetEquipID();

	pLoc = pSrcDev->GetDevInputLocation();
	if (pLoc != nullptr)
	{
		nSrcFlNum = pLoc->GetFloorNumber();
		strSrcStair = pLoc->GetStairName();
		strSrcBuild = pLoc->GetBuildName();
		strSrcBtype = pLoc->GetBTypeName();
		strSrcFloor = pLoc->GetFloorName();
		strS_Room = pLoc->GetRoomName();
	}
	// 	if (nSrcFlNum < 0) // 지하층은 -1 부터 시작
	// 	{
	// 		AddUnderFloorAutoLink();
	// 	}

	// [KHS 2020-2-25 13:53:24] 
	// 로직과 일치 여부 
	posDev = m_ptrSortedDevice.GetHeadPosition();
	while (posDev)
	{
		if (m_bStopFlag == TRUE)
			return -1;
		bFirstAdd = FALSE;
		pTargetDev = (CDataDevice *)m_ptrSortedDevice.GetNext(posDev);
		if (pTargetDev == nullptr)
			continue;
		nTargetIntype = nTargetOuttype = nTargetName = nTargetCont = 0;
		pEq = pTargetDev->GetEqInput();
		if (pEq != nullptr)
			nTargetIntype = (int)pEq->GetEquipID();
		// 설비명이 없으면 자동생성하지 않는다.
		pEq = pTargetDev->GetEqName();
		if (pEq != nullptr)
			nTargetName = (int)pEq->GetEquipID();

		pEq = pTargetDev->GetEqOutContents();
		if (pEq != nullptr)
		{
			nTargetCont = (int)pEq->GetEquipID();
			strDebug = pEq->GetEquipName();
		}

		pEq = pTargetDev->GetEqOutput();
		if (pEq != nullptr)
			nTargetOuttype = (int)pEq->GetEquipID();

		pLoc = pTargetDev->GetDevInputLocation();
		if (pLoc == nullptr)
			continue;


		// 지하층일 때 입력 로직을 달리 한다.
		nTargetFlNum = pLoc->GetFloorNumber();
		strTargetStair = pLoc->GetStairName();
		strT_Room = pLoc->GetRoomName();
		strT_Build = pLoc->GetBuildName();
		strT_BType = pLoc->GetBTypeName();
		strT_Floor = pLoc->GetFloorName();

		//  		if (strSrcBuild != pLoc->GetBuildName())
		//  			continue;
		//  
		//  		if (strSrcBtype != pLoc->GetBTypeName())
		//  			continue;
		//  
		//  		if (strSrcStair != strTargetStair)
		//  			continue;

		// [KHS 2020-5-15 17:25:05] 
		// 최용찬과장 요청으로 위치정보가 입력도 위치정보가 없는 출력이랑 매칭 시킴
		// 		if (pLoc->GetFloorName() == L"-" || pLoc->GetFloorName() == L"")
		// 			continue; 


		// 		if (nTargetFlNum < 0) // 지하층은 '-'
		// 		{
		// 			//AddUnderFloorAutoLink();
		// 			continue;
		// 		}

		// [KHS 2020-5-18 11:25:25] 
		// 교차 감지기 일때
		// 출력 타입 - 프리액션 , 출력내용 - 밸브 일때 
		if (bCrossDetector)
		{
			if (nTargetOuttype == OUTTYPE_PREACTION && nTargetCont == OUTCONT_VALVE)
			{
				// 교차 감지기 일때는 같은 실일때 동작한다.
				if (IsSameRoom(pSrcDev, pTargetDev) == TRUE)
					bFirstAdd = TRUE;
				//[4/28/2021 KHS] 오류 수정 : 교차 감지기 일때 한번 bFirstAdd가 TRUE 이면 계속 TRUE
				// 추가
				else
					bFirstAdd = FALSE;
			}
			else
				bFirstAdd = FALSE;
			// 추가 끝
		}

		// m_spRefAutoLogic은 LG_EQNAME_ID, LG_INTYPE_ID, LG_OUTTYPE_ID, LG_OUTCONT_ID 순으로 정렬됨
		// bCheck가 TRUE인 상태에서 ID가 틀린게 있으면 더이상 같은 로직이 있는지 체크 하지 않는다.
		nAddCnt = 0;
		posLogic = m_spRefAutoLogic->GetHeadPosition();
		while (posLogic)
		{
			bAdded = FALSE;
			pLogic = (CDataAutoLogic*)m_spRefAutoLogic->GetNext(posLogic);
			if (pLogic == nullptr)
				continue;

			// 동작 장비 : 설비명 과 입력 타입을 비교
			// 연동 출력 데이터 : 전체 While 돌면서 로직과 출력이 일치하는거
			if (pLogic->GetEqName() != nSrcName)
				continue;
			if (pLogic->GetInType() != nSrcIntype)
				continue;
			if (pLogic->GetOutType() != nTargetOuttype)
				continue;
			if (pLogic->GetOutContents() != nTargetCont)
				continue;

			// 로직에서 설정된 위치정보와 같은지 확인
			bLocMatch = CheckMatchLoc(pSrcDev, pTargetDev, pLogic);

			// [KHS 2020-6-11 09:41:25] 
			// 전층 , +N층 , 해당층(AND 조건으로 실구분) , 출력회로
			if (!pLogic->GetUseAllFloor())
			{
				if (bLocMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}
			else
			{
				pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
				bAdded = TRUE;
			}
			// Plus N층까지 연동
			if (bAdded == FALSE && nSrcFlNum < N_FLOOR_NONE_NAME && pLogic->GetUsePluseNFloor())
			{
				// 지하 층일때 무시
				if (nSrcFlNum > 0 && nSrcFlNum <= nTargetFlNum
					&& nSrcFlNum + pLogic->GetUsePluseNFloor() >= nTargetFlNum)
				{
					bLevelMatch = CheckMatchLoc(pSrcDev, pTargetDev
						, pLogic->GetUseMatchBuild(), pLogic->GetUseMatchBType()
						, pLogic->GetUseMatchStair(), 0, pLogic->GetUseMatchRoom());
					if (bLevelMatch)
					{
						pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
						bAdded = TRUE;
					}
				}
			}

			/// 출력회로
			if (bAdded == FALSE &&pLogic->GetUseOutput())
			{
				if (pSrcDev->GetFacpID() == pTargetDev->GetFacpID()
					&& pSrcDev->GetUnitID() == pTargetDev->GetUnitID()
					&& pSrcDev->GetChnID() == pTargetDev->GetChnID()
					&& pSrcDev->GetDeviceID() == pTargetDev->GetDeviceID()
					)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}


			// [KHS 2020-6-11 10:24:05] 
			// 1층 화재 시 지하 1층 연동
			// 지하층 로직 
			if (bAdded == FALSE && pLogic->GetUseUnderBasic())
			{
				bUnderMatch = CheckEtcLoc(pSrcDev, pTargetDev, pLogic);
				if (bUnderMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}

// 			if (bAdded == FALSE && pLogic->GetUseParkingBasic())
// 			{
// 				bParkingMatch = CheckEtcLoc(pSrcDev, pTargetDev, pLogic, 0);
// 				if (bParkingMatch)
// 				{
// 					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
// 					bAdded = TRUE;
// 				}
// 			}

			if (bAdded)
			{
				nAddCnt++;
				break;
			}
		}

		// Pattern 검색 - 모든 패턴 중에 있는지 검색
		if (nAddCnt > 0)
		{
			MakeTempPatternData(pSourceAuto, pTargetDev);
		}
	}
	return 1;
}

int CFormAutoMake::AddIndividualAutoLink2(CDataAutoMake * pSourceAuto, YAdoDatabase *pDBUtil)
{
	CDataAutoLogic * pLogic;
	POSITION posLogic;
	CDataDevice * pSrcDev, *pTargetDev;
	CDataEquip * pEq;
	CDataLocBase * pLoc;
	CDataSystem * pSys;
	CString strCont, strSrcStair, strTargetStair, strSrcBuild, strSrcBtype, strSrcFloor, strS_Room, strT_Room;
	CString strT_Build, strT_BType, strT_Floor;
	int nSrcIntype, nSrcOuttype, nSrcName, nSrcCont, nSrcFlNum;
	int nTargetIntype, nTargetOuttype, nTargetName, nTargetCont, nTargetFlNum;
	//int nLi, nLo, nLn, nLc; // 마지막 값
	int nLogicOut = 0, nAddCnt = 0;
	BOOL bAdded = FALSE;
	BOOL bFirstAdd = FALSE;
	BOOL bLocMatch = FALSE, bLevelMatch = FALSE, bParkingMatch = FALSE, bUnderMatch = FALSE;
	BOOL bCrossDetector = FALSE;
	CString strDebug = L"";
	CString strSql, strKey;
	int nCnt, i;
	CRelayTableData * pRefTable = theApp.GetRelayTableData();
	if (pRefTable == nullptr)
		return -1;

	if (pDBUtil == nullptr)
		return -1;

	nSrcIntype = nSrcOuttype = nSrcName = nSrcCont = 0;
	nSrcFlNum = N_FLOOR_NONE_NAME; // 층의 기본값 1000
	strSrcBuild = strSrcBtype = strSrcFloor = strSrcStair = strS_Room = strT_Room = L""; // 층의 기본값 1000
	strT_Build = strT_BType = strT_Floor = L""; // 층의 기본값 1000
	pSys = pSourceAuto->GetSysData();
	if (pSys == nullptr || pSys->GetSysData() == nullptr)
		return 0;
	if (pSys->GetDataType() != SE_RELAY)
		return 0;
	pSrcDev = (CDataDevice *)pSys->GetSysData();

#if _DBG_MAKE_TIME_
	DWORD_PTR dwStart, dwEnd, dwTemp;
	dwStart = GetTickCount();
#endif

	pEq = pSrcDev->GetEqInput();
	if (pEq != nullptr)
	{
		nSrcIntype = (int)pEq->GetEquipID();
		// 교차 감지기 인지 확인
		if(nSrcIntype == INTYPE_CROSSA || nSrcIntype == INTYPE_CROSSB
			|| nSrcIntype == INTYPE_CROSS16_A || nSrcIntype == INTYPE_CROSS17_B
			|| nSrcIntype == INTYPE_CROSS18_A || nSrcIntype == INTYPE_CROSS19_B
			)
			bCrossDetector = TRUE;
	}
	// 설비명이 없으면 자동생성하지 않는다.
	if (pSrcDev->GetEqName() == nullptr)
		return 0;
	pEq = pSrcDev->GetEqName();
	if (pEq != nullptr)
		nSrcName = (int)pEq->GetEquipID();

	//////////////////////////////////////////////////////////////////////////
	pEq = pSrcDev->GetEqOutContents();
	if (pEq != nullptr)
		nSrcCont = (int)pEq->GetEquipID();

	pEq = pSrcDev->GetEqOutput();
	if (pEq != nullptr)
		nSrcOuttype = (int)pEq->GetEquipID();

	pLoc = pSrcDev->GetDevInputLocation();
	if (pLoc != nullptr)
	{
		nSrcFlNum = pLoc->GetFloorNumber();
		strSrcStair = pLoc->GetStairName();
		strSrcBuild = pLoc->GetBuildName();
		strSrcBtype = pLoc->GetBTypeName();
		strSrcFloor = pLoc->GetFloorName();
		strS_Room = pLoc->GetRoomName();
	}
	// 	if (nSrcFlNum < 0) // 지하층은 -1 부터 시작
	// 	{
	// 		AddUnderFloorAutoLink();
	// 	}

	// [KHS 2020-2-25 13:53:24] 
	// 로직과 일치 여부 
	strSql.Format(L" SELECT  MAP_KEY "
		L" FROM     TB_RELAY_LIST "
		L" WHERE(OUTPUT_ID IN "
		L"		(SELECT  LG_OUTTYPE_ID FROM TB_AUTO_LOGIC_V2 WHERE(LG_INTYPE_ID = %d) AND(LG_EQNAME_ID = %d) "
		L" 		GROUP BY LG_OUTTYPE_ID)) "
		L" AND  (OUTCONTENTS_ID IN "
		L" 		(SELECT  LG_OUTCONT_ID FROM TB_AUTO_LOGIC_V2 AS TB_AUTO_LOGIC_V2_1 WHERE(LG_INTYPE_ID =%d) AND(LG_EQNAME_ID =%d) "
		L" 		GROUP BY LG_OUTCONT_ID)) "
		, nSrcIntype, nSrcName
		, nSrcIntype, nSrcName
	);

	if (pDBUtil->OpenQuery(strSql) == FALSE)
		return -1;
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();
	GF_AddDebug(L"	- AddIndividaulEmergency-OpenQuery : %d", dwEnd - dwStart);
	dwStart = dwEnd;
#endif
	nCnt = pDBUtil->GetRecordCount();
	//pDBUtil->MoveFirst();
	for (i = 0; i < nCnt; i++)
	{
#if _DBG_MAKE_TIME_
		dwEnd = GetTickCount();
		//GF_AddDebug(L"	- AddIndividaulEmergency-OpenQuery : %d", dwEnd - dwStart);
		//dwStart = dwEnd;
#endif
		if (m_bStopFlag == TRUE)
		{
			pDBUtil->RSClose();
			return -1;
		}
		bFirstAdd = FALSE;
		
		if (pDBUtil->GetFieldValue(L"MAP_KEY", strKey) == FALSE)
		{
			pDBUtil->MoveNext();
			continue;
		}

		pTargetDev = pRefTable->GetDevice(strKey);
		if (pTargetDev == nullptr)
		{
			pDBUtil->MoveNext();
			continue;
		}
		nTargetIntype = nTargetOuttype = nTargetName = nTargetCont = 0;
		pEq = pTargetDev->GetEqInput();
		if (pEq != nullptr)
			nTargetIntype = (int)pEq->GetEquipID();
		// 설비명이 없으면 자동생성하지 않는다.
		pEq = pTargetDev->GetEqName();
		if (pEq != nullptr)
			nTargetName = (int)pEq->GetEquipID();

		pEq = pTargetDev->GetEqOutContents();
		if (pEq != nullptr)
		{
			nTargetCont = (int)pEq->GetEquipID();
			strDebug = pEq->GetEquipName();
		}

		pEq = pTargetDev->GetEqOutput();
		if (pEq != nullptr)
			nTargetOuttype = (int)pEq->GetEquipID();

		pLoc = pTargetDev->GetDevInputLocation();
		if (pLoc == nullptr)
		{
			pDBUtil->MoveNext();
			continue;
		}


		// 지하층일 때 입력 로직을 달리 한다.
		nTargetFlNum = pLoc->GetFloorNumber();
		strTargetStair = pLoc->GetStairName();
		strT_Room = pLoc->GetRoomName();
		strT_Build = pLoc->GetBuildName();
		strT_BType = pLoc->GetBTypeName();
		strT_Floor = pLoc->GetFloorName();

		if (bCrossDetector)
		{
			if (nTargetOuttype == OUTTYPE_PREACTION && nTargetCont == OUTCONT_VALVE)
			{
				// 교차 감지기 일때는 같은 실일때 동작한다.
				if (IsSameRoom(pSrcDev, pTargetDev) == TRUE)
					bFirstAdd = TRUE;
				//[4/28/2021 KHS] 오류 수정 : 교차 감지기 일때 한번 bFirstAdd가 TRUE 이면 계속 TRUE
				// 추가
				else
					bFirstAdd = FALSE;
			}
			else
				bFirstAdd = FALSE;
			// 추가 끝
		}
#if _DBG_MAKE_TIME_
		dwEnd = GetTickCount();
		GF_AddDebug(L"	- Device Loop : %d", dwEnd - dwStart);
		dwStart = dwEnd;
		dwTemp = dwEnd;
#endif
		// m_spRefAutoLogic은 LG_EQNAME_ID, LG_INTYPE_ID, LG_OUTTYPE_ID, LG_OUTCONT_ID 순으로 정렬됨
		// bCheck가 TRUE인 상태에서 ID가 틀린게 있으면 더이상 같은 로직이 있는지 체크 하지 않는다.
		nAddCnt = 0;
		posLogic = m_spRefAutoLogic->GetHeadPosition();
		while (posLogic)
		{
			bAdded = FALSE;
			pLogic = (CDataAutoLogic*)m_spRefAutoLogic->GetNext(posLogic);
			if (pLogic == nullptr)
				continue;

			// 동작 장비 : 설비명 과 입력 타입을 비교
			// 연동 출력 데이터 : 전체 While 돌면서 로직과 출력이 일치하는거
// 			if (pLogic->GetEqName() != nSrcName)
// 				continue;
// 			if (pLogic->GetInType() != nSrcIntype)
// 				continue;
			if (pLogic->GetOutType() != nTargetOuttype)
				continue;
			if (pLogic->GetOutContents() != nTargetCont)
				continue;

			// 로직에서 설정된 위치정보와 같은지 확인
			bLocMatch = CheckMatchLoc(pSrcDev, pTargetDev, pLogic);

			// [KHS 2020-6-11 09:41:25] 
			// 전층 , +N층 , 해당층(AND 조건으로 실구분) , 출력회로
			if (!pLogic->GetUseAllFloor())
			{
				if (bLocMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}
			else
			{
				pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
				bAdded = TRUE;
			}
			// Plus N층까지 연동
			if (bAdded == FALSE && nSrcFlNum < N_FLOOR_NONE_NAME && pLogic->GetUsePluseNFloor())
			{
				// 지하 층일때 무시
				if (nSrcFlNum > 0 && nSrcFlNum <= nTargetFlNum
					&& nSrcFlNum + pLogic->GetUsePluseNFloor() >= nTargetFlNum)
				{
					bLevelMatch = CheckMatchLoc(pSrcDev, pTargetDev
						, pLogic->GetUseMatchBuild(), pLogic->GetUseMatchBType()
						, pLogic->GetUseMatchStair(), 0, pLogic->GetUseMatchRoom());
					if (bLevelMatch)
					{
						pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
						bAdded = TRUE;
					}
				}
			}

			/// 출력회로
			if (bAdded == FALSE &&pLogic->GetUseOutput())
			{
				if (pSrcDev->GetFacpID() == pTargetDev->GetFacpID()
					&& pSrcDev->GetUnitID() == pTargetDev->GetUnitID()
					&& pSrcDev->GetChnID() == pTargetDev->GetChnID()
					&& pSrcDev->GetDeviceID() == pTargetDev->GetDeviceID()
					)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}


			// [KHS 2020-6-11 10:24:05] 
			// 1층 화재 시 지하 1층 연동
			// 지하층 로직 
			if (bAdded == FALSE && pLogic->GetUseUnderBasic())
			{
				bUnderMatch = CheckEtcLoc(pSrcDev, pTargetDev, pLogic);
				if (bUnderMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}

			if (bAdded)
			{
				nAddCnt++;
				break;
			}
		}
// #if _DBG_MAKE_TIME_
// 		dwEnd = GetTickCount();
// 		GF_AddDebug(L"	- Logic End : %d", dwEnd - dwTemp);
// 		dwStart = dwEnd;
// #endif
		// Pattern 검색 - 모든 패턴 중에 있는지 검색
		if (nAddCnt > 0)
		{
			MakeTempPatternData(pSourceAuto, pTargetDev);
		}

		pDBUtil->MoveNext();
#if _DBG_MAKE_TIME_
		dwEnd = GetTickCount();
		GF_AddDebug(L"	- AddIndividualAutoLink2-One Device : %d", dwEnd - dwTemp);
		dwStart = dwEnd;
#endif
	}
	pDBUtil->RSClose();
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"	- 자동생성(AddIndividualAutoLink2) : %d" , dwEnd-dwStart);
#else
	GF_AddDebug(L"	- AUTOGENERATE(AddIndividualAutoLink2) : %d", dwEnd - dwStart);
#endif

#endif
	return 1;
}


int CFormAutoMake::AddIndividualAutoLink3(CDataAutoMake * pSourceAuto, YAdoDatabase *pDBUtil)
{
	CDataAutoLogic * pLogic;
	POSITION posLogic;
	CDataDevice * pSrcDev, *pTargetDev;
	CDataEquip * pEq;
	CDataLocBase * pLoc;
	CDataSystem * pSys;
	CString strCont, strSrcStair, strTargetStair, strSrcBuild, strSrcBtype, strSrcFloor, strS_Room, strT_Room;
	CString strT_Build, strT_BType, strT_Floor;
	int nSrcIntype, nSrcOuttype, nSrcName, nSrcCont, nSrcFlNum;
	int nTargetIntype, nTargetOuttype, nTargetName, nTargetCont, nTargetFlNum;
	//int nLi, nLo, nLn, nLc; // 마지막 값
	int nLogicOut = 0, nAddCnt = 0;
	BOOL bAdded = FALSE;
	BOOL bFirstAdd = FALSE;
	BOOL bLocMatch = FALSE, bLevelMatch = FALSE, bParkingMatch = FALSE, bUnderMatch = FALSE;
	BOOL bCrossDetector = FALSE;
	CString strDebug = L"";
	CString strSql, strKey;
	int nCnt, i;
	CRelayTableData * pRefTable = theApp.GetRelayTableData();
	if (pRefTable == nullptr)
		return -1;

	if (pDBUtil == nullptr)
		return -1;

	nSrcIntype = nSrcOuttype = nSrcName = nSrcCont = 0;
	nSrcFlNum = N_FLOOR_NONE_NAME; // 층의 기본값 1000
	strSrcBuild = strSrcBtype = strSrcFloor = strSrcStair = strS_Room = strT_Room = L""; // 층의 기본값 1000
	strT_Build = strT_BType = strT_Floor = L""; // 층의 기본값 1000
	pSys = pSourceAuto->GetSysData();
	if (pSys == nullptr || pSys->GetSysData() == nullptr)
		return 0;
	if (pSys->GetDataType() != SE_RELAY)
		return 0;
	pSrcDev = (CDataDevice *)pSys->GetSysData();

#if _DBG_MAKE_TIME_
	DWORD_PTR dwStart, dwEnd, dwTemp;
	dwStart = GetTickCount();
#endif

	pEq = pSrcDev->GetEqInput();
	if (pEq != nullptr)
	{
		nSrcIntype = (int)pEq->GetEquipID();
		// 교차 감지기 인지 확인
		if(nSrcIntype == INTYPE_CROSSA || nSrcIntype == INTYPE_CROSSB
			|| nSrcIntype == INTYPE_CROSS16_A || nSrcIntype == INTYPE_CROSS17_B
			|| nSrcIntype == INTYPE_CROSS18_A || nSrcIntype == INTYPE_CROSS19_B
			)
			bCrossDetector = TRUE;
	}
	// 설비명이 없으면 자동생성하지 않는다.
	if (pSrcDev->GetEqName() == nullptr)
		return 0;
	pEq = pSrcDev->GetEqName();
	if (pEq != nullptr)
		nSrcName = (int)pEq->GetEquipID();

	//////////////////////////////////////////////////////////////////////////
	pEq = pSrcDev->GetEqOutContents();
	if (pEq != nullptr)
		nSrcCont = (int)pEq->GetEquipID();

	pEq = pSrcDev->GetEqOutput();
	if (pEq != nullptr)
		nSrcOuttype = (int)pEq->GetEquipID();

	pLoc = pSrcDev->GetDevInputLocation();
	if (pLoc != nullptr)
	{
		nSrcFlNum = pLoc->GetFloorNumber();
		strSrcStair = pLoc->GetStairName();
		strSrcBuild = pLoc->GetBuildName();
		strSrcBtype = pLoc->GetBTypeName();
		strSrcFloor = pLoc->GetFloorName();
		strS_Room = pLoc->GetRoomName();
	}
	// 	if (nSrcFlNum < 0) // 지하층은 -1 부터 시작
	// 	{
	// 		AddUnderFloorAutoLink();
	// 	}

	// [KHS 2020-2-25 13:53:24] 
	// 로직과 일치 여부 
	strSql.Format(L" SELECT  MAP_KEY "
		L" FROM     TB_RELAY_LIST "
		L" WHERE(OUTPUT_ID IN "
		L"		(SELECT  LG_OUTTYPE_ID FROM TB_AUTO_LOGIC_V2 WHERE(LG_INTYPE_ID = %d) AND(LG_EQNAME_ID = %d) "
		L" 		GROUP BY LG_OUTTYPE_ID)) "
		L" AND  (OUTCONTENTS_ID IN "
		L" 		(SELECT  LG_OUTCONT_ID FROM TB_AUTO_LOGIC_V2 AS TB_AUTO_LOGIC_V2_1 WHERE(LG_INTYPE_ID =%d) AND(LG_EQNAME_ID =%d) "
		L" 		GROUP BY LG_OUTCONT_ID)) "
		, nSrcIntype, nSrcName
		, nSrcIntype, nSrcName
	);

	if (pDBUtil->OpenQuery(strSql) == FALSE)
		return -1;
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();
	GF_AddDebug(L"	- AddIndividaulEmergency-OpenQuery : %d", dwEnd - dwStart);
	dwStart = dwEnd;
#endif
	nCnt = pDBUtil->GetRecordCount();
	//pDBUtil->MoveFirst();
	for (i = 0; i < nCnt; i++)
	{
#if _DBG_MAKE_TIME_
		dwEnd = GetTickCount();
		//GF_AddDebug(L"	- AddIndividaulEmergency-OpenQuery : %d", dwEnd - dwStart);
		//dwStart = dwEnd;
#endif
		if (m_bStopFlag == TRUE)
		{
			pDBUtil->RSClose();
			return -1;
		}
		bFirstAdd = FALSE;

		if (pDBUtil->GetFieldValue(L"MAP_KEY", strKey) == FALSE)
		{
			pDBUtil->MoveNext();
			continue;
		}

		pTargetDev = pRefTable->GetDevice(strKey);
		if (pTargetDev == nullptr)
		{
			pDBUtil->MoveNext();
			continue;
		}
		nTargetIntype = nTargetOuttype = nTargetName = nTargetCont = 0;
		pEq = pTargetDev->GetEqInput();
		if (pEq != nullptr)
			nTargetIntype = (int)pEq->GetEquipID();
		// 설비명이 없으면 자동생성하지 않는다.
		pEq = pTargetDev->GetEqName();
		if (pEq != nullptr)
			nTargetName = (int)pEq->GetEquipID();

		pEq = pTargetDev->GetEqOutContents();
		if (pEq != nullptr)
		{
			nTargetCont = (int)pEq->GetEquipID();
			strDebug = pEq->GetEquipName();
		}

		pEq = pTargetDev->GetEqOutput();
		if (pEq != nullptr)
			nTargetOuttype = (int)pEq->GetEquipID();

		pLoc = pTargetDev->GetDevInputLocation();
		if (pLoc == nullptr)
		{
			pDBUtil->MoveNext();
			continue;
		}


		// 지하층일 때 입력 로직을 달리 한다.
		nTargetFlNum = pLoc->GetFloorNumber();
		strTargetStair = pLoc->GetStairName();
		strT_Room = pLoc->GetRoomName();
		strT_Build = pLoc->GetBuildName();
		strT_BType = pLoc->GetBTypeName();
		strT_Floor = pLoc->GetFloorName();

		if (bCrossDetector)
		{
			if (nTargetOuttype == OUTTYPE_PREACTION && nTargetCont == OUTCONT_VALVE)
			{
				// 교차 감지기 일때는 같은 실일때 동작한다.
				if (IsSameRoom(pSrcDev, pTargetDev) == TRUE)
					bFirstAdd = TRUE;
				//[4/28/2021 KHS] 오류 수정 : 교차 감지기 일때 한번 bFirstAdd가 TRUE 이면 계속 TRUE
				//[4/28/2021 KHS] 오류 수정 : 교차 감지기 일때 한번 bFirstAdd가 TRUE 이면 계속 TRUE
				// 추가
				else
					bFirstAdd = FALSE;
			}
			else
				bFirstAdd = FALSE;
			// 추가 끝
		}
// #if _DBG_MAKE_TIME_
// 		dwEnd = GetTickCount();
// 		GF_AddDebug(L"	- Device Loop : %d", dwEnd - dwStart);
// 		dwStart = dwEnd;
// 		dwTemp = dwEnd;
// #endif
		// m_spRefAutoLogic은 LG_EQNAME_ID, LG_INTYPE_ID, LG_OUTTYPE_ID, LG_OUTCONT_ID 순으로 정렬됨
		// bCheck가 TRUE인 상태에서 ID가 틀린게 있으면 더이상 같은 로직이 있는지 체크 하지 않는다.
		nAddCnt = 0;
		posLogic = m_spRefAutoLogic->GetHeadPosition();
		while (posLogic)
		{
			bAdded = FALSE;
			pLogic = (CDataAutoLogic*)m_spRefAutoLogic->GetNext(posLogic);
			if (pLogic == nullptr)
				continue;

			// 동작 장비 : 설비명 과 입력 타입을 비교
			// 연동 출력 데이터 : 전체 While 돌면서 로직과 출력이 일치하는거
			// 			if (pLogic->GetEqName() != nSrcName)
			// 				continue;
			// 			if (pLogic->GetInType() != nSrcIntype)
			// 				continue;
			if (pLogic->GetOutType() != nTargetOuttype)
				continue;
			if (pLogic->GetOutContents() != nTargetCont)
				continue;

			// 로직에서 설정된 위치정보와 같은지 확인
			bLocMatch = CheckMatchLoc(pSrcDev, pTargetDev, pLogic);

			// [KHS 2020-6-11 09:41:25] 
			// 전층 , +N층 , 해당층(AND 조건으로 실구분) , 출력회로
			if (!pLogic->GetUseAllFloor())
			{
				if (bLocMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}
			else
			{
				pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
				bAdded = TRUE;
			}
			// Plus N층까지 연동
			if (bAdded == FALSE && nSrcFlNum < N_FLOOR_NONE_NAME && pLogic->GetUsePluseNFloor())
			{
				// 지하 층일때 무시
				if (nSrcFlNum > 0 && nSrcFlNum <= nTargetFlNum
					&& nSrcFlNum + pLogic->GetUsePluseNFloor() >= nTargetFlNum)
				{
					bLevelMatch = CheckMatchLoc(pSrcDev, pTargetDev
						, pLogic->GetUseMatchBuild(), pLogic->GetUseMatchBType()
						, pLogic->GetUseMatchStair(), 0, pLogic->GetUseMatchRoom());
					if (bLevelMatch)
					{
						pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
						bAdded = TRUE;
					}
				}
			}

			/// 출력회로
			if (bAdded == FALSE &&pLogic->GetUseOutput())
			{
				if (pSrcDev->GetFacpID() == pTargetDev->GetFacpID()
					&& pSrcDev->GetUnitID() == pTargetDev->GetUnitID()
					&& pSrcDev->GetChnID() == pTargetDev->GetChnID()
					&& pSrcDev->GetDeviceID() == pTargetDev->GetDeviceID()
					)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}


			// [KHS 2020-6-11 10:24:05] 
			// 1층 화재 시 지하 1층 연동
			// 지하층 로직 
			if (bAdded == FALSE && pLogic->GetUseUnderBasic())
			{
				bUnderMatch = CheckEtcLoc(pSrcDev, pTargetDev, pLogic);
				if (bUnderMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}

			if (bAdded)
			{
				nAddCnt++;
				break;
			}
		}
		// #if _DBG_MAKE_TIME_
		// 		dwEnd = GetTickCount();
		// 		GF_AddDebug(L"	- Logic End : %d", dwEnd - dwTemp);
		// 		dwStart = dwEnd;
		// #endif
		// Pattern 검색 - 모든 패턴 중에 있는지 검색
		if (nAddCnt > 0)
		{
			MakeTempPatternData(pSourceAuto, pTargetDev);
		}

		pDBUtil->MoveNext();
#if _DBG_MAKE_TIME_
		dwEnd = GetTickCount();
		GF_AddDebug(L"	- AddIndividualAutoLink3-One Device : %d", dwEnd - dwStart);
		dwStart = dwEnd;
#endif
	}
	pDBUtil->RSClose();
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"	- 자동생성(AddIndividualAutoLink3) : %d", dwEnd - dwStart);
#else
	GF_AddDebug(L"	- AUTOGENERATE(AddIndividualAutoLink3) : %d", dwEnd - dwStart);
#endif

#endif
	return 1;
}


int CFormAutoMake::AddIndividualAutoLink5(CDataAutoMake * pSourceAuto, YAdoDatabase *pDBUtil)
{
	CDataAutoLogic * pLogic;
	POSITION posLogic;
	CDataDevice * pSrcDev, *pTargetDev;
	CDataEquip * pEq;
	CDataLocBase * pLoc;
	CDataSystem * pSys;
	CString strCont, strSrcStair, strTargetStair, strSrcBuild, strSrcBtype, strSrcFloor, strS_Room, strT_Room;
	CString strT_Build, strT_BType, strT_Floor;
	int nSrcIntype, nSrcOuttype, nSrcName, nSrcCont, nSrcFlNum;
	int nTargetIntype, nTargetOuttype, nTargetName, nTargetCont, nTargetFlNum;
	//int nLi, nLo, nLn, nLc; // 마지막 값
	int nLogicOut = 0, nAddCnt = 0;
	BOOL bAdded = FALSE;
	BOOL bFirstAdd = FALSE;
	BOOL bLocMatch = FALSE, bLevelMatch = FALSE, bParkingMatch = FALSE, bUnderMatch = FALSE;
	BOOL bCrossDetector = FALSE;
	CString strDebug = L"";
	CString strSql, strKey;
	int nCnt, i;
	CRelayTableData * pRefTable = theApp.GetRelayTableData();
	if (pRefTable == nullptr)
		return -1;

	if (pDBUtil == nullptr)
		return -1;

	nSrcIntype = nSrcOuttype = nSrcName = nSrcCont = 0;
	nSrcFlNum = N_FLOOR_NONE_NAME; // 층의 기본값 1000
	strSrcBuild = strSrcBtype = strSrcFloor = strSrcStair = strS_Room = strT_Room = L""; // 층의 기본값 1000
	strT_Build = strT_BType = strT_Floor = L""; // 층의 기본값 1000
	pSys = pSourceAuto->GetSysData();
	if (pSys == nullptr || pSys->GetSysData() == nullptr)
		return 0;
	if (pSys->GetDataType() != SE_RELAY)
		return 0;
	pSrcDev = (CDataDevice *)pSys->GetSysData();

#if _DBG_MAKE_TIME_
	DWORD_PTR dwStart, dwEnd, dwTemp;
	dwStart = GetTickCount();
#endif

	pEq = pSrcDev->GetEqInput();
	if (pEq != nullptr)
	{
		nSrcIntype = (int)pEq->GetEquipID();
		// 교차 감지기 인지 확인
		if(nSrcIntype == INTYPE_CROSSA || nSrcIntype == INTYPE_CROSSB
			|| nSrcIntype == INTYPE_CROSS16_A || nSrcIntype == INTYPE_CROSS17_B
			|| nSrcIntype == INTYPE_CROSS18_A || nSrcIntype == INTYPE_CROSS19_B
			)
			bCrossDetector = TRUE;
	}
	// 설비명이 없으면 자동생성하지 않는다.
	if (pSrcDev->GetEqName() == nullptr)
		return 0;
	pEq = pSrcDev->GetEqName();
	if (pEq != nullptr)
		nSrcName = (int)pEq->GetEquipID();

	//////////////////////////////////////////////////////////////////////////
	pEq = pSrcDev->GetEqOutContents();
	if (pEq != nullptr)
		nSrcCont = (int)pEq->GetEquipID();

	pEq = pSrcDev->GetEqOutput();
	if (pEq != nullptr)
		nSrcOuttype = (int)pEq->GetEquipID();

	pLoc = pSrcDev->GetDevInputLocation();
	if (pLoc != nullptr)
	{
		nSrcFlNum = pLoc->GetFloorNumber();
		strSrcStair = pLoc->GetStairName();
		strSrcBuild = pLoc->GetBuildName();
		strSrcBtype = pLoc->GetBTypeName();
		strSrcFloor = pLoc->GetFloorName();
		strS_Room = pLoc->GetRoomName();
	}
	// 	if (nSrcFlNum < 0) // 지하층은 -1 부터 시작
	// 	{
	// 		AddUnderFloorAutoLink();
	// 	}

	// [KHS 2020-2-25 13:53:24] 
	// 로직과 일치 여부 
	strSql.Format(L" SELECT  MAP_KEY "
		L" FROM     TB_RELAY_LIST "
		L" WHERE(OUTPUT_ID IN "
		L"		(SELECT  LG_OUTTYPE_ID FROM TB_AUTO_LOGIC_V2 WHERE(LG_INTYPE_ID = %d) AND(LG_EQNAME_ID = %d) "
		L" 		GROUP BY LG_OUTTYPE_ID)) "
		L" AND  (OUTCONTENTS_ID IN "
		L" 		(SELECT  LG_OUTCONT_ID FROM TB_AUTO_LOGIC_V2 AS TB_AUTO_LOGIC_V2_1 WHERE(LG_INTYPE_ID =%d) AND(LG_EQNAME_ID =%d) "
		L" 		GROUP BY LG_OUTCONT_ID)) "
		, nSrcIntype, nSrcName
		, nSrcIntype, nSrcName
	);

	if (pDBUtil->OpenQuery(strSql) == FALSE)
		return -1;
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();
	GF_AddDebug(L"	- AddIndividaulEmergency-OpenQuery : %d", dwEnd - dwStart);
	dwStart = dwEnd;
#endif
	nCnt = pDBUtil->GetRecordCount();
	//pDBUtil->MoveFirst();
	for (i = 0; i < nCnt; i++)
	{
#if _DBG_MAKE_TIME_
		dwEnd = GetTickCount();
		//GF_AddDebug(L"	- AddIndividaulEmergency-OpenQuery : %d", dwEnd - dwStart);
		//dwStart = dwEnd;
#endif
		if (m_bStopFlag == TRUE)
		{
			pDBUtil->RSClose();
			return -1;
		}
		bFirstAdd = FALSE;

		if (pDBUtil->GetFieldValue(L"MAP_KEY", strKey) == FALSE)
		{
			pDBUtil->MoveNext();
			continue;
		}

		pTargetDev = pRefTable->GetDevice(strKey);
		if (pTargetDev == nullptr)
		{
			pDBUtil->MoveNext();
			continue;
		}
		nTargetIntype = nTargetOuttype = nTargetName = nTargetCont = 0;
		pEq = pTargetDev->GetEqInput();
		if (pEq != nullptr)
			nTargetIntype = (int)pEq->GetEquipID();
		// 설비명이 없으면 자동생성하지 않는다.
		pEq = pTargetDev->GetEqName();
		if (pEq != nullptr)
			nTargetName = (int)pEq->GetEquipID();

		pEq = pTargetDev->GetEqOutContents();
		if (pEq != nullptr)
		{
			nTargetCont = (int)pEq->GetEquipID();
			strDebug = pEq->GetEquipName();
		}

		pEq = pTargetDev->GetEqOutput();
		if (pEq != nullptr)
			nTargetOuttype = (int)pEq->GetEquipID();

		pLoc = pTargetDev->GetDevInputLocation();
		if (pLoc == nullptr)
		{
			pDBUtil->MoveNext();
			continue;
		}


		// 지하층일 때 입력 로직을 달리 한다.
		nTargetFlNum = pLoc->GetFloorNumber();
		strTargetStair = pLoc->GetStairName();
		strT_Room = pLoc->GetRoomName();
		strT_Build = pLoc->GetBuildName();
		strT_BType = pLoc->GetBTypeName();
		strT_Floor = pLoc->GetFloorName();

		if (bCrossDetector)
		{
			if (nTargetOuttype == OUTTYPE_PREACTION && nTargetCont == OUTCONT_VALVE)
			{
				// 교차 감지기 일때는 같은 실일때 동작한다.
				if (IsSameRoom(pSrcDev, pTargetDev) == TRUE)
					bFirstAdd = TRUE;
				//[4/28/2021 KHS] 오류 수정 : 교차 감지기 일때 한번 bFirstAdd가 TRUE 이면 계속 TRUE
// 추가
				else
					bFirstAdd = FALSE;
			}
			else
				bFirstAdd = FALSE;
// 추가 끝

		}
		// #if _DBG_MAKE_TIME_
		// 		dwEnd = GetTickCount();
		// 		GF_AddDebug(L"	- Device Loop : %d", dwEnd - dwStart);
		// 		dwStart = dwEnd;
		// 		dwTemp = dwEnd;
		// #endif
		// m_spRefAutoLogic은 LG_EQNAME_ID, LG_INTYPE_ID, LG_OUTTYPE_ID, LG_OUTCONT_ID 순으로 정렬됨
		// bCheck가 TRUE인 상태에서 ID가 틀린게 있으면 더이상 같은 로직이 있는지 체크 하지 않는다.
		nAddCnt = 0;
		posLogic = m_spRefAutoLogic->GetHeadPosition();
		while (posLogic)
		{
			bAdded = FALSE;
			pLogic = (CDataAutoLogic*)m_spRefAutoLogic->GetNext(posLogic);
			if (pLogic == nullptr)
				continue;

			// 동작 장비 : 설비명 과 입력 타입을 비교
			// 연동 출력 데이터 : 전체 While 돌면서 로직과 출력이 일치하는거
			// 			if (pLogic->GetEqName() != nSrcName)
			// 				continue;
			// 			if (pLogic->GetInType() != nSrcIntype)
			// 				continue;
			if (pLogic->GetOutType() != nTargetOuttype)
				continue;
			if (pLogic->GetOutContents() != nTargetCont)
				continue;

			// 로직에서 설정된 위치정보와 같은지 확인
			bLocMatch = CheckMatchLoc(pSrcDev, pTargetDev, pLogic);

			// [KHS 2020-6-11 09:41:25] 
			// 전층 , +N층 , 해당층(AND 조건으로 실구분) , 출력회로
			if (!pLogic->GetUseAllFloor())
			{
				if (bLocMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}
			else
			{
				pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
				bAdded = TRUE;
			}
			// Plus N층까지 연동
			if (bAdded == FALSE && nSrcFlNum < N_FLOOR_NONE_NAME && pLogic->GetUsePluseNFloor())
			{
				// 지하 층일때 무시
				if (nSrcFlNum > 0 && nSrcFlNum <= nTargetFlNum
					&& nSrcFlNum + pLogic->GetUsePluseNFloor() >= nTargetFlNum)
				{
					bLevelMatch = CheckMatchLoc(pSrcDev, pTargetDev
						, pLogic->GetUseMatchBuild(), pLogic->GetUseMatchBType()
						, pLogic->GetUseMatchStair(), 0, pLogic->GetUseMatchRoom());
					if (bLevelMatch)
					{
						pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
						bAdded = TRUE;
					}
				}
			}

			/// 출력회로
			if (bAdded == FALSE &&pLogic->GetUseOutput())
			{
				if (pSrcDev->GetFacpID() == pTargetDev->GetFacpID()
					&& pSrcDev->GetUnitID() == pTargetDev->GetUnitID()
					&& pSrcDev->GetChnID() == pTargetDev->GetChnID()
					&& pSrcDev->GetDeviceID() == pTargetDev->GetDeviceID()
					)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}


			// [KHS 2020-6-11 10:24:05] 
			// 1층 화재 시 지하 1층 연동
			// 지하층 로직 
			if (bAdded == FALSE && pLogic->GetUseUnderBasic())
			{
				bUnderMatch = CheckEtcLoc(pSrcDev, pTargetDev, pLogic);
				if (bUnderMatch)
				{
					pSrcDev->AddLink(bFirstAdd, pTargetDev, LK_TYPE_RELEAY, LOGIC_ALL_AUTO, pLogic->GetLgId());
					bAdded = TRUE;
				}
			}

			if (bAdded)
			{
				nAddCnt++;
				break;
			}
		}
		// #if _DBG_MAKE_TIME_
		// 		dwEnd = GetTickCount();
		// 		GF_AddDebug(L"	- Logic End : %d", dwEnd - dwTemp);
		// 		dwStart = dwEnd;
		// #endif
		// Pattern 검색 - 모든 패턴 중에 있는지 검색
		if (nAddCnt > 0)
		{
			MakeTempPatternData(pSourceAuto, pTargetDev);
		}

		pDBUtil->MoveNext();
#if _DBG_MAKE_TIME_
		dwEnd = GetTickCount();
		GF_AddDebug(L"	- AddIndividualAutoLink5-One Device : %d", dwEnd - dwStart);
		dwStart = dwEnd;
#endif
	}
	pDBUtil->RSClose();
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"	- 자동생성(AddIndividualAutoLink5) : %d", dwEnd - dwStart);
#else
	GF_AddDebug(L"	- AUTOGENERATE(AddIndividualAutoLink5) : %d", dwEnd - dwStart);
#endif

#endif
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
 	POSITION pos,epos= nullptr;
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

CDataEmBc * CFormAutoMake::AddEmergency(CDataDevice * pSrcDev, CDataLocBase * pInLoc, CDataAutoLogic * pLogic)
{
	CDataEmBc * pEm;
	if (pSrcDev == nullptr || pInLoc == nullptr || pLogic == nullptr)
		return nullptr; 

	pEm = m_pRefFasSysData->GetEmergency(pInLoc->GetBuildName(), pInLoc->GetStairName()
		, pInLoc->GetFloorName());
	return pEm;
}

// 경종	시각경보	비상방송	싸이렌	밸브	자동문	로비폰	옥상문	급기댐퍼	배기댐퍼	급기휀	배기휀	자동폐쇄	창문폐쇄	비상등	소화전기동	부표시기	배연창	1차폐쇄	2차폐쇄
// 위의 내용과 일치 하는지 검사
// nLcCont = pLogic->GetOutContents();
// switch (nLcCont)
// {
// case 1:// 경종
// 
// 	break;
// case 2: //
// 	break;
// case 3: //
// 	break;
// case 4: //
// 	break;
// case 5: //
// 	break;
// case 7: //
// 	break;
// case 8: //
// 	break;
// case 9: //
// 	break;
// case 10: //
// 	break;
// case 11: //
// 	break;
// case 12: //
// 	break;
// case 13: //
// 	break;
// case 14: //
// 	break;
// case 15: //
// 	break;
// case 16: //
// 	break;
// case 19: //
// 	break;
// case 21: //
// 	break;
// case 25: //
// 	break;
// case 28: //
// 	break;
// case 29: //
// 	break;
// }
// 
// break;
int CFormAutoMake::MakeAutoSystemData()
{
	int nCnt = 0;
	CString strKey , strIDKey;
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

int CFormAutoMake::MakeSortedDeviceByLocation()
{
	CString strSql, strKey;
	YAdoDatabase * pDB = nullptr;
	int nCnt, i;
	CDataDevice * pDevice;
	CRelayTableData * pRefTable = theApp.GetRelayTableData();
	if (pRefTable == nullptr)
		return 0; 

	pDB = pRefTable->GetPrjDB();
	if (pDB == nullptr)
		return 0; 

	strSql.Format(L"SELECT  A.* ,B.BD_NAME , C.BTYPE_NAME , D.STAIR_NAME , E.FL_NAME , F.RM_NAME "
		L" FROM     TB_RELAY_LIST A INNER JOIN "
		L" TB_LOC_BUILDING B ON A.BD_ID = B.BD_ID INNER JOIN "
		L" TB_LOC_BTYPE C ON A.BD_ID = C.BD_ID AND A.BTYPE_ID = C.BTYPE_ID INNER JOIN "
		L" TB_LOC_STAIR D ON A.BD_ID = D.BD_ID AND A.BTYPE_ID = D.BTYPE_ID AND "
		L" A.STAIR_ID = D.STAIR_ID INNER JOIN "
		L" TB_LOC_FLOOR E ON A.BD_ID = E.BD_ID AND A.BTYPE_ID = E.BTYPE_ID AND "
		L" A.STAIR_ID = E.STAIR_ID AND A.FL_ID = E.FL_ID INNER JOIN "
		L" TB_LOC_ROOM F ON A.BD_ID = F.BD_ID AND A.BTYPE_ID = F.BTYPE_ID AND "
		L" A.STAIR_ID = F.STAIR_ID AND A.FL_ID = F.FL_ID AND A.RM_ID = F.RM_ID "
		L" WHERE OUTPUT_ID IN(SELECT LG_OUTTYPE_ID FROM TB_AUTO_LOGIC_V2 GROUP BY LG_OUTTYPE_ID) "
		L" AND OUTCONTENTS_ID IN(SELECT  LG_OUTCONT_ID FROM TB_AUTO_LOGIC_V2 GROUP BY  LG_OUTCONT_ID) "
		L" ORDER BY B.BD_NAME, C.BTYPE_NAME, E.FL_NAME, D.STAIR_NAME, F.RM_NAME");

	if (pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_RELAY_LIST open failed");
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (pDB->GetFieldValue(L"MAP_KEY", strKey) == FALSE)
		{
			pDB->MoveNext();
			continue;
		}

		pDevice = pRefTable->GetDevice(strKey);
		if (pDevice != nullptr)
		{
			m_ptrSortedDevice.AddTail(pDevice);
// #ifdef _DEBUG
// 			if (pDevice->GetDeviceNum() == 36)
// 				m_pDebugDevice36 = pDevice;
// 			if (pDevice->GetDeviceNum() == 83)
// 				m_pDebugDevice83 = pDevice;
// 			if (pDevice->GetDeviceNum() == 84)
// 				m_pDebugDevice84 = pDevice;
// #endif
		}

		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}


// 
// int CFormAutoMake::GenerateAutoLinkData()
// {
// 	int nIdx = 0;
// 	CString strKey;
// 	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
// 	CMapAutoSystem::iterator it;
// 	CDataSystem * pData;
// 	CDataDevice * pDevice = nullptr;
// 	CDataAutoMake * pNewAuto;
// 
// 	YAdoDatabase * pDBUtil = nullptr;
// 
// 	pDBUtil = new YAdoDatabase;
// 	pDBUtil->MSSqlInitialize(g_stConfig.szDBPass, g_stConfig.szDBUser
// 		, m_pRefFasSysData->GetPrjName(), g_stConfig.szSource);
// 
// 	if (pDBUtil->DBOpen() == FALSE)
// 	{
// 		GF_AddDebug(L"자동생성 오류 : 데이터베이스를 연결하는데 실패했습니다.");
// 		AfxMessageBox(L"데이터베이스 접속에 실패했습니다.");
// 		return 0;
// 	}
// #if _DBG_MAKE_TIME_
// 	COleDateTime dtCur;
// 	DWORD_PTR dwStart, dwEnd, dwTemp;
// 	dtCur = COleDateTime::GetCurrentTime();
// 	GF_AddDebug(L"자동생성 시작: %s", dtCur.Format(L"%H:%M:%S"));
// 	dwStart = GetTickCount();
// #endif
// 	for (it = m_MapAuto.begin(); it != m_MapAuto.end(); it++)
// 	{
// 		if (m_bStopFlag == TRUE)
// 			return -1;
// 		pNewAuto = it->second;
// 		if (pNewAuto == nullptr)
// 			continue;
// 		strKey = it->first;
// 		if (strKey == L"")
// 			continue;
// 		pData = pNewAuto->GetSysData();
// 		if (pData == nullptr)
// 			continue;
// 		
// 		if (pData->GetDataType() != SE_RELAY)
// 			continue; 
// 		nIdx++;
// #if 0
// //#if _DBG_MAKE_TIME_
// 		dwStart = GetTickCount();
// #endif
// 		//AddIndividualAutoLink2(pNewAuto , pDBUtil);
// 		AddIndividualAutoLink4(pNewAuto);
// #if 0
// //#if _DBG_MAKE_TIME_
// 		dwEnd = GetTickCount();
// 		GF_AddDebug(L"	- 자동생성(AddIndividualAutoLink) : %d", dwEnd - dwStart);
// 		dwStart = dwEnd;
// #endif
// 		AddIndividaulEmergency(pNewAuto);
// #if 0
// //#if _DBG_MAKE_TIME_
// 		dwEnd = GetTickCount();
// 		GF_AddDebug(L"	- 자동생성(AddIndividaulEmergency) : %d", dwEnd - dwStart);
// 		dwStart = dwEnd;
// #endif
// 		ArrangeAutoLink(pNewAuto);
// #if 0
// //#if _DBG_MAKE_TIME_
// 		dwEnd = GetTickCount();
// 		GF_AddDebug(L"	- 자동생성(ArrangeAutoLink) : %d", dwEnd - dwStart);
// 		dwStart = dwEnd;
// #endif
// 		SendMessage(CSWM_PROGRESS_STEP, nIdx, PROG_RESULT_STEP);
// 	}
// 	SendMessage(CSWM_PROGRESS_STEP, nIdx, PROG_RESULT_FINISH);
// #if _DBG_MAKE_TIME_
// 	dtCur = COleDateTime::GetCurrentTime();
// 	GF_AddDebug(L"자동생성 종료 : %s", dtCur.Format(L"%H:%M:%S"));
// #endif
// 	pDBUtil->DBClose();
// 	delete pDBUtil;
// 	pDBUtil = nullptr;
// 	return 1;
// }



int CFormAutoMake::MakeTempPatternData(CDataAutoMake * pSourceAuto,CDataDevice * pTargetDev)
{
	if (pSourceAuto == nullptr)
		return 0; 
	int nCnt, i , nRet =0;
	int nPtId, nFacp, nUnit, nChn, nDev;
	YAdoDatabase * pDB;
	CString strSql;
	CDataPattern * pPtn = nullptr;
	std::shared_ptr<CManagerPattern> spManager;
	pDB = m_pRefFasSysData->GetPrjDB();
	strSql.Format(L"SELECT A.PT_TYPE , A.PT_ID , A.PT_NAME , B.* "
		L" FROM TB_PATTERN_MST AS A INNER JOIN " 
		L" TB_PATTERN_ITEM AS B ON A.PT_ID = B.PT_ID "
		L" WHERE  (B.FACP_ID = %d) AND (B.UNIT_ID = %d) AND (B.CHN_ID = %d) AND (B.RLY_ID = %d) "
		L" ORDER BY B.FACP_ID, B.UNIT_ID, B.CHN_ID, B.RLY_ID, A.PT_TYPE, A.PT_ID"
		, pTargetDev->GetFacpID() , pTargetDev->GetUnitID() , pTargetDev->GetChnID(), pTargetDev->GetDeviceID()
	);

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0; 
	spManager = m_pRefFasSysData->GetPatternManager();
	if (spManager == nullptr)
		return 0; 
	nRet = 1;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (m_bStopFlag == TRUE)
		{
			nRet = 0;
			break;
		}
		nPtId = nFacp = nUnit = nChn = nDev = -1;
		if (pDB->GetFieldValue(L"PT_ID", nPtId) == FALSE)
		{
			nPtId = -1;
			pDB->MoveNext();
			continue;
		}

// 		if (pDB->GetFieldValue(L"FACP_ID", nFacp) == FALSE)
// 			nFacp = -1;
// 		if (pDB->GetFieldValue(L"UNIT_ID", nUnit) == FALSE)
// 			nUnit = -1;
// 		if (pDB->GetFieldValue(L"CHN_ID", nChn) == FALSE)
// 			nChn = -1;
// 		if (pDB->GetFieldValue(L"RLY_ID", nDev) == FALSE)
// 			nDev = -1;
// 		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
// 		{
// 			pDB->MoveNext();
// 			continue;
// 		}

		pPtn = spManager->GetPattern(nPtId);
		if (pPtn == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pSourceAuto->AddTempPattern(m_pRefFasSysData , pPtn , pTargetDev);
		pDB->MoveNext();
	}
	pDB->RSClose();
	//pList = pSourceAuto->GetPatternList();
	return nRet;
}

int CFormAutoMake::ArrangeAutoLink(CDataAutoMake * pSourceAuto)
{
	POSITION pos,savePos;
	CPtrList * pList;
	CDataAutoPattern * pPtn;
	CDataSystem * pSys; 
	pSys = pSourceAuto->GetSysData();
	if (pSys == nullptr)
		return 0; 

	if (pSys->GetDataType() != SE_RELAY)
		return 0; 

	pList = pSourceAuto->GetRefPatternList();
	if (pList == nullptr)
		return 0;
	pos = pList->GetHeadPosition();
	while (pos)
	{
		if (m_bStopFlag == TRUE)
			return -1;
		savePos = pos;
		pPtn = (CDataAutoPattern *)pList->GetNext(pos);
		if (pPtn == nullptr)
			continue; 
		if (pPtn->CheckUseDevice())
			ArrangeRelayLink(pSourceAuto, pPtn);
		else
			DeleteAutoPattern(pPtn , pList , savePos);
	}
	return 1;
}

int CFormAutoMake::ArrangeRelayLink(CDataAutoMake * pSourceAuto, CDataAutoPattern * pPtn)
{
	CDataSystem * pSys;
	CDataDevice * pSrcDev;
	pSys = pSourceAuto->GetSysData();
	if (pSys == nullptr || pSys->GetSysData() == nullptr || pSys->GetDataType() != SE_RELAY)
		return 0;

	pSrcDev = (CDataDevice *)pSys->GetSysData();

	CDataDevice * pTemp;
	CDataAutoDevice * pADev = nullptr;
	CPtrList * pList;
	POSITION posAuto;
	pList = pPtn->GetRefAutoDeviceList();
	posAuto = pList->GetHeadPosition();
	while (posAuto)
	{
		if (m_bStopFlag == TRUE)
			return -1;
		pADev = (CDataAutoDevice*)pList->GetNext(posAuto);
		if (pADev == nullptr)
			continue;
		pTemp = pADev->GetRefDevice();
		pSrcDev->DeleteLinkByID(LK_TYPE_RELEAY,pTemp->GetFacpID(), pTemp->GetUnitID(), pTemp->GetChnID(), pTemp->GetDeviceID());
	}
	return 1;
}


int CFormAutoMake::DeleteAutoPattern(CDataAutoPattern * pPtn, CPtrList * pList, POSITION removePos)
{
	if (pList == nullptr)
		return 0; 

	if (removePos == nullptr)
		return 0; 

	pList->RemoveAt(removePos);
	if (pPtn == nullptr)
	{
		delete pPtn;
		pPtn = nullptr;
	}
	return 1;
}

void CFormAutoMake::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	if (SaveAutoLink() > 0)
// 		AfxMessageBox(L"생성된 연동데이터 저장이 완료 되었습니다." , MB_OK|MB_ICONINFORMATION);
	m_pMakeThread = AfxBeginThread((AFX_THREADPROC)Thread_SaveProc,
		(LPVOID)this);
}


int CFormAutoMake::ProcessSaveAutoLink()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_BTN_MAKE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);

#ifndef ENGLISH_MODE
	if(SaveAutoLink() > 0)
		AfxMessageBox(L"생성된 연동데이터 저장이 완료 되었습니다.",MB_OK | MB_ICONINFORMATION);
#else
	if (SaveAutoLink() > 0)
		AfxMessageBox(L"The generated site logic data has been saved.", MB_OK | MB_ICONINFORMATION);
#endif

	GetDlgItem(IDC_BTN_MAKE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	return 1;
}

LRESULT CFormAutoMake::OnMakeProgress(WPARAM wp, LPARAM lp)
{
	CString str;
	int nP = 0;
	switch (lp)
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
		SetTimer(TM_PROG_TIMER, 1000, nullptr);
		m_nCurTimerIdx = wp;
		nP = (int)(((float)wp / (float)m_nAllCnt) * 100);
#ifndef ENGLISH_MODE
		str.Format(L"[%d]%% 작업중(%d/%d) : 연동입력 개수(%d)", nP, wp, m_nAllCnt, m_nAllCnt-m_nTimePrgCnt);
#else
		str.Format(L"[%d]% in progress ([%d]/[%d]) interlocked input count ([%d])", nP, wp, m_nAllCnt, m_nAllCnt - m_nTimePrgCnt);
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
		str.Format(L"[%d]%% 작업중(%d/%d) :연동입력 개수(%d)", nP,wp , m_nAllCnt, m_nAllCnt - m_nTimePrgCnt);
#else
		str.Format(L"[%d]% in progress ([%d]/[%d]) interlocked input count ([%d])", nP, wp, m_nAllCnt, m_nAllCnt - m_nTimePrgCnt);
#endif
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlProg.SetPos(nP);
		break;
	case PROG_RESULT_FINISH:
#ifndef ENGLISH_MODE
		str.Format(L"%d개의 연동데이터를 생성했습니다.", m_nAllCnt-m_nTimePrgCnt);
#else
		str.Format(L"You have created [%d] piece(s) of site logic data.", m_nAllCnt - m_nTimePrgCnt);
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
	SendMessage(CSWM_PROGRESS_STEP, 0, PROG_RESULT_CANCEL);
}

int CFormAutoMake::SaveAutoLink()
{
	m_nAllCnt = g_stConfig.dwTimeOut;
	m_nAllCnt += g_stConfig.dwTimeOut;
	m_nAllCnt += m_MapIDAuto.size();

	if (m_pRefFasSysData == nullptr)
	{
		SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_ERROR);
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트가 닫혀있는 상태입니다. 프로젝트를 열고 다시 시도해 주십시오.");
		AfxMessageBox(L"프로젝트가 닫혀있는 상태입니다. 프로젝트를 열고 다시 시도해주십시오.");
#else
		GF_AddLog(L"The project is closed. Please open the project and try again.");
		AfxMessageBox(L"The project is closed. Please open the project and try again.");
#endif
		return 0; 
	}
	int nIdx = 0 , nRet =0;
	int n1, n2;
	int nProgOffset =0;
	BOOL bCross = FALSE;
	CString strKey , strSql;
	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
	CMapAutoSystem::iterator it;
	CDataSystem * pData;
	CDataDevice * pDevice = nullptr , *pOriginalDev ;
	CDataAutoMake * pNewAuto;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();

	if(pDb->DropCleanBuffer() == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"데이터베이스의 메모리 버퍼를 초기화하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to initialize the memory buffer of the database.");
#endif
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
	strSql.Format(L"DELETE FROM TB_LINK_RELAY WHERE LG_TYPE=%d" , LOGIC_ALL_AUTO);
	if (pDb->ExecuteSql(strSql) == FALSE)
	{
		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
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
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
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
		if(n1 == INTYPE_CROSSA || n1 == INTYPE_CROSSB
			|| n1 == INTYPE_CROSS16_A || n1 == INTYPE_CROSS17_B
			|| n1 == INTYPE_CROSS18_A || n1 == INTYPE_CROSS19_B
			)
			bCross = TRUE;
		nRet = SaveIndividualAutoLink(pDb , pNewAuto, pOriginalDev , bCross);
		if (nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}

		nRet = SaveIndividualEmergency(pDb, pNewAuto, pOriginalDev);
		if (nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}
		nRet = SaveIndividualPattern(pDb, pNewAuto, pOriginalDev);
		if (nRet <= 0)
		{
			SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
			pDb->RollbackTransaction();
			return 0;
		}

		nIdx++;
		nProgOffset += 1;
		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_STEP);
		//SendMessage(CSWM_PROGRESS_STEP, nIdx, PROG_RESULT_STEP);
	}
	//SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_TIMER_END);
	SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_TIMER_START);
	if(m_pRefFasSysData->TempFunc_CheckIndex() <= 0)
	{
		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
		pDb->RollbackTransaction();
		//m_pRefFasSysData->SendProgStep(this,PROG_RESULT_ERROR,0,0);
		return 0;
	}
	nProgOffset += g_stConfig.dwTimeOut;
	SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_FINISH);
	pDb->CommitTransaction();
	//RemoveAllData();
	return 1;
}


int CFormAutoMake::SaveIndividualAutoLink(YAdoDatabase * pDb,CDataAutoMake * pSourceAuto, CDataDevice *pOriginalDev, BOOL bCross)
{
	CString strSql=L"",strtemp=L"";
	POSITION pos;
	CPtrList * pList;
	int n1, n2,nCnt=0;
	BOOL bFirst = FALSE;
	CDataLinked * pLnk , *pNewLnk= nullptr;
	CDataDevice *pNewDev , * pTempDev;
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
					n1= n2 = 0; 
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
					if(n1 == OUTCONT_VALVE && n2 == OUTTYPE_PREACTION  && IsSameRoom(pOriginalDev,pTempDev))
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
			nCnt ++;
			if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				pDb->ExecuteSql(strSql);
				nCnt = 0; 
			}
		}
	}

	if(nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	return 1;
}

int CFormAutoMake::SaveIndividualEmergency(YAdoDatabase * pDb, CDataAutoMake * pSourceAuto, CDataDevice *pOriginalDev)
{
	CString strSql = L"",strtemp = L"";
	POSITION pos;
	CPtrList * pList;
	CDataLinked * pLnk , *pNewLnk= nullptr;
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
			nCnt ++;
			if(strSql.GetLength() > 65535)
			{
				pDb->ExecuteSql(strSql);
				nCnt = 0;
			}
		}
	}

	if(nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	return 1;
}

int CFormAutoMake::SaveIndividualPattern(YAdoDatabase * pDb, CDataAutoMake * pSourceAuto, CDataDevice *pOriginalDev)
{
	CString strSql = L"",strtemp = L"";
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
		if (pOriginalDev->FindLinkByID(LK_TYPE_PATTERN, nID,0,0,0) == nullptr)
		{
			pNewLnk = new CDataLinked;
			pNewLnk->SetLinkData(pAp->GetPattern() , LOGIC_ALL_AUTO , D_NUM_AUTO_PTN_LOGIC_ID
				,pOriginalDev);
			
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
			nCnt ++;
			if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				pDb->ExecuteSql(strSql);
				nCnt = 0;
			}
			//pDb->ExecuteSql(strSql);
		}
	}

	if(nCnt > 0)
	{
		pDb->ExecuteSql(strSql);
		nCnt = 0;
	}
	return 1;
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
	int nF, nU, nC , nD;
	nF = nU = nC = nD =  -1;
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
			&&hUnit[nF][nU] == nullptr)
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


void CFormAutoMake::OnTvnSelchangedDeviceTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMTreeView->itemNew.hItem == nullptr)
		return; 
	DisplayList(pNMTreeView->itemNew.hItem);

}


int CFormAutoMake::DisplayList(HTREEITEM hItem)
{
	CDataAutoMake * pNewAuto;
	CDataSystem * pData;
	CDataDevice * pDev , *pTemp;
	CDataPattern * pPtn;
	CDataLinked * pDlk;
	CDataLocBase * pLoc;
	CDataEquip * pIn, *pName, *pCont, *pOut;
	POSITION pos;
#ifndef ENGLISH_MODE
	CString strType = L"패턴" , str;
#else
	CString strType = L"PATTERN", str;
#endif
	CString strIn, strOut, strCont, strName,strEqName,strAddr;
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
		else if(pDlk->GetLinkType() == LK_TYPE_PATTERN)
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



int CFormAutoMake::GenerateAutoLinkData2()
{
	BOOL bRet = FALSE;
	int nIdx = 0,nCnt,i,nValue,nAddCnt = 0;
	int nExceptionCnt=0,nRelayLinkCnt = 0;
	CString strKey,strSql;
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
	GF_AddDebug(L"   - SP_GENERATE_LINK 시작: %d",dwEnd- dwTemp);
#else
	GF_AddDebug(L"   - SP_GENERATE_LINK started: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif

	bRet = pDBUtil->ExecuteSqlTimeOut(L"SP_GENERATE_LINK",g_stConfig.dwTimeOut);
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
	GF_AddDebug(L"   - SP_GENERATE_LINK 종료: %d",dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_GENERATE_LINK ended: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif
#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - SP_GENERATE_PTN_BY_SOURCE 시작: %d",dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_GENERATE_PTN_BY_SOURCE started: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif
	nProgOffset = g_stConfig.dwTimeOut;
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_END);
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_START);
	bRet = pDBUtil->ExecuteSqlTimeOut(L"SP_GENERATE_PTN_BY_SOURCE",g_stConfig.dwTimeOut);
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
	GF_AddDebug(L"   - SP_GENERATE_PTN_BY_SOURCE 종료: %d",dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_GENERATE_PTN_BY_SOURCE ended: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif

#if _DBG_MAKE_TIME_
	dwEnd = GetTickCount();

#ifndef ENGLISH_MODE
	GF_AddDebug(L"   - SP_DELETE_TEMPLINK_PTNITEM 시작: %d",dwEnd - dwTemp);
#else
	GF_AddDebug(L"   - SP_DELETE_TEMPLINK_PTNITEM started: %d", dwEnd - dwTemp);
#endif

	dwTemp = dwEnd;
#endif

	nProgOffset += g_stConfig.dwTimeOut;
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_END);
	SendMessage(CSWM_PROGRESS_STEP, nProgOffset, PROG_RESULT_TIMER_START);
	bRet = pDBUtil->ExecuteSqlTimeOut(L"SP_DELETE_TEMPLINK_PTNITEM",g_stConfig.dwTimeOut);
	if(bRet == FALSE)
	{
		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"자동생성 실패 : SP_DELETE_TEMPLINK_PTNITEM을 실행하는데 실패했습니다.\r\n%s"
			,pDBUtil->GetLastErrorString());
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
	GF_AddDebug(L"   - SP_DELETE_TEMPLINK_PTNITEM 종료: %d",dwEnd - dwTemp);
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
	GF_AddDebug(L"   - DataMake 시작: %d",dwEnd - dwTemp);
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

				if(nInType == INTYPE_CROSSA || nInType == INTYPE_CROSSB
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
				if(nRelayLinkCnt > 20)
				{
					nExceptionCnt ++;
#ifndef ENGLISH_MODE
					GF_AddLog(L"** 오류 예상됨 : %s[%s]의 출력 개수가 %d개 입니다.",pSrcDev->GetInputFullName(),pSrcDev->GetDevAddress(),nRelayLinkCnt);
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
				if(strSB == strTB && strST == strTT && strSS == strTS && strSF == strTF && strSR == strTR)
					bFirst = TRUE;
				else
					bFirst = FALSE;
			}
			pSrcDev->AddLink(bFirst, pLink);
			nRelayLinkCnt ++; // 연동 출력 개수만 따로 카운트한다.
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
	GF_AddDebug(L"   - DataMake 종료: %d",dwEnd - dwTemp);
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

	if(nExceptionCnt > 0)
	{
		//GF_AddLog(L"** 오류 예상됨 : %s[%s]의 출력 개수가 %d개 입니다.",pSrcDev->GetInputFullName(),pSrcDev->GetDevAddress(),nRelayLinkCnt);
		CString strMsg;
#ifndef ENGLISH_MODE
		strMsg.Format(L"연동 출력이 20개가 넘는 회로가 %d개 있습니다.\n로그 창을 확인하시기 바랍니다.",nExceptionCnt);
#else
		strMsg.Format(L"The number of circuits with 20 interlock outputs is %d.\nPlease check the log window.", nExceptionCnt);
#endif
		AfxMessageBox(strMsg,MB_OK|MB_ICONERROR);
	}
	return 1;
}

struct ST_DEVTEMP
{
	int		m_nRelayIndex;
	CString m_strKey; // Map 키 , GF_GetSysDataKey의 리턴값(수신기.유닛.계통.회로.타입)

	int m_nFacpID;
	int m_nUnitID;
	int m_nChannelID;
	int m_nDeviceID;

	int m_nInType;
	int m_nOutType;
	int m_nEqName;
	int m_nOutCont;

	int m_nBd;
	int m_nBtpe;
	int m_nStair;
	int m_nFloor;
	int m_nRoom;

	int m_nLevel; 

	CString					m_strEqAddIndex; // 설비 번호 , 방에 감지기1,감지기2 에서 숫자 부분
	
	CPtrList				m_ptrLink;
	CString m_strBd,m_strBtype,m_strStair,m_strFloor,m_strRoom;
	void SetDeviceData(int nFid,int nUid,int nCid,int nRid,int nRnum,int nIn,int nOut,int nEqName,int nOutCont
		,int nBd,int nBtype,int nStair,int nFloor,int nRoom,int nFNum
		,CString strBd,CString strBtype,CString strStair,CString strFloor,CString strRoom
		,CString strMapKey,CString strEqAddIdx
	)
	{
		m_nFacpID = nFid;
		m_nUnitID = nUid;
		m_nChannelID = nCid;
		m_nDeviceID = nRid;
		m_nRelayIndex = nRnum;

		m_nInType = nIn;
		m_nOutType = nOut;
		m_nEqName = nEqName;
		m_nOutCont = nOutCont;

		m_nBd = nBd;
		m_nBtpe = nBtype;
		m_nStair = nStair;
		m_nFloor = nFloor;
		m_nRoom = nRoom;

		m_nLevel = nFNum;

		m_strBd = strBd;
		m_strBtype = strBtype;
		m_strStair = strStair;
		m_strFloor = strFloor;
		m_strRoom = strRoom;

		m_strKey = strMapKey;
		m_strEqAddIndex = strEqAddIdx;
	}

};

int CFormAutoMake::GenerateAutoLinkData3()
{
// 	BOOL bRet = FALSE;
// 	int nIdx = 0,nCnt,i,nValue,nAddCnt = 0;
// 	CString strKey,strSql;
// 	ST_DEVTEMP * pTgtDev = nullptr,* pSrcDev = nullptr;
// 	//CDataDevice * pTgtDev = nullptr,* pSrcDev = nullptr;
// 
// 	CDataLinked *pLk;
// 	CString strAddIdx;
// 	CString strSB,strST,strSS,strSF,strSR,strTB,strTT,strTS,strTF,strTR;
// 	int nUID = 0,nFID,nCID,nRID,nRNum;
// 	int nBd,nBtype , nStair,nFl,nRm , nLevel , nLgID;
// 	int nProgOffset = 0;
// 	int nInType,nOutType,nEqName,nCont;
// 	BOOL bCross,bFirst;
// 
// 	CPtrList ptrTgtList,ptrScrList;
// 	YAdoDatabase * pDBUtil = nullptr;
// 	POSITION tPos,sPos;
// 
// 
// 	BYTE btAllFloor,btEmergency,btOutput,btPluseNFloor;
// 	BYTE btMatchBuild,btMatchBType,btMatchStair,btMatchFloor,btMatchRoom;
// 	BYTE btUnderBasic,btUnderClassBuild,btUnderClassBType,btUnderClassStair,btUnder1F,btUnderB1F;
// 	BYTE btParkingBasic,btParkingClassBuild,btParkingClassStair,btParking1F,btParkingB1F;
// 
// 
// 	pDBUtil = new YAdoDatabase;
// 	pDBUtil->MSSqlInitialize(g_stConfig.szDBPass,g_stConfig.szDBUser
// 		,m_pRefFasSysData->GetPrjName(),g_stConfig.szSource);
// 
// 	if(pDBUtil->DBOpen() == FALSE)
// 	{
// 		GF_AddDebug(L"자동생성 오류 : 데이터베이스를 연결하는데 실패했습니다.");
// 		AfxMessageBox(L"데이터베이스 접속에 실패했습니다.");
// 		return 0;
// 	}
// 
// 	TRACE("1. ======> %d\n",GetTickCount());
// 
// 
// #if _DBG_MAKE_TIME_
// 	COleDateTime dtCur;
// 	DWORD_PTR dwStart,dwEnd,dwTemp;
// 	dtCur = COleDateTime::GetCurrentTime();
// 	GF_AddDebug(L"자동생성 시작: %s",dtCur.Format(L"%H:%M:%S"));
// 	dwStart = GetTickCount();
// #endif
// 	SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_STEP);
// 
// 	if(pDBUtil->ReduceDatabase(m_pRefFasSysData->GetPrjName()) == FALSE)
// 	{
// 		SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_ERROR);
// 		GF_AddLog(L"데이터베이스를 초기화하는데 실패했습니다.");
// 		AfxMessageBox(L"데이터베이스를 초기화하는데 실패했습니다.");
// 		return 0;
// 	}
// 
// 	bCross = bFirst = FALSE;
// 	nInType = nOutType = nEqName = nCont = -1;
// 	SendMessage(CSWM_PROGRESS_STEP,nProgOffset,PROG_RESULT_TIMER_START);
// 
// 	strSql.Format(
// 		L" SELECT A.* , B.BD_NAME , C.BTYPE_NAME , D.STAIR_NAME , E.FL_NAME , F.RM_NAME , E.FL_NUM "
// 		L" FROM "
// 		L" (SELECT Z.*FROM TB_RELAY_LIST Z "
// 		L" 	WHERE EXISTS "
// 		L" 	(SELECT * FROM TB_AUTO_LOGIC_V2 Y WHERE Y.LG_OUTTYPE_ID = Z.OUTPUT_ID AND Y.LG_OUTCONT_ID = Z.OUTCONTENTS_ID)) A "
// 		L" ,TB_LOC_BUILDING B,TB_LOC_BTYPE C,TB_LOC_STAIR D,TB_LOC_FLOOR E,TB_LOC_ROOM  F "
// 		L" WHERE A.BD_ID = B.BD_ID "
// 		L" AND A.BD_ID = C.BD_ID AND A.BTYPE_ID = C.BTYPE_ID "
// 		L" AND A.BD_ID = D.BD_ID AND A.BTYPE_ID = D.BTYPE_ID AND A.STAIR_ID = D.STAIR_ID "
// 		L" AND A.BD_ID = E.BD_ID AND A.BTYPE_ID = E.BTYPE_ID AND A.STAIR_ID = E.STAIR_ID AND A.FL_ID = E.FL_ID "
// 		L" AND A.BD_ID = F.BD_ID AND A.BTYPE_ID = F.BTYPE_ID AND A.STAIR_ID = F.STAIR_ID AND A.FL_ID = F.FL_ID AND A.RM_ID = F.RM_ID "
// 	);
// 	bRet = pDBUtil->OpenQuery(strSql);
// 	if(bRet == FALSE)
// 	{
// 		AfxMessageBox(L"Target 데이터 가져오기 실패");
// 		return 0; 
// 	}
// 
// 	nCnt = pDBUtil->GetRecordCount();
// 	for(i = 0; i < nCnt; i++)
// 	{
//  		pDBUtil->GetFieldValue(L"FACP_ID",nValue);
//  		nFID = nValue;
//  		pDBUtil->GetFieldValue(L"UNIT_ID",nValue);
//  		nUID = nValue;
//  		pDBUtil->GetFieldValue(L"CHN_ID",nValue);
//  		nCID = nValue;
//  		pDBUtil->GetFieldValue(L"RLY_ID",nValue);
//  		nRID = nValue;
//  		pDBUtil->GetFieldValue(L"RLY_NUM",nValue);
//  		nRNum = nValue;
//  
//  		pDBUtil->GetFieldValue(L"INPUT_ID",nValue);
//  		nInType = nValue;
//  		pDBUtil->GetFieldValue(L"OUTPUT_ID",nValue);
//  		nOutType = nValue;
//  		pDBUtil->GetFieldValue(L"OUTCONTENTS_ID",nValue);
//  		nCont = nValue;
//  		pDBUtil->GetFieldValue(L"EQ_ID",nValue);
//  		nEqName = nValue;
//  
//  		pDBUtil->GetFieldValue(L"BD_ID",nValue);
//  		nBd = nValue;
//  		pDBUtil->GetFieldValue(L"BTYPE_ID",nValue);
//  		nBtype = nValue;
//  		pDBUtil->GetFieldValue(L"STAIR_ID",nValue);
//  		nStair = nValue;
//  		pDBUtil->GetFieldValue(L"FL_ID",nValue);
//  		nFl = nValue;
//  		pDBUtil->GetFieldValue(L"RM_ID",nValue);
//  		nRm = nValue;
// 		pDBUtil->GetFieldValue(L"FL_NUM",nValue);
// 		nLevel = nValue;
//  
//  		pDBUtil->GetFieldValue(L"BD_NAME",strTB);
//  		pDBUtil->GetFieldValue(L"BTYPE_NAME",strTT);
//  		pDBUtil->GetFieldValue(L"STAIR_NAME",strTS);
//  		pDBUtil->GetFieldValue(L"FL_NAME",strTF);
//  		pDBUtil->GetFieldValue(L"RM_NAME",strTR);
// 		pDBUtil->GetFieldValue(L"MAP_KEY",strKey);
// 		pDBUtil->GetFieldValue(L"EQ_ADD_IDX",strAddIdx);
// 
//  		pTgtDev = new ST_DEVTEMP;
//  		pTgtDev->SetDeviceData(nFID,nUID,nCID,nRID,nRNum,nInType,nOutType,nEqName,nCont
//  			,nBd,nBtype,nStair,nFl,nRm,nLevel
//  			,strTB,strTT,strTS,strTF,strTR
//  			,strKey,strAddIdx
//  		);
// 		//pTgtDev = m_pRefFasSysData->GetDevice(strKey);
// 		ptrTgtList.AddTail(pTgtDev);
// 		pDBUtil->MoveNext();
// 	}
// 
// 	pDBUtil->RSClose();
// 	dwTemp = dwStart;
// 	dwEnd = GetTickCount();
// 	TRACE("2. ======> Target : %d\n",dwEnd - dwTemp);
// 	dwTemp = dwEnd;
// 
// 	strSql.Format(
// 		L" SELECT A.* , B.BD_NAME , C.BTYPE_NAME , D.STAIR_NAME , E.FL_NAME , F.RM_NAME, E.FL_NUM "
// 		L" FROM "
// 		L" (SELECT Z.*FROM TB_RELAY_LIST Z "
// 		L" 	WHERE EXISTS "
// 		L" 	(SELECT * FROM TB_AUTO_LOGIC_V2 Y WHERE Y.LG_INTYPE_ID = Z.INPUT_ID AND Y.LG_EQNAME_ID = Z.EQ_ID)) A "
// 		L" ,TB_LOC_BUILDING B,TB_LOC_BTYPE C,TB_LOC_STAIR D,TB_LOC_FLOOR E,TB_LOC_ROOM  F "
// 		L" WHERE A.BD_ID = B.BD_ID "
// 		L" AND A.BD_ID = C.BD_ID AND A.BTYPE_ID = C.BTYPE_ID "
// 		L" AND A.BD_ID = D.BD_ID AND A.BTYPE_ID = D.BTYPE_ID AND A.STAIR_ID = D.STAIR_ID "
// 		L" AND A.BD_ID = E.BD_ID AND A.BTYPE_ID = E.BTYPE_ID AND A.STAIR_ID = E.STAIR_ID AND A.FL_ID = E.FL_ID "
// 		L" AND A.BD_ID = F.BD_ID AND A.BTYPE_ID = F.BTYPE_ID AND A.STAIR_ID = F.STAIR_ID AND A.FL_ID = F.FL_ID AND A.RM_ID = F.RM_ID "
// 	);
// 	bRet = pDBUtil->OpenQuery(strSql);
// 	if(bRet == FALSE)
// 	{
// 		AfxMessageBox(L"Target 데이터 가져오기 실패");
// 		return 0;
// 	}
// 
// 
// 	nCnt = pDBUtil->GetRecordCount();
// 	for(i = 0; i < nCnt; i++)
// 	{
//  		pDBUtil->GetFieldValue(L"FACP_ID",nValue);
//  		nFID = nValue;
//  		pDBUtil->GetFieldValue(L"UNIT_ID",nValue);
//  		nUID = nValue;
//  		pDBUtil->GetFieldValue(L"CHN_ID",nValue);
//  		nCID = nValue;
//  		pDBUtil->GetFieldValue(L"RLY_ID",nValue);
//  		nRID = nValue;
//  		pDBUtil->GetFieldValue(L"RLY_NUM",nValue);
//  		nRNum = nValue;
//  
//  		pDBUtil->GetFieldValue(L"INPUT_ID",nValue);
//  		nInType = nValue;
//  		pDBUtil->GetFieldValue(L"OUTPUT_ID",nValue);
//  		nOutType = nValue;
//  		pDBUtil->GetFieldValue(L"OUTCONTENTS_ID",nValue);
//  		nCont = nValue;
//  		pDBUtil->GetFieldValue(L"EQ_ID",nValue);
//  		nEqName = nValue;
//  
//  		pDBUtil->GetFieldValue(L"BD_ID",nValue);
//  		nBd = nValue;
//  		pDBUtil->GetFieldValue(L"BTYPE_ID",nValue);
//  		nBtype = nValue;
//  		pDBUtil->GetFieldValue(L"STAIR_ID",nValue);
//  		nStair = nValue;
//  		pDBUtil->GetFieldValue(L"FL_ID",nValue);
//  		nFl = nValue;
//  		pDBUtil->GetFieldValue(L"RM_ID",nValue);
//  		nRm = nValue;
// 		pDBUtil->GetFieldValue(L"FL_NUM",nValue);
// 		nLevel = nValue;
// 
//  		pDBUtil->GetFieldValue(L"BD_NAME",strTB);
//  		pDBUtil->GetFieldValue(L"BTYPE_NAME",strTT);
//  		pDBUtil->GetFieldValue(L"STAIR_NAME",strTS);
//  		pDBUtil->GetFieldValue(L"FL_NAME",strTF);
//  		pDBUtil->GetFieldValue(L"RM_NAME",strTR);
// 		pDBUtil->GetFieldValue(L"MAP_KEY",strKey);
// 		pDBUtil->GetFieldValue(L"EQ_ADD_IDX",strAddIdx);
// 
//  		pTgtDev = new ST_DEVTEMP;
//  		pTgtDev->SetDeviceData(nFID,nUID,nCID,nRID,nRNum,nInType,nOutType,nEqName,nCont
//  			,nBd,nBtype,nStair,nFl,nRm,nLevel
//  			,strTB,strTT,strTS,strTF,strTR
//  			,strKey,strAddIdx
//  		);
// 		//pTgtDev = m_pRefFasSysData->GetDevice(strKey);
// 		ptrScrList.AddTail(pTgtDev);
// 		pDBUtil->MoveNext();
// 	}
// 
// 	pDBUtil->RSClose();
// 
// 	dwEnd = GetTickCount();
// 	TRACE("3. ======> Source : %d\n",dwEnd - dwTemp);
// 	dwTemp = dwEnd;
// 
// 	strSql.Format(
// 		L" SELECT * FROM TB_AUTO_LOGIC_V2 ORDER BY LG_ID "
// 	);
// 	bRet = pDBUtil->OpenQuery(strSql);
// 	if(bRet == FALSE)
// 	{
// 		AfxMessageBox(L"Target 데이터 가져오기 실패");
// 		return 0;
// 	}
// 
// 	int nAllCnt,nSrcCnt , nTgtCnt,nTemp; 
// 	float fRatio = 0;
// 	nCnt = pDBUtil->GetRecordCount();
// 	nSrcCnt = ptrScrList.GetCount();
// 	nTgtCnt = ptrTgtList.GetCount();
// 	nAllCnt = nCnt * nSrcCnt;
// 	for(i = 0; i < nCnt; i++)
// 	{
// 		pDBUtil->GetFieldValue(L"LG_ID",nLgID);
// 		pDBUtil->GetFieldValue(L"LG_INTYPE_ID",nInType);
// 		pDBUtil->GetFieldValue(L"LG_OUTTYPE_ID",nOutType);
// 		pDBUtil->GetFieldValue(L"LG_EQNAME_ID",nEqName);
// 		pDBUtil->GetFieldValue(L"LG_OUTCONT_ID",nCont);
// 
// 		pDBUtil->GetFieldValue(L"LG_USE_EMER_MAKE",btEmergency);
// 		pDBUtil->GetFieldValue(L"LG_USE_ALL_OUTPUT",btAllFloor);
// 		pDBUtil->GetFieldValue(L"LG_USE_OUTPUT",btOutput);
// 		pDBUtil->GetFieldValue(L"LG_USE_UPPER_FLOOR",btPluseNFloor);
// 
// 		pDBUtil->GetFieldValue(L"LG_USE_LOC_BUILD_MATCH",btMatchBuild);
// 		pDBUtil->GetFieldValue(L"LG_USE_LOC_BTYPE_MATCH",btMatchBType);
// 		pDBUtil->GetFieldValue(L"LG_USE_LOC_STAIR_MATCH",btMatchStair);
// 		pDBUtil->GetFieldValue(L"LG_USE_LOC_FLOOR_MATCH",btMatchFloor);
// 		pDBUtil->GetFieldValue(L"LG_USE_LOC_ROOM_MATCH",btMatchRoom);
// 
// 
// 		pDBUtil->GetFieldValue(L"LG_USE_UNDER_BASIC",btUnderBasic);
// 		pDBUtil->GetFieldValue(L"LG_USE_UNDER_BUILD_CLASSIFY",btUnderClassBuild);
// 		pDBUtil->GetFieldValue(L"LG_USE_UNDER_BTYPE_CLASSIFY",btUnderClassBType);
// 		pDBUtil->GetFieldValue(L"LG_USE_UNDER_STAIR_CLASSIFY",btUnderClassStair);
// 		pDBUtil->GetFieldValue(L"LG_USE_UNDER_GROUND_FLOOR",btUnder1F);
// 		pDBUtil->GetFieldValue(L"LG_USE_UNDER_B1_FLOOR",btUnderB1F);
// 
// 		pDBUtil->GetFieldValue(L"LG_USE_PARKING_BASIC",btParkingBasic);
// 		pDBUtil->MoveNext();
// 
// 
// 		nIdx = 0; 
// 		sPos = ptrScrList.GetHeadPosition();
// 		while(sPos)
// 		{
// 			nTemp = i * nSrcCnt + nIdx;
// 			fRatio = (float)nTemp / (float)nAllCnt*100;
// 			//TRACE(L"Process : %d/%d(%.1f%%)\n",nTemp,nAllCnt,fRatio);
// 			nIdx ++;
// 			pSrcDev = (ST_DEVTEMP *)ptrScrList.GetNext(sPos);
// 			if(pSrcDev == nullptr)
// 				continue;
// 			if(pSrcDev->m_nInType != nInType || pSrcDev->m_nEqName != nEqName)
// 				continue;
// 			tPos = ptrTgtList.GetHeadPosition();
// 			while(tPos)
// 			{
// 				pTgtDev = (ST_DEVTEMP *)ptrTgtList.GetNext(tPos);
// 				if(pTgtDev == nullptr)
// 					continue;
// 				if(pTgtDev->m_nOutType != nOutType || pTgtDev->m_nOutCont != nCont)
// 					continue;
// 
// 				if(btAllFloor)
// 				{
// 					//TRUE;
// 					pLk = new CDataLinked;
// 					pLk->SetLinkData(
// 						pTgtDev->m_nFacpID,pTgtDev->m_nUnitID,pTgtDev->m_nChannelID,pTgtDev->m_nDeviceID
// 						,LK_TYPE_RELEAY,LOGIC_ALL_AUTO,nLgID
// 						,pSrcDev->m_nFacpID,pSrcDev->m_nUnitID,pSrcDev->m_nChannelID,pSrcDev->m_nDeviceID
// 					);
// 					pSrcDev->m_ptrLink.AddTail(pLk);
// 					continue; 
// 				}
// 
// 				if(btUnderBasic == 0)
// 				{
// 					if(btMatchBuild)
// 					{
// 						if(pTgtDev->m_strBd.CompareNoCase(pSrcDev->m_strBd) != 0)
// 							continue;
// 					}
// 
// 					if(btMatchBType)
// 					{
// 						// 건물
// 						if(pTgtDev->m_strBtype.CompareNoCase(pSrcDev->m_strBtype) != 0)
// 							continue;
// 					}
// 
// 					if(btMatchStair)
// 					{
// 						// 건물
// 						if(pTgtDev->m_strStair.CompareNoCase(pSrcDev->m_strStair) != 0)
// 							continue;
// 					}
// 
// 					if(btMatchFloor)
// 					{
// 						// 건물
// 						if(btPluseNFloor > 0)
// 						{
// 							if(pSrcDev->m_nLevel + btPluseNFloor >= pTgtDev->m_nLevel
// 								&& pSrcDev->m_nLevel <= pTgtDev->m_nLevel)
// 								continue;
// 
// 						}
// 						else
// 						{
// 							if(pSrcDev->m_nLevel != pTgtDev->m_nLevel)
// 								continue;
// 						}
// 					}
// 
// 					if(btMatchRoom)
// 					{
// 						// 건물
// 						if(pTgtDev->m_nRoom != pSrcDev->m_nRoom)
// 							continue;
// 					}
// 
// 					// Add;
// 					pLk = new CDataLinked;
// 					pLk->SetLinkData(
// 						pTgtDev->m_nFacpID,pTgtDev->m_nUnitID,pTgtDev->m_nChannelID,pTgtDev->m_nDeviceID
// 						,LK_TYPE_RELEAY,LOGIC_ALL_AUTO,nLgID
// 						,pSrcDev->m_nFacpID,pSrcDev->m_nUnitID,pSrcDev->m_nChannelID,pSrcDev->m_nDeviceID
// 					);
// 					pSrcDev->m_ptrLink.AddTail(pLk);
// 					continue; 
// 				}
//  				else //if(btUnderBasic == 1)
//  				{
//  					if(pSrcDev->m_nLevel < -1) // 지하로직 사용, 발화점이 지하 2층 이하
//  					{
//  						if(btParkingBasic)
//  						{
//  							if(pSrcDev->m_strBd.CompareNoCase(L"주차장") == 0
//  								|| pSrcDev->m_strBtype.CompareNoCase(L"주차장") == 0
//  								|| pTgtDev->m_strBd.CompareNoCase(L"주차장") == 0
//  								|| pTgtDev->m_strBtype.CompareNoCase(L"주차장") == 0
//  								)
//  							{
//  								if(pTgtDev->m_nLevel > -1)
//  									continue; 
//  							}
//  							else
//  							{
//  								if(pTgtDev->m_nLevel > -1)
//  									continue; 
//  
//  								if(btUnderClassBuild)
//  								{
//  									if(pSrcDev->m_strBd.CompareNoCase(pTgtDev->m_strBd) != 0)
//  										continue; 
//  								}
//  
//  								if(btUnderClassBType)
//  								{
//  									if(pSrcDev->m_strBtype.CompareNoCase(pTgtDev->m_strBtype) != 0)
//  										continue;
//  								}
//  
//  								if(btUnderClassStair)
//  								{
//  									if(pSrcDev->m_strStair.CompareNoCase(pTgtDev->m_strStair) != 0)
//  										continue;
//  								}
//  							}
//  						}
//  						else // 지하 로직 사용 , 주차장로직 사용안함
//  						{
//  							if(pTgtDev->m_nLevel > -1)
//  								continue;
//  
//  							if(btUnderClassBuild)
//  							{
//  								if(pSrcDev->m_strBd.CompareNoCase(pTgtDev->m_strBd) != 0)
//  									continue;
//  							}
//  
//  							if(btUnderClassBType)
//  							{
//  								if(pSrcDev->m_strBtype.CompareNoCase(pTgtDev->m_strBtype) != 0)
//  									continue;
//  							}
//  
//  							if(btUnderClassStair)
//  							{
//  								if(pSrcDev->m_strStair.CompareNoCase(pTgtDev->m_strStair) != 0)
//  									continue;
//  							}
//  						}
// 						// Add
// 						pLk = new CDataLinked;
// 						pLk->SetLinkData(
// 							pTgtDev->m_nFacpID,pTgtDev->m_nUnitID,pTgtDev->m_nChannelID,pTgtDev->m_nDeviceID
// 							,LK_TYPE_RELEAY,LOGIC_ALL_AUTO,nLgID
// 							,pSrcDev->m_nFacpID,pSrcDev->m_nUnitID,pSrcDev->m_nChannelID,pSrcDev->m_nDeviceID
// 						);
// 						pSrcDev->m_ptrLink.AddTail(pLk);
// 						continue; 
//  					}
//  					else if(pSrcDev->m_nLevel == -1) // 지하로직 사용 , 발화점이 지하1층
//  					{
//  						if(btParkingBasic)  // 지하1층일 때 지하로직 사용 , 주차장 로직 사용 
//  						{
//  							if(pSrcDev->m_strBd.CompareNoCase(L"주차장") == 0
//  								|| pSrcDev->m_strBtype.CompareNoCase(L"주차장") == 0
//  								|| pTgtDev->m_strBd.CompareNoCase(L"주차장") == 0
//  								|| pTgtDev->m_strBtype.CompareNoCase(L"주차장") == 0
//  								)
//  							{
// 								// 주차장 발화시 지상 1층포함
//  								if(pTgtDev->m_nLevel > 1)
//  									continue;
//  							}
//  							else
//  							{
//  								if(pTgtDev->m_nLevel <= -1)
//  								{
// 									// Add
// 									//continue; 
//  								}
// 								else
// 								{
// 									if(btUnderClassBuild)
// 									{
// 										if(pSrcDev->m_strBd.CompareNoCase(pTgtDev->m_strBd) != 0)
// 											continue;
// 									}
// 
// 									if(btUnderClassBType)
// 									{
// 										if(pSrcDev->m_strBtype.CompareNoCase(pTgtDev->m_strBtype) != 0)
// 											continue;
// 									}
// 
// 									if(btUnderClassStair)
// 									{
// 										if(pSrcDev->m_strStair.CompareNoCase(pTgtDev->m_strStair) != 0)
// 											continue;
// 									}
// 								}
//  								
//  							}
// 							
//  						}
//  						else 
//  						{
// 							if(pTgtDev->m_nLevel <= -1)
// 							{
// 
// 							}
// 							else
// 							{
// 								if(btUnderB1F)
// 								{
// 									if(pTgtDev->m_nLevel != 1)
// 										continue;
// 								}
// 								else
// 								{
// 									if(pTgtDev->m_nLevel > 0)
// 										continue;
// 								}
// 							}
//  
//  							if(btUnderClassBuild)
//  							{
//  								if(pSrcDev->m_strBd.CompareNoCase(pTgtDev->m_strBd) != 0)
//  									continue;
//  							}
//  
//  							if(btUnderClassBType)
//  							{
//  								if(pSrcDev->m_strBtype.CompareNoCase(pTgtDev->m_strBtype) != 0)
//  									continue;
//  							}
//  
//  							if(btUnderClassStair)
//  							{
//  								if(pSrcDev->m_strStair.CompareNoCase(pTgtDev->m_strStair) != 0)
//  									continue;
//  							}
//  						}
// 						// Add
// 						pLk = new CDataLinked;
// 						pLk->SetLinkData(
// 							pTgtDev->m_nFacpID,pTgtDev->m_nUnitID,pTgtDev->m_nChannelID,pTgtDev->m_nDeviceID
// 							,LK_TYPE_RELEAY,LOGIC_ALL_AUTO,nLgID
// 							,pSrcDev->m_nFacpID,pSrcDev->m_nUnitID,pSrcDev->m_nChannelID,pSrcDev->m_nDeviceID
// 						);
// 						pSrcDev->m_ptrLink.AddTail(pLk);
// 						continue;
//  					}
//  					else if(pSrcDev->m_nLevel == 1)
//  					{
//  						if(btParkingBasic)
//  						{
//  							if(pSrcDev->m_strBd.CompareNoCase(L"주차장") == 0
//  								|| pSrcDev->m_strBtype.CompareNoCase(L"주차장") == 0
//  								|| pTgtDev->m_strBd.CompareNoCase(L"주차장") == 0
//  								|| pTgtDev->m_strBtype.CompareNoCase(L"주차장") == 0
//  								)
//  							{
//  								if(pTgtDev->m_nLevel > -1)
//  									continue;
//  							}
//  							else
//  							{
//  								if(pTgtDev->m_nLevel < 0)
//  								{
//  									if(pTgtDev->m_nLevel != -1)
//  										if(btUnder1F == 0)
//  											continue; 
//  
//  									if(btUnderClassBuild)
//  									{
//  										if(pSrcDev->m_strBd.CompareNoCase(pTgtDev->m_strBd) != 0)
//  											continue;
//  									}
//  
//  									if(btUnderClassBType)
//  									{
//  										if(pSrcDev->m_strBtype.CompareNoCase(pTgtDev->m_strBtype) != 0)
//  											continue;
//  									}
//  
//  									if(btUnderClassStair)
//  									{
//  										if(pSrcDev->m_strStair.CompareNoCase(pTgtDev->m_strStair) != 0)
//  											continue;
//  									}
//  								}
//  								else
//  								{
//  									if(btMatchBuild)
//  									{
//  										if(pSrcDev->m_strBd.CompareNoCase(pTgtDev->m_strBd) != 0)
//  											continue;
//  									}
//  
//  									if(btMatchBType)
//  									{
//  										if(pSrcDev->m_strBtype.CompareNoCase(pTgtDev->m_strBtype) != 0)
//  											continue;
//  									}
//  
//  									if(btMatchStair)
//  									{
//  										if(pSrcDev->m_strStair.CompareNoCase(pTgtDev->m_strStair) != 0)
//  											continue;
//  									}
//  									if(btMatchFloor)
//  									{
//  										if(btPluseNFloor > 0)
//  										{
//  											if(pSrcDev->m_nLevel + btPluseNFloor >= pTgtDev->m_nLevel
//  												&& pSrcDev->m_nLevel <= pTgtDev->m_nLevel)
//  											{
//  
//  											}
//  											else
//  												continue; 
//  										}
// 										else
// 										{
// 											if(pSrcDev->m_strFloor.CompareNoCase(pTgtDev->m_strFloor) != 0)
// 												continue;
// 										}
//  										
//  									}
//  									if(btMatchRoom)
//  									{
//  										if(pSrcDev->m_strRoom.CompareNoCase(pTgtDev->m_strRoom) != 0)
//  											continue;
//  									}
//  								}
//  							}
//  						}
//  						else
//  						{
// 							if(pTgtDev->m_nLevel < 0)
// 							{
// 								if(btUnder1F)
// 								{
// 									if(btUnderClassBuild)
// 									{
// 										if(pSrcDev->m_strBd.CompareNoCase(pTgtDev->m_strBd) != 0)
// 											continue;
// 									}
// 
// 									if(btUnderClassBType)
// 									{
// 										if(pSrcDev->m_strBtype.CompareNoCase(pTgtDev->m_strBtype) != 0)
// 											continue;
// 									}
// 
// 									if(btUnderClassStair)
// 									{
// 										if(pSrcDev->m_strStair.CompareNoCase(pTgtDev->m_strStair) != 0)
// 											continue;
// 									}
// 
// 								}
// 								else
// 									continue;
// 
// 							}
// 							else
// 							{
// 								if(btMatchBuild)
// 								{
// 									if(pTgtDev->m_strBd.CompareNoCase(pSrcDev->m_strBd) != 0)
// 										continue;
// 								}
// 
// 								if(btMatchBType)
// 								{
// 									// 건물
// 									if(pTgtDev->m_strBtype.CompareNoCase(pSrcDev->m_strBtype) != 0)
// 										continue;
// 								}
// 
// 								if(btMatchStair)
// 								{
// 									// 건물
// 									if(pTgtDev->m_strStair.CompareNoCase(pSrcDev->m_strStair) != 0)
// 										continue;
// 								}
// 
// 								if(btMatchFloor)
// 								{
// 									// 건물
// 									if(btPluseNFloor > 0)
// 									{
// 										if(pSrcDev->m_nLevel + btPluseNFloor >= pTgtDev->m_nLevel
// 											&& pSrcDev->m_nLevel <= pTgtDev->m_nLevel)
// 										{
// 
// 										}
// 										else
// 											continue;
// 
// 									}
// 									else
// 									{
// 										if(pSrcDev->m_strFloor != pTgtDev->m_strFloor)
// 											continue;
// 									}
// 								}
// 
// 								if(btMatchRoom)
// 								{
// 									// 건물
// 									if(pTgtDev->m_nRoom != pSrcDev->m_nRoom)
// 										continue;
// 								}
// 							}
//  						}
// 						// Add
// 						pLk = new CDataLinked;
// 						pLk->SetLinkData(
// 							pTgtDev->m_nFacpID,pTgtDev->m_nUnitID,pTgtDev->m_nChannelID,pTgtDev->m_nDeviceID
// 							,LK_TYPE_RELEAY,LOGIC_ALL_AUTO,nLgID
// 							,pSrcDev->m_nFacpID,pSrcDev->m_nUnitID,pSrcDev->m_nChannelID,pSrcDev->m_nDeviceID
// 						);
// 						pSrcDev->m_ptrLink.AddTail(pLk);
// 						continue; 
//  					}
//  					else if(pSrcDev->m_nLevel > 1)
//  					{
//  						if(pTgtDev->m_nLevel < 0)
//  							continue; 
// 						else
// 						{
// 							if(btMatchBuild)
// 							{
// 								if(pTgtDev->m_strBd.CompareNoCase(pSrcDev->m_strBd) != 0)
// 									continue;
// 							}
// 
// 							if(btMatchBType)
// 							{
// 								// 건물
// 								if(pTgtDev->m_strBtype.CompareNoCase(pSrcDev->m_strBtype) != 0)
// 									continue;
// 							}
// 
// 							if(btMatchStair)
// 							{
// 								// 건물
// 								if(pTgtDev->m_strStair.CompareNoCase(pSrcDev->m_strStair) != 0)
// 									continue;
// 							}
// 
// 							if(btMatchFloor)
// 							{
// 								// 건물
// 								if(btPluseNFloor > 0)
// 								{
// 									if(pSrcDev->m_nLevel + btPluseNFloor >= pTgtDev->m_nLevel
// 										&& pSrcDev->m_nLevel <= pTgtDev->m_nLevel)
// 									{
// 
// 									}
// 									else
// 										continue;
// 
// 								}
// 								else
// 								{
// 									if(pSrcDev->m_strFloor != pTgtDev->m_strFloor)
// 										continue;
// 								}
// 							}
// 
// 							if(btMatchRoom)
// 							{
// 								// 건물
// 								if(pTgtDev->m_nRoom != pSrcDev->m_nRoom)
// 									continue;
// 							}
// 						}
// 						// Add
// 						pLk = new CDataLinked;
// 						pLk->SetLinkData(
// 							pTgtDev->m_nFacpID,pTgtDev->m_nUnitID,pTgtDev->m_nChannelID,pTgtDev->m_nDeviceID
// 							,LK_TYPE_RELEAY,LOGIC_ALL_AUTO,nLgID
// 							,pSrcDev->m_nFacpID,pSrcDev->m_nUnitID,pSrcDev->m_nChannelID,pSrcDev->m_nDeviceID
// 						);
// 						pSrcDev->m_ptrLink.AddTail(pLk);
// 						continue; 
//  					}
//  					else
//  					{
//  						continue;
//  					}
//  				}
// 			}
// 
// // 			pSrcDev = (CDataDevice *)ptrScrList.GetNext(sPos);
// // 			if(pSrcDev == nullptr)
// // 				continue;
// // 
// // 			if((pSrcDev->GetEqInput() && pSrcDev->GetEqInput()->GetEquipID() != nInType)
// // 				|| (pSrcDev->GetEqName() && pSrcDev->GetEqName()->GetEquipID()!= nEqName))
// // 				continue;
// // 			tPos = ptrTgtList.GetHeadPosition();
// // 			while(tPos)
// // 			{
// // 				pTgtDev = (CDataDevice *)ptrTgtList.GetNext(tPos);
// // 				if(pTgtDev == nullptr)
// // 					continue;
// // 				if((pTgtDev->GetEqOutput() && pTgtDev->GetEqOutput()->GetEquipID() != nOutType)
// // 					|| (pTgtDev->GetEqOutContents() && pTgtDev->GetEqOutContents()->GetEquipID() != nCont))
// // 					continue;
// // 			}
// 		}
// //		TRACE("5-1. ======> %d\n",GetTickCount());
// 	}
// 
// 	pDBUtil->RSClose();
// 	dwEnd = GetTickCount();
// 	TRACE("4. ======> Compare : %d\n",dwEnd - dwTemp);
// 	dwTemp = dwEnd;
// 
// 	// Pattern 생성
// 	sPos = ptrScrList.GetHeadPosition();
// 	while(sPos)
// 	{
// 		pSrcDev = (ST_DEVTEMP *)ptrScrList.GetNext(sPos);
// 		if(pSrcDev == nullptr)
// 			continue;
// 		if(pSrcDev->m_ptrLink.GetCount() <= 0)
// 			continue; 
// 		tPos = pSrcDev->m_ptrLink.GetHeadPosition();
// 		while(tPos)
// 		{
// 			pLk = (CDataLinked *)pSrcDev->m_ptrLink.GetNext(tPos);
// 			if(pLk == nullptr)
// 				continue;
// 		}
// 	}
// 
// 	TRACE("10. ======>End :  %d\n",GetTickCount() - dwStart);
// 
// 	SendMessage(CSWM_PROGRESS_STEP,nIdx + nProgOffset,PROG_RESULT_FINISH);
// 	pDBUtil->ReduceDatabase(m_pRefFasSysData->GetPrjName());
// #if _DBG_MAKE_TIME_
// 	dtCur = COleDateTime::GetCurrentTime();
// 	GF_AddDebug(L"자동생성 종료 : %s",dtCur.Format(L"%H:%M:%S"));
// #endif
// 	pDBUtil->DBClose();
// 	delete pDBUtil;
// 	pDBUtil = nullptr;
	return 1;
}


int CFormAutoMake::MakeTempPatternData2(CDataAutoMake * pSourceAuto)
{
	if (pSourceAuto == nullptr)
		return 0;
	if (pSourceAuto->GetSysData() == nullptr)
		return 0;
	if (pSourceAuto->GetSysData()->GetSysData() == nullptr)
		return 0;

	int nCnt, i, nRet = 0;
	int nPtId, nFacp, nUnit, nChn, nDev, nlp =0;
	YAdoDatabase * pDB;
	CString strSql;
	CDataDevice * pSrcDev;
	CDataPattern * pPtn = nullptr;
	std::shared_ptr<CManagerPattern> spManager;
	pSrcDev = (CDataDevice *)pSourceAuto->GetSysData()->GetSysData();
	pDB = m_pRefFasSysData->GetPrjDB();
	strSql.Format(
		L" SELECT 1 AS LTYPE ,* "
		L" FROM TB_PATTERN_ITEM A "
		L" WHERE "
		L" EXISTS "
		L" ( "
		L" 	SELECT * "
		L" 	FROM TB_TEMP_SAVED_LINK B "
		L" 	WHERE A.FACP_ID = B.TGT_FACP AND A.UNIT_ID = B.TGT_UNIT AND A.CHN_ID = B.TGT_CHN AND A.RLY_ID = B.TGT_RLY "
		L" 	AND B.SRC_FACP = %d AND B.SRC_UNIT =  %d AND SRC_CHN =  %d AND SRC_RLY =  %d "
		L" ) "
		L" UNION "
		L" SELECT 2 AS LTYPE, * "
		L" FROM TB_PATTERN_ITEM A "
		L" WHERE "
		L" EXISTS "
		L" ( "
		L" 	SELECT * "
		L" 	FROM TB_LINK_RELAY B "
		L" 	WHERE A.FACP_ID = B.TGT_FACP AND A.UNIT_ID = B.TGT_UNIT AND A.CHN_ID = B.TGT_CHN AND A.RLY_ID = B.TGT_RLY "
		L" 	AND B.SRC_FACP = %d AND B.SRC_UNIT =  %d AND SRC_CHN =  %d AND SRC_RLY =  %d "
		L"  AND B.LG_TYPE = 1 AND B.LINK_TYPE = 2 "
		L" ) "
		L" ORDER BY A.PT_ID,A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID "
		, pSrcDev->GetFacpID(), pSrcDev->GetUnitID(), pSrcDev->GetChnID(), pSrcDev->GetDeviceID()
		, pSrcDev->GetFacpID(), pSrcDev->GetUnitID(), pSrcDev->GetChnID(), pSrcDev->GetDeviceID()
	);

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;
	spManager = m_pRefFasSysData->GetPatternManager();
	if (spManager == nullptr)
		return 0;
	nRet = 1;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (m_bStopFlag == TRUE)
		{
			nRet = 0;
			break;
		}
		nPtId = nFacp = nUnit = nChn = nDev = -1;
		if (pDB->GetFieldValue(L"PT_ID", nPtId) == FALSE)
		{
			nPtId = -1;
			pDB->MoveNext();
			continue;
		}

		if (pDB->GetFieldValue(L"FACP_ID", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"UNIT_ID", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"CHN_ID", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"RLY_ID", nDev) == FALSE)
			nDev = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
		{
			pDB->MoveNext();
			continue;
		}

		if (nPtId != nlp)
		{
			pPtn = spManager->GetPattern(nPtId);
			if (pPtn == nullptr)
			{
				pDB->MoveNext();
				continue;
			}

			nlp = nPtId;
		}
		pSourceAuto->AddTempPattern(m_pRefFasSysData, pPtn, nFacp, nUnit, nChn, nDev);
		pDB->MoveNext();
	}
	pDB->RSClose();
	//pList = pSourceAuto->GetPatternList();
	
	/// 수동으로 입력한것에 대한 처리


	return nRet;
}

int CFormAutoMake::ArrangeAutoLink2(CDataAutoMake * pSourceAuto)
{
	POSITION pos, savePos;
	CPtrList * pList;
	CDataNewAutoPtn * pPtn;
	CDataSystem * pSys;
	pSys = pSourceAuto->GetSysData();
	if (pSys == nullptr)
		return 0;

	if (pSys->GetDataType() != SE_RELAY)
		return 0;

	pList = pSourceAuto->GetRefPatternList();
	if (pList == nullptr)
		return 0;
	pos = pList->GetHeadPosition();
	while (pos)
	{
		if (m_bStopFlag == TRUE)
			return -1;
		savePos = pos;
		pPtn = (CDataNewAutoPtn *)pList->GetNext(pos);
		if (pPtn == nullptr)
			continue;
		if (pPtn->CheckUseLink())
			ArrangeRelayLink2(pSourceAuto, pPtn);
		else
			DeleteAutoPattern2(pPtn, pList, savePos);
	}
	return 1;
}


int CFormAutoMake::ArrangeRelayLink2(CDataAutoMake * pSourceAuto, CDataNewAutoPtn * pPtn)
{
	CDataSystem * pSys;
	CDataDevice * pSrcDev;
	pSys = pSourceAuto->GetSysData();
	if (pSys == nullptr || pSys->GetSysData() == nullptr || pSys->GetDataType() != SE_RELAY)
		return 0;

	pSrcDev = (CDataDevice *)pSys->GetSysData();

	CDataLinked * pTemp;
	CDataNewAutoLink * pALnk = nullptr;
	CPtrList * pList;
	POSITION posAuto;
	pList = pPtn->GetRefAutoLinkList();
	posAuto = pList->GetHeadPosition();
	while (posAuto)
	{
		if (m_bStopFlag == TRUE)
			return -1;
		pALnk = (CDataNewAutoLink*)pList->GetNext(posAuto);
		if (pALnk == nullptr)
			continue;
		pTemp = pALnk->GetRefLink();
		pSrcDev->DeleteLinkByID(LK_TYPE_RELEAY, pTemp->GetTgtFacp(), pTemp->GetTgtUnit(), pTemp->GetTgtChn(), pTemp->GetTgtDev());
	}
	return 1;
}

int CFormAutoMake::DeleteAutoPattern2(CDataNewAutoPtn * pPtn, CPtrList * pList, POSITION removePos)
{
	if (pList == nullptr)
		return 0;

	if (removePos == nullptr)
		return 0;

	pList->RemoveAt(removePos);
	if (pPtn == nullptr)
	{
		delete pPtn;
		pPtn = nullptr;
	}
	return 1;
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
			AutoMakeStart();
		else if(lp == -1)
			AfxMessageBox(L"사용자가 취소했습니다.");
		else
			AfxMessageBox(L"오류가 발생하여 연동데이터 자동생성을 할 수 없습니다.");
#else
		if (lp == 0)
			AutoMakeStart();
		else if (lp == -1)
			AfxMessageBox(L"User canceled.");
		else
			AfxMessageBox(L"An error has occurred and the site logic data could not be autogenerated.");
#endif
		break;
	}
	return 0;
}