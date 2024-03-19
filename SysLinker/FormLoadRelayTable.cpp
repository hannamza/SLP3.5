// FormLoadRelayTable.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormLoadRelayTable.h"

#include "DlgOutputChange.h"
#include "DlgInputChange.h"
#include "DlgPatternChange.h"

#include "RelayTableData.h"
#include "DataDevice.h"
#include "DataSystem.h"
#include "DataLinked.h"
#include "DataEquip.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataLinked.h"
#include "DataPattern.h"
#include "ManagerPattern.h"
#include "MapSystemData.h"
#include "MapLocation.h"
#include "DataLocation.h"
#include "RTableUpdate.h"
#include "DataLocBase.h"
#include "DataLocBuild.h"
#include "DataLocBType.h"
#include "DataLocStair.h"
#include "DataLocFloor.h"
#include "DataLocRoom.h"
#include "ManagerLocation.h"
#include "DataEmBc.h"
#include "ManagerEmergency.h"
#include "../Common/Utils/YAdoDatabase.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;
// CFormLoadRelayTable
IMPLEMENT_DYNCREATE(CFormLoadRelayTable, CFormView)
CTempPtn::~CTempPtn()
{
	CTempPtnItem * pItem;
	while(!m_ptrTempPntItemList.IsEmpty())
	{
		pItem = (CTempPtnItem *)m_ptrTempPntItemList.RemoveHead();
		if(pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
}

void CTempPtn::AddTempDevice(CDataDevice * pRefDev,int nInsertType)
{
	CTempPtnItem * pItem = new CTempPtnItem;
	pItem->m_pRefDev = pRefDev;
	pItem->m_nInsertType = nInsertType;
	m_ptrTempPntItemList.AddTail(pItem);
}

CFormLoadRelayTable::CFormLoadRelayTable()
	: CFormView(IDD_FORMLOADRELAYTABLE)
	, m_strPath(_T(""))
{
	m_pDlgInput = nullptr;
	m_pDlgOutput = nullptr;
	m_pDlgPattern = nullptr;
	m_nCompareType = CMP_NAME;
	m_pNewRelayTable = nullptr;
	m_bStopFlag = FALSE;
	m_pRelayThread = nullptr;
	m_bPreview = FALSE;
	m_bDiffMaking = FALSE;
	m_bNewFileLoaded = FALSE;
	m_nProgJobCount = 0; 
	m_nJobIndex = 0; 
}

CFormLoadRelayTable::~CFormLoadRelayTable()
{
	if (m_pDlgInput != nullptr)
	{
		delete m_pDlgInput;
		m_pDlgInput = nullptr;
	}
	if (m_pDlgOutput != nullptr)
	{
		delete m_pDlgOutput;
		m_pDlgOutput = nullptr;
	}
	if (m_pDlgPattern != nullptr)
	{
		delete m_pDlgPattern;
		m_pDlgPattern = nullptr;
	}

	RemoveAllData();
}

void CFormLoadRelayTable::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_ctrlTab);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgAll);
	DDX_Text(pDX, IDC_ED_TABLE, m_strPath);
	DDX_Control(pDX, IDC_LB_PATH, m_lbPath);
	DDX_Control(pDX, IDC_PROG_STEP, m_ctrlProgDetail);
}

BEGIN_MESSAGE_MAP(CFormLoadRelayTable, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CFormLoadRelayTable::OnTcnSelchangeTab)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, &CFormLoadRelayTable::OnBnClickedBtnPreview)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CFormLoadRelayTable::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_RD_ADDR, &CFormLoadRelayTable::OnBnClickedRdAddr)
	ON_BN_CLICKED(IDC_RD_NAME, &CFormLoadRelayTable::OnBnClickedRdName)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CFormLoadRelayTable::OnBnClickedBtnBrowser)
	ON_BN_CLICKED(IDC_BTN_STOP, &CFormLoadRelayTable::OnBnClickedBtnStop)
	ON_MESSAGE(CSWM_PROGRESS_STEP, OnProgressEvent)
END_MESSAGE_MAP()


// CFormLoadRelayTable �����Դϴ�.

#ifdef _DEBUG
void CFormLoadRelayTable::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormLoadRelayTable::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormLoadRelayTable �޽��� ó�����Դϴ�.


void CFormLoadRelayTable::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_ctrlProgAll.SetRange(0, 100);
	m_ctrlProgDetail.SetRange(0, 100);

	m_ctrlTab.InsertItem(0, L"�Է�");
	m_ctrlTab.InsertItem(1, L"���");
	m_ctrlTab.InsertItem(2, L"����");

	if (m_pDlgInput == nullptr)
		m_pDlgInput = new CDlgInputChange;
	if (m_pDlgOutput == nullptr)
		m_pDlgOutput = new CDlgOutputChange;
	if (m_pDlgPattern == nullptr)
		m_pDlgPattern = new CDlgPatternChange;
	m_pDlgInput->Create(IDD_DLGINPUTCHANGE, &m_ctrlTab);
	m_pDlgOutput->Create(IDD_DLGOUTPUTCHANGE, &m_ctrlTab);
	m_pDlgPattern->Create(IDD_DLGPATTERNCHANGE, &m_ctrlTab);

	m_pDlgInput->ShowWindow(SW_SHOW);
	m_pDlgOutput->ShowWindow(SW_HIDE);
	m_pDlgPattern->ShowWindow(SW_HIDE);

	m_nCompareType = CMP_NAME;
	((CButton*)GetDlgItem(IDC_RD_NAME))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RD_ADDR))->SetCheck(0);

	m_ctrlProgAll.SetPos(0);
	m_ctrlProgDetail.SetPos(0);
}


void CFormLoadRelayTable::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CFormLoadRelayTable::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SetTabPos();
}

void CFormLoadRelayTable::SetTabPos()
{
	if (m_ctrlTab.GetSafeHwnd() == nullptr)
		return;

	CRect rc, rcTab , rcPrg;
	GetClientRect(&rc);

	//20240318 GBM start - ��Ʈ�� �������� ���� ����
	rc.DeflateRect(4, 205, 4, 4);
	//rc.DeflateRect(4, 135, 4, 4);
	//20240318 GBM end

	m_ctrlTab.MoveWindow(&rc);
	rcTab = rc;
	rcTab = CRect(0, 22, rc.Width()-8, rc.Height() -4);
	if (m_pDlgOutput && m_pDlgOutput->GetSafeHwnd())
		m_pDlgOutput->MoveWindow(&rcTab);
	if (m_pDlgPattern && m_pDlgPattern->GetSafeHwnd())
		m_pDlgPattern->MoveWindow(&rcTab);
	if (m_pDlgInput && m_pDlgInput->GetSafeHwnd())
		m_pDlgInput->MoveWindow(&rcTab);
}


void CFormLoadRelayTable::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	int nSel = m_ctrlTab.GetCurSel();
	switch (nSel)
	{
	case 0:
		if (m_pDlgOutput && m_pDlgOutput->GetSafeHwnd())
			m_pDlgOutput->ShowWindow(SW_HIDE);
		if (m_pDlgPattern && m_pDlgPattern->GetSafeHwnd())
			m_pDlgPattern->ShowWindow(SW_HIDE);
		if (m_pDlgInput && m_pDlgInput->GetSafeHwnd())
			m_pDlgInput->ShowWindow(SW_SHOW);
		break;
	case 1:
		if (m_pDlgOutput && m_pDlgOutput->GetSafeHwnd())
			m_pDlgOutput->ShowWindow(SW_SHOW);
		if (m_pDlgPattern && m_pDlgPattern->GetSafeHwnd())
			m_pDlgPattern->ShowWindow(SW_HIDE);
		if (m_pDlgInput && m_pDlgInput->GetSafeHwnd())
			m_pDlgInput->ShowWindow(SW_HIDE);
		break;
	case 2:
		if (m_pDlgPattern && m_pDlgPattern->GetSafeHwnd())
			m_pDlgPattern->ShowWindow(SW_SHOW);
		if (m_pDlgOutput && m_pDlgOutput->GetSafeHwnd())
			m_pDlgOutput->ShowWindow(SW_HIDE);
		if (m_pDlgInput && m_pDlgInput->GetSafeHwnd())
			m_pDlgInput->ShowWindow(SW_HIDE);
		break;
	}
}

void CFormLoadRelayTable::OnBnClickedBtnPreview()
{
	UpdateData();
	if(CheckUpdateCondition() == FALSE)
	{
		AfxMessageBox(L"���� �����߿� ����/�ڵ� ���� ���θ� �������� �ʾҽ��ϴ�.\n����/�ڵ� ���θ� �����ϰ� �ٽ� ������ �ֽʽÿ�");
		return;
	}
	
	m_bDiffMaking = TRUE;

	int i;
	CString str;
	int nCnt = m_lbPath.GetCount();
	m_arrPath.RemoveAll();
	for (i = 0; i < nCnt; i++)
	{
		m_lbPath.GetText(i, str);
		if (str.GetLength() <= 0)
			continue; 
		m_arrPath.Add(str);
	}

	if (m_arrPath.GetCount() <= 0)
	{
		return; 
	}
	m_bPreview = TRUE;

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
// 	m_ctrlList.DeleteAllItems();
// 	m_ctrlTree.DeleteAllItems();
// 	RemoveAllData();
// 	// ��ü �ۼ�Ʈ�� ������ ����
// 	// 
// 	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
// 	if (m_pRefFasSysData == nullptr)
// 		m_pRefFasSysData = theApp.GetRelayTableData();
// 
// 	if (m_pRefFasSysData == nullptr)
// 	{
// 		AfxMessageBox(L"���������� �ڵ������ϴµ� ���� �߽��ϴ�.(�߰���϶�ǥ �������� ����)");
// 		return;
// 	}
// 
// 	m_spRefAutoLogic = m_pRefFasSysData->GetAutoLogicManager();
// 
// 	if (m_spRefAutoLogic == nullptr)
// 	{
// 		AfxMessageBox(L"���������� �ڵ������ϴµ� ���� �߽��ϴ�.(�ڵ����� ���� �������� ����)");
// 		return;
// 	}
// 	m_bStopFlag = FALSE;
// 	if (InitAutoSystemData() < 0)
// 	{
// 		AfxMessageBox(L"���������� �ڵ������ϴµ� ���� �߽��ϴ�.(������ �ʱ�ȭ ����)");
// 		return;
// 	}
// 
 	// Thread Start
	m_bStopFlag = FALSE;
	m_pRelayThread = AfxBeginThread((AFX_THREADPROC)Thread_RelayProc,
 		(LPVOID)this);
}


LRESULT CFormLoadRelayTable::OnProgressEvent(WPARAM wp, LPARAM lp)
{
	CString str;
	int nP = 0 , nAllCnt=0;
	SU_LPARAM suL;
	suL.dwParam = lp;
	switch (wp)
	{
	case PROG_RESULT_CANCEL:
		//GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"����ڰ� ��� �߽��ϴ�.");
		m_ctrlProgAll.SetPos(100);
		break;
	case PROG_RESULT_ERROR:
		//GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"������ ������ �߻� �߽��ϴ�.");
		m_ctrlProgAll.SetPos(100);
		break;
	case PROG_RESULT_STEP:
		nP = (int)(((float)m_nJobIndex / (float)m_nProgJobCount) * 100);
		m_ctrlProgAll.SetPos(nP);

		nP = (int)(((float)suL.stParam.wStep / (float)suL.stParam.wAllCnt) * 100);
		m_ctrlProgDetail.SetPos(nP);
		break;
	case PROG_RESULT_DETAIL_COMPLETE:
		m_nJobIndex++; 
		nP = (int)(((float)m_nJobIndex / (float)m_nProgJobCount) * 100);
		m_ctrlProgAll.SetPos(nP);

		nP = 0;
		m_ctrlProgDetail.SetPos(nP);
		break;
	case PROG_RESULT_FINISH:
		//str.Format(L"%d���� ���������͸� �����߽��ϴ�.", m_nAllCnt);
		//GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		nP = (int)(((float)m_nJobIndex / (float)m_nProgJobCount) * 100);
		m_ctrlProgAll.SetPos(nP);
		m_ctrlProgDetail.SetPos(100);
		break;
	}
	return 1;
}

void CFormLoadRelayTable::OnBnClickedBtnApply()
{
	int nModuleTableCount = 0;
	nModuleTableCount = m_lbPath.GetCount();
	if (nModuleTableCount == 0)
	{
		AfxMessageBox(L"���õ� �߰�� �϶�ǥ�� �����ϴ�.");
		return;
	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(CheckUpdateCondition() == FALSE)
	{
		AfxMessageBox(L"���� �����߿� ����/�ڵ� ���� ���θ� �������� �ʾҽ��ϴ�.\n����/�ڵ� ���θ� �����ϰ� �ٽ� ������ �ֽʽÿ�");
		return;
	}

	theApp.OnHomeProjectSave();
	// [2022/11/15 10:21:28 KHS] 
	// ����� �϶�ǥ�� version_temp\Relaytable�� �����Ѵ�.
	CString strFile,strSrc,strTarget,strtemp,strPath;
	int nSize,i,nFind;
	CRelayTableData * pOldTable;
	CMapSystemData * pMapNew,*pMapOld;

	pOldTable = theApp.GetRelayTableData();
	strtemp = pOldTable->GetProjectVersionPath();
	if(strtemp.GetLength() <= 0)
	{
		AfxMessageBox(L"������Ʈ�� ���丮 ��ġ������ ���� �����ϴµ� ���� �߽��ϴ�.");
		return;
	}

	if(strtemp.Right(1) != '\\')
		strtemp += L"\\";
	strPath.Format(L"%s%s\\" ,strtemp ,F3_PRJ_DIR_RELAYTABLE);

	nSize = m_arrPath.GetSize();
	for(i = 0; i < nSize; i++)
	{
		strSrc = m_arrPath.GetAt(i);
		if(strSrc.GetLength() <= 0)
			continue;
		nFind = strSrc.ReverseFind('\\');
		if(nFind < 0)
			strFile = strSrc;
		else
			strFile = strSrc.Mid(nFind + 1);
		strTarget = strPath + strFile;
		CopyFile(strSrc,strTarget,FALSE);
	}
	
	m_bDiffMaking = FALSE;
	m_bStopFlag = FALSE;
	m_pRelayThread = AfxBeginThread((AFX_THREADPROC)Thread_RelayProc,
		(LPVOID)this);
}


void CFormLoadRelayTable::OnBnClickedRdAddr()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nCompareType = CMP_ADDR; 
}


void CFormLoadRelayTable::OnBnClickedRdName()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nCompareType = CMP_NAME;
}


void CFormLoadRelayTable::OnBnClickedBtnBrowser()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strDefault = L"*.xls", strFilter = L"Excel �߰���϶�ǥ (*.xls,*.xlsm,*.xlsx)|*.xls;*.xlsm;*.xlsx||All Files (*.*)|*.*||";
	CFileDialog FileDialog(TRUE, NULL, strDefault, OFN_HIDEREADONLY| OFN_ALLOWMULTISELECT, strFilter, this);
	CString strFile,strPath;
	TCHAR buffer[4096] = { 0 }; //����
	FileDialog.m_ofn.lpstrFile = buffer; //���� ������
	FileDialog.m_ofn.nMaxFile = 4096; //���� ũ�� 


	if (FileDialog.DoModal() != IDOK)
		return;

	strFile = FileDialog.GetFileName();
	if(strFile.IsEmpty()) // MultiSelection
	{
		POSITION Pos = FileDialog.GetStartPosition();
		while(Pos != NULL)
		{
			strPath = FileDialog.GetNextPathName(Pos);
			m_lbPath.AddString(strPath);
		}
	}
	else // Single Selection
	{
		strPath = FileDialog.GetPathName();
		m_lbPath.AddString(strPath);
	}

	m_bPreview = FALSE;
	m_bNewFileLoaded = FALSE;
	UpdateData(FALSE);
}


void CFormLoadRelayTable::OnBnClickedBtnStop()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bStopFlag = TRUE;
}

DWORD CFormLoadRelayTable::Thread_RelayProc(LPVOID lpData)
{
	CFormLoadRelayTable	*me;
	int nRet = 0;
	me = (CFormLoadRelayTable *)lpData;
	if ((CFormLoadRelayTable *)NULL == me)
	{
		me->SendMessage(CSWM_PROGRESS_STEP, 0, 0);
		return 0;
	}

	try
	{
		if (me->IsDiffMake() == TRUE)
			nRet = me->MakeDiffDataProc();
		else
		{
			nRet = me->ApplyDiffDataProc();
			if (nRet > 0)
			{
				AfxMessageBox(L"���ο� �߰�� �϶�ǥ�� ����Ͽ� ���������͸� ������Ʈ �ϴµ� �����߽��ϴ�.\n"
				L"���α׷��� ���� �� �ٽ� ������ �ֽʽÿ�");
			}
			else
			{
				AfxMessageBox(L"���ο� �߰�� �϶�ǥ�� ����Ͽ� ���������͸� ������Ʈ �ϴµ� �����߽��ϴ�.\n");
			}
		}

		
	}
	catch (...)
	{
	}
	// 	if(nRet == 1)
	// 		me->SendMessage(CSWM_PROGRESS_STEP, 100, PROG_RESULT_FINISH);
	// 	else
	// 		me->SendMessage(CSWM_PROGRESS_STEP, 100, PROG_RESULT_ERROR);
	/*
	*  Thread�� ���� �Ͽ����� ����.
	*/
	return 0;
}


int CFormLoadRelayTable::MakeDiffDataProc()
{
	// 1. Excel Load
	// 2. ���� , 
	// 3. 
	// 	LoadNewExcel();
	// 	CompareData();
	// 	DisplayItem();

	int nRet = -1;

	CRelayTableData * pOldTable , *pNewTable;
	CMapSystemData * pMapNew, *pMapOld;

	pOldTable = theApp.GetRelayTableData();
	if (pOldTable == nullptr)
		return -1;
	// 1. ȸ�κ� 
	// 2.PATTERN ID ����
	// 3. �ߺ��Ǵ� ȸ�θ� ����ϰų� �ߺ�ȸ�� ������ �����Է� ����Ʈ ����
	// 4.�ߺ��Ǵ� ȸ�θ� ����ϴ� �����Է� ����Ʈ ����
	// 5.�ߺ��Ǵ� ȸ���� �����Է� ����Ʈ
	// 6. ���� ���� ����Ʈ - 
	// 7. Display - input
	// 8. Display - output
	// 9. Display - pattern

	m_nProgJobCount = m_lbPath.GetCount() + 9; // 
	m_nJobIndex = 0;

	if (m_bNewFileLoaded == FALSE)
	{
		pNewTable = LoadNewRelayTable();
		if (pNewTable == 0)
		{
			AfxMessageBox(L"Excel ������ �о���µ� ���� �߽��ϴ�.");
			return -1;
		}
	}
	else
	{
		pNewTable = m_pNewRelayTable;
		m_nJobIndex = m_lbPath.GetCount();
	}
	RemoveCompareResult();
	if (m_nCompareType == CMP_NAME) // �̸��� ���� �Ҷ� �ߺ����� ǥ��
	{
		CompareByName(pOldTable, pNewTable
			,m_vtIntersection , m_vtOnlyOld , m_vtOnlyNew
		);
		pMapOld = &m_mapTempOldName;
		pMapNew = &m_mapTempNewName;
	}
	else
	{
		CompareByAddress(pOldTable, pNewTable
			, m_vtIntersection, m_vtOnlyOld, m_vtOnlyNew
		);

		pMapOld = pOldTable->GetSystemData();
		pMapNew = pNewTable->GetSystemData();
	}
	
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);

	//////////////////////////////////////////////////////////////////////////
	// 1. �� �߰�� �϶�ǥ�� �߰��Ǵ� ���ű� ���� ä�� ȸ���� ID �߰�
	// 2. �� �߰�� �϶�ǥ ����  Pattern�� ����� ID �Է�
	//ChangeNewLoadSystemMapID(pOldTable, pNewTable);


	//2, PATTERN ID ����
	ChangePatternID(&m_ptrTempNewPatternData);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);


	// [KHS 2020-8-14 08:07:04] 
	// 1. �������� �߰��� ���� ����
	// 2. �ڵ� ���� �ٽ� ����
	// 3. Display ����� - �Է��� ����Ǽ� ��µ� ����Ǵ� ���(�ߺ� �϶� �ּ� �����ϸ� ����ϴ� 
	//					- ����� ����� ����ϴ� ���
	// 4. üũ�ڽ� ���� --> �Է¿��� üũ ������ , ����ϴ� ��� ����� üũ ����
	//                     ��¿��� üũ ������ , ��¸� ����
	// 5. �ڵ� ���� �� �������� �߰��Ѱ� �Է�
	
	// [KHS 2020-8-14 08:09:43] 
	// 1. �������� �߰��� ���� ����
	//  1.1. �������� �߰��Ȱ� ��������
	//	1.2. �ߺ��� �׸��� ����ϴ� �Է�/����� �ִ��� Ȯ�� �� ��ȯ
	//	1.1. New Relay Table�� ����

	//  1.1. �������� �߰��� ���������� �������� - �ߺ��Ȱ� ���� �Ϸ�Ǹ� �ٽ� �Է�
	//		LOGIC TYPE ��  LOGIC_MANUAL�� ���
	//		�������� �߰��� �׸� ���� , �ߺ��϶� �̵�
	//	1.2. �ߺ��� �׸��� ����ϴ� �Է�/����� �ִ��� Ȯ�� �� ��ȯ
	//CPtrList ptrlist , ptrDulicate;

	// �ߺ��Ǵ� ȸ�θ� ����ϰų� �ߺ�ȸ�� ������ �����Է� ����Ʈ ����
	GetManualLinkData(&m_ptrManualLink, pOldTable);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
