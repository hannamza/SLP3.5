// DkPaneInputRelay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DkPaneInputRelay.h"
#include "RelayTableData.h"
#include "DlgSetUserTreeView.h"

#include "DataLinked.h"
#include "DataChannel.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataSystem.h"
#include "DataDevice.h"
class CMBtnInputRelay : public CMFCToolBarMenuButton
{
	friend class CDkPaneInputRelay;

	DECLARE_SERIAL(CMBtnInputRelay)

public:
	CMBtnInputRelay(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CMBtnInputRelay, CMFCToolBarMenuButton, 1)
// CDkPaneInputRelay

IMPLEMENT_DYNAMIC(CDkPaneInputRelay, CDockablePane)

CDkPaneInputRelay::CDkPaneInputRelay()
{
	m_pRefFasSysData = nullptr;
	m_bPreventSelect = FALSE;
	m_nLastTreeType = DKPTREE_ADDRESS_VIEW;
	m_bMultiSelectInput = FALSE;
}

CDkPaneInputRelay::~CDkPaneInputRelay()
{
	RemoveAllTreeData();
}


BEGIN_MESSAGE_MAP(CDkPaneInputRelay, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SORT_ADDRESS, &CDkPaneInputRelay::OnSortAddress)
	ON_COMMAND(ID_SORT_CUSTOM, &CDkPaneInputRelay::OnSortCustom)
	ON_COMMAND(ID_SORT_EQUIP, &CDkPaneInputRelay::OnSortEquip)
	ON_COMMAND(ID_SORT_INPUTTYPE, &CDkPaneInputRelay::OnSortInputtype)
	ON_COMMAND(ID_SORT_LOCATION, &CDkPaneInputRelay::OnSortLocation)
	ON_COMMAND(ID_SORT_OUTTYPE, &CDkPaneInputRelay::OnSortOuttype)
	ON_NOTIFY(TVN_SELCHANGED, IDC_INPUTVIEW_TREE, &CDkPaneInputRelay::OnTvnSelchangedLocTree)
	ON_TVN_CHECK_ITEM(IDC_INPUTVIEW_TREE, &CDkPaneInputRelay::OnTvnCheckChangeLocTree)
	ON_TVN_CHECK_PROC_END(IDC_INPUTVIEW_TREE, &CDkPaneInputRelay::OnTvnCheckProcEndLocTree)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CDkPaneInputRelay 메시지 처리기입니다.




int CDkPaneInputRelay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES 
		| TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS 
		| WS_CLIPCHILDREN | TVS_SHOWSELALWAYS | TVS_CHECKBOXES;

	if (!m_ctrlRelayTree.Create(dwViewStyle, rectDummy, this, IDC_INPUTVIEW_TREE))
	{
		TRACE0("클래스 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	// 이미지를 로드합니다.
	m_TbInputRelay.Create(this, AFX_DEFAULT_TOOLBAR_STYLE| CBRS_TOOLTIPS, IDR_TB_DKPRELAY_SORT);
	m_TbInputRelay.LoadToolBar(IDR_TB_DKPRELAY_SORT, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

	m_TbInputRelay.SetPaneStyle(m_TbInputRelay.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY| CBRS_TOOLTIPS);
	m_TbInputRelay.SetPaneStyle(m_TbInputRelay.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_TbInputRelay.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_TbInputRelay.SetRouteCommandsViaFrame(FALSE);
	m_ctrlRelayTree.SetClickCheckSelected(FALSE);
// 
// 	CMenu menuSort;
// 	menuSort.LoadMenu(IDR_TB_DKPRELAY_SORT);
// 
// 	m_TbInputRelay.ReplaceButton(ID_SORT_MENU, CMBtnInputRelay(menuSort.GetSubMenu(0)->GetSafeHmenu()));
// 
// 	CMBtnInputRelay* pButton = DYNAMIC_DOWNCAST(CMBtnInputRelay, m_TbInputRelay.GetButton(0));
// 
// 	if (pButton != NULL)
// 	{
// 		pButton->m_bText = FALSE;
// 		pButton->m_bImage = TRUE;
// 		//pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
// 		pButton->SetMessageWnd(this);
// 	}


	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	//FillClassView();
	return 0;
}

void CDkPaneInputRelay::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_TbInputRelay.CalcFixedLayout(FALSE, TRUE).cy;

	m_TbInputRelay.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_ctrlRelayTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDkPaneInputRelay::OnChangeVisualStyle()
{
	m_ImgList.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_BMP_DEVICE_ICON : IDB_BMP_DEVICE_ICON;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ImgList.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ImgList.Add(&bmp, RGB(0, 255, 255));

	m_ctrlRelayTree.SetImageList(&m_ImgList, TVSIL_NORMAL);

	m_TbInputRelay.CleanUpLockedImages();
	m_TbInputRelay.LoadBitmap(theApp.m_bHiColorIcons ? IDB_BMP_TB_DKP_RELAY : IDB_BMP_TB_DKP_RELAY, 0, 0, TRUE /* 잠금 */);
}


void CDkPaneInputRelay::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AdjustLayout();
}


void CDkPaneInputRelay::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDkPaneInputRelay::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CRect rectTree;
	m_ctrlRelayTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}


void CDkPaneInputRelay::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ctrlRelayTree.SetFocus();

}


void CDkPaneInputRelay::OnSortAddress()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_bPreventSelect = TRUE;
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlRelayTree, DKPTREE_ADDRESS_VIEW, &m_ptrItemList, TRUE);
	m_nLastTreeType = DKPTREE_ADDRESS_VIEW;
	m_bPreventSelect = FALSE;
}


