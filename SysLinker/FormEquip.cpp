// FormEquip.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormEquip.h"

#include "DataEquip.h"
#include "DataFacp.h"
#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"

UINT ThreadAddAndSaveEquip(LPVOID pParam)
{
	CFormEquip* pFE = (CFormEquip*)pParam;

	if (pFE->m_bAdd)
		pFE->m_bThreadSucceeded = pFE->DataAdd();
	else
		pFE->m_bThreadSucceeded = pFE->DataSave();

	pFE->m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(pFE->m_hThreadHandle);

	return 0;
}

UINT ThreadDeleteEquip(LPVOID pParam)
{
	CFormEquip* pFE = (CFormEquip*)pParam;

	pFE->m_bThreadSucceeded = pFE->DataDelete();

	pFE->m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(pFE->m_hThreadHandle);

	return 0;
}


// CFormEquip

enum
{
	EQUIPMENT_TYPE_ADD,
	EQUIPMENT_TYPE_MODIFY,
	EQUIPMENT_TYPE_DELETE
}EDIT_TYPE_ENUM;

IMPLEMENT_DYNCREATE(CFormEquip, CFormView)

CFormEquip::CFormEquip()
	: CFormView(IDD_FORMEQUIP)
	, m_strName(_T(""))
	, m_nNum(0)
	, m_strSymbol(_T(""))
{
	m_pRefFasSysData = nullptr;
	m_pCurrentData = nullptr;
	m_bAdd = FALSE;
}

CFormEquip::~CFormEquip()
{
	m_pCurrentData = nullptr;
}

void CFormEquip::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EQUIP_TREE, m_ctrlTree);
	DDX_Text(pDX, IDC_ED_EQUIP_NAME, m_strName);
	DDX_Control(pDX, IDC_CMB_EQUIPTYPE, m_cmbType);
	DDX_Text(pDX, IDC_ED_EQUIP_ID, m_nNum);
	DDX_Text(pDX, IDC_ED_EQUIP_SYMBOL, m_strSymbol);
	DDX_Control(pDX, IDC_ST_PREVIEW, m_ctrlPreview);
}

BEGIN_MESSAGE_MAP(CFormEquip, CFormView)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_ADD, &CFormEquip::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFormEquip::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CFormEquip::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CFormEquip::OnBnClickedBtnBrowser)
	ON_NOTIFY(TVN_SELCHANGED, IDC_EQUIP_TREE, &CFormEquip::OnTvnSelchangedEquipTree)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
	ON_CBN_SELCHANGE(IDC_CMB_EQUIPTYPE, &CFormEquip::OnCbnSelchangeCmbEquiptype)
END_MESSAGE_MAP()


// CFormEquip 진단입니다.

#ifdef _DEBUG
void CFormEquip::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormEquip::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormEquip 메시지 처리기입니다.
LRESULT CFormEquip::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	if (wp == FORM_PRJ_OPEN || wp == FORM_PRJ_NEW)
	{
		if (AfxGetMainWnd())
		{
			m_pRefFasSysData = theApp.GetRelayTableData();
		}
		if (m_pRefFasSysData == nullptr)
			return 0;
		m_strFileRootPath = m_pRefFasSysData->GetProjectVersionPath() + F3_PRJ_DIR_SYMBOL;
		m_strFileRootPath += L"\\";
		FillCombo();
		FillEquipTree();
	}
	return 1;
}

void CFormEquip::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CBitmap bmp;
	if (!bmp.LoadBitmap(IDB_BMP_EQUIP_ICON))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), IDB_BMP_EQUIP_ICON);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ImgList.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ImgList.Add(&bmp, RGB(0, 255, 255));

	m_ctrlTree.SetImageList(&m_ImgList, TVSIL_NORMAL);

	if (AfxGetMainWnd())
	{
		m_pRefFasSysData = theApp.GetRelayTableData();
	}
	if (m_pRefFasSysData == nullptr)
		return;
	m_strFileRootPath = m_pRefFasSysData->GetProjectVersionPath() + F3_PRJ_DIR_SYMBOL;
	m_strFileRootPath += L"\\";
	FillCombo();
	FillEquipTree();
}


