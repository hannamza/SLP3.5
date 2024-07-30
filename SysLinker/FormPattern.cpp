// FormPattern.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormPattern.h"
#include "DataPattern.h"
#include "DataDevice.h"
#include "DataLinked.h"
#include "RelayTableData.h"
#include "ManagerPattern.h"
#include "DataEmBc.h"
#include "DataPump.h"
#include "DataFacpRelay.h"
#include	"../Common/Utils/YAdoDatabase.h"
#include <algorithm>

UINT ThreadDeletePattern(LPVOID pParam)
{
	CFormPattern* pFP = (CFormPattern*)pParam;
	pFP->m_bThreadSucceeded = TRUE;

	int nPtnID = 0;
	pFP->m_pEditItem = pFP->m_pCurItem;
	nPtnID = ((CDataPattern*)pFP->m_pEditItem->pData)->GetPatternID();
	BOOL bManualMake = ((CDataPattern*)pFP->m_pEditItem->pData)->GetManualMake();
	YAdoDatabase * pDb = pFP->m_pRefFasSysData->GetPrjDB();
	pDb->BeginTransaction();
	if (pFP->DeletePattern(pDb, nPtnID) <= 0)
	{
		pDb->RollbackTransaction();
		//AfxMessageBox(L"패턴을 삭제하는데 실패 했습니다.");
		CString strMsg = _T("");
		strMsg.Format(_T("Failed to delete the pattern [%s]."), ((CDataPattern*)pFP->m_pEditItem->pData)->GetPatternName());
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
		pFP->m_bThreadSucceeded = FALSE;
		return 0;
	}

	if (pFP->DeleteRelay(nPtnID, pDb) <= 0)
	{
		pDb->RollbackTransaction();
		//AfxMessageBox(L"패턴에 포함된 디바이스를 삭제하는데 실패 했습니다.");
		CString strMsg = _T("");
		strMsg.Format(_T("Failed to delete the output circuit included in the pattern [%s]."), ((CDataPattern*)pFP->m_pEditItem->pData)->GetPatternName());
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
		pFP->m_bThreadSucceeded = FALSE;
		return 0;
	}

	if (pFP->m_pRefFasSysData->DeleteLinkFromAllInput(LK_TYPE_PATTERN, nPtnID, 0, 0, 0) <= 0)
	{
		pDb->RollbackTransaction();
		//AfxMessageBox(L"패턴을 사용하는 입력에서 해당 정보를 삭제하는데 실패 했습니다.");
		CString strMsg = _T("");
		strMsg.Format(_T("Failed to delete input circuit information that uses the deleted pattern [%s] as output."), ((CDataPattern*)pFP->m_pEditItem->pData)->GetPatternName());
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
		pFP->m_bThreadSucceeded = FALSE;
		return 0;
	}
	pDb->CommitTransaction();

	//20240703 GBM start - 중계기 일람표 편집 기능 Disable
#ifdef MODULE_TABLE_UPDATE_MODE
	// 중계기 일람표 상의 패턴이고 GT1 수신기가 있다면 중계기 일람표 파일 수정
	if (!bManualMake)
	{
		BOOL bGT1Exist = FALSE;
		bGT1Exist = CNewExcelManager::Instance()->bExistPI && CNewExcelManager::Instance()->bExistFT && CNewExcelManager::Instance()->bExistUT && CNewExcelManager::Instance()->bExistEI;
		if (bGT1Exist)
		{
			BOOL bRet = FALSE;
			bRet = CNewExcelManager::Instance()->UpdatePatternInfo(-1, -1, -1, -1, PATTERN_DELETE, ((CDataPattern*)pFP->m_pEditItem->pData)->GetPatternName(), theApp.m_pFasSysData->m_strPrjName);
			if (!bRet)
			{
				CString strMsg = _T("");
				strMsg.Format(_T("Failed to delete pattern [%s] from the module table file."), ((CDataPattern*)pFP->m_pEditItem->pData)->GetPatternName());
				Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
				pFP->m_bThreadSucceeded = FALSE;
			}
		}
	}
#endif
	//20240703 GBM end

	pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(pFP->m_hThreadHandle);

	return 0;
}

UINT ThreadDeleteLinkedCircuitFromThePattern(LPVOID pParam)
{
	CFormPattern* pFP = (CFormPattern*)pParam;
	pFP->m_bThreadSucceeded = TRUE;

	int nCnt = -1;
	CString strSql;
	CDataLinked* plnk;
	CDataPattern* pPtn = (CDataPattern *)pFP->m_pCurItem->pData;
	if (pPtn == nullptr)
	{
		pFP->m_bThreadSucceeded = FALSE;
		pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
		SetEvent(pFP->m_hThreadHandle);
		return 0;
	}

	YAdoDatabase * pDB;
	pDB = pFP->m_pRefFasSysData->GetPrjDB();
	pDB->BeginTransaction();

	for (int i = 0; i < pFP->m_dataLinkedVec.size(); i++)
	{
		plnk = (CDataLinked*)pFP->m_dataLinkedVec[i];
		if (plnk == nullptr)
		{
			pFP->m_bThreadSucceeded = FALSE;
			pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
			SetEvent(pFP->m_hThreadHandle);
			return 0;
		}

		strSql.Format(L"SELECT * FROM TB_PATTERN_ITEM WHERE NET_ID=1 "
			L" AND PT_ID=%d AND ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d"
			L" AND CHN_ID=%d AND RLY_ID=%d"
			, pPtn->GetPatternID(), plnk->GetLinkType(), plnk->GetTgtFacp(), plnk->GetTgtUnit()
			, plnk->GetTgtChn(), plnk->GetTgtDev()
		);


		if (pDB->OpenQuery(strSql) == FALSE)
		{
			pFP->m_bThreadSucceeded = FALSE;
			pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
			SetEvent(pFP->m_hThreadHandle);
			return 0;
		}

		nCnt = pDB->GetRecordCount();
		if (nCnt == 0)
		{
			pFP->m_bThreadSucceeded = FALSE;
			pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
			SetEvent(pFP->m_hThreadHandle);
			return 0;
		}

		strSql.Format(L"DELETE FROM TB_PATTERN_ITEM WHERE NET_ID=1 "
			L" AND PT_ID=%d AND ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d"
			L" AND CHN_ID=%d AND RLY_ID=%d"
			, pPtn->GetPatternID(), plnk->GetLinkType(), plnk->GetTgtFacp(), plnk->GetTgtUnit()
			, plnk->GetTgtChn(), plnk->GetTgtDev()
		);

		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			pFP->m_bThreadSucceeded = FALSE;
			pDB->RollbackTransaction();
			pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
			SetEvent(pFP->m_hThreadHandle);
			return 0;
		}
	}

	pDB->CommitTransaction();

	//20240703 GBM start - 중계기 일람표 편집 기능 Disable
#ifdef MODULE_TABLE_UPDATE_MODE
	//20240604 GBM start - 해당 패턴이 중계기 일람표 상의 패턴이고 수신기 중 GT1 수신기가 있다면 중계기 일람표를 수정
	BOOL bManualMake = FALSE;
	bManualMake = pPtn->GetManualMake();
	if (!bManualMake)
	{
		BOOL bGT1TypeExist = FALSE;
		for (int i = 0; i < MAX_FACP_COUNT; i++)
		{
			if (CNewInfo::Instance()->m_gi.facpType[i] == GT1)
			{
				bGT1TypeExist = TRUE;
				break;
			}
		}

		if (bGT1TypeExist)
		{
			for (int i = 0; i < pFP->m_dataLinkedVec.size(); i++)
			{
				plnk = (CDataLinked*)pFP->m_dataLinkedVec[i];
				if (plnk == nullptr)
				{
					pFP->m_bThreadSucceeded = FALSE;
					pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
					SetEvent(pFP->m_hThreadHandle);
					return 0;
				}

				CString strPatternName = pPtn->GetPatternName();
				int nFacp = plnk->GetTgtFacp() - 1;
				int nUnit = plnk->GetTgtUnit() - 1;
				int nLoop = plnk->GetTgtChn() - 1;
				int nCircuit = plnk->GetTgtDev();

				if (!CNewExcelManager::Instance()->UpdatePatternInfo(nFacp, nUnit, nLoop, nCircuit, PATTERN_ITEM_DELETE, _T(""), pFP->m_pRefFasSysData->GetPrjName()))
				{
					pFP->m_bThreadSucceeded = FALSE;
					pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
					SetEvent(pFP->m_hThreadHandle);
				}
			}
		}
	}
	//20240604 GBM end
#endif
	//20240703 GBM end

	pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(pFP->m_hThreadHandle);

	return 0;
}

