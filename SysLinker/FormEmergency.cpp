// FormEmergency.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormEmergency.h"
#include "DataEmBc.h"
#include "ManagerEmergency.h"
#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "../Common/Utils/ExcelWrapper.h"
#include "DataLinked.h"
#include <algorithm>

UINT ThreadAddAndSaveEB(LPVOID pParam)
{
	CFormEmergency* pFE = (CFormEmergency*)pParam;
	
	if (pFE->m_bAdd)
		pFE->m_bThreadSucceeded = pFE->DataAdd();
	else
		pFE->m_bThreadSucceeded = pFE->DataSave();

	pFE->m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(pFE->m_hThreadHandle);

	return 0;
}

UINT ThreadDeleteEB(LPVOID pParam)
{
	CFormEmergency* pFE = (CFormEmergency*)pParam;

	pFE->m_bThreadSucceeded = pFE->DataDelete();

	pFE->m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(pFE->m_hThreadHandle);

	return 0;
}

// CFormEmergency

IMPLEMENT_DYNCREATE(CFormEmergency, CFormView)

CFormEmergency::CFormEmergency()
	: CFormView(IDD_FORMEMERGENCY)
	, m_strName(_T(""))
	, m_strAddr(_T(""))
	, m_nNum(0)
	, m_strBuild(_T(""))
	, m_strStair(_T(""))
	, m_strFloor(_T(""))
{
	m_bAdd = FALSE;
	//m_pCurrentData = nullptr;
	//m_nCurSel = -1;
}

CFormEmergency::~CFormEmergency()
{
}

void CFormEmergency::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EMERGENCY_LIST, m_ctrlList);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);
	DDX_Text(pDX, IDC_ED_ADDR, m_strAddr);
	DDX_Text(pDX, IDC_ED_NUM, m_nNum);
	DDX_Text(pDX, IDC_ED_BUILD, m_strBuild);
	DDX_Text(pDX, IDC_ED_STAIR, m_strStair);
	DDX_Text(pDX, IDC_ED_FLOOR, m_strFloor);
}

BEGIN_MESSAGE_MAP(CFormEmergency, CFormView)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_ADD, &CFormEmergency::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFormEmergency::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CFormEmergency::OnBnClickedBtnDel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_EMERGENCY_LIST, &CFormEmergency::OnLvnItemchangedEmergencyList)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CFormEmergency::OnBnClickedBtnLoad)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
	ON_EN_CHANGE(IDC_ED_BUILD, &CFormEmergency::OnEnChangeEdBuild)
	ON_EN_CHANGE(IDC_ED_STAIR, &CFormEmergency::OnEnChangeEdStair)
	ON_EN_CHANGE(IDC_ED_FLOOR, &CFormEmergency::OnEnChangeEdFloor)
END_MESSAGE_MAP()


// CFormEmergency 진단입니다.

#ifdef _DEBUG
void CFormEmergency::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormEmergency::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormEmergency 메시지 처리기입니다.
LRESULT CFormEmergency::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	InitList();
	return 1;
}

void CFormEmergency::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_ctrlList.InsertColumn(0, _T("번호"), LVCFMT_CENTER, 50);
	m_ctrlList.InsertColumn(1, _T("이름"), LVCFMT_CENTER, 250);
	m_ctrlList.InsertColumn(2, _T("주소"), LVCFMT_CENTER, 250);
	m_ctrlList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	InitList();
	theApp.SetFormViewInitComplete(FV_EMERGENCY);
}


void CFormEmergency::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rc, rcList;
	GetClientRect(&rc);
	rc.DeflateRect(5, 8, 5, 5);

	rcList = rc;
	rcList.top = 180;
	rcList.left = 5;

	if (m_ctrlList.GetSafeHwnd())
		m_ctrlList.MoveWindow(&rcList);
}


BOOL CFormEmergency::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::PreCreateWindow(cs);
}


void CFormEmergency::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnClose();
}


int CFormEmergency::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CFormEmergency::AddInit()
{
	m_bAdd = TRUE;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(FALSE);
	m_strAddr = L"";
	CRelayTableData * pTable = theApp.GetRelayTableData();
	if (pTable)
		m_nNum = pTable->GetWholeEmergencyID();
	else
		m_nNum = 0;
	m_strName = L"";

	m_strBuild = L"";
	m_strStair = L"";
	m_strFloor = L"";
	//m_pCurrentData = nullptr;
	UpdateData(FALSE);	

}