BOOL CFormEquip::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::PreCreateWindow(cs);
}


void CFormEquip::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnClose();
}


int CFormEquip::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CFormEquip::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_ctrlTree.GetSafeHwnd() == nullptr || m_ctrlPreview.GetSafeHwnd() == nullptr)
		return; 
	CRect rc,rcTree,rcImage;
	GetClientRect(&rc);
	rcTree.left = 5;
	rcTree.top = 5;
	rcTree.bottom = cy - 5;

	//20240325 GBM start - 컨트롤이 가려지는 오류 수정
	rcTree.right = 250;
	//rcTree.right = 350;
	//20240325 GBM end

	m_ctrlTree.MoveWindow(&rcTree);

	rcImage.left = rcTree.right + 5;
	rcImage.right = cx - 5;
	rcImage.top = 180;
	rcImage.bottom = cy - 5;
	m_ctrlPreview.MoveWindow(&rcImage);
}


void CFormEquip::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//20240408 GBM start - 입력타입/설비이름/출력타입/출력내용일 경우 진행되지 않도록 함
// #ifndef _DEBUG
// 	int nType = ET_NONE;
// 	nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
// 	if ((nType >= ET_INPUTTYPE) && (nType <= ET_OUTCONTENTS))
// 	{
// 		AfxMessageBox(_T("[입력타입/설비이름/출력타입/출력내용]은\n중계기 일람표 (WEB)에서 편집을 진행해 주세요."));
// 		return;
// 	}
// #endif
	//20240408 GBM end

	AddInit();
}


void CFormEquip::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//20240408 GBM start - 입력타입/설비이름/출력타입/출력내용일 경우 진행되지 않도록 함
// #ifndef _DEBUG
// 	int nType = ET_NONE;
// 	nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
// 	if ((nType >= ET_INPUTTYPE) && (nType <= ET_OUTCONTENTS))
// 	{
// 		AfxMessageBox(_T("[입력타입/설비이름/출력타입/출력내용]은\n중계기 일람표 (WEB)에서 편집을 진행해 주세요."));
// 		return;
// 	}
// #endif
	//20240408 GBM end

	//20240527 GBM start - 스레드로 전환
#if 1
	UpdateData();
	
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
	CString strMsg = _T("설비 정보를 저장 중입니다. 잠시 기다려 주세요.");
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadAddAndSaveEquip, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
		GF_AddLog(L"설비 정보 저장에 성공했습니다.");
		Log::Trace("The equipment information was saved successfully.");
	}
	else
	{
		GF_AddLog(L"설비 정보 저장에 실패했습니다.");
		Log::Trace("Failed to save the equipment information.");
	}
#else
	UpdateData();
	if (m_bAdd)
		DataAdd();
	else
		DataSave();
#endif
	//20240527 GBM end
}


void CFormEquip::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//20240408 GBM start - 입력타입/설비이름/출력타입/출력내용일 경우 진행되지 않도록 함
// #ifndef _DEBUG
// 	int nType = ET_NONE;
// 	nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
// 	if ((nType >= ET_INPUTTYPE) && (nType <= ET_OUTCONTENTS))
// 	{
// 		AfxMessageBox(_T("[입력타입/설비이름/출력타입/출력내용]은\n중계기 일람표 (WEB)에서 편집을 진행해 주세요."));
// 		return;
// 	}
// #endif
	//20240408 GBM end

	if (m_pCurrentData == nullptr)
	{
		AfxMessageBox(L"선택된 설비 정보가 없습니다.");
		return;
	}

	if (AfxMessageBox(L"선택된 설비 정보를 삭제하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	//20240527 GBM start - 스레드로 전환
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
	CString strMsg = _T("설비 정보를 삭제 중입니다. 잠시 기다려 주세요.");
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadDeleteEquip, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
		GF_AddLog(L"설비 정보 삭제에 성공했습니다.");
		Log::Trace("The equipment information was deleted successfully.");
	}
	else
	{
		GF_AddLog(L"설비 정보 삭제에 실패했습니다.");
		Log::Trace("Failed to delete the equipment information.");
	}
#else
	DataDelete();
#endif
	//20240527 GBM end
}


