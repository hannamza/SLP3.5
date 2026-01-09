// FormPumpEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "RelayTableData.h"
#include "FormPumpEdit.h"
#include "DlgPumpEditPane.h"
#include "DataPS.h"
#include "DataPump.h"
#include "../Common/Utils/StdioFileEx.h"
#include "../Common/CustomList/Def_CustomCtrl.h"
#include "PumpTypeList.h"

#include "DataFacp.h"
#include "ManagerPump.h"
#include "DataDevice.h"
#include "DataEmBc.h"
#include "DataPS.h"
#include "ManagerPS.h"
#include "DataPump.h"
#include "DataFacpRelay.h"
#include "DataLinked.h"
#include "DataPattern.h"
#include "PumpItem.h"
#include "DlgViewRelay.h"

// CFormPumpEdit

IMPLEMENT_DYNCREATE(CFormPumpEdit, CView)

CFormPumpEdit::CFormPumpEdit()
	//: CView(IDD_FORMPUMPEDIT)
{
	m_pDlgTopPumpEditPane = nullptr;
	m_pRefFasSysData = nullptr;
	m_pCurrentPump = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
	m_pPtrTypeList = new CPumpTypeList;
	m_pPtrTemplete = new CPtrList;
}

CFormPumpEdit::~CFormPumpEdit()
{
	SavePumpTypeList();
	RemoveAllPumpTypeList();
	RemoveAllPumpTemplete();

	if(m_pPtrTypeList)
	{
		delete m_pPtrTypeList;
		m_pPtrTypeList = nullptr;
	}
	if(m_pPtrTemplete)
	{
		delete m_pPtrTemplete;
		m_pPtrTemplete = nullptr;
	}

	if(m_pDlgTopPumpEditPane != nullptr)
	{
		delete m_pDlgTopPumpEditPane;
		m_pDlgTopPumpEditPane = nullptr;
	}
}

// void CFormPumpEdit::DoDataExchange(CDataExchange* pDX)
// {
// 	CView::DoDataExchange(pDX);
// }

void CFormPumpEdit::OnDraw(CDC* /*pDC*/)
{
	//CMDIDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
}

BEGIN_MESSAGE_MAP(CFormPumpEdit, CView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT,OnBCMAllDataInit)
	ON_WM_SIZE()
	ON_MESSAGE(UWM_CHILDPANE_SELECTCHANGE,CFormPumpEdit::OnChildPaneSelectChanged)
	ON_MESSAGE(UWM_CHILDPANE_CURDATACHANGE,CFormPumpEdit::OnChildPaneCurrentDataChanged)
	ON_MESSAGE(UWM_PROJECT_SAVE,CFormPumpEdit::OnProjectSavePumpSave)

	ON_TVN_DROPED_ITEM(IDC_PATTERNVIEW_TREE,OnTvnPatternDropedItem)
	ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE,OnTvnOutputDropedItem)
	ON_TVN_DROPED_ITEM(IDC_EMERGENCYVIEW_TREE,OnTvnEmergencyDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PUMPVIEW_TREE,OnTvnPumpDropedItem)
	ON_TVN_DROPED_ITEM(IDC_CONTACTFACP_TREE,OnTvnContactDropedItem)
	ON_TVN_DROPED_ITEM(IDC_PSWITCHVIEW_TREE,OnTvnPSwitchDropedItem)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PTNLK_MENU_DELPTN,&CFormPumpEdit::OnPtnlkMenuDelptn)
	ON_COMMAND(ID_PTNLK_MENU_VIEWPTN,&CFormPumpEdit::OnPtnlkMenuViewptn)
	ON_COMMAND(ID_RLYLK_MENU_DELRLY,&CFormPumpEdit::OnRlylkMenuDelrly)
	ON_COMMAND(ID_RLYLK_MENU_VIEWRELAY,&CFormPumpEdit::OnRlylkMenuViewrelay)
	ON_COMMAND(ID_RLYLK_MENU_MOVERELAY,&CFormPumpEdit::OnRlylkMenuMoverelay)
END_MESSAGE_MAP()

// CFormPumpEdit 진단입니다.
LRESULT CFormPumpEdit::OnBCMAllDataInit(WPARAM wp,LPARAM lp)
{
	if(wp == FORM_PRJ_OPEN || wp == FORM_PRJ_NEW)
	{
		InitForm();
	}
	return 1;
}
// CFormPumpEdit 진단입니다.


BOOL CFormPumpEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc,&m_brBackground);
	pDC->SetBkColor(m_crBack);
	pDC->SetBkMode(TRANSPARENT);
	return TRUE;

	//return CView::OnEraseBkgnd(pDC);
}

#ifdef _DEBUG
void CFormPumpEdit::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormPumpEdit::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormPumpEdit 메시지 처리기입니다.


int CFormPumpEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if(m_SpMain.Create(this,WS_CHILD | WS_VISIBLE,CRect(0,0,0,0),AFX_IDW_PANE_FIRST + 1) == false ||
		m_SpBtm.Create(&m_SpMain,WS_CHILD | WS_VISIBLE,CRect(0,0,0,0),AFX_IDW_PANE_FIRST + 2) == false)
		return -1;

	if(m_pDlgTopPumpEditPane == nullptr)
		m_pDlgTopPumpEditPane = new CDlgPumpEditPane;

	if(!m_pDlgTopPumpEditPane->CreateDlg(&m_SpMain)) //2 : child pane - pump
		return -1;
	m_pDlgTopPumpEditPane->ShowWindow(SW_SHOW);
	
	if(!m_ctrlPatternList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT,CRect(0,0,800,100),&m_SpBtm,IDC_PATTERN_LIST))
		return -1;

	if(!m_ctrlRelayList.Create(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | LVS_REPORT,CRect(0,0,200,200),&m_SpBtm,IDC_RELAY_LIST))
		return  -1;
	m_ctrlPatternList.ModifyStyle(0,LVS_SHOWSELALWAYS);
	m_ctrlRelayList.ModifyStyle(0,LVS_SHOWSELALWAYS);

	return 0;
}


void CFormPumpEdit::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	InitSplitter();
	


