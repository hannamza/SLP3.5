// DlgTreePane.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgTreePane.h"
#include "afxdialogex.h"
#include "DataPS.h"
#include "DataFacp.h"
#include "ManagerPS.h"
#include "DataPump.h"
#include "ManagerPump.h"
#include "RelayTableData.h"
// CDlgTreePane 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgTreePane, CDialogEx)

CDlgTreePane::CDlgTreePane(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_TREEPANE, pParent)
{
	m_pRefFasSysData = nullptr;
	m_pMainForm = nullptr;
	m_nPaneType = 1; // default presure switch
}

CDlgTreePane::~CDlgTreePane()
{
	RemoveAllData();
}

void CDlgTreePane::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_ctrlTree);
}


BEGIN_MESSAGE_MAP(CDlgTreePane, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgTreePane::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgTreePane::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CDlgTreePane::OnTvnSelchangedTree)
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE, &CDlgTreePane::OnNMSetfocusTree)
END_MESSAGE_MAP()


// CDlgTreePane 메시지 처리기입니다.


BOOL CDlgTreePane::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CBitmap bmp;
	if (!bmp.LoadBitmap(IDB_BMP_PSWITCH_TREE))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), IDB_BMP_PSWITCH_TREE);
		ASSERT(FALSE);
		return TRUE; 
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ImageTree.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ImageTree.Add(&bmp, RGB(255, 0, 0));
	m_ctrlTree.SetImageList(&m_ImageTree, TVSIL_NORMAL);

	if (AfxGetMainWnd())
		m_pRefFasSysData = theApp.GetRelayTableData();
	if (m_nPaneType  == 1)
		FillPSwitchTree();
	else
		FillPumpTree();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgTreePane::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}

void CDlgTreePane::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}


void CDlgTreePane::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_ctrlTree.GetSafeHwnd())
		m_ctrlTree.MoveWindow(0, 0, cx, cy);
}


void CDlgTreePane::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hSel = pNMTreeView->itemNew.hItem;
	if (hSel == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;
	DWORD_PTR dwData;
	dwData = m_ctrlTree.GetItemData(hSel);
// 	if (dwData == 0)
// 		return;
// #ifdef _DEBUG
// 	if (m_nPaneType == 1)
// 	{
// 		//FillPSwitchTree();
// 		CDataPS * pData;
// 		pData = (CDataPS *)dwData;
// 		TRACE(L"");
// 	}
// 	else
// 	{
// 		//FillPumpTree();
// 		CDataPump * pData;
// 		pData = (CDataPump *)dwData;
// 		TRACE(L"");
// 	}
// #endif
	if (m_pMainForm)
		m_pMainForm->SendMessage(UWM_CHILDPANE_TREESELCHANGE, 0, dwData);

	*pResult = 0;
}

void CDlgTreePane::RemoveAllData()
{
	ST_TREEPANE_DATA *pData;
	while (!m_ptrItemList.IsEmpty())
	{
		pData = (ST_TREEPANE_DATA *)m_ptrItemList.RemoveHead();
		if (pData == nullptr)
			continue; 
		delete pData;
		pData = nullptr;
	}
}

int CDlgTreePane::FillPSwitchTree()
{
	m_ctrlTree.DeleteAllItems();
	HTREEITEM hRoot = m_ctrlTree.InsertItem(L"압력스위치 연동", 0, 0, TVI_ROOT);
	m_ctrlTree.SetItemData(hRoot, 0);
	if (m_pRefFasSysData == nullptr)
		return 0;
	ST_TREEPANE_DATA * pTData = nullptr;
	std::shared_ptr<CManagerPS> spPSwitch;
	std::map<int, CDataFacp*> * pMapFacp;
	std::map<int, CDataFacp*>::iterator it;
	CDataFacp * pFacp;
	CDataPS * pData;
	POSITION pos;
	CString strName;
	HTREEITEM hFacp[D_MAX_FACP_COUNT + 1] = { nullptr };
	HTREEITEM hItem;
	pMapFacp = m_pRefFasSysData->GetFacpMap();
	if (pMapFacp == nullptr)
		return 0;
	for (it = pMapFacp->begin(); it != pMapFacp->end(); it++)
	{
		pFacp = it->second;
		if (pFacp == nullptr)
			continue;
		strName.Format(L"[%02d] %s" , pFacp->GetFacpNum(), pFacp->GetFacpName());
		hFacp[pFacp->GetFacpID()] = m_ctrlTree.InsertItem(strName, 1, 1, hRoot);
		pTData = new ST_TREEPANE_DATA;
		pTData->nType = TTYPE_DEV_FACP;
		pTData->pData = pFacp;
		m_ctrlTree.SetItemData(hFacp[pFacp->GetFacpID()],(DWORD_PTR)pTData);
		m_ptrItemList.AddTail(pTData);
	}

	spPSwitch = m_pRefFasSysData->GetPSwitchManager();
	if (spPSwitch == nullptr)
		return 0;
	pos = spPSwitch->GetHeadPosition();
	while (pos)
	{
		pData = spPSwitch->GetNext(pos);
		if (pData == nullptr)
			continue;
		strName.Format(L"[%02d] %s", pData->GetPSwitchID(), pData->GetPSwitchName());
		hItem = m_ctrlTree.InsertItem(strName, 2, 2, hFacp[pData->GetFacpID()]);
		pTData = new ST_TREEPANE_DATA;
		pTData->nType = TTYPE_DEV_PSWITCH;
		pTData->pData = pData;
		m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pTData);
		m_ptrItemList.AddTail(pTData);
	}
	return 1;
}

