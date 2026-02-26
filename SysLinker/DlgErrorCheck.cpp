// DlgErrorCheck.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgErrorCheck.h"
#include "afxdialogex.h"


#include "RelayTableData.h"
#include "ManagerPattern.h"
#include "DataSystem.h"
#include "DataDevice.h"
#include "DataPattern.h"
#include "DataLinked.h"
#include "DataEquip.h"
#include "DataLocStair.h"
#include "../Common/Utils/StdioFileEx.h"

#include "../Common/Utils/YAdoDatabase.h"
// CDlgErrorCheck 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgErrorCheck, CDialogEx)

#ifndef ENGLISH_MODE
CDlgErrorCheck::CDlgErrorCheck(int nCheckType,CRelayTableData * pRefFasSysData,CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_ERRORCHECK, pParent)
{
	m_nCheckType = nCheckType;
	m_bStopFlag = FALSE;
	m_pNotifyWnd = nullptr;
	m_nErrorCnt = 0; 
	m_nAllCnt = 0;
	m_nWarningCnt = 0;
	m_pRefFasSysData = pRefFasSysData;
}
#else
CDlgErrorCheck::CDlgErrorCheck(int nCheckType, CRelayTableData * pRefFasSysData, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_ERRORCHECK_EN, pParent)
{
	m_nCheckType = nCheckType;
	m_bStopFlag = FALSE;
	m_pNotifyWnd = nullptr;
	m_nErrorCnt = 0;
	m_nAllCnt = 0;
	m_nWarningCnt = 0;
	m_pRefFasSysData = pRefFasSysData;
}
#endif

CDlgErrorCheck::~CDlgErrorCheck()
{
	RemoveAllError();
}

void CDlgErrorCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_PROGRESS,m_ctrlProg);
	DDX_Control(pDX,IDC_CMB_TYPE,m_cmbType);
	DDX_Control(pDX,IDC_LIST_REPORT,m_ctrlReport);
}


BEGIN_MESSAGE_MAP(CDlgErrorCheck, CDialogEx)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDOK,&CDlgErrorCheck::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,&CDlgErrorCheck::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_VIEW,&CDlgErrorCheck::OnBnClickedBtnView)
	ON_MESSAGE(CSWM_PROGRESS_STEP,OnMakeProgress)
	ON_BN_CLICKED(IDC_BTN_STOP,&CDlgErrorCheck::OnBnClickedBtnStop)
	ON_NOTIFY(NM_DBLCLK,IDC_LIST_REPORT,&CDlgErrorCheck::OnNMDblclkListReport)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SAVE,&CDlgErrorCheck::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CDlgErrorCheck 메시지 처리기입니다.


BOOL CDlgErrorCheck::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
#ifndef ENGLISH_MODE
	m_ctrlReport.InsertColumn(0,_T("오류종류"),LVCFMT_LEFT,70);
	m_ctrlReport.InsertColumn(1,_T("분류"),LVCFMT_LEFT,150);
	m_ctrlReport.InsertColumn(2,_T("내용"),LVCFMT_LEFT,350);
#else
	m_ctrlReport.InsertColumn(0, _T("ERROR TYPE"), LVCFMT_LEFT, 70);
	m_ctrlReport.InsertColumn(1, _T("SORT"), LVCFMT_LEFT, 150);
	m_ctrlReport.InsertColumn(2, _T("DESCRIPTION"), LVCFMT_LEFT, 350);
#endif
	m_ctrlReport.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	SetPosition();
	int i,nIdx;
	for(i = CHK_ALL_DATA; i < CHK_CNT ; i ++)
	{
		nIdx = m_cmbType.InsertString(i,g_strErrChkText[i]);
		m_cmbType.SetItemData(nIdx,i);
	}
	m_cmbType.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgErrorCheck::CreateChkDlg(CWnd * pParent)
{
#ifndef ENGLISH_MODE
	return Create(IDD_DLG_ERRORCHECK,pParent);
#else
	return Create(IDD_DLG_ERRORCHECK_EN, pParent);
#endif
}

void CDlgErrorCheck::StartErrorCheck(int nCheckType,CWnd * pTargetWnd)
{
	m_bStopFlag = FALSE;
	m_nCheckType = nCheckType;
	m_pNotifyWnd = pTargetWnd;
	m_nErrorCnt = 0;
	m_nAllCnt = 0;
	m_nWarningCnt = 0;
	RemoveAllError();
	m_ctrlReport.DeleteAllItems();
	m_cmbType.SetCurSel(0);

	//20251210 GBM start - 프로젝트를 닫았다가 다시 프로젝트 열고 오류 검사하면 죽는 오류 수정
	m_pRefFasSysData = theApp.m_pFasSysData;
	//20251210 GBM end

	GetDlgItem(IDC_BTN_VIEW)->EnableWindow(FALSE);
	AfxBeginThread((AFX_THREADPROC)Thread_CheckProc,(LPVOID)this);
}

void CDlgErrorCheck::OnSize(UINT nType,int cx,int cy)
{
	CDialogEx::OnSize(nType,cx,cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetPosition();
}


void CDlgErrorCheck::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	lpMMI->ptMinTrackSize.x = 850;
	lpMMI->ptMinTrackSize.y = 500;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CDlgErrorCheck::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
	ShowWindow(SW_HIDE);
}