#ifdef _DEBUG
	CDataDevice * pDevTemp;
	CString strTemp;
	strTemp = GF_GetSysDataKey(SE_RELAY, 0, 0, 0, 95);
	pDevTemp = m_pNewRelayTable->GetDevice(strTemp);
#endif
	// �ߺ��Ǵ� ȸ�θ� ����ϴ� �����Է� ����Ʈ ����
	MakeDuplicationManualOutputLink(pOldTable, m_vtIntersection, m_ptrUSINGDupList);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
#ifdef _DEBUG
// 	CDataDevice * pDevTemp;
// 	CString strTemp;
	strTemp = GF_GetSysDataKey(SE_RELAY, 0, 0, 0, 95);
	pDevTemp = m_pNewRelayTable->GetDevice(strTemp);
#endif
	// �ߺ��Ǵ� ȸ���� �����Է� ����Ʈ
	MakeDuplicationManualInputLink(pOldTable, m_vtIntersection, m_ptrUSEDDupList);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
#ifdef _DEBUG
// 	CDataDevice * pDevTemp;
// 	CString strTemp;
	strTemp = GF_GetSysDataKey(SE_RELAY, 0, 0, 0, 95);
	pDevTemp = m_pNewRelayTable->GetDevice(strTemp);
#endif
	// 1.5���� �������� PATTERN ITEM DB�� �ִµ� RELAY TABLE�� �ִ°� �״�� ���� - ���������� ���� �� ���� �߻�[5/16/2022 KHS]
	// 1.5������ DB�� ������ RELAY TABLE�� ���°� �ٷ� ���� ��Ŵ
	// ���� ���� ����Ʈ - 
	MakePtnChangeData(pNewTable, pOldTable, m_ptrPatternUsedDupList);
#ifdef _DEBUG
// 	CDataDevice * pDevTemp;
// 	CString strTemp;
	strTemp = GF_GetSysDataKey(SE_RELAY, 0, 0, 0, 95);
	pDevTemp = m_pNewRelayTable->GetDevice(strTemp);
#endif
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
	if (m_pDlgInput)
		m_pDlgInput->DisplayCompareResult(m_nCompareType
			, pMapOld, pMapNew
			, &m_vtIntersection, &m_vtOnlyOld, &m_vtOnlyNew
			, &m_ptrUSEDDupList
			, this
		);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);

	if (m_pDlgOutput)
	 	m_pDlgOutput->DisplayCompareResult(m_nCompareType
	 		, pMapOld, pMapNew
	 		, &m_ptrUSINGDupList
			, this
		);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);

	if (m_pDlgPattern)
		m_pDlgPattern->DisplayCompareResult(m_nCompareType
			, pMapOld, pMapNew
			, &m_ptrPatternUsedDupList
			, this
		);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_FINISH, 0, 0);
	AfxMessageBox(L"�̸����⸦ �Ϸ��߽��ϴ�.");

#ifdef _DEBUG
// 	CDataDevice * pDevTemp;
// 	CString strTemp;
	strTemp = GF_GetSysDataKey(SE_RELAY, 0, 0, 0, 95);
	pDevTemp = m_pNewRelayTable->GetDevice(strTemp);
#endif
	
	return 1;
}

int CFormLoadRelayTable::ApplyDiffDataProc()
{
	int nRet = -1;

	CRelayTableData * pOldTable, *pNewTable;
	pOldTable = theApp.GetRelayTableData();
	if (pOldTable == nullptr)
		return -1;
	// 1. ȸ�κ� 
	// 2.PATTERN ID ����
	// 3.�ߺ��Ǵ� ȸ�θ� ����ϴ� �����Է� ����Ʈ ����
	// 4.�ߺ��Ǵ� ȸ���� �����Է� ����Ʈ
	// 5. ���� ���� ����Ʈ - 
	// 6. �� �϶�ǥ�� ���������� ����
	// 7. �� �϶�ǥ ������ �Է�
	// 8. 
	// 9.
	// 10.
	// 11.
	// 12. ����� �Է�
	m_nProgJobCount = m_lbPath.GetCount() + 12;
	if (m_bNewFileLoaded == FALSE)
	{
		pNewTable = LoadNewRelayTable();
		if (pNewTable == 0)
		{
			AfxMessageBox(L"Excel ������ �о���µ� ���� �߽��ϴ�.");
			m_pNewRelayTable->SendProgStep(this, PROG_RESULT_ERROR, 0, 0);
			return -1;
		}
	}
	else
	{
		pNewTable = m_pNewRelayTable;
		m_nJobIndex = m_lbPath.GetCount();
	}

	if (m_bPreview == FALSE)
	{
		if (m_nCompareType == CMP_NAME) // �̸��� ���� �Ҷ� �ߺ����� ǥ��
		{
			CompareByName(pOldTable, pNewTable
				, m_vtIntersection, m_vtOnlyOld, m_vtOnlyNew
			);
		}
		else
		{
			CompareByAddress(pOldTable, pNewTable
				, m_vtIntersection, m_vtOnlyOld, m_vtOnlyNew
			);
		}
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);

		// �ߺ��Ǵ� ȸ�θ� ������ �����Է� ����Ʈ ����
		GetManualLinkData(&m_ptrManualLink, pOldTable);
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
		// �ߺ��Ǵ� ȸ�θ� ����ϴ� �����Է� ����Ʈ ����
		MakeDuplicationManualOutputLink(pOldTable, m_vtIntersection, m_ptrUSINGDupList);
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
		MakeDuplicationManualInputLink(pOldTable, m_vtIntersection, m_ptrUSEDDupList);
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
		MakePtnChangeData(pNewTable, pOldTable, m_ptrPatternUsedDupList);
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
	}
	else
	{
		m_nJobIndex += 5;
	}

	// �������� ���������� ��ü ���� - �޸� �󿡼�
	// 
	//1. �������� ���� �� �߰�  �۾�
	// 2. New Device�� ID���� 
	// 3. �ߺ� ������ ���ο� ���̵� ���� - �ߺ������� �̸��� �ߺ����� ������ ���� ���� , �ּ� �϶��� ����
	// 3.1. �̸����� ������ ���̵� ���λ���
	// 3.1.1. ���� ȸ���� ���� �Է� ������ DB�Է� , �޸� ����
	// 3.1.2. �޸� ����
	// 3.2. �ּҷ� ������ ���� ���� ����
	// 
	// 4. �ߺ� 
	//2. �ߺ����� ���� ��� �۾� -> New Deivce�� ID����
	//3. �Է¿��� �ּ� ���� , 
	YAdoDatabase * pDB = pOldTable->GetPrjDB();
	pDB->BeginTransaction();

	//  [9/20/2022 KHS] Index ����
	m_pNewRelayTable->SetPrjDB(pDB);
	if(m_pNewRelayTable->TempFunc_DropIndex() <= 0)
	{
		pDB->RollbackTransaction();
		m_pNewRelayTable->SendProgStep(this,PROG_RESULT_ERROR,0,0);
		return 0;
	}

	nRet = RemoveAllLinkData(pDB,m_pNewRelayTable); ///< �� �϶�ǥ���� ���������� ����
 	if (nRet <= 0)
 	{
 		pDB->RollbackTransaction();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_ERROR, 0, 0);
		return 0;
 	}
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);

#ifdef _DEBUG
	CDataDevice * pDevTemp;
	CString strTemp;
	strTemp = GF_GetSysDataKey(SE_RELAY, 0, 2, 2, 28);
	pDevTemp = m_pNewRelayTable->GetDevice(strTemp);
#endif
	// 	// �߰��� ���űⰡ ������ �߰�
// 	nRet = AddFacpUnitChannel(pNewTable, pOldTable);
// 	if (nRet <= 0)
// 	{
// 		pDB->RollbackTransaction();
// 		return 0;
// 	}
// 
// 	nRet = ChangeDuplicationItem(pDB, m_pNewRelayTable);///< M_VTINS�� new device�� �� ���̵� ���� , New Relay Table �����ͺ��̽��� �Է�
// 	if (nRet <= 0)
// 	{
// 		pDB->RollbackTransaction();
// 		return 0;
// 	}
// 	
// 	nRet = DeleteOldItemAndAddNewItem(pDB , m_pNewRelayTable); ///< �� �϶�ǥ
// 	if (nRet <= 0)
// 	{
// 		pDB->RollbackTransaction();
// 		return 0;
// 	}
// 	

	nRet = InsertNewData(pDB, m_pNewRelayTable);
	if (nRet <= 0)
	{
		USERLOG(L"DB ����(InsertNewData) : %s",pDB->GetLastErrorString());
		pDB->RollbackTransaction();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_ERROR, 0, 0);
		return 0;
	}
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
	// ����� ���� ó��


	//AutoMakeLink();// �ڵ� ���������� ����

	// �ߺ��Ǵ� �������� ����ϴ� �Է� - �ߺ� �Ǻ��� aDDRESS�̸� ���� , �̸��̸� �ּҺ���
	// �ߺ��Ǵ� �������� �Է��� ��� - �ߺ��Ǻ��� ADDRESS�̸� ���� , �̸��̸� �ּҺ���
	//DuplicationItemAddManualLink(); // �ߺ� �׸��� ���� �Է� �κ�

	//////////////////////////////////////////////////////////////////////////
	// 1. �ߺ��Ǵ� ȸ�θ� ����ϴ� ���
	// 1.1. �̸��� ��ġ�Ҷ��� ���������� ID����
	// 1.2. �ּ��϶� ����
	// 2. �ߺ��Ǵ� ȸ�ΰ� ����ϴ� ����������
	// 2.1. �̸��� ��ġ�Ҷ��� ������ ���������� ID����
	// 2.2. �ּ��϶� ����
	nRet = InputUsingDuplicateItems(pDB, m_pNewRelayTable, pOldTable);
	if (nRet <= 0)
	{
		USERLOG(L"DB ����(InputUsingDuplicateItems) : %s",pDB->GetLastErrorString());

		pDB->RollbackTransaction();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_ERROR, 0, 0);
		return 0;
	}
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
	nRet = OutputUsedByDuplicateItems(pDB, m_pNewRelayTable, pOldTable);
	if (nRet <= 0)
	{

		USERLOG(L"DB ����(OutputUsedByDuplicateItems) : %s",pDB->GetLastErrorString());
		pDB->RollbackTransaction();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_ERROR, 0, 0);
		return 0;
	}
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);
	nRet = NormalItemAddManualLink(pDB, m_pNewRelayTable); // �Ϲ� �߿� ���� �Է� �κ�
	if (nRet <= 0)
	{
		USERLOG(L"DB ����(NormalItemAddManualLink) : %s",pDB->GetLastErrorString());
		pDB->RollbackTransaction();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_ERROR, 0, 0);
		return 0;
	}
	nRet = InsertLocationData(pDB, m_pNewRelayTable); // �Ϲ� �߿� ���� �Է� �κ�
	if (nRet <= 0)
	{
		USERLOG(L"DB ����(InsertLocationData) : %s",pDB->GetLastErrorString());
		pDB->RollbackTransaction();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_ERROR, 0, 0);
		return 0;
	}
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);

	nRet = ChangePatternDB(pDB, m_pNewRelayTable ,&m_ptrPatternUsedDupList,pOldTable->GetCurrentUser());
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_DETAIL_COMPLETE, 0, 0);

	InsertNewEmBroadcast(pDB,m_pNewRelayTable);
	m_pNewRelayTable->SendProgStep(this,PROG_RESULT_DETAIL_COMPLETE,0,0);
	// �� ���� �� ����̽� ������ ���°� ����- TB)PATTERN_ITEM���� �ִ°� ����
    DelDeprecatedPatternItemDB(pDB);

	// Index ���� [9/20/2022 KHS]
	if(m_pNewRelayTable->TempFunc_CheckIndex() <= 0)
	{
		pDB->RollbackTransaction();
		m_pNewRelayTable->SendProgStep(this,PROG_RESULT_ERROR,0,0);
		return 0;
	}
	pDB->CommitTransaction();


	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_FINISH, 0, 0);
	AfxMessageBox(L"���ο� �϶�ǥ ������ �Ϸ� �߽��ϴ�.");
	return 1;
}

int CFormLoadRelayTable::InitNewPtnDevList(CRelayTableData * pNewTable)
{
	std::shared_ptr<CManagerPattern>	spUserAddPattern;
	POSITION pos , posItem;
	CDataPattern * pPtn;
	CTempPtn * pTmpPtn;
	CPtrList * pDevList;
	CDataLinked * pLnk;
	CDataDevice * pData;
	CDataSystem * pSys;
	CString strKey;
	spUserAddPattern = pNewTable->GetPatternManager();
	pos = spUserAddPattern->GetHeadPosition();
	while (pos)
	{
		pPtn = (CDataPattern *)spUserAddPattern->GetNext(pos);
		if (pPtn == nullptr)
			continue; 
		pDevList = pPtn->GetPtnItemList();
		if (pDevList == nullptr)
			continue; 
		pTmpPtn = new CTempPtn;
		pTmpPtn->SetPattern(pPtn);
		m_ptrTempNewPatternData.AddTail(pTmpPtn);
		posItem = pDevList->GetHeadPosition();
		while (posItem)
		{
			pLnk = (CDataLinked *)pDevList->GetNext(posItem);
			if (pLnk == nullptr)
				continue; 
			
			pSys = pNewTable->GetIDSystemItem(SE_RELAY
				, pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
				, pLnk->GetTgtChn(), pLnk->GetTgtDev()
			);

			if (pSys == nullptr || pSys->GetSysData() == nullptr)
				continue; 
			pData = (CDataDevice*)pSys->GetSysData();
			pTmpPtn->AddTempDevice(pData,pLnk->GetLogicType());
		}
	}
	return 1;
}


int CFormLoadRelayTable::ChangePatternID(CPtrList * pPtnList)
{
	POSITION pos, posItem;
	CDataPattern * pPtn;
	CTempPtn * pTmpPtn;
	CPtrList * pDevList;
	CTempPtnItem * pData;
	CString strKey;
	int nCnt = pPtnList->GetCount();
	int nIdx = 0; 
	pos = pPtnList->GetHeadPosition();
	// ������ ���� �������� ���� �ϰ� ���ο� ���������� ����
	// [KHS 2020-11-10 11:43:14] 
	// ���� : ���� ���� �������� �����ϰ� ������׸� �����Ѵ�.(�߰� �Ǵ� ����� ������(�ߺ�������)�� �ּҸ� �����Ͽ� �����Ѵ�.
	while (pos)
	{
		pTmpPtn = (CTempPtn *)pPtnList->GetNext(pos);
		if (pTmpPtn == nullptr || pTmpPtn->m_pRefPtn == nullptr)
			continue;
		pDevList = &(pTmpPtn->m_ptrTempPntItemList);
		pPtn = pTmpPtn->m_pRefPtn;
		// 
		pPtn->RemoveAllLink();
		posItem = pDevList->GetHeadPosition();
		while (posItem)
		{
			pData = (CTempPtnItem *)pDevList->GetNext(pos);
			if (pData == nullptr)
				continue;
			pPtn->AddDeviceToPattern(pData->m_pRefDev,pData->m_nInsertType);
		}
		nIdx++;
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, nCnt, nIdx);
	}
	return 1;
}


int CFormLoadRelayTable::ChangePatternDB(YAdoDatabase * pDb, CRelayTableData * pNewTable
	, CPtrList * pPatternList, CString strCurrentUser)
{
	CRPatternUpdate * pUpdate;
	POSITION pos;
	//CDataPattern * pPtn;
	CString strKey,strSql=L"" , strCmd , strWhere,strtemp=L"";
	int i, nCnt , nID,nUCnt=0;
	CRUpdateItem * pItem;

	pos = pPatternList->GetHeadPosition();
	while (pos)
	{
		pUpdate = (CRPatternUpdate *)pPatternList->GetNext(pos);
		if (pUpdate == nullptr)
			continue;
		
		// ���� �϶�
		if(pUpdate->m_nChnageType == 0)
		{
			// ������ Pattern ID
			if(pUpdate->m_nOldPID <= 0)
				continue; 
			strtemp.Format(L"DELETE FROM TB_PATTERN_ITEM "
				L" WHERE PT_ID=%d ;\n"
				,nID
			);
			strSql += strtemp;
			nUCnt ++;
			if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				pDb->ExecuteSql(strSql);
				nUCnt = 0;
				strSql = L"";
			}
			strtemp.Format(L"DELETE FROM TB_PATTERN_MST "
				L" WHERE PT_ID=%d ;\n"
				,pUpdate->m_nOldPID
			);
			strSql += strtemp;
			nUCnt ++;
			if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				pDb->ExecuteSql(strSql);
				nUCnt = 0;
				strSql = L"";
			}
		}
		else// ���� �� �߰� �϶� 
		{
			if(pUpdate->m_nOldPID <= 0 && pUpdate->m_nNewPID <= 0)
				continue;

			nID = 0;
			// ���� ���� ��ȣ ����
			if(pUpdate->m_nOldPID > 0)
				nID = pUpdate->m_nOldPID;
			else
				nID = pUpdate->m_nNewPID;

			if(nID <= 0)
				continue;
			//strSql.Format(L"SELECT * FROM TB_PATTERN_MST WHERE PT_ID=%d ", nID);

			strtemp.Format(L"INSERT INTO TB_PATTERN_MST(NET_ID,PT_TYPE , PT_ID , PT_NAME,MANUAL_MAKE , ADD_USER) "
				L" VALUES(1,%d,%d,'%s',%d,'%s') ;\n"
				,pUpdate->m_sNewType,pUpdate->m_nNewPID,pUpdate->m_strNewPtnName
				,0,strCurrentUser
			);

			strSql += strtemp;
			nUCnt ++;
			if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				pDb->ExecuteSql(strSql);
				nUCnt = 0;
				strSql = L"";
			}

			// ���� ���� ���� - ������(old,new�� ������)
			nCnt = pUpdate->m_vtIns.size();
			for(i = 0; i < nCnt; i++)
			{
				pItem = pUpdate->m_vtIns[i];
				if(pItem == nullptr)
					continue;
				if(pItem->m_pNewCopyDev == nullptr || pItem->m_pOldCopyDev == nullptr)
					continue;
				strCmd.Format(L"UPDATE TB_PATTERN_ITEM SET FACP_ID=%d , UNIT_ID=%d , CHN_ID=%d , RLY_ID=%d , INSERT_TYPE=%d "
					,pItem->m_pNewCopyDev->GetFacpID(),pItem->m_pNewCopyDev->GetUnitID()
					,pItem->m_pNewCopyDev->GetChnID(),pItem->m_pNewCopyDev->GetDeviceID()
					,LOGIC_ALL_AUTO
				);
				strWhere.Format(L" WHERE PT_ID=%d AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d  ;\n"
					,nID
					,pItem->m_pOldCopyDev->GetFacpID(),pItem->m_pOldCopyDev->GetUnitID()
					,pItem->m_pOldCopyDev->GetChnID(),pItem->m_pOldCopyDev->GetDeviceID()
				);

				strtemp = strCmd + strWhere;
				strSql += strtemp;
				nUCnt ++;
				if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
				{
					if(pDb->ExecuteSql(strSql) == FALSE)
					{
						//pChgLink->btChangeType = CHANGE_ACTION_FAILED;
						//return 0;
						USERLOG(L"Update Error : %s\n",strSql);
						break;
					}
					nUCnt = 0;
					strSql = L"";
				}

			}

			// ���ο� ���� �߰� 
			nCnt = pUpdate->m_vtAdd.size();
			for(i = 0; i < nCnt; i++)
			{
				pItem = pUpdate->m_vtAdd[i];
				if(pItem == nullptr)
					continue;
				if(pItem->m_pNewCopyDev == nullptr)
					continue;

				strtemp.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,ITEM_TYPE,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,INSERT_TYPE,ADD_USER) "
					L" VALUES(1,%d,%d,%d,%d,%d,%d,%d,'%s') ;\n"
					,nID,LK_TYPE_RELEAY
					,pItem->m_pNewCopyDev->GetFacpID(),pItem->m_pNewCopyDev->GetUnitID()
					,pItem->m_pNewCopyDev->GetChnID(),pItem->m_pNewCopyDev->GetDeviceID()
					,LOGIC_ALL_AUTO,strCurrentUser
				);
				strSql += strtemp;
				nUCnt ++;
				if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
				{
					if(pDb->ExecuteSql(strSql) == FALSE)
					{
						//pChgLink->btChangeType = CHANGE_ACTION_FAILED;
						//return 0;
						USERLOG(L"INSERT Error : %s\n",strSql);
						break;
					}
					nUCnt = 0;
					strSql = L"";
				}

			}