#ifndef ENGLISH_MODE
	m_ctrlPatternList.InsertColumn(0,L"제어 접점",LVCFMT_LEFT,150);
	m_ctrlPatternList.InsertColumn(1,L"접점 종류",LVCFMT_LEFT,150);

	m_ctrlRelayList.InsertColumn(0,L"출력이름",LVCFMT_LEFT,300);
	m_ctrlRelayList.InsertColumn(1,L"입력타입",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(2,L"출력타입",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(3,L"설비명",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(4,L"설비번호",LVCFMT_LEFT,50);
	m_ctrlRelayList.InsertColumn(5,L"출력설명",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(6,L"위치",LVCFMT_LEFT,150);
	m_ctrlRelayList.InsertColumn(7,L"연동타입",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(8,L"출력종류",LVCFMT_LEFT,80);
#else
	m_ctrlPatternList.InsertColumn(0,L"CONTROL RELAY CONTACT",LVCFMT_LEFT,150);
	m_ctrlPatternList.InsertColumn(1,L"RELAY CONTACT TYPE",LVCFMT_LEFT,150);

	m_ctrlRelayList.InsertColumn(0,L"OUTPUT NAME",LVCFMT_LEFT,300);
	m_ctrlRelayList.InsertColumn(1,L"INPUT TYPE",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(2,L"OUTPUT TYPE",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(3,L"EQUIPMENT NAME",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(4,L"EQUIPMENT NO.",LVCFMT_LEFT,50);
	m_ctrlRelayList.InsertColumn(5,L"OUTPUT DESCRIPTION",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(6,L"LOCATION",LVCFMT_LEFT,150);
	m_ctrlRelayList.InsertColumn(7,L"INTERLOCK TYPE",LVCFMT_LEFT,80);
	m_ctrlRelayList.InsertColumn(8,L"OUTPUT TYPE",LVCFMT_LEFT,80);
#endif
	m_ctrlPatternList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlRelayList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
// 
// 	if(AfxGetMainWnd())
// 	{
// 		// WPARAM : ADD/DEL , LPARAM : WINDOW
// 		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);
// 		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);
// 		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);
// 		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);
// 
// 		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);
// 
// 		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlRelayList);
// 
// 		m_pRefFasSysData = theApp.GetRelayTableData();
// 
// 	}
// 
// 	if(m_pDlgTopPumpEditPane)
// 	{
// 		m_pDlgTopPumpEditPane->SetMainWindow(this);
// 		//m_pDlgTopPumpEditPane->SetChangeData(m_pChangeData);
// 	}
	InitForm();
	theApp.SetFormViewInitComplete(FV_PUMP);
}

void CFormPumpEdit::RemoveAllPumpTypeList()
{
	ST_PUMPTYPE * pData; 
	while(!m_pPtrTypeList->IsEmpty())
	{
		pData = (ST_PUMPTYPE *)m_pPtrTypeList->RemoveHead();
		if(pData == nullptr)
			continue; 
		delete pData;
		pData = nullptr;
	}

}


void CFormPumpEdit::RemoveAllPumpTemplete()
{
	ST_PUMPTEMPLETE * pData;
	while(!m_pPtrTemplete->IsEmpty())
	{
		pData = (ST_PUMPTEMPLETE *)m_pPtrTemplete->RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}

}

int CFormPumpEdit::LoadPumpTypeList()
{
	int nSize;
	CStdioFileEx file;
	CString strEtcPath , strPath,strLine,strtemp;
	ST_PUMPTYPE * pData;
	vector<CString> vtLine;

	RemoveAllPumpTypeList();
	if(m_pRefFasSysData == nullptr)
		return 0; 
	strEtcPath = m_pRefFasSysData->GetProjectVersionPath(TRUE) + F3_PRJ_DIR_ETC;
	strPath.Format(_T("%s\\%s"),strEtcPath,FN_PUMPTYPE_LIST);

	if(GF_IsExistFile(strPath) == FALSE)
	{
		strtemp = g_strAppPath;
		if(strtemp.Right(1) != '\\')
			strtemp += "\\";
		strPath.Format(L"%sConfig\\%s",strtemp,FN_DEFAULT_PUMPTYPE_LIST);
		if(GF_IsExistFile(strPath) == FALSE)
		{
#ifndef ENGLISH_MODE
			strtemp.Format(
				L"'Project\\Version\\ETC\\%s'파일이 없어 프로그램 기본 펌프/PS목록을 가져오려 했으나\n"
				L"Config폴더 안 '%s'파일이 없습니다.\n"
				L"프로그램을 재 설치 하거나 Config폴더 안 '%s'파일을 추가하십시요"
				,FN_PUMPTYPE_LIST,FN_DEFAULT_PUMPTYPE_LIST,FN_DEFAULT_PUMPTYPE_LIST
			);
#else
			strtemp.Format(
				L"The file 'Project\Version\ETC\%s' is missing\n"
				L"Attempting to retrieve the program's default PUMP/PS list, but the file '%s' in the Config folder is missing.\n"
				L"Please reinstall the program or add the file '%s' in the Config folder."
				,FN_PUMPTYPE_LIST,FN_DEFAULT_PUMPTYPE_LIST,FN_DEFAULT_PUMPTYPE_LIST
			);
#endif
			AfxMessageBox(strtemp,MB_ICONWARNING);
			return 0; 
		}
	}

	if(!file.Open(strPath,CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
#ifndef ENGLISH_MODE
		GF_AddLog(L"파일을 가져오는데 실패했습니다.\n");
#else
		GF_AddLog(L"Failed to import the file.\n");
#endif
		return 0;
	}
	//strLine = L"#TYPE,건물,건물종류,계단,층,실,이름,기타\n";
	while(file.ReadString(strLine))
	{
		if(strLine.GetLength() <= 0)
			continue;
		vtLine = GF_SplitString(strLine,_T(","));
		nSize = (int)vtLine.size();
		strtemp = vtLine[0].GetAt(0);
		if(nSize < 5)
			continue;
		if(strtemp.GetLength() > 0 && strtemp.GetAt(0) == '#')
			continue;
		pData = new ST_PUMPTYPE;
		strtemp = vtLine[0];
		pData->nPumpType = _wtoi(strtemp);
		pData->strTypeName = vtLine[1];
		pData->strLcdMsg = vtLine[2];
		strtemp = vtLine[3];
		pData->nPsType = _wtoi(strtemp);
		strtemp = vtLine[4];
		pData->nPsUse = _wtoi(strtemp);
		m_pPtrTypeList->AddTail(pData);
	}
	//m_pPtrTypeList->Sort(TRUE);
	file.Close();
	return 0;
}

ST_PUMPTYPE * CFormPumpEdit::FindPumpType(CString strTypeName)
{
	POSITION pos;
	ST_PUMPTYPE * pData;
	pos = m_pPtrTypeList->GetHeadPosition();
	while(pos)
	{
		pData = (ST_PUMPTYPE *)m_pPtrTypeList->GetNext(pos);
		if(pData == nullptr)
			continue;

		if(pData->strTypeName.CompareNoCase(strTypeName) == 0)
			return pData;
	}
	return nullptr;
}

int CFormPumpEdit::LoadPumpTemplete()
{
	int nSize,i,nStartIdx;
	CStdioFileEx file;
	CString strEtcPath,strPath,strLine,strtemp;
	ST_PUMPTEMPLETE * pData;
	ST_PUMPTYPE * pType;
	vector<CString> vtLine;

	if(m_pPtrTemplete == nullptr)
		m_pPtrTemplete = new CPtrList;
	RemoveAllPumpTemplete();
	if(m_pRefFasSysData == nullptr)
		return 0;
	strEtcPath = m_pRefFasSysData->GetProjectVersionPath(TRUE) + F3_PRJ_DIR_ETC;
	strPath.Format(_T("%s\\%s"),strEtcPath,FN_PUMPTEMPLETE_LIST);

	if(GF_IsExistFile(strPath) == FALSE)
	{
		strtemp = g_strAppPath;
		if(strtemp.Right(1) != '\\')
			strtemp += "\\";
		strPath.Format(L"%sConfig\\%s",strtemp,FN_DEFAULT_PUMPTEMPLETE_LIST);
		if(GF_IsExistFile(strPath) == FALSE)
		{
#ifndef ENGLISH_MODE
			strtemp.Format(
				L"'Project\\Version\\ETC\\%s'파일이 없습니다.\n"
				L"또한 프로그램 기본 펌프 템플릿이 Config폴더 안 '%s'파일이 없습니다. \n"
				L"프로그램을 재 설치 하거나 Config폴더 안 '%s'파일을 추가하십시요."
				,FN_PUMPTEMPLETE_LIST,FN_DEFAULT_PUMPTEMPLETE_LIST,FN_DEFAULT_PUMPTEMPLETE_LIST
			);
#else
			strtemp.Format(
				L"The file 'Project\Version\ETC\%s' is missing.\n"
				L"Also, the program's default pump template file '%s' in the Config folder is missing.\n"
				L"Please reinstall the program or add the file '%s' in the Config folder."
				,FN_PUMPTEMPLETE_LIST,FN_DEFAULT_PUMPTEMPLETE_LIST,FN_DEFAULT_PUMPTEMPLETE_LIST
			);
#endif
			AfxMessageBox(strtemp,MB_ICONWARNING);
			return 0;
		}
	}

	if(!file.Open(strPath,CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
#ifndef ENGLISH_MODE
		GF_AddLog(L"파일을 가져오는데 실패했습니다.\n");
#else
		GF_AddLog(L"Failed to import the file.\n");
#endif
		return 0;
	}
	//strLine = L"#TYPE,건물,건물종류,계단,층,실,이름,기타\n";
	while(file.ReadString(strLine))
	{
		if(strLine.GetLength() <= 0)
			continue;
		vtLine = GF_SplitString(strLine,_T(","));
		nSize = (int)vtLine.size();
		strtemp = vtLine[0].GetAt(0);
		if(nSize < 3)
			continue;
		if(strtemp.GetLength() > 0 && strtemp.GetAt(0) == '#')
			continue;
		nStartIdx = 0; 
		pData = new ST_PUMPTEMPLETE;
		for(i = 0; i < D_MAX_TEMPLETE_ITEM_CNT; i++)
			pData->pArrPumpType[i] = nullptr;

		for(i = 2; i < nSize; i++)
		{
			strtemp = vtLine[i];
			if(strtemp.GetLength() <= 0)
				continue; 
			pType = FindPumpType(strtemp);
			if(pType == nullptr)
				continue; 
			pData->pArrPumpType[nStartIdx] = pType;
			nStartIdx ++;
		}
		strtemp = vtLine[0];
		pData->nID = _wtoi(strtemp);
		pData->strTempleteName = vtLine[1];
		m_pPtrTemplete->AddTail(pData);
	}
	file.Close();
	return 0;
}


int CFormPumpEdit::SavePumpTypeList()
{
	CString strEtcPath,strPath,strLine,strtemp;
	ST_PUMPTYPE * pData;
	POSITION pos;
	CFile fRv;
	char szBuff[256] = { 0 };
	int nTempSize;

	if(m_pRefFasSysData == nullptr)
		return 0;

	if(m_pPtrTypeList == nullptr || m_pPtrTypeList->GetCount() <= 0)
		return 0; 
	strEtcPath = m_pRefFasSysData->GetProjectVersionPath(TRUE) + F3_PRJ_DIR_ETC;
	strPath.Format(_T("%s\\%s"),strEtcPath,FN_PUMPTYPE_LIST);

	fRv.Open(strPath,CFile::modeCreate | CFile::modeReadWrite);
#ifndef ENGLISH_MODE
	strLine = L"#Pump Type(1:휀 2:펌프 3:한전 4:발전기),Type Name,Lcd Msg,Ps Type(1:비지속 2:지속),PS사용여부(0:사용 1:사용안함 2:사용할수없음)\n";
#else
	strLine = L"#Pump Type(1:FAN 2:Pump 3:Utility Power 4:Generator),Type Name,Lcd Msg,Ps Type(1:Non-latching 2:Latching),PS USe OR Not(0:Use 1:Not Use 2:Not Available)\n";
#endif
	nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
	fRv.Write(szBuff,nTempSize);
	pos = m_pPtrTypeList->GetHeadPosition();
	while(pos)
	{
		pData = (ST_PUMPTYPE *)m_pPtrTypeList->GetNext(pos);
		if(pData == nullptr)
			continue;
		strLine.Format(
			L"%d,%s,%s,%d,%d\n" 
			, pData->nPumpType,pData->strTypeName,pData->strLcdMsg,pData->nPsType,pData->nPsUse
		);

		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
		fRv.Write(szBuff,nTempSize);
	}
	fRv.Close();
	return 0;
}

int CFormPumpEdit::InitSplitter()
{
	SetSplitterPos();
	SetSplitterPos();
	m_SpMain.AddRow();
	m_SpMain.AddRow();
	m_SpMain.AddColumn();
	m_SpMain.SetWindow(0,0,m_pDlgTopPumpEditPane->GetSafeHwnd());
	m_SpMain.SetWindow(1,0,m_SpBtm.m_hWnd);
	m_SpMain.SetMinHeight(20);

	m_SpBtm.AddRow();
	m_SpBtm.AddColumn();
	m_SpBtm.AddColumn();
	m_SpBtm.SetWindow(0,0,m_ctrlPatternList.GetSafeHwnd());
	m_SpBtm.SetWindow(0,1,m_ctrlRelayList.GetSafeHwnd());
	m_SpBtm.SetMinHeight(20);


	m_SpMain.SetHeightForStatic(0,700);
	m_SpMain.SetHeightForStatic(1,300);
	//m_SpMain.SetMinHeight(500);
	m_SpBtm.SetWidthForStatic(0,300);
	m_SpBtm.SetWidthForStatic(1,400);
	//m_SpBtm.SetMinWidth(250);
	m_SpMain.Update();
	return 0;
}

int CFormPumpEdit::SetSplitterPos()
{
	if(m_SpMain.GetSafeHwnd() == nullptr)
		return 0;

	CRect rc,rcDev;
	GetClientRect(&rc);
	rc.top = rc.top;
	rc.DeflateRect(5,0,5,5);
	m_SpMain.MoveWindow(&rc);
	m_SpBtm.Update();
	m_SpMain.Update();
	return 0;
}
void CFormPumpEdit::InitList()
{
	m_ctrlPatternList.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
	m_pCurrentPump = nullptr;
}


int CFormPumpEdit::InitForm()
{
	// 해당 창이 떠 있는 상태에서 Project Open 시
	m_pDlgTopPumpEditPane->SetMainWindow(this);
// 	if(m_pCurrentPump != nullptr)
// 	{
// 		delete m_pCurrentPump;
// 		m_pCurrentPump = nullptr;
// 	}
// 	m_pChangeData = new CDataPump;


	if(AfxGetMainWnd())
	{
		// WPARAM : ADD/DEL , LPARAM : WINDOW
		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);
		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);
		//AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
		//AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_CONTACTVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlPatternList);

		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND,DROP_WND_ADD,(LPARAM)&m_ctrlRelayList);

		m_pRefFasSysData = theApp.GetRelayTableData();
	}
	LoadPumpTypeList();
	LoadPumpTemplete();

	if(m_pDlgTopPumpEditPane)
	{
		m_pDlgTopPumpEditPane->SetPumpTypeList(m_pPtrTypeList);
		m_pDlgTopPumpEditPane->SetPumpTemplete(m_pPtrTemplete);
		m_pDlgTopPumpEditPane->InitDlg();
		m_pDlgTopPumpEditPane->SetMainWindow(this);
	}

	return 0;
}


void CFormPumpEdit::OnSize(UINT nType,int cx,int cy)
{
	CView::OnSize(nType,cx,cy);

	// TODO: 여                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 기에 메시지 처리기 코드를 추가합니다.
	SetSplitterPos();
}


LRESULT CFormPumpEdit::OnChildPaneSelectChanged(WPARAM wp,LPARAM lp)
{
	// 	if (lp == 0)
	// 		return 0;
	m_ctrlPatternList.SetRedraw(FALSE);
	m_ctrlRelayList.SetRedraw(FALSE);
	m_ctrlPatternList.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();

	CDataPump * pData;
	CDataEmBc * pEm;
	//CDataPump * pPmp = nullptr;
	CDataFacpRelay * pContact = nullptr;
	CDataDevice * pDev;
	CDataPattern * pPtn;
	CPtrList * pLIst;
	POSITION pos;
	CDataLinked * pLink;
	CPumpItem * pOldItem,* pCurItem;
	int nPIdx = 0,nRIdx = 0;
	pCurItem = (CPumpItem *)lp;
	pOldItem = (CPumpItem *)wp;
	m_pCurrentPump = pCurItem;
	if(pCurItem == nullptr)
	{
		m_ctrlPatternList.SetRedraw();
		m_ctrlRelayList.SetRedraw();
		return 0;
	}

	pData = (CDataPump *)pCurItem->GetDataPump();
	if(pData == nullptr)
	{
		m_ctrlPatternList.SetRedraw();
		m_ctrlRelayList.SetRedraw();
		return 0;
	}
	
	pLIst = pData->GetLinkList();
	pos = pLIst->GetHeadPosition();
	while(pos)
	{
		pLink = (CDataLinked*)pLIst->GetNext(pos);
		if(pLink == nullptr)
			continue;
		switch(pLink->GetLinkType())
		{
		case LK_TYPE_EMERGENCY:
			pEm = m_pRefFasSysData->GetEmergency(pLink->GetTgtFacp());
			if(pEm == nullptr)
				continue;
			m_ctrlPatternList.InsertItem(nPIdx,pEm->GetEmName());

			m_ctrlPatternList.SetItemText(nPIdx,1,g_szLinkTypeString[LK_TYPE_EMERGENCY]);
			m_ctrlPatternList.SetItemData(nPIdx,(DWORD_PTR)pLink);
			nPIdx++;
			break;
		case LK_TYPE_PUMP:
			// 			pPmp = m_pRefFasSysData->GetPump(pLink->GetTgtFacp(), pLink->GetTgtUnit());
			// 			if (pPmp == nullptr)
			// 				continue;
			// 			m_ctrlPatternList.InsertItem(nPIdx, pPmp->GetPumpName());
			// 
			// 			m_ctrlPatternList.SetItemText(nPIdx, 1, g_szLinkTypeString[LK_TYPE_PUMP]);
			// 			m_ctrlPatternList.SetItemData(nPIdx, (DWORD_PTR)pLink);
			// 			nPIdx++;
			continue;
		case LK_TYPE_PATTERN:
			pPtn = m_pRefFasSysData->GetPattern(pLink->GetTgtFacp());
			if(pPtn == nullptr)
				continue;
			m_ctrlPatternList.InsertItem(nPIdx,pPtn->GetPatternName());

			m_ctrlPatternList.SetItemText(nPIdx,1,g_szPatternTypeString[pPtn->GetPatternType()]);
			m_ctrlPatternList.SetItemData(nPIdx,(DWORD_PTR)pLink);
			nPIdx++;
			continue;
		case LK_TYPE_RELEAY:
			pDev = m_pRefFasSysData->GetDeviceByID(pLink->GetTgtFacp(),pLink->GetTgtUnit(),pLink->GetTgtChn(),pLink->GetTgtDev());
			if(pDev == nullptr)
				continue;
			m_ctrlRelayList.InsertItem(nRIdx,pDev->GetOutContentsFullName());
			m_ctrlRelayList.SetItemText(nRIdx,1,pDev->GetInputTypeName());
			m_ctrlRelayList.SetItemText(nRIdx,2,pDev->GetOutputTypeName());
			m_ctrlRelayList.SetItemText(nRIdx,3,pDev->GetEquipName());
			m_ctrlRelayList.SetItemText(nRIdx,4,pDev->GetEqAddIndex());
			m_ctrlRelayList.SetItemText(nRIdx,5,pDev->GetOutContentsName());
			m_ctrlRelayList.SetItemText(nRIdx,6,pDev->GetOutputLocationName());
			m_ctrlRelayList.SetItemText(nRIdx,7,g_szLogicTypeString[pLink->GetLogicType()]);
#ifndef ENGLISH_MODE
			m_ctrlRelayList.SetItemText(nRIdx,8,L"감지기/중계기");
#else
			m_ctrlRelayList.SetItemText(nRIdx,8,L"DETECTOR/MODULE");
#endif
			m_ctrlRelayList.SetItemData(nRIdx,(DWORD_PTR)pLink);
			nRIdx++;
			continue;
		case LK_TYPE_FACP_RELAY:
			pContact = m_pRefFasSysData->GetContactFacp(pLink->GetTgtFacp(),pLink->GetTgtUnit());
			if(pContact == nullptr)
				continue;
			m_ctrlPatternList.InsertItem(nPIdx,pContact->GetFRelayName());

			m_ctrlPatternList.SetItemText(nPIdx,1,g_szLinkTypeString[LK_TYPE_FACP_RELAY]);
			m_ctrlPatternList.SetItemData(nPIdx,(DWORD_PTR)pLink);
			nPIdx++;
			continue;
		}
	}
	m_ctrlPatternList.SetRedraw();
	m_ctrlRelayList.SetRedraw();
	return 1;
}

LRESULT CFormPumpEdit::OnChildPaneCurrentDataChanged(WPARAM wp,LPARAM lp)
{
	CPumpItem * pData;
	int nRet = 0; 
	int nCmd,nIdx;
	LRESULT lRet = 1;
	if(lp == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"펌프의 제어 종류(추가/삭제/저장)를 알수 없습니다.");
#else
		AfxMessageBox(L"I don't know what type of control (add/delete/save) the pump has.");
#endif
		return 1;
	}
	nCmd  = LOWORD(wp);
	nIdx = HIWORD(wp); // CCustomListCtrl m_Manager에서 인덱스(Vector , 0부터 시작)
	pData = (CPumpItem *)lp;
	switch(nCmd)
	{
	case DATA_ADD:
		nRet = DataAdd(pData);
		break;
	case DATA_SAVE:
		nRet = DataSave(pData);
		break;
	case DATA_DEL:
		nRet = DataDelete(pData);
		m_pCurrentPump = nullptr;
		break;
	case DATA_MOVE: // Drag & Drop으로 Drop 대상이 되는 아이템은 삭제(DATA_DEL)처리하고 MOVE로 처리한다.
		nRet = DataMove(pData,nIdx); // CCustomListCtrl m_Manager에서 인덱스(Vector , 0부터 시작)
		break;
	case DATA_ALLDEL:
		nRet = DataDeleteFacp((int)lp);
		break;
	}

	if(nRet <= 0)
		return 1; 

	lRet = 0;

	if(AfxGetMainWnd())
	{
		AfxGetMainWnd()->SendMessage(UWM_DKP_PUMP_REFRESH,DATA_ALL,0);
		AfxGetMainWnd()->SendMessage(UWM_DKP_PSWITCH_REFRESH,DATA_ALL,0);
	}
	return lRet;
}


LRESULT CFormPumpEdit::OnProjectSavePumpSave(WPARAM wp,LPARAM lp)
{
	if(m_pDlgTopPumpEditPane == nullptr || m_pDlgTopPumpEditPane->GetSafeHwnd() == nullptr)
		return 1;
	if(m_pDlgTopPumpEditPane->SaveCustomControlChangeData() == FALSE)
		return 1;
	return 0;
}

void CFormPumpEdit::OnTvnOutputDropedItem(NMHDR *pNMHDR,LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if(ptc == nullptr)
		return;
	if(m_pRefFasSysData == nullptr)
		return;

	if(m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	m_ctrlRelayList.SetRedraw(FALSE);

	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataSystem * pSys;
	CDataDevice * pDev;
	CDataLinked	* pLink;
	CString str;
	CDataPump	* pPump;
	pPump = m_pCurrentPump->GetDataPump();
	if(pPump == nullptr)
		return; 

	vtItem = ptc->vtDropItems;
	int i = 0,nIdx = 0,nFind = -1;
	for(; i < vtItem.size(); i++)
	{
		if(vtItem[i] == nullptr)
			continue;
		if(vtItem[i]->dwItemData == 0)
			continue;

		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if(pData->pData == nullptr)
			continue;
		if(pData->nDataType != TTYPE_DEV_DEVICE)
			continue;

		pSys = (CDataSystem *)pData->pData;
		if(pSys == nullptr || pSys->GetSysData() == nullptr)
			continue;

		pDev = (CDataDevice *)pSys->GetSysData();
		if(pDev == nullptr)
			continue;

		nFind = FindItemLink(LK_TYPE_RELEAY,pDev->GetFacpID()
			,pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID());

		if(nFind >= 0)
			continue;

		m_ctrlRelayList.InsertItem(0,pDev->GetOutContentsFullName());
		m_ctrlRelayList.SetItemText(0,1,pDev->GetInputTypeName());
		m_ctrlRelayList.SetItemText(0,2,pDev->GetOutputTypeName());
		m_ctrlRelayList.SetItemText(0,3,pDev->GetEquipName());
		m_ctrlRelayList.SetItemText(0,4,pDev->GetEqAddIndex());
		m_ctrlRelayList.SetItemText(0,5,pDev->GetOutContentsName());
		m_ctrlRelayList.SetItemText(0,6,pDev->GetOutputLocationName());
#ifndef ENGLISH_MODE
		m_ctrlRelayList.SetItemText(0,7,L"수동");
		m_ctrlRelayList.SetItemText(0,8,L"감지기/중계기");
#else
		m_ctrlRelayList.SetItemText(0,7,L"MANUAL");
		m_ctrlRelayList.SetItemText(0,8,L"DETECTOR/MODULE");
#endif

		pLink = new CDataLinked;
		pLink->SetLinkData(pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
			,LK_TYPE_RELEAY,LOGIC_MANUAL,0
			,pPump->GetFacpID(),pPump->GetPumpID(),-1,-1
		);

		if(InsertLink(pLink) > 0)
			pPump->AddLink(pLink);
		m_ctrlRelayList.SetItemData(0,(DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();
	m_ctrlRelayList.SetRedraw();
}


void CFormPumpEdit::OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if(m_pCurrentPump == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	m_ctrlRelayList.SetRedraw(FALSE);

	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataPattern * pPtn;
	CDataLinked	* pLink;
	CDataPump	* pPump;
	CString str;

	pPump = m_pCurrentPump->GetDataPump();
	if(pPump == nullptr)
		return; 

	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;


		pPtn = (CDataPattern *)pData->pData;
		if (pPtn == nullptr)
			continue;

		nFind = FindItemLink(LK_TYPE_PATTERN,(int)pPtn->GetPatternID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pPtn->GetPatternName());

		m_ctrlPatternList.SetItemText(0, 1, g_szPatternTypeString[pPtn->GetPatternType()]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pPtn->GetPatternID(), 0, 0, 0
			, LK_TYPE_PATTERN, LOGIC_MANUAL, 0
			,pPump->GetFacpID(),pPump->GetPumpID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			pPump->AddLink(pLink);
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();
	m_ctrlRelayList.SetRedraw();
}


void CFormPumpEdit::OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataEmBc * pItem;
	CDataLinked	* pLink;
	CString str;
	CDataPump	* pPump;

	pPump = m_pCurrentPump->GetDataPump();
	if(pPump == nullptr)
		return;
	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;


		pItem = (CDataEmBc *)pData->pData;
		if (pItem == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_EMERGENCY, pItem->GetEmID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pItem->GetEmName());

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_EMERGENCY]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pItem->GetEmID(), 0, 0, 0
			, LK_TYPE_EMERGENCY, LOGIC_MANUAL, 0
			,pPump->GetFacpID(),pPump->GetPumpID(), -1, -1
		);

		if (InsertLink(pLink) > 0)
			pPump->AddLink(pLink);

		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}


void CFormPumpEdit::OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataPump * pPm;
	CDataLinked	* pLink;
	CString str;
	CDataPump	* pPump;
	pPump = m_pCurrentPump->GetDataPump();
	if(pPump == nullptr)
		return;

	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;


		pPm = (CDataPump *)pData->pData;
		if (pPm == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_PUMP, pPm->GetFacpID(), pPm->GetPumpID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pPm->GetPumpName());

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_PUMP]);


		pLink = new CDataLinked;
		pLink->SetLinkData(pPm->GetFacpID() , (int)pPm->GetPumpID(), 0, 0
			, LK_TYPE_PUMP, LOGIC_MANUAL, 0
			,pPump->GetFacpID(),pPump->GetPumpID(),-1,-1
		);

		if (InsertLink(pLink) > 0)
			pPump->AddLink(pLink);
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}

void CFormPumpEdit::OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataFacpRelay * pCont;
	CDataLinked	* pLink;
	CString str;
	CDataPump	* pPump;
	pPump = m_pCurrentPump->GetDataPump();
	if(pPump == nullptr)
		return;
	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;


		pCont = (CDataFacpRelay *)pData->pData;
		if (pCont == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_FACP_RELAY, pCont->GetFacpID(), pCont->GetRelayID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pCont->GetFRelayName());

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_FACP_RELAY]);


		pLink = new CDataLinked;
		pLink->SetLinkData(pCont->GetFacpID(), (int)pCont->GetRelayID(), 0, 0
			, LK_TYPE_FACP_RELAY, LOGIC_MANUAL, 0
			,pPump->GetFacpID(),pPump->GetPumpID(),-1,-1
		);

		if (InsertLink(pLink) > 0)
			pPump->AddLink(pLink);
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

}


void CFormPumpEdit::OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
#if _USE_PSWITCH_
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;

	if (m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
		return;

	m_ctrlPatternList.SetRedraw(FALSE);
	VT_HITEM vtItem;
	ST_TREEITEM *pData;
	CDataPS * pPs;
	CDataLinked	* pLink;
	CString str;
	CDataPump	* pPump;
	pPump = m_pCurrentPump->GetDataPump();
	if(pPump == nullptr)
		return;
	vtItem = ptc->vtDropItems;
	int i = 0, nIdx = 0, nFind = -1;
	for (; i < vtItem.size(); i++)
	{
		if (vtItem[i] == nullptr)
			continue;
		if (vtItem[i]->dwItemData == 0)
			continue;
		pData = (ST_TREEITEM*)vtItem[i]->dwItemData;
		if (pData->pData == nullptr)
			continue;


		pPs = (CDataPS *)pData->pData;
		if (pPs == nullptr)
			continue;
		nFind = FindItemLink(LK_TYPE_G_FLAG, pPs->GetFacpID(), pPs->GetPSwitchID());

		if (nFind >= 0)
			continue;

		m_ctrlPatternList.InsertItem(0, pPs->GetPSwitchName());

		m_ctrlPatternList.SetItemText(0, 1, g_szLinkTypeString[LK_TYPE_G_FLAG]);


		pLink = new CDataLinked;
		pLink->SetLinkData((int)pPs->GetFacpID(), pPs->GetPSwitchID(), 0, 0
			, LK_TYPE_G_FLAG, LOGIC_MANUAL, 0
			,pPump->GetFacpID(),pPump->GetPumpID(),-1,-1
		);

		if (InsertLink(pLink) > 0)
			pPump->AddLink(pLink);
		m_ctrlPatternList.SetItemData(0, (DWORD_PTR)pLink);
	}
	m_ctrlPatternList.SetRedraw();

#endif
}


int CFormPumpEdit::FindItemLink(int nType,int nFacp,int nUnit,int nChn,int nRelay)
{
	int i,nCnt;
	CDataLinked * pLink = nullptr;
	CXListCtrl * pList = nullptr;
	if(LK_TYPE_RELEAY == nType)
	{
		pList = &m_ctrlRelayList;
	}
	else
	{
		pList = &m_ctrlPatternList;
	}

	nCnt = pList->GetItemCount();
	if(nCnt <= 0)
		return -1;

	for(i = 0; i < nCnt; i++)
	{
		pLink = (CDataLinked *)pList->GetItemData(i);
		if(pLink == nullptr)
			continue;

		if(pLink->GetLinkType() != nType)
			continue;

		if(pLink->GetTgtFacp() != nFacp)
			continue;
		if(nUnit > 0 && pLink->GetTgtUnit() != nUnit)
			continue;

		if(nChn > 0 && pLink->GetTgtChn() != nChn)
			continue;

		if(nRelay > 0 && pLink->GetTgtDev() != nRelay)
			continue;

		return i;
	}

	return -1;
}


int CFormPumpEdit::InsertLink(CDataLinked * pLink)
{
	if(pLink == nullptr)
		return 0;
	if(m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
		return 0;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if(pDB == nullptr)
	{
		return 0;
	}

	CString strSql;
	int nCnt = 0;
	int nSrcFacp,nSrcUnit;
	int nTgtFacp,nTgtUnit,nTgtChn,nTgtDev;
	CDataPump	* pPump;
	pPump = m_pCurrentPump->GetDataPump();
	if(pPump == nullptr)
		return 0;
	nSrcFacp = nSrcUnit = -1;
	nTgtFacp = nTgtUnit = nTgtChn = nTgtDev = -1;
	// 패턴은 모두 SOURCE DEVICE를 저장한다.

	if(pLink->GetLogicType() != LOGIC_ALL_AUTO)
	{
		nSrcFacp = pPump->GetFacpID();
		nSrcUnit = pPump->GetPumpID();
	}

	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_PUMP "
		L"WHERE SRC_FACP=%d AND PMP_ID=%d  "
		L"AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		,nSrcFacp,nSrcUnit
		,nTgtFacp,nTgtUnit,nTgtChn,nTgtDev
	);

	if(pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	if(nCnt != 0)
		return 1;
	pDB->RSClose();
	strSql.Format(L"INSERT INTO TB_LINK_PUMP(SRC_FACP , PMP_ID "
		L", LINK_TYPE "
		L", FACP_ID , UNIT_ID , CHN_ID, RLY_ID "
		L", ADD_USER) "
		L" VALUES(%d,%d"
		L", %d"
		L", %d,%d,%d,%d"
		L", '%s')"
		,nSrcFacp,nSrcUnit
		,pLink->GetLinkType()
		,nTgtFacp,nTgtUnit,nTgtChn,nTgtDev
		,m_pRefFasSysData->GetCurrentUser()
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}

int CFormPumpEdit::DeleteLink(CDataLinked * pLink,YAdoDatabase * pDB)
{
	if(pLink == nullptr)
		return 0;
	if(m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
		return 0;
	if(pDB == nullptr)
	{
		return 0;
	}

	CString strSql;
	int nCnt = 0;
	int nSrcFacp,nSrcUnit;
	int nTgtFacp,nTgtUnit,nTgtChn,nTgtDev;
	CDataPump	* pPump;

	pPump = m_pCurrentPump->GetDataPump();
	nSrcFacp = nSrcUnit = -1;
	nTgtFacp = nTgtUnit = nTgtChn = nTgtDev = -1;
	// 패턴은 모두 SOURCE DEVICE를 저장한다.

	//if (pLink->GetLogicType() != LOGIC_ALL_AUTO)
	{
		nSrcFacp = pPump->GetFacpID();
		nSrcUnit = pPump->GetPumpID();
	}

	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_PUMP "
		L"WHERE SRC_FACP=%d AND PMP_ID=%d  "
		L"AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		,nSrcFacp,nSrcUnit
		,nTgtFacp,nTgtUnit,nTgtChn,nTgtDev
	);

	if(pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	pDB->RSClose();
	if(nCnt == 0)
		return 1;

	strSql.Format(L"DELETE FROM TB_LINK_PUMP "
		L"WHERE SRC_FACP=%d AND PMP_ID=%d  "
		L"AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		,nSrcFacp,nSrcUnit
		,nTgtFacp,nTgtUnit,nTgtChn,nTgtDev
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}

int CFormPumpEdit::DataAdd(CPumpItem * pItem)
{
	if(m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터를 가져오는데 실패 했습니다.");
#else
		AfxMessageBox(L"Failed to retrieve project data.");
#endif
		return 0;
	}
	int nRet = 0;
	int nFacpID = 0,nItemID = 0;
	BOOL bPsAdd = FALSE;
	std::shared_ptr<CManagerPump> spPump;
	CDataPump	* pPump = nullptr;
	spPump = m_pRefFasSysData->GetPumpManager();
	if(spPump == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"펌프 목록을 가져오는데 실패 했습니다.");
#else
		AfxMessageBox(L"Failed to get pump list.");
#endif
		return 0;
	}

	POSITION pos = nullptr;
	CDataPS	* pPs,*pTemp;
	std::shared_ptr<CManagerPS> spPsManager = m_pRefFasSysData->GetPSwitchManager();
	if(spPsManager == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"압력스위치 목록을 가져오는데 실패 했습니다.");
#else
		AfxMessageBox(L"Failed to get PS list.");

#endif
		return 0;
	}

	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if(pDb == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
#else
		AfxMessageBox(L"The project database has not been connected.");
#endif
		return 0;
	}
	if(m_pDlgTopPumpEditPane->m_pCurFacpData == nullptr)
		return 0;
	nFacpID = m_pDlgTopPumpEditPane->m_pCurFacpData->GetFacpID();
	nItemID = pItem->GetItemID();

	if(nFacpID <= 0 || nItemID <= 0)
		return 0;

	pDb->BeginTransaction();
	nRet = DataPumpAdd(pItem , pDb);
	if(nRet == 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	if(pItem->GetUsePs() == FALSE)
	{
		pDb->CommitTransaction();
		pItem->UpdatePumpData();
		spPump->AddTail(pItem->GetDataPump());
		return 1;
	}

	pPs = pItem->GetDataPS();
	if(pItem->GetDataPS() == nullptr)
	{
		// 수신기,PS ID로 검색 해서 있으면 List의 Position을 저장
		// 없으면 추가 , Position 저장
		// 나중에 실패 시 List에서 삭제(이미 있는것도 삭제한다).
		pTemp = spPsManager->GetPSData(nFacpID,nItemID);
		if(pTemp == nullptr)
		{
			pPs = new CDataPS;
			pItem->SetNewPs(pPs);
			bPsAdd = TRUE;
			//pos = spPsManager->AddTail(pPs);
		}
		else
		{
			pPs = pTemp;
			//pos = spPsManager->Find(pTemp);
		}
	}
	else
		bPsAdd = TRUE;
	nRet = DataPsAdd(pItem , pDb);
	if(nRet <=0 )
	{
		pDb->RollbackTransaction();
		if(pos)
			spPsManager->RemoveAt(pos);

		if(bPsAdd && pPs)
		{
			delete pPs;
			pPs = nullptr;
		}
		pItem->SetDataPS(nullptr);
		return 0;
	}
	pDb->CommitTransaction();

	pItem->UpdatePumpData();
	spPump->AddTail(pItem->GetDataPump());

	pItem->UpdatePsData();
	if(bPsAdd)
		spPsManager->AddTail(pItem->GetDataPS());

	return nRet;
}


int CFormPumpEdit::DataPumpAdd(CPumpItem * pItem ,YAdoDatabase * pDB)
{
// 	std::shared_ptr<CManagerPump> spPump;
// 	spPump = m_pRefFasSysData->GetPumpManager();
// 	if(spPump == nullptr)
// 	{
// 		AfxMessageBox(L"펌프 목록을 가져오는데 실패 했습니다.");
// 		return 0;
// 	}

	CString strSql;
	CDataPump * pPmp = nullptr;
	pPmp = pItem->GetDataPump();
	if(pPmp == nullptr)
		return 0; 
	strSql.Format(
		L"INSERT INTO TB_PUMP_MST(NET_ID,FACP_ID,PMP_ID,PMP_TYPE,PMP_NAME,PMP_LCD,PMP_PCB,ADD_USER,PMP_PREFIX,PMP_TYPENAME,PMP_USEPS) "
		L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s','%s','%s',%d)"
		,pPmp->GetFacpID(),pItem->GetItemID(),pItem->GetInPump()
		, pItem->GetPumpFullName() , pItem->GetPumpLcdMsg() , pPmp->GetPcb()
		, m_pRefFasSysData->GetCurrentUser()
		, pItem->GetPrefix() , pItem->GetTypeName(),pPmp->GetUsePS()
	);

// 	strSql.Format(
// 		L"INSERT INTO TB_PUMP_MST(NET_ID,FACP_ID,PMP_ID,PMP_TYPE,PMP_NAME,PMP_LCD,PMP_PCB,ADD_USER,PMP_PREFIX,PMP_TYPENAME) "
// 		L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s','%s','%s')"
// 		,pPmp->GetFacpID(),pPmp->GetPumpID(),pPmp->GetPumpType()
// 		,pPmp->GetPumpName(),pPmp->GetPumpLcd(),pPmp->GetPcb()
// 		,m_pRefFasSysData->GetCurrentUser()
// 		,pPmp->GetPrefix(),pPmp->GetTypeName()
// 	);

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 입력하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to enter the pump information into the project database.");
#endif
		return 0;
	}
	//spPump->AddTail(pPmp);
	return 1;
}


int CFormPumpEdit::DataPsAdd(CPumpItem * pItem,YAdoDatabase * pDB)
{
	CString strSql;
	CDataPS * pPs = nullptr;
	pPs = pItem->GetDataPS();
	if(pPs == nullptr)
		return 0;

	strSql.Format(
		L"INSERT INTO TB_PSWITCH_MST(NET_ID,FACP_ID,PS_ID,PS_TYPE,PS_NAME,PS_LCD,PS_PCB,ADD_USER,PS_PREFIX,PS_TYPENAME) "
		L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s','%s','%s')"
		,pPs->GetFacpID(),pItem->GetItemID(), pItem->GetInPs() 
		,pItem->GetPsFullName(),L"",pPs->GetPcb()
		,m_pRefFasSysData->GetCurrentUser()
		, pItem->GetPrefix(),pItem->GetTypeName()
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 PS 정보를 입력하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to enter the PS information into the project database.");
#endif
		return 0;
	}
	return 1;
}


int CFormPumpEdit::DataSave(CPumpItem * pItem)
{
	// 	if (pData == m_pCurrentPump)
	// 		return 0;

	CString strSql;
	int		nCnt,nRet = 0;
	int nFacpID = 0,nItemID = 0;
	CDataPump	* pPump = nullptr;
	CDataPS * pPs = nullptr;
	BOOL bAdd = FALSE;
	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if(pDb == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
#else
		AfxMessageBox(L"The project database has not been connected.");
#endif
		return 0;
	}

	pPump = pItem->GetDataPump();
	if(pPump == nullptr)
	{
		return 0;
	}

	std::shared_ptr<CManagerPS> spPsManager = m_pRefFasSysData->GetPSwitchManager();
	if(spPsManager == nullptr)
	{
		return 0;
	}

	if(m_pDlgTopPumpEditPane->m_pCurFacpData == nullptr)
		return 0; 
	nFacpID = m_pDlgTopPumpEditPane->m_pCurFacpData->GetFacpID();
	nItemID = pItem->GetItemID();

	if(nFacpID <= 0 || nItemID <= 0)
		return 0; 

	pDb->BeginTransaction();
	nRet = DataPumpSave(pItem , pDb);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	// 1. PS가 체크 && DB에 PS있는지 -> uPDATE
	// 2. PS uncheck && DB에 있는지 -> DEL
	// 3. PS 체크 && DB없는지  --> ADD
	// 4. PS UNCHECK && DB에 없는지 --> NONE

// 
// 	if(pItem->GetUsePs() != TRUE)
// 	{
// 		pDb->CommitTransaction();
// 		pItem->UpdatePumpData();
// 		return 1;
// 	}
	
	// [2025/10/14 16:07:17 KHS] 
	// 펌프 와 PS는 수신기,아이디 동일
	// DB에 있는지 확인 
	// DB에 없으면 새로 추가
	// DB에 있는데 압력스위치 목록에 없는 경우
	// DB에 있고 압력 스위치 목록에 있는 경우
	strSql.Format(L"SELECT * FROM TB_PSWITCH_MST WHERE FACP_ID=%d AND PS_ID=%d"
		,nFacpID,nItemID
	);
	if(pDb->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 가져오는데 실패 했습니다.");
#else
		AfxMessageBox(L"Failed to Retrive the PS information in the project database.");
#endif
		pDb->RollbackTransaction();
		return 0;
	}
	nCnt = pDb->GetRecordCount();
	if(pItem->GetUsePs())
	{
		if(nCnt == 0)
		{
			// Add
			bAdd = TRUE; // 성공 시 PS목록에 추가
			pPs = new CDataPS;
			pItem->SetNewPs(pPs);
			nRet = DataPsAdd(pItem,pDb);
		}
		else
		{
			bAdd = FALSE; // 성공 시 PS목록에 추가 안함
			pPs = pItem->GetDataPS();
			if(pPs == nullptr)
			{
				CDataPS * pTemp;
				pTemp = spPsManager->GetPSData(nFacpID,nItemID);
				if(pTemp == nullptr)
				{
					if(pItem->GetUsePs())
					{
						pPs = new CDataPS;
						pItem->SetNewPs(pPs);
						//spPsManager->AddTail(pPs);
						bAdd = TRUE; // 성공 시 PS목록에 추가
					}
				}
				else
				{
					pPs = pTemp;
				}
			}
			nRet = DataPsSave(pItem,pDb);
		}
	}
	else
	{
		bAdd = FALSE;
		// PS Delete
		if(nCnt > 0)
		{
			nRet = DataPsDelete(pItem,pDb);
			if(nRet > 0)
			{
				pPs = spPsManager->GetPSData(nFacpID,nItemID);
				if(pPs)
				{
					spPsManager->RemoveData(pPs);
					delete pPs;
					pPs = nullptr;
				}
				pItem->SetDataPS(nullptr);
			}
		}
		else
		{
			nRet = 1;
		}
	}
	
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		// bAdd == TRUE : 압력 스위치 목록에 추가 함
		// 삭제 
		if(bAdd && pPs != nullptr)
		{
			delete pPs;
			pPs = nullptr;
		}
		pItem->SetDataPS(nullptr);
		return 0;
	}
	pDb->CommitTransaction();

	if(bAdd)
		spPsManager->AddTail(pPs);


	pItem->UpdatePsData();
	pItem->UpdatePumpData();
	return 1;
}

int CFormPumpEdit::DataPumpSave(CPumpItem * pItem,YAdoDatabase * pDB)
{
	int nCnt;
	CString strSql;
	//BOOL bAdd = FALSE;

	CDataPump * pPmp = nullptr;
	pPmp = pItem->GetDataPump();
	if(pPmp == nullptr)
		return 0;
	strSql.Format(L"SELECT * FROM TB_PUMP_MST WHERE FACP_ID=%d AND PMP_ID=%d"
		,pPmp->GetFacpID(),pPmp->GetPumpID()
	);

	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 수정하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to save the pump information in the project database.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	if(nCnt > 0)
	{
		//bAdd = FALSE;
		strSql.Format(
			L"UPDATE TB_PUMP_MST SET PMP_TYPE=%d,PMP_NAME='%s',PMP_LCD='%s',PMP_PCB=%d,PMP_USEPS=%d "
			L", PMP_PREFIX='%s' , PMP_TYPENAME='%s' " 
			L" WHERE FACP_ID=%d AND PMP_ID=%d "
			, pItem->GetInPump() , pItem->GetPumpFullName() , pItem->GetPumpLcdMsg() , pPmp->GetPcb(),pPmp->GetUsePS()
			, pItem->GetPrefix() , pItem->GetTypeName()
			, pPmp->GetFacpID(), pItem->GetItemID()
		);
	}
	else
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 수정하는데 실패했습니다.\n변경할 펌프가 없습니다.");
#else
		AfxMessageBox(L"Failed to save the pump information in the project database.");
#endif
		return 0; 
	}

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 수정하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to save the pump information in the project database.");
#endif
		return 0;
	}
	return 1;
}


int CFormPumpEdit::DataPsSave(CPumpItem * pItem,YAdoDatabase * pDB)
{
	CString strSql;
	CDataPS * pPs = nullptr;
	pPs = pItem->GetDataPS();
	if(pPs == nullptr)
		return 0;

	strSql.Format(L"UPDATE TB_PSWITCH_MST SET PS_TYPE=%d,PS_NAME='%s',PS_LCD='%s',PS_PCB=%d "
		L" , PS_PREFIX='%s' , PS_TYPENAME='%s' "
		L" WHERE FACP_ID=%d AND PS_ID=%d "
		,pPs->GetPSwitchType(),pPs->GetPSwitchName(),pPs->GetPSwitchLcd(),pPs->GetPcb()
		,pPs->GetPrefix(),pPs->GetTypeName()
		,pPs->GetFacpID(),pPs->GetPSwitchID()
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 저장하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to save the PS information in the project database.");
#endif
		return 0;
	}
	return 1;
}

int CFormPumpEdit::DataDelete(CPumpItem * pItem)
{
	// 	if (pData == m_pCurrentPump)
	// 		return 0;
	CString strSql;
	CPtrList ptrLinkList;
	int nRet;
	CDataPump	* pPump = nullptr;
	std::shared_ptr<CManagerPump> spPump = m_pRefFasSysData->GetPumpManager();
	if(spPump == nullptr)
		return 0;
	CDataPS * pPs;
	std::shared_ptr<CManagerPS> spPs = m_pRefFasSysData->GetPSwitchManager();
	if(spPs == nullptr)
		return 0;

	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if(pDb == nullptr )
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
#else
		AfxMessageBox(L"The project database has not been connected.");
#endif
		return 0;
	}

	pPump = pItem->GetDataPump();
	if(pPump == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"펌프 정보가 없습니다.");
#else
		AfxMessageBox(L"No Pump Information.");
#endif
		return 0;
	}

	pDb->BeginTransaction();
	nRet = DataPumpDelete(pItem , pDb);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	nRet = DataPsDelete(pItem,pDb);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	// 펌프를 사용하는 Relay에서 펌프 삭제
	nRet = DataDeletePumpInRelay(pPump,pDb,&ptrLinkList);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	// 펌프의 출력 삭제
	nRet = DataDeleteLinkInPump(pPump,pDb);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	pDb->CommitTransaction();

	CDataDevice * pDev;
	POSITION dPos;
	dPos = ptrLinkList.GetHeadPosition();
	while(dPos)
	{
		pDev = (CDataDevice *)ptrLinkList.GetNext(dPos);
		if(pDev == nullptr)
			continue;
		pDev->DeleteLinkByID(LK_TYPE_PUMP,pPump->GetFacpID(),pPump->GetPumpID(),0,0);
	}
	ptrLinkList.RemoveAll();
	spPump->RemoveData(pPump);
	if(pPump)
	{
		delete pPump;
		pPump = nullptr;
	}
	pItem->SetDataPump(nullptr);

	pPs = pItem->GetDataPS();
	if(pPs)
	{
		spPs->RemoveData(pPs);
		if(pPs)
		{
			delete pPs;
			pPs = nullptr;
		}
	}
	pItem->SetDataPS(nullptr);
	
	m_ctrlPatternList.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
	return 1;
}