void CFormEquip::OnBnClickedBtnBrowser()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strDefault = L"*.Bmp", strFilter = L"Symbol File (*.Bmp)|*.Bmp|All Files (*.*)|*.*||";
	CFileDialog FileDialog(TRUE, NULL, strDefault, OFN_HIDEREADONLY, strFilter, this);
	FileDialog.m_ofn.lpstrInitialDir = m_strFileRootPath;

	if (FileDialog.DoModal() != IDOK)
		return;

	CString strPath = FileDialog.GetFileName();
	m_strSymbol = strPath;
	m_ctrlPreview.SetImageFile(m_strFileRootPath + strPath);
	//m_ctrlPreview.SetImageFile(L"D:\\04. Ficon3\\01. Code\\Run\\x64\\Debug\\Symbol\\" + strPath);
	UpdateData(FALSE);
}


void CFormEquip::OnTvnSelchangedEquipTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem == nullptr)
		return;

	AddCancel();
	DisplayEquip(hItem);

}

int CFormEquip::FillCombo()
{
	int nType;
	if (m_pRefFasSysData == nullptr)
		m_pRefFasSysData = theApp.GetRelayTableData();
	int nIdx = 0;

	for (nType = ET_INPUTTYPE; nType < ET_ALL; nType++)
	{
		m_cmbType.InsertString(nType - 1, g_strEquipTypeString[nType]);
		m_cmbType.SetItemData(nType - 1, nType);
	}
	m_cmbType.SetCurSel(0);
	return 0;
}


int CFormEquip::FillEquipTree()
{
	m_ctrlTree.DeleteAllItems();

	HTREEITEM hParent = nullptr , hRoot , hItem;
	YAdoDatabase * pDb = nullptr;
	CString strSql, str;
	POSITION pos;
	int nType , nID , nTemp;
	CDataEquip * pEq;
	CString strTypeName, strDesc, strName, strSymbol;
	std::shared_ptr <CManagerEquip> spManager = nullptr;

	hRoot = m_ctrlTree.InsertItem(L"설비 정보",0,0, TVI_ROOT);
	if (m_pRefFasSysData == nullptr)
		return 0; 

	for (nType = ET_INPUTTYPE; nType < ET_ALL; nType++)
	{
		hParent = m_ctrlTree.InsertItem(g_strEquipTypeString[nType], nType , nType, hRoot);
		spManager = m_pRefFasSysData->GetEquipManager(nType);
		if (spManager == nullptr)
			continue; 
		pos = spManager->GetHeadPosition();
		while (pos)
		{
			pEq = spManager->GetNext(pos);
			if (pEq == nullptr)
				continue;
			nTemp = (int)pEq->GetEquipType();
			if (nType != nTemp)
				continue;

			nID = (int)pEq->GetEquipID();
			strName = pEq->GetEquipName();
			hItem = m_ctrlTree.InsertItem(strName, nType, nType, hParent);
			m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pEq);
		}
	}
	return 1;
}

void CFormEquip::AddInit()
{
	m_bAdd = TRUE;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(FALSE);
	CString strPath;
	m_strSymbol = L"BASIC.BMP";
	strPath = m_strFileRootPath + m_strSymbol;
	m_ctrlPreview.SetImageFile(strPath);
	m_nNum = 0; 
	m_strName = L"";
	m_cmbType.SetCurSel(-1);
	m_pCurrentData = nullptr;
	UpdateData(FALSE);
}


void CFormEquip::AddCancel()
{
	m_bAdd = FALSE;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(TRUE);
}


int CFormEquip::DisplayEquip(HTREEITEM hItem)
{
	CDataEquip * pEq = nullptr;

	if (hItem == nullptr)
		return 0;
	pEq = (CDataEquip*)m_ctrlTree.GetItemData(hItem);
	if (pEq == nullptr)
		return 0; 
	
	m_pCurrentData = pEq;
	m_nNum = (int)pEq->GetEquipID();
	m_strName = pEq->GetEquipName();
	m_cmbType.SetCurSel((int)pEq->GetEquipType() - 1);
	m_strSymbol = pEq->GetFileName();
	CString strPath;
	strPath = m_strFileRootPath + m_strSymbol;
	m_ctrlPreview.SetImageFile(strPath);
	UpdateData(FALSE);
	return 1;
}