int CDlgTreePane::FillPumpTree()
{
	m_ctrlTree.DeleteAllItems();
	HTREEITEM hRoot = m_ctrlTree.InsertItem(L"펌프 연동", 0, 0, TVI_ROOT);
	m_ctrlTree.SetItemData(hRoot, 0);
	if (m_pRefFasSysData == nullptr)
		return 0;
	ST_TREEPANE_DATA * pTData = nullptr;
	std::shared_ptr<CManagerPump> spPump;
	std::map<int, CDataFacp*> * pMapFacp;
	std::map<int, CDataFacp*>::iterator it;
	CDataFacp * pFacp;
	CDataPump * pData;
	CString strName;
	POSITION pos;
	HTREEITEM hFacp[D_MAX_FACP_COUNT + 1] = { nullptr };
	HTREEITEM hItem;
	pMapFacp = m_pRefFasSysData->GetFacpMap();
	if (pMapFacp == nullptr)
		return 0;
	for (it = pMapFacp->begin(); it != pMapFacp->end(); it++)
	{
		pFacp = it->second;
		if (pFacp == nullptr)
			continue;
		strName.Format(L"[%02d] %s", pFacp->GetFacpNum(), pFacp->GetFacpName());
		hFacp[pFacp->GetFacpID()] = m_ctrlTree.InsertItem(strName, 1, 1, hRoot);
		//hFacp[pFacp->GetFacpID()] = m_ctrlTree.InsertItem(pFacp->GetFacpName(), 1, 1, hRoot);
		pTData = new ST_TREEPANE_DATA;
		pTData->nType = TTYPE_DEV_FACP;
		pTData->pData = pFacp;
		m_ctrlTree.SetItemData(hFacp[pFacp->GetFacpID()], (DWORD_PTR)pTData);
		m_ptrItemList.AddTail(pTData);
	}

	spPump = m_pRefFasSysData->GetPumpManager();
	if (spPump == nullptr)
		return 0;
	pos = spPump->GetHeadPosition();
	while (pos)
	{
		pData = spPump->GetNext(pos);
		if (pData == nullptr)
			continue;
		strName.Format(L"[%02d] %s", pData->GetPumpID(), pData->GetPumpName());
		hItem = m_ctrlTree.InsertItem(strName, 2, 2, hFacp[pData->GetFacpID()]);
		pTData = new ST_TREEPANE_DATA;
		pTData->nType = TTYPE_DEV_PUMP;
		pTData->pData = pData;
		m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pTData);
		m_ptrItemList.AddTail(pTData);
	}
	return 1;
}
BOOL CDlgTreePane::CreateDlg(int nPaneType , CWnd * pParent)
{
	m_nPaneType = nPaneType;
	return Create(IDD_DLG_TREEPANE, pParent);
}

int CDlgTreePane::InitDlg()
{
	if (AfxGetMainWnd())
		m_pRefFasSysData = theApp.GetRelayTableData();
	if (m_nPaneType == 1)
		FillPSwitchTree();
	else
		FillPumpTree();
	m_ctrlTree.Expand(TVI_ROOT, TVE_EXPAND);
	return 1;
}

