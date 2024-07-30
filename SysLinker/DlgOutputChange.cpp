// DlgOutputChange.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgOutputChange.h"
#include "afxdialogex.h"
#include "RTableUpdate.h"
#include "DataDevice.h"
#include "DataSystem.h"
#include "MapSystemData.h"
#include "RelayTableData.h"


// CDlgOutputChange ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgOutputChange, CDialogEx)

CDlgOutputChange::CDlgOutputChange(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLGOUTPUTCHANGE, pParent)
{
	memset(m_hItem, 0, sizeof(HTREEITEM) * D_MAX_FACP_COUNT * D_MAX_UNIT_COUNT * 4);
}

CDlgOutputChange::~CDlgOutputChange()
{
	
}

void CDlgOutputChange::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUT_TREE, m_ctrlOutTree);
	DDX_Control(pDX, IDC_RELAY_LIST, m_ctrlRelayList);
}


BEGIN_MESSAGE_MAP(CDlgOutputChange, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CDlgOutputChange::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgOutputChange::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_OUT_TREE, &CDlgOutputChange::OnTvnSelchangedOutTree)
	ON_NOTIFY(LVN_XL_CHECK, IDC_RELAY_LIST, OnRelayListCheck)
END_MESSAGE_MAP()


// CDlgOutputChange �޽��� ó�����Դϴ�.


void CDlgOutputChange::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SetSplitterPos();
}


BOOL CDlgOutputChange::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rc;
	GetClientRect(&rc);
	rc.DeflateRect(4, 4, 4, 4);
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	if (m_SpMain.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,            // the parent of the splitter pane
		&m_ctrlOutTree,    // left pane
		&m_ctrlRelayList,    // right pane
		IDC_STATIC_SLIT_PANE, // this ID is used for saving/restoring splitter
							  // position and therefore it must be unique 
							  // within your application
		rc,        // dimensions of the splitter pane
		90,            // left constraint for splitter position
		110         // right constraint for splitter position
	) == false)
		return -1;

	VERIFY(m_ctrlRelayList.m_cImageList.Create(IDB_CHECKBOXES, 16, 3, RGB(255, 0, 255)));
	m_ctrlRelayList.m_HeaderCtrl.SetImageList(&m_ctrlRelayList.m_cImageList);
#ifndef ENGLISH_MODE
	m_ctrlRelayList.InsertColumn(0, _T("���� ȸ��"), LVCFMT_LEFT, 350);
	m_ctrlRelayList.InsertColumn(1, _T("�� ȸ��"), LVCFMT_LEFT, 350);
	m_ctrlRelayList.InsertColumn(2, _T("����"), LVCFMT_LEFT, 70);
#else
	m_ctrlRelayList.InsertColumn(0, _T("PREVIOUS CIRCUIT"), LVCFMT_LEFT, 350);
	m_ctrlRelayList.InsertColumn(1, _T("NEW CIRCUIT"), LVCFMT_LEFT, 350);
	m_ctrlRelayList.InsertColumn(2, _T("EDIT"), LVCFMT_LEFT, 70);
#endif
	m_ctrlRelayList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	int i;
	HDITEM hditem;
	for (i = 0; i < m_ctrlRelayList.m_HeaderCtrl.GetItemCount(); i++)
	{
		hditem.mask = HDI_IMAGE | HDI_FORMAT;
		m_ctrlRelayList.m_HeaderCtrl.GetItem(i, &hditem);
		hditem.fmt |= HDF_IMAGE;
		if (i == 2) // 2��° column�� check box�� setting
			hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
		else
			hditem.iImage = XHEADERCTRL_NO_IMAGE;

		m_ctrlRelayList.m_HeaderCtrl.SetItem(i, &hditem);
	}


	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDlgOutputChange::SetSplitterPos()
{
	if (m_SpMain.GetSafeHwnd() == nullptr)
		return;
	if (m_SpMain.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(&rc);
		rc.DeflateRect(4, 4, 4, 4);
		m_SpMain.MoveWindow(&rc);
	}
}

void CDlgOutputChange::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
}


void CDlgOutputChange::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnCancel();
}