void CDkPaneInputRelay::OnSortCustom()
{
	CDlgSetUserTreeView dlg(FALSE);
	if (dlg.DoModal() != IDOK)
		return;
	if (AfxGetApp())
		((CSysLinkerApp*)AfxGetApp())->SaveCustomTreeViewItem(FALSE);
	if (g_tvInput.size() <= 0)
		return;
	m_bPreventSelect = TRUE;
	RemoveAllTreeData();

	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTreeByCustom(&m_ctrlRelayTree, &m_ptrItemList, &g_tvInput, FALSE);
	m_nLastTreeType = DKPTREE_CUSTOM_VIEW;
	m_bPreventSelect = FALSE;
}


void CDkPaneInputRelay::OnSortEquip()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_bPreventSelect = TRUE;
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlRelayTree, DKPTREE_EQNAME_VIEW, &m_ptrItemList, TRUE);
	m_bPreventSelect = FALSE;
	m_nLastTreeType = DKPTREE_EQNAME_VIEW;
}


void CDkPaneInputRelay::OnSortInputtype()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_bPreventSelect = TRUE;
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlRelayTree, DKPTREE_INPUT_TYPE_VIEW, &m_ptrItemList, TRUE);
	m_bPreventSelect = FALSE;
	m_nLastTreeType = DKPTREE_INPUT_TYPE_VIEW;
}


void CDkPaneInputRelay::OnSortLocation()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_bPreventSelect = TRUE;
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlRelayTree, DKPTREE_LOCATION_VIEW , &m_ptrItemList, TRUE);
	m_bPreventSelect = FALSE;
	m_nLastTreeType = DKPTREE_LOCATION_VIEW;
}


void CDkPaneInputRelay::OnSortOuttype()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_bPreventSelect = TRUE;
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlRelayTree, DKPTREE_OUTPUT_TYPE_VIEW, &m_ptrItemList, TRUE);
	m_bPreventSelect = FALSE;
	m_nLastTreeType = DKPTREE_OUTPUT_TYPE_VIEW;
}


void CDkPaneInputRelay::SetRelayTable(CRelayTableData * pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
}


int CDkPaneInputRelay::InitTree()
{
	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"감지기/중계기의 정보가 설정되지 않았습니다.");
		return 0; 
	}
//	m_bPreventSelect = TRUE;
	switch (m_nLastTreeType)
	{
	case DKPTREE_INPUT_TYPE_VIEW:
		OnSortInputtype();
		break;
	case DKPTREE_OUTPUT_TYPE_VIEW:
		OnSortOuttype();
		break;
	case DKPTREE_LOCATION_VIEW:
		OnSortLocation();
		break;
	case DKPTREE_EQNAME_VIEW:
		OnSortEquip();
		break;
	case DKPTREE_CUSTOM_VIEW:
		OnSortCustom();
		break;
	case DKPTREE_ADDRESS_VIEW:
		OnSortAddress();
		break;
	case DKPTREE_CONTENTS_EQNAME_VIEW:
		//OnSortInputtype();
		break;
	}