UINT ThreadAddLinkedCircuitToThePattern(LPVOID pParam)
{
	//
	CFormPattern* pFP = (CFormPattern*)pParam;
	pFP->m_bThreadSucceeded = TRUE;
	CDataPattern * pPtn;
	CDataLinked * plnk;

	pPtn = (CDataPattern *)pFP->m_pEditItem->pData;
	pFP->m_pRefFasSysData->ChangePattern(pPtn, pPtn->GetManualMake(), pFP->m_dataLinkedVec);

	//20240703 GBM start - 중계기 일람표 편집 기능 Disable
#ifdef MODULE_TABLE_UPDATE_MODE
	if (pPtn->GetManualMake() != 1)
	{
		int nLinked = pFP->m_dataLinkedVec.size();
		for (int i = 0; i < nLinked; i++)
		{
			plnk = pFP->m_dataLinkedVec[i];
			int nFacp = plnk->GetTgtFacp() - 1;
			int nUnit = plnk->GetTgtUnit() - 1;
			int nLoop = plnk->GetTgtChn() - 1;
			int nCircuit = plnk->GetTgtDev();

			if (!CNewExcelManager::Instance()->UpdatePatternInfo(nFacp, nUnit, nLoop, nCircuit, PATTERN_ITEM_ADD, pPtn->GetPatternName(), pFP->m_pRefFasSysData->GetPrjName()))
			{
				pFP->m_bThreadSucceeded = FALSE;
				pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
				SetEvent(pFP->m_hThreadHandle);
			}
		}
	}
#endif
	//20240703 GBM end

	pFP->m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(pFP->m_hThreadHandle);

	return 0;
}

// CFormPattern

IMPLEMENT_DYNCREATE(CFormPattern, CFormView)

#ifndef ENGLISH_MODE
CFormPattern::CFormPattern()
	: CFormView(IDD_FORMPATTERN)
	, m_strPatternName(_T(""))
	, m_strPatternCode(_T(""))
	, m_uPatternID(0)
	, m_nPtnCount(0)
{
	m_pCurItem = nullptr;
	m_pEditItem = nullptr;
	m_pRefFasSysData = nullptr;
	m_bAdd = FALSE;
	m_nManualMakeStatus = -1;
}
#else
CFormPattern::CFormPattern()
	: CFormView(IDD_FORMPATTERN_EN)
	, m_strPatternName(_T(""))
	, m_strPatternCode(_T(""))
	, m_uPatternID(0)
	, m_nPtnCount(0)
{
	m_pCurItem = nullptr;
	m_pEditItem = nullptr;
	m_pRefFasSysData = nullptr;
	m_bAdd = FALSE;
	m_nManualMakeStatus = -1;
}
#endif

CFormPattern::~CFormPattern()
{
}

void CFormPattern::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATTERN_TREE, m_ctrlPtnTree);
	DDX_Control(pDX, IDC_RELAY_LIST, m_ctrlRelayList);
	DDX_Text(pDX, IDC_ED_PTN_NAME, m_strPatternName);
	DDX_Text(pDX, IDC_ED_PTN_CODE, m_strPatternCode);
	DDX_Text(pDX, IDC_ED_PTN_ID, m_uPatternID);
	DDX_Control(pDX, IDC_CMB_PTN_TYPE, m_cmbPtnType);
	DDX_Text(pDX, IDC_ED_PTN_CNT, m_nPtnCount);
}

BEGIN_MESSAGE_MAP(CFormPattern, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_ADD, &CFormPattern::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFormPattern::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CFormPattern::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_RELAY_ADD, &CFormPattern::OnBnClickedBtnRelayAdd)
	ON_BN_CLICKED(IDC_BTN_RELAY_DEL, &CFormPattern::OnBnClickedBtnRelayDel)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	//ON_NOTIFY(TVN_ITEMCHANGED, IDC_PATTERN_TREE, &CFormPattern::OnTvnItemChangedPatternTree)
	ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE, OnTvnOutputDropedItem)
	ON_TVN_DROPED_ITEM(IDC_EMERGENCYVIEW_TREE, OnTvnEmergencyDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PUMPVIEW_TREE, OnTvnPumpDropedItem)
	ON_TVN_DROPED_ITEM(IDC_CONTACTFACP_TREE, OnTvnContactDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PSWITCHVIEW_TREE, OnTvnPSwitchDropedItem)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PATTERN_TREE, &CFormPattern::OnTvnSelchangedPatternTree)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_POPUP_PTNLIST_DEL, &CFormPattern::OnPopupPtnlistDel)
	ON_COMMAND(ID_ITEM_SELECT_ITEM, &CFormPattern::OnItemSelectItem)
	ON_BN_CLICKED(IDC_RD_MANAUL_MAKE,&CFormPattern::OnBnClickedRdManaulMake)
	ON_BN_CLICKED(IDC_RD_TABLE_MAKE,&CFormPattern::OnBnClickedRdTableMake)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFormPattern 진단입니다.

#ifdef _DEBUG
void CFormPattern::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormPattern::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormPattern 메시지 처리기입니다.


void CFormPattern::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pRefFasSysData = theApp.GetRelayTableData();

	if (m_pRefFasSysData != nullptr)
		m_pRefFasSysData->FillPatternTree(&m_ctrlPtnTree, &m_ptrItemList);
	if (AfxGetMainWnd())
	{
		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRelayList);
		
 		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRelayList);
 		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRelayList);
		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRelayList);
	}

	CBitmap bmp;
	if (!bmp.LoadBitmap(IDB_BMP_PATTERN_ICON))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), IDB_BMP_PATTERN_ICON);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ImgList.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ImgList.Add(&bmp, RGB(0, 255, 255));

	m_ctrlPtnTree.SetImageList(&m_ImgList, TVSIL_NORMAL);

#ifndef ENGLISH_MODE
	m_ctrlRelayList.InsertColumn(0, L"출력이름", LVCFMT_LEFT, 300);
	m_ctrlRelayList.InsertColumn(1, L"입력타입", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(2, L"출력타입", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(3, L"설비명", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(4, L"설비번호", LVCFMT_LEFT, 50);
	m_ctrlRelayList.InsertColumn(5, L"출력설명", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(6, L"위치", LVCFMT_LEFT, 150);
	m_ctrlRelayList.InsertColumn(7,L"출력종류",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(8,L"자동/수동",LVCFMT_LEFT,80);
#else
	m_ctrlRelayList.InsertColumn(0, L"OUTPUT NAME", LVCFMT_LEFT, 300);
	m_ctrlRelayList.InsertColumn(1, L"INPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(2, L"OUTPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(3, L"EQUIPMENT NAME", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(4, L"EQUIPMENT NO.", LVCFMT_LEFT, 50);
	m_ctrlRelayList.InsertColumn(5, L"OUTPUT DESCRIPTION", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(6, L"LOCATION", LVCFMT_LEFT, 150);
	m_ctrlRelayList.InsertColumn(7, L"OUTPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelayList.InsertColumn(8, L"AUTO/MANUAL", LVCFMT_LEFT, 80);
#endif
	m_ctrlRelayList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	int i = 0; 
	for (i = 0; i < PTN_COUNT; i++)
	{
		m_cmbPtnType.InsertString(i,g_szPatternTypeString[i]);
		m_cmbPtnType.SetItemData(i ,i );
	}
	theApp.SetFormViewInitComplete(FV_MAKEPATTERN);
}


void CFormPattern::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rc , rcTree,rcList;
	GetClientRect(&rc);
	rc.DeflateRect(5, 8, 5, 5);

	rcTree = rc;
	rcTree.right = rc.left + 300;

	rcList = rc;
	rcList.top = 142;
	rcList.left = rcTree.right + 5;
	if (m_ctrlPtnTree.GetSafeHwnd())
		m_ctrlPtnTree.MoveWindow(&rcTree);
	
	if (m_ctrlRelayList.GetSafeHwnd())
		m_ctrlRelayList.MoveWindow(&rcList);
}


void CFormPattern::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AddInit();

}


void CFormPattern::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	int nSel = -1 , nType = PTN_PATTERN;
	nSel = m_cmbPtnType.GetCurSel();
	if (nSel < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"선택된 패턴 타입이 없습니다.");
#else
		AfxMessageBox(L"No pattern type selected.");
#endif
		return;
	}
	nType = (int)m_cmbPtnType.GetItemData(nSel);

	if (m_strPatternName.GetLength() <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"패턴 이름이 없습니다.");
#else
		AfxMessageBox(L"The pattern name doesn't exist.");
#endif
		return;
	}
	if(m_nManualMakeStatus == -1)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"패턴을 수동으로 생성했는지 일람표로 생성했는지 체크되지 않았습니다.\n'수동으로 생성' 또는 '일람표로 생성'을 선택해 주시기 바랍니다.");
#else
		AfxMessageBox(L"You have not checked whether the pattern was created manually or from a table.\nPlease select 'Create Manually' or 'Create from Table'.");
#endif
		return;
	}

	if (m_bAdd)
		AddDatabasePattern(m_uPatternID, m_strPatternName, nType, m_strPatternCode);
	else
	{
		SaveDatabasePattern(m_pCurItem, m_strPatternName, nType,m_nManualMakeStatus);
	}

	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_DKP_PATTERN_REFRESH, DATA_ALL, 0);

}