void CDlgOutputChange::DisplayCompareResult(
	int nCompareType
	, CMapSystemData * pOldMap
	, CMapSystemData * pNewMap
	, CPtrList * pPtrUSINGDupList
	, CWnd				* pMainWnd
)
{
	CRTableUpdate * pUpdate;
	POSITION pos;
	HTREEITEM hRoot , hItem;
	SU_LPARAM suL;
	int nStep = 0; 
	suL.stParam.wAllCnt = pPtrUSINGDupList->GetCount();
	m_ctrlOutTree.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
#ifndef ENGLISH_MODE
	hRoot = m_ctrlOutTree.InsertItem(L"���", 0, 0, TVI_ROOT);
#else
	hRoot = m_ctrlOutTree.InsertItem(L"OUTPUT", 0, 0, TVI_ROOT);
#endif
	pos = pPtrUSINGDupList->GetHeadPosition();
	while (pos)
	{
		nStep++;
		pUpdate = (CRTableUpdate *)pPtrUSINGDupList->GetNext(pos);
		if (pUpdate == nullptr 
			|| pUpdate->m_pOldSourceCopyDev == nullptr || pUpdate->m_pNewSourceCopyDev == nullptr)
			continue; 

		hItem = m_ctrlOutTree.InsertItem(pUpdate->m_pOldSourceCopyDev->GetInputFullName(), 1, 1, hRoot);
		m_ctrlOutTree.SetItemData(hItem, (DWORD_PTR)pUpdate);
		suL.stParam.wStep = nStep;
		pMainWnd->PostMessageW(CSWM_PROGRESS_STEP, PROG_RESULT_STEP, suL.dwParam);
	}
}


int CDlgOutputChange::DisplayList(HTREEITEM hItem)
{
	m_ctrlRelayList.DeleteAllItems();
	if (hItem == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return 0;
	}

	CRTableUpdate * pUpdate = (CRTableUpdate*)m_ctrlOutTree.GetItemData(hItem);
	if (pUpdate == nullptr)
	{
		return 0;
	}

	if (pUpdate->m_pNewSourceCopyDev == nullptr || pUpdate->m_pOldSourceCopyDev == nullptr)
	{
		return 0;
	}

	if (pUpdate->m_vtUpdate.size() <= 0)
	{
		return 0; 
	}

	int i, nCnt , nIdx=0; 
	CRUpdateItem * pItem;
	nCnt = pUpdate->m_vtUpdate.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = pUpdate->m_vtUpdate[i];
		if (pItem == nullptr)
			continue; 
		if (pItem->m_pNewCopyDev == nullptr || pItem->m_pOldCopyDev == nullptr)
			continue; 

		m_ctrlRelayList.InsertItem(nIdx, pItem->m_pOldCopyDev->GetOutputFullName());
		m_ctrlRelayList.SetItemText(nIdx, 1, pItem->m_pNewCopyDev->GetOutputFullName());
#ifndef ENGLISH_MODE
		m_ctrlRelayList.SetItemText(nIdx, 2, L"����");
#else
		m_ctrlRelayList.SetItemText(nIdx, 2, L"CHANGE");
#endif
		m_ctrlRelayList.SetCheckbox(nIdx, 2, pItem->m_bChangeFlag);
		m_ctrlRelayList.SetItemData(nIdx, (DWORD_PTR)pItem);
		m_ctrlRelayList.SetEnabled(nIdx, pItem->m_bEditable);
		nIdx++;
	}
	return 1;
}

void CDlgOutputChange::OnTvnSelchangedOutTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem == nullptr)
		return;
	DisplayList(hItem);
}

void CDlgOutputChange::OnRelayListCheck(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if (pNMHDR == nullptr)
		return;

	XNOTIFY_CHECK *pxnData = reinterpret_cast<XNOTIFY_CHECK*>(pNMHDR);
	if (pxnData == nullptr)
		return;
	int nChecked = 0;
	CRUpdateItem * pItem = nullptr;
	int nIdx = pxnData->dispinfo.item.iItem;
	if (nIdx < 0)
		return;

	nChecked = pxnData->nCheckState;
	pItem = (CRUpdateItem*)m_ctrlRelayList.GetItemData(nIdx);
	if (pItem == nullptr)
		return;
	
	if (pItem->m_bEditable == FALSE)
		pxnData->nCheckState = !nChecked;

	if (nChecked)
	{
		pItem->m_bChangeFlag = TRUE;
		pItem->m_bPrevStatus = TRUE;
	}
	else
	{
		pItem->m_bChangeFlag = FALSE;
		pItem->m_bPrevStatus = FALSE;
	}
}