void CFormEmergency::InitData()
{
	m_strAddr = L"";
	m_nNum = 0;
	m_strName = L"";
	//m_pCurrentData = nullptr;
	//UpdateData(FALSE);

}


void CFormEmergency::AddCancel()
{
	m_bAdd = FALSE;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(TRUE);
}

void CFormEmergency::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AddInit();
}

void CFormEmergency::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//20240527 GBM start - 스레드로 전환
#if 1
	UpdateData();	// DataAdd(), DataSave()에서 빼냄

	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
	CString strMsg = _T("비상방송 정보를 저장 중입니다. 잠시 기다려 주세요.");
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadAddAndSaveEB, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
		GF_AddLog(L"비상방송 정보 저장에 성공했습니다.");
		Log::Trace("Emergency broadcast information was saved successfully.");
	}
	else
	{
		GF_AddLog(L"비상방송 정보 저장에 실패했습니다.");
		Log::Trace("Failed to save emergency broadcast information.");
	}
#else
	UpdateData();	// DataAdd(), DataSave()에서 빼냄
	if (m_bAdd)
		DataAdd();
	else
		DataSave();
#endif
	//20240527 GBM end

	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_DKP_EMERGENCY_REFRESH, DATA_ALL, 0);
}


void CFormEmergency::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nSel = m_ctrlList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
	{
		AfxMessageBox(L"선택된 비상방송 정보가 없습니다.");
		return;
	}

	if (AfxMessageBox(L"선택된 비상방송 정보를 삭제하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	//20240527 GBM start - 스레드로 전환
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
	CString strMsg = _T("비상방송 정보를 삭제 중입니다. 잠시 기다려 주세요.");
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadDeleteEB, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
		GF_AddLog(L"비상방송 정보 삭제에 성공했습니다.");
		Log::Trace("Emergency broadcast information was deleted successfully.");
	}
	else
	{
		GF_AddLog(L"비상방송 정보를 삭제 실패했습니다.");
		Log::Trace("Failed to delete emergency broadcast information.");
	}
#else
	DataDelete();
#endif
	//20240527 GBM end

	UpdateData();	//DataDelete() -> InitData()에서 빼냄

	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_DKP_EMERGENCY_REFRESH, DATA_ALL, 0);
}


void CFormEmergency::OnLvnItemchangedEmergencyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	if (pNMLV->iItem < 0)
		return ; 
	AddCancel();
	DisplayItem(pNMLV->iItem);
}


int CFormEmergency::DisplayItem(int nIdx)
{
	CDataEmBc * pData;
	m_strName = L"";
	pData = (CDataEmBc *)m_ctrlList.GetItemData(nIdx);
	if (pData == nullptr)
		return 0; 
	//m_pCurrentData = pData;

	m_strName = pData->GetEmName();
	m_strAddr = pData->GetEmAddr(); 
	m_nNum = pData->GetEmID();
	m_strBuild = pData->GetBuildName();
	m_strStair = pData->GetStairName();
	m_strFloor = pData->GetFloorName();
	UpdateData(FALSE);
	return 1;
}


int CFormEmergency::InitList()
{
	m_ctrlList.SetRedraw(FALSE);
	m_ctrlList.DeleteAllItems();

	int nIdx=0;
	POSITION pos;
	//INT_PTR nSize;
	CDataEmBc * pData;
	CString str;
	std::shared_ptr<CManagerEmergency>	spManager;
	CRelayTableData * pTable = theApp.GetRelayTableData();
	if (pTable == nullptr)
	{
		m_ctrlList.SetRedraw();
		return 0;
	}
	spManager = pTable->GetEmergencyManager();
	if (spManager == nullptr)
	{
		m_ctrlList.SetRedraw();
		return 0;
	}
	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pData = spManager->GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData == nullptr)
			continue;

		str.Format(L"%d", pData->GetEmID());
		m_ctrlList.InsertItem(nIdx, str);
		m_ctrlList.SetItemText(nIdx, 1, pData->GetEmName());
		m_ctrlList.SetItemText(nIdx, 2, pData->GetEmAddr());
		m_ctrlList.SetItemData(nIdx, (DWORD_PTR)pData);
		nIdx++;
	}
	m_ctrlList.SetRedraw();

	return 1;
}