void CDlgErrorCheck::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
	ShowWindow(SW_HIDE);

}

void CDlgErrorCheck::OnBnClickedBtnView()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos;
	ST_ERRCHECK * pData;
	CString strErr = L"",strType = L"",strDesc = L"";;
	int nSel,nType;
	m_ctrlReport.DeleteAllItems();


	nSel = m_cmbType.GetCurSel();
	if(nSel < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"선택된 아이템이 없습니다.");
#else
		AfxMessageBox(L"No items have been selected.");
#endif
		return;
	}
	nType = m_cmbType.GetItemData(nSel);

	pos = m_ptrErrorList.GetHeadPosition();
	while(pos)
	{
		pData = (ST_ERRCHECK *)m_ptrErrorList.GetNext(pos);
		if(pData == nullptr)
			continue; 

		if((nType == 0 || nType == pData->btType) == FALSE)
			continue; 

		switch(pData->btType)
		{
		case CHK_PATTERN_CNT:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_PATTERN_ITEM_CNT:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_TEXT_CNT:
			strErr = L"WARNING";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_LEVEL_DUP:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_NOINPUT:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_OUTPUT_CNT:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_NOUSE_OUTPUT:
			strErr = L"WARNING";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_INPUT_WITHOUT_OUTPUT:
			strErr = L"WARNING";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_GAS_VALVE_REDUNDANCY:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		default:
			strErr = L"Unknown";
			strType = L"Unknown";
			strDesc = L"Unknown";
			break;
		}
		nSel = m_ctrlReport.AddItem(strErr);
		m_ctrlReport.SetItemText(nSel,1,strType);
		m_ctrlReport.SetItemText(nSel,2,strDesc);
		m_ctrlReport.SetItemData(nSel,(LPARAM)pData);
	}
	m_ctrlReport.Complete();
}


void CDlgErrorCheck::SetPosition()
{
	CRect rc,rcwin;
	GetClientRect(&rc);

	if(m_ctrlReport.GetSafeHwnd() != nullptr)
	{
		rcwin.top = 100;
		rcwin.left = 10;
		rcwin.right = rc.right - 10;
		rcwin.bottom = rc.bottom - 10;
		m_ctrlReport.MoveWindow(&rcwin);
	}
}


DWORD CDlgErrorCheck::Thread_CheckProc(LPVOID lpData)
{
	CDlgErrorCheck	*me;
	int nRet = 0;
	me = (CDlgErrorCheck *)lpData;
	if((CDlgErrorCheck *)NULL == me)
	{
		me->SendMessage(CSWM_PROGRESS_STEP,0,0);
		return 0;
	}

	try
	{
		me->ProcErrorCheck();
	}
	catch(...)
	{
	}
	return 0;
}

LRESULT CDlgErrorCheck::OnMakeProgress(WPARAM wp,LPARAM lp)
{
	CString str;
	int nP = 0;
	switch(lp)
	{
	case PROG_RESULT_CANCEL:
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
#ifndef ENGLISH_MODE
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"사용자가 취소했습니다.");
#else
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"User canceled.");
#endif
		m_ctrlProg.SetPos(100);
		m_ctrlReport.Complete();
		if(m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
			m_pNotifyWnd->SendMessage(UWM_ERRORCHECK_NOTIFY , (WPARAM)m_nCheckType,-1);
		break;
	case PROG_RESULT_ERROR:
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
#ifndef ENGLISH_MODE
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"생성 중 오류가 발생했습니다.");
#else
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"An error has occurred during creation.");
#endif
		m_ctrlProg.SetPos(100);
		m_ctrlReport.Complete();
		if(m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
			m_pNotifyWnd->SendMessage(UWM_ERRORCHECK_NOTIFY,(WPARAM)m_nCheckType,-2);
		break;
	case PROG_RESULT_STEP:
		nP = (int)(((float)wp / (float)m_nAllCnt) * 100);
#ifndef ENGLISH_MODE
		str.Format(L"[%d]%% (%d/%d) 검사 중 - 오류 %d, 경고 %d",nP,wp,m_nAllCnt,m_nErrorCnt,m_nWarningCnt);
#else
		str.Format(L"Checked [%d]%% ([%d]/[%d]) - [%d] Error(s), [%d] Warning(s)", nP, wp, m_nAllCnt, m_nErrorCnt, m_nWarningCnt);
#endif
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlProg.SetPos(nP);
		break;
	case PROG_RESULT_FINISH:
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
		//str.Format(L"%d개의 데이터를 검사했습니다.",m_nAllCnt);
#ifndef ENGLISH_MODE
		str.Format(L"전체 %d개의 데이터 중 오류 %d개,경고 %d개를 발견했습니다.",m_nAllCnt,m_nErrorCnt,m_nWarningCnt);
#else
		str.Format(L"We found [%d] error(s) and [%d] warning(s) out of a total of [%d] pieces of data.", m_nErrorCnt, m_nWarningCnt, m_nAllCnt);
#endif
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlReport.Complete();
		m_ctrlProg.SetPos(100);
// 		if(m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
// 			m_pNotifyWnd->SendMessage(UWM_ERRORCHECK_NOTIFY,(WPARAM)m_nCheckType,m_nErrorCnt);
		break;
	}
	return 0;
}