void CFormPattern::OnBnClickedBtnDel()
{
	//20240603 GBM start - 스레드로 전환하고 중계기 일람표 상의 패턴이라면 중계기 일람표에 적용
#if 1
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

 	if (m_pRefFasSysData == nullptr)
		return ;
	if (m_pCurItem == nullptr || m_pCurItem->pData == nullptr)
		return;

	if (m_pCurItem->nDataType == PTN_PATTERN)
		return; 
#ifndef ENGLISH_MODE
	if(AfxMessageBox(L"선택된 패턴을 삭제하시겠습니까?" , MB_YESNO) == IDNO)
		return;
#else
	if (AfxMessageBox(L"Do you want to delete the selected pattern?", MB_YESNO) == IDNO)
		return;
#endif

	UpdateData();

	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("패턴을 삭제하는 중입니다. 잠시 기다려 주세요.");
#else
	CString strMsg = _T("Deleting the pattern. Wait for a moment.");
#endif
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadDeletePattern, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	m_pProgressBarDlg = nullptr;

	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_DKP_PATTERN_REFRESH, DATA_ALL, 0);

	if (m_bThreadSucceeded)
	{
		CString strMsg = _T("");
		strMsg.Format(_T("The pattern [%s] was successfully deleted."), ((CDataPattern*)m_pEditItem->pData)->GetPatternName());
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
#ifndef ENGLISH_MODE
		AfxMessageBox(L"패턴을 삭제하는데 성공했습니다.");
#else
		AfxMessageBox(L"Successfully deleted the pattern.");
#endif
	}
	else
	{
		CString strMsg = _T("");
		strMsg.Format(_T("Failed to delete the pattern [%s]."), ((CDataPattern*)m_pEditItem->pData)->GetPatternName());
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
#ifndef ENGLISH_MODE
		AfxMessageBox(L"패턴을 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pattern.");
#endif
	}

	m_ctrlPtnTree.DeleteItem(m_pEditItem->hItem);
	m_pRefFasSysData->DeletePatternInMemory((CDataPattern*)m_pEditItem->pData);
	POSITION pos = m_ptrItemList.Find(m_pEditItem);
	if (pos)
		m_ptrItemList.RemoveAt(pos);

	delete m_pEditItem;
	InitView();
	//
	
#else
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CPtrList ptrList;
	int nPtnID = 0;
	if (m_pRefFasSysData == nullptr)
		return;
	if (m_pCurItem == nullptr || m_pCurItem->pData == nullptr)
		return;

	if (m_pCurItem->nDataType == PTN_PATTERN)
		return;
	if (AfxMessageBox(L"선택된 패턴을 삭제 하시겠습니까?", MB_YESNO) == IDNO)
		return;
	m_pEditItem = m_pCurItem;
	nPtnID = ((CDataPattern*)m_pEditItem->pData)->GetPatternID();
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	pDb->BeginTransaction();
	if (DeletePattern(pDb, nPtnID) <= 0)
	{
		pDb->RollbackTransaction();
		AfxMessageBox(L"패턴을 삭제하는데 실패 했습니다.");
		return;
	}

	if (DeleteRelay(nPtnID, pDb) <= 0)
	{
		pDb->RollbackTransaction();
		AfxMessageBox(L"패턴에 포함된 디바이스를 삭제하는데 실패 했습니다.");
		return;
	}

	if (m_pRefFasSysData->DeleteLinkFromAllInput(LK_TYPE_PATTERN, nPtnID, 0, 0, 0) <= 0)
	{
		pDb->RollbackTransaction();
		AfxMessageBox(L"패턴을 사용하는 입력에서 해당 정보를 삭제하는데 실패 했습니다.");
		return;
	}
	pDb->CommitTransaction();

	m_ctrlPtnTree.DeleteItem(m_pEditItem->hItem);
	m_pRefFasSysData->DeletePatternInMemory((CDataPattern*)m_pEditItem->pData);
	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_DKP_PATTERN_REFRESH, DATA_ALL, 0);
	POSITION pos = m_ptrItemList.Find(m_pEditItem);
	if (pos)
		m_ptrItemList.RemoveAt(pos);
	//delete (CDataPattern *)m_pCurItem->pData;
	delete m_pEditItem;
	m_pEditItem = nullptr;
	m_pCurItem = nullptr;
	InitView();

	AfxMessageBox(L"패턴을 삭제하는데 성공했습니다.");
#endif
	//20240603 GBM end
}


void CFormPattern::OnBnClickedBtnRelayAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AfxMessageBox(L"구현중");
}


void CFormPattern::OnBnClickedBtnRelayDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	CDataPattern * pPtn; 
// 	CDataLinked * plnk;
// 	//CDataDevice * pDev;
// 	CPtrList * pList;
// 	CString strSql;
// 	POSITION pos = nullptr;
// 
// 	if (m_pRefFasSysData == nullptr)
// 		return;
// 
// 	if (m_pCurItem == nullptr)
// 		return;
// 	int nSel; 
// 	nSel = m_ctrlRelayList.GetNextItem(-1, LVNI_SELECTED);
// 	if (nSel < 0)
// 		return; 
// 
// 	pPtn = (CDataPattern *)m_pCurItem->pData;
// 	if (pPtn == nullptr)
// 		return; 
// 	plnk = (CDataLinked *)m_ctrlRelayList.GetItemData(nSel);
// 	if (plnk == nullptr)
// 		return; 
// 	
// 	pList = pPtn->GetPtnItemList();
// 	
// 	pos = pList->Find(plnk);
// 	if (pos == nullptr)
// 		return; 
// 
// 	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
// 	if (pDB == nullptr)
// 		return; 
// 
// 	strSql.Format(L"DELETE FROM TB_PATTERN_ITEM WHERE NET_ID=1 "
// 		L" AND PT_ID=%d AND ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d"
// 		L" AND CHN_ID=%d AND RLY_ID=%d"
// 		, pPtn->GetPatternID(),plnk->GetLinkType(), plnk->GetTgtFacp(), plnk->GetTgtUnit()
// 		, plnk->GetTgtChn() , plnk->GetTgtDev()
// 	);
// 
// 	if (pDB->ExecuteSql(strSql) == FALSE)
// 	{
// 		AfxMessageBox(L"삭제하는데 실패 했습니다.");
// 		return; 
// 	}
// 	pList->RemoveAt(pos);
// 	m_ctrlRelayList.DeleteItem(nSel);
// 	if (AfxGetMainWnd())
// 		AfxGetMainWnd()->SendMessage(UWM_DKP_PATTERN_REFRESH, DATA_ALL, 0);



	//////////////////////////////////////////////////////////////////////////

	//20240604 GBM start - 스레드로 전환
#if 1
	CString str;
	int nRet = 0, nCnt, i;
	CDataPattern * pPtn;
	BOOL bError = FALSE;
	CString strSql, strError = L"";
	CDataLinked * plnk;
	POSITION pos;
	std::vector<int> vtSel;

	if (m_pCurItem == nullptr)
		return;
	int nSel;
	nSel = m_ctrlRelayList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
		return;

	pPtn = (CDataPattern *)m_pCurItem->pData;
	if (pPtn == nullptr)
		return;

	nCnt = m_ctrlRelayList.GetSelectedCount();
	if (nCnt < 0)
		return;