int CFormEmergency::DataDelete()
{
	int nSel = m_ctrlList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
	{
		AfxMessageBox(L"선택된 비상방송 데이터가 없습니다.");
		return 0;
	}
	CRelayTableData * pTable = theApp.GetRelayTableData();
	if (pTable == nullptr)
	{
		AfxMessageBox(L"프로젝트 정보가 없습니다. \n데이터를 삭제하는데 실패 했습니다.");
		return 0;
	}
	YAdoDatabase * pDB = pTable->GetPrjDB();
	if (pDB == nullptr)
	{
		AfxMessageBox(L"프로젝트에 데이터베이스 정보가 없습니다. \n데이터를 삭제하는데 실패 했습니다.");
		return 0;
	}

	int nCnt;
	CString strSql;
	CDataEmBc * pData;
	pData = (CDataEmBc*)m_ctrlList.GetItemData(nSel);
	if (pData == nullptr)
	{
		AfxMessageBox(L"비상방송 정보가 없습니다.");
		return 0;
	}
	strSql.Format(L"SELECT * FROM TB_EM_BC WHERE EM_ID=%d "
		, pData->GetEmID()
	);

	if (pDB->OpenQuery(strSql) == FALSE)
	{
		AfxMessageBox(L"데이터베이스에서 비상방송 데이터를 가져오는데 실패 했습니다. \n데이터를 삭제하는데 실패 했습니다.");
		return 0;
	}

	nCnt = pDB->GetRecordCount();
	pDB->RSClose();
	if (nCnt <= 0)
	{
		AfxMessageBox(L"해당 비상방송 데이터가 없습니다.");
		return 0;
	}

	pDB->BeginTransaction();
	strSql.Format(L"DELETE TB_EM_BC WHERE  EM_ID=%d "
		, pData->GetEmID()
	);
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		pDB->RollbackTransaction();
		AfxMessageBox(L"데이터를 삭제하는데 실패 했습니다.");
		return 0;
	}

	m_ctrlList.DeleteItem(nSel);
	//int nIdx = FindItem(m_pCurrentData);
	
	// tree 삭제
	std::shared_ptr <CManagerEmergency> spManager = nullptr;
	spManager = pTable->GetEmergencyManager();
	if (spManager == nullptr)
	{
		pDB->RollbackTransaction();
		AfxMessageBox(L"데이터를 삭제하는데 실패 했습니다.");
		return 0;
	}
	spManager->RemoveEmergency(pData->GetEmID());

	//20240507 GBM start - 비상방송 편집 중계기 일람표 적용
	int nNum = pData->GetEmID();
	CString strRemarks = pData->GetEmName();
	CString strCommContent = pData->GetEmAddr();
	CString strWin32AppProjectName = theApp.m_pFasSysData->GetPrjName();
	CString strMsg1 = _T("");
	CString strMsg2 = _T("");
	BOOL bRet = FALSE;

	bRet = CNewExcelManager::Instance()->UpdateOneEBInfo(nNum, _T(""), _T(""), strWin32AppProjectName);
	if (bRet)
	{
		strMsg1.Format(_T("The emergency broadcast [ID : %d, Name : %s, Address : %s] has been successfully deleted from the module table file."), nNum, strRemarks, strCommContent);
		strMsg2.Format(_T("비상방송 [번호 : %d, 이름 : %s, 주소 : %s]를 중계기 일람표에서 삭제하는 데에 성공했습니다."), nNum, strRemarks, strCommContent);
	}
	else
	{
		strMsg1.Format(_T("Deleting the emergency broadcast [ID : %d, Name : %s, Address : %s] from the module table file failed."), nNum, strRemarks, strCommContent);
		strMsg2.Format(_T("비상방송 [번호 : %d, 이름 : %s, 주소 : %s]를 중계기 일람표에서 삭제하는 데에 실패했습니다."), nNum, strRemarks, strCommContent);
	}

	Log::Trace("%s", CCommonFunc::WCharToChar(strMsg1.GetBuffer(0)));
	GF_AddLog(strMsg2);
	//20240507 GBM end
	
	delete pData;
	pData = nullptr;
	pDB->CommitTransaction();
	InitData();
	//AfxMessageBox(L"데이터를 삭제하는데 성공 했습니다.");

	return 1;
}