int CFormPumpEdit::DataPumpDelete(CPumpItem * pItem , YAdoDatabase * pDB)
{
	// 	if (pData == m_pCurrentPump)
	// 		return 0;
	CString strSql;
	int nCnt;
	CDataPump * pPmp = nullptr;
	pPmp = pItem->GetDataPump();
	if(pPmp == nullptr)
		return 0;
	strSql.Format(L"SELECT * FROM TB_PUMP_MST WHERE FACP_ID=%d AND PMP_ID=%d"
		,pPmp->GetFacpID(),pPmp->GetPumpID()
	);

	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.\n데이터베이스 열기 실패");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	if(nCnt <= 0)
	{
		return 1;
	}
	strSql.Format(L"DELETE FROM TB_PUMP_MST WHERE FACP_ID=%d AND PMP_ID=%d"
		,pPmp->GetFacpID(),pItem->GetItemID()
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}
	

	return 1;
}

int CFormPumpEdit::DataPsDelete(CPumpItem * pItem,YAdoDatabase * pDB)
{
	// 	if (pData == m_pCurrentPump)
	// 		return 0;
	CString strSql;
	int nCnt , nFacpID =0 , nPsId = 0;

// 	CDataPS * pPs = nullptr;
// 	pPs = pItem->GetDataPS();
// 	if(pPs == nullptr)
// 	{
// 		return 0;
// 	}
	if(m_pDlgTopPumpEditPane->m_pCurFacpData == nullptr)
		return 0; 
	nPsId = pItem->GetItemID();
	nFacpID = m_pDlgTopPumpEditPane->m_pCurFacpData->GetFacpID();
	strSql.Format(L"SELECT * FROM TB_PSWITCH_MST WHERE FACP_ID=%d AND PS_ID=%d"
		,nFacpID,nPsId
	);

	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 가져오는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to Retrieve the pressure switch information from the project database.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	if(nCnt <= 0)
	{
		return 1;
	}
	strSql.Format(L"DELETE FROM TB_PSWITCH_MST WHERE FACP_ID=%d AND PS_ID=%d"
		,nFacpID,nPsId
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pressure switch information from the project database.");
#endif
		return 0;
	}

	return 1;
}