#ifndef ENGLISH_MODE
	if (AfxMessageBox(L"패턴 항목을 삭제하시겠습니까?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
#else
	if (AfxMessageBox(L"Do you want to delete the pattern item?", MB_YESNO | MB_ICONQUESTION) == IDNO)
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

	m_dataLinkedVec.clear();
	pos = m_ctrlRelayList.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nSelected = m_ctrlRelayList.GetNextSelectedItem(pos);
		vtSel.push_back(nSelected);
	}
	sort(vtSel.begin(), vtSel.end());

	for (int i = 0; i < vtSel.size(); i++)
	{
		plnk = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		m_dataLinkedVec.push_back(plnk);
	}

	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("선택된 출력 회로를 패턴에서 삭제하는 중입니다.\n                       잠시 기다려 주세요.");
#else
	CString strMsg = _T("Deleting the selected output circuit from the pattern.\n                        Wait for a moment.");
#endif
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadDeleteLinkedCircuitFromThePattern, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	m_pProgressBarDlg = nullptr;

	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		plnk = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		if (plnk == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
			strError = L"삭제하는데 실패했습니다. 연동 출력에 대한 정보를 가져오는데 실패했습니다.";
			bError = TRUE;
			break;;
		}
		pPtn->DeleteItemPtr(plnk);
		delete plnk;	//20240604 GBM - 메모리 누수 수정
		m_ctrlRelayList.DeleteItem(vtSel[i]);
	}
	
#ifndef ENGLISH_MODE
	if (m_bThreadSucceeded)
		AfxMessageBox(L"연동 출력 정보를 삭제하는데 성공했습니다.");
	else
		AfxMessageBox(L"연동 출력 정보를 삭제하는데 실패했습니다.");
#else
	if (m_bThreadSucceeded)
		AfxMessageBox(L"Successfully deleted the interlock output information.");
	else
		AfxMessageBox(L"Failed to delete the interlock output information.");
#endif

	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_DKP_PATTERN_REFRESH, DATA_ALL, 0);
#else
	CString str;
	int nRet = 0, nCnt, i;
	CDataPattern * pPtn;
	BOOL bError = FALSE;
	CString strSql, strError = L"";
	CDataLinked * plnk;
	POSITION pos;
	std::vector<int> vtSel;
	YAdoDatabase * pDB;

	if (m_pCurItem == nullptr)
	return;
	int nSel;
	nSel = m_ctrlRelayList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
		return;

	pPtn = (CDataPattern *)m_pCurItem->pData;
	if (pPtn == nullptr)
	return;

	nCnt = m_ctrlRelayList.GetSelectedCount();
	if (nCnt < 0)
		return;

	if (AfxMessageBox(L"패턴 항목을 삭제하시겠습니까?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	return;

	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"삭제하는데 실패 했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
		return;
	}

	pDB = m_pRefFasSysData->GetPrjDB();
	pos = m_ctrlRelayList.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nSelected = m_ctrlRelayList.GetNextSelectedItem(pos);
		vtSel.push_back(nSelected);
	}
	sort(vtSel.begin(), vtSel.end());

	pDB->BeginTransaction();

	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		plnk = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		if (plnk == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
			strError = L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.";
			bError = TRUE;
			break;;
		}

		strSql.Format(L"SELECT * FROM TB_PATTERN_ITEM WHERE NET_ID=1 "
			L" AND PT_ID=%d AND ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d"
			L" AND CHN_ID=%d AND RLY_ID=%d"
			, pPtn->GetPatternID(), plnk->GetLinkType(), plnk->GetTgtFacp(), plnk->GetTgtUnit()
			, plnk->GetTgtChn(), plnk->GetTgtDev()
		);


		if (pDB->OpenQuery(strSql) == FALSE)
		{
			strError = L"삭제하는데 실패 했습니다. 데이터베이스에서 패턴항목을 가져오는데 실패했습니다.";
			bError = TRUE;
			break;;
		}

		nCnt = pDB->GetRecordCount();
		if (nCnt == 0)
			continue;

		strSql.Format(L"DELETE FROM TB_PATTERN_ITEM WHERE NET_ID=1 "
			L" AND PT_ID=%d AND ITEM_TYPE=%d AND FACP_ID=%d AND UNIT_ID=%d"
			L" AND CHN_ID=%d AND RLY_ID=%d"
			, pPtn->GetPatternID(), plnk->GetLinkType(), plnk->GetTgtFacp(), plnk->GetTgtUnit()
			, plnk->GetTgtChn(), plnk->GetTgtDev()
		);

		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			strError = L"삭제하는데 실패 했습니다. 데이터베이스에서 패턴항목을 삭제하는데 실패했습니다.";
			bError = TRUE;
			break;;
		}
	}

	if (bError == TRUE)
	{
		pDB->RollbackTransaction();
		AfxMessageBox(strError);
		return;
	}

	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		plnk = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		if (plnk == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
			strError = L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.";
			bError = TRUE;
			break;;
		}
		pPtn->DeleteItemPtr(plnk);
		delete plnk;	//20240604 GBM - 메모리 누수 수정
		m_ctrlRelayList.DeleteItem(vtSel[i]);
	}
	pDB->CommitTransaction();
	AfxMessageBox(L"연동 출력 정보를 삭제하는데 성공했습니다");
	if (AfxGetMainWnd())
	AfxGetMainWnd()->SendMessage(UWM_DKP_PATTERN_REFRESH, DATA_ALL, 0);
#endif
	//20240604 GBM end

}


BOOL CFormPattern::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::PreCreateWindow(cs);
}


void CFormPattern::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//RemoveAllTreeData();
	if (AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRelayList);

// 		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRelayList);
// 		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRelayList);
// 		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_DEL, (LPARAM)&m_ctrlRelayList);
		m_pRefFasSysData = theApp.GetRelayTableData();
	}

	CFormView::OnClose();
}


int CFormPattern::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
// 
// 
// void CFormPattern::OnTvnItemChangedPatternTree(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	HTREEITEM hItem = m_ctrlPtnTree.GetSelectedItem();
// 	if (hItem == nullptr)
// 		return; 
// 
// 	AddCancel();
// 	DisplayPattern(hItem);
// 
// 	*pResult = 0;
// }
// 

