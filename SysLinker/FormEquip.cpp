// FormEquip.cpp : ���� �����Դϴ�.
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


// CFormEquip �����Դϴ�.

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


// CFormEquip �޽��� ó�����Դϴ�.
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

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CBitmap bmp;
	if (!bmp.LoadBitmap(IDB_BMP_EQUIP_ICON))
	{
		TRACE(_T("��Ʈ���� �ε��� �� �����ϴ�. %x\n"), IDB_BMP_EQUIP_ICON);
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CFormView::PreCreateWindow(cs);
}


void CFormEquip::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CFormView::OnClose();
}


int CFormEquip::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}


void CFormEquip::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_ctrlTree.GetSafeHwnd() == nullptr || m_ctrlPreview.GetSafeHwnd() == nullptr)
		return; 
	CRect rc,rcTree,rcImage;
	GetClientRect(&rc);
	rcTree.left = 5;
	rcTree.top = 5;
	rcTree.bottom = cy - 5;

	//20240325 GBM start - ��Ʈ���� �������� ���� ����
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//20240408 GBM start - �Է�Ÿ��/�����̸�/���Ÿ��/��³����� ��� ������� �ʵ��� ��
// #ifndef _DEBUG
// 	int nType = ET_NONE;
// 	nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
// 	if ((nType >= ET_INPUTTYPE) && (nType <= ET_OUTCONTENTS))
// 	{
// 		AfxMessageBox(_T("[�Է�Ÿ��/�����̸�/���Ÿ��/��³���]��\n�߰�� �϶�ǥ (WEB)���� ������ ������ �ּ���."));
// 		return;
// 	}
// #endif
	//20240408 GBM end

	AddInit();
}


void CFormEquip::OnBnClickedBtnSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//20240408 GBM start - �Է�Ÿ��/�����̸�/���Ÿ��/��³����� ��� ������� �ʵ��� ��
// #ifndef _DEBUG
// 	int nType = ET_NONE;
// 	nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
// 	if ((nType >= ET_INPUTTYPE) && (nType <= ET_OUTCONTENTS))
// 	{
// 		AfxMessageBox(_T("[�Է�Ÿ��/�����̸�/���Ÿ��/��³���]��\n�߰�� �϶�ǥ (WEB)���� ������ ������ �ּ���."));
// 		return;
// 	}
// #endif
	//20240408 GBM end

	//20240527 GBM start - ������� ��ȯ
