// FormLinkView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormLinkView.h"
#include "RelayTableData.h"
#include "DataLinked.h"
#include "DataDevice.h"
#include "DataPattern.h"
#include "DataEquip.h"
#include "DataPump.h"
#include "DataEmBc.h"
#include "../Common/Utils/ExcelWrapper.h"
#include "../Common/Utils/StdioFileEx.h"

#ifndef ENGLISH_MODE
TCHAR *szCol[]=
{
	L"입력회로",
	L"출력이름",
	L"입력타입",
	L"출력타입",
	L"설비명", 
	L"설비번호",
	L"출력설명",
	L"위치", 
	L"연동타입",
	L"출력종류",
	NULL
};
#else
TCHAR *szCol[] =
{
	L"INPUT CIRCUIT",
	L"OUTPUT NAME",
	L"INPUT TYPE",
	L"OUTPUT TYPE",
	L"EQUIPMENT NAME",
	L"EQUIPMENT NO.",
	L"OUTPUT DESCRIPTION",
	L"LOCATION",
	L"INTERLOCK TYPE",
	L"OUTPUT CATEGORY",
	NULL
};
#endif

IMPLEMENT_DYNCREATE(CFormLinkView, CFormView)

#ifndef ENGLISH_MODE
CFormLinkView::CFormLinkView()
	: CFormView(IDD_FORMLINKVIEW)
{
	m_pRefFasSysData = nullptr;
	m_pExcelThread = nullptr;
	m_hStop = nullptr;
	m_bStopFlag = FALSE;
	m_strFullPath = L"";
}
#else
CFormLinkView::CFormLinkView()
	: CFormView(IDD_FORMLINKVIEW_EN)
{
	m_pRefFasSysData = nullptr;
	m_pExcelThread = nullptr;
	m_hStop = nullptr;
	m_bStopFlag = FALSE;
	m_strFullPath = L"";
}
#endif

CFormLinkView::~CFormLinkView()
{
	RemoveAllData();
}

void CFormLinkView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICE_TREE, m_ctrlTree);
	DDX_Control(pDX, IDC_RELAY_LIST, m_ctrlRelay);
}

BEGIN_MESSAGE_MAP(CFormLinkView, CFormView)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_EXCEL, &CFormLinkView::OnBnClickedBtnExcel)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CFormLinkView::OnBnClickedBtnRefresh)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)

	ON_NOTIFY(TVN_SELCHANGED, IDC_DEVICE_TREE, &CFormLinkView::OnTvnSelchangedDeviceTree)
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(CSWM_PROGRESS_STEP, OnMakeProgress)
END_MESSAGE_MAP()


// CFormLinkView 진단입니다.

#ifdef _DEBUG
void CFormLinkView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormLinkView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormLinkView 메시지 처리기입니다.

LRESULT CFormLinkView::OnMakeProgress(WPARAM wp, LPARAM lp)
{
	CString str;
	int nP = 0;
// 	switch (lp)
// 	{
// 	case PROG_RESULT_CANCEL:
// 		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"사용자가 취소 했습니다.");
// 		m_ctrlProg.SetPos(100);
// 		break;
// 	case PROG_RESULT_ERROR:
// 		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"생성중 오류가 발생 했습니다.");
// 		m_ctrlProg.SetPos(100);
// 		break;
// 	case PROG_RESULT_STEP:
// 		nP = (int)(((float)wp / (float)m_nAllCnt) * 100);
// 		str.Format(L"[%d]%% 작업중(%d/%d)", nP, wp, m_nAllCnt);
// 		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
// 		m_ctrlProg.SetPos(nP);
// 		break;
// 	case PROG_RESULT_FINISH:
// 		str.Format(L"%d개의 연동데이터를 생성했습니다.", m_nAllCnt);
// 		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
// 		m_ctrlProg.SetPos(100);
// 		DisplayAutoMake();
// 		break;
// 	}
	return 1;
}

DWORD CFormLinkView::Thread_ExcelProc(LPVOID lpData)
{
	CFormLinkView	*me;
	int nRet = 0;
	me = (CFormLinkView *)lpData;
	if ((CFormLinkView *)NULL == me)
	{
		me->SendMessage(CSWM_PROGRESS_STEP, 0, 0);
		return 0;
	}

	try
	{
		nRet = me->GenerateLinkExcel();
	}
	catch (...)
	{
	}
	return 0;
}

int CFormLinkView::GenerateLinkExcel()
{
	int nIdx = 0 , i;
	CString strSheet , strtemp;
	CExcelWrapper xls;
	if (xls.Create() == false)
		return 0;
#ifndef ENGLISH_MODE
	strSheet.Format(L"%s 연동출력표", m_pRefFasSysData->GetPrjName());
#else
	strSheet.Format(L"%s Interlock Output Table", m_pRefFasSysData->GetPrjName());
#endif
	xls.SetSheetName(1, strSheet);

	for (i = 0; szCol[i] != nullptr; i++)
		xls.SetItemText(1, i + 1, szCol[i]);

	WriteExcel(TVI_ROOT, &xls, 2);

	xls.SaveCopyAs(m_strFullPath);
	xls.Close();

	//for (it = m_MapAuto.begin(); it != m_MapAuto.end(); it++)
	{
		SendMessage(CSWM_PROGRESS_STEP, nIdx, PROG_RESULT_STEP);
	}
	SendMessage(CSWM_PROGRESS_STEP, nIdx, PROG_RESULT_FINISH);
#ifndef ENGLISH_MODE
	strtemp.Format(L"다음파일명으로\n%s\n저장되었습니다.", m_strFullPath);
#else
	strtemp.Format(L"Saved as follows in the\n%s:", m_strFullPath);
#endif
	AfxMessageBox(strtemp);
	return 1;
}