int CFormEquip::DataDelete()
{
	//20240422 GBM start - 편집 여부 체크
	int nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
	int nID = m_nNum;
	if (!CheckEditableEquipment(EQUIPMENT_TYPE_DELETE, nType, nID))
		return 0;
	//20240422 GBM end

	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB == nullptr)
		return 0;

	int nCnt;
	CString strSql;
	CDataEquip * pEq = nullptr;

	pEq = m_pCurrentData;
	strSql.Format(L"SELECT * FROM TB_EQUIP_MST WHERE EQ_ID=%d AND EQ_TYPE=%d"
		, pEq->GetEquipID(), pEq->GetEquipType()
	);

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	nCnt = pDB->GetRecordCount();
	pDB->RSClose();
	if (nCnt <= 0)
		return 2;

	strSql.Format(L"DELETE TB_EQUIP_MST WHERE  EQ_ID=%d AND EQ_TYPE=%d"
		, pEq->GetEquipID(), pEq->GetEquipType()
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;


	// tree 삭제
	std::shared_ptr <CManagerEquip> spManager = nullptr;
	spManager = m_pRefFasSysData->GetEquipManager(pEq->GetEquipType());
	if (spManager == nullptr)
		return 0;

	spManager->RemoveEquip(pEq->GetEquipID());
	//spManager->RemoveAt(m_pCurrentData->GetEquipID());
	//spManager->SetAt(m_pCurrentData->GetEquipID(),nullptr);


	//HTREEITEM hItem = GF_FindTreeByText(&m_ctrlTree, TVI_ROOT, m_pCurrentData->GetEquipName());
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	if (hItem)
		m_ctrlTree.DeleteItem(hItem);

	//20240411 GBM start - 편집을 막았지만 추후 출력타입에 의한 연동정지키 편집 기능 요청이 있을 수 있으므로 일단 작업해 놓음 -> 조건에 따라 편집 가능
	if (pEq->GetEquipType() >= ET_INPUTTYPE && pEq->GetEquipType() <= ET_OUTCONTENTS)
	{
		BOOL bRet = FALSE;
		for (int nFacp = 0; nFacp < MAX_FACP_COUNT; nFacp++)
		{
			int nFacpType = -1;
			nFacpType = CNewInfo::Instance()->m_gi.facpType[nFacp];
			if (nFacpType == GT1)
			{
				bRet = TRUE;
				break;
			}
		}

		if (bRet)
		{
			int nType = pEq->GetEquipType();
			int nIndex = pEq->GetEquipID();
			CString strWin32AppProjectName = theApp.m_pFasSysData->GetPrjName();

			CString strType = g_strEquipTypeString[nType];
			CString strMsg1 = _T("");
			CString strMsg2 = _T("");

			bRet = CNewExcelManager::Instance()->UpdateOneEquipmentInfo(nType, pEq->GetEquipID(), _T(""), strWin32AppProjectName);
			if (bRet)
			{
				strMsg1.Format(_T("The equipment [%s ID - %d : %s] definition has been successfully deleted from the module table file."), strType, nIndex, m_strName);
				strMsg2.Format(_T("설비 정의 [%s ID - %d : %s]를 중계기 일람표 파일에서 삭제하는 데에 성공했습니다."), strType, nIndex, m_strName);
			}
			else
			{
				strMsg1.Format(_T("Deleting the new equipment [%s ID - %d : %s] definition from the module table file failed."), strType, nIndex, m_strName);
				strMsg2.Format(_T("설비 정의 [%s ID - %d : %s]를 중계기 일람표 파일에서 삭제하는 데에 실패했습니다."), strType, nIndex, m_strName);
			}

			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg1.GetBuffer(0)));
			GF_AddLog(strMsg2);
		}
	}
	//20240411 GBM end

	delete pEq;
	pEq = nullptr;

	m_pCurrentData = nullptr;

	return 1;
}

