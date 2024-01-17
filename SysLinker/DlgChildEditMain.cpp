// DlgChildEditMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildEditMain.h"
#include "afxdialogex.h"
#include "DlgChildEditFacp.h"
#include "DlgChildEditChn.h"
#include "DlgChildEditUnit.h"
#include "DlgChildEditRelay.h"
#include "DlgChildEditEmpty.h"
#include "DataChannel.h"
#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"
// CDlgChildEditMain 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildEditMain, CDialogEx)

CDlgChildEditMain::CDlgChildEditMain(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CHILD_MAIN, pParent)
{
	for (int i = 0; i < LOCAL_D_MAX_PAGE; i++)
		m_pPage[i] = nullptr;
	m_pMainForm = nullptr;
	m_pRefFasSysData = nullptr;
	m_nAddType = -1;
	m_pRefCurData = nullptr;
	m_bPreviewComplete = FALSE;
	m_nAction = DATA_SAVE;
}

CDlgChildEditMain::~CDlgChildEditMain()
{
	for (int i = 0; i < LOCAL_D_MAX_PAGE; i++)
	{
		if (m_pPage[i] == nullptr)
			continue; 
		delete m_pPage[i];
		m_pPage[i] = nullptr;
	}
}

void CDlgChildEditMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_FRAME, m_stFrame);
}


BEGIN_MESSAGE_MAP(CDlgChildEditMain, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCANCEL, &CDlgChildEditMain::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgChildEditMain::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlgChildEditMain::OnBnClickedBtnAdd)
	ON_COMMAND(IDC_ADD_FACP, &CDlgChildEditMain::OnAddFacp)
	ON_COMMAND(IDC_ADD_RELAY, &CDlgChildEditMain::OnAddRelay)
	ON_COMMAND(IDC_ADD_UNIT, &CDlgChildEditMain::OnAddUnit)
	ON_COMMAND(IDC_ADD_CHN, &CDlgChildEditMain::OnAddChn)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgChildEditMain::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDlgChildEditMain::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, &CDlgChildEditMain::OnBnClickedBtnPreview)
END_MESSAGE_MAP()


// CDlgChildEditMain 메시지 처리기입니다.
BOOL CDlgChildEditMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	CreateAllPage();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgChildEditMain::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_stFrame.GetSafeHwnd() == nullptr)
		return; 
	int nWidth = 875;
	CRect rc;
	rc.left = rc.top = 2; 
	rc.right = cx-2 > nWidth ? nWidth : cx-2;
	rc.bottom = 135;
	m_stFrame.MoveWindow(&rc);
	rc.DeflateRect(2, 2, 3, 3);
	for (int i = 0; i < LOCAL_D_MAX_PAGE; i++)
	{
		m_pPage[i]->MoveWindow(&rc);
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
void CDlgChildEditMain::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}


void CDlgChildEditMain::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


int CDlgChildEditMain::CreateAllPage()
{
	CRect rc;
	m_stFrame.GetClientRect(&rc);
	m_stFrame.ClientToScreen(&rc);
	ScreenToClient(&rc);

	if (m_pPage[TTYPE_DEV_PRJNAME] == nullptr)
		m_pPage[TTYPE_DEV_PRJNAME] = new CDlgChildEditEmpty;
	m_pPage[TTYPE_DEV_PRJNAME]->CreateDlg(&m_stFrame);

	if (m_pPage[TTYPE_DEV_FACP] == nullptr)
		m_pPage[TTYPE_DEV_FACP] = new CDlgChildEditFacp;
	m_pPage[TTYPE_DEV_FACP]->CreateDlg(&m_stFrame);

	if (m_pPage[TTYPE_DEV_UNIT] == nullptr)
		m_pPage[TTYPE_DEV_UNIT] = new CDlgChildEditUnit;
	m_pPage[TTYPE_DEV_UNIT]->CreateDlg(&m_stFrame);

	if (m_pPage[TTYPE_DEV_CHANNEL] == nullptr)
		m_pPage[TTYPE_DEV_CHANNEL] = new CDlgChildEditChn;
	m_pPage[TTYPE_DEV_CHANNEL]->CreateDlg(&m_stFrame);

	if (m_pPage[TTYPE_DEV_DEVICE] == nullptr)
		m_pPage[TTYPE_DEV_DEVICE] = new CDlgChildEditRelay;
	m_pPage[TTYPE_DEV_DEVICE]->CreateDlg(&m_stFrame);

	int i = 0;
	for (i = 0; i < LOCAL_D_MAX_PAGE; i++)
	{
		if (m_pPage[i] == nullptr)
			continue; 
		m_pPage[i]->SetMainWindow(m_pMainForm);
		m_pPage[i]->SetEditDlg(this);
	}

	ShowChildPage(TTYPE_DEV_DEVICE);
	return 1;
}