int CFormPattern::DisplayPattern(HTREEITEM hItem)
{
	m_ctrlRelayList.SetRedraw(FALSE);
	m_ctrlRelayList.DeleteAllItems();
	if (hItem == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return 0;
	}

	ST_TREEITEM * pItem = (ST_TREEITEM*)m_ctrlPtnTree.GetItemData(hItem);
	if (pItem == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return 0;
	}

	if (pItem->nDataType != TTYPE_DEV_PATTERN)
	{
		m_ctrlRelayList.SetRedraw();
		return 0;
	}

	if (pItem->pData == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return 0;
	}

	m_pCurItem = pItem;

	int nType , nIdx = 0 ;
	CString str;
	COLORREF crBk,crText;
	CDataEmBc * pEm;
	CDataPump * pPm;
	CDataLinked * plnk;
	CDataDevice * pDev;
	CDataPattern * pTemp;
	CDataPattern * pPtn = (CDataPattern*)pItem->pData;
	m_strPatternName = pPtn->GetPatternName();
	m_uPatternID = pPtn->GetPatternID();
	nType = pPtn->GetPatternType();
	SetTypeCombo((int)nType);
	CPtrList * pList = pPtn->GetPtnItemList();
	if (pList == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return 0;
	}
	m_nPtnCount = pList->GetCount();
	POSITION pos;
	pos = pList->GetHeadPosition();
	while (pos)
	{
		plnk = (CDataLinked *)pList->GetNext(pos);
		if (plnk == nullptr)
			continue; 

		switch (plnk->GetLinkType())
		{
		case LK_TYPE_PATTERN:
			pTemp = m_pRefFasSysData->GetPattern(plnk->GetTgtFacp());
			if (pTemp == nullptr)
				continue;
			m_ctrlRelayList.InsertItem(nIdx, pTemp->GetPatternName());
			m_ctrlRelayList.SetItemText(nIdx, 1, L"");
			m_ctrlRelayList.SetItemText(nIdx, 2, L"");
			m_ctrlRelayList.SetItemText(nIdx, 3, L"");
			m_ctrlRelayList.SetItemText(nIdx, 4, L"");
			m_ctrlRelayList.SetItemText(nIdx, 5, L"");
			m_ctrlRelayList.SetItemText(nIdx, 6, L"");
#ifndef ENGLISH_MODE
			m_ctrlRelayList.SetItemText(nIdx,7,L"패턴");
#else
			m_ctrlRelayList.SetItemText(nIdx, 7, L"PATTERN");
#endif
			//m_ctrlRelayList.SetItemText(nIdx,8,L"-");
			m_ctrlRelayList.SetItemData(nIdx, (DWORD_PTR)plnk);
			nIdx++;
			break;
		case LK_TYPE_EMERGENCY:
			pEm = m_pRefFasSysData->GetEmergency(plnk->GetTgtFacp());
			if (pEm == nullptr)
				continue;
			m_ctrlRelayList.InsertItem(nIdx, pEm->GetEmName());
			m_ctrlRelayList.SetItemText(nIdx, 1, L"");
			m_ctrlRelayList.SetItemText(nIdx, 2, L"");
			m_ctrlRelayList.SetItemText(nIdx, 3, L"");
			m_ctrlRelayList.SetItemText(nIdx, 4, L"");
			m_ctrlRelayList.SetItemText(nIdx, 5, L"");
			m_ctrlRelayList.SetItemText(nIdx, 6, L"");
#ifndef ENGLISH_MODE
			m_ctrlRelayList.SetItemText(nIdx, 7, L"비상방송");
#else
			m_ctrlRelayList.SetItemText(nIdx, 7, L"비상방송");
#endif
			m_ctrlRelayList.SetItemData(nIdx, (DWORD_PTR)plnk);
			nIdx++;
			break;
		case LK_TYPE_PUMP:
			pPm = m_pRefFasSysData->GetPump(plnk->GetTgtFacp(), plnk->GetTgtUnit());
			if (pPm == nullptr)
				continue;
			m_ctrlRelayList.InsertItem(nIdx, pPm->GetPumpName());
			m_ctrlRelayList.SetItemText(nIdx, 1, L"");
			m_ctrlRelayList.SetItemText(nIdx, 2, L"");
			m_ctrlRelayList.SetItemText(nIdx, 3, L"");
			m_ctrlRelayList.SetItemText(nIdx, 4, L"");
			m_ctrlRelayList.SetItemText(nIdx, 5, L"");
			m_ctrlRelayList.SetItemText(nIdx, 6, L"");
#ifndef ENGLISH_MODE
			m_ctrlRelayList.SetItemText(nIdx, 7, L"펌프");
#else
			m_ctrlRelayList.SetItemText(nIdx, 7, L"PUMP");
#endif
			m_ctrlRelayList.SetItemData(nIdx, (DWORD_PTR)plnk);
			nIdx++;
			break;
		case LK_TYPE_RELEAY:
			pDev = m_pRefFasSysData->GetDeviceByID(plnk->GetTgtFacp(), plnk->GetTgtUnit(), plnk->GetTgtChn(), plnk->GetTgtDev());
			if (pDev == nullptr)
				continue;
			if(plnk->GetLogicType() == LOGIC_MANUAL)
			{
#ifndef ENGLISH_MODE
				str = L"수동";
#else
				str = L"MANUAL";
#endif
				crBk = RGB(255,0,0);
				crText = RGB(255,255,255);
			}
			else
			{
#ifndef ENGLISH_MODE
				str = L"자동";
#else
				str = L"AUTO";
#endif
				//m_ctrlRelayList.SetCo
				crBk = RGB(255,255,255);
				crText = RGB(0,0,0);
			}
			m_ctrlRelayList.InsertItem(nIdx, pDev->GetOutContentsFullName(),crText,crBk);
			m_ctrlRelayList.SetItemText(nIdx, 1, pDev->GetInputTypeName(),crText,crBk);
			m_ctrlRelayList.SetItemText(nIdx, 2, pDev->GetOutputTypeName(),crText,crBk);
			m_ctrlRelayList.SetItemText(nIdx, 3, pDev->GetEquipName(),crText,crBk);
			m_ctrlRelayList.SetItemText(nIdx, 4, pDev->GetEqAddIndex(),crText,crBk);
			m_ctrlRelayList.SetItemText(nIdx, 5, pDev->GetOutContentsName(),crText,crBk);
			m_ctrlRelayList.SetItemText(nIdx, 6, pDev->GetOutputLocationName(),crText,crBk);
#ifndef ENGLISH_MODE
			m_ctrlRelayList.SetItemText(nIdx, 7, L"감지기/중계기",crText,crBk);
#else
			m_ctrlRelayList.SetItemText(nIdx, 7, L"DETECTOR/MODULE", crText, crBk);
#endif
			m_ctrlRelayList.SetItemText(nIdx, 8,str,crText,crBk);
			m_ctrlRelayList.SetItemData(nIdx,(DWORD_PTR)plnk);
			nIdx++;
			break;
		default:
			continue;
		}
	}

	((CButton*)GetDlgItem(IDC_RD_MANAUL_MAKE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RD_TABLE_MAKE))->SetCheck(0);

	if(pPtn->GetManualMake() == 1)
	{
		m_nManualMakeStatus = 1;
		((CButton*)GetDlgItem(IDC_RD_MANAUL_MAKE))->SetCheck(1);
	}
	else if(pPtn->GetManualMake() == 0)
	{
		m_nManualMakeStatus = 0;
		((CButton*)GetDlgItem(IDC_RD_TABLE_MAKE))->SetCheck(1);
	}
	else
	{
		m_nManualMakeStatus = -1;
#ifndef ENGLISH_MODE
		AfxMessageBox(L"패턴을 수동으로 생성했는지 일람표로 생성했는지 체크되지 않았습니다.\n'수동으로 생성' 또는 '일람표로 생성'을 선택해 주시기 바랍니다.");
#else
		AfxMessageBox(L"You have not checked whether the pattern was created manually or from a table.\nPlease select 'Create Manually' or 'Create from Table'.");
#endif
	}

	m_ctrlRelayList.SetRedraw();
	UpdateData(FALSE);
	return 1;
}

void CFormPattern::AddInit()
{
	m_bAdd = TRUE;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(FALSE);
	m_ctrlRelayList.DeleteAllItems();
	m_pCurItem = nullptr;
	m_pEditItem = nullptr;
	m_cmbPtnType.SetCurSel(PTN_CUSTOM);
	m_uPatternID = GetWholePatternID();
	if (m_uPatternID <= 0)
		m_uPatternID = 1;
	
	m_strPatternCode.Format(L"%03d", m_uPatternID);
	m_strPatternName = L"";
	m_cmbPtnType.SetCurSel(-1);
	UpdateData(FALSE);
}

void CFormPattern::InitView()
{
	m_ctrlRelayList.DeleteAllItems();
	m_pCurItem = nullptr;
	m_pEditItem = nullptr;
	m_cmbPtnType.SetCurSel(-1);
	m_uPatternID = 0;
	m_strPatternName = L"";
	m_cmbPtnType.SetCurSel(-1);
	UpdateData(FALSE);
}


void CFormPattern::AddCancel()
{
	m_bAdd = FALSE;
	//m_pEditItem = nullptr;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(TRUE);
}