void CFormLinkView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CBitmap bmp;
	if (!bmp.LoadBitmap(IDB_BMP_DEVICE_ICON))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), IDB_BMP_DEVICE_ICON);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ImgDeviceIcons.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ImgDeviceIcons.Add(&bmp, RGB(0, 255, 255));

	m_ctrlTree.SetImageList(&m_ImgDeviceIcons, TVSIL_NORMAL);

#ifndef ENGLISH_MODE
	m_ctrlRelay.InsertColumn(0, L"출력이름", LVCFMT_LEFT, 300);
	m_ctrlRelay.InsertColumn(1, L"입력타입", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(2, L"출력타입", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(3, L"설비명", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(4, L"설비번호", LVCFMT_LEFT, 50);
	m_ctrlRelay.InsertColumn(5, L"출력설명", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(6, L"위치", LVCFMT_LEFT, 150);
	m_ctrlRelay.InsertColumn(7, L"연동타입", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(8, L"출력종류", LVCFMT_LEFT, 80);
#else
	m_ctrlRelay.InsertColumn(0, L"OUTPUT NAME", LVCFMT_LEFT, 300);
	m_ctrlRelay.InsertColumn(1, L"INPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(2, L"OUTPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(3, L"EQUIPMENT NAME", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(4, L"EQUIPMENT NO.", LVCFMT_LEFT, 50);
	m_ctrlRelay.InsertColumn(5, L"OUTPUT DESCRIPTION", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(6, L"LOCATION", LVCFMT_LEFT, 150);
	m_ctrlRelay.InsertColumn(7, L"INTERLOCK TYPE", LVCFMT_LEFT, 80);
	m_ctrlRelay.InsertColumn(8, L"OUTPUT TYPE", LVCFMT_LEFT, 80);
#endif

// 	m_ctrlRelay.InsertColumn(0, _T("타입"), LVCFMT_LEFT, 80);
// 	m_ctrlRelay.InsertColumn(1, _T("감지기/중계기"), LVCFMT_LEFT, 300);
// 	m_ctrlRelay.InsertColumn(2, _T("입력타입"), LVCFMT_LEFT, 80);
// 	m_ctrlRelay.InsertColumn(3, _T("출력타입"), LVCFMT_LEFT, 80);
// 	m_ctrlRelay.InsertColumn(4, _T("설비명"), LVCFMT_LEFT, 80);
// 	m_ctrlRelay.InsertColumn(5, _T("번호"), LVCFMT_LEFT, 50);
// 	m_ctrlRelay.InsertColumn(6, _T("출력내용"), LVCFMT_LEFT, 80);
// 	m_ctrlRelay.InsertColumn(7, _T("위치"), LVCFMT_LEFT, 80);
	m_ctrlRelay.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_pRefFasSysData = theApp.GetRelayTableData();

	if (m_pRefFasSysData == nullptr)
		return; 

	m_pRefFasSysData->FillDeviceTree(&m_ctrlTree , DKPTREE_ADDRESS_VIEW, &m_ptrItemList,TRUE);

}


BOOL CFormLinkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::PreCreateWindow(cs);
}


int CFormLinkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CFormLinkView::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnClose();
}


void CFormLinkView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rc, rcTree, rcList;
	GetClientRect(&rc);
	rc.DeflateRect(5, 8, 5, 5);
	rcTree = rc;

	rcTree.top = 30;
	rcTree.right = rc.left + 300;

	rcList = rc;
	rcList.top = 30;

	rcList.left = rcTree.right + 5;
	if (m_ctrlTree.GetSafeHwnd())
		m_ctrlTree.MoveWindow(&rcTree);

	if (m_ctrlRelay.GetSafeHwnd())
		m_ctrlRelay.MoveWindow(&rcList);
}

void CFormLinkView::OnBnClickedBtnExcel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strPath,strPrjName , strtemp,strLine;
	int i = 0 , nCnt; 
#define _USE_LINKDATA_WRITE_EXCEL_ 0
#if _USE_LINKDATA_WRITE_EXCEL_
	CExcelWrapper xls;
	CString strDefault = L"*.xls", strFilter = L"연동 출력표(*.xls)|*.xls|All Files (*.*)|*.*||";
	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"프로젝트가 닫혀있습니다. 프로젝트를 여신 후 다시 실행 해 주십시요");
		return; 
	}

	nCnt = m_ctrlTree.GetCount();
	if (nCnt <= 0)
	{
		AfxMessageBox(L"저장할 데이터가 없습니다.");
		return;
	}
	CFileDialog FileDialog(FALSE, NULL, strDefault, OFN_HIDEREADONLY, strFilter, this);
	if (FileDialog.DoModal() != IDOK)
		return;

	strPath = FileDialog.GetPathName();

	strPrjName = m_pRefFasSysData->GetPrjName();
	strtemp.Format(L"%s 연동출력표", strPrjName);
	m_strFullPath = strtemp;
	if (xls.Create() == false)
		return ;

	// sheet 번호는 1부터
// 	if (xls.SetWorkSheetChange(1) == FALSE)
// 		return;
	
	xls.SetSheetName(1, strtemp);

	for( i =0 ;szCol[i] != nullptr ; i++)
		xls.SetItemText(1, i+1, szCol[i]);

	WriteExcel(TVI_ROOT, &xls, 2);

	xls.SaveCopyAs(strPath);
	xls.Close();
#else
	CStdioFileEx file;
	CFileException e;
	char szAsc[2048] = { 0 };
	CString strDefault = L"*.csv";
#ifndef ENGLISH_MODE
	CString strFilter = L"연동 출력표(*.csv)|*.csv|All Files (*.*)|*.*||";
#else
	CString strFilter = L"Interlock Output Table(*.csv)|*.csv|All Files (*.*)|*.*||";
#endif
	if (m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트가 닫혀있습니다. 프로젝트를 여신 후 다시 실행 해 주십시오.");
#else
		AfxMessageBox(L"The project is closed. Please open the project and run it again.");
#endif
		return;
	}

	nCnt = m_ctrlTree.GetCount();
	if (nCnt <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"저장할 데이터가 없습니다.");
#else
		AfxMessageBox(L"No data to save.");
#endif
		return;
	}
	strPrjName = m_pRefFasSysData->GetPrjName();
#ifndef ENGLISH_MODE
	strtemp.Format(L"%s 연동출력표.csv", strPrjName);
#else
	strtemp.Format(L"%s Interlock Output Table.csv", strPrjName);
#endif

	CFileDialog FileDialog(FALSE, NULL, strtemp, OFN_HIDEREADONLY, strFilter, this);
	if (FileDialog.DoModal() != IDOK)
		return;

	strPath = FileDialog.GetPathName();

	m_strFullPath = strPath;
	if (!file.Open(m_strFullPath, CFile::modeCreate | CFile::modeWrite))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
#ifndef ENGLISH_MODE
		GF_AddLog(L"연동출력표 파일을 생성하는데 실패했습니다.\n");
		AfxMessageBox(L"연동출력표 파일을 생성하는데 실패했습니다");
#else
		GF_AddLog(L"Failed to generate an interlock output table file.\n");
		AfxMessageBox(L"Failed to generate an interlock output table file.");
#endif
		return ;
	}

	strLine = L"";
	for (i = 0; szCol[i] != nullptr; i++)
	{
		if (i == 0)
			strLine = szCol[i];
		else
		{
			strLine += L",";
			strLine += szCol[i];
		}
	}
	strLine += L"\r\n";
	//GF_Unicode2ASCII(strLine.GetBuffer(), szAsc, 2048);
	file.WriteString(strLine);

	WriteCSV(TVI_ROOT, &file);
	file.Close();
