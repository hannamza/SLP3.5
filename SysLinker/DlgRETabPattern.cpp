// DlgRETabPattern.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgRETabPattern.h"
#include "afxdialogex.h"
#include "DataPattern.h"
#include "DataChannel.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataSystem.h"
#include "DataLinked.h"
#include "MapSystemData.h"
#include "DataDevice.h"
#include "RelayTableData.h"
#include	"../Common/Utils/YAdoDatabase.h"

// CDlgRETabPattern 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRETabPattern, CDialogEx)

CDlgRETabPattern::CDlgRETabPattern(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_TAB_PATTERN, pParent)
{
	m_pRefCurData = nullptr;
	m_pRefNewData = nullptr;
	m_pRefFasSysData = nullptr;
	m_pRefChangeList = nullptr;
	m_pRefMapChangeDevice = nullptr;
	m_nAction = DATA_SAVE;
}

CDlgRETabPattern::~CDlgRETabPattern()
{
}

void CDlgRETabPattern::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AFTER_LIST, m_ctrlAfterList);
	DDX_Control(pDX, IDC_TREE, m_ctrlTree);
}


BEGIN_MESSAGE_MAP(CDlgRETabPattern, CDialogEx)
//	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CDlgRETabPattern::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRETabPattern::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CDlgRETabPattern::OnTvnSelchangedTree)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDlgRETabPattern::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgRETabPattern::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CDlgRETabPattern 메시지 처리기입니다.


BOOL CDlgRETabPattern::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pRefFasSysData = theApp.GetRelayTableData();
	m_ImageTree.Create(IDB_BMP_DEVICE_ICON, 16, 8, RGB(0, 255, 255));
	m_ctrlTree.SetImageList(&m_ImageTree, TVSIL_NORMAL);
	m_ctrlAfterList.InsertColumn(0, _T("기존 감지기/중계기"), LVCFMT_LEFT, 250);
	m_ctrlAfterList.InsertColumn(1, _T("새 감지기/중계기"), LVCFMT_LEFT, 250);
	m_ctrlAfterList.InsertColumn(2, _T("적용"), 120);
	m_ctrlAfterList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0
		, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgRETabPattern::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnClose();
}


void CDlgRETabPattern::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rcLeft,rcRight;
	rcLeft = CRect(4, 35, cx / 2 - 4 - 2, cy - 4);
	rcRight = CRect(rcLeft.right + 4, 35, cx -4, cy - 4);
	if (m_ctrlTree.GetSafeHwnd())
		m_ctrlTree.MoveWindow(&rcLeft);
	if (m_ctrlAfterList.GetSafeHwnd())
		m_ctrlAfterList.MoveWindow(&rcRight);
}


void CDlgRETabPattern::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}

void CDlgRETabPattern::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}
void CDlgRETabPattern::RefreshChange(int nDataType, int nAction)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nAction = nAction;
	RemoveAllData();
	FillTree(nDataType);
}

void CDlgRETabPattern::RemoveAllData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_ctrlTree.GetSafeHwnd())
		m_ctrlTree.DeleteAllItems();
	if (m_ctrlAfterList.GetSafeHwnd())
		m_ctrlAfterList.DeleteAllItems();
}

void CDlgRETabPattern::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	if (pNMTreeView->itemNew.hItem == nullptr)
		return;
	m_ctrlAfterList.DeleteAllItems();
	if (m_ctrlTree.GetItemData(pNMTreeView->itemNew.hItem) == 0)
		return;
	CDataPattern * pPtn = (CDataPattern*)m_ctrlTree.GetItemData(pNMTreeView->itemNew.hItem);
	if (pPtn)
		FillPatternList(pPtn);
}


void CDlgRETabPattern::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_nAction == DATA_DEL)
	{
		AfxMessageBox(L"삭제 시에는 영향을 받는 항목을 삭제할 수 없습니다.");
		return;
	}
	int i, nCnt, nChk;
	ST_CHANGE_LINKDATA * pData;
	nCnt = m_ctrlAfterList.GetItemCount();
	for (i = nCnt - 1; i >= 0; i--)
	{
		nChk = m_ctrlAfterList.GetItemCheckedState(i, 2);
		if (nChk != 1)
			continue;

		pData = (ST_CHANGE_LINKDATA *)m_ctrlAfterList.GetItemData(i);
		if (pData == nullptr)
			continue;

		pData->btChangeType = CHANGE_ACTION_DEL;
		m_ctrlAfterList.DeleteItem(i);
	}
}


void CDlgRETabPattern::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CDlgRETabPattern::GetIDNum(ST_TREEITEM* pTreeItem, int &nFid, int &nUid, int &nCid, int &nDevid
	, int &nFNum, int &nUNum, int &nCNum, int &nDevNum
)
{
	CDataSystem * pData;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev = nullptr;
	nFid = nUid = nCid = nDevid = -1;
	nFNum = nUNum = nCNum = nDevNum = -1;

	if (pTreeItem == nullptr || pTreeItem->pData == nullptr)
		return FALSE;
	pData = (CDataSystem *)pTreeItem->pData;
	switch (pTreeItem->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pData->GetSysData();
		nFid = pFacp->GetFacpID();
		nFNum = pFacp->GetFacpNum();
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pData->GetSysData();
		nFid = pUnit->GetFacpID();
		nUid = pUnit->GetUnitID();
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(nFid);
		nUNum = pUnit->GetUnitNum();
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pData->GetSysData();
		nFid = pChn->GetFacpID();
		nUid = pChn->GetUnitID();
		nCid = pChn->GetChnID();
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(nFid);
		nUNum = m_pRefFasSysData->CvtUnitIDToNum(nFid, nUid);
		nCNum = pChn->GetChnNum();
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pData->GetSysData();
		nFid = pDev->GetFacpID();
		nUid = pDev->GetUnitID();
		nCid = pDev->GetChnID();
		nDevid = pDev->GetDeviceID();
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(nFid);
		nUNum = m_pRefFasSysData->CvtUnitIDToNum(nFid, nUid);
		nCNum = m_pRefFasSysData->CvtChannelIDToNum(nFid, nUid, nCid);
		nDevNum = pDev->GetDeviceNum();
		break;
	}
	return TRUE;
}

