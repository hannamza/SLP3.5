// DlgRETabRelay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgRETabRelay.h"
#include "afxdialogex.h"

#include "DataChannel.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataSystem.h"
#include "DataLinked.h"
#include "MapSystemData.h"
#include "DataDevice.h"
#include "RelayTableData.h"
#include	"../Common/Utils/YAdoDatabase.h"

// CDlgRETabRelay 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRETabRelay, CDialogEx)

#ifndef ENGLISH_MODE
CDlgRETabRelay::CDlgRETabRelay(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_TAB_RELAY, pParent)
{
	m_pRefCurData = nullptr;
	m_pRefNewData = nullptr;
	m_pRefFasSysData = nullptr;
	m_pRefChangeList = nullptr;
	m_pRefMapChangeDevice = nullptr;
	m_nAction = DATA_SAVE;
}
#else
CDlgRETabRelay::CDlgRETabRelay(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_TAB_RELAY_EN, pParent)
{
	m_pRefCurData = nullptr;
	m_pRefNewData = nullptr;
	m_pRefFasSysData = nullptr;
	m_pRefChangeList = nullptr;
	m_pRefMapChangeDevice = nullptr;
	m_nAction = DATA_SAVE;
}
#endif

CDlgRETabRelay::~CDlgRETabRelay()
{
}

void CDlgRETabRelay::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AFTER_LIST, m_ctrlAfterList);
	DDX_Control(pDX, IDC_TREE, m_ctrlTree);
}


BEGIN_MESSAGE_MAP(CDlgRETabRelay, CDialogEx)
//	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CDlgRETabRelay::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRETabRelay::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDlgRETabRelay::OnBnClickedBtnDel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CDlgRETabRelay::OnTvnSelchangedTree)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgRETabRelay::OnBnClickedBtnSave)
	ON_NOTIFY(LVN_XL_CHECK, IDC_AFTER_LIST, OnAfterListCheck)
END_MESSAGE_MAP()


// CDlgRETabRelay 메시지 처리기입니다.


BOOL CDlgRETabRelay::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
// 	CBitmap bmp;
// 	if (!bmp.LoadBitmap(IDB_BMP_DEVICE_ICON))
// 	{
// 		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), IDB_BMP_DEVICE_ICON);
// 		ASSERT(FALSE);
// 		return;
// 	}
// 	BITMAP bmpObj;
// 	bmp.GetBitmap(&bmpObj);
// 	UINT nFlags = ILC_MASK;
// 	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;
// 	m_ImageTree.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
// 	m_ImageTree.Add(&bmp, RGB(0, 255, 255));
// 	//
	m_pRefFasSysData = theApp.GetRelayTableData();
	m_ImageTree.Create(IDB_BMP_DEVICE_ICON, 16, 8, RGB(0, 255, 255));
	m_ctrlTree.SetImageList(&m_ImageTree, TVSIL_NORMAL);
#ifndef ENGLISH_MODE
	m_ctrlAfterList.InsertColumn(0, _T("기존 감지기/중계기"), LVCFMT_LEFT, 250);
	m_ctrlAfterList.InsertColumn(1, _T("새 감지기/중계기"), LVCFMT_LEFT, 250);
	m_ctrlAfterList.InsertColumn(2, _T("변경"),120);
#else
	m_ctrlAfterList.InsertColumn(0, _T("EXISTING DETECTOR/MODULE"), LVCFMT_LEFT, 250);
	m_ctrlAfterList.InsertColumn(1, _T("NEW DETECTOR/MODULE"), LVCFMT_LEFT, 250);
	m_ctrlAfterList.InsertColumn(2, _T("CHANGE"), 120);
#endif
	m_ctrlAfterList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0
		, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgRETabRelay::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnClose();
}


void CDlgRETabRelay::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rcLeft, rcRight;
	rcLeft = CRect(4, 35, cx / 2 - 4 - 2, cy - 4);
	rcRight = CRect(rcLeft.right + 4, 35, cx - 4, cy - 4);
	if (m_ctrlTree.GetSafeHwnd())
		m_ctrlTree.MoveWindow(&rcLeft);
	if (m_ctrlAfterList.GetSafeHwnd())
		m_ctrlAfterList.MoveWindow(&rcRight);
}