HTREEITEM CDlgTreePane::FindTreeItem(int nFindType, int nFacpID, int nPID,HTREEITEM hItem)
{
	ST_TREEPANE_DATA * pTData = nullptr;
	CDataPS * pPs;
	CDataFacp *pFacp;
	CDataPump * pPump;
	DWORD_PTR dwTemp;
	HTREEITEM hitemFind, hItemChile, hItemSibling;
	hitemFind = hItemChile = hItemSibling = NULL;
	if (hItem == nullptr )
		return nullptr;
	if (hItem != TVI_ROOT)
		dwTemp = m_ctrlTree.GetItemData(hItem);
	else
		dwTemp = 0;
	pTData = (ST_TREEPANE_DATA *)dwTemp;

	if (pTData != 0 && pTData->pData != nullptr && pTData->nType == nFindType)
	{
		if (m_nPaneType == 1) //PSwitch
		{
			if (pTData->nType == 1)
			{
				pFacp = (CDataFacp*)pTData->pData;
				if (pFacp->GetFacpID() == nFacpID)
					return hItem; 
			}
			else
			{
				pPs = (CDataPS*)pTData->pData;
				if (pPs->GetFacpID() == nFacpID)
				{
					if (nPID > 0)
					{
						if (pPs->GetPSwitchID() == nPID)
							return hItem;
					}
					else
						return hItem;
				}

			}
			
		}
		else
		{
			if (pTData->nType == 1)
			{
				pFacp = (CDataFacp*)pTData->pData;
				if (pFacp->GetFacpID() == nFacpID)
					return hItem;
			}
			else
			{
				pPump = (CDataPump *)pTData->pData;
				if (pPump->GetFacpID() == nFacpID)
				{
					if (nPID > 0)
					{
						if (pPump->GetPumpID() == nPID)
							return hItem;
					}
					else
						return hItem;
				}
			}
		}
	}

	// 자식 노드를 찾는다.
	hItemChile = m_ctrlTree.GetChildItem(hItem);
	while (hItemChile)
	{
		// check the children of the current item
		HTREEITEM hFound = FindTreeItem(nFindType, nFacpID, nPID, hItemChile);
		if (hFound)
			return hFound;

		// get the next sibling of the current item
		hItemChile = m_ctrlTree.GetNextSiblingItem(hItemChile);
	}
	return nullptr;
}

int CDlgTreePane::AddTreeData(LPVOID pData)
{
	ST_TREEPANE_DATA * pTData = nullptr;
	//CDataPS * pPs , *pSNew;
	//CDataPump *pPump , *pPNew;
	HTREEITEM hItem , hData;
	if (pData == nullptr)
		return 0; 
	if (m_nPaneType == 1)
	{
		//pPs = (CDataPS *)pData;
		// [KHS 2020-2-7 16:41:14] 
		// GetRootItem --> TVI_ROOT 변환
		// hItem = FindTreeItem(TTYPE_DEV_FACP, pPs->GetFacpID(),0, m_ctrlTree.GetRootItem());
		hItem = FindTreeItem(TTYPE_DEV_FACP,((CDataPS *)pData)->GetFacpID(),0,TVI_ROOT);
		if (hItem == nullptr)
			return 0; 
// 		pSNew = new CDataPS;
// 		pSNew->CopyData(pPs);
		pTData = new ST_TREEPANE_DATA;
		pTData->nType = TTYPE_DEV_PSWITCH;
		pTData->pData = pData;
		hData = m_ctrlTree.InsertItem(((CDataPS *)pData)->GetPSwitchName(), 2, 2, hItem);
		m_ctrlTree.SetItemData(hData, (DWORD_PTR)pTData);
	}
	else
	{
		//pPump = (CDataPump *)pData;
		
		// [KHS 2020-2-7 16:41:14] 
				// GetRootItem --> TVI_ROOT 변환
				// hItem = FindTreeItem(TTYPE_DEV_FACP, pPump->GetFacpID(), 0, m_ctrlTree.GetRootItem());
		hItem = FindTreeItem(TTYPE_DEV_FACP,((CDataPump *)pData)->GetFacpID(), 0, TVI_ROOT);
		if (hItem == nullptr)
			return 0;
// 		pPNew = new CDataPump;
// 		pPNew->CopyData(pPump);
		pTData = new ST_TREEPANE_DATA;
		pTData->nType = TTYPE_DEV_PUMP;
		pTData->pData = pData;
		hData = m_ctrlTree.InsertItem(((CDataPump *)pData)->GetPumpName(), 2, 2, hItem);
		m_ctrlTree.SetItemData(hData, (DWORD_PTR)pTData);
	}
	if(pTData !=nullptr)
		m_ptrItemList.AddTail(pTData);
	return 1;
}