// 	RemoveAllTreeData();
// 	m_pRefFasSysData->FillDeviceTree(&m_ctrlRelayTree, , &m_ptrItemList);
//	m_bPreventSelect = FALSE;
	return 1;
}

int CDkPaneInputRelay::RemoveAllTreeData()
{
	m_bPreventSelect = TRUE;
	ST_TREEITEM * pItem;
	if(m_ctrlRelayTree.GetSafeHwnd())
		m_ctrlRelayTree.DeleteAllItems();
	while (!m_ptrItemList.IsEmpty())
	{
		pItem = (ST_TREEITEM *)m_ptrItemList.RemoveHead();
		if (pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
	m_bPreventSelect = FALSE;
	return 0;
}

void CDkPaneInputRelay::OnTvnSelchangedLocTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
 	if (m_bPreventSelect)
 		return;

 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
 	HTREEITEM hSel = m_ctrlRelayTree.GetSelectedItem();
 	if (hSel == nullptr)
 		return;
 	ST_TREEITEM * pItem = (ST_TREEITEM*)m_ctrlRelayTree.GetItemData(hSel);
 
 	VT_HITEM VtChecked;
	ST_HITEM * pData;
 	int nLevel = 0 , nCnt,i;
	m_ctrlRelayTree.GetCheckedList(TVI_ROOT,&VtChecked,nLevel);
 	nCnt = VtChecked.size();
	if(nCnt <= 0)
 	{
 		if (pItem)
 			AfxGetMainWnd()->SendMessage(UWM_DKP_INPUTVIEW_ITEMCHANGE, 0, (LPARAM)pItem);
 	}

	for(i = 0; i < nCnt; i++)
	{
		pData = VtChecked[i];
		if(pData != nullptr)
		{
			delete pData;
			pData = nullptr;
		}
	}
	VtChecked.clear();
	*pResult = 0;
}

void CDkPaneInputRelay::OnTvnCheckChangeLocTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CDkPaneInputRelay::OnTvnCheckProcEndLocTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	NMTVNCHECK * pNMCheck = (NMTVNCHECK *)pNMTreeView;

	VT_HITEM VtChecked;
	int nLevel = 0;
	m_ctrlRelayTree.GetCheckedList(TVI_ROOT, &VtChecked, nLevel);

	int nCnt, i, nRCnt = 0;
	ST_HITEM * pItem = nullptr;
	ST_TREEITEM * pTreeItem = nullptr;;
	RemoveCheckItems();
	nCnt = VtChecked.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = VtChecked[i];
		if (pItem == nullptr || pItem->dwItemData == 0)
			continue;

		pTreeItem = (ST_TREEITEM*)m_ctrlRelayTree.GetItemData(pItem->hItem);
		if(pTreeItem == nullptr || pTreeItem->pData == nullptr)
			continue; 
		
		if (pTreeItem->nDataType != TTYPE_DEV_DEVICE)
			continue; 

		m_ptrSelectItems.AddTail(pTreeItem);

// 		pNewItem = new ST_HITEM;
// 		pNewItem->dwItemData = pItem->dwItemData;
// 		pNewItem->hItem = pItem->hItem;
// 		pNewItem->nTreeLevel = pItem->nTreeLevel;
// 		pNewItem->tsCheck = pItem->tsCheck;
// 		wcscpy_s(pNewItem->szCaption, sizeof(pNewItem->szCaption) / sizeof(TCHAR), pItem->szCaption);
// 		m_vtCheckItems.push_back(pNewItem);
		nRCnt++;
	}

	if (nRCnt > 1)
	{
		// 입력에서 2개 이상선택했을 때
		m_bMultiSelectInput = TRUE;
		AfxGetMainWnd()->SendMessage(UWM_DKP_INPUTVIEW_ITEMCHANGE, (WPARAM)&m_ptrSelectItems, (LPARAM)pItem);
	}
	else
	{
		m_bMultiSelectInput = FALSE;
		m_ctrlRelayTree.SelectItem(pNMCheck->TriggerItem);
	}

	for(i = 0; i < nCnt; i++)
	{
		pItem = VtChecked[i];
		if(pItem != nullptr)
		{
			delete pItem;
			pItem = nullptr;
		}
	}
	VtChecked.clear();
	*pResult = 0;
}