int CFormEquip::DataAdd()
{
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB == nullptr)
		return 0;

	CString strSql;
	int nID, nType, nSel;
	//UpdateData();
	nID = m_nNum;
	nSel = m_cmbType.GetCurSel();
	if (nSel < 0)
		return 0;
	nType = (int)m_cmbType.GetItemData(nSel);

	//20240422 GBM start - 편집 여부 체크
	if (!CheckEditableEquipment(EQUIPMENT_TYPE_ADD, nType, nID))
		return 0;
	//20240422 GBM end
	
	strSql.Format(L"INSERT TB_EQUIP_MST(EQ_ID,EQ_TYPE , EQ_NAME ,EQ_DESC,EQ_SYMBOL,ADD_USER) "
		L" VALUES(%d,%d"
		L",'%s','%s','%s','%s')"
		, nID,nType
		,m_strName,m_strName,m_strSymbol,m_pRefFasSysData->GetCurrentUser()
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"데이터를 수정하는데 실패했습니다.");
		return 0;
	}

	m_pCurrentData = new CDataEquip;
	m_pCurrentData->SetData(nID, nType, m_strName, L"", m_strSymbol);
	std::shared_ptr <CManagerEquip> spManager = nullptr;
	spManager = m_pRefFasSysData->GetEquipManager((int)m_pCurrentData->GetEquipType());
	if (spManager == nullptr)
		return 0;
	spManager->AddTail(m_pCurrentData);

	//HTREEITEM hItem = GF_FindTreeByText(&m_ctrlTree, TVI_ROOT, g_strEquipTypeString[nType]);
	// [KHS 2020-2-7 16:41:14] 
	// GetRootItem --> TVI_ROOT 변환
	// HTREEITEM hItem = GF_FindTreeByText(&m_ctrlTree, m_ctrlTree.GetRootItem(), g_strEquipTypeString[nType]);
	HTREEITEM hItem = GF_FindTreeByText(&m_ctrlTree, m_ctrlTree.GetRootItem(), g_strEquipTypeString[nType]);
	if (hItem == nullptr)
		return 0;
	HTREEITEM htemp = m_ctrlTree.InsertItem(m_strName, nType, nType, hItem);
	m_ctrlTree.SetItemData(htemp, (DWORD_PTR)m_pCurrentData);
	m_ctrlTree.SelectItem(htemp);
	m_bAdd = TRUE;

	//20240411 GBM start - 편집을 막았지만 추후 출력타입에 의한 연동정지키 편집 기능 요청이 있을 수 있으므로 일단 작업해 놓음 -> 조건에 따라 편집 가능
	if (nType >= ET_INPUTTYPE && nType <= ET_OUTCONTENTS)
	{
		BOOL bRet = FALSE;
		for (int nFacp = 0; nFacp < MAX_FACP_COUNT; nFacp++)
		{
			int nFacpType = -1;
			nFacpType = CNewInfo::Instance()->m_gi.facpType[nFacp];
			if (nFacpType == GT1)
			{
				bRet = TRUE;
				break;
			}
		}

		if (bRet)
		{
			CString strWin32AppProjectName = theApp.m_pFasSysData->GetPrjName();
			CString strType = g_strEquipTypeString[nType];
			CString strMsg1 = _T("");
			CString strMsg2 = _T("");

			bRet = CNewExcelManager::Instance()->UpdateOneEquipmentInfo(nType, nID, m_strName, strWin32AppProjectName);
			if (bRet)
			{
				strMsg1.Format(_T("The new equipment [%s ID - %d : %s] definition has been successfully added to the module table file."), strType, nID, m_strName);
				strMsg2.Format(_T("새 설비 정의 [%s ID - %d : %s]를 중계기 일람표 파일에 추가하는 데에 성공했습니다."), strType, nID, m_strName);
			}
			else
			{
				strMsg1.Format(_T("Adding the new equipment [%s ID - %d : %s] definition to the module table file failed."), strType, nID, m_strName);
				strMsg2.Format(_T("새 설비 정의 [%s ID - %d : %s]를 중계기 일람표 파일에 추가하는 데에 실패했습니다."), strType, nID, m_strName);
			}

			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg1.GetBuffer(0)));
			GF_AddLog(strMsg2);
		}
	}
	//20240411 GBM end

	return 1;
}


