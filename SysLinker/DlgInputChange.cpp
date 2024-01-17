// DlgInputChange.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInputChange.h"
#include "afxdialogex.h"

#include "RelayTableData.h"
#include "DataDevice.h"
#include "DataSystem.h"

#include "RTableUpdate.h"
// CDlgInputChange 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputChange, CDialogEx)

CDlgInputChange::CDlgInputChange(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLGINPUTCHANGE, pParent)
{
	m_pVtIns = nullptr;
	m_pVtOld = nullptr;
	m_pVtNew = nullptr;
	m_pRefUSEDDupList = nullptr;
}

CDlgInputChange::~CDlgInputChange()
{
	m_pVtIns = nullptr;
	m_pVtOld = nullptr;
	m_pVtNew = nullptr;
	m_pRefUSEDDupList = nullptr;
}

void CDlgInputChange::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INPUT, m_ctrlInputList);
}


BEGIN_MESSAGE_MAP(CDlgInputChange, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CDlgInputChange::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgInputChange::OnBnClickedCancel)
	ON_NOTIFY(LVN_XL_CHECK, IDC_LIST_INPUT, OnInputListCheck)
END_MESSAGE_MAP()


// CDlgInputChange 메시지 처리기입니다.


BOOL CDlgInputChange::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	VERIFY(m_ctrlInputList.m_cImageList.Create(IDB_CHECKBOXES, 16, 3, RGB(255, 0, 255)));
	m_ctrlInputList.m_HeaderCtrl.SetImageList(&m_ctrlInputList.m_cImageList);
	m_ctrlInputList.InsertColumn(0, _T("이전회로"), LVCFMT_LEFT, 350);
	m_ctrlInputList.InsertColumn(1, _T("새 회로"), LVCFMT_LEFT, 350);
	m_ctrlInputList.InsertColumn(2, _T("수정"), LVCFMT_LEFT, 70);
	m_ctrlInputList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	int i;
	HDITEM hditem;
	for (i = 0; i < m_ctrlInputList.m_HeaderCtrl.GetItemCount(); i++)
	{
		hditem.mask = HDI_IMAGE | HDI_FORMAT;
		m_ctrlInputList.m_HeaderCtrl.GetItem(i, &hditem);
		hditem.fmt |= HDF_IMAGE;
		if (i == 2) // 2번째 column을 check box로 setting
			hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
		else
			hditem.iImage = XHEADERCTRL_NO_IMAGE;

		m_ctrlInputList.m_HeaderCtrl.SetItem(i, &hditem);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgInputChange::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_ctrlInputList.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(&rc);
		rc.DeflateRect(4, 4, 4, 4);
		m_ctrlInputList.MoveWindow(&rc);
	}
}


void CDlgInputChange::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


void CDlgInputChange::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}