int CFormPattern::RemoveAllTreeData()
{
	if (m_ctrlRelayList.GetSafeHwnd())
		m_ctrlRelayList.DeleteAllItems();

	if (m_ctrlPtnTree.GetSafeHwnd())
		m_ctrlPtnTree.DeleteAllItems();
	ST_TREEITEM * pItem; 
	while (!m_ptrItemList.IsEmpty())
	{
		pItem = (ST_TREEITEM *)m_ptrItemList.RemoveHead();
		if (pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
	return 1;
}


int CFormPattern::AddDatabasePattern(UINT nID , CString strPattern , int nType , CString strCode)
{
	//CDataDevice * pDev;
	CDataLinked *plnk;
	CDataPattern * pPtn; 
	CPtrList ptrList;
	if (m_pRefFasSysData == nullptr)
		return -1;

	int i = 0 , nCnt; 
	nCnt = m_ctrlRelayList.GetItemCount();
	for (i = 0; i < nCnt; i++)
	{
		plnk = (CDataLinked*)m_ctrlRelayList.GetItemData(i);
		if (plnk == nullptr)
			continue; 
		ptrList.AddTail(plnk);
	}
	pPtn = m_pRefFasSysData->AddNewPattern(nID, strPattern, nType,1, &ptrList);
	ChangeTreeItem(DATA_ADD, pPtn->GetPatternType(), pPtn);
	AddCancel();
#ifndef ENGLISH_MODE
	AfxMessageBox(L"패턴을 추가하는데 성공했습니다.");
#else
	AfxMessageBox(L"Successfully added a pattern.");
#endif
	return 1;
}


int CFormPattern::SaveDatabasePattern(ST_TREEITEM * pItem, CString strPattern, int nType,int nManualMakeStatuse)
{
	//20240604 GBM start - 스레드로 전환
#if 1
	CDataLinked * plnk;
	CDataPattern * pPtn;
	int i = 0, nCnt;
	nCnt = m_ctrlRelayList.GetItemCount();
	m_dataLinkedVec.clear();
	for (i = 0; i < nCnt; i++)
	{
		plnk = (CDataLinked*)m_ctrlRelayList.GetItemData(i);
		if (plnk == nullptr)
			continue;
		m_dataLinkedVec.push_back(plnk);
	}

	if (m_pRefFasSysData == nullptr)
		return -1;
	if (m_pCurItem == nullptr)
		return -1;

	m_pEditItem = m_pCurItem;
	pPtn = (CDataPattern*)m_pEditItem->pData;
	pPtn->SetPatternName(strPattern);
	pPtn->SetPatternType(nType);
	pPtn->SetManualMake(nManualMakeStatuse);

	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("수정된 패턴 정보를 저장하는 중입니다. 잠시 기다려 주세요.");
#else
	CString strMsg = _T("Saving the edited pattern information. Wait for a moment.");
#endif
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadAddLinkedCircuitToThePattern, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	m_pProgressBarDlg = nullptr;

	pPtn = (CDataPattern*)m_pEditItem->pData;
	ChangeTreeItem(DATA_SAVE, pPtn->GetPatternType(), pPtn);
#ifndef ENGLISH_MODE
	AfxMessageBox(L"패턴을 저장하는데 성공했습니다.");
#else

	AfxMessageBox(L"Successfully saved the pattern.");
#endif
	m_pEditItem = nullptr;
	return 1;
#else
	CDataLinked * plnk;
	//CDataDevice * pDev;
	CDataPattern * pPtn;
	CPtrList ptrList;
	CString strSql;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if (m_pRefFasSysData == nullptr)
		return -1;
	if (m_pCurItem == nullptr)
		return -1;
	m_pEditItem = m_pCurItem;
	int i = 0, nCnt;
	nCnt = m_ctrlRelayList.GetItemCount();
	for (i = 0; i < nCnt; i++)
	{
		plnk = (CDataLinked*)m_ctrlRelayList.GetItemData(i);
		if (plnk == nullptr)
			continue;
		ptrList.AddTail(plnk);
	}
	pPtn = (CDataPattern*)m_pEditItem->pData;
	pPtn->SetPatternName(strPattern);
	pPtn->SetPatternType(nType);
	pPtn->SetManualMake(nManualMakeStatuse);

	pPtn = m_pRefFasSysData->ChangePattern((CDataPattern*)m_pEditItem->pData, nManualMakeStatuse, &ptrList);
	// 	pPtn->SetPatternName(strPattern);
	// 	pPtn->SetPatternType(nType);
	// 	pPtn->SetManualMake(nManualMakeStatuse);
	ChangeTreeItem(DATA_SAVE, pPtn->GetPatternType(), pPtn);
	AfxMessageBox(L"패턴을 저장하는데 성공했습니다.");
	m_pEditItem = nullptr;
	return 1;
#endif
	//20240604 GBM end
}


int CFormPattern::DeletePattern(YAdoDatabase * pDB, int nPtnID)
{
	//UpdateData();		//스레드 변경으로 주석처리
	CString strSql;
	strSql.Format(L"DELETE FROM TB_PATTERN_MST WHERE PT_ID=%d" , nPtnID);
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		return 0; 
	}

	return 1;
}

int CFormPattern::AddRelay()
{
	return 0;
}

int CFormPattern::DeleteRelay(UINT nID, YAdoDatabase * pDB)
{
	CString strSql;
	strSql.Format(L"DELETE FROM TB_PATTERN_ITEM WHERE NET_ID=1 "
		L" AND PT_ID=%d "
		, nID
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		return 0;
	}

	return 1;
}


int CFormPattern::GetWholePatternID()
{
	if (m_pRefFasSysData == nullptr)
		return -1;


	std::shared_ptr<CManagerPattern> spMng = nullptr;
	spMng = m_pRefFasSysData->GetPatternManager();
	if (spMng == nullptr)
		return -1;

	return spMng->GetWholePatternID();
}


int CFormPattern::ChangeTreeItem(int nAddType , int nType, CDataPattern* pPtn)
{
	if (pPtn == nullptr)
		return 0; 

	HTREEITEM hParent = TVI_ROOT , hItem;
	int nTemp = PTN_PATTERN;
	ST_TREEITEM * pItem;
	HTREEITEM hChild = m_ctrlPtnTree.GetChildItem(TVI_ROOT);
	while (hChild != NULL)
	{
		if (hChild == TVI_ROOT)
			nTemp = 0;
		else
		{
			pItem = (ST_TREEITEM *)m_ctrlPtnTree.GetItemData(hChild);
			if (pItem == nullptr)
				continue; 
			if (pItem->nDataType == PTN_PATTERN)
				nTemp = (int)pItem->pData;
			else
				nTemp = PTN_PATTERN;
			//nTemp = (int)m_ctrlPtnTree.GetItemData(hChild);
		}
		if (nType == nTemp)
		{
			hParent = hChild;
			break;
		}
		hChild = m_ctrlPtnTree.GetNextSiblingItem(hChild);
	}
	if (nAddType == DATA_ADD)
	{
		pItem = new ST_TREEITEM;
		nType = pPtn->GetPatternType();
		hItem = m_ctrlPtnTree.InsertItem(pPtn->GetPatternName(), PTN_PATTERN, PTN_PATTERN, hParent);
		pItem->hItem = hItem;
		pItem->hParent = hParent;
		pItem->nDataType = TIMG_DEV_PATTERN;
		pItem->pData = pPtn;
		m_ctrlPtnTree.SetItemData(hItem, (DWORD_PTR)pItem);
		m_ptrItemList.AddTail(pItem);
		m_pCurItem = pItem;
	}
	else
	{
		if (m_pCurItem == nullptr)
			m_pCurItem = FindTreeItem((int)pPtn->GetPatternID(), pPtn->GetPatternType(), pPtn->GetPatternName());

		if (m_pCurItem == nullptr)
			return 0; 
		m_ctrlPtnTree.SetItemText(m_pCurItem->hItem, pPtn->GetPatternName());
	}
	m_ctrlPtnTree.RedrawWindow();
	return 1;
}


ST_TREEITEM* CFormPattern::FindTreeItem(int nID , int nType ,CString strName)
{
	CDataPattern * pPtn; 
	POSITION pos;
	ST_TREEITEM * pItem; 
	pos = m_ptrItemList.GetHeadPosition();
	while (pos)
	{
		pItem = (ST_TREEITEM *)m_ptrItemList.GetNext(pos);
		if (pItem == nullptr)
			continue; 
		if (pItem->nDataType != TIMG_DEV_PATTERN)
			continue; 

		pPtn = (CDataPattern *)pItem->pData;
		if (pPtn == nullptr)
			continue; 

		if (pPtn->GetPatternID() == nID)
			return pItem;
	}

	return nullptr;
}

void CFormPattern::OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	CDataPattern * pPtn;
	CDataDevice * pDev;
	CDataLinked * plnk;
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataSystem * pSys;
	CPtrList * pList;
	CString strSql;
	COLORREF crText=RGB(255,255,255),crBk=RGB(255,0,0);
	POSITION pos = nullptr;
	int i = 0, nIdx = 0, nFind = -1;
	YAdoDatabase * pDB = nullptr; //m_pRefFasSysData->GetPrjDB();
	m_ctrlRelayList.SetRedraw(FALSE);
	//m_ctrlRelayList.DeleteAllItems();
	if (m_pRefFasSysData == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return;
	}

	if (m_pCurItem == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return;
	}
	if (m_pCurItem->pData == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return;
	}

	pPtn = (CDataPattern *)m_pCurItem->pData;
	pList = pPtn->GetPtnItemList();
	if (pList == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return;
	}

	pDB = m_pRefFasSysData->GetPrjDB();
	pDB->BeginTransaction();

	vtItem = ptc->vtDropItems;
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
		
		nIdx = FindRelayListCtrl(LK_TYPE_RELEAY 
			, pDev->GetFacpID() , pDev->GetUnitID() , pDev->GetChnID() , pDev->GetDeviceID());
		if (nIdx >= 0)
			continue; 
		//패턴 수동 추가
		strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,ITEM_TYPE,INSERT_TYPE,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,%d,%d,%d,'%s') "
			, pPtn->GetPatternID(),LK_TYPE_RELEAY,LOGIC_MANUAL
			, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
			, m_pRefFasSysData->GetCurrentUser()
		);

		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			pDB->RollbackTransaction();
			m_ctrlRelayList.SetRedraw();
			return;
		}
		
	}
	pDB->CommitTransaction();

	nIdx = 0;
	vtItem = ptc->vtDropItems;
	for (i=0; i < vtItem.size(); i++)
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
		// 기존에 있는지 확인
		nFind = FindRelayListCtrl(LK_TYPE_RELEAY
			, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID());
		if (nFind >= 0)
			continue;
		plnk = new CDataLinked;
		plnk->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
			, LK_TYPE_RELEAY, LOGIC_MANUAL , 0, pPtn->GetPatternID());
		m_ctrlRelayList.InsertItem(nIdx,pDev->GetOutContentsFullName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx,1,pDev->GetInputTypeName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 2, pDev->GetOutputTypeName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 3, pDev->GetEquipName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 4, pDev->GetEqAddIndex(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 5, pDev->GetOutContentsName(),crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx, 6, pDev->GetOutputLocationName(),crText,crBk);
#ifndef ENGLISH_MODE
		m_ctrlRelayList.SetItemText(nIdx,7,L"감지기/중계기",crText,crBk);
		m_ctrlRelayList.SetItemText(nIdx,8,L"수동",crText,crBk);
#else
		m_ctrlRelayList.SetItemText(nIdx, 7, L"DETECTOR/MODULE", crText, crBk);
		m_ctrlRelayList.SetItemText(nIdx, 8, L"MANUAL", crText, crBk);
#endif
		m_ctrlRelayList.SetItemData(nIdx, (DWORD_PTR)plnk);
		nIdx++;
		pList->AddTail(plnk);
	}
	m_ctrlRelayList.SetRedraw();

}