int CDlgErrorCheck::ProcErrorCheck()
{
	// 전체 개수 : 패터 개수 + 층개수(sql사용) + 회로개수
	// 1. 패턴 내 출력 개수  -(검사개수 패턴 개수)
	// 2. 층 번호 중복
	// 3. 글자수 초과 
	// 4. 연동출력 개수
	// 5. 출력 없는 입력
	// 6. 사용되지 않는 출력
	int nAllCnt = 0,i ,nPtnCnt , nCnt,nItemCnt,nOffset=0,nRCnt,nCCnt;
	int nBid,nBtype,nSid,nFlid,nFlNum;
	int nSize = 0; 
	BOOL bRet = FALSE;
	CString strKey,strSql,strName,strDesc;
	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
	std::shared_ptr<CManagerPattern> spPmng = m_pRefFasSysData->GetPatternManager();
	std::shared_ptr<CManagerLocation> spLmng = m_pRefFasSysData->GetLocationManager();
	CDataPattern * pPtn;
	CMapSystemData::iterator it;
	CDataSystem * pData; 
	CDataDevice * pDev = nullptr;
	CDataEquip * pIn = nullptr;
	int nInType = 0; 
	CDataLinked *pLink = nullptr;
	CDataLocStair * pLoc;
	CPtrList * pListLink = nullptr;
	YAdoDatabase * pDBUtil = nullptr;

	POSITION pos;
	char szStrBuff[256] = { 0 };

	//20251209 GBM start - 가스A, 가스B, 밸브 중복 체크를 위한 변수
	CDataDevice* pTemp;
	CRelayTableData* pRelayTableData = theApp.GetRelayTableData();
	if (pRelayTableData == nullptr)
		return 0;
	pRelayTableData->SetRecheckOutputContentInfo();
	int nListSize = pRelayTableData->m_roci.IDVec.size();
	int* nCountList = new int[nListSize];
	//20251209 GBM end

	pRelayTableData->CheckAndSetFacpAndUnitType();	// 이 함수를 호출해서 ROM 파일 버전을 세팅
	int nRomVersion = CNewInfo::Instance()->m_gi.romVersion;	
	int nMaxPatternCount = CNewInfo::Instance()->m_nMaxPatternCount;
	int nMaxPatternItemCount = CNewInfo::Instance()->m_nMaxPatternItemCount;
	int nMaxLinkCount = CNewInfo::Instance()->m_nMaxLinkCount;


	pDBUtil = new YAdoDatabase;
	pDBUtil->MSSqlInitialize(g_stConfig.szDBPass,g_stConfig.szDBUser
		,m_pRefFasSysData->GetPrjName(),g_stConfig.szSource);

	if(pDBUtil->DBOpen() == FALSE)
	{
#ifndef ENGLISH_MODE
		GF_AddDebug(L"자동생성 오류 : 데이터베이스를 연결하는 데에 실패했습니다.");
		AfxMessageBox(L"데이터베이스 접속에 실패했습니다.");
#else
		GF_AddDebug(L"Autogeneration error: failed to connect to the database.");
		AfxMessageBox(L"Failed to connect to the database.");
#endif
		return 0;

	}
	// 전체 개수 
	// 1.패턴 개수

	//20251210 GBM start - 패턴 자체 개수 검사 추가
	int nPatternCount = spPmng->GetCount();
	nCnt = nPatternCount + 1;	
	//20251210 GBM end

	nPtnCnt = nCnt;
	nAllCnt += nCnt;

	nCnt = 0; 
	// 2. 계단 개수 - sql 처리
	strSql = L"SELECT COUNT(*) AS CNT FROM TB_LOC_FLOOR";
	if(pDBUtil->OpenQuery(strSql))
	{
		pDBUtil->GetFieldValue(L"CNT" ,nCnt);
		nAllCnt += nCnt;
	}
	// 3. 회로 개수 - sql
	strSql = L"SELECT COUNT(*) AS CNT FROM TB_RELAY_LIST";
	if(pDBUtil->OpenQuery(strSql))
	{
		pDBUtil->GetFieldValue(L"CNT",nCnt);
		if(m_nCheckType != ERR_CHECK_MAKEAUTOLINK)
			nCnt *= 4;
		else
			nCnt *= 2;
		nAllCnt += nCnt;
	}

	m_nAllCnt = nAllCnt;
#if _DBG_MAKE_TIME_
	COleDateTime dtCur;
	DWORD_PTR dwStart,dwEnd,dwTemp;
	dtCur = COleDateTime::GetCurrentTime();
#ifndef ENGLISH_MODE
	GF_AddDebug(L"오류 검사 시작: %s",dtCur.Format(L"%H:%M:%S"));
#else
	GF_AddDebug(L"Error check start time: %s", dtCur.Format(L"%H:%M:%S"));
#endif
	dwStart = GetTickCount();
	dwTemp = dwStart;
#endif
	SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_STEP);
	//nProgress = 0;
	nOffset = 0;
	if(spPmng)
	{
		if(m_bStopFlag == TRUE)
		{
			SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_CANCEL);
			pDBUtil->DBClose();
			delete pDBUtil;
			pDBUtil = nullptr;
			return 0; 
		}

		//20251210 GBM start - 패턴 자체 개수 추가	
		if (nPatternCount >= nMaxPatternCount)
		{
			// 사용자가 리스트에 아이템을 클릭하면 가장 빠른 번호의 패턴이 선택된 상태에서 트리가 펼쳐지도록 함
			pos = spPmng->GetHeadPosition();
			pPtn = spPmng->GetNext(pos);
			if (pPtn != nullptr)
			{
#ifndef ENGLISH_MODE
				strDesc.Format(L"패턴 개수 초과 - %d개", nPatternCount);
#else
				strDesc.Format(L"Number of patterns exceeded - %d", nPatternCount);
#endif
				InsertErrorList(CHK_PATTERN_CNT, nPatternCount, (LPVOID)pPtn, strDesc);
			}
		}
		SendMessage(CSWM_PROGRESS_STEP, nOffset, PROG_RESULT_STEP);
		nOffset++;
		//20251210 GBM end

		nItemCnt = 0;
		pos = spPmng->GetHeadPosition();
		while(pos)
		{
			if(m_bStopFlag == TRUE)
			{
				SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_CANCEL);
				return 0;
			}
			pPtn = spPmng->GetNext(pos);
			
			if(pPtn == nullptr)
				continue; 
			nItemCnt = pPtn->GetItemCount();
			if(nItemCnt >= nMaxPatternItemCount)
			{
				// Error 확인 - 리스트 입력
#ifndef ENGLISH_MODE
				strDesc.Format(L"패턴 : %s 개수초과 - %d개" , pPtn->GetPatternName(), nItemCnt);	//20240611 GBM - 오류 수정, 패턴 명 변수 미입력
#else
				strDesc.Format(L"Pattern : %s count limit exceeded - [%d] patterns", pPtn->GetPatternName(), nItemCnt);	//20240611 GBM - 오류 수정, 패턴 명 변수 미입력
#endif
				InsertErrorList(CHK_PATTERN_ITEM_CNT,nItemCnt,pPtn,strDesc);
			}
			nOffset ++;
			SendMessage(CSWM_PROGRESS_STEP,nOffset,PROG_RESULT_STEP);
		}
	}
	else
	{
		nOffset += nPtnCnt;
		SendMessage(CSWM_PROGRESS_STEP,nOffset,PROG_RESULT_STEP);
	}

	// 층번호 중복
	strSql.Format(
		L"SELECT A.* FROM TB_LOC_FLOOR A ,"
		L"	(SELECT BD_ID,BTYPE_ID,STAIR_ID,FL_NUM,COUNT(*) AS CNT FROM TB_LOC_FLOOR"
		L"		WHERE FL_NUM < 1000"
		L"		GROUP BY FL_NUM,BD_ID,BTYPE_ID,STAIR_ID"
		L"		HAVING COUNT(*) > 1) B"
		L"	WHERE A.BD_ID = B.BD_ID AND A.BTYPE_ID = B.BTYPE_ID  AND A.FL_NUM = B.FL_NUM");

	if(pDBUtil->OpenQuery(strSql))
	{
		nCnt = pDBUtil->GetRecordCount();
		for(i = 0; i < nCnt; i++)
		{
			pDBUtil->GetFieldValue(L"BD_ID",nBid);
			pDBUtil->GetFieldValue(L"BTYPE_ID",nBtype);
			pDBUtil->GetFieldValue(L"STAIR_ID",nSid);
			pDBUtil->GetFieldValue(L"FL_ID",nFlid);
			pDBUtil->GetFieldValue(L"FL_NUM",nFlNum);
			pDBUtil->GetFieldValue(L"FL_NAME",strName);
			pLoc = (CDataLocStair*)m_pRefFasSysData->GetLocation(nBid,nBtype,nSid);
			if(pLoc == nullptr)
			{
				pDBUtil->MoveNext();
				continue; 
			}
#ifndef ENGLISH_MODE
			strDesc.Format(L"\"%s %s %s %s\" 층번호 중복 : %d "
				,pLoc->GetBuildName() ,pLoc->GetBTypeName() ,pLoc->GetStairName() ,strName
				,nFlNum);
#else
			strDesc.Format(L"\"%s %s %s %s\" floor number duplicate: %d "
				, pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName(), strName
				, nFlNum);
#endif
			InsertErrorList(CHK_LEVEL_DUP,nSize,pLoc,strDesc);
			pDBUtil->MoveNext();
		}
		pDBUtil->RSClose();
	}


	for(it = pRefMap->begin(); it != pRefMap->end(); it++)
	{
		if(m_bStopFlag == TRUE)
		{
			SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_CANCEL);
			pDBUtil->DBClose();
			delete pDBUtil;
			pDBUtil = nullptr;
			return 0;
		}
		
		pData = it->second;
		if(pData == nullptr)
			continue;
		strKey = it->first;
		if(strKey == L"")
			continue;
		if(pData->GetSysData() == nullptr || pData->GetDataType() != SE_RELAY)
			continue;
		SendMessage(CSWM_PROGRESS_STEP,nOffset,PROG_RESULT_STEP);
		if(m_nCheckType != ERR_CHECK_MAKEAUTOLINK)
			nOffset += 4;
		else
			nOffset += 2;
		pDev = (CDataDevice *)pData->GetSysData();

		TRACE(L"=================1. %s\n" , pDev->GetInputFullName());
		// 이름 길이
		strName = pDev->GetEquipLocationName();
		memset(szStrBuff,0,256);
		nSize = GF_Unicode2ASCII(strName.GetBuffer(),szStrBuff,256);
		strName.ReleaseBuffer();
		nSize += 1;
		//sizeof(szStrBuff);

		//20240424 GBM start - 수신기 타입에 따른 글자수 제한 처리
		int nFacpNum = pDev->GetFacpNum();
		ASSERT(nFacpNum > -1);
		int nFacpType = CNewInfo::Instance()->m_gi.facpType[nFacpNum];
		int nLimit;
		CString strFacpType;
		if (nFacpType == GT1)
		{
			nLimit = 80;
#ifndef ENGLISH_MODE
			strFacpType = _T("[GT1 수신기]");
#else
			strFacpType = _T("[GT1 FACP]");
#endif
		}
		else
		{
			nLimit = 40;
#ifndef ENGLISH_MODE
			strFacpType = _T("[F3 수신기]");
#else
			strFacpType = _T("[F3 FACP]");
#endif
		}			
		//20240424 GBM end

		if(nSize > nLimit)
		{
#ifndef ENGLISH_MODE
			strDesc.Format(L"[%s] \"%s\" 글자수 초과(%d) "
				,strFacpType ,pDev->GetInputFullName(),nSize);
#else
			strDesc.Format(L"[%s] \"%s\" character length exceeded(%d) "
				, strFacpType, pDev->GetInputFullName(), nSize);
#endif	
			InsertErrorList(CHK_TEXT_CNT,nSize,pDev,strDesc);
		}

		if(pDev->GetEqInput() == nullptr)
		{
#ifndef ENGLISH_MODE
			strDesc.Format(L"\"%s\" 입력 타입 없음"
				,pDev->GetInputFullName());
#else
			strDesc.Format(L"No \"%s\" input type"
				, pDev->GetInputFullName());
#endif
			InsertErrorList(CHK_NOINPUT,0,pDev,strDesc);
		}

		// 20251209 GBM start - 아래에서 연동데이터 자동 생성 시 진행하지 않으므로 여기서 가스, 밸브 중복 여부를 판단해야 함
		// 가스, 밸브 중복 여부 판단
		CString strRecheckErr = _T("");
		memset(nCountList, 0, sizeof(int) * nListSize);
		CDataLinked* pDataLinked = nullptr;
		int nType = -1;
		CPtrList* pList = pDev->GetLinkedList();
		pos = pList->GetHeadPosition();
		while (pos)
		{
			pDataLinked = (CDataLinked*)pList->GetNext(pos);
			if(pDataLinked == nullptr)
				continue;

			nType = pDataLinked->GetLinkType();
			switch (nType)
			{
			case LK_TYPE_RELEAY:
				pTemp = pRelayTableData->GetDeviceByID(pDataLinked->GetTgtFacp(), pDataLinked->GetTgtUnit(), pDataLinked->GetTgtChn(), pDataLinked->GetTgtDev());
				if (pTemp == nullptr)
					break;

				for (int i = 0; i < nListSize; i++)
				{
					int nEquipID = pRelayTableData->m_roci.IDVec[i];
					CDataEquip* pDataEquip = pTemp->GetEqOutContents();
					if (pDataEquip != nullptr)
					{
						if (nEquipID == pDataEquip->GetEquipID())
						{
							nCountList[i]++;
						}
					}
				}

				break;

			default:
				break;
			}
		}

		BOOL bAlert = FALSE;
		if (nListSize > 0)
		{
#ifndef ENGLISH_MODE
			strRecheckErr.Format(_T("[%02d%02d-%d%03d] %s의 연동데이터에 문제가 있습니다. "), pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum(), pDev->GetEquipLocationName());
#else
			strRecheckErr.Format(_T("There is a problem with [%02d%02d-%d%03d] %s linked data. "), pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum(), pDev->GetEquipLocationName());
#endif
			for (int i = 0; i < nListSize; i++)
			{
				if (nCountList[i] > 1)
				{
					CString strTemp = _T("");
#ifndef ENGLISH_MODE
					strTemp.Format(_T("[%s]의 연동데이터 개수가 %d개 입니다. "), pRelayTableData->m_roci.nameVec[i], nCountList[i]);
#else
					strTemp.Format(_T("The number of linked data for [%s] is %d "), pRelayTableData->m_roci.nameVec[i], nCountList[i]);
#endif
					strRecheckErr += strTemp;
					bAlert = TRUE;
				}
			}
		}
		
		if (bAlert)
		{
			InsertErrorList(CHK_GAS_VALVE_REDUNDANCY, 0, pDev, strRecheckErr);
		}
		// 20251209 GBM end

		if(m_nCheckType == 2) // 연동데이터 생성
			continue; 