int CFormPumpEdit::DataMovePumpInRelay(CDataPump * pPmp,YAdoDatabase * pDB,int nMoveVectorIdx,CPtrList * pPtrInList)
{
	int i,nCnt;
	int nF,nU,nC,nD;
	CDataDevice * pDev;
	CString strSql;
	strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
		,LK_TYPE_PUMP,pPmp->GetFacpID(),pPmp->GetPumpID()
	);
	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프의 출력 정보를 삭제하는데 실패했습니다.\n데이터베이스 열기 실패");
#else
		AfxMessageBox(L"Failed to delete pump output information from project database.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	for(i = 0; i < nCnt; i ++)
	{
		pDB->GetFieldValue(L"SRC_FACP",nF);
		pDB->GetFieldValue(L"SRC_UNIT",nU);
		pDB->GetFieldValue(L"SRC_CHN",nC);
		pDB->GetFieldValue(L"SRC_RLY",nD);
		pDev = m_pRefFasSysData->GetDeviceByID(nF,nU,nC,nD);
		if(pDev != nullptr)
			pPtrInList->AddTail(pDev);
	}

	strSql.Format(L"UPDATE TB_LINK_RELAY SET TGT_UNIT=%d WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d  "
		,nMoveVectorIdx + 1 ,LK_TYPE_PUMP,pPmp->GetFacpID(),pPmp->GetPumpID()
	);
// 	strSql.Format(L"DELETE FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
// 		,LK_TYPE_PUMP,pPmp->GetFacpID(),pPmp->GetPumpID()
// 	);
	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 수정하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to modify the pump information from the project database.");
#endif
		return 0;
	}
	return 1;
}