void CDlgChildEditMain::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CRect rc;
	CMenu mn , * pContext; 
	GetDlgItem(IDC_BTN_ADD)->GetWindowRect(&rc);
	mn.LoadMenu(IDR_ADD_MENU);
	pContext = mn.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, rc.left, rc.bottom, this);
}


void CDlgChildEditMain::OnAddFacp()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_nAction = DATA_ADD;
	m_nAddType = TTYPE_DEV_FACP;
	m_bPreviewComplete = TRUE;
	ShowChildPage(m_nAddType);
	DeleteTreeItem(m_pRefNewData);
	SetNewTreeItem(SE_FACP, m_pRefNewData);
	m_pPage[m_nAddType]->DisplayItem(nullptr, m_pRefNewData);
	m_pPage[m_nAddType]->SetAddFlag(TRUE);
}


void CDlgChildEditMain::OnAddRelay()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_nAction = DATA_ADD;
	m_nAddType = TTYPE_DEV_DEVICE;
	ShowChildPage(m_nAddType);
	m_bPreviewComplete = TRUE;

	DeleteTreeItem(m_pRefNewData);
	SetNewTreeItem(SE_RELAY, m_pRefNewData);
	m_pPage[m_nAddType]->DisplayItem(nullptr, m_pRefNewData);
	m_pPage[m_nAddType]->SetAddFlag(TRUE);
}


void CDlgChildEditMain::OnAddUnit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_nAction = DATA_ADD;
	m_nAddType = TTYPE_DEV_UNIT;
	ShowChildPage(m_nAddType);
	m_bPreviewComplete = TRUE;

	DeleteTreeItem(m_pRefNewData);
	SetNewTreeItem(SE_UNIT, m_pRefNewData);
	m_pPage[m_nAddType]->DisplayItem(nullptr, m_pRefNewData);
	m_pPage[m_nAddType]->SetAddFlag(TRUE);
}


void CDlgChildEditMain::OnAddChn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_nAction = DATA_ADD;
	m_nAddType = TTYPE_DEV_CHANNEL;
	ShowChildPage(m_nAddType);
	m_bPreviewComplete = TRUE;
	DeleteTreeItem(m_pRefNewData);
	SetNewTreeItem(SE_CHANNEL, m_pRefNewData);
	m_pPage[m_nAddType]->DisplayItem(nullptr, m_pRefNewData);
	m_pPage[m_nAddType]->SetAddFlag(TRUE);
}


void CDlgChildEditMain::ShowChildPage(int nType)
{
	int i = 0; 
	m_nCurType = nType;
	for (i = 0; i < LOCAL_D_MAX_PAGE; i++)
	{
		if (m_pPage[i] == nullptr)
			continue; 
		if (i == nType)
			m_pPage[i]->ShowWindow(SW_SHOW);
		else
			m_pPage[i]->ShowWindow(SW_HIDE);
	}
}