#if _DELETE_OLD_PATTERN_ == 1
			nCnt = pUpdate->m_vtDel.size();
			for(i = 0; i < nCnt; i++)
			{
				pItem = pUpdate->m_vtDel[i];
				if(pItem == nullptr)
					continue;
				if(pItem->m_pOldCopyDev == nullptr)
					continue;
				strtemp.Format(L"DELETE FROM TB_PATTERN_ITEM "
					L" WHERE PT_ID=%d AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d ;\n"
					,nID
					,pItem->m_pOldCopyDev->GetFacpID(),pItem->m_pOldCopyDev->GetUnitID()
					,pItem->m_pOldCopyDev->GetChnID(),pItem->m_pOldCopyDev->GetDeviceID()
				);

				if(pDb->ExecuteSql(strtemp) == FALSE)
				{
					//pChgLink->btChangeType = CHANGE_ACTION_FAILED;
					//return 0;
					break;
				}
			}
#endif
		}
		

		
	}

	if(nUCnt > 0)
	{
		if(pDb->ExecuteSql(strSql) == FALSE)
		{
		}
	}



	return 1;
}

// ���̻� ������� �ʴ� ���� ������ ���� - ���Ͼ������� �ִµ� ȸ�� ������ ���°��
int CFormLoadRelayTable::DelDeprecatedPatternItemDB(YAdoDatabase * pDB)
{
	CString strSql;
	struct ST_RM_PTN
	{
		int nn;
		int nf;
		int nu;
		int nc;
		int nr;
	};
	ST_RM_PTN * pData;
	CPtrList ptrDelList;
	int nn,nf,nu,nc,nr,i,nCnt,nValue;
	strSql.Format(
		L" SELECT B.NET_ID , B.FACP_ID ,B.UNIT_ID,B.CHN_ID,B.RLY_ID FROM TB_RELAY_LIST A "
		L" RIGHT JOIN TB_PATTERN_ITEM B "
		L" ON A.FACP_ID = B.FACP_ID AND A.UNIT_ID = B.UNIT_ID AND A.CHN_ID = B.CHN_ID AND A.RLY_ID = B.RLY_ID "
		L" WHERE A.FACP_ID IS NULL");

	if(pDB->OpenQuery(strSql) == FALSE)
		return 0; 

	nCnt = pDB->GetRecordCount();
	for(i = 0; i < nCnt; i++)
	{
		if(pDB->GetFieldValue(L"NET_ID",nValue) == FALSE)
		{
			pDB->MoveNext();
			continue;
		}
		nn = nValue;
		if(pDB->GetFieldValue(L"FACP_ID",nValue) == FALSE)
		{
			pDB->MoveNext();
			continue;
		}
		nf = nValue;
		if(pDB->GetFieldValue(L"UNIT_ID",nValue) == FALSE)
		{
			pDB->MoveNext();
			continue;
		}
		nu = nValue;
		if(pDB->GetFieldValue(L"CHN_ID",nValue) == FALSE)
		{
			pDB->MoveNext();
			continue;
		}
		nc = nValue;
		if(pDB->GetFieldValue(L"RLY_ID",nValue) == FALSE)
		{
			pDB->MoveNext();
			continue;
		}
		nr = nValue;

		if(nr <=0 || nn <= 0 || nf <= 0 ||nu <= 0 || nc <= 0 ||nr <=0)
		{
			pDB->MoveNext();
			continue;
		}

		pData = new ST_RM_PTN;
		pData->nn = nn;
		pData->nf = nf;
		pData->nu = nu;
		pData->nc = nc;
		pData->nr = nr;
		ptrDelList.AddTail(pData);
		pDB->MoveNext();
	}
	pDB->RSClose();


	nCnt = ptrDelList.GetCount();
	while(!ptrDelList.IsEmpty())
	{
		pData = (ST_RM_PTN*)ptrDelList.RemoveHead();
		if(pData == nullptr)
			continue; 

		strSql.Format(L"DELETE FROM TB_PATTERN_ITEM "
			L" WHERE NET_ID=%d AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
			,pData->nn,pData->nf,pData->nu,pData->nc,pData->nr
		);
		if(pDB->ExecuteSql(strSql) == FALSE)
		{
			
			continue;
		}
	}
	return 1;
}
// 
// int CFormLoadRelayTable::ChangeNewLoadSystemMapID(
// 	CRelayTableData * pOldTable , CRelayTableData * pNewTable
// 	, VT_RUPDATEITEM &vtIns, VT_RUPDATEITEM &vtNew
// )
// {
// 	int nRet = -1;
// 	CMapSystemData mapName;
// 	CMapSystemData *pOldMap , *pNewMap;
// 	CMapSystemData::iterator it;
// 	CDataSystem * pOldSys ,*pNewSys;
// 	CDataDevice * pNewDev, *pOldDev;
// 	CDataChannel * pNewChn, *pOldChn;
// 	CDataUnit * pNewUnit, *pOldUnit;
// 	CDataFacp * pNewFacp, *pOldFacp;
// 	CString strKey , strIDKey;
// 	int nTemp=0;
// 	//1. Facp Map �ʱ�ȭ
// 	//2. Unit 
// 	// 3. ID Map 
// 	pNewMap = pNewTable->GetSystemData();
// 	if (pNewMap == nullptr)
// 		return 0; 
// 	/// Map Ű�� sort - facp,unit,... ������ ���ĵ�
// 	for (it = pNewMap->begin(); it != pNewMap->end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pNewSys = it->second;
// 		if (pNewSys == nullptr || pNewSys->GetSysData() == nullptr)
// 			continue;
// 		// ID Map ���� ����
// 		// Facp map
// 		// Unit map
// 		switch (pNewSys->GetDataType())
// 		{
// 		case SE_RELAY:
// 			pNewDev = (CDataDevice *)pNewSys->GetSysData();
// 			strKey = pNewSys->GetKey();
// 			pOldSys = pOldTable->GetSystemItem(strKey);
// 			if (pOldSys == nullptr || pOldSys->GetSysData())
// 			{
// 				// ���� �߰�� �϶�ǥ�� �����Ͱ� ����
// 				// ���� �߰� - ID �ٽ� ����
// 			}
// 			else
// 			{
// 				pOldDev = (CDataDevice*)pOldSys->GetSysData();
// 				pNewDev->SetFacpID(pOldDev->GetFacpID());
// 				pNewDev->SetUnitID(pOldDev->GetUnitID());
// 				pNewDev->SetChnID(pOldDev->GetChnID());
// 				pNewDev->SetDeviceID(pOldDev->GetDeviceID());
// 				strIDKey = GF_GetIDSysDataKey(SE_RELAY
// 					, pOldChn->GetFacpID(), pOldChn->GetUnitID()
// 					, pOldChn->GetChnID(), pOldDev->GetDeviceID());
// 				pNewTable->SetIDSystem(strIDKey, pNewSys);
// 			}
// 			
// 			break;
// 		case SE_CHANNEL:
// 			pNewChn = (CDataChannel *)pNewSys->GetSysData();
// 			strKey = pNewSys->GetKey();
// 			pOldSys = pOldTable->GetSystemItem(strKey);
// 			if (pOldSys == nullptr || pOldSys->GetSysData())
// 			{
// 
// 			}
// 			else
// 			{
// 				pOldChn = (CDataChannel*)pOldSys->GetSysData();
// 				pNewChn->SetFacpID(pOldChn->GetFacpID());
// 				pNewChn->SetUnitID(pOldChn->GetUnitID());
// 				pNewChn->SetChnID(pOldChn->GetChnID());
// 
// 				pNewChn->SetFacpNum(pOldChn->GetFacpNum());
// 				pNewChn->SetUnitNum(pOldChn->GetUnitNum());
// 				pNewChn->SetChnNum(pOldChn->GetChnNum());
// 
// 				strIDKey = GF_GetIDSysDataKey(SE_CHANNEL
// 					, pOldChn->GetFacpID(), pOldChn->GetUnitID(), pOldChn->GetChnID());
// 				pNewTable->SetIDSystem(strIDKey, pNewSys);
// 			}
// 			
// 			break;
// 		case SE_UNIT:
// 			pNewUnit = (CDataUnit *)pNewSys->GetSysData();
// 			
// 			nTemp = pNewTable->CvtFacpIDToNum(pNewUnit->GetFacpID());
// 			pOldUnit = pOldTable->GetUnitByNum(nTemp, pNewUnit->GetUnitNum());
// 			if (pOldUnit == nullptr)
// 			{
// 
// 			}
// 			else
// 			{
// 				// Facp Map ����
// 				// ID System Map ����
// 				strKey.Format(L"%02d.%02d", nTemp, pNewUnit->GetUnitNum());
// 				pNewTable->AddUnitMap(strKey, pNewUnit);
// 				strIDKey = GF_GetIDSysDataKey(SE_UNIT, pOldUnit->GetFacpID(), pOldUnit->GetUnitID());
// 				pNewTable->SetIDSystem(strIDKey, pNewSys);
// 			}
// 			
// 			break;
// 		case SE_FACP:
// 			pNewFacp = (CDataFacp *)pNewSys->GetSysData();
// 			pOldFacp = pOldTable->GetFacpByNum(pNewFacp->GetFacpNum());
// 			if (pOldFacp == nullptr)
// 			{
// 				
// 			}
// 			else
// 			{
// 				// Facp Map ����
// 				// ID System Map ����
// 				pNewTable->AddFacpMapData(pOldFacp->GetFacpID(), pNewFacp);
// 				strIDKey = GF_GetIDSysDataKey(SE_FACP, pOldFacp->GetFacpID());
// 				pNewTable->SetIDSystem(strIDKey, pNewSys);
// 			}
// 			
// 			break;
// 		default :
// 			continue;
// 		}
// 	}
// 	return 1;
// }

// 
// int CFormLoadRelayTable::ChangeNewLoadPatternID(
// 	CRelayTableData * pOldTable, CRelayTableData * pNewTable)
// {
// 	int nRet = -1;
// 	CMapSystemData mapName;
// 	CMapSystemData *pOldMap;
// 	CMapSystemData::iterator it;
// 	CDataSystem * pData, *pFind;
// 	CDataDevice * pDev;
// 	CString strKey;
// 	int i = 0;
// 	pOldMap = pTable->GetSystemData();
// 	for (it = pOldMap->begin(); it != pOldMap->end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr || pData->GetSysData() == nullptr)
// 			continue;
// 
// 		if (pData->GetDataType() != SE_RELAY)
// 			continue;
// 
// 		pDev = (CDataDevice *)pData->GetSysData();
// 		for (i = 0; i < 100; i++)
// 		{
// 			strKey = pDev->MakeCompareName(i);
// 			pFind = (*pMapName)[strKey];
// 			if (pFind == nullptr)
// 				break;
// 		}
// 		(*pMapName)[strKey] = pData;
// 	}
// 	return 1;
// }

CRelayTableData *  CFormLoadRelayTable::LoadNewRelayTable()
{
	int i = 0,nSize;
	BOOL bSuc = FALSE;
	CString str;
	int nCnt = m_lbPath.GetCount();
	m_arrPath.RemoveAll();
	for(i = 0; i < nCnt; i++)
	{
		m_lbPath.GetText(i,str);
		if(str.GetLength() <= 0)
			continue;
		m_arrPath.Add(str);
	}

	if(m_arrPath.GetCount() <= 0)
	{
		AfxMessageBox(L"���õ� �϶�ǥ�� �����ϴ�.");
		return nullptr;
	}
	
	if (m_pNewRelayTable != nullptr)
	{
		delete m_pNewRelayTable;
		m_pNewRelayTable = nullptr;
	}

	CRelayTableData * pOldTable;
	m_pNewRelayTable = new CRelayTableData;
	pOldTable = theApp.GetRelayTableData();
	if (pOldTable == nullptr)
		return nullptr;

	// [KHS 2021-1-8 13:28:06] 
	// ���� ������ ��ȣ�� ������� �ʵ��� �̸� ���� �߰�� ���̺��� ���������� ������ �´�.
	m_pNewRelayTable->CopyAllEquipContainer(
		pOldTable->GetEquipManager(ET_INPUTTYPE), pOldTable->GetEquipManager(ET_EQNAME)
		, pOldTable->GetEquipManager(ET_OUTPUTTYPE), pOldTable->GetEquipManager(ET_OUTCONTENTS)
		, pOldTable->GetEquipManager(ET_PUMPTYPE), pOldTable->GetEquipManager(ET_PSTYPE)
		, pOldTable->GetEquipManager(ET_PMPNAME)
	);

	
	nSize = m_arrPath.GetSize();
	for (i = 0; i < nSize; i++)
	{
		str = m_arrPath.GetAt(i);
		if (str.GetLength() <= 0)
			continue; 
		m_pNewRelayTable->AddDeviceTable(str);
	}

	
	m_pNewRelayTable->ProcessDeviceTableList(this);
	//m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, nAllCnt, nAllCnt);

	//20240319 GBM start - ������ ����ϴ� ���̺��� ��� �����ϰ� �����͸� �Է��� ���� ������ F4 ���̺��� �߰�
	CNewDBManager::Instance()->SetDBAccessor(theApp.m_pFasSysData->m_pDB);

	BOOL bRet = FALSE;

	// F4�߰� ���̺��� ���� ����� �߰�� �϶�ǥ�� F4 �߰� ������ �ִ� ��쿡�� ����, �翬�� �ش� Sheet �� �ϳ��� ������ �� �ְ����� Ȥ�� �Ϻ����� ������ DB�� ���� �ʵ��� ��
	bRet = CNewExcelManager::Instance()->bExistFT && CNewExcelManager::Instance()->bExistUT && CNewExcelManager::Instance()->bExistPI;
	if (bRet)
	{
		bRet = CNewDBManager::Instance()->CheckAndCreateF4DBTables();
		if (bRet)
		{
			GF_AddLog(L"F4 ���� ���̺� (������Ʈ, ���ű� TYPE, UNIT TYPE) ������ �����߽��ϴ�.");
			Log::Trace("Inserting new DB table succeeded!");
		}
		else
		{
			GF_AddLog(L"F4 ���� ���̺� (������Ʈ, ���ű� TYPE, UNIT TYPE) ������ �����߽��ϴ�, DB�� Ȯ���ϼ���.");
			Log::Trace("Inserting new DB table failed!");
		}
		
		bRet = CNewDBManager::Instance()->InsertDatasIntoF4DBTables();
		if (bRet)
		{
			GF_AddLog(L"F4 ���� ���̺� (������Ʈ, ���ű� TYPE, UNIT TYPE) ������ �߰��� �����߽��ϴ�.");
			Log::Trace("F4 DB table insertion succeeded!");
		}
		else
		{
			GF_AddLog(L"F4 ���� ���̺� (������Ʈ, ���ű� TYPE, UNIT TYPE) ������ �߰��� �����߽��ϴ�, DB�� Ȯ���ϼ���.");
			Log::Trace("F4 DB table insertion failed!");
		}
	}
	//20240319 GBM end

	//ChangeNewLoadSystemMapID(pOldTable, m_pNewRelayTable);
	m_mapTempNewName.clear();
	m_mapTempOldName.clear();
	MakeNameMap(m_pNewRelayTable , &m_mapTempNewName);
	MakeNameMap(theApp.GetRelayTableData(), &m_mapTempOldName);
	InitNewPtnDevList(m_pNewRelayTable);
	m_bNewFileLoaded = TRUE;
	return m_pNewRelayTable;
}

BOOL CFormLoadRelayTable::CompareByName(CRelayTableData * pOld , CRelayTableData * pNew
	, VT_RUPDATEITEM &vtIns, VT_RUPDATEITEM &vtOld, VT_RUPDATEITEM &vtNew
)
{
	//CMapSystemData mapIntersection , mapOnlyOld , mapOnlyNew;
	//std::vector<CDataSystem*> vtInter, vtOnlyOld, vtOnlyNew;
	//int nAllCnt = 0; 
	CDataDevice *pNewDev, *pOldDev;
	CDataSystem *pTemp;
	CDataSystem *pNewSys, *pOldSys;
	CString strKey , strIDKey;
	CMapSystemData::iterator it;
	CMapSystemData mapTemp;
	CMapSystemData *pOldMap , *pNewMap;
	CRUpdateItem * pItem;
	int nAddRIdx = 1; 
	//nAllCnt = m_lbPath.GetCount();
	
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, 3, 0);

	set_intersection(m_mapTempOldName.begin(), m_mapTempOldName.end()
		, m_mapTempNewName.begin(), m_mapTempNewName.end()
		, inserter(mapTemp, mapTemp.begin())
		, m_mapTempOldName.value_comp()
	);
	pOldMap = &m_mapTempOldName;
	pNewMap = &m_mapTempNewName;

	// mapTemp : �̸� ���� Map
	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pTemp = (CDataSystem *)it->second;
		if (pTemp->GetSysData() == nullptr)
			continue; 
		if (pTemp->GetDataType() != SE_RELAY)
		{
			// ���ű� , ���� , ���� ID ����
			pOldSys = (*pOldMap)[strKey];
			pNewSys = (*pNewMap)[strKey];
			ChangeSystemData(pNew , pNewSys, pOldSys);
			continue;
		}

		strKey = it->first;
		pOldSys = (*pOldMap)[strKey];
		pNewSys = (*pNewMap)[strKey];

		if (pOldSys == nullptr || pNewSys == nullptr
			|| pOldSys->GetSysData() == nullptr || pNewSys->GetSysData() == nullptr)
			continue;

		pNewDev = (CDataDevice*)pNewSys->GetSysData();
		pOldDev = (CDataDevice*)pOldSys->GetSysData();

#ifdef _DEBUG
		if (pNewDev->GetDeviceNum() == 28 && pNewDev->GetUnitNum() == 2
			&& pNewDev->GetFacpNum() == 0 && pNewDev->GetChnNum() == 2)
		{
			TRACE(_T(""));
		}