int CFormPumpEdit::DataDeletePumpInRelay(CDataPump * pPmp,YAdoDatabase * pDB,CPtrList * pPtrInList)
{
	int i,nCnt;
	int nF,nU,nC,nD;
	CDataDevice * pDev;
	CString strSql;
	strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
		,LK_TYPE_PUMP,pPmp->GetFacpID(),pPmp->GetPumpID()
	);
	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에서 펌프를 출력으로 사용하는 정보를 삭제했습니다.\n데이터베이스 열기 실패");
#else
		AfxMessageBox(L"Removed information from the project database about using the pump as an output.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	for(i = 0; i < nCnt; i ++)
	{
		pDB->GetFieldValue(L"SRC_FACP",nF);
		pDB->GetFieldValue(L"SRC_UNIT",nU);
		pDB->GetFieldValue(L"SRC_CHN",nC);
		pDB->GetFieldValue(L"SRC_RLY",nD);
		pDev = m_pRefFasSysData->GetDeviceByID(nF,nU,nC,nD);
		if(pDev != nullptr)
			pPtrInList->AddTail(pDev);
	}
	strSql.Format(L"DELETE FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
		,LK_TYPE_PUMP,pPmp->GetFacpID(),pPmp->GetPumpID()
	);
	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에서 펌프를 출력으로 사용하는 정보를 삭제했습니다.");
#else
		AfxMessageBox(L"Removed information from the project database about using the pump as an output.");
#endif
		return 0;
	}
	return 1;
}