void CDlgRETabRelay::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


void CDlgRETabRelay::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}


void CDlgRETabRelay::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_nAction == DATA_DEL)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제 시에는 영향을 받는 항목을 삭제할 수 없습니다.");
#else
		AfxMessageBox(L"Once deleted, you can't delete the affected items.");
#endif
		return;
	}
	int i, nCnt , nChk; 
	ST_CHANGE_LINKDATA * pData;
	nCnt = m_ctrlAfterList.GetItemCount();
	for (i = nCnt -1; i >= 0; i--)
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

void CDlgRETabRelay::RefreshChange(int nDataType, int nAction)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// Tree Reset
	m_nAction = nAction;
	RemoveAllData();
	FillTree(nDataType);
}

void CDlgRETabRelay::RemoveAllData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_ctrlTree.GetSafeHwnd())
		m_ctrlTree.DeleteAllItems();
	if (m_ctrlAfterList.GetSafeHwnd())
		m_ctrlAfterList.DeleteAllItems();
}


BOOL CDlgRETabRelay::GetIDNum(ST_TREEITEM* pTreeItem , int &nFid, int &nUid, int &nCid, int &nDevid
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


int CDlgRETabRelay::FillTree(int nDataType)
{
	RemoveAllData();
	HTREEITEM hPInput, hPOutput;
	if (m_pRefCurData == nullptr || m_pRefCurData->pData == nullptr
		|| m_pRefCurData->nDataType != nDataType)
		return  0;
	if (m_pRefNewData == nullptr || m_pRefNewData->pData == nullptr
		|| m_pRefNewData->nDataType != nDataType)
		return  0;
	hPInput = m_ctrlTree.InsertItem(CHANGETREE_INPUT_TEXT);
	m_ctrlTree.SetItemData(hPInput, (DWORD_PTR)0);
	hPOutput = m_ctrlTree.InsertItem(CHANGETREE_OUTPUT_TEXT);
	m_ctrlTree.SetItemData(hPOutput, (DWORD_PTR)0);
	TreeOutputItemAdd(hPOutput);

	return 1;
}

int CDlgRETabRelay::TreeOutputItemAdd(HTREEITEM hParent)
{
	if (m_pRefChangeList == nullptr)
		return 0;
	int nIdx = 0;
	POSITION pos;
	ST_CHANGE_LINKDATA * pData , *ptc;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pInput = nullptr;
	HTREEITEM hFacp[D_MAX_FACP_COUNT] = { nullptr };
	HTREEITEM hUnit[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT] = { nullptr };
	HTREEITEM hChn[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT][5] = { nullptr };
	HTREEITEM hItem;
	int nlf, nlu, nlc, nld; //< 마지막 ID
	int nFacp, nUnit, nChn, nDev;
	nlf = nlu = nlc = nld = -1;
	nFacp = nUnit = nChn = nDev = -1;
	pos = m_pRefChangeList->GetHeadPosition();
// 	while (pos)
// 	{
// 		pData = (ST_CHANGE_LINKDATA*)m_pRefChangeList->GetNext(pos);
// 		if (pData == nullptr || pData->pChange == nullptr)
// 			continue;
// 
// 		if (pData->btInOutType == CHANGE_LK_FACP)
// 		{
// 			pFacp = (CDataFacp*)pData->pChange;
// 			hFacp[pInput->GetFacpID()]
// 				= m_ctrlTree.InsertItem(pFacp->GetFacpName(), TIMG_DEV_FACP, TIMG_DEV_FACP, hParent);
// 			m_ctrlTree.SetItemData(hFacp[pFacp->GetFacpID()], (DWORD_PTR)0);
// 		}
// 		if (pData->btInOutType == CHANGE_LK_UNIT)
// 		{
// 
// 			pUnit = (CDataUnit *)pData->pChange;
// 			hUnit[pUnit->GetFacpID()][pUnit->GetUnitID()]
// 				= m_ctrlTree.InsertItem(pUnit->GetUnitName()
// 					, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hFacp[pUnit->GetFacpID()]);
// 			m_ctrlTree.SetItemData(hUnit[pUnit->GetFacpID()][pUnit->GetUnitID()], (DWORD_PTR)0);
// 		}
// 		if (pData->btInOutType == CHANGE_LK_CHN)
// 		{
// 			// Facp 입력
// 			pChn = (CDataChannel *)pData->pChange;
// 			hChn[pChn->GetFacpID()][pChn->GetUnitID()][pChn->GetChnID()]
// 				= m_ctrlTree.InsertItem(pChn->GetChnName()
// 					, TIMG_DEV_CHANNEL, TIMG_DEV_CHANNEL
// 					, hUnit[pChn->GetFacpID()][pChn->GetUnitID()]);
// 			m_ctrlTree.SetItemData(hChn[pChn->GetFacpID()][pChn->GetUnitID()][pChn->GetChnID()], (DWORD_PTR)0);
// 		}
// 	}

	pos = m_pRefChangeList->GetHeadPosition();

	while (pos)
	{
		nFacp = nUnit = nChn = nDev = -1;
		pData = (ST_CHANGE_LINKDATA*)m_pRefChangeList->GetNext(pos);
		if (pData == nullptr )
			continue;

		if (!(
			pData->btInOutType == CHANGE_LK_OUTPUT
// 			|| pData->btInOutType == CHANGE_LK_FACP
// 			|| pData->btInOutType == CHANGE_LK_UNIT
// 			|| pData->btInOutType == CHANGE_LK_CHN
			))
			continue;

		if ( pData->pRefInput == nullptr)
			continue;

		pInput = (CDataDevice*)pData->pRefInput;
		// tree에 facp ,unit,chn있는지 확인
		if (hFacp[pInput->GetFacpID()] == nullptr)
		{
			// Facp 입력
			pFacp = m_pRefFasSysData->GetFacpByID(pInput->GetFacpID());
			if (pFacp == nullptr)
			{
				ptc = FindChangeData(CHANGE_LK_FACP, nullptr, pInput->GetFacpID(), 0, 0, 0);
				if (ptc == nullptr)
					continue; 
				pFacp = (CDataFacp*)ptc->pChange;
			}
			hFacp[pInput->GetFacpID()]  
				= m_ctrlTree.InsertItem(pFacp->GetFacpName(), TIMG_DEV_FACP, TIMG_DEV_FACP, hParent);
			m_ctrlTree.SetItemData(hFacp[pInput->GetFacpID()], (DWORD_PTR)0);
		}
		if (hUnit[pInput->GetFacpID()][pInput->GetUnitID()] == nullptr)
		{
			// Facp 입력
			pUnit = m_pRefFasSysData->GetUnitByID(pInput->GetFacpID(), pInput->GetUnitID());
			if (pUnit == nullptr)
			{
				ptc = FindChangeData(CHANGE_LK_UNIT
					, nullptr
					, pInput->GetFacpID(), pInput->GetUnitID()
					, 0, 0);
				if (ptc == nullptr)
					continue;
				pUnit = (CDataUnit *)ptc->pChange;
			}
			hUnit[pUnit->GetFacpID()][pUnit->GetUnitID()]
				= m_ctrlTree.InsertItem(pUnit->GetUnitName()
					, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hFacp[pUnit->GetFacpID()]);
			m_ctrlTree.SetItemData(hUnit[pUnit->GetFacpID()][pUnit->GetUnitID()], (DWORD_PTR)0);
		}
		if (hChn[pInput->GetFacpID()][pInput->GetUnitID()][pInput->GetChnID()] == nullptr)
		{
			// Facp 입력
			pChn = m_pRefFasSysData->GetChannelByID(pInput->GetFacpID()
				, pInput->GetUnitID(), pInput->GetChnID());
			if (pChn == nullptr)
			{
				ptc = FindChangeData(CHANGE_LK_CHN
					,  nullptr
					, pInput->GetFacpID(), pInput->GetUnitID()
					, pInput->GetChnID(), 0);
				if (ptc == nullptr)
					continue;
				pChn = (CDataChannel *)ptc->pChange;
			}

			hChn[pChn->GetFacpID()][pChn->GetUnitID()][pChn->GetChnID()]
				= m_ctrlTree.InsertItem(pChn->GetChnName()
					, TIMG_DEV_CHANNEL, TIMG_DEV_CHANNEL
					, hUnit[pChn->GetFacpID()][pChn->GetUnitID()]);
			m_ctrlTree.SetItemData(hChn[pChn->GetFacpID()][pChn->GetUnitID()][pChn->GetChnID()], (DWORD_PTR)0);
		}
		nFacp = pInput->GetFacpID();
		nUnit = pInput->GetUnitID();
		nChn = pInput->GetChnID();
		nDev = pInput->GetDeviceID();
		if (nlf != nFacp || nlu != nUnit || nlc != nChn || nld != nDev)
		{
			hItem = m_ctrlTree.InsertItem(pInput->GetOutContentsFullName()
				, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, hChn[pInput->GetFacpID()][pInput->GetUnitID()][pInput->GetChnID()]);
			m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pInput);
			nlf = nFacp;
			nlu = nUnit;
			nlc = nChn;
			nld = nDev;
		}
	}
	return 1;
}


