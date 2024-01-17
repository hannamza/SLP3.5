// DlgPatternChange.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgPatternChange.h"
#include "afxdialogex.h"
#include "RTableUpdate.h"
#include "DataDevice.h"
#include "DataPattern.h"


// CDlgPatternChange 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgPatternChange, CDialogEx)

CDlgPatternChange::CDlgPatternChange(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLGPATTERNCHANGE, pParent)
{

}

CDlgPatternChange::~CDlgPatternChange()
{
}

void CDlgPatternChange::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATTERN_TREE, m_ctrlPatternTree);
	DDX_Control(pDX, IDC_RELAY_LIST, m_ctrlRelayList);
}


BEGIN_MESSAGE_MAP(CDlgPatternChange, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CDlgPatternChange::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgPatternChange::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PATTERN_TREE, &CDlgPatternChange::OnTvnSelchangedPatternTree)
	ON_NOTIFY(LVN_XL_CHECK, IDC_RELAY_LIST, OnRelayListCheck)
END_MESSAGE_MAP()

BOOL CDlgPatternChange::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rc;
	GetClientRect(&rc);
	rc.DeflateRect(4, 4, 4, 4);
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (m_SpMain.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,            // the parent of the splitter pane
		&m_ctrlPatternTree,    // left pane
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
	m_ctrlRelayList.InsertColumn(0, _T("이전회로"), LVCFMT_LEFT, 350);
	m_ctrlRelayList.InsertColumn(1, _T("새 회로"), LVCFMT_LEFT, 350);
	m_ctrlRelayList.InsertColumn(2, _T("수정"), LVCFMT_LEFT, 70);
	m_ctrlRelayList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

// 	int i;
// 	HDITEM hditem;
// 	for (i = 0; i < m_ctrlRelayList.m_HeaderCtrl.GetItemCount(); i++)
// 	{
// 		hditem.mask = HDI_IMAGE | HDI_FORMAT;
// 		m_ctrlRelayList.m_HeaderCtrl.GetItem(i, &hditem);
// 		hditem.fmt |= HDF_IMAGE;
// 		if (i == 2) // 2번째 column을 check box로 setting
// 			hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
// 		else
// 			hditem.iImage = XHEADERCTRL_NO_IMAGE;
// 
// 		m_ctrlRelayList.m_HeaderCtrl.SetItem(i, &hditem);
// 	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


// CDlgPatternChange 메시지 처리기입니다.

void CDlgPatternChange::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetSplitterPos();
}

void CDlgPatternChange::SetSplitterPos()
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

void CDlgPatternChange::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


void CDlgPatternChange::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}

void CDlgPatternChange::DisplayCompareResult(
	int nCompareType
	, CMapSystemData * pOldMap
	, CMapSystemData * pNewMap
	, CPtrList * pPatternList
	, CWnd				* pMainWnd
)
{
	CRPatternUpdate * pUpdate;
	POSITION pos;
	HTREEITEM hRoot, hItem;
	int nIdx=0; 
	SU_LPARAM suL;
	suL.stParam.wAllCnt = pPatternList->GetCount();
	m_ctrlPatternTree.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
	hRoot = m_ctrlPatternTree.InsertItem(L"패턴", 0, 0, TVI_ROOT);
	pos = pPatternList->GetHeadPosition();
	while (pos)
	{
		nIdx++;
		pUpdate = (CRPatternUpdate *)pPatternList->GetNext(pos);
		if(pUpdate == nullptr)
			continue; 
		if(pUpdate->m_nChnageType == 0) // 삭제
		{
			if(pUpdate->m_nOldPID <= 0)
				continue;
			pUpdate->m_strOldPtnName += L"- 전체삭제";
		}
		else
		{
			if(pUpdate->m_nOldPID <= 0 || pUpdate->m_nNewPID <= 0)
				continue;
		}

		hItem = m_ctrlPatternTree.InsertItem(pUpdate->m_strOldPtnName, 1, 1, hRoot);
		m_ctrlPatternTree.SetItemData(hItem, (DWORD_PTR)pUpdate);
		suL.stParam.wStep = nIdx;
		pMainWnd->PostMessageW(CSWM_PROGRESS_STEP, PROG_RESULT_STEP, suL.dwParam);
	}
	m_ctrlPatternTree.Expand(m_ctrlPatternTree.GetRootItem() , TVE_EXPAND);
}