#endif

		strIDKey = GF_GetIDSysDataKey(SE_RELAY, pOldDev->GetFacpID(), pOldDev->GetUnitID()
			, pOldDev->GetChnID(), pOldDev->GetDeviceID());
		
		//����� ���̵�� ������ �ý����� �ִ��� Ȯ��
		pTemp = pNew->GetIDSystemItem(SE_RELAY, pOldDev->GetFacpID(), pOldDev->GetUnitID()
			, pOldDev->GetChnID(), pOldDev->GetDeviceID());
		if (pTemp == nullptr)
		{
			pNew->SetIDSystem(strIDKey, pNewSys);
 			pNewDev->SetRelayIndex(pOld->GetLastRelayIndex() + nAddRIdx);
 			nAddRIdx ++;
		}
		else
		{
			// �̸��� �ּҰ� ������ �ִ°Ŷ� ������ ���� ������ �����ϵ��� �����͸� ������ �´�
			if (pTemp->GetKey() == pNewSys->GetKey())
			{
				pNewDev->SetFacpID(pOldDev->GetFacpID());
				pNewDev->SetUnitID(pOldDev->GetUnitID());
				pNewDev->SetChnID(pOldDev->GetChnID());
				pNewDev->SetDeviceID(pOldDev->GetDeviceID());
				pNewDev->SetRelayIndex(pOldDev->GetRelayIndex());
				//pNewDev->SetNeedMakeID(FALSE);
				pNew->SetIDSystem(strIDKey, pNewSys);
			}
			else
			{
				// ������ �ִ°Ŷ� ID�� ������ Device�� ���� ������ ���Ӱ� �����Ѵ�.
				// �̸��� ���Ƶ� Address�� Ʋ������ �Ʒ� Comapare Type(�̸�,�ּҺ�)�� �°� �ߺ��̸� ���� �����͸� ��������
				// ���°��̸� ���Ӱ� ID�� �����Ѵ�.
				pNewDev->SetNeedMakeID(TRUE);
		
			}
			// [KHS 2021-1-8 13:36:54] 
			// Relay Index �߰�
			pNewDev->SetRelayIndex(pOldDev->GetRelayIndex());
		}

		if (m_nCompareType == CMP_ADDR)
		{
			// �ּҰ� ������ �̸��� ������ ���� ȸ��
			if (pNewDev->IsEqualName(pOldDev))
			{
				//pNewDev->CopyExceptLink(pOldDev);
				continue;
			}

			// �ּҰ� ������ �̸��� Ʋ���� �ٸ� ȸ��
		}
		else if (m_nCompareType == CMP_NAME)
		{
			if (pNewDev->IsEqualAddress(pOldDev))
			{
				//pNewDev->CopyExceptLink(pOldDev);
				continue;
			}
		}
		else
			continue;
		

		pItem = new CRUpdateItem;
		pItem->m_pNewCopyDev = new CDataDevice;
		pItem->m_pOldCopyDev = new CDataDevice;
		pItem->m_pNewCopyDev->CopyExceptLink(pNewDev);
		pItem->m_pOldCopyDev->CopyExceptLink(pOldDev);

		// �̸��� ������ �ּҰ� Ʋ����� ���� ��ũ �� ����ڰ� �������� �Է��� ��ũ�� ������ �´�.
		pItem->m_pNewCopyDev->CopyLink(pOldDev, LOGIC_MANUAL);

		vtIns.push_back(pItem);
		//VT_RUPDATEITEM[strKey] = pTemp;
	}
	mapTemp.clear();
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, 3, 1);

	//m_pNewRelayTable->SendProgressStep(this, PROG_RESULT_STEP, nAllCnt, nAllCnt, 3, 1);

	// ���� �϶�ǥ���� �ִ°�
	set_difference(m_mapTempOldName.begin(), m_mapTempOldName.end()
		, m_mapTempNewName.begin(), m_mapTempNewName.end()
		, inserter(mapTemp, mapTemp.begin())
		, m_mapTempOldName.value_comp()
	);
	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pTemp = (CDataSystem *)it->second;
		if (pTemp->GetSysData() == nullptr || pTemp->GetDataType() != SE_RELAY)
			continue;

		strKey = it->first;
		pOldSys = m_mapTempOldName[strKey];

		if (pOldSys == nullptr 
			|| pOldSys->GetSysData() == nullptr )
			continue;

		pOldDev = (CDataDevice*)pOldSys->GetSysData();
		pItem = new CRUpdateItem;
		pItem->m_pNewCopyDev = nullptr;
		pItem->m_pOldCopyDev = new CDataDevice;
		pItem->m_pOldCopyDev->CopyExceptLink(pOldDev);

		vtOld.push_back(pItem);
		//VT_RUPDATEITEM[strKey] = pTemp;
	}
	mapTemp.clear();
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, 3, 2);

	//m_pNewRelayTable->SendProgressStep(this, PROG_RESULT_STEP, nAllCnt, nAllCnt, 3, 2);

	// �� �϶�ǥ���� �ִ°�
	// �߰� �ؾߵȴ�.
	set_difference(m_mapTempNewName.begin(), m_mapTempNewName.end()
		, m_mapTempOldName.begin(), m_mapTempOldName.end()
		, inserter(mapTemp, mapTemp.begin())
		, m_mapTempNewName.value_comp()
	);

	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pTemp = (CDataSystem *)it->second;
		if (pTemp->GetSysData() == nullptr)
			continue;
		if (pTemp->GetDataType() != SE_RELAY)
		{
			SetNewIDSysData(pNew , pTemp);
			continue; 
		}
		strKey = it->first;
		pNewSys = m_mapTempNewName[strKey];
		
		SetNewIDSysData(pNew, pTemp);

		if (pNewSys == nullptr
			|| pNewSys->GetSysData() == nullptr)
			continue;

		pNewDev = (CDataDevice*)pNewSys->GetSysData();
		pNewDev->SetRelayIndex(pOld->GetLastRelayIndex() + nAddRIdx);
		nAddRIdx ++;
		//pNewDev->SetRelayIndex(pOld->GetNewRelayIndex());
#ifdef _DEBUG
		if(strKey == L"00.00.2.001.4")
			TRACE(L"");

#endif
		pItem = new CRUpdateItem;
		pItem->m_pNewCopyDev = new CDataDevice;
		pItem->m_pOldCopyDev = nullptr;
		pItem->m_pNewCopyDev->CopyExceptLink(pNewDev);
		
		vtNew.push_back(pItem);
		//VT_RUPDATEITEM[strKey] = pTemp;
	}
	mapTemp.clear();

	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, 3, 3);
	return TRUE;
}

BOOL CFormLoadRelayTable::CompareByAddress(CRelayTableData * pOld, CRelayTableData * pNew
	, VT_RUPDATEITEM &vtIns, VT_RUPDATEITEM &vtOld, VT_RUPDATEITEM &vtNew
)
{
	CMapSystemData *pOldMap , * pNewMap;
	CDataDevice *pNewDev, *pOldDev;
	CDataSystem *pTemp;
	CDataSystem *pNewSys, *pOldSys;
	CString strKey, strIDKey;
	CMapSystemData::iterator it;
	CMapSystemData mapTemp;
	CRUpdateItem * pItem;
	int nAddRIdx = 1;
	//CRelayTableData * pOldTable;
	int nAllCnt = 0;
	nAllCnt = m_lbPath.GetCount();

	//pOldTable = theApp.GetRelayTableData();
	pOldMap = pOld->GetSystemData();
	pNewMap = pNew->GetSystemData();

	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, 3, 0);
	//m_pNewRelayTable->SendProgressStep(this, PROG_RESULT_STEP, nAllCnt, nAllCnt, 3, 0);
	// ������ - �� ����� Key
	set_intersection(pOldMap->begin(), pOldMap->end()
		, pNewMap->begin(), pNewMap->end()
		, inserter(mapTemp, mapTemp.begin())
		, pOldMap->value_comp()
	);
	
	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pTemp = (CDataSystem *)it->second;
		if (pTemp->GetSysData() == nullptr)
			continue;
		if (pTemp->GetDataType() != SE_RELAY)
		{
			// ���ű� , ���� , ���� ID ����
			pOldSys = (*pOldMap)[strKey];
			pNewSys = (*pNewMap)[strKey];
			ChangeSystemData(pNew, pNewSys, pOldSys);
			continue;
		}
		strKey = it->first;
		pOldSys = (*pOldMap)[strKey];
		pNewSys = (*pNewMap)[strKey];

		if (pOldSys == nullptr || pNewSys == nullptr
			|| pOldSys->GetSysData() == nullptr || pNewSys->GetSysData() == nullptr)
			continue;

		pNewDev = (CDataDevice*)pNewSys->GetSysData();
		pOldDev = (CDataDevice*)pOldSys->GetSysData();

		if (pNewDev->GetDeviceNum() == 28 && pNewDev->GetUnitNum() == 2
			&& pNewDev->GetFacpNum() == 0 && pNewDev->GetChnNum() == 2)
		{
			TRACE(_T(""));
		}
		// �� �߰�� �϶�ǥ�� Device ID�� ���� �߰�� �϶�ǥ ���� ���̵�� ����
		pNewDev->SetFacpID(pOldDev->GetFacpID());
		pNewDev->SetUnitID(pOldDev->GetUnitID());
		pNewDev->SetChnID(pOldDev->GetChnID());
		pNewDev->SetDeviceID(pOldDev->GetDeviceID());

		pNewDev->SetRelayIndex(pOldDev->GetRelayIndex());
		
		strIDKey = GF_GetIDSysDataKey(SE_RELAY, pOldDev->GetFacpID(), pOldDev->GetUnitID()
			, pOldDev->GetChnID(), pOldDev->GetDeviceID());
		pNew->SetIDSystem(strIDKey, pNewSys);

		if (m_nCompareType == CMP_ADDR)
		{
			// �ּҰ� ������ �̸��� ������ ���� ȸ��
			if (pNewDev->IsEqualName(pOldDev))
			{
				//pNewDev->CopyExceptLink(pOldDev);
				continue;
			}

			// �ּҰ� ������ �̸��� Ʋ���� �ٸ� ȸ��
		}
		else if (m_nCompareType == CMP_NAME)
		{
			if (pNewDev->IsEqualAddress(pOldDev))
			{
				//pNewDev->CopyExceptLink(pOldDev);
				continue;
			}
		}
		else
			continue;

		//pNewDev->CopyExceptLink(pOldDev);
		pItem = new CRUpdateItem;
		pItem->m_pNewCopyDev = new CDataDevice;
		pItem->m_pOldCopyDev = new CDataDevice;

		pItem->m_pNewCopyDev->CopyExceptLink(pNewDev);
		pItem->m_pOldCopyDev->CopyExceptLink(pOldDev);

		vtIns.push_back(pItem);
	}
	mapTemp.clear();

	//m_pNewRelayTable->SendProgressStep(this, PROG_RESULT_STEP, nAllCnt, nAllCnt, 3, 1);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, 3, 1);

	set_difference(pOldMap->begin(), pOldMap->end()
		, pNewMap->begin(), pNewMap->end()
		, inserter(mapTemp, mapTemp.begin())
		, pOldMap->value_comp()
	);
	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pTemp = (CDataSystem *)it->second;
		if (pTemp->GetSysData() == nullptr || pTemp->GetDataType() != SE_RELAY)
			continue;

		strKey = it->first;
		pOldSys = (*pOldMap)[strKey];

		if (pOldSys == nullptr
			|| pOldSys->GetSysData() == nullptr)
			continue;

		pOldDev = (CDataDevice*)pOldSys->GetSysData();
		pItem = new CRUpdateItem;
		pItem->m_pNewCopyDev = nullptr;
		pItem->m_pOldCopyDev = new CDataDevice;
		pItem->m_pOldCopyDev->CopyExceptLink(pOldDev);

		vtOld.push_back(pItem);
		//VT_RUPDATEITEM[strKey] = pTemp;
	}
	mapTemp.clear();
	//m_pNewRelayTable->SendProgressStep(this, PROG_RESULT_STEP, nAllCnt, nAllCnt, 3, 2);
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, 3, 2);

	set_difference(pNewMap->begin(), pNewMap->end()
		, pOldMap->begin(), pOldMap->end()
		, inserter(mapTemp, mapTemp.begin())
		, pNewMap->value_comp()
	);
	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pTemp = (CDataSystem *)it->second;
		if (pTemp->GetSysData() == nullptr)
			continue;

		if (pTemp->GetDataType() != SE_RELAY)
		{
			SetNewIDSysData(pNew, pTemp);
			continue;
		}
		SetNewIDSysData(pNew, pTemp);

		strKey = it->first;
		pNewSys = (*pNewMap)[strKey];

		if (pNewSys == nullptr
			|| pNewSys->GetSysData() == nullptr)
			continue;

		
		pNewDev = (CDataDevice*)pNewSys->GetSysData();
#ifdef _DEBUG
		if(strKey == L"00.00.2.001.4")
			TRACE(L"");

#endif
		pNewDev->SetRelayIndex(pOld->GetLastRelayIndex() + nAddRIdx);
		nAddRIdx ++;
		pItem = new CRUpdateItem;
		pItem->m_pNewCopyDev = new CDataDevice;
		pItem->m_pOldCopyDev = nullptr;
		pItem->m_pNewCopyDev->CopyExceptLink(pNewDev);
		
		vtNew.push_back(pItem);
		//VT_RUPDATEITEM[strKey] = pTemp;
	}
	mapTemp.clear();
	m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, 3, 3);
	return TRUE;
}