void CFormPattern::OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
 	*pResult = 0;
 	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
 	if (ptc == nullptr)
 		return;
 
 	if (m_pRefFasSysData == nullptr)
 		return;
 
 	if (m_pCurItem == nullptr || m_pCurItem->pData == nullptr)
 		return;
 
 
 	VT_HITEM vtItem;
 	ST_TREEITEM *pData;
 	CDataEmBc * pItem;
 	CDataLinked	* pLink;
 	CString str , strSql;
	CDataPattern * pPtn;
 	YAdoDatabase * pDB = nullptr; //m_pRefFasSysData->GetPrjDB();
 	pDB = m_pRefFasSysData->GetPrjDB();
 	if (pDB == nullptr)
 		return; 
 
	pPtn = (CDataPattern *)m_pCurItem->pData;
 	m_ctrlRelayList.SetRedraw(FALSE);
 	pDB->BeginTransaction();
 	int i = 0, nIdx = 0, nFind = -1;
 	vtItem = ptc->vtDropItems;
 
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
 
 		nIdx = FindRelayListCtrl(LK_TYPE_EMERGENCY , pItem->GetEmID() , 0 , 0,0);
 		if (nIdx >= 0)
 			continue;
 
 		strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,ITEM_TYPE,INSERT_TYPE,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
 			L" VALUES(1,%d,%d,%d,%d,%d,%d,%d,'%s') "
 			, pPtn->GetPatternID(), LK_TYPE_EMERGENCY ,LOGIC_MANUAL, pItem->GetEmID(), 0, 0, 0
			, m_pRefFasSysData->GetCurrentUser()
 		);
 
 		if (pDB->ExecuteSql(strSql) == FALSE)
 		{
 			pDB->RollbackTransaction();
 			m_ctrlRelayList.SetRedraw();
 			return;
 		}
 
 	}
 	pDB->CommitTransaction();
 
 	for (i=0; i < vtItem.size(); i++)
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
 
		nFind = FindRelayListCtrl(LK_TYPE_EMERGENCY, pItem->GetEmID() , 0 ,0,0);
		//nFind = FindItemLink(LK_TYPE_EMERGENCY, pItem->GetEmID());
 
 		if (nFind >= 0)
 			continue;
 
 		m_ctrlRelayList.InsertItem(0, pItem->GetEmName());
 
		m_ctrlRelayList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_EMERGENCY]);
 
 
 		pLink = new CDataLinked;
 		pLink->SetLinkData(pItem->GetEmID(), 0, 0, 0
 			, LK_TYPE_EMERGENCY, LOGIC_MANUAL, 0
 			, pPtn->GetPatternID(), 0, 0, 0
 		);
 
		pPtn->AddItemToPattern(pLink);
		m_ctrlRelayList.SetItemData(0, (DWORD_PTR)pLink);
 	}
	m_ctrlRelayList.SetRedraw();

}


void CFormPattern::OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurItem == nullptr || m_pCurItem->pData == nullptr)
		return;

	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataPump * pPm;
	CDataLinked	* pLink;
	CString str , strSql;
	CDataPattern * pPtn;
	YAdoDatabase * pDB = nullptr; //m_pRefFasSysData->GetPrjDB();
	int i = 0, nIdx = 0, nFind = -1;

	pPtn = (CDataPattern *)m_pCurItem->pData;

	pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB == nullptr)
		return;

	vtItem = ptc->vtDropItems;
	m_ctrlRelayList.SetRedraw(FALSE);
	pDB->BeginTransaction();
	vtItem = ptc->vtDropItems;

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

		nIdx = FindRelayListCtrl(LK_TYPE_PUMP, pPm->GetFacpID(), pPm->GetPumpID(), 0, 0);
		if (nIdx >= 0)
			continue;

		strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,ITEM_TYPE,INSERT_TYPE,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,%d,%d,%d,'%s') "
			, pPtn->GetPatternID(), LK_TYPE_PUMP,LOGIC_MANUAL, pPm->GetFacpID(), pPm->GetPumpID(), 0, 0
			, m_pRefFasSysData->GetCurrentUser()
		);

		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			pDB->RollbackTransaction();
			m_ctrlRelayList.SetRedraw();
			return;
		}

	}
	pDB->CommitTransaction();

	for (i=0; i < vtItem.size(); i++)
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
		nFind = FindRelayListCtrl(LK_TYPE_PUMP, pPm->GetFacpID(), pPm->GetPumpID(),0,0);

		if (nFind >= 0)
			continue;

		m_ctrlRelayList.InsertItem(0, pPm->GetPumpName());

		m_ctrlRelayList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_PUMP]);


		pLink = new CDataLinked;
		pLink->SetLinkData(pPm->GetFacpID(), pPm->GetPumpID(), 0, 0
			, LK_TYPE_PUMP, LOGIC_MANUAL, 0
			, pPtn->GetPatternID()
		);

		pPtn->AddItemToPattern(pLink);
		m_ctrlRelayList.SetItemData(0, (DWORD_PTR)pLink);
	}


	m_ctrlRelayList.SetRedraw();

}

void CFormPattern::OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurItem == nullptr || m_pCurItem->pData == nullptr)
		return;

	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataFacpRelay * pCont;
	CDataLinked	* pLink;
	CString str, strSql;
	CDataPattern * pPtn;
	YAdoDatabase * pDB = nullptr; //m_pRefFasSysData->GetPrjDB();
	int i = 0, nIdx = 0, nFind = -1;

	pPtn = (CDataPattern *)m_pCurItem->pData;

	pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB == nullptr)
		return;

	vtItem = ptc->vtDropItems;
	m_ctrlRelayList.SetRedraw(FALSE);
	pDB->BeginTransaction();
	vtItem = ptc->vtDropItems;

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

		nIdx = FindRelayListCtrl(LK_TYPE_FACP_RELAY, pCont->GetFacpID(), pCont->GetRelayID(), 0, 0);
		if (nIdx >= 0)
			continue;

		strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,ITEM_TYPE,INSERT_TYPE,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,%d,%d,%d,'%s') "
			, pPtn->GetPatternID(), LK_TYPE_FACP_RELAY,LOGIC_MANUAL, pCont->GetFacpID(), pCont->GetRelayID(), 0, 0
			, m_pRefFasSysData->GetCurrentUser()
		);

		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			pDB->RollbackTransaction();
			m_ctrlRelayList.SetRedraw();
			return;
		}

	}
	pDB->CommitTransaction();

	for (i = 0; i < vtItem.size(); i++)
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
		nFind = FindRelayListCtrl(LK_TYPE_FACP_RELAY, pCont->GetFacpID(), pCont->GetRelayID(), 0, 0);

		if (nFind >= 0)
			continue;

		m_ctrlRelayList.InsertItem(0, pCont->GetFRelayName());

		m_ctrlRelayList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_FACP_RELAY]);


		pLink = new CDataLinked;
		pLink->SetLinkData(pCont->GetFacpID(), pCont->GetRelayID(), 0, 0
			, LK_TYPE_FACP_RELAY, LOGIC_MANUAL, 0
			, pPtn->GetPatternID()
		);

		pPtn->AddItemToPattern(pLink);
		m_ctrlRelayList.SetItemData(0, (DWORD_PTR)pLink);
	}

	m_ctrlRelayList.SetRedraw();

	delete ptc;
}