//////////////////////////////////////////////////////////////////////////
/// 연동데이터 자동 생성 시 확인하지 않음

		// 4. 연동출력 개수 확인 - 접점 , 회로
		// 5. 출력 없는 입력 - 점점 + 회로
		pIn = pDev->GetEqInput();
		if(pIn)
		{
			nInType = pIn->GetEquipID();
			if(nInType == 1 // 자탐감지기
				|| nInType == 2 // A교차감지기
				|| nInType == 3 // B교차감지기
				|| nInType == 4 // 일반감지기
				|| nInType == 6 // 열식아나로그
				|| nInType == 7 // 연식아나로그
				|| nInType == 8 // 열식주소형
				|| nInType == 9 // 연식주소형
				)
			{
				if(GetContactNRelayCount(pDev,nRCnt,nCCnt) == FALSE)
				{
					// Warning - 출력 없음
#ifndef ENGLISH_MODE
					strDesc.Format(L"\"%s\" 출력없음:접점(%d) 출력(%d)"
						,pDev->GetInputFullName(),nRCnt,nCCnt);
#else
					strDesc.Format(L"No \"%s\" output: contact (%d) Output (%d)"
						, pDev->GetInputFullName(), nRCnt, nCCnt);
#endif
					InsertErrorList(CHK_INPUT_WITHOUT_OUTPUT,0,pDev,strDesc);
				}
				else
				{
					if((nRCnt + nCCnt) <= 0)
					{
						// 5. 출력 없는 입력 - 점점 + 회로
						// Warning 출력 없음 - 
#ifndef ENGLISH_MODE
						strDesc.Format(L"\"%s\" 출력없음:접점(%d) 출력(%d)"
							,pDev->GetInputFullName(),nRCnt,nCCnt);
#else
						strDesc.Format(L"No \"%s\" output: contact (%d) Output (%d)"
							, pDev->GetInputFullName(), nRCnt, nCCnt);
#endif
						InsertErrorList(CHK_INPUT_WITHOUT_OUTPUT,0,pDev,strDesc);
					}
					else
					{
						if(nCCnt > nMaxLinkCount)
						{
#ifndef ENGLISH_MODE
							strDesc.Format(L"\"%s\" 연동출력 접점 개수초과:접점(%d) 출력(%d)"
								,pDev->GetInputFullName(),nRCnt,nCCnt);
#else
							strDesc.Format(L"\"%s\" interlock output relay contact limit exceeded: contact (%d) output (%d)"
								, pDev->GetInputFullName(), nRCnt, nCCnt);
#endif
							InsertErrorList(CHK_OUTPUT_CNT,nCCnt,pDev,strDesc);
						}

						if(nRCnt > nMaxLinkCount)
						{
#ifndef ENGLISH_MODE
							strDesc.Format(L"\"%s\" 연동출력 회로 개수초과:접점(%d) 출력(%d)"
								,pDev->GetInputFullName(),nRCnt,nCCnt);
#else
							strDesc.Format(L"\"%s\" interlock output circuit limit exceeded: contact (%d) output (%d)"
								, pDev->GetInputFullName(), nRCnt, nCCnt);
#endif
							InsertErrorList(CHK_OUTPUT_CNT,nRCnt,pDev,strDesc);
						}
					}
				}
			}
		}


		TRACE(L"=================2. %s\n",pDev->GetInputFullName());
		// 6. 사용되지 않는 출력 - 출력인지 확인
		if(pDev->GetEqOutput() > 0)
		{
			strSql.Format(
				L"SELECT (A.NUM + B.NUM + C.NUM + D.NUM) AS NUM FROM "
				L"	(SELECT COUNT(*) AS NUM  FROM "
				L"		(SELECT X.TGT_FACP FROM TB_LINK_RELAY X WHERE X.LINK_TYPE = %d) L , "
				L"		(SELECT * FROM TB_PATTERN_ITEM Y WHERE Y.FACP_ID=%d AND Y.UNIT_ID=%d AND Y.CHN_ID=%d AND Y.RLY_ID=%d) M"
				L"	WHERE M.PT_ID = L.TGT_FACP) A, "
				L"	(SELECT COUNT(*) AS NUM FROM TB_LINK_RELAY N "
				"	WHERE N.LINK_TYPE=%d AND N.TGT_FACP=%d AND N.TGT_UNIT=%d AND N.TGT_CHN=%d AND N.TGT_RLY=%d) B,"
				L"	(SELECT COUNT(*) AS NUM  FROM "
				L"	(SELECT X.FACP_ID FROM TB_LINK_PUMP X WHERE X.LINK_TYPE = %d) L, "
				L"		(SELECT * FROM TB_PATTERN_ITEM Y WHERE Y.FACP_ID = %d AND Y.UNIT_ID = %d AND Y.CHN_ID = %d AND Y.RLY_ID = %d) M "
				L"		WHERE M.PT_ID = L.FACP_ID) C, "
				L"		(SELECT COUNT(*) AS NUM FROM TB_LINK_PUMP N "
				L"			WHERE N.LINK_TYPE = %d AND N.FACP_ID = %d AND N.UNIT_ID = %d AND N.CHN_ID = %d AND N.RLY_ID = %d) D "
				,LK_TYPE_PATTERN //X.LINK_TYPE
				,pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
				,LK_TYPE_RELEAY
				,pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
				,LK_TYPE_PATTERN
				,pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
				,LK_TYPE_RELEAY
				,pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
			);


			TRACE(L"%s\n",strSql);
			if(pDBUtil->OpenQuery(strSql) == false)
			{
				// Warning 입력
#ifndef ENGLISH_MODE
				strDesc.Format(L"입력이름:\"%s\" - 출력이름:\"%s\" 사용되지 않는 출력"
					,pDev->GetInputFullName(),pDev->GetOutputFullName());
#else
				strDesc.Format(L"Input Name: \"%s\" - Output Name: \"%s\" unused outputs"
					, pDev->GetInputFullName(), pDev->GetOutputFullName());
#endif
				InsertErrorList(CHK_NOUSE_OUTPUT,nRCnt,pDev,strDesc);
			}
			else
			{
				nCnt = pDBUtil->GetRecordCount();
				for(i = 0; i < nCnt; i++)
				{
					pDBUtil->GetFieldValue(L"NUM",nItemCnt);
					if(nItemCnt <= 0)
					{
						// Warning 입력
#ifndef ENGLISH_MODE
						strDesc.Format(L"입력이름:\"%s\" - 출력이름:\"%s\" 사용되지 않는 출력"
							,pDev->GetInputFullName(),pDev->GetOutputFullName());
#else
						strDesc.Format(L"Input Name: \"%s\" - Output Name: \"%s\" unused outputs"
							, pDev->GetInputFullName(), pDev->GetOutputFullName());
#endif
						InsertErrorList(CHK_NOUSE_OUTPUT,nRCnt,pDev,strDesc);
					}
					pDBUtil->MoveNext();
				}
				pDBUtil->RSClose();
			}
		}
	}