#if 1
	UpdateData();
	
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
	CString strMsg = _T("���� ������ ���� ���Դϴ�. ��� ��ٷ� �ּ���.");
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadAddAndSaveEquip, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("������ ��� ����! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
		GF_AddLog(L"���� ���� ���忡 �����߽��ϴ�.");
		Log::Trace("The equipment information was saved successfully.");
	}
	else
	{
		GF_AddLog(L"���� ���� ���忡 �����߽��ϴ�.");
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//20240408 GBM start - �Է�Ÿ��/�����̸�/���Ÿ��/��³����� ��� ������� �ʵ��� ��
// #ifndef _DEBUG
// 	int nType = ET_NONE;
// 	nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
// 	if ((nType >= ET_INPUTTYPE) && (nType <= ET_OUTCONTENTS))
// 	{
// 		AfxMessageBox(_T("[�Է�Ÿ��/�����̸�/���Ÿ��/��³���]��\n�߰�� �϶�ǥ (WEB)���� ������ ������ �ּ���."));
// 		return;
// 	}
// #endif
	//20240408 GBM end

	if (m_pCurrentData == nullptr)
	{
		AfxMessageBox(L"���õ� ���� ������ �����ϴ�.");
		return;
	}

	if (AfxMessageBox(L"���õ� ���� ������ �����Ͻðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	//20240527 GBM start - ������� ��ȯ
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
	CString strMsg = _T("���� ������ ���� ���Դϴ�. ��� ��ٷ� �ּ���.");
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadDeleteEquip, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("������ ��� ����! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
		GF_AddLog(L"���� ���� ������ �����߽��ϴ�.");
		Log::Trace("The equipment information was deleted successfully.");
	}
	else
	{
		GF_AddLog(L"���� ���� ������ �����߽��ϴ�.");
		Log::Trace("Failed to delete the equipment information.");
	}
#else
	DataDelete();
#endif
	//20240527 GBM end
}


void CFormEquip::OnBnClickedBtnBrowser()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

	hRoot = m_ctrlTree.InsertItem(L"���� ����",0,0, TVI_ROOT);
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
	//20240422 GBM start - ���� ���� üũ
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


	// tree ����
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

	//20240411 GBM start - ������ �������� ���� ���Ÿ�Կ� ���� ��������Ű ���� ��� ��û�� ���� �� �����Ƿ� �ϴ� �۾��� ���� -> ���ǿ� ���� ���� ����
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
				strMsg2.Format(_T("���� ���� [%s ID - %d : %s]�� �߰�� �϶�ǥ ���Ͽ��� �����ϴ� ���� �����߽��ϴ�."), strType, nIndex, m_strName);
			}
			else
			{
				strMsg1.Format(_T("Deleting the new equipment [%s ID - %d : %s] definition from the module table file failed."), strType, nIndex, m_strName);
				strMsg2.Format(_T("���� ���� [%s ID - %d : %s]�� �߰�� �϶�ǥ ���Ͽ��� �����ϴ� ���� �����߽��ϴ�."), strType, nIndex, m_strName);
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

	//20240422 GBM start - ���� ���� üũ
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
		AfxMessageBox(L"�����͸� �����ϴµ� �����߽��ϴ�.");
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
	// GetRootItem --> TVI_ROOT ��ȯ
	// HTREEITEM hItem = GF_FindTreeByText(&m_ctrlTree, m_ctrlTree.GetRootItem(), g_strEquipTypeString[nType]);
	HTREEITEM hItem = GF_FindTreeByText(&m_ctrlTree, m_ctrlTree.GetRootItem(), g_strEquipTypeString[nType]);
	if (hItem == nullptr)
		return 0;
	HTREEITEM htemp = m_ctrlTree.InsertItem(m_strName, nType, nType, hItem);
	m_ctrlTree.SetItemData(htemp, (DWORD_PTR)m_pCurrentData);
	m_ctrlTree.SelectItem(htemp);
	m_bAdd = TRUE;

	//20240411 GBM start - ������ �������� ���� ���Ÿ�Կ� ���� ��������Ű ���� ��� ��û�� ���� �� �����Ƿ� �ϴ� �۾��� ���� -> ���ǿ� ���� ���� ����
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
				strMsg2.Format(_T("�� ���� ���� [%s ID - %d : %s]�� �߰�� �϶�ǥ ���Ͽ� �߰��ϴ� ���� �����߽��ϴ�."), strType, nID, m_strName);
			}
			else
			{
				strMsg1.Format(_T("Adding the new equipment [%s ID - %d : %s] definition to the module table file failed."), strType, nID, m_strName);
				strMsg2.Format(_T("�� ���� ���� [%s ID - %d : %s]�� �߰�� �϶�ǥ ���Ͽ� �߰��ϴ� ���� �����߽��ϴ�."), strType, nID, m_strName);
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
			if (AfxMessageBox(L"���� �����Ͱ� �����ϴ�. ���� �߰��Ͻðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
				return 0;
		}
		return DataAdd();
	}
	else
	{
		if (m_bAdd)
		{
			//20240422 GBM start - ���� ���� üũ
			int nType = m_cmbType.GetCurSel() + 1;	// Enum index == Combo box index + 1 
			int nID = m_nNum;
			if (!CheckEditableEquipment(EQUIPMENT_TYPE_MODIFY, nType, nID))
				return 0;
			//20240422 GBM end

			if (AfxMessageBox(L"�̹� �����Ͱ� �ֽ��ϴ�. ���������͸� �����Ͻðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
				return 0;
			std::shared_ptr <CManagerEquip> spManager = nullptr;
			spManager = m_pRefFasSysData->GetEquipManager(nType);
			if (spManager == nullptr)
			{
				AfxMessageBox(L"�����͸� �������µ� �����߽��ϴ�.");
				return 0;
			}

			m_pCurrentData = spManager->GetEquip(nID);
			if (m_pCurrentData == nullptr)
			{
				AfxMessageBox(L"�����͸� �������µ� �����߽��ϴ�.");
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
		AfxMessageBox(L"�����͸� �����ϴµ� �����߽��ϴ�.");
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

	//20240411 GBM start - ������ �������� ���� ���Ÿ�Կ� ���� ��������Ű ���� ��� ��û�� ���� �� �����Ƿ� �ϴ� �۾��� ���� -> ���ǿ� ���� ���� ����
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
				strMsg2.Format(_T("���� ���� [%s ID - %d : %s]�� �߰�� �϶�ǥ ���Ͽ��� �����ϴ� ���� �����߽��ϴ�."), strType, nID, m_strName);
			}
			else
			{
				strMsg1.Format(_T("Failed to modify the equipment [%s ID - %d : %s] in the module table file."), strType, nID, m_strName);
				strMsg2.Format(_T("���� ���� [%s ID - %d : %s]�� �߰�� �϶�ǥ ���Ͽ��� �����ϴ� ���� �����߽��ϴ�."), strType, nID, m_strName);
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CFormEquip::OnCbnSelchangeCmbEquiptype()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
			if (nID > EQUIPMENT_DEFINITION::�˼������Է�Ÿ�� && nID <= EQUIPMENT_DEFINITION::CCTV)
			{
				strMsg.Format(_T("[�Է� Ÿ�� ID : %d]�� �߰��� �� �����ϴ�."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
		else if (nEquimentType == ET_OUTPUTTYPE)
		{
			if (nID > EQUIPMENT_DEFINITION::�˼��������Ÿ�� && nID <= EQUIPMENT_DEFINITION::����������)
			{
				strMsg.Format(_T("[��� Ÿ�� ID : %d]�� �߰��� �� �����ϴ�."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
	}
	else if (nEditType == EQUIPMENT_TYPE_MODIFY)
	{
// 		if (nEquimentType == ET_INPUTTYPE)
// 		{
// 			if (nID > EQUIPMENT_DEFINITION::�˼������Է�Ÿ�� && nID <= EQUIPMENT_DEFINITION::CCTV)
// 			{
// 				strMsg.Format(_T("[�Է� Ÿ�� ID : %d]��/�� ������ �� �����ϴ�."), nID);
// 				AfxMessageBox(strMsg);
// 				return FALSE;
// 			}
// 		}
// 		else if (nEquimentType == ET_OUTPUTTYPE)
// 		{
// 			if (nID > EQUIPMENT_DEFINITION::�˼��������Ÿ�� && nID <= EQUIPMENT_DEFINITION::����������)
// 			{
// 				strMsg.Format(_T("[��� Ÿ�� ID : %d]��/�� ������ �� �����ϴ�."), nID);
// 				AfxMessageBox(strMsg);
// 				return FALSE;
// 			}
// 		}
	}
	else if (nEditType == EQUIPMENT_TYPE_DELETE)
	{
		if (nEquimentType == ET_INPUTTYPE)
		{
			if (nID > EQUIPMENT_DEFINITION::�˼������Է�Ÿ�� && nID <= EQUIPMENT_DEFINITION::CCTV)
			{
				strMsg.Format(_T("[�Է� Ÿ�� ID : %d]�� ������ �� �����ϴ�."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
		else if (nEquimentType == ET_OUTPUTTYPE)
		{
			if (nID > EQUIPMENT_DEFINITION::�˼��������Ÿ�� && nID <= EQUIPMENT_DEFINITION::����������)
			{
				strMsg.Format(_T("[��� Ÿ�� ID : %d]�� ������ �� �����ϴ�."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
		//20240430 GBM start - ���ȸ�� 3: ���� ���� �Ұ��� �ϵ��� ��
		else if (nEquimentType == ET_OUTCONTENTS)
		{
			if (nID == 3)
			{
				strMsg.Format(_T("[��� ȸ�� ID : %d] ���� ������ �� �����ϴ�."), nID);
				AfxMessageBox(strMsg);
				return FALSE;
			}
		}
		//20240430 GBM end
	}

	return TRUE;
}