int CDlgChildEditMain::TreeSelectChange(ST_TREEITEM * pCurItem, ST_TREEITEM * pNewItem)
{
	m_nAction = DATA_SAVE;
	m_pRefCurData = pCurItem;
	m_pRefNewData = pNewItem;
	if (pCurItem == nullptr)
	{
		ShowChildPage(-1);
		for (int i = 0; i < LOCAL_D_MAX_PAGE; i++)
		{
			if (m_pPage[i] == nullptr)
				continue;
			m_pPage[i]->DisplayItem(pCurItem, pNewItem);
		}
	}
	else
	{
		ShowChildPage(m_pRefCurData->nDataType);
		m_pPage[pCurItem->nDataType]->DisplayItem(pCurItem, pNewItem);
	}
	m_bPreviewComplete = FALSE;
	return 1;
}

LRESULT CDlgChildEditMain::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	if (wp == FORM_PRJ_CLOSE)
	{
		m_pRefFasSysData = nullptr;
		return 1;
	}


	return 1;
}


void CDlgChildEditMain::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bPreviewComplete == FALSE)
	{
		if (AfxMessageBox(L"미리보기 후 저장하는 것을 권장합니다.\n미리보기를 하시겠습니까?"
			, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnBnClickedBtnPreview();
			return;
		}
	}
	if (m_pPage[m_nCurType]->GetChangeData() == FALSE)
		return;
	if (m_pMainForm)
		m_pMainForm->SendMessage(UWM_RELAYEDITFORM_DATACHANGE, m_bPreviewComplete, m_nAction);
	m_nAction = DATA_SAVE;
}

void CDlgChildEditMain::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pRefCurData == nullptr)
		return; 
	m_nAction = DATA_DEL;

// 	if (m_bPreviewComplete == FALSE)
// 	{
// 		if (AfxMessageBox(L"미리보기 후 저장하는 것을 권장합니다.\n미리보기를 하시겠습니까?"
// 			, MB_YESNO | MB_ICONQUESTION) == IDYES)
// 		{
// 			OnBnClickedBtnPreview();
// 			return;
// 		}
// 	}
	if (m_pMainForm)
		m_pMainForm->SendMessage(UWM_RELAYEDITFORM_DATACHANGE, m_bPreviewComplete,DATA_DEL);

	//switch (m_pRefCurData->nDataType)
	//{
	//case TTYPE_DEV_FACP:
	//	break;
	//case TTYPE_DEV_UNIT:
	//	break;
	//case TTYPE_DEV_CHANNEL:
	//	break;
	//case TTYPE_DEV_DEVICE:
	//	break;
	//}
}

void CDlgChildEditMain::OnBnClickedBtnPreview()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pPage[m_nCurType]->GetChangeData() == FALSE)
		return;
	if (m_pMainForm)
		m_pMainForm->SendMessage(UWM_RELAYEDITFORM_PREVIEWCHANGE, m_nCurType, m_nAction);
	m_bPreviewComplete = TRUE;
}

void CDlgChildEditMain::CopyTreeItem(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData)
{
	if (pCurData == nullptr || pCurData->pData == nullptr)
		return; 
	CDataSystem * pSys = new CDataSystem;
	pNewData->hItem = pCurData->hItem;
	pNewData->hParent = pCurData->hParent;
	pNewData->nDataType = pCurData->nDataType;
	pSys->CopyData((CDataSystem*)pCurData->pData);
	pNewData->pData = pSys;
}

void CDlgChildEditMain::DeleteTreeItem(ST_TREEITEM * pNewData)
{
	if (pNewData == nullptr || pNewData->pData == nullptr)
		return;
	CDataSystem * pSys = (CDataSystem*)pNewData->pData;
	pSys->DeleteData();
	delete pSys;
	memset((void*)pNewData, 0, sizeof(ST_TREEITEM));
}

void CDlgChildEditMain::SetNewTreeItem(int nType , ST_TREEITEM * pNewData)
{
	CDataSystem * pSys = new CDataSystem;
	pSys->SetNewSysData(nType);
	pNewData->hItem = nullptr;
	pNewData->hParent = nullptr;
	pNewData->nDataType = nType;
	pNewData->pData = pSys;
}