int CDkPaneInputRelay::SelectItem(int nType, LPVOID pItem)
{
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	CDataLinked * pLnk;
	int nf, nu, nc, nr;
	nf = nu = nc = nr = -1;
	if (pItem == nullptr)
		return 0; 

	switch (nType)
	{
	case SE_FACP:
		pFacp = (CDataFacp*)pItem;
		nf = pFacp->GetFacpID();
		break;
	case SE_UNIT:
		pUnit = (CDataUnit*)pItem;
		nf = pUnit->GetFacpID();
		nu = pUnit->GetUnitID();
		break;
	case SE_CHANNEL:
		pChn = (CDataChannel*)pItem;
		nf = pChn->GetFacpID();
		nu = pChn->GetUnitID();
		nc = pChn->GetChnID();
		break;
	case SE_RELAY:
		pDev = (CDataDevice*)pItem;
		nf = pDev->GetFacpID();
		nu = pDev->GetUnitID();
		nc = pDev->GetChnID();
		nr = pDev->GetDeviceID();
		break;
	case TTYPE_DEV_LINKED_TARGETITEM:
		pLnk = (CDataLinked *)pItem;
		nf = pLnk->GetTgtFacp();
		nu = pLnk->GetTgtUnit();
		nc = pLnk->GetTgtChn();
		nr = pLnk->GetTgtDev();
		nType = SE_RELAY;
		break;
	}

	HTREEITEM hItem;
	hItem = FindTreeItem(nType, nf, nu, nc, nr, TVI_ROOT);
	if (hItem == nullptr)
		return 0;
	m_ctrlRelayTree.SelectItem(hItem);
// 	m_ctrlRelayTree.SetItemState(hItem, TVIF_STATE | TVIS_SELECTED, TVIF_STATE | TVIS_SELECTED);
// 	m_ctrlRelayTree.SetFocus();
	return 1;
}

HTREEITEM CDkPaneInputRelay::FindTreeItem(int nFindType, int nFid, int nUid, int nCid, int nRid, HTREEITEM hItem)
{
	ST_TREEITEM * pTData = nullptr;
	DWORD_PTR dwTemp;
	CDataSystem * pItem;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	HTREEITEM hitemFind, hItemChile, hItemSibling;
	hitemFind = hItemChile = hItemSibling = NULL;
	if (hItem == nullptr)
		return nullptr;
	if (hItem != TVI_ROOT)
		dwTemp = m_ctrlRelayTree.GetItemData(hItem);
	else
		dwTemp = 0;
	pTData = (ST_TREEITEM *)dwTemp;

	if (pTData != 0 && pTData->pData != nullptr && pTData->nDataType == nFindType)
	{
		pItem = (CDataSystem*)pTData->pData;
		if (pItem != nullptr && pItem->GetSysData() != nullptr)
		{
			switch (pItem->GetDataType())
			{
			case SE_FACP:
				pFacp = (CDataFacp*)pItem->GetSysData();
				if (pFacp->GetFacpID() == nFid)
					return hItem;
				else
					break;
			case SE_UNIT:
				pUnit = (CDataUnit*)pItem->GetSysData();
				if (pUnit->GetFacpID() == nFid && pUnit->GetUnitID() == nUid)
					return hItem;
				else
					break;
			case SE_CHANNEL:
				pChn = (CDataChannel*)pItem->GetSysData();
				if (pChn->GetFacpID() == nFid && pChn->GetUnitID() == nUid
					&& pChn->GetChnID() == nCid)
					return hItem;
				else
					break;
			case SE_RELAY:
				pDev = (CDataDevice*)pItem->GetSysData();
				if (pDev->GetFacpID() == nFid && pDev->GetUnitID() == nUid
					&& pDev->GetChnID() == nCid && pDev->GetDeviceID() == nRid)
					return hItem;
				else
					break;
			}
		}

		// 		if (pItem->IsEqual(pSysSelect))
		// 			return hItem;
	}

	// 자식 노드를 찾는다.
	hItemChile = m_ctrlRelayTree.GetChildItem(hItem);
	while (hItemChile)
	{
		// check the children of the current item
		HTREEITEM hFound = FindTreeItem(nFindType, nFid, nUid, nCid, nRid, hItemChile);
		if (hFound)
			return hFound;

		// get the next sibling of the current item
		hItemChile = m_ctrlRelayTree.GetNextSiblingItem(hItemChile);
	}
	return nullptr;
}