int CFormEquip::DataSave()
{
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB == nullptr)
		return 0;

	int nCnt;
	CString strSql;
	int nID, nType, nSel;
	//UpdateData();
	if (m_pCurrentData == nullptr)
	{
		nID = m_nNum;
		nSel = m_cmbType.GetCurSel();
		if (nSel < 0)
			return 0;
		nType = (int)m_cmbType.GetItemData(nSel);
	}
	else
	{
		nID = (int)m_pCurrentData->GetEquipID();
		nType = (int)m_pCurrentData->GetEquipType();
	}

	strSql.Format(L"SELECT * FROM TB_EQUIP_MST WHERE EQ_ID=%d AND EQ_TYPE=%d "
		, nID, nType
	);

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

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
	else
	{
		if (m_bAdd)
		{
			//20240422 GBM start - 편집 여부 체크
			int nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
			int nID = m_nNum;
			if (!CheckEditableEquipment(EQUIPMENT_TYPE_MODIFY, nType, nID))
				return 0;
			//20240422 GBM end

			if (AfxMessageBox(L"이미 데이터가 있습니다. 기존데이터를 수정하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
				return 0;
			std::shared_ptr <CManagerEquip> spManager = nullptr;
			spManager = m_pRefFasSysData->GetEquipManager(nType);
			if (spManager == nullptr)
			{
				AfxMessageBox(L"데이터를 가져오는데 실패했습니다.");
				return 0;
			}

			m_pCurrentData = spManager->GetEquip(nID);
			if (m_pCurrentData == nullptr)
			{
				AfxMessageBox(L"데이터를 가져오는데 실패했습니다.");
				return 0;
			}
		}
	}

	strSql.Format(L"UPDATE TB_EQUIP_MST SET EQ_NAME='%s', EQ_DESC='%s', EQ_SYMBOL='%s' "
		L" WHERE EQ_ID=%d AND EQ_TYPE=%d  "
		, m_strName, m_strName, m_strSymbol
		, nID, nType
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"데이터를 수정하는데 실패했습니다.");
		return 0;
	}

	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	if (hItem == nullptr)
		return 0;
	if (m_pCurrentData->GetEquipName().CompareNoCase(m_strName) != 0)
	{
		m_pCurrentData->SetEquipName(m_strName);
		if (m_pRefFasSysData)
			m_pRefFasSysData->ChangeDevName(m_pCurrentData->GetEquipType(), m_pCurrentData->GetEquipID());

		if (AfxGetApp())
			((CSysLinkerApp*)AfxGetApp())->PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
	}
	m_pCurrentData->SetEquipName(m_strName);
	m_pCurrentData->SetFileName(m_strSymbol);
	m_ctrlTree.SetItemText(hItem, m_pCurrentData->GetEquipName());

	//20240411 GBM start - 편집을 막았지만 추후 출력타입에 의한 연동정지키 편집 기능 요청이 있을 수 있으므로 일단 작업해 놓음 -> 조건에 따라 편집 가능
	if (nType >= ET_INPUTTYPE && nType <= ET_OUTCONTENTS)
	{
		BOOL bRet = FALSE;
		for (int nFacp = 0; nFacp < MAX_FACP_COUNT; nFacp++)
		{
			int nFacpType = -1;
			nFacpType = CNewInfo::Instance()->m_gi.facpType[nFacp];
			if (nFacpType == GT1)
			{
				bRet = TRUE;
				break;
			}
		}

		if (bRet)
		{
			CString strWin32AppProjectName = theApp.m_pFasSysData->GetPrjName();
			CString strType = g_strEquipTypeString[nType];
			CString strMsg1 = _T("");
			CString strMsg2 = _T("");

			bRet = CNewExcelManager::Instance()->UpdateOneEquipmentInfo(nType, nID, m_strName, strWin32AppProjectName);
			if (bRet)
			{
				strMsg1.Format(_T("The equipment [%s ID - %d : %s] definition was successfully modified in the module table file."), strType, nID, m_strName);
				strMsg2.Format(_T("설비 정의 [%s ID - %d : %s]를 중계기 일람표 파일에서 수정하는 데에 성공했습니다."), strType, nID, m_strName);
			}
			else
			{
				strMsg1.Format(_T("Failed to modify the equipment [%s ID - %d : %s] in the module table file."), strType, nID, m_strName);
				strMsg2.Format(_T("설비 정의 [%s ID - %d : %s]를 중계기 일람표 파일에서 수정하는 데에 실패했습니다."), strType, nID, m_strName);
			}

			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg1.GetBuffer(0)));
			GF_AddLog(strMsg2);
		}
	}
	//20240411 GBM end

	return 1;
}