int CDlgRETabPattern::FillTree(int nDataType)
{
	RemoveAllData();
	HTREEITEM hPOutput;
	if (m_pRefCurData == nullptr || m_pRefCurData->pData == nullptr
		|| m_pRefCurData->nDataType != nDataType)
		return  0;
	if (m_pRefNewData == nullptr || m_pRefNewData->pData == nullptr
		|| m_pRefNewData->nDataType != nDataType)
		return  0;

	hPOutput = m_ctrlTree.InsertItem(CHANGETREE_PATTERN_TEXT);
	m_ctrlTree.SetItemData(hPOutput, (DWORD_PTR)0);
	TreePatternItemAdd(hPOutput);
	return 1;
}

int CDlgRETabPattern::TreePatternItemAdd(HTREEITEM hParent)
{
	if (m_pRefChangeList == nullptr)
		return 0;
	int nIdx = 0;
	POSITION pos;
	ST_CHANGE_LINKDATA * pData;
	CDataPattern * pPtn = nullptr;
	HTREEITEM hPtn[D_MAX_PATTERN_COUNT+1] = { nullptr };
	pos = m_pRefChangeList->GetHeadPosition();
	while (pos)
	{
		pData = (ST_CHANGE_LINKDATA*)m_pRefChangeList->GetNext(pos);
		if (pData == nullptr || pData->pRefInput == nullptr)
			continue;

		if (pData->btInOutType != CHANGE_LK_PATTERN)
			continue;

		if (pPtn != nullptr && pPtn == pData->pRefInput)
			continue;

		pPtn = (CDataPattern*)pData->pRefInput;
		// tree에 facp ,unit,chn있는지 확인
 		if (hPtn[pPtn->GetPatternID()] == nullptr)
 		{
 			// Facp 입력
			hPtn[pPtn->GetPatternID()] 
 				= m_ctrlTree.InsertItem(pPtn->GetPatternName() , TIMG_DEV_PATTERN, TIMG_DEV_PATTERN, hParent);
 			m_ctrlTree.SetItemData(hPtn[pPtn->GetPatternID()], (DWORD_PTR)pPtn);
 		}
	}
	return 1;
}

int CDlgRETabPattern::FillPatternList(CDataPattern * pPtn)
{
	if (pPtn == nullptr)
		return 0;
	m_ctrlAfterList.SetRedraw(FALSE);
	m_ctrlAfterList.DeleteAllItems();
	CPtrList * pList;
	int nIdx = 0, nPtnID=0;
	POSITION pos;
	CDataDevice *pOld, *pChange;
	CDataLinked * plnk;
	ST_CHANGE_LINKDATA * pData;
	pList = pPtn->GetPtnItemList();
	nPtnID = (int)pPtn->GetPatternID();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		plnk = (CDataLinked*)pList->GetNext(pos);
		if (plnk == nullptr)
			continue;
		if (plnk->GetLinkType() != LK_TYPE_RELEAY)
			continue;
		
		pData = FindChangeData(CHANGE_LK_PATTERN, nPtnID, plnk->GetTgtFacp()
			, plnk->GetTgtUnit(), plnk->GetTgtChn()
			, plnk->GetTgtDev()
		);
		if (pData == nullptr || pData->pRefOld == nullptr || pData->pChange == nullptr)
			continue;
		if (pData->btChangeType == CHANGE_ACTION_DEL)
			continue;

		pOld = (CDataDevice*)pData->pRefOld;
		pChange = (CDataDevice*)pData->pChange;
		m_ctrlAfterList.InsertItem(nIdx, pOld->GetOutputFullName());
		m_ctrlAfterList.SetItemText(nIdx, 1, pChange->GetOutputFullName());
		m_ctrlAfterList.SetCheckbox(nIdx, 2, pData->btChangeType == CHANGE_ACTION_UPDATE ? 1:0);
		m_ctrlAfterList.SetItemData(nIdx, (DWORD_PTR)pData);
		nIdx++;
	}
	m_ctrlAfterList.SetRedraw();
	return 1;
}

ST_CHANGE_LINKDATA * CDlgRETabPattern::FindChangeData(int nType, int nPtnID, int nFid, int nUid, int nCid, int nDid)
{
	if (m_pRefChangeList == nullptr)
		return 0;
	int nIdx = 0;
	POSITION pos;
	ST_CHANGE_LINKDATA * pData;
	CDataDevice * pDev = nullptr;
	CDataPattern * pInput = nullptr;
	pos = m_pRefChangeList->GetHeadPosition();

	while (pos)
	{
		pData = (ST_CHANGE_LINKDATA*)m_pRefChangeList->GetNext(pos);
		if (pData == nullptr || pData->pRefInput == nullptr)
			continue;

		if (pData->btInOutType != nType)
			continue;
		pInput = (CDataPattern*)pData->pRefInput;
		if (nPtnID != pInput->GetPatternID())
			continue; 

		pDev = (CDataDevice*)pData->pChange;
		if (pDev == nullptr)
			continue;
		if (pDev->GetFacpID() == nFid && pDev->GetUnitID() == nUid
			&& pDev->GetChnID() == nCid && pDev->GetDeviceID() == nDid)
			return pData;
	}
	return nullptr;
}