void CDlgRETabRelay::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	if (pNMTreeView->itemNew.hItem == nullptr)
		return; 

	if (m_ctrlTree.GetItemData(pNMTreeView->itemNew.hItem) == 0)
	{
		if(m_ctrlTree.GetItemText(pNMTreeView->itemNew.hItem) == CHANGETREE_INPUT_TEXT)
			FillInputList();
		else
			m_ctrlAfterList.DeleteAllItems();
		return;
	}

	CDataDevice * pDev = (CDataDevice*)m_ctrlTree.GetItemData(pNMTreeView->itemNew.hItem);
	if (pDev)
		FillOutputList(pDev);
	else
		m_ctrlAfterList.DeleteAllItems();
}

int CDlgRETabRelay::FillOutputList(CDataDevice * pDev)
{
	if (pDev == nullptr)
		return 0; 
	m_ctrlAfterList.SetRedraw(FALSE);
	m_ctrlAfterList.DeleteAllItems();
	CPtrList * pList;
	int nIdx = 0;
	POSITION pos;
	CDataLinked * pLink , *pOLnk , *pNLnk;
	CDataDevice *pOld, *pChange;
	ST_CHANGE_LINKDATA * pData = nullptr;
	pList = pDev->GetLinkedList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pLink = (CDataLinked*)pList->GetNext(pos);
		if (pLink == nullptr)
			continue;
	
		pData = FindChangeData(CHANGE_LK_OUTPUT, pDev , pLink->GetTgtFacp()
							,pLink->GetTgtUnit() , pLink->GetTgtChn()
							,pLink->GetTgtDev()
		);
		if (pData == nullptr || pData->pRefOld == nullptr || pData->pChange == nullptr)
			continue; 
		if (pData->btInOutType != CHANGE_LK_OUTPUT)
			continue;

		if (pData->btChangeType == CHANGE_ACTION_DEL)
			continue;
		
		//asdf
		pOLnk = (CDataLinked*)pData->pRefOld;
		pNLnk = (CDataLinked*)pData->pChange;
		pOld = m_pRefFasSysData->GetDeviceByID(pOLnk->GetTgtFacp()
			, pOLnk->GetTgtUnit(), pOLnk->GetTgtChn()
			, pOLnk->GetTgtDev());
		if (pOld == nullptr)
			continue; 
		pChange = (*m_pRefMapChangeDevice)[pOld->GetDevKey()];
		if (pChange == nullptr)
			continue; 
		m_ctrlAfterList.InsertItem(nIdx, pOld->GetOutputFullName());
		m_ctrlAfterList.SetItemText(nIdx, 1,pChange->GetOutputFullName());
		m_ctrlAfterList.SetCheckbox(nIdx, 2, pData->btChangeType == CHANGE_ACTION_UPDATE ? 1 : 0);
		m_ctrlAfterList.SetItemData(nIdx, (DWORD_PTR)pData);
		nIdx++;
	}
	m_ctrlAfterList.SetRedraw();

	return 1;
}