#endif

#ifndef ENGLISH_MODE
	strtemp.Format(L"다음파일명으로\n%s\n저장되었습니다.",strPath);
#else
	strtemp.Format(L"Saved as follows in the\n%s:", strPath);
#endif
	AfxMessageBox(strtemp);
}


int CFormLinkView::WriteExcel(HTREEITEM hItem, CExcelWrapper * pXls,int nStartRow)
{
	CString strText;
	CDataSystem * pSys;
	CDataDevice * pDev;
	ST_TREEITEM * pItem;
	BOOL bIncludeCheckItem = FALSE, bTemp = FALSE;
	HTREEITEM hChild = nullptr;

	if(hItem != nullptr)
		hChild = m_ctrlTree.GetChildItem(hItem);
	else
	{
		// [KHS 2020-2-7 16:41:14] 
				// GetRootItem --> TVI_ROOT 변환
				// hChild = m_ctrlTree.GetRootItem();
		hChild = nullptr;
	}

	while (hChild != nullptr)
	{
		// Column은 1부터시작
		if (m_ctrlTree.ItemHasChildren(hChild))
		{
			nStartRow = WriteExcel(hChild, pXls, nStartRow);
			nStartRow++;
		}
		if (hChild == TVI_ROOT)
			pItem = nullptr;
		else
			pItem = (ST_TREEITEM *)m_ctrlTree.GetItemData(hChild);

		if (pItem == nullptr)
		{
			hChild = m_ctrlTree.GetNextSiblingItem(hChild);
			continue;
		}

		if (pItem->nDataType != TTYPE_DEV_DEVICE)
		{
			hChild = m_ctrlTree.GetNextSiblingItem(hChild);
			continue;
		}
		if (pItem->pData == nullptr)
		{
			hChild = m_ctrlTree.GetNextSiblingItem(hChild);
			continue;
		}
		pSys = (CDataSystem *)pItem->pData;
		if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		{
			hChild = m_ctrlTree.GetNextSiblingItem(hChild);
			continue;
		}
		pDev = (CDataDevice *)pSys->GetSysData();
		nStartRow = WriteExcelOutput(pDev,pXls,nStartRow);
		hChild = m_ctrlTree.GetNextSiblingItem(hChild);
	}
	return nStartRow;
}