void CDlgInputChange::DisplayCompareResult(
	int nCompareType
	, CMapSystemData * pOldMap
	, CMapSystemData * pNewMap
	, VT_RUPDATEITEM * pVtIns
	, VT_RUPDATEITEM * pVtOld
	, VT_RUPDATEITEM * pVtNew
	, CPtrList			* pRefUSEDDupList
	, CWnd				* pMainWnd
)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CRUpdateItem * pItem = nullptr;
	CString strKey;
	int nIdx = 0 , i , nCnt , nStep=0;
	COLORREF crFore, crBack;
	SU_LPARAM suL;
	m_ctrlInputList.DeleteAllItems();

	m_pVtIns = pVtIns;
	m_pVtOld = pVtOld;
	m_pVtNew = pVtNew;
	m_pRefUSEDDupList = pRefUSEDDupList;

	suL.stParam.wAllCnt = pVtIns->size() + pVtOld->size() + pVtNew->size();
	crFore = RGB(255, 255, 255);
	crBack = RGB(255, 0, 0);
	nCnt = pVtIns->size();
	for (i = 0 ; i < nCnt ; i++)
	{
		nStep++;
		pItem = (*pVtIns)[i];
		if (pItem == nullptr)
			continue; 

		if (pItem->m_pNewCopyDev == nullptr || pItem->m_pOldCopyDev == nullptr)
			continue;

		m_ctrlInputList.InsertItem(nIdx, pItem->m_pOldCopyDev->GetInputFullName(),crFore,crBack);
		m_ctrlInputList.SetItemText(nIdx,1, pItem->m_pNewCopyDev->GetInputFullName(), crFore, crBack);
		m_ctrlInputList.SetItemText(nIdx, 2, L"변경");
		m_ctrlInputList.SetCheckbox(nIdx, 2, 1);
		m_ctrlInputList.SetItemData(nIdx, (DWORD_PTR)pItem);
		nIdx++;
		suL.stParam.wStep = nStep;
		pMainWnd->PostMessageW(CSWM_PROGRESS_STEP, PROG_RESULT_STEP, suL.dwParam);
	}

	crFore = RGB(255, 255, 255);
	crBack = RGB(0, 0, 255);
	nCnt = pVtOld->size();
	for (i = 0; i < nCnt; i++)
	{
		nStep++;
		pItem = (*pVtOld)[i];
		if (pItem == nullptr)
			continue;

		if (pItem->m_pOldCopyDev == nullptr)
			continue;

		m_ctrlInputList.InsertItem(nIdx, pItem->m_pOldCopyDev->GetInputFullName(), crFore, crBack);
		m_ctrlInputList.SetItemText(nIdx, 1, L"", crFore, crBack);
		m_ctrlInputList.SetItemText(nIdx, 2, L"삭제");
		m_ctrlInputList.SetCheckbox(nIdx, 2, 1);
		m_ctrlInputList.SetItemData(nIdx, (DWORD_PTR)pItem);
		nIdx++;
		suL.stParam.wStep = nStep;
		pMainWnd->PostMessageW(CSWM_PROGRESS_STEP, PROG_RESULT_STEP, suL.dwParam);
	}

	crFore = RGB(0, 0, 0);
	crBack = RGB(0, 255, 0);
	nCnt = pVtNew->size();
	for (i = 0; i < nCnt; i++)
	{
		nStep++;
		pItem = (*pVtNew)[i];
		if (pItem == nullptr)
			continue;

		if (pItem->m_pNewCopyDev == nullptr )
			continue;

		m_ctrlInputList.InsertItem(nIdx, L"", crFore, crBack);
		m_ctrlInputList.SetItemText(nIdx, 1, pItem->m_pNewCopyDev->GetInputFullName(), crFore, crBack);
		m_ctrlInputList.SetItemText(nIdx, 2, L"추가");
		m_ctrlInputList.SetCheckbox(nIdx, 2, 1);
		m_ctrlInputList.SetItemData(nIdx, (DWORD_PTR)pItem);
		nIdx++;
		suL.stParam.wStep = nStep;
		pMainWnd->PostMessageW(CSWM_PROGRESS_STEP, PROG_RESULT_STEP, suL.dwParam);
	}
}

void CDlgInputChange::OnInputListCheck(NMHDR *pNMHDR, LRESULT *pResult)
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
	pItem = (CRUpdateItem*)m_ctrlInputList.GetItemData(nIdx);
	if (pItem == nullptr)
		return;
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
	CDataDevice * pDev = pItem->GetOldDevice();
	if (pDev == nullptr)
		return; 
	SetOutputEnable(pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum(), nChecked);
}

void CDlgInputChange::SetOutputEnable(int nFacpNum, int nUnitNum, int nChnNum, int nDevNum , BOOL bEnable)
{
	if (m_pRefUSEDDupList == nullptr)
		return; 

	CRTableUpdate * pUpdate; 
	CRUpdateItem * pItem;
	POSITION pos;
	int nCnt, i;
	pos = m_pRefUSEDDupList->GetHeadPosition();
	while (pos)
	{
		pUpdate = (CRTableUpdate*)m_pRefUSEDDupList->GetNext(pos);
		if (pUpdate == nullptr)
			continue; 
		nCnt = pUpdate->m_vtUpdate.size();
		for (i = 0; i < nCnt; i++)
		{
			pItem = pUpdate->m_vtUpdate[i];
			if (pItem == nullptr || pItem->m_pOldCopyDev == nullptr)
				continue; 

			if (pItem->m_pOldCopyDev->GetFacpNum() == nFacpNum
				&& pItem->m_pOldCopyDev->GetUnitNum() == nUnitNum
				&& pItem->m_pOldCopyDev->GetChnNum() == nChnNum
				&& pItem->m_pOldCopyDev->GetDeviceNum() == nDevNum
				)
			{
				pItem->m_bEditable = bEnable;
				if (bEnable)
				{
					pItem->m_bChangeFlag = pItem->m_bPrevStatus;
				}
				else
					pItem->m_bChangeFlag = FALSE;
			}
		}
	}
}