int CFormEmergency::DataMultiDelete()
{
	CString str;
	int nRet = 0, nCnt, i;
	nCnt = m_ctrlList.GetSelectedCount();
	if (nCnt < 0)
	{
		AfxMessageBox(L"선택된 비상방송 데이터가 없습니다.");
		return 0;
	}

	CRelayTableData * pTable = theApp.GetRelayTableData();
	if (pTable == nullptr)
	{
		AfxMessageBox(L"프로젝트 정보가 없습니다. \n데이터를 삭제하는데 실패 했습니다.");
		return 0;
	}
	YAdoDatabase * pDB = pTable->GetPrjDB();
	if (pDB == nullptr)
	{
		AfxMessageBox(L"프로젝트에 데이터베이스 정보가 없습니다. \n데이터를 삭제하는데 실패 했습니다.");
		return 0;
	}

	BOOL bError = FALSE;
	POSITION pos;
	CString strSql, strError;
	CDataEmBc * pData;
	std::vector<int> vtSel;
	pos = m_ctrlList.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nSelected = m_ctrlList.GetNextSelectedItem(pos);
		vtSel.push_back(nSelected);
	}
	sort(vtSel.begin(), vtSel.end());

	pDB->BeginTransaction();
	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		pData = (CDataEmBc*)m_ctrlList.GetItemData(vtSel[i]);
		if (pData == nullptr)
		{
			strError = L"삭제하는데 실패 했습니다. 비상방송 정보를 가져오는데 실패 했습니다.";
			bError = TRUE;
			break;;
		}

		strSql.Format(L"SELECT * FROM TB_EM_BC WHERE EM_ID=%d "
			, pData->GetEmID()
		);
		if (pDB->OpenQuery(strSql) == FALSE)
		{
			strError = L"삭제하는데 실패 했습니다. 데이터베이스에서 비상방송 정보를 가져오는데 실패했습니다.";
			bError = TRUE;
			break;;
		}

		nCnt = pDB->GetRecordCount();
		if (nCnt == 0)
			continue;

		strSql.Format(L"DELETE TB_EM_BC WHERE  EM_ID=%d "
			, pData->GetEmID()
		);

		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			strError = L"삭제하는데 실패 했습니다. 데이터베이스에서 비상방송 정보를 삭제하는데 실패했습니다.";
			bError = TRUE;
			break;;
		}
	}
	if (bError == TRUE)
	{
		pDB->RollbackTransaction();
		AfxMessageBox(strError);
		return 0;
	}
	pDB->CommitTransaction();

	std::shared_ptr <CManagerEmergency> spManager = nullptr;
	spManager = pTable->GetEmergencyManager();
	for (i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		pData = (CDataEmBc*)m_ctrlList.GetItemData(vtSel[i]);
		if (pData == nullptr)
			continue;
		spManager->RemoveEmergency(pData->GetEmID());
		delete pData;
		pData = nullptr;
		m_ctrlList.DeleteItem(vtSel[i]);
	}
	InitData();
	AfxMessageBox(L"데이터를 삭제하는데 성공 했습니다.");
	return 1;
}
int CFormEmergency::DataAdd()
{
	CRelayTableData * pTable = theApp.GetRelayTableData();
	if (pTable == nullptr)
		return 0;
	YAdoDatabase * pDB = pTable->GetPrjDB();
	if (pDB == nullptr)
		return 0;

	int nCnt;
	CString strSql;
	int nID;
	CDataEmBc * pData;
	//UpdateData();
	nID = m_nNum;

	strSql.Format(L"INSERT TB_EM_BC(NET_ID,EM_ID,EM_ADDR , EM_NAME ,ADD_USER) "
		L" VALUES(1, %d,'%s','%s','%s')"
		, nID, m_strAddr, m_strName, pTable->GetCurrentUser()
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"데이터를 추가하는데 실패 했습니다.");
		return 0;
	}
	pData = new CDataEmBc;
	pData->SetData(nID, 0, m_strAddr, m_strName);
	std::shared_ptr <CManagerEmergency> spManager = nullptr;
	spManager = pTable->GetEmergencyManager();
	if (spManager == nullptr)
	{
		AfxMessageBox(L"데이터를 추가하는데 실패 했습니다.");
		return 0;
	}
	spManager->AddTail(pData);

	nCnt = m_ctrlList.GetItemCount();
	CString str;
	str.Format(L"%d", pData->GetEmID());
	m_ctrlList.InsertItem(nCnt, str);
	m_ctrlList.SetItemText(nCnt, 1, pData->GetEmName());
	m_ctrlList.SetItemText(nCnt, 2, pData->GetEmAddr());
	m_ctrlList.SetItemData(nCnt, (DWORD_PTR)pData);
	// 먼저 현재 선택상태를 해제합니다
	m_ctrlList.SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	// 원하는 아이템을 선택합니다
	m_ctrlList.SetItemState(nCnt, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	// 선택된 아이템을 표시합니다
	m_ctrlList.EnsureVisible(nCnt, false);
	// 리스트 컨트롤에 포커스를 맞춥니다
	m_ctrlList.SetFocus();
	//AfxMessageBox(L"비상방송 정보를 추가하는데 성공 했습니다.");

	//20240507 GBM start - 비상방송 편집 중계기 일람표 적용
	int nNum = pData->GetEmID();
	CString strRemarks = pData->GetEmName();
	CString strCommContent = pData->GetEmAddr();
	CString strWin32AppProjectName = theApp.m_pFasSysData->GetPrjName();
	CString strMsg1 = _T("");
	CString strMsg2 = _T("");
	BOOL bRet = FALSE;

	bRet = CNewExcelManager::Instance()->UpdateOneEBInfo(nNum, strRemarks, strCommContent, strWin32AppProjectName);
	if (bRet)
	{
		strMsg1.Format(_T("The new emergency broadcast [ID : %d, Name : %s, Address : %s] has been successfully added to the module table file."), nNum, strRemarks, strCommContent);
		strMsg2.Format(_T("새 비상방송 [번호 : %d, 이름 : %s, 주소 : %s]를 중계기 일람표에 추가하는 데에 성공했습니다."), nNum, strRemarks, strCommContent);
	}
	else
	{
		strMsg1.Format(_T("Adding the emergency broadcast [ID : %d, Name : %s, Address : %s] to the module table file failed."), nNum, strRemarks, strCommContent);
		strMsg2.Format(_T("새 비상방송 [번호 : %d, 이름 : %s, 주소 : %s]를 중계기 일람표에 추가하는 데에 실패했습니다."), nNum, strRemarks, strCommContent);
	}

	Log::Trace("%s", CCommonFunc::WCharToChar(strMsg1.GetBuffer(0)));
	GF_AddLog(strMsg2);
	//20240507 GBM end

	return 1;
}