int CDlgTreePane::ChangeTreeData(LPVOID pData)
{
	ST_TREEPANE_DATA * pTData = nullptr;
	CDataPS * pPs , * pSTemp;
	CDataPump *pPump , *pPTemp;
	HTREEITEM hItem;
	if (pData == nullptr)
		return 0;
	if (m_nPaneType == 1)
	{
		pPs = (CDataPS *)pData;
		// [KHS 2020-2-7 16:41:14] 
				// GetRootItem --> TVI_ROOT 변환
				// hItem = FindTreeItem(TTYPE_DEV_PSWITCH, pPs->GetFacpID(), pPs->GetPSwitchID(), m_ctrlTree.GetRootItem());
		hItem = FindTreeItem(TTYPE_DEV_PSWITCH, pPs->GetFacpID(), pPs->GetPSwitchID(), TVI_ROOT);
		if (hItem == nullptr)
			return 0;
		pTData = (ST_TREEPANE_DATA *)m_ctrlTree.GetItemData(hItem);
		if (pTData == nullptr)
		{
			pSTemp = new CDataPS;
			pSTemp->CopyData(pPs);
			pTData = new ST_TREEPANE_DATA;
			pTData->nType = TTYPE_DEV_PSWITCH;
			pTData->pData = pSTemp;
			m_ptrItemList.AddTail(pTData);
			m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pTData);
		}
		else
		{
			pSTemp = (CDataPS *)pTData->pData;
			pSTemp->CopyData(pPs);
		}
		m_ctrlTree.SetItemText(hItem, pPs->GetPSwitchName());
	}
	else
	{
		pPump = (CDataPump *)pData;
		// [KHS 2020-2-7 16:41:14] 
				// GetRootItem --> TVI_ROOT 변환
				// hItem = FindTreeItem(TTYPE_DEV_PUMP, pPump->GetFacpID(), pPump->GetPumpID(), m_ctrlTree.GetRootItem());
		hItem = FindTreeItem(TTYPE_DEV_PUMP, pPump->GetFacpID(), pPump->GetPumpID(), TVI_ROOT);
		if (hItem == nullptr)
			return 0;
		pTData = (ST_TREEPANE_DATA *)m_ctrlTree.GetItemData(hItem);
		if (pTData == nullptr)
		{
			pPTemp = new CDataPump;
			pPTemp->CopyData(pPump);
			pTData = new ST_TREEPANE_DATA;
			pTData->nType = TTYPE_DEV_PUMP;
			pTData->pData = pPTemp;
			m_ptrItemList.AddTail(pTData);
			m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pTData);
		}
		else
		{
			pPTemp = (CDataPump *)pTData->pData;
			pPTemp->CopyData(pPump);
		}
		m_ctrlTree.SetItemText(hItem,pPump->GetPumpName());
	}
	return 1;
}


int CDlgTreePane::DeleteTreeData(LPVOID pData)
{
	CDataPS * pPs;
	CDataPump *pPump;
	HTREEITEM hItem;
	POSITION pos;
	ST_TREEPANE_DATA * pTData = nullptr;
	if (pData == nullptr)
		return 0;
	if (m_nPaneType == 1)
	{
		pPs = (CDataPS *)pData;
		// [KHS 2020-2-7 16:41:14] 
				// GetRootItem --> TVI_ROOT 변환
				// hItem = FindTreeItem(TTYPE_DEV_PSWITCH, pPs->GetFacpID(), pPs->GetPSwitchID(), m_ctrlTree.GetRootItem());
		hItem = FindTreeItem(TTYPE_DEV_PSWITCH, pPs->GetFacpID(), pPs->GetPSwitchID(), TVI_ROOT);
		if (hItem == nullptr)
			return 0;
		pTData = (ST_TREEPANE_DATA *)m_ctrlTree.GetItemData(hItem);
		m_ctrlTree.DeleteItem(hItem);
	}
	else
	{
		pPump = (CDataPump *)pData;
		// [KHS 2020-2-7 16:41:14] 
				// GetRootItem --> TVI_ROOT 변환
				// hItem = FindTreeItem(TTYPE_DEV_PUMP, pPump->GetFacpID(), pPump->GetPumpID(), m_ctrlTree.GetRootItem());
		hItem = FindTreeItem(TTYPE_DEV_PUMP, pPump->GetFacpID(), pPump->GetPumpID(), TVI_ROOT);
		if (hItem == nullptr)
			return 0;
		pTData = (ST_TREEPANE_DATA *)m_ctrlTree.GetItemData(hItem);
		m_ctrlTree.DeleteItem(hItem);
	}

	if (pTData)
	{
		pos = m_ptrItemList.Find(pTData);
		if (pos)
			m_ptrItemList.RemoveAt(pos);
		delete pTData;
		pTData = nullptr;
	}
	return 1;
}


void CDlgTreePane::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_ctrlTree.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(&rc);
		m_ctrlTree.MoveWindow(rc);
	}

	//RedrawWindow();
}


void CDlgTreePane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	if (m_ctrlTree.GetSafeHwnd())
	{
		m_ctrlTree.Invalidate();
		//m_ctrlTree.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		CRect rc;
		GetClientRect(&rc);
		m_ctrlTree.MoveWindow(rc);
	}
}


BOOL CDlgTreePane::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CDialogEx::OnEraseBkgnd(pDC);

}


void CDlgTreePane::OnNMSetfocusTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	if (m_ctrlTree.GetSafeHwnd())
	{
		//m_ctrlTree.Invalidate();
		//m_ctrlTree.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		CRect rc;
		GetClientRect(&rc);
		m_ctrlTree.MoveWindow(rc);
	}
}