HTREEITEM CDkPaneInputRelay::FindTreeItem(int nFindType, ST_TREEITEM * pFind, HTREEITEM hItem)
{
	ST_TREEITEM * pTData = nullptr;
	DWORD_PTR dwTemp;
	CDataSystem * pItem , *pFSys;
	CDataFacp * pFacp , * pFF;
	CDataUnit * pUnit , *pFU;
	CDataChannel * pChn , *pFC;
	CDataDevice * pDev , *pFD;
	HTREEITEM hitemFind, hItemChile, hItemSibling;
	hitemFind = hItemChile = hItemSibling = NULL;
	if (hItem == nullptr )
		return nullptr;

	if (pFind == nullptr || pFind->pData == nullptr)
		return nullptr;

	if (hItem != TVI_ROOT)
		dwTemp = m_ctrlRelayTree.GetItemData(hItem);
	else
		dwTemp = 0;
	pTData = (ST_TREEITEM *)dwTemp;
	
	pFSys = (CDataSystem *)pFind->pData;

	if (pTData != 0 && pTData->pData != nullptr && pTData->nDataType == nFindType)
	{
		pItem = (CDataSystem*)pTData->pData;
		if (pItem != nullptr && pItem->GetSysData() != nullptr)
		{
			if (pItem->GetDataType() != pFSys->GetDataType())
				return nullptr;

			switch (pItem->GetDataType())
			{
			case SE_FACP:
				pFacp = (CDataFacp*)pItem->GetSysData();
				pFF = (CDataFacp*)pFSys->GetSysData();
				if (pFacp->GetFacpID() == pFF->GetFacpID())
					return hItem;
				else
					break;
			case SE_UNIT:
				pUnit = (CDataUnit*)pItem->GetSysData();
				pFU = (CDataUnit*)pFSys->GetSysData();
				if (pUnit->GetFacpID() == pFU->GetFacpID()
					&& pUnit->GetUnitID() == pFU->GetUnitID())
					return hItem;
				else
					break;
			case SE_CHANNEL:
				pChn = (CDataChannel*)pItem->GetSysData();
				pFC = (CDataChannel*)pFSys->GetSysData();
				if (pChn->GetFacpID() == pFC->GetFacpID() 
					&& pChn->GetUnitID() == pFC->GetUnitID()
					&& pChn->GetChnID() == pFC->GetChnID())
					return hItem;
				else
					break;
			case SE_RELAY:
				pDev = (CDataDevice*)pItem->GetSysData();
				pFD = (CDataDevice*)pItem->GetSysData();
				if (pDev->GetFacpID() == pFD->GetFacpID()
					&& pDev->GetUnitID() == pFD->GetFacpID()
					&& pDev->GetChnID() == pFD->GetFacpID()
					&& pDev->GetDeviceID() == pFD->GetFacpID())
					return hItem;
				else
					break;
			}
		}

		// 		if (pItem->IsEqual(pSysSelect))
		// 			return hItem;
	}

	// 자식 노드를 찾는다.
	hItemChile = m_ctrlRelayTree.GetChildItem(hItem);
	while (hItemChile)
	{
		// check the children of the current item
		HTREEITEM hFound = FindTreeItem(nFindType, pFind, hItemChile);
		if (hFound)
			return hFound;

		// get the next sibling of the current item
		hItemChile = m_ctrlRelayTree.GetNextSiblingItem(hItemChile);
	}
	return nullptr;
}


void CDkPaneInputRelay::RemoveCheckItems()
{
	ST_TREEITEM * pItem; 
	while (m_ptrSelectItems.IsEmpty() == FALSE)
	{
		pItem = (ST_TREEITEM *)m_ptrSelectItems.RemoveHead();
		if (pItem == nullptr)
			continue; 

	}
	
}