#if _DBG_MAKE_TIME_
	dtCur = COleDateTime::GetCurrentTime();
#ifndef ENGLISH_MODE
	GF_AddDebug(L"오류 검사 종료 : %s",dtCur.Format(L"%H:%M:%S"));
#else
	GF_AddDebug(L"Error check end time: %s", dtCur.Format(L"%H:%M:%S"));
#endif
#endif


	SendMessage(CSWM_PROGRESS_STEP,m_nAllCnt,PROG_RESULT_FINISH);
	pDBUtil->DBClose();
	delete pDBUtil;
	pDBUtil = nullptr;

	if(m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
		m_pNotifyWnd->SendMessage(UWM_ERRORCHECK_NOTIFY,(WPARAM)m_nCheckType,m_nErrorCnt);

	delete[] nCountList;	//20251211 GBM - 동적할당 해제

	return 1;
}


BOOL CDlgErrorCheck::GetContactNRelayCount(CDataDevice* pDev,int& nRelayCnt,int& nContactCnt)
{
	if(pDev == nullptr)
		return FALSE;
	CDataLinked * pData;
	CPtrList * pListLink;
	POSITION pos;
	pListLink = pDev->GetLinkedList();
	if(pListLink == nullptr)
		return FALSE;

	nRelayCnt = 0; 
	nContactCnt = 0; 
	pos = pListLink->GetHeadPosition();
	while(pos)
	{
		pData = (CDataLinked *)pListLink->GetNext(pos);
		if(pData == nullptr)
			continue;
		if(pData->GetLinkType() == LK_TYPE_RELEAY)
			nRelayCnt ++;
		else
			nContactCnt ++;
	}

	return TRUE;
}