BOOL CFormLoadRelayTable::SetNewIDSysData(CRelayTableData * pNewTable, CDataSystem *pNewSys)
{
	CDataFacp * pTempFacp, *pNewFacp;
	CDataUnit * pTempUnit, *pNewUnit;
	CDataDevice  *pNewDev;
	CDataSystem *pTempSys;
	CDataChannel * pTempChn, *pNewChn;
	CString strIDKey, strtemp;
	int nId = 0;
	if (pNewSys == nullptr ||
		pNewSys->GetSysData() == nullptr || pNewSys->GetDataType() == SE_RELAY)
		return FALSE;
	// ã�� : ���� ä���� �߰��ϴµ� ������ ������ ������ �� �Լ����� UnitData�� �����ؼ� �ִ´�.
	//       �̷��� ���� �� 
	//        ������ 
	switch (pNewSys->GetDataType())
	{
	case SE_FACP:
		//���� �ʿ� 
		pNewFacp = (CDataFacp *)pNewSys->GetSysData();
		//strIDKey = GF_GetIDSysDataKey(SE_FACP, pNewFacp->GetFacpID());
		pTempSys = pNewTable->GetIDSystemItem(SE_FACP, pNewFacp->GetFacpID());
		if (pNewSys != pTempSys)
		{
			// �̹� ���̵� ����� : Whole New ID
			// ���̵� �ý��ۺ���
			nId = pNewTable->GetWholeFacpID(pNewFacp->GetFacpNum());
			if (nId <= 0)
				return FALSE;
			pNewFacp->SetFacpID(nId);
			strIDKey = GF_GetIDSysDataKey(SE_FACP, pNewFacp->GetFacpID()); 
			pNewTable->SetIDSystem(strIDKey, pNewSys);
		}
		break;
	case SE_UNIT:
		pNewUnit = (CDataUnit *)pNewSys->GetSysData();
		
		pTempFacp = pNewTable->GetFacpByNum(pNewUnit->GetFacpNum());
		if (pTempFacp != nullptr && pTempFacp->GetFacpID() != pNewUnit->GetFacpID())
			pNewUnit->SetFacpID(pTempFacp->GetFacpID());

		pTempSys = pNewTable->GetIDSystemItem(SE_UNIT, pNewUnit->GetFacpID(), pNewUnit->GetUnitID());
		if (pNewSys != pTempSys)
		{
			// �̹� ���̵� ����� : Whole New ID
			// ���̵� �ý��ۺ���
			nId = pNewTable->GetWholeUnitID(pNewUnit->GetFacpNum() , pNewUnit->GetUnitNum());
			if (nId <= 0)
				return FALSE;
			pNewUnit->SetFacpID(nId);
			pNewUnit->SetUnitID(nId);
			strIDKey = GF_GetIDSysDataKey(SE_UNIT, pNewUnit->GetFacpID(), pNewUnit->GetUnitID());
			pNewTable->SetIDSystem(strIDKey, pNewSys);
		}
		break;
	case SE_CHANNEL:
		pNewChn = (CDataChannel *)pNewSys->GetSysData();

		pTempFacp = pNewTable->GetFacpByNum(pNewChn->GetFacpNum());
		if (pTempFacp != nullptr && pTempFacp->GetFacpID() != pNewChn->GetFacpID())
			pNewChn->SetFacpID(pTempFacp->GetFacpID());

		pTempUnit = pNewTable->GetUnitByNum(pNewChn->GetFacpNum(), pNewChn->GetUnitNum());
		if (pTempUnit != nullptr &&  pTempUnit->GetUnitID() != pNewChn->GetUnitID())
			pNewChn->SetUnitID(pTempUnit->GetUnitID());

		pTempSys = pNewTable->GetIDSystemItem(SE_CHANNEL
			, pNewChn->GetFacpID(), pNewChn->GetUnitID()
			, pNewChn->GetChnID());
		if (pNewSys != pTempSys)
		{
			// �̹� ���̵� ����� : Whole New ID
			// ���̵� �ý��ۺ���
			nId = pNewTable->GetWholeChnID(pNewChn->GetFacpNum(), pNewChn->GetUnitNum(), pNewChn->GetChnNum());
			if (nId <= 0)
				return FALSE;
			pNewChn->SetChnID(nId);
			strIDKey = GF_GetIDSysDataKey(SE_CHANNEL
				, pNewChn->GetFacpID(), pNewChn->GetUnitID()
				, pNewChn->GetChnID());
			pNewTable->SetIDSystem(strIDKey, pNewSys);
		}
		break;
	case SE_RELAY:
		pNewDev = (CDataDevice *)pNewSys->GetSysData();

		pTempFacp = pNewTable->GetFacpByNum(pNewDev->GetFacpNum());
		if (pTempFacp != nullptr && pTempFacp->GetFacpID() != pNewDev->GetFacpID())
			pNewDev->SetFacpID(pTempFacp->GetFacpID());

		pTempUnit = pNewTable->GetUnitByNum(pNewDev->GetFacpNum(), pNewDev->GetUnitNum());
		if (pTempUnit != nullptr && pTempUnit->GetUnitID() != pNewDev->GetUnitID())
			pNewDev->SetUnitID(pTempUnit->GetUnitID());

		pTempChn = pNewTable->GetChannelByNum(pNewDev->GetFacpNum(), pNewDev->GetUnitNum(), pNewDev->GetChnNum());
		if (pTempChn != nullptr && pTempChn->GetChnID() != pNewDev->GetChnID())
			pNewDev->SetChnID(pTempChn->GetChnID());

		pTempSys = pNewTable->GetIDSystemItem(SE_RELAY
			, pNewDev->GetFacpID(), pNewDev->GetUnitID()
			, pNewDev->GetChnID(), pNewDev->GetDeviceID());
		if (pNewSys != pTempSys)
		{
			// �̹� ���̵� ����� : Whole New ID
			// ���̵� �ý��ۺ���
			nId = pNewTable->GetWholeDeviceID(
				pNewDev->GetFacpNum(), pNewDev->GetUnitNum()
				, pNewDev->GetChnNum(), pNewDev->GetDeviceNum());
			if (nId <= 0)
				return FALSE;
			pNewDev->SetDeviceID(nId);
			strIDKey = GF_GetIDSysDataKey(SE_RELAY
				, pNewDev->GetFacpID(), pNewDev->GetUnitID()
				, pNewDev->GetChnID(), pNewDev->GetDeviceID());
			pNewTable->SetIDSystem(strIDKey, pNewSys);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CFormLoadRelayTable::ChangeSystemData(CRelayTableData * pNewTable
	, CDataSystem *pNewSys, CDataSystem * pOldSys)
{
	CDataFacp *pNewFacp;
	CDataUnit *pNewUnit;
	CDataDevice *pNewDev;
	CDataChannel  *pNewChn;
	CString strIDKey , strtemp;
	int i = 0;
	if (pNewSys == nullptr || 
		pNewSys->GetSysData() == nullptr || pNewSys->GetDataType() == SE_RELAY)
		return FALSE;
	if (pOldSys == nullptr ||
		pOldSys->GetSysData() == nullptr || pOldSys->GetDataType() == SE_RELAY)
		return FALSE;

	switch (pNewSys->GetDataType())
	{
	case SE_FACP:
		pNewFacp = (CDataFacp *)pNewSys->GetSysData();
		strIDKey = GF_GetIDSysDataKey(SE_FACP, pNewFacp->GetFacpID());
		pNewTable->RemoveSysIDMap(strIDKey);
		pNewTable->RemoveFacpMap(pNewFacp->GetFacpNum());
		pNewSys->CopyData(pOldSys);
		strIDKey = GF_GetIDSysDataKey(SE_FACP, pNewFacp->GetFacpID());
		// FacpMap ����
		pNewTable->SetFacpMap(pNewFacp->GetFacpNum(), pNewFacp);
		break;
	case SE_UNIT:
		pNewUnit = (CDataUnit *)pNewSys->GetSysData();
		strIDKey = GF_GetIDSysDataKey(SE_UNIT, pNewUnit->GetFacpID(), pNewUnit->GetUnitID());
		pNewTable->RemoveSysIDMap(strIDKey);
		pNewSys->CopyData(pOldSys); 
		strIDKey = GF_GetIDSysDataKey(SE_UNIT, pNewUnit->GetFacpID(), pNewUnit->GetUnitID());
		strtemp.Format(L"%02d.%02d", pNewTable->CvtFacpIDToNum(pNewUnit->GetFacpID()), pNewUnit->GetUnitNum());
		pNewTable->SetUnitMap(strtemp, pNewUnit);
		break;
	case SE_CHANNEL:
		pNewChn = (CDataChannel *)pNewSys->GetSysData();
		strIDKey = GF_GetIDSysDataKey(SE_CHANNEL
			, pNewChn->GetFacpID(), pNewChn->GetUnitID()
			, pNewChn->GetChnID()
		);
		pNewTable->RemoveSysIDMap(strIDKey);
		pNewSys->CopyData(pOldSys);
		strIDKey = GF_GetIDSysDataKey(SE_CHANNEL
			, pNewChn->GetFacpID(), pNewChn->GetUnitID()
			, pNewChn->GetChnID()
		);
		break;
	case SE_RELAY:
		pNewDev = (CDataDevice *)pNewSys->GetSysData();
		strIDKey = GF_GetIDSysDataKey(SE_RELAY
			, pNewDev->GetFacpID(), pNewDev->GetUnitID()
			, pNewDev->GetChnID() , pNewDev->GetDeviceID()
		);
		pNewTable->RemoveSysIDMap(strIDKey);
		pNewSys->CopyData(pOldSys);
		strIDKey = GF_GetIDSysDataKey(SE_RELAY
			, pNewDev->GetFacpID(), pNewDev->GetUnitID()
			, pNewDev->GetChnID(), pNewDev->GetDeviceID()
		); break;
	default:
		return FALSE;
	}
	pNewTable->SetIDSystem(strIDKey, pNewSys);
	return TRUE;
}

BOOL CFormLoadRelayTable::MakeNameMap(CRelayTableData* pTable, CMapSystemData *pMapName)
{
	CMapSystemData mapName;
	CMapSystemData *pOldMap;
	CMapSystemData::iterator it;
	CDataSystem * pData, *pFind;
	CDataDevice * pDev;
	CString strKey;
	int i = 0;
	pOldMap = pTable->GetSystemData();
	for (it = pOldMap->begin(); it != pOldMap->end(); it++)
	{
		pData = it->second;
		if (pData == nullptr || pData->GetSysData() == nullptr)
			continue;

		if (pData->GetDataType() != SE_RELAY)
			continue; 
		pDev = (CDataDevice *)pData->GetSysData();
		if (pDev->GetDeviceNum() == 28 && pDev->GetUnitNum() == 2
			&& pDev->GetFacpNum() == 0 && pDev->GetChnNum() == 2)
		{
			TRACE(_T(""));
		}
		for (i = 0; i < 100; i++)
		{
			strKey = pDev->MakeCompareName(i);
			pFind = (*pMapName)[strKey];
			if (pFind == nullptr)
				break;
		}
		(*pMapName)[strKey] = pData;
	}
	return TRUE;
}

// �����Է� ���� ��Ŵ
BOOL CFormLoadRelayTable::MakePtnItemMap(
	CDataPattern *pPtn, CRelayTableData * pTable
	,  CMapSystemData *pRetMap, int nCompareType)
{
	CMapSystemData mapName;
	CMapSystemData::iterator it;
	CDataSystem * pData, *pFind;
	CDataDevice * pDev;
	CString strKey;
	CDataLinked * pLnk;
	CPtrList * pList;
	POSITION pos;
	int i = 0;
	pList = pPtn->GetPtnItemList();
	if (pPtn == nullptr)
		return FALSE;
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pLnk = (CDataLinked*)pList->GetNext(pos);
		if (pLnk == nullptr)
			continue; 
		// ���� ������ ���ΰ� �������� �Է��� ������ ���� �Ѵ�.
		if(pLnk->GetLogicType() == LOGIC_MANUAL)
			continue; 
		pData = pTable->GetIDSystemItem(SE_RELAY , pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
			,pLnk->GetTgtChn(), pLnk->GetTgtDev());
		if (pData == nullptr)
			continue; 

		if (pData->GetSysData() == nullptr)
			continue;
		pDev = (CDataDevice*)pData->GetSysData();
		if (nCompareType == CMP_ADDR)
		{
			strKey = pData->GetKey();
		}
		else
		{
			for (i = 0; i < 100; i++)
			{
				strKey = pDev->MakeCompareName(i);
				pFind = (*pRetMap)[strKey];
				if (pFind == nullptr)
					break;
			}
		}
		(*pRetMap)[strKey] = pData;
	}
	return TRUE;
}

void CFormLoadRelayTable::RemoveAllData()
{
	RemoveCompareResult();
	RemoveUSINGDupList();
	RemoveUSEDDupList();
	RemovePatternChange();
	m_mapTempNewName.clear();
	m_mapTempOldName.clear();
	
	CDataLinked * pLink;
	while (!m_ptrManualLink.IsEmpty())
	{
		pLink = (CDataLinked *)m_ptrManualLink.RemoveHead();
		if (pLink == nullptr)
			continue;
		delete pLink;
		pLink = nullptr;
	}
}


void CFormLoadRelayTable::RemoveCompareResult()
{
	int  i, nCnt;
	CRUpdateItem * pItem;
	nCnt = m_vtIntersection.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtIntersection[i];
		if (pItem == nullptr)
			continue; 
		delete pItem;
		pItem = nullptr;
	}
	m_vtIntersection.clear();

	nCnt = m_vtOnlyOld.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtOnlyOld[i];
		if (pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
	m_vtOnlyOld.clear();

	nCnt = m_vtOnlyNew.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtOnlyNew[i];
		if (pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
	m_vtOnlyNew.clear();
}

void CFormLoadRelayTable::RemoveUSINGDupList()
{
	CRTableUpdate * pOutUpdate = nullptr;
	while (!m_ptrUSINGDupList.IsEmpty())
	{
		pOutUpdate = (CRTableUpdate *)m_ptrUSINGDupList.RemoveHead();
		if (pOutUpdate == nullptr)
			continue; 
		delete pOutUpdate;
		pOutUpdate = nullptr;
	}
}

void CFormLoadRelayTable::RemoveUSEDDupList()
{
	CRTableUpdate * pOutUpdate = nullptr;
	while (!m_ptrUSEDDupList.IsEmpty())
	{
		pOutUpdate = (CRTableUpdate *)m_ptrUSEDDupList.RemoveHead();
		if (pOutUpdate == nullptr)
			continue;
		delete pOutUpdate;
		pOutUpdate = nullptr;
	}
}


void CFormLoadRelayTable::RemovePatternChange()
{
	CRPatternUpdate * pOutUpdate = nullptr;
	while (!m_ptrPatternUsedDupList.IsEmpty())
	{
		pOutUpdate = (CRPatternUpdate *)m_ptrPatternUsedDupList.RemoveHead();
		if (pOutUpdate == nullptr)
			continue;
		delete pOutUpdate;
		pOutUpdate = nullptr;
	}
}
int CFormLoadRelayTable::GetManualLinkData(CPtrList * pList , CRelayTableData * pOldTable)
{
	YAdoDatabase * pDB;
	CString strSql , strExcept;
	int i, nCnt , nRet = 0;
	CDataDevice * pDev = nullptr, *pNewDev = nullptr, *pTemp = nullptr;
	CDataLinked *pOldLink = nullptr , * pCopiedLink = nullptr;

	int nFacp, nUnit, nChn, nDev;
	int nlf, nlu, nlc, nld; //< ������ ID
	int ntf, ntu, ntc, ntd , nLkType;

	m_ptrManualLink.RemoveAll();

	pDB = pOldTable->GetPrjDB();
	if (pDB == nullptr)
	{
		USERLOG(L"������Ʈ �����ͺ��̽��� �������µ� �����߽��ϴ�.");
		return 0;
	}
	
	strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LG_TYPE=%d " 
		, LOGIC_MANUAL);
	strExcept = GetDulicationLinkQuery(m_vtIntersection);
	if (strExcept.GetLength() > 0)
	{
		strSql += "\nEXCEPT\nSELECT * FROM TB_LINK_RELAY \n WHERE ";
		strSql += strExcept;
	}
	strExcept = GetDulicationSourceQuery(m_vtIntersection);
	if (strExcept.GetLength() > 0)
	{
		strSql += "\nEXCEPT\nSELECT * FROM TB_LINK_RELAY \n WHERE ";
		strSql += strExcept;
	}

	if (pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"��������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	pDev = nullptr;
	nLkType = nlf = nlu = nlc = nld = -1;

	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;
		nLkType = ntf = ntu = ntc = ntd = -1;
		if (pDB->GetFieldValue(L"SRC_FACP", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"SRC_UNIT", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"SRC_CHN", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"SRC_RLY", nDev) == FALSE)
			nDev = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
		{
			pDB->MoveNext();
			continue;
		}

		if (pDev == nullptr 
			|| nlf != nFacp || nlu != nUnit || nlc != nChn || nld != nDev)
		{
			nlf = nFacp;
			nlu = nUnit;
			nlc = nChn;
			nld = nDev;
			pDev = pOldTable->GetDeviceByID(nlf, nlu, nlc, nld);
		}

		if (pDev == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		if (pDB->GetFieldValue(L"LINK_TYPE", nLkType) == FALSE)
			nLkType = -1;
		if (pDB->GetFieldValue(L"TGT_FACP", ntf) == FALSE)
			ntf = -1;
		if (pDB->GetFieldValue(L"TGT_UNIT", ntu) == FALSE)
			ntu = -1;
		if (pDB->GetFieldValue(L"TGT_CHN", ntc) == FALSE)
			ntc = -1;
		if (pDB->GetFieldValue(L"TGT_RLY", ntd) == FALSE)
			ntd = -1;
		if (ntf < 0 || ntu < 0 || ntc < 0 || ntd < 0)
		{
			pDB->MoveNext();
			continue;
		}
		pOldLink = pDev->GetLink(nLkType, ntf, ntu, ntc, ntd);
		if (pOldLink == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pCopiedLink = new CDataLinked;
		pCopiedLink->CopyData(pOldLink);
		pList->AddTail(pCopiedLink);
		nRet++;
		pDB->MoveNext();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, nCnt, i);
	}
	pDB->RSClose();
	return nRet;
}

int CFormLoadRelayTable::MakeDuplicationManualOutputLink(
	CRelayTableData * pOldTable
	, VT_RUPDATEITEM &vtIns, CPtrList& ptrList)
{
	int i, nCnt;
	CString strSql, strExcept , strKey;
	CDataDevice * pDevOldSrc = nullptr, *pDevNewSrc = nullptr, *pTemp = nullptr;
	CDataDevice * pCopiedOldSrc = nullptr, *pCopiedNewSrc = nullptr;
	CDataDevice * pCopiedOldTarget = nullptr, *pCopiedNewTarget = nullptr;
	CDataDevice * pDevOldTarget = nullptr, *pDevNewTarget = nullptr;
	CDataLinked *pOldLink = nullptr;
	CRTableUpdate * pOutputUpdate = nullptr;
	int nFacp, nUnit, nChn, nDev;
	int nlf, nlu, nlc, nld; //< ������ ID
	int ntf, ntu, ntc, ntd, nLkType;

	YAdoDatabase * pDB; 
	pDB = pOldTable->GetPrjDB();
	if (pDB == nullptr)
	{
		USERLOG(L"������Ʈ �����ͺ��̽��� �������µ� �����߽��ϴ�.");
		return 0;
	}
	RemoveUSINGDupList();
	// �������� �߰��� ���������Ϳ��� �ߺ��Ǵ� �׸��� ����ϴ� ��� ����
	strExcept = GetDulicationLinkQuery(vtIns);
	if (strExcept.GetLength() > 0)
	{
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY  WHERE LG_TYPE = %d \n"
			, LOGIC_MANUAL
			);
		strExcept = L" AND (" + strExcept + L")";
		strSql += strExcept;
	}
	else
		return 1;
	TRACE(strSql + L"\n");
	// ���������� �� �������� �Էµ� �͵� �߿� 
	// �Է��� �ߺ��Ǵ� ȸ���� ���� �� ������ �κ�
	// ���� ��� ���� �Է� �߿� ���� �Է°� �ߺ��Ǵ� ���� ���� , ����� �������� �ԷµȰ͸� ǥ��.
	strExcept = GetDulicationSourceQuery(vtIns);
	if (strExcept.GetLength() > 0)
	{
		strSql += "\nEXCEPT\nSELECT * FROM TB_LINK_RELAY \n WHERE ";
		strSql += strExcept;
	}
	strSql += L" ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY,TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY ";

	TRACE(strSql + L"\n");
	if (pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"��������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	pDevOldSrc = nullptr;
	nLkType = nlf = nlu = nlc = nld = -1;

	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;
		nLkType = ntf = ntu = ntc = ntd = -1;
		if (pDB->GetFieldValue(L"SRC_FACP", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"SRC_UNIT", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"SRC_CHN", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"SRC_RLY", nDev) == FALSE)
			nDev = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
		{
			pDB->MoveNext();
			continue;
		}

		if (nlf != nFacp || nlu != nUnit || nlc != nChn || nld != nDev)
		{
			nlf = nFacp;
			nlu = nUnit;
			nlc = nChn;
			nld = nDev;
			pDevOldSrc = pOldTable->GetDeviceByID(nlf , nlu , nlc ,nld);

			// [2023/1/6 14:21:41 KHS] 
			// TB_LINK_RELAY���� �ִµ� TB_RELAY_LIST�� ���°��
			if(pDevOldSrc == nullptr)
				continue; 
			pDevNewSrc = ConvertUpdateItem(TRUE, pDevOldSrc);
			pOutputUpdate = new CRTableUpdate;
			if (pDevOldSrc != nullptr)
			{
				pOutputUpdate->m_pOldSourceCopyDev = new CDataDevice;
				pOutputUpdate->m_pOldSourceCopyDev->CopyExceptLink(pDevOldSrc);
			}
			if (pDevNewSrc != nullptr)
			{
				pOutputUpdate->m_pNewSourceCopyDev = new CDataDevice;
				pOutputUpdate->m_pNewSourceCopyDev->CopyExceptLink(pDevNewSrc);
			}
			else
			{
				pOutputUpdate->m_pNewSourceCopyDev = new CDataDevice;
				pOutputUpdate->m_pNewSourceCopyDev->CopyExceptLink(pOutputUpdate->m_pOldSourceCopyDev);

				pDevNewSrc = ConvertUpdateItem(TRUE, pDevOldSrc);
			}
			ptrList.AddTail(pOutputUpdate);
		}

		if (pOutputUpdate == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

		if (pDB->GetFieldValue(L"LINK_TYPE", nLkType) == FALSE)
			nLkType = -1;
		if (pDB->GetFieldValue(L"TGT_FACP", ntf) == FALSE)
			ntf = -1;
		if (pDB->GetFieldValue(L"TGT_UNIT", ntu) == FALSE)
			ntu = -1;
		if (pDB->GetFieldValue(L"TGT_CHN", ntc) == FALSE)
			ntc = -1;
		if (pDB->GetFieldValue(L"TGT_RLY", ntd) == FALSE)
			ntd = -1;
		if (ntf < 0 || ntu < 0 || ntc < 0 || ntd < 0)
		{
			pDB->MoveNext();
			continue;
		}

		pDevOldTarget = pOldTable->GetDeviceByID(ntf, ntu, ntc, ntd);

		if (pDevOldTarget == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

		pDevNewTarget = ConvertUpdateItem(TRUE , pDevOldTarget);
		if (pDevNewTarget == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pOutputUpdate->AddUpdateItem(pDevOldTarget, pDevNewTarget);

		//nRet++;
		pDB->MoveNext();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, nCnt, i);
	}
	pDB->RSClose();
	return 1;
}


int CFormLoadRelayTable::MakeDuplicationManualInputLink(
	CRelayTableData * pOldTable
	, VT_RUPDATEITEM &vtIns, CPtrList& ptrList)
{
	int i, nCnt;
	CString strSql, strExcept;
	CDataDevice * pDevOldSrc = nullptr, *pDevNewSrc = nullptr, *pTemp = nullptr;
	CRTableUpdate * pOutputUpdate = nullptr;
	int nFacp, nUnit, nChn, nDev;

	YAdoDatabase * pDB;
	pDB = pOldTable->GetPrjDB();
	if (pDB == nullptr)
	{
		USERLOG(L"������Ʈ �����ͺ��̽��� �������µ� �����߽��ϴ�.");
		return 0;
	}
	RemoveUSINGDupList();
	// �������� �߰��� �Է��߿� 
	strExcept = GetDulicationSourceQuery(vtIns);
	if (strExcept.GetLength() > 0)
	{
		strSql.Format(L"SELECT SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY FROM TB_LINK_RELAY  WHERE LG_TYPE = %d \n"
			, LOGIC_MANUAL
		);
		strExcept = L" AND (" + strExcept + L")";
		strSql += strExcept;
	}
	else
		return 1;

	strSql += L" GROUP BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY ";
//	strSql += L" ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY,TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY ";

	TRACE(strSql + L"\n");
	if (pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"��������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	pDevOldSrc = nullptr;

	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;
		if (pDB->GetFieldValue(L"SRC_FACP", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"SRC_UNIT", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"SRC_CHN", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"SRC_RLY", nDev) == FALSE)
			nDev = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
		{
			pDB->MoveNext();
			continue;
		}

		pDevOldSrc = pOldTable->GetDeviceByID(nFacp, nUnit, nChn, nDev);
		pDevNewSrc = ConvertUpdateItem(TRUE, pDevOldSrc);
		pOutputUpdate = new CRTableUpdate;
		pOutputUpdate->m_pNewSourceCopyDev = new CDataDevice;
		pOutputUpdate->m_pOldSourceCopyDev = new CDataDevice;
		pOutputUpdate->m_pOldSourceCopyDev->CopyExceptLink(pDevOldSrc);
		if (pDevNewSrc == nullptr)
			pOutputUpdate->m_pNewSourceCopyDev->CopyExceptLink(pDevOldSrc);
		else
			pOutputUpdate->m_pNewSourceCopyDev->CopyExceptLink(pDevNewSrc);

		// �����Էµ� �� �߿� Pattern ������ ���� �߻�
		// ����1. ���� ������Ʈ�Ǵ� �϶�ǥ�� ���� �� --> ���� ������Ʈ�ϴ� �϶�ǥ�� ���� �߰�
		// ����2. ���� ������Ʈ �Ǵ� �϶�ǥ�� ���� ������ ������ �Ϻ� ȸ�θ� ���� �� - �� ���Ͽܿ� ���� ȸ�� �������� �߰�
		// ����3. ���� ������Ʈ �Ǵ� �϶�ǥ�� ���� ������ ������ �������� ���� ��� - ���� �����Ѵ�.
		pOutputUpdate->m_pNewSourceCopyDev->CopyLink(pDevOldSrc, LOGIC_MANUAL);
		ptrList.AddTail(pOutputUpdate);

		//nRet++;
		pDB->MoveNext();
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, nCnt, i);
	}
	pDB->RSClose();
	return 1;
}


int CFormLoadRelayTable::MakePtnChangeData(
	CRelayTableData * pNewTable, CRelayTableData * pOldTable, CPtrList& ptrList)
{
	CRPatternUpdate * pUpdate;
	CRUpdateItem *pUItem;
	POSITION pos,iPos;
	CPtrList * pItemList;
	CDataDevice * pNewDev;
	CDataLinked * pLnk;
	CDataSystem * pSys;
	CDataPattern * pOld,*pNew;
	std::shared_ptr<CManagerPattern> spOldMng,spNewMng;
	int nCnt, nStep , nPCnt1 , nPCnt2;
	int nOldMaxPtnID = 0;
	RemovePatternChange();
	if (pOldTable == nullptr || pNewTable == nullptr || pNewTable == nullptr)
		return 0; 
	spOldMng = pOldTable->GetPatternManager();
	spNewMng = pNewTable->GetPatternManager();

	if (spOldMng == nullptr || spNewMng == nullptr)
		return 0;
	nOldMaxPtnID = spOldMng->GetMaxPatternID();
	nPCnt2 = spNewMng->GetCount();
	nPCnt1 = spOldMng->GetCount();
	nCnt = nPCnt1 + nPCnt2;
	nStep = 0; 
	pos = spNewMng->GetHeadPosition();
	while (pos)
	{
		nStep++;
		pNew = spNewMng->GetNext(pos);
		if (pNew == nullptr)
			continue; 
		TRACE(L"pNew->GetPatternName() : %s\n" ,pNew->GetPatternName());

		if(pNew->GetPatternName().Find(L"�溸���20F") >= 0)
			TRACE(L"pNew->GetPatternName().Find(L\"�溸���20F\")");

		pOld = spOldMng->FindPatternData(pNew->GetPatternName());
		if (pOld == nullptr)
		{
			// ���� �߰��ϴ� ����
			pUpdate = new CRPatternUpdate;
			pUpdate->SetNewPattern(pNew);
			pUpdate->m_nNewPID = nOldMaxPtnID;
			pItemList = pNew->GetPtnItemList();
			iPos = pItemList->GetHeadPosition();
			while (iPos)
			{
				pLnk = (CDataLinked*)pItemList->GetNext(iPos);
				if (pLnk == nullptr)
					continue;

				pSys = pNewTable->GetIDSystemItem(SE_RELAY, pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
					, pLnk->GetTgtChn(), pLnk->GetTgtDev());
				if (pSys == nullptr)
					continue;

				if (pSys->GetSysData() == nullptr)
					continue;
				pNewDev = (CDataDevice*)pSys->GetSysData();
				pUItem = new CRUpdateItem;
				pUItem->m_pNewCopyDev = new CDataDevice;
				pUItem->m_pOldCopyDev = nullptr;
				pUItem->m_pNewCopyDev->CopyExceptLink(pNewDev);
				pUpdate->m_vtAdd.push_back(pUItem);
			}
			nOldMaxPtnID++;
		}
		else
		{
			// ������ �ִ� ���� [10/21/2021 KHS]
			pUpdate = new CRPatternUpdate;
			//pCopy = new CDataPattern;
			//pCopy->CopyDataExceptDevice(pNew);
			pUpdate->SetNewPattern(pNew);
			//pCopy = new CDataPattern;
			//pCopy->CopyDataExceptDevice(pOld);
			pUpdate->SetOldPattern(pOld);

			// ���� ���ϰ� �����ϰ� ���� �ϱ�����
			if (pNew->GetPatternID() != pOld->GetPatternID())
				pUpdate->m_nNewPID = pOld->GetPatternID();

			if (MakeChangedPtnItem(pNewTable, pOldTable, pNew, pOld, pUpdate) <= 0)
			{
				delete pUpdate;
				pUpdate = nullptr;
				continue;
			}
		}

		
		ptrList.AddTail(pUpdate);
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, nCnt, nStep);
	}

	// [KHS 2021-2-18 08:13:48] 
	// �������� �ִ� ���� �߰�� �϶�ǥ ������Ʈ �ϸ鼭 �߰��ؾߵȴ�.
	pos = spOldMng->GetHeadPosition();
	while (pos)
	{
		nStep++;
		pOld = spOldMng->GetNext(pos);
		if (pOld == nullptr)
			continue;
		// �������� ���� ������ üũ���� �ʴ´�.
		if(pOld->GetManualMake() == 1)
			continue; 

		pNew = spNewMng->FindPatternData(pOld->GetPatternName());
		if (pNew == nullptr)
		{
			// ���� �ϴ� ���� - ���� �߰��ϴ� �϶�ǥ���� ���� ������ �� �ִ� ����
			pUpdate = new CRPatternUpdate;
			pUpdate->m_nChnageType = 0; // ����
			pUpdate->SetOldPattern(pOld);
			pUpdate->SetNewPattern(NULL);
			pItemList = pOld->GetPtnItemList();
			iPos = pItemList->GetHeadPosition();
			while (iPos)
			{
				pLnk = (CDataLinked*)pItemList->GetNext(iPos);
				if (pLnk == nullptr)
					continue;
				// ���� �϶�ǥ���� ������ ã��
				pSys = pOldTable->GetIDSystemItem(SE_RELAY,pLnk->GetTgtFacp(),pLnk->GetTgtUnit()
					,pLnk->GetTgtChn(),pLnk->GetTgtDev());
				if(pSys == nullptr)
					continue;

				if(pSys->GetSysData() == nullptr)
					continue;
				pNewDev = (CDataDevice*)pSys->GetSysData();

				pUItem = new CRUpdateItem;
				pUItem->m_pNewCopyDev = nullptr;
				pUItem->m_pOldCopyDev = new CDataDevice;
				pUItem->m_pOldCopyDev->CopyExceptLink(pNewDev);
				pUpdate->m_vtDel.push_back(pUItem);
			}
			ptrList.AddTail(pUpdate);
		}
		m_pNewRelayTable->SendProgStep(this, PROG_RESULT_STEP, nCnt, nStep);
	}
	return 1;
}


int CFormLoadRelayTable::MakeChangedPtnItem(
	CRelayTableData * pNewTable, CRelayTableData * pOldTable
	, CDataPattern*pNewPtn, CDataPattern * pOldPtn
	, CRPatternUpdate * pUpdate)
{
	CDataSystem *pOldSys , *pNewSys, *pTemp;
	CString strKey;
	CDataDevice *pNewDev, *pOldDev;
	CRUpdateItem * pItem;
	int nCnt = 0; 
	//CDataPattern * pOld, *pNew, *pCopy;

	std::shared_ptr<CManagerPattern> spOldMng, spNewMng;
	CMapSystemData mapOld, mapNew , mapTemp;
	CMapSystemData::iterator it;
	if (pOldTable == nullptr || pNewTable == nullptr 
		|| pNewPtn == nullptr || pOldPtn == nullptr
		|| pUpdate == nullptr)
		return 0;

	if (pUpdate->m_nOldPID <= 0 || pUpdate->m_nNewPID <= 0)
		return 0;


	if (MakePtnItemMap(pNewPtn, pNewTable, &mapNew , m_nCompareType) == FALSE)
		return 0; 

	// Pattern ������ ���� �� �������� �Է��� ������ ���� ��Ű�� �ʴ´�.
	// ���ο� �϶�ǥ�� ������ ������ �ڵ�/���� ����
	if (MakePtnItemMap(pOldPtn, pOldTable, &mapOld, m_nCompareType) == FALSE)
	{
		mapNew.clear();
		return 0;
	}

	// OLD , NEW ��� �ִ� ���� �׸��
	set_intersection(mapOld.begin(), mapOld.end()
		, mapNew.begin(), mapNew.end()
		, inserter(mapTemp, mapTemp.begin())
		, mapOld.value_comp()
	);

	
	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pTemp = (CDataSystem *)it->second;
		if (pTemp->GetSysData() == nullptr || pTemp->GetDataType() != SE_RELAY)
			continue;

		strKey = it->first;
		pOldSys = mapOld[strKey];
		pNewSys = mapNew[strKey];

		if (pOldSys == nullptr || pNewSys == nullptr
			|| pOldSys->GetSysData() == nullptr || pNewSys->GetSysData() == nullptr)
			continue;

		pNewDev = (CDataDevice*)pNewSys->GetSysData();
		pOldDev = (CDataDevice*)pOldSys->GetSysData();
		if (m_nCompareType == CMP_ADDR)
		{
			// �ּҰ� ������ �̸��� ������ ���� ȸ��
			if (pNewDev->IsEqualName(pOldDev))
			{
				//pNewDev->CopyExceptLink(pOldDev);
				continue;
			}

			// �ּҰ� ������ �̸��� Ʋ���� �ٸ� ȸ��
		}
		else if (m_nCompareType == CMP_NAME)
		{
			if (pNewDev->IsEqualAddress(pOldDev))
			{
				//pNewDev->CopyExceptLink(pOldDev);
				continue;
			}
		}
		else
			continue;

		pItem = new CRUpdateItem;
		pItem->m_pNewCopyDev = new CDataDevice;
		pItem->m_pOldCopyDev = new CDataDevice;
		pItem->m_pNewCopyDev->CopyExceptLink(pNewDev);
		pItem->m_pOldCopyDev->CopyExceptLink(pOldDev);
		pUpdate->m_vtIns.push_back(pItem);
		nCnt++;
		//VT_RUPDATEITEM[strKey] = pTemp;
	}
	mapTemp.clear();

// 	// ���� - ������������ �ִ� �׸� ���� - �������� �߰��� ȸ�δ� ��������(mapOld�� ���� �ֱ� ������)
#if _DELETE_OLD_PATTERN_ == 1
 	set_difference(mapOld.begin(), mapOld.end()
 		, mapNew.begin(), mapNew.end()
 		, inserter(mapTemp, mapTemp.begin())
 		, mapOld.value_comp()
 	);
 	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
 	{
 		if (it->second == nullptr)
 			continue;
 		pTemp = (CDataSystem *)it->second;
 		if (pTemp->GetSysData() == nullptr || pTemp->GetDataType() != SE_RELAY)
 			continue;
 
 		strKey = it->first;
 		pOldSys = mapOld[strKey];
 
 		if (pOldSys == nullptr
 			|| pOldSys->GetSysData() == nullptr)
 			continue;
 
 		pOldDev = (CDataDevice*)pOldSys->GetSysData();
 		pItem = new CRUpdateItem;
 		pItem->m_pNewCopyDev = nullptr;
 		pItem->m_pOldCopyDev = new CDataDevice;
 		pItem->m_pOldCopyDev->CopyExceptLink(pOldDev);
 
		//pUpdate->m_vtDel.push_back(pItem);
		pUpdate->m_vtDel.push_back(pItem);
 		nCnt++;
 
 		//VT_RUPDATEITEM[strKey] = pTemp;
 	}
 	mapTemp.clear();
#endif
	//m_pNewRelayTable->SendProgressStep(this, PROG_RESULT_STEP, nAllCnt, nAllCnt, 3, 2);

	set_difference(mapNew.begin(), mapNew.end()
		, mapOld.begin(), mapOld.end()
		, inserter(mapTemp, mapTemp.begin())
		, mapNew.value_comp()
	);
	for (it = mapTemp.begin(); it != mapTemp.end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pTemp = (CDataSystem *)it->second;
		if (pTemp->GetSysData() == nullptr || pTemp->GetDataType() != SE_RELAY)
			continue;

		strKey = it->first;
		pNewSys = mapNew[strKey];

		if (pNewSys == nullptr
			|| pNewSys->GetSysData() == nullptr)
			continue;

		pNewDev = (CDataDevice*)pNewSys->GetSysData();

		pItem = new CRUpdateItem;
		pItem->m_pNewCopyDev = new CDataDevice;
		pItem->m_pOldCopyDev = nullptr;
		pItem->m_pNewCopyDev->CopyExceptLink(pNewDev);
		pUpdate->m_vtAdd.push_back(pItem);
		nCnt++;

		//VT_RUPDATEITEM[strKey] = pTemp;
	}
	mapTemp.clear();
	return nCnt;
}

CString CFormLoadRelayTable::GetDulicationLinkQuery(VT_RUPDATEITEM &vtIns)
{
	CString str, strSql;
	CRUpdateItem * pItem;
	CDataDevice * pOldDev;
	int i = 0;
	strSql = str = L"";
	for (i = 0 ; i < vtIns.size() ; i++)
	{
		pItem = vtIns[i];
		if (pItem == nullptr 
			|| pItem->GetNewDevice() == nullptr || pItem->GetOldDevice() == nullptr)
			continue;

		pOldDev = pItem->GetOldDevice();
		if(i == 0) 
			str.Format(L" (TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d) \n"
				, pOldDev->GetFacpID() , pOldDev->GetUnitID() , pOldDev->GetChnID() , pOldDev->GetDeviceID()
			);
		else
			str.Format(L" OR (TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d) \n"
				, pOldDev->GetFacpID(), pOldDev->GetUnitID(), pOldDev->GetChnID(), pOldDev->GetDeviceID()
			);
		strSql += str;
	}
	return strSql;
}

CString CFormLoadRelayTable::GetDulicationSourceQuery(VT_RUPDATEITEM &vtIns)
{
	CString str, strSql;
	CRUpdateItem * pItem;
	CDataDevice * pOldDev;
	int i = 0;
	strSql = str = L"";
	for (i = 0; i < vtIns.size(); i++)
	{
		pItem = vtIns[i];
		if (pItem == nullptr
			|| pItem->GetNewDevice() == nullptr || pItem->GetOldDevice() == nullptr)
			continue;

		pOldDev = pItem->GetOldDevice();
		if (i == 0)
			str.Format(L" (SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d) \n"
				, pOldDev->GetFacpID(), pOldDev->GetUnitID(), pOldDev->GetChnID(), pOldDev->GetDeviceID()
			);
		else
			str.Format(L" OR (SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d) \n"
				, pOldDev->GetFacpID(), pOldDev->GetUnitID(), pOldDev->GetChnID(), pOldDev->GetDeviceID()
			);
		strSql += str;
	}
	return strSql;
}

CDataDevice* CFormLoadRelayTable::ConvertUpdateItem(BOOL bConvertNew, CDataDevice * pDevice)
{
	CRUpdateItem * pItem;
	int nCnt, i; 
	CDataDevice * pOld, *pNew;
	nCnt = m_vtIntersection.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtIntersection[i];

		if (pItem == nullptr)
			continue;
		pNew = pItem->GetNewDevice();
		pOld = pItem->GetOldDevice();
		if (pNew == nullptr || pOld == nullptr)
			continue;

		if (bConvertNew)
		{
			// Old to New
			if (pOld->IsEqualAddress(pDevice))
				return pNew;
		}
		else
		{
			if (pNew->IsEqualAddress(pDevice))
				return pOld;
		}
	}
	return nullptr;
}