int CDlgPatternChange::DisplayList(HTREEITEM hItem)
{
	m_ctrlRelayList.DeleteAllItems();
	if (hItem == nullptr)
	{
		m_ctrlRelayList.SetRedraw();
		return 0;
	}
	int i,nCnt,nIdx = 0;
	CRUpdateItem * pItem;
	CRPatternUpdate * pUpdate = (CRPatternUpdate*)m_ctrlPatternTree.GetItemData(hItem);
	if (pUpdate == nullptr)
	{
		return 0;
	}

	if(pUpdate->m_nChnageType == 0) // 삭제
	{
		if(pUpdate->m_nOldPID <= 0)
			return 0;
		nCnt = pUpdate->m_vtDel.size();
		for(i = 0; i < nCnt; i++)
		{
			pItem = pUpdate->m_vtDel[i];
			if(pItem == nullptr)
				continue;
			if(pItem->m_pOldCopyDev == nullptr)
				continue;

			m_ctrlRelayList.InsertItem(nIdx,pItem->m_pOldCopyDev->GetOutputFullName());
			m_ctrlRelayList.SetItemText(nIdx,1,L"");
			m_ctrlRelayList.SetItemText(nIdx,2,L"삭제");
			//m_ctrlRelayList.SetCheckbox(nIdx, 2, pItem->m_bChangeFlag);
			m_ctrlRelayList.SetItemData(nIdx,(DWORD_PTR)pItem);
			m_ctrlRelayList.SetEnabled(nIdx,pItem->m_bEditable);
			nIdx++;
		}
	}
	else
	{
		if(pUpdate->m_nOldPID <= 0 || pUpdate->m_nNewPID <= 0)
			return 0;
		nCnt = pUpdate->m_vtIns.size();
		for(i = 0; i < nCnt; i++)
		{
			pItem = pUpdate->m_vtIns[i];
			if(pItem == nullptr)
				continue;
			if(pItem->m_pNewCopyDev == nullptr || pItem->m_pOldCopyDev == nullptr)
				continue;

			m_ctrlRelayList.InsertItem(nIdx,pItem->m_pOldCopyDev->GetOutputFullName());
			m_ctrlRelayList.SetItemText(nIdx,1,pItem->m_pNewCopyDev->GetOutputFullName());
			m_ctrlRelayList.SetItemText(nIdx,2,L"변경");
			//m_ctrlRelayList.SetCheckbox(nIdx, 2, pItem->m_bChangeFlag);
			m_ctrlRelayList.SetItemData(nIdx,(DWORD_PTR)pItem);
			m_ctrlRelayList.SetEnabled(nIdx,pItem->m_bEditable);
			nIdx++;
		}

		nCnt = pUpdate->m_vtAdd.size();
		for(i = 0; i < nCnt; i++)
		{
			pItem = pUpdate->m_vtAdd[i];
			if(pItem == nullptr)
				continue;
			if(pItem->m_pNewCopyDev == nullptr)
				continue;

			m_ctrlRelayList.InsertItem(nIdx,L"");
			m_ctrlRelayList.SetItemText(nIdx,1,pItem->m_pNewCopyDev->GetOutputFullName());
			m_ctrlRelayList.SetItemText(nIdx,2,L"추가");
			//m_ctrlRelayList.SetCheckbox(nIdx, 2, pItem->m_bChangeFlag);
			m_ctrlRelayList.SetItemData(nIdx,(DWORD_PTR)pItem);
			m_ctrlRelayList.SetEnabled(nIdx,pItem->m_bEditable);
			nIdx++;
		}

		nCnt = pUpdate->m_vtDel.size();
		for(i = 0; i < nCnt; i++)
		{
			pItem = pUpdate->m_vtDel[i];
			if(pItem == nullptr)
				continue;
			if(pItem->m_pOldCopyDev == nullptr)
				continue;

			m_ctrlRelayList.InsertItem(nIdx,pItem->m_pOldCopyDev->GetOutputFullName());
			m_ctrlRelayList.SetItemText(nIdx,1,L"");
			m_ctrlRelayList.SetItemText(nIdx,2,L"삭제");
			//m_ctrlRelayList.SetCheckbox(nIdx, 2, pItem->m_bChangeFlag);
			m_ctrlRelayList.SetItemData(nIdx,(DWORD_PTR)pItem);
			m_ctrlRelayList.SetEnabled(nIdx,pItem->m_bEditable);
			nIdx++;
		}
	}
	
	return 1;
}

void CDlgPatternChange::OnTvnSelchangedPatternTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem == nullptr)
		return;
	DisplayList(hItem);
}

void CDlgPatternChange::OnRelayListCheck(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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