void CDlgErrorCheck::RemoveAllError()
{
	ST_ERRCHECK * pData; 
	while(!m_ptrErrorList.IsEmpty())
	{
		pData = (ST_ERRCHECK *)m_ptrErrorList.RemoveHead();
		if(pData == nullptr)	//20240912 GBM - 메모리 누수 발생 수정 ( != -> == )
			continue; 
		delete pData;
		pData = nullptr;
	}
}

void CDlgErrorCheck::OnBnClickedBtnStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bStopFlag = TRUE;
}


void CDlgErrorCheck::OnNMDblclkListReport(NMHDR *pNMHDR,LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	int nSel; 
	ST_ERRCHECK * pData;
	nSel = m_ctrlReport.GetNextItem(-1,LVNI_SELECTED);
	if(nSel < 0)
		return; 

	pData = (ST_ERRCHECK *)m_ctrlReport.GetItemData(nSel);
	if(pData == nullptr || pData->lpData == nullptr)
		return; 

	switch(pData->btType)
	{
	case CHK_PATTERN_CNT:	// 패턴 자체 개수가 제한 개수보다 많을 때는 패턴 화면을 열어 패턴 중 가장 빠른 번호의 패턴을 가리키고 트리를 펼치도록 함
	case CHK_PATTERN_ITEM_CNT:
	{
		CDataPattern * pPtn = (CDataPattern*)pData->lpData;
		theApp.ViewFormSelectItem(FV_MAKEPATTERN,TTYPE_DEV_PATTERN,(DWORD_PTR)pPtn->GetPatternID());
	}
		break;
	case CHK_TEXT_CNT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_LEVEL_DUP:
		theApp.ViewFormSelectItem(FV_LOCATION,0,(DWORD_PTR)pData->lpData);
		break;
	case CHK_NOINPUT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_OUTPUT_CNT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_NOUSE_OUTPUT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_INPUT_WITHOUT_OUTPUT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_GAS_VALVE_REDUNDANCY:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY, SE_RELAY, (LPARAM)pData->lpData);
	}
	break;
	default:
		return;
	}
}