CDataDevice* CFormLoadRelayTable::ConvertUpdateItem(BOOL bConvertNew
	, int nFID, int nUID, int nCID, int nDID)
{
	CRUpdateItem * pItem;
	int nCnt, i;
	CDataDevice * pOld, *pNew;
	nCnt = m_vtIntersection.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtIntersection[i];

		if (pItem == nullptr)
			continue;
		pNew = pItem->GetNewDevice();
		pOld = pItem->GetOldDevice();
		if (pNew == nullptr || pOld == nullptr)
			continue;

		if (bConvertNew)
		{
			// Old to New
			if (pOld->GetFacpID() == nFID && pOld->GetUnitID() == nUID
				&& pOld->GetChnID() == nCID && pOld->GetDeviceID() == nDID)
				return pNew;
		}
		else
		{
			if (pNew->GetFacpID() == nFID && pNew->GetUnitID() == nUID
				&& pNew->GetChnID() == nCID && pNew->GetDeviceID() == nDID)
				return pOld;
		}
	}
	return nullptr;
}

int CFormLoadRelayTable::RemoveAllLinkData(YAdoDatabase * pDB, CRelayTableData * pNewTable)
{
	CMapSystemData::iterator it;
	CDataDevice *pDev;
	CDataSystem *pSys;
	CMapSystemData * pMap;

	CString strSql;
	strSql = L"DELETE FROM TB_LINK_RELAY ";
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0; 

	pMap = pNewTable->GetSystemData();
	for (it = pMap->begin(); it != pMap->end(); it++)
	{
		if (it->second == nullptr)
			continue; 
		pSys = it->second;
		if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
			continue; 
		pDev = (CDataDevice *)pSys->GetSysData();
		pDev->RemoveAllLink();
	}
	return 1;
}

int CFormLoadRelayTable::AddFacpUnitChannel(CRelayTableData * pNewTable, CRelayTableData * pOldTable)
{
	CMapSystemData * pOldMap , *pNewMap;
	CMapIDSystemData *pIDMap;
	CDataFacp  *pNewFacp;
	CDataUnit *pNewUnit;
	CDataSystem *pOldSys ,*pNewSys ;
	CDataChannel *pNewChn; 
	std::map<int, CDataFacp *> * pMapFacp;
	std::map<int, CDataFacp *>::iterator fit;
	std::map<CString, CDataUnit *>::iterator uit;
	std::map<CString, CDataUnit *> * pMapUnit;
	CMapSystemData::iterator it;

	CString strKey , strIDKey , strtemp;
	/// idmap , map ,  facp , unit,chn ����
	/// 
	pMapFacp = pNewTable->GetFacpMap();
	pMapUnit = pNewTable->GetUnitMap();
	pNewMap = pNewTable->GetSystemData();
	pOldMap = pOldTable->GetSystemData();
	pIDMap = pNewTable->GetIDSystemData();

	pMapFacp->erase(pMapFacp->begin(), pMapFacp->end());
	pMapUnit->erase(pMapUnit->begin(), pMapUnit->end());

	pMapFacp->clear();
	pMapUnit->clear();
	for (it = pNewMap->begin(); it != pNewMap->end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pNewSys = it->second;
		if (pNewSys->GetSysData() == nullptr || pNewSys->GetDataType() == SE_RELAY)
			continue;
		strKey = it->first;
		(*pNewMap)[strKey] = nullptr;
		//pNewMap->erase(strKey);
		switch (pNewSys->GetDataType())
		{
		case SE_FACP:
			pNewFacp = (CDataFacp *)pNewSys->GetSysData();
			strIDKey = GF_GetIDSysDataKey(SE_FACP, pNewFacp->GetFacpID());
			pIDMap->erase(strIDKey);

		case SE_UNIT:
			pNewUnit = (CDataUnit *)pNewSys->GetSysData();
			strIDKey = GF_GetIDSysDataKey(SE_UNIT, pNewUnit->GetFacpID(), pNewUnit->GetUnitID());
			pIDMap->erase(strIDKey);
			break;
		case SE_CHANNEL:
			pNewChn = (CDataChannel *)pNewSys->GetSysData();
			strIDKey = GF_GetIDSysDataKey(SE_CHANNEL, pNewChn->GetFacpID(), pNewChn->GetUnitID());
			pIDMap->erase(strIDKey);
			break;
		default:
			continue;
		}
		delete pNewSys;
	}

	
	for (it = pOldMap->begin(); it != pOldMap->end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pOldSys = it->second;
		if (pOldSys->GetSysData() == nullptr || pOldSys->GetDataType() == SE_RELAY)
			continue;
		//strKey = GF_GetSysDataKey(pOldSys->GetDataType(), pNewFacp->GetFacpNum());
		strKey = pOldSys->GetKey();
		pNewSys = (*pNewMap)[strKey];
		
		if (pNewSys == nullptr)
		{
			pNewSys = new CDataSystem;
			(*pNewMap)[strKey] = pNewSys;
		}
		pNewSys->CopyData(pOldSys);

		switch (pOldSys->GetDataType())
		{
		case SE_FACP:
			pNewFacp = (CDataFacp *)pNewSys->GetSysData();
			strIDKey = GF_GetIDSysDataKey(SE_FACP, pNewFacp->GetFacpID());
			(*pIDMap)[strIDKey] = pNewSys;
			(*pMapFacp)[pNewFacp->GetFacpNum()] = pNewFacp;
			break;
		case SE_UNIT:
			pNewUnit = (CDataUnit *)pNewSys->GetSysData();
			strIDKey = GF_GetIDSysDataKey(SE_UNIT, pNewUnit->GetFacpID() , pNewUnit->GetUnitID());
			(*pIDMap)[strIDKey] = pNewSys;
			strtemp.Format(L"%02d.%02d", pOldTable->CvtFacpIDToNum(pNewUnit->GetFacpID()), pNewUnit->GetUnitNum());
			(*pMapUnit)[strtemp] = pNewUnit;

			break;
		case SE_CHANNEL:
			pNewChn = (CDataChannel *)pNewSys->GetSysData();
			strIDKey = GF_GetIDSysDataKey(SE_CHANNEL, pNewChn->GetFacpID(), pNewChn->GetUnitID());
			(*pIDMap)[strIDKey] = pNewSys;
			break;
		default:
			continue; 
		}
		pNewSys->CopyData(pOldSys);
	}

	return 1;
}

