// DlgChildRelayTree.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildRelayTree.h"
#include "afxdialogex.h"
#include "DataChannel.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataSystem.h"
#include "DataDevice.h"
#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"

// CDlgChildRelayTree ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgChildRelayTree, CDialogEx)

CDlgChildRelayTree::CDlgChildRelayTree(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CHILD_RELAYTREE, pParent)
{
	m_pRefFasSysData = nullptr;
	m_pMainForm = nullptr;
}

CDlgChildRelayTree::~CDlgChildRelayTree()
{
	RemoveAllData();
}

void CDlgChildRelayTree::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_ctrlRelay);
}


BEGIN_MESSAGE_MAP(CDlgChildRelayTree, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CDlgChildRelayTree::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgChildRelayTree::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CDlgChildRelayTree::OnTvnSelchangedTree)
	ON_TVN_CHECK_ITEM(IDC_TREE, &CDlgChildRelayTree::OnTvnCheckChangeItem)
END_MESSAGE_MAP()


// CDlgChildRelayTree �޽��� ó�����Դϴ�.

BOOL CDlgChildRelayTree::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_pRefFasSysData = theApp.GetRelayTableData();

	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlRelay, DKPTREE_ADDRESS_VIEW, &m_ptrItemList, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgChildRelayTree::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	RemoveAllData();
	CDialogEx::OnClose();
}

void CDlgChildRelayTree::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_ctrlRelay.GetSafeHwnd())
		m_ctrlRelay.MoveWindow(0, 0, cx, cy);
}



void CDlgChildRelayTree::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
}


void CDlgChildRelayTree::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnCancel();
}


void CDlgChildRelayTree::RemoveAllData()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	ST_TREEITEM * pTData;
	while (!m_ptrItemList.IsEmpty())
	{
		pTData = (ST_TREEITEM *)m_ptrItemList.RemoveHead();
		if (pTData == nullptr)
			continue;
		delete pTData;
		pTData = nullptr;
	}
}
void CDlgChildRelayTree::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (pNMTreeView == nullptr)
		return; 
	if (pNMTreeView->itemNew.hItem == nullptr)
	{
		m_pMainForm->SendMessage(UWM_RELAYEDITFORM_TREESELCHANGE, 0, 0);
		return;
	}
	ST_TREEITEM * pItem; 
	pItem = (ST_TREEITEM*)m_ctrlRelay.GetItemData(pNMTreeView->itemNew.hItem);
	if (pItem == nullptr)
	{
		m_pMainForm->SendMessage(UWM_RELAYEDITFORM_TREESELCHANGE, 0, 0);
		return;
	}
	if(m_pMainForm)
		m_pMainForm->SendMessage(UWM_RELAYEDITFORM_TREESELCHANGE, 0, (LPARAM)pItem);
	*pResult = 0;
}

void CDlgChildRelayTree::OnTvnCheckChangeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVNCHECK pNmCheck = reinterpret_cast<LPNMTVNCHECK>(pNMHDR);
	// ���� ���õ� �׸� ��û
	*pResult = 0;
}


int CDlgChildRelayTree::RefreshTree(int nType, int nFid, int nUid, int nCid, int nRid)
{
	CDataFacp * pFacp = nullptr;
	CDataUnit * pUnit = nullptr;
	CDataChannel * pChn = nullptr;
	CDataDevice * pDevice = nullptr;

	m_ctrlRelay.DeleteAllItems();
	RemoveAllData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlRelay, DKPTREE_ADDRESS_VIEW, &m_ptrItemList, TRUE);

	if (nFid <= 0)
		return 1;

	HTREEITEM hItem;
	hItem = FindTreeItem(nType, nFid,nUid,nCid,nRid, TVI_ROOT);
	if (hItem == nullptr)
		return 0; 
	m_ctrlRelay.Select(hItem, TVGN_FIRSTVISIBLE);
	m_ctrlRelay.SetItemState(hItem, TVIF_STATE | TVIS_SELECTED, TVIF_STATE | TVIS_SELECTED);
	m_ctrlRelay.SetFocus();
	return 1;
}


HTREEITEM CDlgChildRelayTree::FindTreeItem(int nFindType, int nFid, int nUid, int nCid, int nRid, HTREEITEM hItem)
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
		dwTemp = m_ctrlRelay.GetItemData(hItem);
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
					&& pChn->GetChnID() == nCid )
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

	// �ڽ� ��带 ã�´�.
	hItemChile = m_ctrlRelay.GetChildItem(hItem);
	while (hItemChile)
	{
		// check the children of the current item
		HTREEITEM hFound = FindTreeItem(nFindType, nFid, nUid, nCid, nRid, hItemChile);
		if (hFound)
			return hFound;

		// get the next sibling of the current item
		hItemChile = m_ctrlRelay.GetNextSiblingItem(hItemChile);
	}
	return nullptr;
}

int CDlgChildRelayTree::ReloadData()
{
	m_ctrlRelay.DeleteAllItems();
	RemoveAllData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlRelay, DKPTREE_ADDRESS_VIEW, &m_ptrItemList, TRUE);
	return 0;
}