int CFormLinkView::WriteExcelOutput(CDataDevice* pDev, CExcelWrapper * pXls, int nStartRow)
{
	int nIdx = 0;
	CString str;
	CDataDevice *pOut;
	CDataLinked * pLink;
	POSITION posDev;
	CPtrList * pDList;
	pDList = pDev->GetLinkedList();
	if (pDList == nullptr || pDList->GetCount() <= 0)
		return nStartRow;
	nIdx = nStartRow;
	// Relay
	posDev = pDList->GetHeadPosition();
	while (posDev)
	{
		pLink = (CDataLinked*)pDList->GetNext(posDev);
		if (pLink == nullptr)
			continue;

		if (pLink->GetLinkType() != LK_TYPE_RELEAY)
			continue;

		pOut = m_pRefFasSysData->GetDeviceByID(pLink->GetTgtFacp(), pLink->GetTgtUnit()
			, pLink->GetTgtChn(), pLink->GetTgtDev());

		if (pOut == nullptr)
			continue;
		// 		m_ctrlRelay.InsertItem(nIdx, pOut->GetOutContentsFullName());
		// 		m_ctrlRelay.SetItemText(nIdx, 1, pOut->GetInputTypeName());
		// 		m_ctrlRelay.SetItemText(nIdx, 2, pOut->GetOutputTypeName());
		// 		m_ctrlRelay.SetItemText(nIdx, 3, pOut->GetEquipName());
		// 		m_ctrlRelay.SetItemText(nIdx, 4, pOut->GetEqAddIndex());
		// 		m_ctrlRelay.SetItemText(nIdx, 5, pOut->GetOutContentsName());
		// 		m_ctrlRelay.SetItemText(nIdx, 6, pOut->GetOutputLocationName());
		// 		m_ctrlRelay.SetItemText(nIdx, 7, g_szLogicTypeString[pLink->GetLogicType()]);
		// 		m_ctrlRelay.SetItemText(nIdx, 8, L"감지기/중계기");

		pXls->SetItemText(nIdx, 1, pDev->GetInputFullName());
		pXls->SetItemText(nIdx, 2, pOut->GetOutContentsFullName());
		pXls->SetItemText(nIdx, 3, pOut->GetInputTypeName());
		pXls->SetItemText(nIdx, 4, pOut->GetOutputTypeName());
		pXls->SetItemText(nIdx, 5, pOut->GetEquipName());
		pXls->SetItemText(nIdx, 6, pOut->GetEqAddIndex());
		pXls->SetItemText(nIdx, 7, pOut->GetOutContentsName());
		pXls->SetItemText(nIdx, 8, pOut->GetOutputLocationName());
		pXls->SetItemText(nIdx, 9, g_szLogicTypeString[pLink->GetLogicType()]);
#ifndef ENGLISH_MODE
		pXls->SetItemText(nIdx, 10, L"감지기/중계기");
#else
		pXls->SetItemText(nIdx, 10, L"DETECTOR/MODULE");
#endif
		nIdx++;
	}

	// Pattern
	posDev = pDList->GetHeadPosition();
	while (posDev)
	{
		pLink = (CDataLinked*)pDList->GetNext(posDev);
		if (pLink == nullptr)
			continue;

		if (pLink->GetLinkType() == LK_TYPE_RELEAY)
			continue;
		nIdx = WriteExcelLink(pDev, pLink, pXls, nIdx, L"▶");
	}
	return nIdx;
}