int CFormLoadRelayTable::DeleteOldItemAndAddNewItem(YAdoDatabase * pDB
	, CRelayTableData * pNewTable)
{
	CRUpdateItem * pItem;
	CDataSystem *pSys;
	CDataDevice *pDevNew;
	int i, nCnt; 
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	int nFId, nUId, nCId;
	CDataEquip *pInput, *pOut, *pContents, *pEqName; 
	CDataLocBase * pLoc;
	CString strSql, str;
	CMapSystemData * pMap;
	strSql = L"DELETE FROM TB_RELAY_LIST ";
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	pMap = pNewTable->GetSystemData();

// 	nCnt = m_vtOnlyOld.size();
// 	for (i = 0; i < nCnt; i++)
// 	{
// 		pItem = m_vtOnlyOld[i];
// 		if (pItem == nullptr || pItem->m_pOldCopyDev == nullptr)
// 			continue; 
// 		pDevOld = pItem->m_pOldCopyDev;
// // 		strSql.Format(L"DELETE FROM TB_RELAY_LIST "
// // 			L"WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
// // 			,pDevOld->GetFacpID() , pDevOld->GetUnitID() , pDevOld->GetChnID(),pDevOld->GetDeviceID()
// // 		);
// // 		if (pDb->ExecuteSql(strSql) == FALSE)
// // 			return 0;
// 		pMemDev = pNewTable->GetDevice(pDevOld->GetDevKey());
// 		if (pMemDev != nullptr)
// 		{
// 			delete pMemDev;
// 			pMemDev = nullptr;
// 		}
// 		pNewTable->DeleteDeviceInMemory(pDevOld);
// 	}

	nCnt = m_vtOnlyNew.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtOnlyNew[i];
		if (pItem == nullptr || pItem->m_pNewCopyDev == nullptr)
			continue;
		pDevNew = pItem->m_pNewCopyDev;
		
		nInput = nOut = nContents = 0;
		nBuild = nBtype = nStair = nFloor = nRoom = 0;
		pInput = pDevNew->GetEqInput();
		pOut = pDevNew->GetEqOutput();
		pContents = pDevNew->GetEqOutContents();
		pEqName = pDevNew->GetEqName();
		if (pInput)
			nInput = (int)pInput->GetEquipID();
		if (pOut)
			nOut = (int)pOut->GetEquipID();
		if (pContents)
			nContents = (int)pContents->GetEquipID();
		if (pEqName)
			nEqID = (int)pEqName->GetEquipID();
		pLoc = pDevNew->GetDevInputLocation();
		if (pLoc)
		{
			nBuild = (int)pLoc->GetBuildID();
			nBtype = (int)pLoc->GetBTypeID();
			nStair = (int)pLoc->GetStairID();
			nFloor = (int)pLoc->GetFloorID();
			nRoom = (int)pLoc->GetRoomID();
		}
		nFId = nUId = nCId = 0;

		if (MakeNewIDs(pNewTable, nFId, nUId, nCId
			, pDevNew->GetFacpNum(), pDevNew->GetUnitNum(), pDevNew->GetChnNum()) == FALSE)
			return 0; 

		pSys = pNewTable->AddSystemDeviceDataByNum(pDevNew->GetFacpNum()
			, pDevNew->GetUnitNum(), pDevNew->GetChnNum(), pDevNew->GetDeviceNum()
			, pLoc, pLoc, pInput, pOut, pContents, nullptr, pEqName
			, pDevNew->GetEqAddIndex(), pDevNew->GetRepeatorNum()
			, 0
			, nullptr, nullptr, nullptr
			, pNewTable->GetNewRelayIndex()
		);
		if (pSys)
			pNewTable->IncreaseRelayIndex();
// 
// 		if (pSys == nullptr || pSys->GetSysData() == nullptr)
// 			return 0;
// 		pMemDev = (CDataDevice *)pSys->GetSysData();
// 
// 		strSql.Format(L"INSERT INTO TB_RELAY_LIST(NET_ID,FACP_ID,UNIT_ID"
// 			L",CHN_ID,RLY_ID,RLY_NUM "
// 			L",MAP_KEY"
// 			L",RPT_NUM,INPUT_ID"
// 			L",OUTPUT_ID "
// 			L",BD_ID,BTYPE_ID"
// 			L",STAIR_ID,FL_ID"
// 			L",RM_ID,OUTCONTENTS_ID"
// 			L",EQ_ID , EQ_ADD_IDX , ADD_USER) "
// 			L" VALUES(1,%d,%d"
// 			L",%d,%d,%d"
// 			L",'%s'"
// 			L",%d,%d"
// 			L",%d"
// 			L",%d,%d"
// 			L",%d,%d"
// 			L",%d,%d"
// 			L",%d,'%s','ADMIN') "
// 			, pMemDev->GetFacpID(), pMemDev->GetUnitID()
// 			, pMemDev->GetChnID(), pMemDev->GetDeviceID(), pMemDev->GetDeviceNum()
// 			, pMemDev->GetDevKey()
// 			, pMemDev->GetRepeatorNum(), nInput
// 			, nOut
// 			, nBuild, nBtype
// 			, nStair, nFloor
// 			, nRoom, nContents
// 			, nEqID, pMemDev->GetEqAddIndex()
// 		);
// 
// 		if (pDB->ExecuteSql(strSql) == FALSE)
// 			return 0;

	}


	return 1;
}

BOOL CFormLoadRelayTable::MakeNewIDs(CRelayTableData* pTable
	, int& nFId, int& nUId, int& nCId
	, int nFnum , int nUnum , int nCnum
)
{
	CDataSystem * pSys;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	nFId = pTable->CvtFacpNumToID(nFnum);
	if (nFId <= 0)
	{
		pSys = pTable->AddSystemFacpDataByNum(nFnum);
		if (pSys == nullptr || pSys->GetSysData() == nullptr)
			return FALSE;
		pFacp = (CDataFacp*)pSys->GetSysData();
		nFId = pFacp->GetFacpID();
	}

	nUId = pTable->CvtUnitNumToID(nFnum, nUnum);
	if (nUId <= 0)
	{
		pSys = pTable->AddSystemUnitDataByNum(nFnum, nUnum);
		if (pSys == nullptr || pSys->GetSysData() == nullptr)
			return FALSE;
		pUnit = (CDataUnit *)pSys->GetSysData();
		nUId = pUnit->GetUnitID();
	}

	nCId = pTable->CvtChannelNumToID(nFnum, nUnum, nCnum);
	if (nCId <= 0)
	{
		pSys = pTable->AddSystemChannelDataByNum(nFnum, nUnum, nCnum);
		if (pSys == nullptr || pSys->GetSysData() == nullptr)
			return FALSE;
		pChn = (CDataChannel*)pSys->GetSysData();
		nCId = pChn->GetChnID();
	}

// 	nDId = pTable->CvtRelayNumToID(nFnum, nUnum, nCnum, nDnum);
// 	if (nDId <= 0)
// 	{
// 		pSys = pTable->AddSystemDeviceDataByNum(nFnum);
// 		if (pSys == nullptr || pSys->GetSysData())
// 			return FALSE;
// 		pDev = (CDataDevice*)pSys->GetSysData();
// 		nDId = pDev->GetFacpID();
// 	}
	return TRUE;
}


int CFormLoadRelayTable::ChangeDuplicationItem(YAdoDatabase * pDB, CRelayTableData * pNewTable)
{
	// 1. ���� ��ũ  ?
	CRUpdateItem * pItem;
	CDataSystem *pSys;
	CDataDevice *pDevNew, *pMemDev;
	int i, nCnt;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	int nFId, nUId, nCId;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	CDataLocBase * pLoc;
	CString strSql, str;
	CMapSystemData * pMap;
	pMap = pNewTable->GetSystemData();

	nCnt = m_vtIntersection.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtIntersection[i];
		if (pItem == nullptr || pItem->m_pNewCopyDev == nullptr)
			continue;
		pDevNew = pItem->m_pNewCopyDev;

		nInput = nOut = nContents = 0;
		nBuild = nBtype = nStair = nFloor = nRoom = 0;
		pInput = pDevNew->GetEqInput();
		pOut = pDevNew->GetEqOutput();
		pContents = pDevNew->GetEqOutContents();
		pEqName = pDevNew->GetEqName();
		if (pInput)
			nInput = (int)pInput->GetEquipID();
		if (pOut)
			nOut = (int)pOut->GetEquipID();
		if (pContents)
			nContents = (int)pContents->GetEquipID();
		if (pEqName)
			nEqID = (int)pEqName->GetEquipID();
		pLoc = pDevNew->GetDevInputLocation();
		if (pLoc)
		{
			nBuild = (int)pLoc->GetBuildID();
			nBtype = (int)pLoc->GetBTypeID();
			nStair = (int)pLoc->GetStairID();
			nFloor = (int)pLoc->GetFloorID();
			nRoom = (int)pLoc->GetRoomID();
		}
		nFId = nUId = nCId = 0;
		if (MakeNewIDs(pNewTable, nFId, nUId, nCId
			, pDevNew->GetFacpNum(), pDevNew->GetUnitNum(), pDevNew->GetChnNum()) == FALSE)
			return 0;

		pSys = pNewTable->AddSystemDeviceDataByNum(pDevNew->GetFacpNum()
			, pDevNew->GetUnitNum(), pDevNew->GetChnNum(), pDevNew->GetDeviceNum()
			, pLoc, pLoc, pInput, pOut, pContents, nullptr, pEqName
			, pDevNew->GetEqAddIndex(), pDevNew->GetRepeatorNum()
			, 0
			, nullptr, nullptr, nullptr
			, pDevNew->GetRelayIndex()
		);
// 
 		if (pSys == nullptr || pSys->GetSysData() == nullptr)
 			return 0;
 		pMemDev = (CDataDevice *)pSys->GetSysData();
		pDevNew->CopyExceptLink(pMemDev);
// 
// 		strSql.Format(L"INSERT INTO TB_RELAY_LIST(NET_ID,FACP_ID,UNIT_ID"
// 			L",CHN_ID,RLY_ID,RLY_NUM "
// 			L",MAP_KEY"
// 			L",RPT_NUM,INPUT_ID"
// 			L",OUTPUT_ID "
// 			L",BD_ID,BTYPE_ID"
// 			L",STAIR_ID,FL_ID"
// 			L",RM_ID,OUTCONTENTS_ID"
// 			L",EQ_ID , EQ_ADD_IDX , ADD_USER) "
// 			L" VALUES(1,%d,%d"
// 			L",%d,%d,%d"
// 			L",'%s'"
// 			L",%d,%d"
// 			L",%d"
// 			L",%d,%d"
// 			L",%d,%d"
// 			L",%d,%d"
// 			L",%d,'%s','ADMIN') "
// 			, pMemDev->GetFacpID(), pMemDev->GetUnitID()
// 			, pMemDev->GetChnID(), pMemDev->GetDeviceID(), pMemDev->GetDeviceNum()
// 			, pMemDev->GetDevKey()
// 			, pMemDev->GetRepeatorNum(), nInput
// 			, nOut
// 			, nBuild, nBtype
// 			, nStair, nFloor
// 			, nRoom, nContents
// 			, nEqID, pMemDev->GetEqAddIndex()
// 		);
// 
// 		if (pDb->ExecuteSql(strSql) == FALSE)
// 			return 0;

	}


	return 1;
}

int CFormLoadRelayTable::InsertNewData(YAdoDatabase * pDB, CRelayTableData * pNewTable)
{
	CMapSystemData::iterator it;
	CDataSystem * pData;
	CDataDevice * pDev;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataLocBase * pLoc;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nType ;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	LPVOID pTemp;
	CString strSql = L"",strtemp= L"";
	int nCnt = 0;
	CMapSystemData *pMap = pNewTable->GetSystemData();

	strSql = L"DELETE FROM TB_RELAY_LIST ";
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	for (it = pMap->begin(); it != pMap->end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		nType = pData->GetDataType();
		pTemp = pData->GetSysData();
		if (pTemp == nullptr)
			continue;
		nInput = nOut = nContents = nEqID = 0;
		strtemp = L"";
		switch (nType)
		{
		case SE_FACP:
			pFacp = (CDataFacp *)pTemp;
			strtemp.Format(L"INSERT INTO TB_FACP(NET_ID,FACP_ID,FACP_NUM,FACP_TYPE,FACP_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,'%s','ADMIN') ;\n"
				, pFacp->GetFacpID(), pFacp->GetFacpNum(), pFacp->GetFacpType(), pFacp->GetFacpName()
			);
			pDB->ExecuteSql(strtemp);
			continue; 
		case SE_UNIT:
			pUnit = (CDataUnit *)pTemp;
			//nUNum = CvtUnitNumToID(pUnit->GetFacpID(), pUnit->GetUnitID());
			strtemp.Format(L"INSERT INTO TB_UNIT(NET_ID,FACP_ID,UNIT_ID,UNIT_NUM,UNIT_TYPE,UNIT_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,%d,'%s','ADMIN') ;\n"
				, pUnit->GetFacpID(), pUnit->GetUnitID(), pUnit->GetUnitNum(), pUnit->GetUnitType()
				, pUnit->GetUnitName()
			);
			pDB->ExecuteSql(strtemp);
			continue;
		case SE_CHANNEL:
			pChn = (CDataChannel *)pTemp;
			strtemp.Format(L"INSERT INTO TB_CHANNEL(NET_ID,FACP_ID,UNIT_ID,CHN_ID,CHN_NUM,CHN_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,%d,"
				L"'%s','ADMIN') ;\n"
				, pChn->GetFacpID(), pChn->GetUnitID(), pChn->GetChnID(), pChn->GetChnNum()
				, pChn->GetChnName()
			);
			pDB->ExecuteSql(strtemp);
			continue;
		case SE_RELAY:
			pDev = (CDataDevice *)pTemp;
			nInput = nOut = nContents = nEqID = 0;
			nInput = nOut = nContents = 0;
			nBuild = nBtype = nStair = nFloor = nRoom = 0;
			pInput = pDev->GetEqInput();
			pOut = pDev->GetEqOutput();
			pContents = pDev->GetEqOutContents();
			pEqName = pDev->GetEqName();
			if (pInput)
				nInput = pInput->GetEquipID();
			if (pOut)
				nOut = pOut->GetEquipID();
			if (pContents)
				nContents = pContents->GetEquipID();
			if (pEqName)
				nEqID = pEqName->GetEquipID();
			pLoc = pDev->GetDevInputLocation();
			if (pLoc)
			{
				nBuild = pLoc->GetBuildID();
				nBtype = pLoc->GetBTypeID();
				nStair = pLoc->GetStairID();
				nFloor = pLoc->GetFloorID();
				nRoom = pLoc->GetRoomID();
			}

			strtemp.Format(L"INSERT INTO TB_RELAY_LIST(NET_ID,RIDX,FACP_ID,UNIT_ID"
				L",CHN_ID,RLY_ID,RLY_NUM "
				L",MAP_KEY"
				L",RPT_NUM,INPUT_ID"
				L",OUTPUT_ID"
				L",BD_ID,BTYPE_ID"
				L",STAIR_ID,FL_ID"
				L",RM_ID,OUTCONTENTS_ID"
				L",EQ_ID , EQ_ADD_IDX , ADD_USER) "
				L" VALUES(1,%d,%d,%d"
				L",%d,%d,%d"
				L",'%s'"
				L",%d,%d"
				L",%d"
				L",%d,%d"
				L",%d,%d"
				L",%d,%d"
				L",%d,'%s','%s') ;\n"
				, pDev->GetRelayIndex()
				, pDev->GetFacpID(), pDev->GetUnitID()
				, pDev->GetChnID(), pDev->GetDeviceID(), pDev->GetDeviceNum()
				, pDev->GetDevKey()
				, pDev->GetRepeatorNum(), nInput
				, nOut
				, nBuild, nBtype
				, nStair, nFloor
				, nRoom, nContents
				, nEqID, pDev->GetEqAddIndex()
				, pNewTable->GetCurrentUser()
			);
			TRACE(strtemp);
			strSql += strtemp;
			nCnt ++;
			if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				if(pDB->ExecuteSql(strSql) == FALSE)
				{
					USERLOG(L"QUERY ���� ����");
					if(nType == SE_RELAY)
						return 0;
				}
				strSql = L"";
				nCnt = 0;
			}
			break;
		default:
			continue;
		}
		
		
		
	}

	if(nCnt > 0)
	{
		if(pDB->ExecuteSql(strSql) == FALSE)
		{
			USERLOG(L"QUERY ���� ����");
			if(nType == SE_RELAY)
				return 0;
		}
	}
	return 1;
}


int CFormLoadRelayTable::AutoMakeLink()
{
	
	return 1;
}

int CFormLoadRelayTable::NormalItemAddManualLink(YAdoDatabase * pDB , CRelayTableData * pNewTable)
{
	POSITION pos;
	CDataLinked * pLnk , *pNewLnk;
	CDataDevice * pNewDev;
	CString strSql;
	pos = m_ptrManualLink.GetHeadPosition();
	while (pos)
	{
		pLnk = (CDataLinked *)m_ptrManualLink.GetNext(pos);
		if (pLnk == nullptr)
			continue; 
		strSql.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
			L", LINK_TYPE,LG_TYPE , LG_ID "
			L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
			L", ADD_USER) "
			L" VALUES(%d,%d,%d,%d"
			L", %d,%d,%d"
			L", %d,%d,%d,%d"
			L", '%s')"
			, pLnk->GetSrcFacp(), pLnk->GetSrcUnit(), pLnk->GetSrcChn(), pLnk->GetSrcDev()
			, pLnk->GetLinkType(), pLnk->GetLogicType(), pLnk->GetLogicID()
			, pLnk->GetTgtFacp(), pLnk->GetTgtUnit(), pLnk->GetTgtChn(), pLnk->GetTgtDev()
			, pNewTable->GetCurrentUser()
		);
		if (pDB->ExecuteSql(strSql) == FALSE)
			return 0;

		pNewDev = pNewTable->GetDeviceByID(pLnk->GetSrcFacp(), pLnk->GetSrcUnit(), pLnk->GetSrcChn(), pLnk->GetSrcDev());
		if (pNewDev != nullptr)
		{
			pNewLnk = new CDataLinked;
			pNewLnk->CopyData(pLnk);
			pNewDev->AddLink(FALSE, pNewLnk);
		}
		

	}
	return 1;
}


int CFormLoadRelayTable::InputUsingDuplicateItems(YAdoDatabase * pDB
	, CRelayTableData * pNewTable, CRelayTableData * pOldTable)
{
	//////////////////////////////////////////////////////////////////////////
	// 1. �ߺ��Ǵ� ȸ�θ� ����ϴ� ���
	// 1.1. �̸��� ��ġ�Ҷ��� ���������� ID����
	// 1.2. �ּ��϶� ����
	// m_ptrUSINGDupList
	POSITION pos,savePos;
	CRTableUpdate *pUpdate;
	CRUpdateItem * pItem;
//	CDataLinked *pOldLnk;
	CDataDevice * pDevNew,*pDevSrcOld, *pDevSource;
	CString strSql,strWhere,str,strtemp;
	int nUCnt = 0;
	int i = 0,nCnt , nRCnt; 
	pos = m_ptrUSINGDupList.GetHeadPosition();
	if (m_nCompareType == CMP_NAME)
	{
		// ID ���� old to new
		while (pos)
		{
			savePos = pos;
			pUpdate = (CRTableUpdate *)m_ptrUSINGDupList.GetNext(pos);
			if (pUpdate == nullptr 
				|| pUpdate->m_pOldSourceCopyDev == nullptr || pUpdate->m_pNewSourceCopyDev == nullptr)
				continue; 
			// 1. Memory ���� ,
			pDevSource = pNewTable->GetDevice(pUpdate->m_pNewSourceCopyDev->GetDevKey());
			pDevSrcOld = pOldTable->GetDevice(pUpdate->m_pOldSourceCopyDev->GetDevKey());
				// Source�� �ߺ��������� ����(SQL ������ ���ܽ�Ŵ) - source�� id�� ����ȵ�
			// Taget�� ���̵�� ���� ���Ѿ� �� - m_vtIntersection���� ���̵� �����´�.
			nCnt = pUpdate->m_vtUpdate.size();
			for (i = 0; i < nCnt; i++)
			{
				pItem = pUpdate->m_vtUpdate[i];
				if (pItem == nullptr || pItem->m_pNewCopyDev == nullptr || pItem->m_pOldCopyDev == nullptr)
					continue; 
				pDevNew = pNewTable->GetDevice(pItem->m_pNewCopyDev->GetDevKey());
				str = L"SELECT    * FROM TB_LINK_RELAY ";
				// ����Ǳ� �� ��ũ�ִ��� Ȯ��
				strWhere.Format(L" WHERE LINK_TYPE=%d "
					L" AND SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
					L" AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
					, LK_TYPE_RELEAY
					, pDevSource->GetFacpID(), pDevSource->GetUnitID(), pDevSource->GetChnID(), pDevSource->GetDeviceID()
					, pItem->m_pOldCopyDev->GetFacpID(), pItem->m_pOldCopyDev->GetUnitID(), pItem->m_pOldCopyDev->GetChnID(), pItem->m_pOldCopyDev->GetDeviceID()
				);
				strSql = str + strWhere;
				TRACE(strSql + L"\n");
				if (pDB->OpenQuery(strSql) == FALSE)
					return 0;

				nRCnt = pDB->GetRecordCount();
				pDB->RSClose();
				if (nRCnt > 0)
				{
					//Update
					str.Format(L"UPDATE TB_LINK_RELAY "
						L" SET TGT_FACP = %d , TGT_UNIT = %d , TGT_CHN = %d , TGT_RLY = %d "
						, pDevNew->GetFacpID(), pDevNew->GetUnitID(), pDevNew->GetChnID(), pDevNew->GetDeviceID()
					);
					strSql = str + strWhere;
				}
				else
				{
					strSql.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
						L", LINK_TYPE,LG_TYPE , LG_ID "
						L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
						L", ADD_USER) "
						L" VALUES(%d,%d,%d,%d"
						L", %d,%d,%d"
						L", %d,%d,%d,%d"
						L", '%s')"
						, pDevSource->GetFacpID(), pDevSource->GetUnitID(), pDevSource->GetChnID(), pDevSource->GetDeviceID()
						, LK_TYPE_RELEAY, LOGIC_MANUAL, 0
						, pDevNew->GetFacpID(), pDevNew->GetUnitID(), pDevNew->GetChnID(), pDevNew->GetDeviceID()
						, pOldTable->GetCurrentUser()
					);
				}
				TRACE(strSql + L"\n");
				if (pDB->ExecuteSql(strSql) == FALSE)
					return 0; 
				pDevSource->AddLink(FALSE, pDevNew, LK_TYPE_RELEAY, LOGIC_MANUAL, 0);
			}
		}
	}
	else
	{
		// ����
		while (pos)
		{
			savePos = pos;
			pUpdate = (CRTableUpdate *)m_ptrUSINGDupList.GetNext(pos);
			if (pUpdate == nullptr
				|| pUpdate->m_pOldSourceCopyDev == nullptr || pUpdate->m_pNewSourceCopyDev == nullptr)
				continue;
			// 1. Memory ���� ,
			pDevSource = pNewTable->GetDevice(pUpdate->m_pNewSourceCopyDev->GetDevKey());
			pDevSrcOld = pOldTable->GetDevice(pUpdate->m_pOldSourceCopyDev->GetDevKey());
			// Source�� �ߺ��������� ����(SQL ������ ���ܽ�Ŵ) - source�� id�� ����ȵ�
			// Taget�� ���̵�� ���� ���Ѿ� �� - m_vtIntersection���� ���̵� �����´�.
			nCnt = pUpdate->m_vtUpdate.size();
			for (i = 0; i < nCnt; i++)
			{
				pItem = pUpdate->m_vtUpdate[i];
				if (pItem == nullptr || pItem->m_pNewCopyDev == nullptr || pItem->m_pOldCopyDev == nullptr)
					continue;
				pDevNew = pNewTable->GetDevice(pItem->m_pNewCopyDev->GetDevKey());
				str = L"DELETE FROM TB_LINK_RELAY ";
				// ����Ǳ� �� ��ũ�ִ��� Ȯ��
				strWhere.Format(L" WHERE LINK_TYPE=%d "
					L" AND SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
					L" AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
					, LK_TYPE_RELEAY
					, pDevSource->GetFacpID(), pDevSource->GetUnitID(), pDevSource->GetChnID(), pDevSource->GetDeviceID()
					, pItem->m_pOldCopyDev->GetFacpID(), pItem->m_pOldCopyDev->GetUnitID(), pItem->m_pOldCopyDev->GetChnID(), pItem->m_pOldCopyDev->GetDeviceID()
				);
				strSql = str + strWhere;
				TRACE(strSql + L"\n");
				if (pDB->ExecuteSql(strSql) == FALSE)
					return 0;
			}
		}
	}
	return 1;
}