void CFormPattern::OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
// 	*pResult = 0;
// #if _USE_PSWITCH_
// 	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
// 	if (ptc == nullptr)
// 		return;
// 
// 	if (m_pRefFasSysData == nullptr)
// 		return;
// 
// 	if (m_pCurItem == nullptr || m_pCurItem->pData == nullptr)
// 		return;
// 
// 	m_ctrlRelayList.SetRedraw(FALSE);
// 
// 	VT_HITEM vtItem;
// 	ST_TREEITEM *pData;
// 	CDataPS * pPs;
// 	CDataLinked	* pLink;
// 	CString str;
// 	vtItem = ptc->vtDropItems;
// 	int i = 0, nIdx = 0, nFind = -1;
// 	for (; i < vtItem.size(); i++)
// 	{
// 		if (vtItem[i] == nullptr)
// 			continue;
// 		if (vtItem[i]->dwItemData == 0)
// 			continue;
// 		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
// 		if (pData->pData == nullptr)
// 			continue;
// 
// 		if (pData->nDataType != TTYPE_DEV_PSWITCH)
// 			continue;
// 		pPs = (CDataPS *)pData->pData;
// 		if (pPs == nullptr)
// 			continue;
// 
// 		nFind = FindItemLink(LK_TYPE_G_FLAG, pPs->GetFacpID(), pPs->GetPSwitchID());
// 
// 		if (nFind >= 0)
// 			continue;
// 
// 		m_ctrlPatternList.InsertItem(0, pPs->GetPSwitchName());
// 
// 		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_G_FLAG]);
// 
// 
// 		pLink = new CDataLinked;
// 		pLink->SetLinkData(pPs->GetFacpID(), pPs->GetPSwitchID(), 0, 0
// 			, LK_TYPE_G_FLAG, LOGIC_MANUAL, 0
// 			, m_pDevice->GetFacpID(), m_pDevice->GetUnitID(), m_pDevice->GetChnID(), m_pDevice->GetDeviceID()
// 		);
// 
// 		m_pRefFasSysData->InsertLinkedTable(m_pDevice, pLink);
// 		m_pDevice->AddLink(FALSE, pLink);
// 		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
// 	}
// 	m_ctrlPatternList.SetRedraw();
// 
// #endif
}


int CFormPattern::FindRelayListCtrl(CDataLinked* plnk)
{
	CDataLinked * pTemp;
	//CDataDevice * pData;
	int i = 0, nCnt;
	nCnt = m_ctrlRelayList.GetItemCount();
	for (i = 0; i < nCnt; i++)
	{
		pTemp = (CDataLinked*)m_ctrlRelayList.GetItemData(i);
		if (pTemp == nullptr)
			continue; 

		if (pTemp->IsEqual(plnk) == TRUE)
			return i;
	}
	return -1;
}

int CFormPattern::FindRelayListCtrl(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	CDataLinked * pTemp;
	//CDataDevice * pData;
	int i = 0, nCnt;
	nCnt = m_ctrlRelayList.GetItemCount();
	for (i = 0; i < nCnt; i++)
	{
		pTemp = (CDataLinked*)m_ctrlRelayList.GetItemData(i);
		if (pTemp == nullptr)
			continue;

		if (pTemp->GetLinkType() != nType)
			continue; 
		if (pTemp->GetTgtFacp() != nFacpID)
			continue;
		if (pTemp->GetTgtUnit() != nUnitID)
			continue;
		if (pTemp->GetTgtChn() != nChnID)
			continue;
		if (pTemp->GetTgtDev() != nRelayID)
			continue;
		return i;
	}
	return -1;
}

void CFormPattern::OnTvnSelchangedPatternTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem == nullptr)
		return;

	AddCancel();
	DisplayPattern(hItem);
	*pResult = 0;
}

LRESULT CFormPattern::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
// 	if (m_ctrlRelayList.GetSafeHwnd())
// 		m_ctrlRelayList.DeleteAllItems();
// 	if (m_ctrlPatternList)
// 		m_ctrlPatternList.DeleteAllItems();
	if (wp == FORM_PRJ_OPEN || wp == FORM_PRJ_NEW)
	{
		m_pRefFasSysData = theApp.GetRelayTableData();

		if (m_pRefFasSysData != nullptr)
			m_pRefFasSysData->FillPatternTree(&m_ctrlPtnTree, &m_ptrItemList);
	}
	return 0;
}


int CFormPattern::SetTypeCombo(int nType)
{
	int nCnt, i,nData;
	nCnt = m_cmbPtnType.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		nData = (int)m_cmbPtnType.GetItemData(i);
		if (nData == nType)
		{
			m_cmbPtnType.SetCurSel(i);
			return i;
		}
	}
	return -1;
}


void CFormPattern::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu, *pContext;
	UINT nFlag = 0;
	int nIdx = -1;
	CPoint ptHit;
	CPoint pt = point;
	ptHit = pt;
	CWnd * pHitWnd = WindowFromPoint(pt);
	//ScreenToClient(&ptHit);
	if (point == CPoint(-1, -1))
		return;

	if (!(pHitWnd == (CWnd*)&m_ctrlRelayList))
		return;
	
	m_ctrlRelayList.ScreenToClient(&ptHit);
	nIdx = m_ctrlRelayList.HitTest(ptHit, nullptr);
	if (nIdx < 0)
		return;

#ifndef ENGLISH_MODE
	menu.LoadMenu(IDR_POPUP_PTNLIST);
#else
	menu.LoadMenu(IDR_POPUP_PTNLIST_EN);
#endif
	pContext = menu.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
}

int CFormPattern::SelectItem(int nItemType, DWORD_PTR dwData)
{
	HTREEITEM hFind = nullptr;
	int nPid = 0;
	if (nItemType == TTYPE_DEV_PATTERN)
		nPid = dwData;
	else
		nPid = ((CDataLinked*)dwData)->GetTgtFacp();

	if (nPid <= 0)
		return 0;

	hFind = FindPatternItem(nPid, TVI_ROOT);
	if (hFind == nullptr)
		return 0; 

	m_ctrlPtnTree.SelectItem(hFind);
	return 1;
}


HTREEITEM CFormPattern::FindPatternItem(int nPid , HTREEITEM hParent)
{
	ST_TREEITEM * pTData = nullptr;
	DWORD_PTR dwTemp;
	CDataPattern * pPtn;
	HTREEITEM hitemFind, hItemChile, hItemSibling;
	hitemFind = hItemChile = hItemSibling = NULL;
	if (hParent == nullptr)
		return nullptr;
	if (hParent != TVI_ROOT)
		dwTemp = m_ctrlPtnTree.GetItemData(hParent);
	else
		dwTemp = 0;
	pTData = (ST_TREEITEM *)dwTemp;
	
	if (pTData != 0 && pTData->pData != nullptr && pTData->nDataType == TTYPE_DEV_PATTERN)
	{
		pPtn = (CDataPattern*)pTData->pData;
		if (pPtn != nullptr && pPtn->GetPatternID() == nPid)
			return hParent;
	}

	// 자식 노드를 찾는다.
	hItemChile = m_ctrlPtnTree.GetChildItem(hParent);
	while (hItemChile)
	{
		// check the children of the current item
		HTREEITEM hFound = FindPatternItem(nPid, hItemChile);
		if (hFound)
			return hFound;

		// get the next sibling of the current item
		hItemChile = m_ctrlPtnTree.GetNextSiblingItem(hItemChile);
	}
	return nullptr;
}


void CFormPattern::OnPopupPtnlistDel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OnBnClickedBtnRelayDel();
}


void CFormPattern::OnItemSelectItem()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDataLinked * plnk;
	int nItemType;
	FormViewStyle nViewType;
	DWORD_PTR dwData=0;
	int nSel = m_ctrlRelayList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
		return; 

	plnk = (CDataLinked *)m_ctrlRelayList.GetItemData(nSel);
	if (plnk == nullptr)
		return;

	switch (plnk->GetLinkType())
	{
	case LK_TYPE_RELEAY:
		nViewType = FV_MAKELINKER;
		nItemType = TTYPE_DEV_LINKED_TARGETITEM;
		dwData = (DWORD_PTR)plnk;
		break;
	case LK_TYPE_EMERGENCY:
		nViewType = FV_EMERGENCY;
		nItemType = TTYPE_DEV_EMERGENCY;
		dwData = plnk->GetTgtFacp();
		break;
	case LK_TYPE_PUMP:
// 		nViewType = FV_PUMP;
// 		nItemType = TTYPE_DEV_PUMP;
// 		dwData = plnk->GetTgtFacp();
	//	break;
		return;
	default:
		return; 
	}
	theApp.ViewFormSelectItem(nViewType, nItemType, dwData);
}


void CFormPattern::OnBnClickedRdManaulMake()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nManualMakeStatus = 1;
}


void CFormPattern::OnBnClickedRdTableMake()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nManualMakeStatus = 0;
}

//20240319 GBM start - 패턴 편집 창 메모리 누수 오류 처리
void CFormPattern::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	RemoveAllTreeData();
}
//20240319 GBM end