int CFormLinkView::WriteExcelLink(CDataDevice * pInDev, CDataLinked *pLnk, CExcelWrapper * pXls, int nStartRow, CString strPrefix)
{
	int nIdx = 0;
	CString str;
	CDataDevice *pOut;
	CDataPattern * pPtn;
	CDataPump *pPump;
	CDataEmBc * pEm;
	POSITION posDev;
	CPtrList * pDList;
	CDataLinked * pData;
	nIdx = nStartRow;
	// Pattern
	pDList = nullptr;
	switch (pLnk->GetLinkType())
	{
	case LK_TYPE_RELEAY:
		pOut = m_pRefFasSysData->GetDeviceByID(pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
			, pLnk->GetTgtChn(), pLnk->GetTgtDev());

		if (pOut == nullptr)
			return nIdx;
		pXls->SetItemText(nIdx, 1, pInDev->GetInputFullName());
#ifndef ENGLISH_MODE
		str.Format(L"%s출력:%s", strPrefix, pOut->GetOutContentsFullName());
#else
		str.Format(L"%sOutput:%s", strPrefix, pOut->GetOutContentsFullName());
#endif
		strPrefix = str + L"▶";
		pXls->SetItemText(nIdx, 2, str);
		pXls->SetItemText(nIdx, 3, pOut->GetInputTypeName());
		pXls->SetItemText(nIdx, 4, pOut->GetOutputTypeName());
		pXls->SetItemText(nIdx, 5, pOut->GetEquipName());
		pXls->SetItemText(nIdx, 6, pOut->GetEqAddIndex());
		pXls->SetItemText(nIdx, 7, pOut->GetOutContentsName());
		pXls->SetItemText(nIdx, 8, pOut->GetOutputLocationName());
		pXls->SetItemText(nIdx, 9, g_szLogicTypeString[pLnk->GetLogicType()]);
#ifndef ENGLISH_MODE
		pXls->SetItemText(nIdx, 10, L"감지기/중계기");
#else
		pXls->SetItemText(nIdx, 10, L"DETECTOR/MODULE");
#endif
		nIdx++;
		break;
	case LK_TYPE_PATTERN:
		pPtn = m_pRefFasSysData->GetPattern(pLnk->GetTgtFacp());
		if (pPtn == nullptr)
			return nIdx;
		pXls->SetItemText(nIdx, 1, pInDev->GetInputFullName());
#ifndef ENGLISH_MODE
		str.Format(L"%s패턴:%s", strPrefix, pPtn->GetPatternName());
#else
		str.Format(L"%sPattern:%s", strPrefix, pPtn->GetPatternName());
#endif
		strPrefix = str + L"▶";
		//strPrefix = strPrefix + L"[" + pPtn->GetPatternName() + L"]";
		pXls->SetItemText(nIdx, 2, str);
		pXls->SetItemText(nIdx, 3, L"");
		pXls->SetItemText(nIdx, 4, L"");
		pXls->SetItemText(nIdx, 5, L"");
		pXls->SetItemText(nIdx, 6, L"");
		pXls->SetItemText(nIdx, 7, L"");
		str.Format(L"%d", pPtn->GetPatternID());
		pXls->SetItemText(nIdx, 8, str);
#ifndef ENGLISH_MODE
		pXls->SetItemText(nIdx, 9, L"패턴");
		pXls->SetItemText(nIdx, 10, L"패턴");
#else
		pXls->SetItemText(nIdx, 9, L"Pattern");
		pXls->SetItemText(nIdx, 10, L"Pattern");
#endif
		nIdx++;
		pDList = pPtn->GetPtnItemList();
		//WriteLink(pInDev,pLink)
		break;
	case LK_TYPE_EMERGENCY:
		pEm = m_pRefFasSysData->GetEmergency(pLnk->GetTgtFacp());
		if (pEm == nullptr)
			return nIdx;
#ifndef ENGLISH_MODE
		str.Format(L"%s비방:%s", strPrefix, pEm->GetEmName());
#else
		str.Format(L"%sPublic Address:%s", strPrefix, pEm->GetEmName());
#endif
		strPrefix = str + L"▶";
		pXls->SetItemText(nIdx, 1, pInDev->GetInputFullName());
		pXls->SetItemText(nIdx, 2, str);
		pXls->SetItemText(nIdx, 3, L"");
		pXls->SetItemText(nIdx, 4, L"");
		pXls->SetItemText(nIdx, 5, L"");
		pXls->SetItemText(nIdx, 6, L"");
		pXls->SetItemText(nIdx, 7, L"");
		str.Format(L"%d", pEm->GetEmID());
		pXls->SetItemText(nIdx, 8, str);
#ifndef ENGLISH_MODE
		pXls->SetItemText(nIdx, 9, L"비상방송");
		pXls->SetItemText(nIdx, 10, L"비상방송");
#else
		pXls->SetItemText(nIdx, 9, L"PUBLIC ADDRESS");
		pXls->SetItemText(nIdx, 10, L"PUBLIC ADDRESS");
#endif
		nIdx++;
		break;
	case LK_TYPE_PUMP:
		pPump = m_pRefFasSysData->GetPump(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
		if (pPump == nullptr)
			return nIdx;
		pXls->SetItemText(nIdx, 1, pInDev->GetInputFullName());
#ifndef ENGLISH_MODE
		str.Format(L"%s펌프:%s", strPrefix, pPump->GetPumpName());
#else
		str.Format(L"%sPump:%s", strPrefix, pPump->GetPumpName());
#endif
		strPrefix = str + L"▶";
		pXls->SetItemText(nIdx, 2, str);
		pXls->SetItemText(nIdx, 3, L"");
		pXls->SetItemText(nIdx, 4, L"");
		pXls->SetItemText(nIdx, 5, L"");
		pXls->SetItemText(nIdx, 6, L"");
		pXls->SetItemText(nIdx, 7, L"");
		str.Format(L"%d", pPump->GetPumpID());
		pXls->SetItemText(nIdx, 8, str);
#ifndef ENGLISH_MODE
		pXls->SetItemText(nIdx, 9, L"펌프");
		pXls->SetItemText(nIdx, 10, L"펌프");
#else
		pXls->SetItemText(nIdx, 9, L"PUMP");
		pXls->SetItemText(nIdx, 10, L"PUMP");
#endif
		nIdx++;
		pDList = pPump->GetLinkList();
		break;
	default:
		return nIdx;
	}

	if (pDList == nullptr)
		return nIdx;
	posDev = pDList->GetHeadPosition();
	while (posDev)
	{
		pData = (CDataLinked*)pDList->GetNext(posDev);
		if (pData == nullptr)
			continue;
		nIdx = WriteExcelLink(pInDev, pData, pXls, nIdx, strPrefix);
	}
	return nIdx;
}


int CFormLinkView::WriteCSV(HTREEITEM hItem, CStdioFileEx * pFile)
{
	CString strText;
	CDataSystem * pSys;
	CDataDevice * pDev;
	ST_TREEITEM * pItem;
	BOOL bIncludeCheckItem = FALSE, bTemp = FALSE;
	HTREEITEM hChild = nullptr;

	if (hItem != nullptr)
		hChild = m_ctrlTree.GetChildItem(hItem);
	else
	{
		// [KHS 2020-2-7 16:41:14] 
		// GetRootItem --> TVI_ROOT 변환
		// hChild = m_ctrlTree.GetRootItem();
		hChild = nullptr;
	}

	while (hChild != nullptr)
	{
		// Column은 1부터시작
		if (m_ctrlTree.ItemHasChildren(hChild))
		{
			WriteCSV(hChild, pFile);
		}
		if (hChild == TVI_ROOT)
			pItem = nullptr;
		else
			pItem = (ST_TREEITEM *)m_ctrlTree.GetItemData(hChild);

		if (pItem == nullptr)
		{
			hChild = m_ctrlTree.GetNextSiblingItem(hChild);
			continue;
		}

		if (pItem->nDataType != TTYPE_DEV_DEVICE)
		{
			hChild = m_ctrlTree.GetNextSiblingItem(hChild);
			continue;
		}
		if (pItem->pData == nullptr)
		{
			hChild = m_ctrlTree.GetNextSiblingItem(hChild);
			continue;
		}
		pSys = (CDataSystem *)pItem->pData;
		if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		{
			hChild = m_ctrlTree.GetNextSiblingItem(hChild);
			continue;
		}
		pDev = (CDataDevice *)pSys->GetSysData();

		WriteCSVOutput(pDev, pFile);
		hChild = m_ctrlTree.GetNextSiblingItem(hChild);
	}
	return 1;
}


int CFormLinkView::WriteCSVOutput(CDataDevice* pDev, CStdioFileEx * pFile)
{
	CString str,strLine;
	CDataDevice *pOut;
	CDataLinked * pLink;
	POSITION posDev;
	CPtrList * pDList;
	pDList = pDev->GetLinkedList();
	if (pDList == nullptr || pDList->GetCount() <= 0)
		return 1;
	// Relay
	posDev = pDList->GetHeadPosition();
	while (posDev)
	{
		pLink = (CDataLinked*)pDList->GetNext(posDev);
		if (pLink == nullptr)
			continue;

		if (pLink->GetLinkType() != LK_TYPE_RELEAY)
			continue;

		pOut = m_pRefFasSysData->GetDeviceByID(pLink->GetTgtFacp(), pLink->GetTgtUnit()
			, pLink->GetTgtChn(), pLink->GetTgtDev());

		if (pOut == nullptr)
			continue;
		strLine = L"";
		strLine = pOut->GetInputFullName(); strLine += L",";
		strLine += pOut->GetOutContentsFullName(); strLine += L",";
		strLine += pOut->GetInputTypeName(); strLine += L",";
		strLine += pOut->GetOutputTypeName(); strLine += L",";
		strLine += pOut->GetEquipName(); strLine += L",";
		strLine += pOut->GetEqAddIndex(); strLine += L",";
		strLine += pOut->GetOutContentsName(); strLine += L",";
		strLine += pOut->GetOutputLocationName(); strLine += L",";
		strLine += g_szLogicTypeString[pLink->GetLogicType()]; strLine += L",";
#ifndef ENGLISH_MODE
		strLine += L"감지기/중계기"; strLine += L"\r\n";
#else
		strLine += L"DETECTOR/MODULE"; strLine += L"\r\n";
#endif
		pFile->WriteString(strLine);
	}

	// Pattern
	posDev = pDList->GetHeadPosition();
	while (posDev)
	{
		pLink = (CDataLinked*)pDList->GetNext(posDev);
		if (pLink == nullptr)
			continue;

		if (pLink->GetLinkType() == LK_TYPE_RELEAY)
			continue;
		WriteCSVLink(pDev, pLink, pFile,L"▶");
	}
	return 1;
}


int CFormLinkView::WriteCSVLink(CDataDevice * pInDev, CDataLinked *pLnk, CStdioFileEx * pFile, CString strPrefix)
{
	CString str , strLine;
	CDataDevice *pOut;
	CDataPattern * pPtn;
	CDataPump *pPump;
	CDataEmBc * pEm;
	POSITION posDev;
	CPtrList * pDList;
	CDataLinked * pData;
	// Pattern
	pDList = nullptr;
	switch (pLnk->GetLinkType())
	{
	case LK_TYPE_RELEAY:
		pOut = m_pRefFasSysData->GetDeviceByID(pLnk->GetTgtFacp(), pLnk->GetTgtUnit()
			, pLnk->GetTgtChn(), pLnk->GetTgtDev());

		if (pOut == nullptr)
			return 1;
		strLine = pInDev->GetInputFullName(); strLine += L",";
#ifndef ENGLISH_MODE
		str.Format(L"%s출력:%s", strPrefix, pOut->GetOutContentsFullName());
#else
		str.Format(L"%sOutput:%s", strPrefix, pOut->GetOutContentsFullName());
#endif
		strPrefix = str + L"▶";
		strLine += str; strLine += L",";
		strLine += pOut->GetInputTypeName(); strLine += L",";
		strLine += pOut->GetOutputTypeName(); strLine += L",";
		strLine += pOut->GetEquipName(); strLine += L",";
		strLine += pOut->GetEqAddIndex(); strLine += L",";
		strLine += pOut->GetOutContentsName(); strLine += L",";
		strLine += g_szLogicTypeString[pLnk->GetLogicType()]; strLine += L",";
#ifndef ENGLISH_MODE
		strLine += L"감지기/중계기"; strLine += L"\r\n";
#else
		strLine += L"DETECTOR/MODULE"; strLine += L"\r\n";
#endif
		break;
	case LK_TYPE_PATTERN:
		pPtn = m_pRefFasSysData->GetPattern(pLnk->GetTgtFacp());
		if (pPtn == nullptr)
			return 1;
		strLine = pInDev->GetInputFullName(); strLine += L",";
#ifndef ENGLISH_MODE
		str.Format(L"%s패턴:%s", strPrefix, pPtn->GetPatternName());
#else
		str.Format(L"%sPatterns:%s", strPrefix, pPtn->GetPatternName());
#endif
		strPrefix = str + L"▶";
		strLine += str; strLine += L",";
		strLine += L",";//3
		strLine += L",";//4
		strLine += L",";//5
		strLine += L",";//6
		strLine += L",";//7
		str.Format(L"%d", pPtn->GetPatternID());
		strLine += str; strLine += L",";
#ifndef ENGLISH_MODE
		strLine += L"패턴"; strLine += L",";
		strLine += L"패턴"; strLine += L"\r\n";
#else
		strLine += L"PATTERN"; strLine += L",";
		strLine += L"PATTERN"; strLine += L"\r\n";
#endif
		pDList = pPtn->GetPtnItemList();
		//WriteLink(pInDev,pLink)
		break;
	case LK_TYPE_EMERGENCY:
		pEm = m_pRefFasSysData->GetEmergency(pLnk->GetTgtFacp());
		if (pEm == nullptr)
			return 1;
		strLine = pInDev->GetInputFullName(); strLine += L",";
#ifndef ENGLISH_MODE
		str.Format(L"%s비방:%s", strPrefix, pEm->GetEmName());
#else
		str.Format(L"%sPublic Address:%s", strPrefix, pEm->GetEmName());
#endif
		strPrefix = str + L"▶";
		strLine += str; strLine += L",";
		strLine += L",";//3
		strLine += L",";//4
		strLine += L",";//5
		strLine += L",";//6
		strLine += L",";//7
		str.Format(L"%d", pEm->GetEmID());
		strLine += str; strLine += L",";
#ifndef ENGLISH_MODE
		strLine += L"비상방송"; strLine += L",";
		strLine += L"비상방송"; strLine += L"\r\n";
#else
		strLine += L"PUBLIC ADDRESS"; strLine += L",";
		strLine += L"PUBLIC ADDRESS"; strLine += L"\r\n";
#endif
		break;
	case LK_TYPE_PUMP:
		pPump = m_pRefFasSysData->GetPump(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
		if (pPump == nullptr)
			return 1;
		strLine = pInDev->GetInputFullName(); strLine += L",";
#ifndef ENGLISH_MODE
		str.Format(L"%s펌프:%s", strPrefix, pPump->GetPumpName());
#else
		str.Format(L"%sPump:%s", strPrefix, pPump->GetPumpName());
#endif
		strPrefix = str + L"▶";
		strLine += str; strLine += L",";
		strLine += L",";//3
		strLine += L",";//4
		strLine += L",";//5
		strLine += L",";//6
		strLine += L",";//7
		str.Format(L"%d", pPump->GetPumpID());
		strLine += str; strLine += L",";
#ifndef ENGLISH_MODE
		strLine += L"펌프"; strLine += L",";
		strLine += L"펌프"; strLine += L"\r\n";
#else
		strLine += L"PUMP"; strLine += L",";
		strLine += L"PUMP"; strLine += L"\r\n";
#endif
		pDList = pPump->GetLinkList();
		break;
	default:
		return 1;
	}
	pFile->WriteString(strLine);
	if (pDList == nullptr)
		return 1;
	posDev = pDList->GetHeadPosition();
	while (posDev)
	{
		pData = (CDataLinked*)pDList->GetNext(posDev);
		if (pData == nullptr)
			continue;
		WriteCSVLink(pInDev, pData, pFile, strPrefix);
	}
	return 1;
}



void CFormLinkView::OnBnClickedBtnRefresh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CRelayTableData * pFasSysData = theApp.GetRelayTableData();
	if (pFasSysData == nullptr)
		return;

	//pFasSysData->FillDeviceTreeList(&m_ctrlTreeList);
}

LRESULT CFormLinkView::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
// 	if (m_ctrlRelayList.GetSafeHwnd())
// 		m_ctrlRelayList.DeleteAllItems();
// 	if (m_ctrlPatternList)
// 		m_ctrlPatternList.DeleteAllItems();
	return 0;
}