void CFormEquip::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CFormEquip::OnCbnSelchangeCmbEquiptype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel,nType , nWhole = 0;
	CString str;
	if (m_bAdd)
	{
		if (m_pRefFasSysData == nullptr)
			return;
		nSel = m_cmbType.GetCurSel();
		if (nSel < 0)
			return;
		nType = (int)m_cmbType.GetItemData(nSel);
		nWhole = m_pRefFasSysData->GetWholeEquipID(nType);
		str.Format(L"%d" , nWhole);
		GetDlgItem(IDC_ED_EQUIP_ID)->SetWindowText(str);
	}
}

BOOL CFormEquip::CheckEditableEquipment(int nEditType, int nEquimentType, int nID)
{
	CString strMsg;

	if (nEditType == EQUIPMENT_TYPE_ADD)
	{
		if (nEquimentType == ET_INPUTTYPE)
		{
			if (nID > EQUIPMENT_DEFINITION::알수없는입력타입 && nID <= EQUIPMENT_DEFINITION::CCTV)
			{
				strMsg.Format(_T("[입력 타입 ID : %d]는 추가할 수 없습니다."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
		else if (nEquimentType == ET_OUTPUTTYPE)
		{
			if (nID > EQUIPMENT_DEFINITION::알수없는출력타입 && nID <= EQUIPMENT_DEFINITION::유도등정지)
			{
				strMsg.Format(_T("[출력 타입 ID : %d]는 추가할 수 없습니다."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
	}
	else if (nEditType == EQUIPMENT_TYPE_MODIFY)
	{
// 		if (nEquimentType == ET_INPUTTYPE)
// 		{
// 			if (nID > EQUIPMENT_DEFINITION::알수없는입력타입 && nID <= EQUIPMENT_DEFINITION::CCTV)
// 			{
// 				strMsg.Format(_T("[입력 타입 ID : %d]는/은 변경할 수 없습니다."), nID);
// 				AfxMessageBox(strMsg);
// 				return FALSE;
// 			}
// 		}
// 		else if (nEquimentType == ET_OUTPUTTYPE)
// 		{
// 			if (nID > EQUIPMENT_DEFINITION::알수없는출력타입 && nID <= EQUIPMENT_DEFINITION::유도등정지)
// 			{
// 				strMsg.Format(_T("[출력 타입 ID : %d]는/은 변경할 수 없습니다."), nID);
// 				AfxMessageBox(strMsg);
// 				return FALSE;
// 			}
// 		}
	}
	else if (nEditType == EQUIPMENT_TYPE_DELETE)
	{
		if (nEquimentType == ET_INPUTTYPE)
		{
			if (nID > EQUIPMENT_DEFINITION::알수없는입력타입 && nID <= EQUIPMENT_DEFINITION::CCTV)
			{
				strMsg.Format(_T("[입력 타입 ID : %d]는 삭제할 수 없습니다."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
		else if (nEquimentType == ET_OUTPUTTYPE)
		{
			if (nID > EQUIPMENT_DEFINITION::알수없는출력타입 && nID <= EQUIPMENT_DEFINITION::유도등정지)
			{
				strMsg.Format(_T("[출력 타입 ID : %d]는 삭제할 수 없습니다."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
		//20240430 GBM start - 출력회로 3: 밸브는 삭제 불가능 하도록 함
		else if (nEquimentType == ET_OUTCONTENTS)
		{
			if (nID == 3)
			{
				strMsg.Format(_T("[출력 회로 ID : %d] 밸브는 삭제할 수 없습니다."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
		//20240430 GBM end
	}

	return TRUE;
}