int CFormPumpEdit::DataDeleteLinkInPump(CDataPump * pPmp,YAdoDatabase * pDB)
{
	CString strSql;
	strSql.Format(L"DELETE FROM TB_LINK_PUMP WHERE SRC_FACP=%d AND PMP_ID=%d "
		,pPmp->GetFacpID(),pPmp->GetPumpID()
	);
	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프의 출력 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pump output information from the project database.");
#endif
		return 0;
	}

	return 1;

}

int CFormPumpEdit::DataMove(CPumpItem * pItem,int nMoveVectorIdx)
{
	// MoveIdx --> 0부터 , ID는 1부터 
	if(m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터를 가져오는데 실패 했습니다.");
#else
		AfxMessageBox(L"Failed to retrieve project data.");
#endif
		return 0;
	}
	int nRet = 0;
	CDataPump	* pPump = nullptr;
	CDataPS	* pPs = nullptr;
	std::shared_ptr<CManagerPump> spPump;
	std::shared_ptr<CManagerPS> spPs;
	CPtrList	ptrLinkList;
	CPtrList * pList = nullptr;
	CDataLinked * pLnk = nullptr;
	POSITION pos;

	spPump = m_pRefFasSysData->GetPumpManager();
	if(spPump == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"펌프 목록을 가져오는데 실패 했습니다.");
#else
		AfxMessageBox(L"Failed to get pump list.");
#endif
		return 0;
	}
	spPs = m_pRefFasSysData->GetPSwitchManager();
	if(spPs == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"압력스위치 목록을 가져오는데 실패 했습니다.");
#else
		AfxMessageBox(L"Failed to get pump list");