void CDlgErrorCheck::InsertErrorList(BYTE btType,int nErrorCnt,LPVOID lpData,CString strDesc)
{
	ST_ERRCHECK * pErr;
	CString strErr= L"",strType = L"";
	pErr = new ST_ERRCHECK;
	pErr->btType = btType;
	pErr->nErrorData = nErrorCnt;
	pErr->lpData = lpData;
	pErr->strDesc = strDesc;
	switch(btType)
	{
	case CHK_PATTERN_CNT:
		strErr = L"ERROR";
		strType = g_strErrChkText[btType];
		pErr->btError = 1;
		m_nErrorCnt++;
		break;
	case CHK_PATTERN_ITEM_CNT:
		strErr = L"ERROR";
		strType = g_strErrChkText[btType];
		pErr->btError = 1;
		m_nErrorCnt ++;
		break;
	case CHK_TEXT_CNT:
		strErr = L"WARNING";
		strType = g_strErrChkText[btType];
		pErr->btError = 0;
		m_nWarningCnt ++;
		//pErr->btError = 1;
		//m_nErrorCnt ++;
		break;
	case CHK_LEVEL_DUP:
		strErr = L"WARNING";
		strType = g_strErrChkText[btType];
		pErr->btError = 0;
		m_nWarningCnt ++;
		//pErr->btError = 1;
		//m_nErrorCnt ++;
		break;
	case CHK_NOINPUT:
		strErr = L"ERROR";
		strType = g_strErrChkText[btType];
		pErr->btError = 1;
		m_nErrorCnt ++;
		break;
	case CHK_OUTPUT_CNT:
		strErr = L"ERROR";
		strType = g_strErrChkText[btType];
		pErr->btError = 1;
		m_nErrorCnt ++;
		break;
	case CHK_NOUSE_OUTPUT:
		strErr = L"WARNING";
		strType = g_strErrChkText[btType];
		pErr->btError = 0;
		m_nWarningCnt ++;
		break;
	case CHK_INPUT_WITHOUT_OUTPUT:
		strErr = L"WARNING";
		strType = g_strErrChkText[btType];
		pErr->btError = 0;
		m_nWarningCnt ++;
		break;
	case CHK_GAS_VALVE_REDUNDANCY:
		strErr = L"ERROR";
		strType = g_strErrChkText[btType];
		pErr->btError = 1;
		m_nErrorCnt++;
		break;
	default:
		strErr = L"Unknown";
		strType = L"Unknown";
		strDesc = L"Unknown";
		pErr->btError = 1;
		m_nErrorCnt ++;
		break;
	}
	int nLast = m_ctrlReport.AddItem(strErr);
	m_ctrlReport.SetItemText(nLast,1,strType);
	m_ctrlReport.SetItemText(nLast,2,strDesc);
	m_ctrlReport.SetItemData(nLast,(LPARAM)pErr);
	m_ptrErrorList.AddTail(pErr);

}