int CDkPaneInputRelay::AddTreeItem(int nType, CDataSystem * pItem)
{
	ST_TREEITEM * pTData = nullptr;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	CMapSystemData * pMap;
	CDataSystem * pSys;
	int i;
	CString strKey , strAdd;
	int nf, nu, nc, nr;
	int nNumb[4] = { -1 };
	HTREEITEM hItem[4] = { nullptr };
	nf = nu = nc = nr = -1;
	if (pItem == nullptr)
		return -1;

	// 주소 , Custom View일때 FACP , UNIT , CHannel 추가
	if (pItem->GetDataType() < SE_RELAY)
	{
		if (m_nLastTreeType != DKPTREE_ADDRESS_VIEW)
			return 0;

		// [KHS 2020-9-4 13:26:22] 
		// 사용자 정의 트리는 지원안하는것으로 한다.
		if (m_nLastTreeType != DKPTREE_CUSTOM_VIEW)
			return 0;
	}

	strKey = pItem->GetKey();
	std::vector<CString> vtArray = GF_SplitString(strKey, L".");

	if (vtArray.size() < 5)
		return -1;

	for (i = 0; i < 4; i++)
	{
		try
		{
			nNumb[i] = _wtoi(vtArray[i]);
		}
		catch (...)
		{
			return -1;
		}
	}

	nf = m_pRefFasSysData->CvtFacpNumToID(nNumb[0]);
	nu = m_pRefFasSysData->CvtUnitNumToID(nNumb[0], nNumb[1]);
	nc = m_pRefFasSysData->CvtChannelNumToID(nNumb[0], nNumb[1], nNumb[2]);

	pMap = m_pRefFasSysData->GetSystemData();
	if (pItem->GetDataType() >= SE_FACP)
	{
		// 수신기 추가
		if (nf < 1)
			return -1;

		hItem[0] = FindTreeItem(SE_FACP, nf, -1, -1, -1, TVI_ROOT);
		if (hItem[0] == nullptr)
		{
			pTData = new ST_TREEITEM;
			memset((void*)pTData, 0, sizeof(ST_TREEITEM));
			strAdd = GF_GetSysDataKey(SE_FACP, nNumb[0]);
			pTData->nDataType = TTYPE_DEV_FACP;
			pSys = (*pMap)[strAdd];
			pFacp = (CDataFacp *)pSys->GetSysData();
			hItem[0] = m_ctrlRelayTree.InsertItem(pFacp->GetFacpName(), TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
			pTData->hParent = TVI_ROOT;
			pTData->hItem = hItem[0];
			pTData->pData = pSys;
			m_ctrlRelayTree.SetItemData(hItem[0], (DWORD_PTR)pTData);
			m_ptrItemList.AddTail(pTData);
		}
	}

	if (hItem[0] != nullptr && pItem->GetDataType() >= SE_UNIT)
	{
		if (nu < 1)
			return -1;

		hItem[1] = FindTreeItem(SE_UNIT, nf, nu, -1, -1, hItem[0]);
		if (hItem[1] == nullptr)
		{
			pTData = new ST_TREEITEM;
			memset((void*)pTData, 0, sizeof(ST_TREEITEM));
			pTData->nDataType = TTYPE_DEV_UNIT;
			strAdd = GF_GetSysDataKey(SE_UNIT, nNumb[0], nNumb[1]);
			pSys = (*pMap)[strAdd];
			pUnit = (CDataUnit *)pSys->GetSysData();
			hItem[1] = m_ctrlRelayTree.InsertItem(pUnit->GetUnitName(), TIMG_DEV_UNIT, TIMG_DEV_UNIT, hItem[0]);
			pTData->hParent = hItem[0];
			pTData->hItem = hItem[1];
			pTData->pData = pSys;
			m_ctrlRelayTree.SetItemData(hItem[1], (DWORD_PTR)pTData);
			m_ptrItemList.AddTail(pTData);
		}
	}

	if (hItem[1] != nullptr &&  pItem->GetDataType() >= SE_CHANNEL)
	{
		if (nc < 1)
			return -1;

		hItem[2] = FindTreeItem(SE_CHANNEL, nf, nu, nc, -1, hItem[1]);
		if (hItem[2] == nullptr)
		{
			pTData = new ST_TREEITEM;
			memset((void*)pTData, 0, sizeof(ST_TREEITEM));
			pTData->nDataType = TTYPE_DEV_CHANNEL;
			strAdd = GF_GetSysDataKey(SE_CHANNEL, nNumb[0], nNumb[1], nNumb[2]);
			pSys = (*pMap)[strAdd];
			pChn = (CDataChannel *)pSys->GetSysData();
			hItem[2] = m_ctrlRelayTree.InsertItem(pChn->GetChnName(), TIMG_DEV_CHANNEL, TIMG_DEV_CHANNEL, hItem[1]);
			pTData->hParent = hItem[1];
			pTData->hItem = hItem[2];
			pTData->pData = pSys;
			m_ctrlRelayTree.SetItemData(hItem[2], (DWORD_PTR)pTData);
			m_ptrItemList.AddTail(pTData);
		}
	}

	if (hItem[2] != nullptr &&  pItem->GetDataType() >= SE_RELAY)
	{
		pTData = new ST_TREEITEM;
		memset((void*)pTData, 0, sizeof(ST_TREEITEM));
		pTData->nDataType = TTYPE_DEV_DEVICE;
		strAdd = GF_GetSysDataKey(SE_RELAY, nNumb[0], nNumb[1], nNumb[2],nNumb[3]);
		pSys = (*pMap)[strAdd];
		pDev = (CDataDevice *)pSys->GetSysData();
// 		AfxMessageBox(m_ctrlRelayTree.GetItemText(hItem[0]));
// 		AfxMessageBox(m_ctrlRelayTree.GetItemText(hItem[1]));
// 		AfxMessageBox(m_ctrlRelayTree.GetItemText(hItem[2]));
		hItem[3] = m_ctrlRelayTree.InsertItem(pDev->GetInputFullName(), TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, hItem[2]);
		pTData->hParent = hItem[2];
		pTData->hItem = hItem[3];
		pTData->pData = pSys;
		m_ptrItemList.AddTail(pTData);
		m_ctrlRelayTree.SetItemData(hItem[3], (DWORD_PTR)pTData);
		m_ctrlRelayTree.SelectItem(hItem[3]);
		return 1;
	}
	
	return 0;
}

int CDkPaneInputRelay::ChangeTreeItem(int nType, ST_TREEITEM * pItem)
{
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	CDataSystem * pSys;
	
	if (pItem == nullptr || pItem->pData == nullptr)
		return 0;
	pSys = (CDataSystem*)pItem->pData;
	HTREEITEM hFind = nullptr;
	hFind = pItem->hItem;
	if (hFind == nullptr)
		hFind = FindTreeItem(pSys->GetDataType(), pItem, TVI_ROOT);
	if (hFind == nullptr)
		return 0; 
	switch (pSys->GetDataType())
	{
	case SE_FACP:
		pFacp = (CDataFacp*)pSys->GetSysData();
		m_ctrlRelayTree.SetItemText(hFind, pFacp->GetFacpName());
		break;
	case SE_UNIT:
		pUnit = (CDataUnit*)pSys->GetSysData();
		m_ctrlRelayTree.SetItemText(hFind, pUnit->GetUnitName());
		break;
	case SE_CHANNEL:
		pChn = (CDataChannel*)pSys->GetSysData();
		m_ctrlRelayTree.SetItemText(hFind, pChn->GetChnName());

		break;
	case SE_RELAY:
		pDev = (CDataDevice*)pSys->GetSysData();
		m_ctrlRelayTree.SetItemText(hFind, pDev->GetInputFullName());
		break;
	
	}
	m_ctrlRelayTree.SelectItem(pItem->hItem);
	return 1;
}

int CDkPaneInputRelay::DeleteTreeItem(int nType, ST_TREEITEM * pItem)
{
	if (pItem == nullptr || pItem->pData == nullptr)
		return 0;

	ST_TREEITEM * pData;
	HTREEITEM hFind = nullptr; 
	hFind = pItem->hItem;
// 	if(hFind == nullptr)
// 		hFind = FindTreeItem(nType, pItem, TVI_ROOT);
	if (hFind == nullptr)
		return 0;
	pData = (ST_TREEITEM *)m_ctrlRelayTree.GetItemData(hFind);

	if (pData == nullptr)
		return 1;
	m_ctrlRelayTree.DeleteItem(hFind);

	POSITION pos = m_ptrItemList.Find(pData);
	if (pos == nullptr)
		return 1;
	m_ptrItemList.RemoveAt(pos);
	delete pData;
	pData = nullptr;

	// 	m_ctrlRelayTree.SetItemState(hItem, TVIF_STATE | TVIS_SELECTED, TVIF_STATE | TVIS_SELECTED);
	// 	m_ctrlRelayTree.SetFocus();
	return 1;
}

void CDkPaneInputRelay::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_ctrlRelayTree.GetSafeHwnd())
	{
		m_ctrlRelayTree.DeleteAllItems();
	}
	CDockablePane::OnClose();
}


void CDkPaneInputRelay::OnDestroy()
{
	CDockablePane::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_ctrlRelayTree.GetSafeHwnd())
	{
		m_ctrlRelayTree.DeleteAllItems();
	}
}