int CFormLoadRelayTable::OutputUsedByDuplicateItems(YAdoDatabase * pDB
	, CRelayTableData * pNewTable, CRelayTableData * pOldTable)
{
	//////////////////////////////////////////////////////////////////////////
	// 2. �ߺ��Ǵ� ȸ�ΰ� ����ϴ� ����������
	// 2.1. �̸��� ��ġ�Ҷ��� ������ ���������� ID����
	// 2.2. �ּ��϶� ����
	//m_ptrUSEDDupList
	POSITION pos , lPos;
	CRTableUpdate * pUpdate;
	CDataLinked *pOldLnk , *pNewLnk;
	CDataDevice  *pSrcNew ,*pSrcOld ,*pTgtNew; 
	CPtrList * pList;
	CString	str, strSql, strWhere;
	int nRCnt;
	pos = m_ptrUSEDDupList.GetHeadPosition();
	if (m_nCompareType == CMP_NAME)
	{
		while (pos)
		{
			pUpdate = (CRTableUpdate*)m_ptrUSEDDupList.GetNext(pos);
			if (pUpdate == nullptr 
				|| pUpdate->m_pOldSourceCopyDev == nullptr || pUpdate->m_pNewSourceCopyDev == nullptr)
				continue; 
			pSrcNew = pNewTable->GetDevice(pUpdate->m_pNewSourceCopyDev->GetDevKey());
			if (pSrcNew == nullptr)
				continue; 
			//pSrcOld = pUpdate->m_pOldSourceCopyDev;
			pList = pUpdate->m_pNewSourceCopyDev->GetLinkedList();
			lPos = pList->GetHeadPosition();
			while (lPos)
			{
				pOldLnk = (CDataLinked *)pList->GetNext(lPos);
				if (pOldLnk == nullptr)
					continue; 
				str = L"SELECT    * FROM TB_LINK_RELAY ";
				// ����Ǳ� �� ��ũ�ִ��� Ȯ��
				strWhere.Format(L" WHERE LINK_TYPE=%d AND LG_TYPE=%d "
					L" AND SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
					L" AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
					, pOldLnk->GetLinkType() , LOGIC_MANUAL
					, pOldLnk->GetSrcFacp(), pOldLnk->GetSrcUnit(), pOldLnk->GetSrcChn(), pOldLnk->GetSrcDev()
					, pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit(), pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev()
				);
				strSql = str + strWhere;
				if (pDB->OpenQuery(strSql) == FALSE)
					continue ;
				nRCnt = pDB->GetRecordCount();
				if (pOldLnk->GetLinkType() == LK_TYPE_PATTERN)
				{
					// �����Էµ� �� �߿� Pattern ������ ���� �߻�
					// ����1. ���� ������Ʈ�Ǵ� �϶�ǥ�� ���� �� --> ���� ������Ʈ�ϴ� �϶�ǥ�� ���� �߰�
					// ����2. ���� ������Ʈ �Ǵ� �϶�ǥ�� ���� ������ ������ �Ϻ� ȸ�θ� ���� �� - �� ���Ͽܿ� ���� ȸ�� �������� �߰�
					// ����3. ���� ������Ʈ �Ǵ� �϶�ǥ�� ���� ������ ������ �������� ���� ��� - ���� �����Ѵ�.

					// ���� ã�� --> �ִ°�� �������� ������ ���� �κ� �߰� ���ִ°� �³�?
					// 2021.02.17���� ������ Ʋ���� �´� ���� ���� ���� ��ȣ ����
					// ���� ���� ���� �� ����
					if (nRCnt > 0)
					{
						str.Format(L"UPDATE TB_LINK_RELAY "
							L" SET SRC_FACP = %d , SRC_UNIT = %d , SRC_CHN = %d , SRC_RLY = %d "
							, pSrcNew->GetFacpID(), pSrcNew->GetUnitID(), pSrcNew->GetChnID(), pSrcNew->GetDeviceID()
						);
						strSql = str + strWhere;
					}
					else
					{
						strSql.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
							L", LINK_TYPE,LG_TYPE , LG_ID "
							L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
							L", ADD_USER) "
							L" VALUES(%d,%d,%d,%d"
							L", %d,%d,%d"
							L", %d,%d,%d,%d"
							L", '%s')"
							, pSrcNew->GetFacpID(), pSrcNew->GetUnitID(), pSrcNew->GetChnID(), pSrcNew->GetDeviceID()
							, pOldLnk->GetLinkType(), LOGIC_MANUAL, 0
							, pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit(), pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev()
							, pOldTable->GetCurrentUser()
						);
					}

					if (pDB->ExecuteSql(strSql) == FALSE)
						return 0;

					pNewLnk = new CDataLinked;
					pNewLnk->CopyData(pOldLnk);
					pNewLnk->SetSrcFacp(pSrcNew->GetFacpID());
					pNewLnk->SetSrcUnit(pSrcNew->GetUnitID());
					pNewLnk->SetSrcChn(pSrcNew->GetChnID());
					pNewLnk->SetSrcDev(pSrcNew->GetDeviceID());
					pSrcNew->AddLink(FALSE, pNewLnk);
				}
				else
				{
					if (nRCnt > 0)
					{
						str.Format(L"UPDATE TB_LINK_RELAY "
							L" SET SRC_FACP = %d , SRC_UNIT = %d , SRC_CHN = %d , SRC_RLY = %d "
							, pSrcNew->GetFacpID(), pSrcNew->GetUnitID(), pSrcNew->GetChnID(), pSrcNew->GetDeviceID()
						);
						strSql = str + strWhere;
					}
					else
					{
						if (pOldLnk->GetLinkType() == LK_TYPE_RELEAY)
						{
							pTgtNew = ConvertUpdateItem(TRUE, pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit()
								, pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev());
							if (pTgtNew == nullptr)
								pTgtNew = pOldTable->GetDeviceByID(pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit()
									, pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev());
							strSql.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
								L", LINK_TYPE,LG_TYPE , LG_ID "
								L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
								L", ADD_USER) "
								L" VALUES(%d,%d,%d,%d"
								L", %d,%d,%d"
								L", %d,%d,%d,%d"
								L", '%s')"
								, pSrcNew->GetFacpID(), pSrcNew->GetUnitID(), pSrcNew->GetChnID(), pSrcNew->GetDeviceID()
								, LK_TYPE_RELEAY, LOGIC_MANUAL, 0
								, pTgtNew->GetFacpID(), pTgtNew->GetUnitID(), pTgtNew->GetChnID(), pTgtNew->GetDeviceID()
								, pOldTable->GetCurrentUser()
							);
						}
						else
						{
							strSql.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
								L", LINK_TYPE,LG_TYPE , LG_ID "
								L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
								L", ADD_USER) "
								L" VALUES(%d,%d,%d,%d"
								L", %d,%d,%d"
								L", %d,%d,%d,%d"
								L", '%s')"
								, pSrcNew->GetFacpID(), pSrcNew->GetUnitID(), pSrcNew->GetChnID(), pSrcNew->GetDeviceID()
								, pOldLnk->GetLinkType(), LOGIC_MANUAL, 0
								, pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit(), pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev()
								, pOldTable->GetCurrentUser()
							);
						}
					}
					if (pDB->ExecuteSql(strSql) == FALSE)
						return 0;

					pNewLnk = new CDataLinked;
					pNewLnk->CopyData(pOldLnk);
					pNewLnk->SetSrcFacp(pSrcNew->GetFacpID());
					pNewLnk->SetSrcUnit(pSrcNew->GetUnitID());
					pNewLnk->SetSrcChn(pSrcNew->GetChnID());
					pNewLnk->SetSrcDev(pSrcNew->GetDeviceID());
					pSrcNew->AddLink(FALSE, pNewLnk);
				}
				
			}
			
		}
	}
	else
	{
		while (pos)
		{
			pUpdate = (CRTableUpdate*)m_ptrUSEDDupList.GetNext(pos);
			if (pUpdate == nullptr
				|| pUpdate->m_pOldSourceCopyDev == nullptr || pUpdate->m_pNewSourceCopyDev == nullptr)
				continue;
			pSrcNew = pNewTable->GetDevice(pUpdate->m_pNewSourceCopyDev->GetDevKey());
			if (pSrcNew == nullptr)
				continue;
			pSrcOld = pUpdate->m_pOldSourceCopyDev;
			str = L"DELETE FROM TB_LINK_RELAY ";
			// ����Ǳ� �� ��ũ�ִ��� Ȯ��
			strWhere.Format(L" WHERE "
				L" SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
				, pSrcOld->GetFacpID(), pSrcOld->GetUnitID(), pSrcOld->GetChnID(), pSrcOld->GetDeviceID()
			);
			strSql = str + strWhere;
			if (pDB->ExecuteSql(strSql) == FALSE)
				return 0;
			pSrcNew->RemoveAllLink();
		}
	}
	return 1;
}

int CFormLoadRelayTable::UpdateOldTable()
{
	return 1;
}


int CFormLoadRelayTable::InsertLocationData(YAdoDatabase * pDB, CRelayTableData * pNewTable)
{
	CString strSql= L"",strtemp = L"";
	int nCnt = 0; 

	strSql = L"DELETE FROM TB_LOC_BUILDING ";
	pDB->ExecuteSql(strSql);
	strSql = L"DELETE FROM TB_LOC_BTYPE ";
	pDB->ExecuteSql(strSql);
	strSql = L"DELETE FROM TB_LOC_STAIR ";
	pDB->ExecuteSql(strSql);
	strSql = L"DELETE FROM TB_LOC_FLOOR ";
	pDB->ExecuteSql(strSql);
	strSql = L"DELETE FROM TB_LOC_ROOM ";
	pDB->ExecuteSql(strSql);

	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;
	CDataLocFloor * pFloor;
	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	CMapLocRoom::iterator rit;
	POSITION pos;
	CPtrList * pList;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocBase * pLoc = nullptr;
	spManager = pNewTable->GetLocationManager();

	pList = spManager->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		strtemp.Format(L"INSERT INTO TB_LOC_BUILDING(NET_ID, BD_ID,BD_NAME,BD_DESC,ADD_USER) "
			L" VALUES(1, %d,'%s','','%s') ;\n"
			,pBuild->GetBuildID(),pBuild->GetBuildName()
			,pNewTable->GetCurrentUser()
		);

		strSql += strtemp;
		nCnt ++;
		if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
		{
			if(pDB->ExecuteSql(strSql) == FALSE)
				return 0;
			nCnt = 0; 
			strSql = L"";
		}

		

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			strtemp.Format(L"INSERT INTO TB_LOC_BTYPE(NET_ID, BD_ID,BTYPE_ID,BTYPE_NAME,ADD_USER) "
				L" VALUES(1, %d,%d,'%s','ADMIN') ;\n"
				,pData->GetBuildID(),pData->GetBTypeID(),pData->GetBTypeName()
			);
			strSql += strtemp;
			nCnt ++;
			if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
			{
				if(pDB->ExecuteSql(strSql) == FALSE)
					return 0;
				nCnt = 0;
				strSql = L"";
			}
			pBType = (CDataLocBType*)pData;
			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;

#if _USE_STAIR_NUM_
				strtemp.Format(L"INSERT INTO TB_LOC_STAIR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,STAIR_NAME,STAIR_NUM,ADD_USER) "
					L" VALUES(1, %d,%d,%d,'%s',%d,'ADMIN') ;\n"
					,pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
					,pData->GetStairName(),pData->GetStairNum()
				);
#else
				strtemp.Format(L"INSERT INTO TB_LOC_STAIR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,STAIR_NAME,ADD_USER) "
					L" VALUES(1, %d,%d,%d,'%s','ADMIN') ;\n"
					,pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
					,pData->GetStairName()
				);
#endif
				strSql += strtemp;
				nCnt ++;
				if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
				{
					if(pDB->ExecuteSql(strSql) == FALSE)
						return 0;
					nCnt = 0;
					strSql = L"";
				}
				pStair = (CDataLocStair*)pData;

				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;

					strtemp.Format(L"INSERT INTO TB_LOC_FLOOR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,FL_NAME,FL_NUM,FL_MIDDLE,ADD_USER) "
						L" VALUES(1, %d,%d,%d,%d,'%s',%d,%d,'ADMIN') ;\n"
						,pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
						,pData->GetFloorID(),pData->GetFloorName(),pData->GetFloorNumber(),pData->GetMiddleFloor()
					);
					strSql += strtemp;
					nCnt ++;
					if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
					{
						if(pDB->ExecuteSql(strSql) == FALSE)
							return 0;
						nCnt = 0;
						strSql = L"";
					}
					pFloor = (CDataLocFloor*)pData;

					for(rit = pFloor->m_mapRoom.begin(); rit != pFloor->m_mapRoom.end(); rit++)
					{
						pData = rit->second;
						if(pData == nullptr)
							continue;
						strtemp.Format(L"INSERT INTO TB_LOC_ROOM(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,RM_ID,RM_NAME,ADD_USER) "
							L" VALUES(1, %d,%d,%d,%d,%d,'%s','ADMIN') "
							, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
							, pData->GetFloorID(), pData->GetRoomID(), pData->GetRoomName()
						);

						strSql += strtemp;
						nCnt ++;
						if(strSql.GetLength() > MAX_QUERY_STRING_SIZE)
						{
							if(pDB->ExecuteSql(strSql) == FALSE)
								return 0;
							nCnt = 0;
							strSql = L"";
						}
					}
				}
			}
		}
	}

	if(nCnt > 0)
	{
		if(pDB->ExecuteSql(strSql) == FALSE)
			return 0;
		nCnt = 0;
		strSql = L"";
	}

// 	int nType;
// 	for (it = pMap->begin(); it != pMap->end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		strSql = L"";
// 		switch (nType)
// 		{
// 		case LT_BUILD:
// 			strSql.Format(L"INSERT INTO TB_LOC_BUILDING(NET_ID, BD_ID,BD_NAME,BD_DESC,ADD_USER) "
// 				L" VALUES(1, %d,'%s','','%s') "
// 				, pData->GetBuildID(), pData->GetBuildName()
// 				, pNewTable->GetCurrentUser()
// 			);
// 			break;
// 		case LT_BTYPE:
// 			strSql.Format(L"INSERT INTO TB_LOC_BTYPE(NET_ID, BD_ID,BTYPE_ID,BTYPE_NAME,ADD_USER) "
// 				L" VALUES(1, %d,%d,'%s','ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetBTypeName()
// 			);
// 			break;
// 		case LT_STAIR:
// #if _USE_STAIR_NUM_
// 			strSql.Format(L"INSERT INTO TB_LOC_STAIR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,STAIR_NAME,STAIR_NUM,ADD_USER) "
// 				L" VALUES(1, %d,%d,%d,'%s',%d,'ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetStairName(), pData->GetStairNum()
// 			);
// #else
// 			strSql.Format(L"INSERT INTO TB_LOC_STAIR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,STAIR_NAME,ADD_USER) "
// 				L" VALUES(1, %d,%d,%d,'%s','ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetStairName()
// 			);
// #endif
// 			break;
// 		case LT_FLOOR:
// 			strSql.Format(L"INSERT INTO TB_LOC_FLOOR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,FL_NAME,FL_NUM,FL_MIDDLE,ADD_USER) "
// 				L" VALUES(1, %d,%d,%d,%d,'%s',%d,%d,'ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetFloorID(), pData->GetFloorName(), pData->GetFloorNumber(), pData->GetMiddleFloor()
// 			);
// 			break;
// 		case LT_ROOM:
// 			strSql.Format(L"INSERT INTO TB_LOC_ROOM(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,RM_ID,RM_NAME,ADD_USER) "
// 				L" VALUES(1, %d,%d,%d,%d,%d,'%s','ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetFloorID(), pData->GetRoomID(), pData->GetRoomName()
// 			);
// 			break;
// 		}
// 
// 		if (strSql == L"")
// 			continue;
// 
// 		if (pDB->ExecuteSql(strSql) == FALSE)
// 			return 0; 
// 
// 	}
	return 1;
}

BOOL CFormLoadRelayTable::CheckUpdateCondition()
{
	CRelayTableData * pOldTable;
	CDataPattern * pPtn;
	int nCnt = 0;
	std::shared_ptr<CManagerPattern> spOldMng,spNewMng;
	pOldTable = theApp.GetRelayTableData();
	if(pOldTable == nullptr)
		return false;
	spOldMng = pOldTable->GetPatternManager();

	if(spOldMng == nullptr)
		return false;

	POSITION pos = spOldMng->GetHeadPosition();
	
	if(pos == nullptr)
		return TRUE;

	while(pos)
	{
		pPtn = spOldMng->GetNext(pos);
		if(pPtn == nullptr)
			continue; 

		if(pPtn->GetManualMake() < 0)
			nCnt ++;
	}

	if(nCnt > 0)
		return FALSE;
	return TRUE;
}



int CFormLoadRelayTable::InsertNewEmBroadcast(YAdoDatabase * pDB,CRelayTableData * pNewTable)
{
	CString strSql = L"",strtemp = L"";
	std::shared_ptr<CManagerEmergency> spManager;
	POSITION pos;
	CDataEmBc *pData;
	strSql = L"DELETE FROM TB_EM_BC ";
	pDB->ExecuteSql(strSql);
	
	spManager = pNewTable->GetEmergencyManager();

	pos = spManager->GetHeadPosition();
	while(pos)
	{
		pData = (CDataEmBc*)spManager->GetNext(pos);
		if(pData == nullptr)
			continue;

		strSql.Format(L"INSERT TB_EM_BC(NET_ID,EM_ID,EM_ADDR , EM_NAME ,ADD_USER) "
			L" VALUES(1, %d,'%s','%s','%s')"
			,pData->GetEmID(),pData->GetEmAddr(),pData->GetEmName(),pNewTable->GetCurrentUser()
		);

		if(pDB->ExecuteSql(strSql) == FALSE)
		{
			//AfxMessageBox(L"�����͸� �߰��ϴµ� ���� �߽��ϴ�.");
			return 0;
		}

	}

	return 1;
}