int CDlgRETabRelay::FillInputList()
{
	m_ctrlAfterList.DeleteAllItems();
	if (m_pRefChangeList == nullptr)
		return 0;
	int nIdx = 0;
	POSITION pos;
	ST_CHANGE_LINKDATA * pData;
	CDataDevice * pInput = nullptr;
	pos = m_pRefChangeList->GetHeadPosition();

	while (pos)
	{
		pData = (ST_CHANGE_LINKDATA*)m_pRefChangeList->GetNext(pos);
		if (pData == nullptr)
			continue;

		if (pData->btInOutType != CHANGE_LK_INPUT)
			continue;

// 		if (pData->btChangeType == CHANGE_ACTION_DEL)
// 			continue; 

		pInput = (CDataDevice*)pData->pRefOld;
		m_ctrlAfterList.InsertItem(nIdx, pInput->GetInputFullName());
		m_ctrlAfterList.SetItemText(nIdx, 1, ((CDataDevice*)pData->pChange)->GetInputFullName());
		m_ctrlAfterList.SetCheckbox(nIdx, 2, pData->btChangeType == CHANGE_ACTION_UPDATE ? 1 : 0);
		m_ctrlAfterList.SetItemData(nIdx, (DWORD_PTR)pData);
		nIdx++;
	}
	return 1;
}