void CDlgErrorCheck::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
// 	CWnd * pWnd = AfxGetMainWnd();
// 	if(pWnd && pWnd->GetSafeHwnd())
// 		pWnd->PostMessageW(UWM_ERRORCHECK_CLOSE,0,0);
}


void CDlgErrorCheck::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCnt = m_ctrlReport.GetItemCount();
	if(nCnt <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"오류 검색 결과가 없습니다.");
#else
		AfxMessageBox(L"No errors have been found.");
#endif
		return ; 
	}
	int nTempSize = 0;
	char szBuff[2048] = { 0 };
	CFile file;
	COleDateTime dtCur;
	CString strtemp,strPrjName,strPath,strLine;
	CString strDefault = L"*.csv";
#ifndef ENGLISH_MODE
	CString strFilter = L"오류 검사 결과(*.csv)|*.csv|All Files (*.*)|*.*||";
#else
	CString strFilter = L"Error check results(*.csv)|*.csv|All Files (*.*)|*.*||";
#endif
	strPrjName = m_pRefFasSysData->GetPrjName();
	dtCur = COleDateTime::GetCurrentTime();
#ifndef ENGLISH_MODE
	strtemp.Format(L"%s_오류검사결과(%4d%02d%02d).csv",strPrjName ,dtCur.GetYear(),dtCur.GetMonth(),dtCur.GetDay());
#else
	strtemp.Format(L"%s_Error_check_results(%4d%02d%02d).csv", strPrjName, dtCur.GetYear(), dtCur.GetMonth(), dtCur.GetDay());
#endif
	CFileDialog FileDialog(FALSE,NULL,strtemp,OFN_HIDEREADONLY,strFilter,this);
	if(FileDialog.DoModal() != IDOK)
		return;

	strPath = FileDialog.GetPathName();
	if(!file.Open(strPath,CFile::modeCreate | CFile::modeWrite))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
#ifndef ENGLISH_MODE
		GF_AddLog(L"오류검사결과 파일을 생성하는데 실패했습니다.\n");
		AfxMessageBox(L"오류검사결과 파일을 생성하는데 실패했습니다.");
#else
		GF_AddLog(L"Failed to create the error check file.\n");
		AfxMessageBox(L"Failed to create the error check file.");
#endif
		return;
	}

#ifndef ENGLISH_MODE
	strLine = L"#오류종류,오류분류,내용\n";
#else
	strLine = L"#errortype,errorclassification,content\n";
#endif
	nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,2048);
	file.Write(szBuff,nTempSize);
	for(int i = 0; i < nCnt; i ++)
	{
		strLine.Format(L"%s,%s,%s\n"
			,m_ctrlReport.GetItemText(i,0)
			,m_ctrlReport.GetItemText(i,1)
			,m_ctrlReport.GetItemText(i,2)
		);
		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,2048);
		file.Write(szBuff,nTempSize);
	}
	file.Close();
#ifndef ENGLISH_MODE
	strtemp.Format(L"다음 파일명으로\n[%s]\n저장되었습니다.",strPath);
#else
	strtemp.Format(L"The file has been saved as [%s].", strPath);
#endif
	AfxMessageBox(strtemp);
}