void CFormLinkView::OnTvnSelchangedDeviceTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem == nullptr)
		return;
	FillListCtrl(hItem);
}


int CFormLinkView::RemoveAllData()
{
	ST_TREEITEM * pData;
	if(m_ctrlTree.GetSafeHwnd())
		m_ctrlRelay.DeleteAllItems();
	while (!m_ptrItemList.IsEmpty())
	{
		pData = (ST_TREEITEM *)m_ptrItemList.RemoveHead();
		if (pData == nullptr)
			continue; 
		delete pData;
		pData = nullptr;
	}
	return 1;
}

int CFormLinkView::FillListCtrl(HTREEITEM hItem)
{
	//RemoveAllData();
	CDataSystem * pSys;
	m_ctrlRelay.SetRedraw(FALSE);
	m_ctrlRelay.DeleteAllItems();
	ST_TREEITEM * pItem = (ST_TREEITEM*)m_ctrlTree.GetItemData(hItem);
	if (pItem == nullptr)
	{
		m_ctrlRelay.SetRedraw();
		return 0;
	}
	if (pItem->nDataType != TTYPE_DEV_DEVICE)
	{
		m_ctrlRelay.SetRedraw();
		return 0;
	}
	if (pItem->pData == nullptr)
	{
		m_ctrlRelay.SetRedraw();
		return 0;
	}
	pSys = (CDataSystem*)pItem->pData;
	if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
	{
		m_ctrlRelay.SetRedraw();
		return 0;
	}

	int nIdx = 0; 
	CDataDevice * pDev , *pOut;
	CDataPattern * pPtn;
	CDataLinked * pLink ;
	CDataEmBc * pEm;
	CDataPump * pPump;
	POSITION posDev;
	CPtrList * pDList;

	pDev = (CDataDevice *)pSys->GetSysData();
	pDList = pDev->GetLinkedList();
	if (pDList == nullptr || pDList->GetCount() <= 0)
	{
		m_ctrlRelay.SetRedraw();
		return 0;
	}

	// Relay
	posDev = pDList->GetHeadPosition();
	while (posDev)
	{
		pLink = (CDataLinked*)pDList->GetNext(posDev);
		if (pLink == nullptr)
			continue; 

		if (pLink->GetLinkType() == LK_TYPE_PATTERN)
			continue;

		pOut = m_pRefFasSysData->GetDeviceByID(pLink->GetTgtFacp(), pLink->GetTgtUnit()
			,pLink->GetTgtChn(), pLink->GetTgtDev());

		if (pOut == nullptr)
			continue; 

// 		m_ctrlRelay.InsertColumn(0, L"출력이름", LVCFMT_LEFT, 300);
// 		m_ctrlRelay.InsertColumn(1, L"입력타입", LVCFMT_LEFT, 80);
// 		m_ctrlRelay.InsertColumn(2, L"출력타입", LVCFMT_LEFT, 80);
// 		m_ctrlRelay.InsertColumn(3, L"설비명", LVCFMT_LEFT, 80);
// 		m_ctrlRelay.InsertColumn(4, L"설비번호", LVCFMT_LEFT, 50);
// 		m_ctrlRelay.InsertColumn(5, L"출력설명", LVCFMT_LEFT, 80);
// 		m_ctrlRelay.InsertColumn(6, L"위치", LVCFMT_LEFT, 150);
// 		m_ctrlRelay.InsertColumn(7, L"연동타입", LVCFMT_LEFT, 80);
// 		m_ctrlRelay.InsertColumn(8, L"출력종류", LVCFMT_LEFT, 80);
		m_ctrlRelay.InsertItem(nIdx, pOut->GetOutContentsFullName());
		m_ctrlRelay.SetItemText(nIdx, 1, pOut->GetInputTypeName());
		m_ctrlRelay.SetItemText(nIdx, 2, pOut->GetOutputTypeName());
		m_ctrlRelay.SetItemText(nIdx, 3, pOut->GetEquipName());
		m_ctrlRelay.SetItemText(nIdx, 4, pOut->GetEqAddIndex());
		m_ctrlRelay.SetItemText(nIdx, 5, pOut->GetOutContentsName());
		m_ctrlRelay.SetItemText(nIdx, 6, pOut->GetOutputLocationName());
		m_ctrlRelay.SetItemText(nIdx, 7, g_szLogicTypeString[pLink->GetLogicType()]);
#ifndef ENGLISH_MODE
		m_ctrlRelay.SetItemText(nIdx, 8, L"감지기/중계기");
#else
		m_ctrlRelay.SetItemText(nIdx, 8, L"DETECTOR/MODULE");
#endif
		m_ctrlRelay.SetItemData(nIdx,(DWORD_PTR)pOut);
		nIdx++;
	}

	// Pattern
	posDev = pDList->GetHeadPosition();
	while (posDev)
	{
		pLink = (CDataLinked*)pDList->GetNext(posDev);
		if (pLink == nullptr)
			continue;

		if (pLink->GetLinkType() == LK_TYPE_RELEAY)
			continue;

		switch (pLink->GetLinkType())
		{
		case LK_TYPE_PATTERN:
			pPtn = m_pRefFasSysData->GetPattern(pLink->GetTgtFacp());

			if (pPtn == nullptr)
				continue;
			pPtn = m_pRefFasSysData->GetPattern(pLink->GetTgtFacp());
			if (pPtn == nullptr)
				continue;
			m_ctrlRelay.InsertItem(nIdx, pPtn->GetPatternName());
			m_ctrlRelay.SetItemText(nIdx, 1, L"");
			m_ctrlRelay.SetItemText(nIdx, 2, L"");
			m_ctrlRelay.SetItemText(nIdx, 3, L"");
			m_ctrlRelay.SetItemText(nIdx, 4, L"");
			m_ctrlRelay.SetItemText(nIdx, 5, L"");
			m_ctrlRelay.SetItemText(nIdx, 6, L"");
#ifndef ENGLISH_MODE
			m_ctrlRelay.SetItemText(nIdx, 7, L"패턴");
			m_ctrlRelay.SetItemText(nIdx, 8, L"패턴");
#else
			m_ctrlRelay.SetItemText(nIdx, 7, L"PATTERN");
			m_ctrlRelay.SetItemText(nIdx, 8, L"PATTERN");
#endif
			m_ctrlRelay.SetItemData(nIdx, (DWORD_PTR)pLink);
			nIdx++;
			break;
		case LK_TYPE_EMERGENCY:
			pEm = m_pRefFasSysData->GetEmergency(pLink->GetTgtFacp());
			if (pEm == nullptr)
				continue; 
			m_ctrlRelay.InsertItem(nIdx, pEm->GetEmName());
			m_ctrlRelay.SetItemText(nIdx, 1, L"");
			m_ctrlRelay.SetItemText(nIdx, 2, L"");
			m_ctrlRelay.SetItemText(nIdx, 3, L"");
			m_ctrlRelay.SetItemText(nIdx, 4, L"");
			m_ctrlRelay.SetItemText(nIdx, 5, L"");
			m_ctrlRelay.SetItemText(nIdx, 6, L"");
#ifndef ENGLISH_MODE
			m_ctrlRelay.SetItemText(nIdx, 7, L"비상방송");
			m_ctrlRelay.SetItemText(nIdx, 8, L"비상방송");
#else
			m_ctrlRelay.SetItemText(nIdx, 7, L"PUBLIC ADDRESS");
			m_ctrlRelay.SetItemText(nIdx, 8, L"PUBLIC ADDRESS");
#endif
			m_ctrlRelay.SetItemData(nIdx, (DWORD_PTR)pLink);
			nIdx++;
			break;
		case LK_TYPE_PUMP:
			pPump = m_pRefFasSysData->GetPump(pLink->GetTgtFacp(), pLink->GetTgtUnit());
			if (pPump == nullptr)
				continue;
			m_ctrlRelay.InsertItem(nIdx, pPump->GetPumpName());
			m_ctrlRelay.SetItemText(nIdx, 1, L"");
			m_ctrlRelay.SetItemText(nIdx, 2, L"");
			m_ctrlRelay.SetItemText(nIdx, 3, L"");
			m_ctrlRelay.SetItemText(nIdx, 4, L"");
			m_ctrlRelay.SetItemText(nIdx, 5, L"");
			m_ctrlRelay.SetItemText(nIdx, 6, L"");
#ifndef ENGLISH_MODE
			m_ctrlRelay.SetItemText(nIdx, 7, L"펌프");
			m_ctrlRelay.SetItemText(nIdx, 8, L"펌프");
#else
			m_ctrlRelay.SetItemText(nIdx, 7, L"PUMP");
			m_ctrlRelay.SetItemText(nIdx, 8, L"PUMP");
#endif
			m_ctrlRelay.SetItemData(nIdx, (DWORD_PTR)pLink);
			nIdx++;
			break;
		default:
			continue; 
		}
	}
	m_ctrlRelay.SetRedraw();

	return 1;
}

void CFormLinkView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