ST_CHANGE_LINKDATA * CDlgRETabRelay::FindChangeData(int nType, LPVOID pSrc, int nFid, int nUid, int nCid, int nDid)
{
	if (m_pRefChangeList == nullptr)
		return 0;
	int nIdx = 0;
	POSITION pos;
	ST_CHANGE_LINKDATA * pData;
	CDataDevice * pInput = nullptr , *pTDev = nullptr;
	CDataLinked * pNLnk = nullptr;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	pos = m_pRefChangeList->GetHeadPosition();

	while (pos)
	{
		pData = (ST_CHANGE_LINKDATA*)m_pRefChangeList->GetNext(pos);
		if (pData == nullptr || pData->pChange == nullptr)
			continue;

		if (pData->btInOutType != nType)
			continue;
		switch (pData->btInOutType)
		{
		case CHANGE_LK_OUTPUT:
			if (pData->pRefInput == nullptr)
				continue;
			pTDev = (CDataDevice*)pData->pRefInput;
			if (pTDev->IsEqual((CDataDevice*)pSrc) == FALSE)
				continue;
			pNLnk = (CDataLinked*)pData->pChange;
			if (pNLnk->GetTgtFacp() == nFid && pNLnk->GetTgtUnit() == nUid
				&& pNLnk->GetTgtChn() == nCid && pNLnk->GetTgtDev() == nDid)
				return pData;
			else
				break;
		case CHANGE_LK_FACP:
			pFacp = (CDataFacp*)pData->pChange;
			if(pFacp->GetFacpID() == nFid)
				return pData;
			else
				break;
		case CHANGE_LK_UNIT:
			pUnit = (CDataUnit*)pData->pChange;
			if (pUnit->GetFacpID() == nFid && pUnit->GetUnitID() == nUid)
				return pData;
			else
				break;
		case CHANGE_LK_CHN:
			pChn = (CDataChannel*)pData->pChange;
			if (pChn->GetFacpID() == nFid && pChn->GetUnitID() == nUid
				&& pChn->GetChnID() == nCid )
				return pData;
			else
				break;
		}
	}
	return nullptr;
}

void CDlgRETabRelay::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgRETabRelay::OnAfterListCheck(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (pNMHDR == nullptr)
		return;

	XNOTIFY_CHECK *pxnData = reinterpret_cast<XNOTIFY_CHECK*>(pNMHDR);
	if (pxnData == nullptr)
		return; 
	int nChecked = 0;
	ST_CHANGE_LINKDATA * pData = nullptr;
	int nIdx = pxnData->dispinfo.item.iItem;
	if (nIdx < 0)
		return;

	nChecked = pxnData->nCheckState;
	pData = (ST_CHANGE_LINKDATA*)m_ctrlAfterList.GetItemData(nIdx);
	if (pData == nullptr)
		return;
	if (nChecked)
		pData->btChangeType = CHANGE_ACTION_UPDATE;
	else
		pData->btChangeType = CHANGE_ACTION_KEEP;
}