#endif
		return 0;
	}

	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if(pDb == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
#else
		AfxMessageBox(L"The project database has not been connected.");
#endif
		return 0;
	}

	pPump = pItem->GetDataPump();
	pPs = pItem->GetDataPS();
	if(pPump == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"펌프 정보가 없습니다.");
#else
		AfxMessageBox(L"No Pump Infomation");
#endif
		return 0; 
	}

	if(pItem->GetUsePs() == TRUE && pPs == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"PS 정보가 없습니다.");
#else
		AfxMessageBox(L"No PS Infomation");
#endif
		return 0;
	}

	pDb->BeginTransaction();
	nRet = DataPumpMove(pItem , pDb,nMoveVectorIdx);

	if(nRet == 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}
	nRet = DataLinkMove(pItem , pDb,nMoveVectorIdx);

	if(nRet <=0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	nRet = DataMovePumpInRelay(pPump,pDb,nMoveVectorIdx,&ptrLinkList);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	if(pItem->GetUsePs() == FALSE)
	{
		pDb->CommitTransaction();
		// 1. Link 수정
		// 2. 
		pList = pPump->GetLinkList();
		pos = pList->GetHeadPosition();
		while(pos)
		{
			pLnk = (CDataLinked *)pList->GetNext(pos);
			if(pLnk == nullptr)
				continue;
			pLnk->SetSrcUnit(nMoveVectorIdx + 1);
		}
		pPump->SetPumpID(nMoveVectorIdx + 1);
		return 1;
	}

	nRet = DataPsMove(pItem , pDb,nMoveVectorIdx);
	if(nRet == 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}
	pDb->CommitTransaction();

	pList = pPump->GetLinkList();
	if(pList == nullptr)
	{
		//AfxMessageBox(L"데이터베이스 변경은 성공 했지만\n펌프동작 시 작동하는 연동출력 목록을 수정하는데 실패 했습니다.\n프로그램을 반드시 재시작 하시길 바랍니다.");
		//return 1;
	}
	else
	{
		pos = pList->GetHeadPosition();
		while(pos)
		{
			pLnk = (CDataLinked *)pList->GetNext(pos);
			if(pLnk == nullptr)
				continue;
			pLnk->SetSrcUnit(nMoveVectorIdx + 1);
		}
	}
	pPump->SetPumpID(nMoveVectorIdx + 1);
	if(pPs)
		pPs->SetPSwitchID(nMoveVectorIdx + 1);

	CDataDevice * pDev;
	while(ptrLinkList.IsEmpty() == FALSE)
	{
		pDev = (CDataDevice *)ptrLinkList.RemoveHead();
		if(pDev == nullptr)
			continue;
		pLnk = pDev->GetLink(LK_TYPE_PUMP,pPump->GetFacpID(),pPump->GetPumpID(),0,0);
		if(pLnk)
			pLnk->SetTgtUnit(nMoveVectorIdx + 1);
	}

	return 1;
}

int CFormPumpEdit::DataPumpMove(CPumpItem * pItem,YAdoDatabase * pDB,int nMoveVectorIdx)
{
	// 1. Pump를 사용하는 연동 출력 수정
	// 2. Pump 아이디 변경
	// 3. Pump가 사용하는 연동 출력 수정
	CString strSql;
	CDataPump * pPmp = nullptr;
	pPmp = pItem->GetDataPump();
	if(pPmp == nullptr)
		return 0;
	strSql.Format(
		L"UPDATE TB_PUMP_MST SET PMP_ID=%d "
		L" WHERE FACP_ID=%d AND PMP_ID=%d "
		,nMoveVectorIdx + 1
		,pPmp->GetFacpID(),pPmp->GetPumpID()
	);
	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 수정하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to Modify the pump information into the project database.");
#endif
		return 0;
	}

	return 1;
}


int CFormPumpEdit::DataPsMove(CPumpItem * pItem,YAdoDatabase * pDB,int nMoveVectorIdx)
{
	CString strSql;
	CDataPS * pPs = nullptr;
	pPs = pItem->GetDataPS();
	if(pPs == nullptr)
		return 0; 

	strSql.Format(
		L" UPDATE TB_PSWITCH_MST SET PS_ID=%d "
		L" WHERE FACP_ID=%d AND PS_ID=%d"
		,nMoveVectorIdx + 1
		,pPs->GetFacpID(),pPs->GetPSwitchID()
	);
	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 PS 정보를 수정하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to Modify the PS information into the project database.");
#endif
		return 0;
	}

	return 1;
}

int CFormPumpEdit::DataLinkMove(CPumpItem * pItem,YAdoDatabase * pDB,int nMoveVectorIdx)
{
	
	CString strSql;
	CDataPump * pPmp = nullptr;
	pPmp = pItem->GetDataPump();
	if(pPmp == nullptr)
		return 0;
	if(pPmp->GetLinkList() == nullptr)
		return 1;
	strSql.Format(
		L"UPDATE TB_LINK_PUMP SET PMP_ID=%d "
		L" WHERE SRC_FACP=%d AND PMP_ID=%d "
		,nMoveVectorIdx + 1
		,pPmp->GetFacpID(),pPmp->GetPumpID(),pPmp->GetPumpType()
	);
	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 연동 정보를 변경하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to update Pump Link information in project database.");
#endif
		return 0;
	}

	return 1;
}


int CFormPumpEdit::DataDeleteFacp(int nFacpID)
{
	// 	if (pData == m_pCurrentPump)
	// 		return 0;
	CString strSql;
	int nRet;
	CPtrList ptrLinkList;
	CDataPump	* pPump = nullptr;
	std::shared_ptr<CManagerPump> spPump = m_pRefFasSysData->GetPumpManager();
	if(spPump == nullptr)
		return 0;
	std::shared_ptr<CManagerPS> spPs = m_pRefFasSysData->GetPSwitchManager();
	if(spPs == nullptr)
		return 0;

	YAdoDatabase * pDb = m_pRefFasSysData->GetPrjDB();
	if(pDb == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
#else
		AfxMessageBox(L"The project database has not been connected.");
#endif
		return 0;
	}
	
	if(nFacpID <= 0)
		return 0;

	pDb->BeginTransaction();
	nRet = DataPumpDeleteFacp(nFacpID,pDb);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}
	
	nRet = DataPsDeleteFacp(nFacpID,pDb);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	nRet = DataLinkDeleteFacp(nFacpID,pDb);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	nRet = DataDeleteFacpInRelay(nFacpID,pDb,&ptrLinkList);
	if(nRet <= 0)
	{
		pDb->RollbackTransaction();
		return 0;
	}

	pDb->CommitTransaction();

	CDataDevice * pDev;
	CDataPS	* pRmPs;
	CDataPump * pRmPmp;
	CPtrList * pLinkList;
	CDataLinked * pLnk;
	POSITION curPos , savePos;
	curPos = spPump->GetHeadPosition();
	while(curPos)
	{
		savePos = curPos;
		pRmPmp = spPump->GetNext(curPos);
		if(pRmPmp == nullptr)
			continue;
		if(pRmPmp->GetFacpID() != nFacpID)
			continue;
		spPump->RemoveAt(savePos);
		delete pRmPmp;
		pRmPmp = nullptr;
	}

	curPos = spPs->GetHeadPosition();
	while(curPos)
	{
		savePos = curPos;
		pRmPs = spPs->GetNext(curPos);
		if(pRmPs == nullptr)
			continue;
		if(pRmPs->GetFacpID() != nFacpID)
			continue;
		spPs->RemoveAt(savePos);
		delete pRmPs;
		pRmPs = nullptr;
	}

	while(!ptrLinkList.IsEmpty())
	{
		pDev = (CDataDevice*)ptrLinkList.RemoveHead();
		if(pDev == nullptr)
			continue; 
		pLinkList = pDev->GetLinkedList();
		if(pLinkList == nullptr)
			continue; 

		curPos = pLinkList->GetHeadPosition();
		while(curPos)
		{
			savePos = curPos;
			pLnk = (CDataLinked*)pLinkList->GetNext(curPos);
			if(pLnk == nullptr)
				continue; 
			if(pLnk->GetLinkType() != LK_TYPE_PUMP)
				continue; 

			if(pLnk->GetTgtFacp() != nFacpID)
				continue; 

			pLinkList->RemoveAt(savePos);
			delete pLnk;
			pLnk = nullptr;
		}
	}

	m_ctrlPatternList.DeleteAllItems();
	m_ctrlRelayList.DeleteAllItems();
	return 1;
}

int CFormPumpEdit::DataPumpDeleteFacp(int nFacpID,YAdoDatabase * pDB)
{
	// 	if (pData == m_pCurrentPump)
	// 		return 0;
	CString strSql;
	int nCnt;
	CDataPump * pPmp = nullptr;
	
	if(nFacpID <= 0)
		return 0;
	strSql.Format(L"SELECT * FROM TB_PUMP_MST WHERE FACP_ID=%d "
		,nFacpID
	);

	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 가져오는데 실패했습니다.\n데이터베이스 열기 실패");
#else
		AfxMessageBox(L"Failed to retrieve the pump information from the project database.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	if(nCnt <= 0)
	{
		return 1;
	}
	strSql.Format(L"DELETE FROM TB_PUMP_MST WHERE FACP_ID=%d"
		,nFacpID
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}
	return 1;
}

int CFormPumpEdit::DataPsDeleteFacp(int nFacpID,YAdoDatabase * pDB)
{
	// 	if (pData == m_pCurrentPump)
	// 		return 0;
	CString strSql;
	int nCnt;

	// 	CDataPS * pPs = nullptr;
	// 	pPs = pItem->GetDataPS();
	// 	if(pPs == nullptr)
	// 	{
	// 		return 0;
	// 	}
	if(nFacpID <= 0)
		return 0;
	strSql.Format(L"SELECT * FROM TB_PSWITCH_MST WHERE FACP_ID=%d "
		,nFacpID
	);

	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pressure switch information from the project database.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	if(nCnt <= 0)
	{
		return 1;
	}
	strSql.Format(L"DELETE FROM TB_PSWITCH_MST WHERE FACP_ID=%d "
		,nFacpID
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 압력스위치 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pressure switch information from the project database.");
#endif
		return 0;
	}

	return 1;
}
int CFormPumpEdit::DataLinkDeleteFacp(int nFacpID,YAdoDatabase * pDB)
{
	// 	if (pData == m_pCurrentPump)
	// 		return 0;
	CString strSql;
	int nCnt;

	if(nFacpID <= 0)
		return 0;
	strSql.Format(L"SELECT * FROM TB_LINK_PUMP WHERE SRC_FACP=%d "
		,nFacpID
	);

	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.\n데이터베이스 열기 실패");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	if(nCnt <= 0)
	{
		return 1;
	}
	strSql.Format(L"DELETE FROM TB_LINK_PUMP WHERE SRC_FACP=%d "
		,nFacpID
	);

	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}

	return 1;
}