int CFormEmergency::DataSave()
{
	CRelayTableData * pTable = theApp.GetRelayTableData();
	if (pTable == nullptr)
		return 0;
	YAdoDatabase * pDB = pTable->GetPrjDB();
	if (pDB == nullptr)
		return 0;
	int nCnt;
	CString strSql;
	int nID , nSel;
	CDataEmBc * pData;

	//UpdateData();

	nSel = m_ctrlList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
	{
		AfxMessageBox(L"선택된 비상방송이 없습니다.");
		return 0; 
	}
	pData = (CDataEmBc*)m_ctrlList.GetItemData(nSel);
	
	// 비상방송 순번을 변경했는지 체크
	if (pData->GetEmID() != m_nNum)
	{
		nID = m_nNum;
	}
	else
		nID = pData->GetEmID();

	strSql.Format(L"SELECT * FROM TB_EM_BC WHERE EM_ID=%d "
		, nID
	);

	if (pDB->OpenQuery(strSql) == FALSE)
	{
		AfxMessageBox(L"데이터베이스에서 비상방송정보를 가져오는데 실팼습니다.");
		return 0;
	}

	nCnt = pDB->GetRecordCount();
	pDB->RSClose();
	if (nCnt <= 0)
	{
		if (m_bAdd == FALSE)
		{
			if (AfxMessageBox(L"기존 데이터가 없습니다. 새로 추가하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
				return 0;
		}
		return DataAdd();
	}
// 	else
// 	{
// 		if (m_bAdd)
// 		{
// 			if (AfxMessageBox(L"이미 데이터가 있습니다. 기존데이터를 수정하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
// 				return 0;
// 			std::shared_ptr <CManagerEmergency> spManager = nullptr;
// 			spManager = pTable->GetEmergencyManager();
// 			if (spManager == nullptr)
// 			{
// 				AfxMessageBox(L"데이터를 추가하는데 실패 했습니다.");
// 				return 0;
// 			}
// 			spManager->SetAtGrow(nID, m_pCurrentData);
// 			m_pCurrentData = spManager->GetAt(nID);
// 			if (m_pCurrentData == nullptr)
// 			{
// 				AfxMessageBox(L"데이터를 가져오는데 실패 했습니다.");
// 				return 0;
// 			}
// 		}
// 	}
	int nIdx = FindItem(m_nNum);
	if (nIdx < 0)
	{
		AfxMessageBox(L"입력한 비상방송 아이디의 데이터가 없습니다.");
		return 0;
	}
	pData = (CDataEmBc *)m_ctrlList.GetItemData(nIdx);

	if (pData == nullptr)
	{
		AfxMessageBox(L"입력한 비상방송 데이터가 없습니다.");
		return 0;
	}
	strSql.Format(L"UPDATE TB_EM_BC SET EM_NAME='%s' , EM_ADDR='%s' , EM_ID=%d "
		L" WHERE EM_ID=%d "
		, m_strName, m_strAddr , m_nNum
		, nID
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"데이터를 수정하는데 실패 했습니다.");
		return 0;
	}

	pData->SetData(m_nNum, 0, m_strAddr, m_strName);
	CString str;
	str.Format(L"%d",m_nNum);
	m_ctrlList.SetItemText(nIdx,0, str);
	m_ctrlList.SetItemText(nIdx, 1, pData->GetEmName());
	m_ctrlList.SetItemText(nIdx, 2, pData->GetEmAddr());
	m_ctrlList.SetItemData(nIdx, (DWORD_PTR)pData);

	//20240502 GBM start - 비상방송 편집 중계기 일람표 적용
	int nNum = pData->GetEmID();
	CString strRemarks = pData->GetEmName();
	CString strCommContent = pData->GetEmAddr();
	CString strWin32AppProjectName = theApp.m_pFasSysData->GetPrjName();
	CString strMsg1 = _T("");
	CString strMsg2 = _T("");
	BOOL bRet = FALSE;

	bRet = CNewExcelManager::Instance()->UpdateOneEBInfo(nNum, strRemarks, strCommContent, strWin32AppProjectName);
	if (bRet)
	{
		strMsg1.Format(_T("The new emergency broadcast [ID : %d, Name : %s, Address : %s] has been successfully modified in the module table file."), nNum, strRemarks, strCommContent);
		strMsg2.Format(_T("비상방송 [번호 : %d, 이름 : %s, 주소 : %s]를 중계기 일람표에 수정하는 데에 성공했습니다."), nNum, strRemarks, strCommContent);
	}
	else
	{
		strMsg1.Format(_T("Failed to modify the emergency broadcast [ID : %d, Name : %s, Address : %s] in the module table file failed."), nNum, strRemarks, strCommContent);
		strMsg2.Format(_T("비상방송 [번호 : %d, 이름 : %s, 주소 : %s]를 중계기 일람표에 수정하는 데에 실패했습니다."), nNum, strRemarks, strCommContent);
	}

	Log::Trace("%s", CCommonFunc::WCharToChar(strMsg1.GetBuffer(0)));
	GF_AddLog(strMsg2);
	//20240502 GBM end

	return 1;
}
//
//int CFormEmergency::FindItem(CDataEmBc* pData)
//{
//	CDataEmBc * pTemp;
//	int nCnt, i;
//	nCnt = m_ctrlList.GetItemCount();
//	for (i = 0; i < nCnt; i++)
//	{
//		pTemp = (CDataEmBc *)m_ctrlList.GetItemData(i);
//		if (pTemp == nullptr)
//			continue;
//
//		if (pData == pTemp)
//			return i;
//	}
//	return -1;
//}

int CFormEmergency::FindItem(int nID)
{
	CDataEmBc * pTemp;
	int nCnt, i;
	nCnt = m_ctrlList.GetItemCount();
	for (i = 0; i < nCnt; i++)
	{
		pTemp = (CDataEmBc *)m_ctrlList.GetItemData(i);
		if (pTemp == nullptr)
			continue;

		if (pTemp->GetEmID() == nID)
			return i;
	}
	return -1;
}


void CFormEmergency::OnBnClickedBtnLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (AfxMessageBox(L"기존 비상방송 데이터가 모두 삭제 됩니다\n그래도 진행하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return; 
	}

	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
	if (pRelayTable == nullptr)
	{
		AfxMessageBox(L"프로젝트에 로그인하거나 새로 생성한 후에 진행해 주십시요");
		return; 
	}
	CExcelWrapper xls;
	CString strDefault = L"*.xls", strFilter = L"비상방송 데이터(*.xls)|*.xls|All Files (*.*)|*.*||";
	CFileDialog FileDialog(TRUE, NULL, strDefault, OFN_HIDEREADONLY, strFilter, this);
	int i,nSheetCnt;
	BOOL bRead = FALSE;
	CString strSheetName , strSql;
	YAdoDatabase * pDB; 
	std::shared_ptr<CManagerEmergency> spManager;
	FileDialog.m_ofn.lpstrInitialDir = pRelayTable->GetProjectVersionPath();

	if (FileDialog.DoModal() != IDOK)
		return;
	pDB = pRelayTable->GetPrjDB();
	if (pDB == nullptr)
		return;

	CString strPath = FileDialog.GetPathName();
	if (xls.Open(strPath) == false)
	{
		AfxMessageBox(L"Excel파일을 여는데 실패 했습니다.");
		return;
	}

	

	nSheetCnt = xls.GetSheetCount();
	for (i = 0; i < nSheetCnt; i++)
	{
		// sheet 번호는 1부터
		if (xls.SetWorkSheetChange(i + 1) == FALSE)
			continue;
		strSheetName = xls.GetSheetName(i + 1);
		if (strSheetName.CompareNoCase(L"eb") != 0)
			continue;
		bRead = TRUE;
		break;
	}

	if (bRead == FALSE)
	{
		AfxMessageBox(L"Excel파일 내에 'EB' Sheet가 없습니다.");
		return;
	}

	spManager = pRelayTable->GetEmergencyManager();
	if (spManager)
		spManager->RemoveAllEmergency();

	strSql.Format(L"DELETE FROM TB_EM_BC");
	if (pDB)
		pDB->ExecuteSql(strSql);

	pRelayTable->ParsingEmergencyData(&xls , spManager);
	pRelayTable->InsertPrjBaseEmergencyDB();
	InitList();
	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(UWM_DKP_EMERGENCY_REFRESH, DATA_ALL, 0);
}


void CFormEmergency::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

CString CFormEmergency::MakeName()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData();
	m_strName = m_strBuild + L"#" + m_strStair + L"#" + m_strFloor;
	UpdateData(FALSE);
	return m_strName;
}