int CFormPumpEdit::DataDeleteFacpInRelay(int nFacpID,YAdoDatabase * pDB,CPtrList * pPtrInList)
{
	CString strSql;
	int nF,nU,nC,nD,nCnt,i;
	CDataDevice * pDev;
	strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
		,LK_TYPE_PUMP,nFacpID
	);
	if(pDB->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.\n데이터베이스 열기 실패");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	for(i = 0; i < nCnt; i ++)
	{
		pDB->GetFieldValue(L"SRC_FACP",nF);
		pDB->GetFieldValue(L"SRC_UNIT",nU);
		pDB->GetFieldValue(L"SRC_CHN",nC);
		pDB->GetFieldValue(L"SRC_RLY",nD);
		pDev = m_pRefFasSysData->GetDeviceByID(nF,nU,nC,nD);
		if(pDev != nullptr)
			pPtrInList->AddTail(pDev);
	}

	strSql.Format(L"DELETE FROM TB_LINK_RELAY WHERE TGT_FACP=%d AND LINK_TYPE=%d "
		,nFacpID , LK_TYPE_PUMP
	);
	if(pDB->ExecuteSql(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 데이터베이스에 펌프 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete the pump information from the project database.");
#endif
		return 0;
	}

	return 1;

}



void CFormPumpEdit::OnContextMenu(CWnd* pWnd,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CMenu menu,*pContext;
	UINT nID;
	UINT nFlag = 0;
	int nIdx = -1;
	CPoint ptHit;
	CPoint pt = point;
	ptHit = pt;
	CWnd * pHitWnd = WindowFromPoint(pt);
	//ScreenToClient(&ptHit);
	if(point == CPoint(-1,-1))
		return;

	if(!(pHitWnd == (CWnd*)&m_ctrlPatternList || pHitWnd == (CWnd*)&m_ctrlRelayList))
		return;
	if(pHitWnd == (CWnd*)&m_ctrlPatternList)
	{
#ifndef ENGLISH_MODE
		nID = IDR_POPUP_PTNLINK;
#else
		nID = IDR_POPUP_PTNLINK_EN;
#endif
		m_ctrlPatternList.ScreenToClient(&ptHit);
		nIdx = m_ctrlPatternList.HitTest(ptHit,nullptr);
	}
	else
	{
#ifndef ENGLISH_MODE
		nID = IDR_POPUP_RELAYLINK;
#else
		nID = IDR_POPUP_RELAYLINK_EN;
#endif
		m_ctrlRelayList.ScreenToClient(&ptHit);
		nIdx = m_ctrlRelayList.HitTest(ptHit,nullptr);
	}
	if(nIdx < 0)
		return;
	menu.LoadMenu(nID);
	pContext = menu.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}


void CFormPumpEdit::OnPtnlkMenuDelptn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nIdx,nRet = -1;
	nIdx = m_ctrlPatternList.GetNextItem(-1,LVNI_SELECTED);
	if(nIdx < 0)
		return;

#ifndef ENGLISH_MODE
	if(AfxMessageBox(L"패턴정보를 삭제하시겠습니까?",MB_YESNO) == IDNO)
		return;
#else
	if(AfxMessageBox(L"Do you want to delete the pattern information?",MB_YESNO) == IDNO)
		return;
#endif

	CString strSql;
	CDataLinked * pLink;
	YAdoDatabase * pDB;
	CDataPump	* pPump;
	pPump = m_pCurrentPump->GetDataPump();
	if(m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
	{
		AfxMessageBox(L"선택된 펌프가 없습니다.");
		return;
	}
	pLink = (CDataLinked*)m_ctrlPatternList.GetItemData(nIdx);
	if(pLink == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 패턴에 대한 정보를 가져오는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete. Failed to retrieve information about the pattern.");
#endif
		return;
	}

	if(m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
#else
		AfxMessageBox(L"Failed to delete. Invalid project settings information.");
#endif
		return;
	}

	pDB = m_pRefFasSysData->GetPrjDB();
	if(pDB == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 프로젝트의 데이터베이스 정보가 잘못됐습니다.");
#else
		AfxMessageBox(L"Failed to delete. The project has invalid database information.");
#endif
		return;
	}

	if(DeleteLink(pLink,pDB) == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"연동된 패턴 정보를 삭제하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to delete interlocked pattern information.");
#endif
		return;
	}
	m_ctrlPatternList.DeleteItem(nIdx);
	pPump->DeleteLink(pLink);
#ifndef ENGLISH_MODE
	AfxMessageBox(L"연동된 패턴 정보를 삭제하는데 성공했습니다.");
#else
	AfxMessageBox(L"Successfully deleted the interlock pattern information.");
#endif
}


void CFormPumpEdit::OnPtnlkMenuViewptn()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nIdx,nRet = -1;
	FormViewStyle iView = FV_COUNT;
	int nItemType = TTYPE_DEV_PRJNAME;
	nIdx = m_ctrlPatternList.GetNextItem(-1,LVNI_SELECTED);
	if(nIdx < 0)
		return;

	CString strSql;
	CDataLinked * pLink;
	pLink = (CDataLinked*)m_ctrlPatternList.GetItemData(nIdx);
	if(pLink == nullptr)
	{
		return;
	}
	switch(pLink->GetLinkType())
	{
	case LK_TYPE_RELEAY:
		iView = FV_MAKELINKER;
		nItemType = TTYPE_DEV_LINKED_TARGETITEM;
		break;
	case LK_TYPE_PATTERN:
		iView = FV_MAKEPATTERN;
		nItemType = TTYPE_DEV_LINKED_TARGETITEM;
		break;
	case LK_TYPE_PUMP:
		return;
// 		iView = FV_PUMP;
// 		nItemType = TTYPE_DEV_PUMP;
// 		break;
	case LK_TYPE_EMERGENCY:
		iView = FV_EMERGENCY;
		nItemType = TTYPE_DEV_EMERGENCY;
		break;
	default:
		return;
	}
	theApp.ViewFormSelectItem(iView,nItemType,(DWORD_PTR)pLink);
}


void CFormPumpEdit::OnRlylkMenuDelrly()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString str;
	int nRet = 0,nCnt,i;
	nCnt = m_ctrlRelayList.GetSelectedCount();
	if(nCnt < 0)
		return;

#ifndef ENGLISH_MODE
	if(AfxMessageBox(L"연동 출력을 삭제하시겠습니까?",MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
#else
	if(AfxMessageBox(L"Do you want to delete the interlock output?",MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
#endif

	if(m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 프로젝트 설정 정보가 잘못됐습니다.");
#else
		AfxMessageBox(L"Failed to delete. Invalid project settings information.");
#endif
		return;
	}

	BOOL bError = FALSE;
	CString strSql,strError = L"";
	CDataLinked * pLink;
	POSITION pos;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	CDataPump	* pPump;
	pPump = m_pCurrentPump->GetDataPump();
	if(m_pCurrentPump == nullptr || m_pCurrentPump->GetDataPump() == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"선택된 펌프가 없습니다.");
#else
		AfxMessageBox(L"No pump selected.");
#endif
		return;
	}
	if(pDB == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"삭제하는데 실패했습니다. 프로젝트의 데이터베이스 정보가 잘못됐습니다.");
#else
		AfxMessageBox(L"Failed to delete. The project has invalid database information.");
#endif
		return;
	}
	pos = m_ctrlRelayList.GetFirstSelectedItemPosition();
	std::vector<int> vtSel;


	while(pos)
	{
		int nSelected = m_ctrlRelayList.GetNextSelectedItem(pos);
		vtSel.push_back(nSelected);
	}

	pDB->BeginTransaction();

	for(i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		pLink = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		if(pLink == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
#ifndef ENGLISH_MODE
			strError = L"삭제하는데 실패했습니다. 연동 출력에 대한 정보를 가져오는데 실패했습니다.";
#else
			strError = L"Failed to delete. Failed to retrieve information about the interlock output.";
#endif
			bError = TRUE;
			break;;
		}

		if(DeleteLink(pLink,pDB) == 0)
		{
#ifndef ENGLISH_MODE
			strError = L"삭제하는데 실패했습니다. 데이터베이스에서 연동정보를 삭제하는데 실패했습니다.";
#else
			strError = L"Failed to delete. Failed to delete the interlocked information from the database.";
#endif
			bError = TRUE;
			break;
		}
	}

	if(bError == TRUE)
	{
		pDB->RollbackTransaction();
		AfxMessageBox(strError);
		vtSel.clear();
		return;
	}

	for(i = (int)vtSel.size() - 1; i >= 0; --i)
	{
		pLink = (CDataLinked*)m_ctrlRelayList.GetItemData(vtSel[i]);
		if(pLink == nullptr)
		{
			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
#ifndef ENGLISH_MODE
			strError = L"삭제하는데 실패했습니다. 연동 출력에 대한 정보를 가져오는데 실패했습니다.";
#else
			strError = L"Failed to delete. Failed to retrieve information about the linked output.";
#endif
			bError = TRUE;
			break;;
		}
		pPump->DeleteLink(pLink);
		m_ctrlRelayList.DeleteItem(vtSel[i]);
	}
	pDB->CommitTransaction();
	vtSel.clear();
#ifndef ENGLISH_MODE
	AfxMessageBox(L"연동 출력 정보를 삭제하는데 성공했습니다.");
#else
	AfxMessageBox(L"Successfully deleted the interlock output information.");
#endif
}


void CFormPumpEdit::OnRlylkMenuViewrelay()
{
	int nIdx;
	nIdx = m_ctrlRelayList.GetNextItem(-1,LVNI_SELECTED);
	if(nIdx < 0)
		return;
	CDataLinked * pLnk = (CDataLinked *)m_ctrlRelayList.GetItemData(nIdx);
	if(pLnk == nullptr)
		return;
	CDataDevice * pDev;
	pDev = m_pRefFasSysData->GetDeviceByID(pLnk->GetTgtFacp(),pLnk->GetTgtUnit()
		,pLnk->GetTgtChn(),pLnk->GetTgtDev());
	if(pDev == nullptr)
		return;
	CDlgViewRelay dlg(pDev);
	dlg.DoModal();
}

void CFormPumpEdit::OnRlylkMenuMoverelay()
{
	int nIdx;
	nIdx = m_ctrlRelayList.GetNextItem(-1,LVNI_SELECTED);
	if(nIdx < 0)
		return;
	CDataLinked * pLnk = (CDataLinked *)m_ctrlRelayList.GetItemData(nIdx);
	if(pLnk == nullptr)
		return;
	theApp.ViewFormSelectItem(FV_MAKELINKER,TTYPE_DEV_LINKED_TARGETITEM,(DWORD_PTR)pLnk);
}