void CFormEmergency::OnEnChangeEdBuild()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MakeName();
}


void CFormEmergency::OnEnChangeEdStair()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MakeName();
}


void CFormEmergency::OnEnChangeEdFloor()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MakeName();
}


int CFormEmergency::SelectItem(int nItemType, DWORD_PTR dwData)
{
	int nRet = -1; 
	int nEmID = 0;
	if (nItemType == TTYPE_DEV_EMERGENCY)
		nEmID = dwData;
	else
		nEmID = ((CDataLinked*)dwData)->GetTgtFacp();

	if (nEmID <= 0)
		return 0;
	nRet = FindEmergencyItem(nEmID);
	if (nRet < 0)
		return 0;

	// 먼저 현재 선택상태를 해제합니다
	m_ctrlList.SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	// 원하는 아이템을 선택합니다
	m_ctrlList.SetItemState(nRet, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	// 선택된 아이템을 표시합니다
	m_ctrlList.EnsureVisible(nRet, false);
	// 리스트 컨트롤에 포커스를 맞춥니다
	m_ctrlList.SetFocus();
	return 1;
}

int CFormEmergency::FindEmergencyItem(int nEmID)
{
	int nCnt, i;
	CDataEmBc * pData;
	nCnt = m_ctrlList.GetItemCount();
	for (i = 0; i < nCnt; i++)
	{
		pData = (CDataEmBc *)m_ctrlList.GetItemData(i);
		if (pData == nullptr)
			continue; 
		if (nEmID == pData->GetEmID())
			return i; 
	}
	return -1;
}
