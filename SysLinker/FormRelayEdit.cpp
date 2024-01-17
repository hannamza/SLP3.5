// FormRelayEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormRelayEdit.h"
#include "DlgChildRelayTree.h"
#include "DlgChildEditMain.h"
#include "DlgChildChangeView.h"
#include "RelayTableData.h"
#include "DataChannel.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataSystem.h"
#include "MapSystemData.h"
#include "DataDevice.h"
#include "DataEquip.h"
#include "DataPattern.h"
#include "RelayTableData.h"
#include	"../Common/Utils/YAdoDatabase.h"
#include "ManagerPattern.h"
#include "DataPattern.h"
#include "DataLinked.h"
// CFormRelayEdit

IMPLEMENT_DYNCREATE(CFormRelayEdit, CFormView)

CFormRelayEdit::CFormRelayEdit()
	: CFormView(IDD_FORMRELAYEDIT)
{
	m_pDlgTopTreePane = nullptr;
	m_pDlgTopMainPane = nullptr;
	m_pDlgBottomChangePane = nullptr;
	m_pRefFasSysData = nullptr;
	m_pCurData = nullptr;
	m_pNewData = nullptr;
	m_pNewData = new ST_TREEITEM;
	m_pNewData->pData = new CDataSystem;
	//DeleteTreeItem(m_pNewData);
}

CFormRelayEdit::~CFormRelayEdit()
{
	if (m_pDlgTopTreePane)
	{
		delete m_pDlgTopTreePane;
		m_pDlgTopTreePane = nullptr;
	}
	if (m_pDlgTopMainPane)
	{
		delete m_pDlgTopMainPane;
		m_pDlgTopMainPane = nullptr;
	}
	if (m_pDlgBottomChangePane)
	{
		delete m_pDlgBottomChangePane;
		m_pDlgBottomChangePane = nullptr;
	}

	RemoveChangeDevice();
	DeleteTreeItem(m_pNewData);
	if (m_pNewData != nullptr)
	{
		if (m_pNewData->pData)
		{
			delete (CDataSystem*)m_pNewData->pData;
			m_pNewData->pData = nullptr;
		}
		delete m_pNewData;
		m_pNewData = nullptr;
	}
}

void CFormRelayEdit::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_ctrlTab);
}

BEGIN_MESSAGE_MAP(CFormRelayEdit, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(UWM_RELAYEDITFORM_TREESELCHANGE, OnRelayTreeSelChanged)
	ON_MESSAGE(UWM_RELAYEDITFORM_PREVIEWCHANGE, OnEditDlgPreview)
	ON_MESSAGE(UWM_RELAYEDITFORM_DATACHANGE, OnEditDlgDataChange)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CFormRelayEdit 진단입니다.

#ifdef _DEBUG
void CFormRelayEdit::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormRelayEdit::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormRelayEdit 메시지 처리기입니다.


int CFormRelayEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (m_SpMain.Create(this, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), AFX_IDW_PANE_FIRST) == false ||
		m_SpTop.Create(&m_SpMain, WS_CHILD | WS_VISIBLE, CRect(0, 0, 600, 600), 200) == false ||
		m_SpBottom.Create(&m_SpMain, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), 201) == false)
		return -1;

	if (m_pDlgTopTreePane == nullptr)
		m_pDlgTopTreePane = new CDlgChildRelayTree;

	if (!m_pDlgTopTreePane->CreateDlg(&m_SpTop))
		return -1;
	m_pDlgTopTreePane->ShowWindow(SW_SHOW);

	if (m_pDlgTopMainPane == nullptr)
		m_pDlgTopMainPane = new CDlgChildEditMain;

	if (!m_pDlgTopMainPane->CreateDlg(&m_SpTop))
		return -1;
	m_pDlgTopMainPane->ShowWindow(SW_SHOW);


	if (m_pDlgBottomChangePane == nullptr)
		m_pDlgBottomChangePane = new CDlgChildChangeView;

	if (!m_pDlgBottomChangePane->CreateDlg(&m_SpBottom))
		return -1;
	m_pDlgBottomChangePane->ShowWindow(SW_SHOW);
	return 1;
}


void CFormRelayEdit::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ResizeParentToFit();
	InitSplitter();

	if (AfxGetMainWnd())
	{
		m_pRefFasSysData = theApp.GetRelayTableData();
	}

	if (m_pDlgTopTreePane)
	{
		m_pDlgTopTreePane->SetMainWindow(this);
	}
	if (m_pDlgTopMainPane)
	{
		m_pDlgTopMainPane->SetMainWindow(this);
		m_pDlgTopMainPane->SetTreeItem(m_pCurData , m_pNewData);
	}
	if (m_pDlgBottomChangePane)
	{
		m_pDlgBottomChangePane->SetMainWindow(this);
		m_pDlgBottomChangePane->SetChangeContainer(&m_ptrChangeList, &m_mapChangeDevice);
	}
}


void CFormRelayEdit::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	SetSplitterPos();
		// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


int CFormRelayEdit::SetSplitterPos()
{
	if (m_SpMain.GetSafeHwnd() == nullptr)
		return 0;

	CRect rc, rcDev;
	GetClientRect(&rc);
	rc.DeflateRect(5, 5, 5, 5);
	m_SpMain.MoveWindow(&rc);


	// 	if (m_ctrlRightBottomRelayList.GetSafeHwnd())
	// 		m_ctrlRightBottomRelayList.R();
	// 
	// 	if (m_ctrlPatternList.GetSafeHwnd())
	// 		m_ctrlPatternList.RedrawWindow();
	return 0;
}

int CFormRelayEdit::InitSplitter()
{
	SetSplitterPos();
	m_SpMain.AddRow();
	m_SpMain.AddRow();
	m_SpMain.AddColumn();
	m_SpMain.SetWindow(0, 0, m_SpTop.m_hWnd);
	m_SpMain.SetWindow(1, 0, m_SpBottom.m_hWnd);
	m_SpMain.SetMinHeight(20);

	m_SpTop.AddRow();
	m_SpTop.AddColumn();
	m_SpTop.AddColumn();
	m_SpTop.SetWindow(0, 0, m_pDlgTopTreePane->GetSafeHwnd());
	m_SpTop.SetWindow(0, 1, m_pDlgTopMainPane->GetSafeHwnd());
	m_SpTop.SetMinHeight(20);


	m_SpBottom.AddRow();
	m_SpBottom.AddColumn();
	m_SpBottom.SetWindow(0, 0, m_pDlgBottomChangePane->GetSafeHwnd());
	m_SpBottom.SetMinHeight(20);

	m_SpMain.SetHeightForStatic(0, 400);
	//m_SpMain.SetHeightForStatic(1, 600);

	m_SpTop.SetWidthForStatic(0, 300);
	//m_SpTop.SetWidthForStatic(1, 350);
	//m_SpTop.SetWidthForStatic(2, 350);

	m_SpMain.Update();
	return 0;
}

int CFormRelayEdit::InitForm()
{
	//if (AfxGetMainWnd())
	//{
	//	// WPARAM : ADD/DEL , LPARAM : WINDOW
	//	AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightTopPatternList);
	//	AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRightBottomRelayList);

	//	m_pRefFasSysData = theApp.GetRelayTableData();
	//}

	//if (m_pDlgLeftBottomPSwitchInfo)
	//	m_pDlgLeftBottomPSwitchInfo->InitDlg();

	//if (m_pDlgLeftTopTreePane)
	//	m_pDlgLeftTopTreePane->InitDlg();
	return 0;
}

LRESULT CFormRelayEdit::OnRelayTreeSelChanged(WPARAM wp, LPARAM lp)
{
	m_pCurData = nullptr;
	RemoveChangeDevice();
	DeleteTreeItem(m_pNewData);
	m_pCurData = (ST_TREEITEM *)lp;
	CopyTreeItem(m_pCurData, m_pNewData);
	if (m_pDlgTopMainPane)
		m_pDlgTopMainPane->TreeSelectChange(m_pCurData , m_pNewData);

	if(m_pDlgBottomChangePane)
		m_pDlgBottomChangePane->TreeSelectChange(m_pCurData, m_pNewData);
	return 0;
}

LRESULT CFormRelayEdit::OnEditDlgPreview(WPARAM wp, LPARAM lp)
{
	if (lp == DATA_DEL)
		MakeDeleteDeviceMap(m_pCurData);
	else if (lp == DATA_SAVE)
		MakeChangeDeviceMap(m_pNewData, CHANGE_ACTION_UPDATE);
	if (m_pDlgBottomChangePane)
		m_pDlgBottomChangePane->EditDlgPreviewChange(wp,lp);
	return 0;
}

LRESULT CFormRelayEdit::OnEditDlgDataChange(WPARAM wp, LPARAM lp)
{
	// 미리보기를 하지 않을 경우 영향을 미치는 부분을 유지한채 저장한다.
	// 기존 데이터를 변경한다.
	int nRet = 0;
	if (wp == 0) ///< 미리보기를 하지 않은경우
	{
		if (lp == DATA_DEL)
			MakeDeleteDeviceMap(m_pCurData);
		else if (lp == DATA_SAVE)
			MakeChangeDeviceMap(m_pNewData, CHANGE_ACTION_KEEP);
	}
	switch (lp)
	{
	case DATA_ADD:
		nRet = DataAdd();
		break;
	case DATA_SAVE:
		nRet = DataSave();
		break;
	case DATA_DEL:
		nRet = DataDelete();
		break;
	}

	if (nRet <= 0)
	{
		AfxMessageBox(L"실패");
		return 0;
	}
	else
	{
		AfxMessageBox(L"성공");
	}
	return 1;
}

void CFormRelayEdit::RefreshAllData()
{
	m_pCurData = nullptr;
	if (m_pDlgTopTreePane)
		m_pDlgTopTreePane->ReloadData();
	if(m_pDlgBottomChangePane)
		m_pDlgBottomChangePane->ReloadData();
	if (AfxGetApp())
		((CSysLinkerApp*)AfxGetApp())->PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
}



int CFormRelayEdit::DataAdd()
{
	int nRet = 0;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	CDataSystem * pSys = nullptr;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	int nf, nu, nc, nr;
	nf= nu= nc= nr = -1;
	pDB->BeginTransaction();
	switch (m_pNewData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pSys = DataAddFacp(pDB);
		if (pSys != nullptr && pSys->GetSysData() != nullptr)
		{
			pFacp = (CDataFacp*)pSys->GetSysData();
			nf = pFacp->GetFacpID();
		}
		break;
	case TTYPE_DEV_UNIT:
		pSys = DataAddUnit(pDB);
		if (pSys != nullptr && pSys->GetSysData() != nullptr)
		{
			pUnit = (CDataUnit*)pSys->GetSysData();
			nf = pUnit->GetFacpID();
			nu = pUnit->GetUnitID();
		}
		break;
	case TTYPE_DEV_CHANNEL:
		pSys = DataAddChn(pDB);
		if (pSys != nullptr && pSys->GetSysData() != nullptr)
		{
			pChn = (CDataChannel*)pSys->GetSysData();
			nf = pChn->GetFacpID();
			nu = pChn->GetUnitID();
			nc = pChn->GetChnID();
		}
		break;
	case TTYPE_DEV_DEVICE:
		pSys = DataAddRelay(pDB);
		if (pSys != nullptr && pSys->GetSysData() != nullptr)
		{
			pDev = (CDataDevice*)pSys->GetSysData();
			nf = pDev->GetFacpID();
			nu = pDev->GetUnitID();
			nc = pDev->GetChnID();
			nr = pDev->GetDeviceID();
		}
		break;
	}

	if (pSys == nullptr)
	{
		pDB->RollbackTransaction();
		return 0;
	}
	pDB->CommitTransaction();

	
	if (m_pDlgTopTreePane)
		m_pDlgTopTreePane->RefreshTree(m_pNewData->nDataType ,nf,nu,nc,nr);
	return 1;
}


int CFormRelayEdit::DataSave()
{
	// 2. Device 저장
	// 3. 영향받는 부분 저장
	int nRet = 0; 
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	pDB->BeginTransaction();
	switch (m_pNewData->nDataType)
	{
	case TTYPE_DEV_FACP:
		nRet = DataSaveFacp(pDB);
		break;
	case TTYPE_DEV_UNIT:
		nRet = DataSaveUnit(pDB);
		break;
	case TTYPE_DEV_CHANNEL:
		nRet = DataSaveChn(pDB);
		break;
	case TTYPE_DEV_DEVICE:
		nRet = DataSaveRelay(pDB);
		break;
	}
	if (nRet <= 0)
	{
		pDB->RollbackTransaction();
		return 0; 
	}
	nRet = SaveAffectedByChange(pDB);
	if (nRet <= 0)
	{
		pDB->RollbackTransaction();
		return 0;
	}
	pDB->CommitTransaction();
	CopyTreeItem(m_pNewData, m_pCurData);
	ApplyChangeSystemMemory();
	RefreshAllData();
	return 1;
}


int CFormRelayEdit::DataDelete()
{
	// 현재 삭제
	// 연결되는 부분 삭제
	int nRet = 0;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	pDB->BeginTransaction();
	switch (m_pNewData->nDataType)
	{
	case TTYPE_DEV_FACP:
		nRet = DataDeleteFacp(pDB);
		break;
	case TTYPE_DEV_UNIT:
		nRet = DataDeleteUnit(pDB);
		break;
	case TTYPE_DEV_CHANNEL:
		nRet = DataDeleteChn(pDB);
		break;
	case TTYPE_DEV_DEVICE:
		nRet = DataDeleteRelay(pDB);
		break;
	}
	if (nRet <= 0)
	{
		pDB->RollbackTransaction();
		return 0;
	}
	nRet = DeleteAffectedByChange(pDB);
	if (nRet <= 0)
	{
		pDB->RollbackTransaction();
		return 0;
	}
	pDB->CommitTransaction();
	ApplyDeleteSystemMemory();
	DeleteTreeItem(m_pNewData);
	RefreshAllData();
	return 1;
}


CDataSystem * CFormRelayEdit::DataAddFacp(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return nullptr;
	if (m_pNewData->nDataType != TTYPE_DEV_FACP || m_pNewData->pData == nullptr)
		return nullptr;
	CString strSql;
	CDataSystem * pSys;
	CDataFacp *pNew;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_FACP || pSys->GetSysData() == nullptr)
		return nullptr;
	pNew = (CDataFacp*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_FACP WHERE FACP_ID=%d ", pNew->GetFacpID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return nullptr;

	if (pDB->GetRecordCount() > 0)
	{
		strSql.Format(L"UPDATE TB_FACP SET FACP_NAME='%s' , FACP_NUM=%d , FACP_TYPE=%d WHERE FACP_ID =%d"
			, pNew->GetFacpName(), pNew->GetFacpNum(), pNew->GetFacpType()
			, pNew->GetFacpID()
		);
	}
	else
	{
		strSql.Format(L"INSERT INTO TB_FACP(NET_ID,FACP_ID,FACP_NUM,FACP_TYPE,FACP_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetFacpNum(), pNew->GetFacpType(), pNew->GetFacpName()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return nullptr;
	pSys = m_pRefFasSysData->AddSystemFacpDataByNum(pNew->GetFacpNum(), pNew->GetFacpID(), pNew->GetFacpType(), pNew->GetFacpName());
	
// 	m_pCurData->nDataType = TTYPE_DEV_FACP;
// 	m_pCurData->pData = pSys;
	return pSys;

}


CDataSystem * CFormRelayEdit::DataAddUnit(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return nullptr;
	if (m_pNewData->nDataType != TTYPE_DEV_UNIT || m_pNewData->pData == nullptr)
		return nullptr;
	CString strSql;
	CDataSystem * pSys;
	CDataUnit *pNew;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_UNIT || pSys->GetSysData() == nullptr)
		return nullptr;
	pNew = (CDataUnit*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_UNIT WHERE FACP_ID=%d AND UNIT_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return nullptr;

	if (pDB->GetRecordCount() > 0)
	{
		strSql.Format(L"UPDATE TB_UNIT SET UNIT_NAME='%s' , UNIT_NUM=%d ,UNIT_TYPE=%d "
			L" WHERE FACP_ID =%d AND UNIT_ID=%d "
			, pNew->GetUnitName(), pNew->GetUnitNum(), pNew->GetUnitType()
			, pNew->GetFacpID(), pNew->GetUnitID()
		);
	}
	else
	{
		strSql.Format(L"INSERT INTO TB_UNIT(NET_ID,FACP_ID,UNIT_ID,UNIT_NUM,UNIT_TYPE,UNIT_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetUnitNum(), pNew->GetUnitType()
			, pNew->GetUnitName()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return nullptr;

	pSys = m_pRefFasSysData->AddSystemUnitDataByID(pNew->GetFacpID(), pNew->GetUnitNum()
		,pNew->GetUnitID(), pNew->GetUnitType(), pNew->GetUnitName());

// 	m_pCurData->nDataType = TTYPE_DEV_UNIT;
// 	m_pCurData->pData = pSys;
	return pSys;
}

CDataSystem * CFormRelayEdit::DataAddChn(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return nullptr;
	if (m_pNewData->nDataType != TTYPE_DEV_CHANNEL || m_pNewData->pData == nullptr)
		return nullptr;
	CString strSql;
	CDataSystem * pSys;
	CDataChannel *pNew;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_CHANNEL || pSys->GetSysData() == nullptr)
		return nullptr;
	pNew = (CDataChannel*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return nullptr;

	if (pDB->GetRecordCount() > 0)
	{
		strSql.Format(L"UPDATE TB_CHANNEL SET CHN_NAME='%s' , CHN_NUM=%d "
			L" WHERE FACP_ID =%d AND UNIT_ID=%d AND CHN_ID=%d "
			, pNew->GetChnName(), pNew->GetChnNum()
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID()
		);
	}
	else
	{
		strSql.Format(L"INSERT INTO TB_CHANNEL(NET_ID,FACP_ID,UNIT_ID,CHN_ID,CHN_NUM,CHN_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,"
			L"'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetChnNum()
			, pNew->GetChnName()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return nullptr;

	pSys = m_pRefFasSysData->AddSystemChannelDataByID(pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnNum()
		, pNew->GetChnNum(), pNew->GetChnName()
	);
	
// 	m_pCurData->nDataType = TTYPE_DEV_CHANNEL;
// 	m_pCurData->pData = pSys;
	return pSys;
}

CDataSystem * CFormRelayEdit::DataAddRelay(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return nullptr;
	if (m_pNewData->nDataType != TTYPE_DEV_DEVICE || m_pNewData->pData == nullptr)
		return nullptr;

	CString strSql, strCmd, strWhere;
	CDataSystem * pSys;
	CDataDevice *pNew;
	CDataLocBase * pLoc;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	int nRelayIndex = 0;
	BOOL bInsert = FALSE;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		return nullptr;

	pNew = (CDataDevice*)pSys->GetSysData();
	nRelayIndex = m_pRefFasSysData->GetNewRelayIndex();
	pNew->SetRelayIndex(nRelayIndex);
	nInput = nOut = nContents = 0;
	nBuild = nBtype = nStair = nFloor = nRoom = 0;
	pInput = pNew->GetEqInput();
	pOut = pNew->GetEqOutput();
	pContents = pNew->GetEqOutContents();
	pEqName = pNew->GetEqName();
	if (pInput)
		nInput = (int)pInput->GetEquipID();
	if (pOut)
		nOut = (int)pOut->GetEquipID();
	if (pContents)
		nContents = (int)pContents->GetEquipID();
	if (pEqName)
		nEqID = (int)pEqName->GetEquipID();
	pLoc = pNew->GetDevInputLocation();
	if (pLoc)
	{
		nBuild = (int)pLoc->GetBuildID();
		nBtype = (int)pLoc->GetBTypeID();
		nStair = (int)pLoc->GetStairID();
		nFloor = (int)pLoc->GetFloorID();
		nRoom = (int)pLoc->GetRoomID();
	}

	strSql.Format(L"SELECT * FROM TB_RELAY_LIST FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
	);
	if (pDB->OpenQuery(strSql) == FALSE)
		return nullptr;

	if (pDB->GetRecordCount() <= 0)
	{
		strCmd.Format(L"UPDATE TB_RELAY_LIST SET RLY_NUM=%d,MAP_KEY='%s',RPT_NUM=%d"
			L",INPUT_ID=%d,OUTPUT_ID=%d "
			L",BD_ID=%d,BTYPE_ID=%d,STAIR_ID=%d,FL_ID=%d,RM_ID=%d "
			L",OUTCONTENTS_ID=%d,EQ_ID=%d , EQ_ADD_IDX='%s' "
			, pNew->GetDeviceNum(), pNew->GetDevKey(), pNew->GetRepeatorNum()
			, nInput, nOut
			, nBuild, nBtype, nStair, nFloor, nRoom
			, nContents, nEqID, pNew->GetEqAddIndex()
		);
		strWhere.Format(L" WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
		);
	}
	else
	{
		strSql.Format(L"INSERT INTO TB_RELAY_LIST(NET_ID,RIDX,FACP_ID,UNIT_ID"
			L",CHN_ID,RLY_ID,RLY_NUM "
			L",MAP_KEY"
			L",RPT_NUM,INPUT_ID"
			L",OUTPUT_ID "
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
			L",%d,'%s','ADMIN') "
			, pNew->GetRelayIndex()
			, pNew->GetFacpID(), pNew->GetUnitID()
			, pNew->GetChnID(), pNew->GetDeviceID(), pNew->GetDeviceNum()
			, pNew->GetDevKey()
			, pNew->GetRepeatorNum(), nInput
			, nOut
			, nBuild, nBtype
			, nStair, nFloor
			, nRoom, nContents
			, nEqID, pNew->GetEqAddIndex()
		);
		bInsert = TRUE;
	}

	strSql = strCmd + strWhere;

	if (pDB->ExecuteSql(strSql) == FALSE)
		return nullptr;

	pSys = m_pRefFasSysData->AddSystemDeviceDataByID(pNew->GetFacpID(), pNew->GetUnitID()
		, pNew->GetChnID(), pNew->GetDeviceID(), pNew->GetDeviceNum()
		, nBuild, nBtype, nStair, nFloor, nRoom
		, -1, -1, -1, -1, -1
		, nInput, nOut, nContents
		, nEqID, pNew->GetEqAddIndex()
		, pNew->GetDeviceNum()/4
		, 0
		, pNew->GetDevKey()
		, nullptr, nullptr, nullptr
		, pNew->GetRelayIndex()
	);
	if(pSys && bInsert)
		m_pRefFasSysData->IncreaseRelayIndex();
// 	m_pCurData->nDataType = TTYPE_DEV_DEVICE;
// 	m_pCurData->pData = pSys;

	return pSys;
}

int CFormRelayEdit::DataSaveFacp(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;
	if (m_pNewData->nDataType != TTYPE_DEV_FACP || m_pNewData->pData == nullptr)
		return 0;
	if (m_pCurData == nullptr || m_pCurData->nDataType != TTYPE_DEV_FACP || m_pCurData->pData == nullptr)
		return 0;

	CString strSql;
	CDataSystem * pSys;
	CDataFacp* pOld, *pNew;
	pSys = (CDataSystem*)m_pCurData->pData;
	if (pSys->GetDataType() != SE_FACP || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataFacp*)pSys->GetSysData();
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_FACP || pSys->GetSysData() == nullptr)
		return 0;
	pNew = (CDataFacp*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_FACP WHERE FACP_ID=%d ", pNew->GetFacpID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	if (pDB->GetRecordCount() <= 0)
	{
		strSql.Format(L"INSERT INTO TB_FACP(NET_ID,FACP_ID,FACP_NUM,FACP_TYPE,FACP_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetFacpNum(), pNew->GetFacpType(), pNew->GetFacpName()
		);
	}
	else
	{
		strSql.Format(L"UPDATE TB_FACP SET FACP_NAME='%s' , FACP_NUM=%d , FACP_TYPE=%d WHERE FACP_ID =%d"
			, pNew->GetFacpName(), pNew->GetFacpNum(), pNew->GetFacpType()
			, pOld->GetFacpID()
		);
	}
	

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1;
}

int CFormRelayEdit::DataSaveChn(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;
	if (m_pNewData->nDataType != TTYPE_DEV_CHANNEL|| m_pNewData->pData == nullptr)
		return 0;
	if (m_pCurData == nullptr || m_pCurData->nDataType != TTYPE_DEV_CHANNEL || m_pCurData->pData == nullptr)
		return 0;

	CString strSql;
	CDataSystem * pSys;
	CDataChannel* pOld, *pNew;
	pSys = (CDataSystem*)m_pCurData->pData;
	if (pSys->GetDataType() != SE_CHANNEL || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataChannel*)pSys->GetSysData();
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_CHANNEL || pSys->GetSysData() == nullptr)
		return 0;
	pNew = (CDataChannel*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID() , pNew->GetChnID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	if (pDB->GetRecordCount() <= 0)
	{
		strSql.Format(L"INSERT INTO TB_CHANNEL(NET_ID,FACP_ID,UNIT_ID,CHN_ID,CHN_NUM,CHN_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,"
			L"'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetChnNum()
			, pNew->GetChnName()
		);
	}
	else
	{
		strSql.Format(L"UPDATE TB_CHANNEL SET CHN_NAME='%s' , CHN_NUM=%d "
			L" WHERE FACP_ID =%d AND UNIT_ID=%d AND CHN_ID=%d "
			, pNew->GetChnName(), pNew->GetChnNum()
			, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID()
		);
	}

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1;
}

int CFormRelayEdit::DataSaveUnit(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;
	if (m_pNewData->nDataType != TTYPE_DEV_UNIT || m_pNewData->pData == nullptr)
		return 0;
	if (m_pCurData == nullptr || m_pCurData->nDataType != TTYPE_DEV_UNIT || m_pCurData->pData == nullptr)
		return 0;

	CString strSql;
	CDataSystem * pSys;
	CDataUnit* pOld, *pNew;
	pSys = (CDataSystem*)m_pCurData->pData;
	if (pSys->GetDataType() != SE_UNIT || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataUnit*)pSys->GetSysData();
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_UNIT || pSys->GetSysData() == nullptr)
		return 0;
	pNew = (CDataUnit*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_UNIT WHERE FACP_ID=%d AND UNIT_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID());

	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	if (pDB->GetRecordCount() <= 0)
	{
		strSql.Format(L"INSERT INTO TB_UNIT(NET_ID,FACP_ID,UNIT_ID,UNIT_NUM,UNIT_TYPE,UNIT_NAME,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,'%s','ADMIN') "
			, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetUnitNum(), pNew->GetUnitType()
			, pNew->GetUnitName()
		);
	}
	else
	{
		strSql.Format(L"UPDATE TB_UNIT SET UNIT_NAME='%s' , UNIT_NUM=%d ,UNIT_TYPE=%d "
			L" WHERE FACP_ID =%d AND UNIT_ID=%d "
			, pNew->GetUnitName(), pNew->GetUnitNum(), pNew->GetUnitType()
			, pOld->GetFacpID(), pOld->GetUnitID()
		);
	}
	

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1;
}

int CFormRelayEdit::DataSaveRelay(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;
	if (m_pNewData->nDataType != TTYPE_DEV_DEVICE || m_pNewData->pData == nullptr)
		return 0;
	if (m_pCurData == nullptr || m_pCurData->nDataType != TTYPE_DEV_DEVICE || m_pCurData->pData == nullptr)
		return 0;

	CString strSql,strCmd,strWhere;
	CDataSystem * pSys;
	CDataDevice* pOld, *pNew;
	CDataLocBase * pLoc;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	pSys = (CDataSystem*)m_pCurData->pData;
	if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataDevice*)pSys->GetSysData();
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		return 0;
	pNew = (CDataDevice*)pSys->GetSysData();

	strSql.Format(L"SELECT * FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
	);
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	if (pDB->GetRecordCount() <= 0)
		return 0;

	nInput = nOut = nContents = 0;
	nBuild = nBtype = nStair = nFloor = nRoom = 0;
	pInput = pNew->GetEqInput();
	pOut = pNew->GetEqOutput();
	pContents = pNew->GetEqOutContents();
	pEqName = pNew->GetEqName();
	if (pInput)
		nInput = (int)pInput->GetEquipID();
	if (pOut)
		nOut = (int)pOut->GetEquipID();
	if (pContents)
		nContents = (int)pContents->GetEquipID();
	if (pEqName)
		nEqID = (int)pEqName->GetEquipID();
	pLoc = pNew->GetDevInputLocation();
	if (pLoc)
	{
		nBuild = (int)pLoc->GetBuildID();
		nBtype = (int)pLoc->GetBTypeID();
		nStair = (int)pLoc->GetStairID();
		nFloor = (int)pLoc->GetFloorID();
		nRoom = (int)pLoc->GetRoomID();
	}

	strCmd.Format(L"UPDATE TB_RELAY_LIST SET RLY_NUM=%d,MAP_KEY='%s',RPT_NUM=%d"
		L",INPUT_ID=%d,OUTPUT_ID=%d "
		L",BD_ID=%d,BTYPE_ID=%d,STAIR_ID=%d,FL_ID=%d,RM_ID=%d "
		L",OUTCONTENTS_ID=%d,EQ_ID=%d , EQ_ADD_IDX='%s' "
		L",FACP_ID=%d,UNIT_ID=%d , CHN_ID=%d "
		, pNew->GetDeviceNum(), pNew->GetDevKey(), pNew->GetRepeatorNum()
		, nInput, nOut
		, nBuild, nBtype, nStair, nFloor, nRoom
		, nContents, nEqID, pNew->GetEqAddIndex()
		, pNew->GetFacpID() , pNew->GetUnitID(),pNew->GetChnID()
	);
	strWhere.Format(L" WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID()
	);

	strSql = strCmd + strWhere;

	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}

int CFormRelayEdit::DataDeleteFacp(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;

	// device , chn , unit,facp
	if (m_pCurData == nullptr || m_pCurData->nDataType != TTYPE_DEV_FACP || m_pCurData->pData == nullptr)
		return 0;

	int nCnt, i;
	int nF, nU, nC;
	int nif, niu;
	CString strSql,strKey;
	CDataSystem * pSys , *pTemp;
	CDataFacp* pOld;
	CMapSystemData * pMap;
	pSys = (CDataSystem*)m_pCurData->pData;
	if (pSys->GetDataType() != SE_FACP || pSys->GetSysData() == nullptr)
		return 0;
	pMap = m_pRefFasSysData->GetSystemData();
	if (pMap == nullptr)
		return 0; 

	pOld = (CDataFacp*)pSys->GetSysData();
	
	nF = pOld->GetFacpNum();
	nif = pOld->GetFacpID();
	strSql.Format(L"DELETE FROM TB_FACP WHERE FACP_ID=%d ", pOld->GetFacpID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	strKey = GF_GetSysDataKey(SE_FACP, nF);
	pTemp = (*pMap)[strKey];
	if(pTemp)
		m_ptrSysRefChangeList.AddTail(pTemp);


	strSql.Format(L"SELECT  * FROM TB_UNIT WHERE FACP_ID=%d ", pOld->GetFacpID());
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (pDB->GetFieldValue(L"UNIT_NUM", nU) == FALSE)
			continue;

		strKey = GF_GetSysDataKey(SE_UNIT, nF , nU);
		pTemp = (*pMap)[strKey];
		if (pTemp)
			m_ptrSysRefChangeList.AddTail(pTemp);
	}
	strSql.Format(L"DELETE FROM TB_UNIT WHERE FACP_ID=%d ", pOld->GetFacpID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	strSql.Format(L"SELECT * FROM TB_CHANNEL WHERE FACP_ID=%d ", pOld->GetFacpID());
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (pDB->GetFieldValue(L"UNIT_ID", niu) == FALSE)
			continue;
		if (pDB->GetFieldValue(L"CHN_NUM", nC) == FALSE)
			continue;
		nU = m_pRefFasSysData->CvtUnitIDToNum(nF, niu);
		strKey = GF_GetSysDataKey(SE_CHANNEL,nF, nU , nC);
		pTemp = (*pMap)[strKey];
		if (pTemp)
			m_ptrSysRefChangeList.AddTail(pTemp);
	}
	strSql.Format(L"DELETE FROM TB_CHANNEL WHERE FACP_ID=%d ", pOld->GetFacpID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

// 	strSql.Format(L"SELECT * FROM TB_RELAY_LIST WHERE FACP_ID=%d ", pOld->GetFacpID());
// 	if (pDB->OpenQuery(strSql) == FALSE)
// 		return 0;
// 	nCnt = pDB->GetRecordCount();
// 	for (i = 0; i < nCnt; i++)
// 	{
// 		if (pDB->GetFieldValue(L"UNIT_ID", niu) == FALSE)
// 			continue;
// 		if (pDB->GetFieldValue(L"CHN_NUM", nC) == FALSE)
// 			continue;
// 		nU = m_pRefFasSysData->CvtUnitIDToNum(nF, niu);
// 		strKey = GF_GetSysDataKey(SE_CHANNEL, nF, nU, nC);
// 		pTemp = (*pMap)[strKey];
// 		if (pTemp)
// 			m_ptrSysRefChangeList.AddTail(pTemp);
// 	}
// 	strSql.Format(L"DELETE * FROM TB_RELAY_LIST WHERE FACP_ID=%d ", pOld->GetFacpID());
// 	if (pDB->ExecuteSql(strSql) == FALSE)
// 		return 0;

	return 1;
}

int CFormRelayEdit::DataDeleteUnit(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;

	// device , chn , unit,facp
	if (m_pCurData == nullptr || m_pCurData->nDataType != TTYPE_DEV_UNIT || m_pCurData->pData == nullptr)
		return 0;

	int nCnt, i;
	int nF, nU, nC;
	int nif, niu;
	CString strSql, strKey;
	CDataSystem * pSys, *pTemp;
	CDataUnit* pOld;
	CMapSystemData * pMap;
	

	pSys = (CDataSystem*)m_pCurData->pData;
	if (pSys->GetDataType() != SE_UNIT || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataUnit*)pSys->GetSysData();
	pMap = m_pRefFasSysData->GetSystemData();
	if (pMap == nullptr)
		return 0;
	nif = pOld->GetFacpID();
	nU = pOld->GetUnitNum();
	niu = pOld->GetUnitID();
	nF = m_pRefFasSysData->CvtFacpIDToNum(nif);
	strSql.Format(L"DELETE FROM TB_UNIT WHERE FACP_ID=%d AND UNIT_ID=%d"
		, pOld->GetFacpID(), pOld->GetUnitID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	strKey = GF_GetSysDataKey(SE_UNIT, nF, nU);
	pTemp = (*pMap)[strKey];
	if (pTemp)
		m_ptrSysRefChangeList.AddTail(pTemp);

	


	strSql.Format(L"SELECT * FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d"
		, pOld->GetFacpID(), pOld->GetUnitID());
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (pDB->GetFieldValue(L"CHN_NUM", nC) == FALSE)
			continue;
		strKey = GF_GetSysDataKey(SE_CHANNEL, nF, nU, nC);
		pTemp = (*pMap)[strKey];
		if (pTemp)
			m_ptrSysRefChangeList.AddTail(pTemp);
	}

	strSql.Format(L"DELETE FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d"
		, pOld->GetFacpID(), pOld->GetUnitID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	strSql.Format(L"DELETE FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d"
		, pOld->GetFacpID(), pOld->GetUnitID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1;
}

int CFormRelayEdit::DataDeleteChn(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;

	// device , chn , unit,facp
	if (m_pCurData == nullptr || m_pCurData->nDataType != TTYPE_DEV_CHANNEL || m_pCurData->pData == nullptr)
		return 0;

	int nF, nU, nC;
	int nif, niu, nic;
	CString strSql, strKey;
	CDataSystem * pSys, *pTemp;
	CDataChannel* pOld;
	CMapSystemData * pMap;
	pSys = (CDataSystem*)m_pCurData->pData;
	if (pSys->GetDataType() != SE_CHANNEL || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataChannel*)pSys->GetSysData();
	pMap = m_pRefFasSysData->GetSystemData();
	if (pMap == nullptr)
		return 0;

	nif = pOld->GetFacpID();
	niu = pOld->GetUnitID();
	nic = pOld->GetChnID();
	nC = pOld->GetChnNum();
	nU = m_pRefFasSysData->CvtUnitIDToNum(nif , niu);
	nF = m_pRefFasSysData->CvtFacpIDToNum(nif);


	strSql.Format(L"DELETE FROM TB_CHANNEL WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d "
		, pOld->GetFacpID(), pOld->GetUnitID() , pOld->GetChnID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	strKey = GF_GetSysDataKey(SE_CHANNEL, nF, nU, nC);
	pTemp = (*pMap)[strKey];
	if (pTemp)
		m_ptrSysRefChangeList.AddTail(pTemp);

	strSql.Format(L"DELETE FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d "
		, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;


	return 1;
}

int CFormRelayEdit::DataDeleteRelay(YAdoDatabase * pDB)
{
	if (m_pRefFasSysData == nullptr)
		return 0;

	// device , chn , unit,facp
	if (m_pCurData == nullptr || m_pCurData->nDataType != TTYPE_DEV_DEVICE || m_pCurData->pData == nullptr)
		return 0;

	CString strSql;
	CDataSystem * pSys;
	CDataDevice* pOld;
	pSys = (CDataSystem*)m_pCurData->pData;
	if (pSys->GetDataType() != SE_RELAY || pSys->GetSysData() == nullptr)
		return 0;
	pOld = (CDataDevice*)pSys->GetSysData();

	strSql.Format(L"DELETE FROM TB_RELAY_LIST WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
		, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(),pOld->GetDeviceID());
	if (pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1;
}

void CFormRelayEdit::CopyTreeItem(ST_TREEITEM * pSource, ST_TREEITEM * pDest)
{
	if (pSource == nullptr || pSource->pData == nullptr)
		return;
	if (pDest == nullptr || pDest->pData == nullptr)
		return;

	CDataSystem * pSys ;
	pDest->hItem = pSource->hItem;
	pDest->hParent = pSource->hParent;
	pDest->nDataType = pSource->nDataType;
	pSys = (CDataSystem *)pDest->pData;
	pSys->CopyData((CDataSystem*)pSource->pData);
	pDest->pData = pSys;
}

void CFormRelayEdit::DeleteTreeItem(ST_TREEITEM * pNewData)
{
	if (pNewData == nullptr || pNewData->pData == nullptr)
		return;
	CDataSystem * pSys = (CDataSystem*)pNewData->pData;
	pSys->DeleteData();
	memset((void*)pNewData, 0, sizeof(ST_TREEITEM));
	pNewData->pData = pSys;
}

void CFormRelayEdit::SetNewTreeItem(int nType, ST_TREEITEM * pNewData)
{
	CDataSystem * pSys = (CDataSystem*)pNewData->pData;
	pSys->SetNewSysData(nType);
	pNewData->hItem = nullptr;
	pNewData->hParent = nullptr;
	pNewData->nDataType = nType;
	pNewData->pData = pSys;
}

void CFormRelayEdit::RemoveChangeDevice()
{
	ST_CHANGE_LINKDATA * pChgLink;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice *pDev;
	CDataPattern * pPtn = nullptr;
	while (m_ptrChangeList.IsEmpty() != TRUE)
	{
		pChgLink = (ST_CHANGE_LINKDATA *)m_ptrChangeList.RemoveHead();
		if (pChgLink == nullptr)
			continue;

		if (pChgLink->pChange)
		{
			switch (pChgLink->btInOutType)
			{
			case CHANGE_LK_FACP:
				pFacp = (CDataFacp*)pChgLink->pChange;
				if (pFacp)
				{
					delete pFacp;
					pFacp = nullptr;
				}
				break;
			case CHANGE_LK_UNIT:
				pUnit = (CDataUnit*)pChgLink->pChange;
				if (pUnit)
				{
					delete pUnit;
					pUnit = nullptr;
				}
				break;
			case CHANGE_LK_CHN:
				pChn = (CDataChannel*)pChgLink->pChange;
				if (pChn)
				{
					delete pChn;
					pChn = nullptr;
				}
				break;
			case CHANGE_LK_DEV:
			case CHANGE_LK_PATTERN:
			case CHANGE_LK_INPUT:
				pDev = (CDataDevice*)pChgLink->pChange;
				if (pDev)
				{
					delete pDev;
					pDev = nullptr;
				}
				break;
// 			case CHANGE_LK_OUTPUT:
// 				plnk = (CDataLinked *)pChgLink->pChange;
// 
// 				if (plnk)
// 				{
// 					delete plnk;
// 					plnk = nullptr;
// 				}
// 				break;
			}
		}
		
		delete pChgLink;
		pChgLink = nullptr;
	}

	m_ptrSysRefChangeList.RemoveAll();
	
// 	CMapChangeDevice::iterator it;
// 	for (it = m_mapChangeDevice.begin(); it != m_mapChangeDevice.end(); it++)
// 	{
// 		if (it->second == nullptr)
// 			continue;
// 		pDev = it->second;
// 		delete pDev;
// 		pDev = nullptr;
// 
// 	}
	m_mapChangeDevice.clear();
}

int CFormRelayEdit::MakeChangeDeviceMap(ST_TREEITEM * pNewData, int nDefaultAction)
{
	RemoveChangeDevice();
	if (pNewData == nullptr || pNewData->pData == nullptr)
		return 0; 

	if (IsSameAddress(m_pCurData, pNewData) == 1)
		return 0;

	MakeInputChangeData(pNewData, nDefaultAction);
	MakeInputLinkChangeData(pNewData);
	MakeOutputChangeData(pNewData, nDefaultAction);
	MakePatternChangeData(pNewData, nDefaultAction);
	return 1;
}

int CFormRelayEdit::MakeDeleteDeviceMap(ST_TREEITEM * pCurData)
{
	RemoveChangeDevice();
	if (pCurData == nullptr || pCurData->pData == nullptr)
		return 0;
	MakeInputDeleteData(pCurData);
	MakeOutputDeleteData(pCurData);
	MakeUsedLinkDeleteData(pCurData);
	MakePatternDeleteData(pCurData);
	return 1;
}

int CFormRelayEdit::GetChangeNumber(CDataDevice * pDev, int nbF, int nbU, int nbC, int nbD
	, int iF, int iU, int iC, int iD
	, int &nFid, int &nUid, int &nCid, int &nDevid
	, int &nFNum, int &nUNum, int &nCNum, int &nDevNum)
{
	// Return
	/*
	0 : 변경되지 않음
	-1: 변경 실패 
	1: 변경 성공
	*/
	int nRet = 0;
	if (nbF >= 0)
	{
		nFNum = nbF;
		nFid = iF;
		nRet = 1;
	}
	else
	{
		nFNum = pDev->GetFacpNum();
		nFid = pDev->GetFacpID();
	}

	if (nbU >= 0)
	{
		nUNum = nbU;
		nUid = iU ;
		nRet = 1;
	}
	else
	{
		nUNum = pDev->GetUnitNum();
		nUid = pDev->GetUnitID();
	}

	if (nbC >= 0)
	{
		nCNum = nbC;
		nCid = iC;
		nRet = 1;
	}
	else
	{
		nCNum = pDev->GetChnNum();
		nCid = pDev->GetChnID();
	}

	if (nbD >= 0)
	{
		nDevNum = nbD;
		nDevid = iD;
		nRet = 1;
	}
	else
	{
		nDevNum = pDev->GetDeviceNum();
		nDevid = pDev->GetDeviceID();
	}
	return nRet;
}

int CFormRelayEdit::ChangeDevice(int nType , LPVOID pNewSrc, LPVOID pOldData, int nbF, int nbU, int nbC, int nbD
	, int iF, int iU, int iC, int iD
	, int &nFid, int &nUid, int &nCid, int &nDevid

)
{
	/*
	1. 변경 해야되는지 확인
		1.1. 현재 Device가 변경을 해야되는 확인
		1.2. 변경 하지 않을 때 Return : 0
		1.3. 변경 했을 때 Return : 1
	2. 변경을 해야 할 때 변경가능 여부 확인
		2.1. 변경될 Device가 기존 Device Map에 이미 있는지 확인
		2.2. 없으면 변경될 Device를 변경한다.
		2.3. 있으면 유지 - 이미 있음으로 표시
		
	3. 변경 가능할 때 변경될 데이터 Map에 있는지 확인
		3.1. 변경할 Address 생성한다.
		3.2. 기존 Addres로 확인한다.
		3.3. 있으면 변경하지 않는다.
		3.4. 변경 했음을 Return 한다.
	4. CDataLinked가 있는지 확인
		4.1. CDataLinked가 있으면 현재 함수를 다시 호출
		4.2. 변경 했음을 Return 했을 때 CDataLinked를 새로 생성하고 기존 CDataLinked를 삭제한다.
	5. 

	*/
	
	int nRet = 0;
// 	CString strKey;
// 	CDataDevice * pTemp , * pSrcDev ,*pOldCvtDev , *pNewCvtDev;
// 	CDataPattern * pNewSrcPtn = nullptr;
// 	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
// 	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;
// 	ST_CHANGE_LINKDATA * pChgLink , * pNewLinked;
// 	CMapSystemData * pMap;
// 	CDataSystem * pData;
// 
// 	POSITION pos;
// 	CDataLinked * pLink , * pCvtLnk;
// 	CPtrList * pNewList = nullptr;
// 	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
// 	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;
// 	
// 	if (nType == CHANGE_LK_OUTPUT)
// 	{
// 		pCvtLnk = (CDataLinked *)pOldData;
// 		pOldCvtDev = m_pRefFasSysData->GetDevice(
// 			pCvtLnk->GetTgtFacp(), pCvtLnk->GetTgtUnit()
// 			, pCvtLnk->GetTgtChn(), pCvtLnk->GetTgtDev()
// 		);
// 		if (pOldCvtDev == nullptr)
// 			return -1;
// 	}
// 	else
// 		pOldCvtDev = (CDataDevice*)pOldData;
// 	//1.1. 입력된 CDataDevice를 변경해야되는지 확인
// 	nRet = GetChangeNumber(pOldCvtDev, nbF, nbU, nbC, nbD, iF, iU, iC, iD
// 		, nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);
// 	
// 	nFid = nT_Fid;
// 	nUid = nT_Uid;
// 	nCid = nT_Cid;
// 	nDevid = nT_Did;
// 
// 	if (nRet <= 0)
// 	{
// 		// 변경하지 않아도 됨
// 		return 0;
// 	}
// 	
// 	pChgLink = new ST_CHANGE_LINKDATA;
// 	pChgLink->btInOutType = nType;
// 	pChgLink->btChangeType = CHANGE_ACTION_UPDATE;
// 	pChgLink->pRefInput = pNewSrc;
// 	pChgLink->pRefOld = pOldData;
// 
// 	// 변경될 데이터 할당
// 	switch (nType)
// 	{
// 	case CHANGE_LK_INPUT:
// 		pChgLink->pChange = new CDataDevice;
// 		((CDataDevice*)(pChgLink->pChange))->CopyData((CDataDevice*)pOldData);
// 		break;
// 	case CHANGE_LK_OUTPUT:
// 		pSrcDev = (CDataDevice *)pNewSrc;
// 		pChgLink->pChange = new CDataLinked;
// 		((CDataLinked*)(pChgLink->pChange))->CopyData((CDataLinked*)pOldData);
// 		break;
// 	case CHANGE_LK_PATTERN:
// 		pChgLink->pChange = new CDataDevice;
// 		((CDataDevice*)(pChgLink->pChange))->CopyData((CDataDevice*)pOldData);
// 		break;
// 	}
// 	// 2.1.변경될 Device가 기존 Device Map에 이미 있는지 확인
// 	strKey = GF_GetSysDataKey(SE_RELAY, nT_FNum, nT_UNum, nT_CNum, nT_DNum);
// 	pNewCvtDev = m_pRefFasSysData->GetDevice(strKey);
// 	if (pNewCvtDev != nullptr)
// 	{
// 		// 중복
// 		//continue;
// 		pChgLink->btChangeType = CHANGE_ACTION_KEEP;
// 		nRet = 0;
// 	}
// 	else
// 	{
// 		// 기존에 데이터 없음
// 		if (CHANGE_LK_OUTPUT == nType)
// 		{
// 			((CDataLinked*)pChgLink->pChange)->SetLinkData(
// 				nT_Fid, nT_Uid, nT_Cid, nT_Did
// 				, LK_TYPE_RELEAY , pCvtLnk->GetLogicType(), pCvtLnk->GetLogicID()
// 				, pSrcDev->GetFacpID(), pSrcDev->GetUnitID()
// 				, pSrcDev->GetChnID(), pSrcDev->GetDeviceID()
// 			);
// 			// 새로 생성하는 데이터는 기존 Device에서 복사해온다.
// 			pNewCvtDev = new CDataDevice;
// 			pNewCvtDev->CopyData(pOldCvtDev);
// 			// 연동데이터(CDataLinked)를 변경하면서 변경되는 Device도 변경해준다.
// 			pNewLinked = new ST_CHANGE_LINKDATA;
// 			pNewLinked->btInOutType = CHANGE_LK_INPUT;
// 			pNewLinked->btChangeType = CHANGE_ACTION_UPDATE;
// 			pNewLinked->pRefInput = nullptr;
// 			pNewLinked->pRefOld = pOldCvtDev;
// 			pNewLinked->pChange = pNewCvtDev;
// 			m_ptrChangeList.AddTail(pNewLinked);
// 			//pNewCvtDev->SetAddressInfo(nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);
// 		}
// 		else
// 		{
// 			pNewCvtDev = ((CDataDevice*)pChgLink->pChange);
// 			//pNewCvtDev->SetAddressInfo(nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);
// 		}
// 		pNewCvtDev->SetAddressInfo(nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);
// 		nRet = 1;
// 	}
// 	m_ptrChangeList.AddTail(pChgLink);
// 	// [KHS 2020-5-11 10:00:58] 
// 	// 변경되는 device의 linked 데이터 수정
// 	pSrcDev = m_mapChangeDevice[pOldCvtDev->GetDevKey()];
// 	if (pSrcDev == nullptr)
// 		m_mapChangeDevice[pOldCvtDev->GetDevKey()] = pNewCvtDev;
// 	
// 
// 	pNewList = pNewCvtDev->GetLinkedList();
// 	pos = pNewList->GetHeadPosition();
// 	while (pos)
// 	{
// 		pLink = (CDataLinked*)pNewList->GetNext(pos);
// 		if (pLink == nullptr)
// 			continue; 
// 		if (pLink->GetLinkType() != LK_TYPE_RELEAY)
// 			continue;
// 		pTemp = m_pRefFasSysData->GetDevice(pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());
// 		if (pTemp == nullptr)
// 			continue; 
// 
// 
// 		if (ChangeDevice(CHANGE_LK_OUTPUT,(LPVOID)pNewCvtDev, pTemp, nbF, nbU, nbC, nbD, iF, iU, iC, iD
// 			,nT_Fid, nT_Uid, nT_Cid, nT_Did) == 1)
// 		{
// 			pLink->SetTgtFacp(nT_Fid);
// 			pLink->SetTgtUnit(nT_Uid);
// 			pLink->SetTgtChn(nT_Cid);
// 			pLink->SetTgtDev(nT_Did);
// 		}
// 	}
	return nRet;
}


// [KHS 2020-5-12 14:08:24] 
// Old Version
// int CFormRelayEdit::MakeInputChangeData(ST_TREEITEM * pNewData, int nDefaultAction)
//  {
//  	if (pNewData == nullptr || pNewData->pData == nullptr)
//  		return 0;
//  
//  /*
//  	1. 현재 선택된 항목에 ID , Number 설정 - 현재 선택된 항목이 FACP 이면 FACP ID/Number외에는 0보다 작은 값이다.
//  	2. 변경 대상 회로 찾기 - 
//  	3. ST_CHANGE_LINKDATA 설정 - 변경될 회로 설정하면서 중복 검사
//  
//  */
//  	CString strKey;
//  	int nO_Fid, nO_Uid, nO_Cid, nO_Did;
//  	int nO_FNum, nO_UNum, nO_CNum, nO_DNum;
//  	int nN_Fid, nN_Uid, nN_Cid, nN_Did;
//  	int nN_FNum, nN_UNum, nN_CNum, nN_DNum;
//  
//  	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
//  	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;
//  	//int nFacp, nUnit, nChn, nDev, nTemp;
//  	ST_CHANGE_LINKDATA * pChgLink;
//  	CMapSystemData * pMap;
//  	CDataSystem * pData;
//  	CDataDevice * pDev, *pTemp;
//  	CMapSystemData::iterator it;
//  	
//  	nO_Fid = nO_Uid = nO_Cid = nO_Did = -1;
//  	nO_FNum = nO_UNum = nO_CNum = nO_DNum = -1;
//  	nN_Fid = nN_Uid = nN_Cid = nN_Did = -1;
//  	nN_FNum = nN_UNum = nN_CNum = nN_DNum = -1;
//  
//  	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
//  	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;
//  	GetIDNum(pNewData, nN_Fid, nN_Uid, nN_Cid, nN_Did, nN_FNum, nN_UNum, nN_CNum, nN_DNum);
//  	GetIDNum(m_pCurData, nO_Fid, nO_Uid, nO_Cid, nO_Did, nO_FNum, nO_UNum, nO_CNum, nO_DNum);
//  	pMap = m_pRefFasSysData->GetSystemData();
//  	for (it = pMap->begin(); it != pMap->end(); it++)
//  	{
//  		pData = it->second;
//  		if (pData == nullptr)
//  			continue;
//  
//  		if (pData->GetDataType() != SE_RELAY)
//  			continue;
//  		if (pData->GetSysData() == nullptr)
//  			continue;
//  
//  		pDev = (CDataDevice *)pData->GetSysData();
//  		
//  		if (ChangeDevice(CHANGE_LK_INPUT, nullptr, pDev
//  			, nN_Fid, nN_Uid, nN_Cid, nN_Did
//  			, nN_FNum, nN_UNum, nN_CNum, nN_DNum
//  			,nT_Fid, nT_Uid, nT_Cid, nT_Did) == 1)
//  		{
//  			// ChangeDevice에서 현재 Device를 변경 목록에 넣는다.
//  		}
//  	}
//  	return 1;
//  }
int CFormRelayEdit::MakeInputChangeData(ST_TREEITEM * pNewData, int nDefaultAction)
{
	if (pNewData == nullptr || pNewData->pData == nullptr)
		return 0;

	int nO_Fid, nO_Uid, nO_Cid, nO_Did;
	int nO_FNum, nO_UNum, nO_CNum, nO_DNum;
	int nN_Fid, nN_Uid, nN_Cid, nN_Did;
	int nN_FNum, nN_UNum, nN_CNum, nN_DNum;
	  
	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;
	CDataFacp * pFacp , * pNF;
	CDataUnit * pUnit , * pNU;
	CDataChannel * pChn, *pNC;
	CDataDevice * pOldDev , * pNewDev, *pTemp;
	CDataSystem *pSys , *pNewSys;
	CDataPattern * pPtn = nullptr;
	ST_CHANGE_LINKDATA * pChgLink;
	CString strSql,strKey , strtemp;
	int nCnt, i,nRet=0;
	int nFacp, nUnit, nChn, nDev;
	int nlf, nlu, nlc;

	nO_Fid = nO_Uid = nO_Cid = nO_Did = -1;
	nO_FNum = nO_UNum = nO_CNum = nO_DNum = -1;
	nN_Fid = nN_Uid = nN_Cid = nN_Did = -1;
	nN_FNum = nN_UNum = nN_CNum = nN_DNum = -1;
	nlf = nlu = nlc = -1;
	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;
	GetIDNum(pNewData, nN_Fid, nN_Uid, nN_Cid, nN_Did, nN_FNum, nN_UNum, nN_CNum, nN_DNum);
	GetIDNum(m_pCurData, nO_Fid, nO_Uid, nO_Cid, nO_Did, nO_FNum, nO_UNum, nO_CNum, nO_DNum);

	pSys = (CDataSystem *)m_pCurData->pData;
	pNewSys = (CDataSystem *)pNewData->pData;
	switch (m_pCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_RELAY_LIST "
			L"WHERE FACP_ID = %d  "
			L"ORDER BY FACP_ID, UNIT_ID,CHN_ID, RLY_ID"
			, pFacp->GetFacpID()
		);
// 		if (nN_FNum != nO_FNum)
// 		{
// 			pNF = m_pRefFasSysData->GetFacpByNum(nN_FNum);
// 			if (pNF == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_FACP;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->pRefOld = pFacp;
// 				pChgLink->pChange = (CDataFacp*)pNewSys->GetSysData();
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// 		}
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_RELAY_LIST "
			L"WHERE FACP_ID = %d AND UNIT_ID = %d "
			L"ORDER BY FACP_ID, UNIT_ID,CHN_ID, RLY_ID"
			, pUnit->GetFacpID()
			, pUnit->GetUnitID()
		);
	
// 		if (nN_FNum != nO_FNum
// 			|| nN_UNum != nO_UNum)
// 		{
// 			pFacp = m_pRefFasSysData->GetFacpByNum(nN_FNum);
// 			if (pFacp == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_UNIT;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefOld = pFacp;
// 				pNF = new CDataFacp; 
// 				pNF->CopyData(pFacp);
// 				pNF->SetFacpID(nN_Fid);
// 				pNF->SetFacpNum(nN_FNum);
// 				strtemp.Format(L"FACP_%02d", nN_FNum);
// 				pNF->SetFacpName(strtemp);
// 				pChgLink->pChange = pNF;
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// 			pNU = m_pRefFasSysData->GetUnitByNum(nN_FNum , nN_UNum);
// 			if (pNU == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_UNIT;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->pRefOld = nullptr;
// 				pChgLink->pChange = (CDataUnit*)pNewSys->GetSysData();
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// 		}
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_RELAY_LIST "
			L"WHERE FACP_ID = %d AND UNIT_ID = %d AND CHN_ID = %d "
			L"ORDER BY FACP_ID, UNIT_ID,CHN_ID, RLY_ID"
			, pChn->GetFacpID()
			, pChn->GetUnitID(), pChn->GetChnID()
		);
// 		if (nN_FNum != nO_FNum
// 			|| nN_UNum != nO_UNum
// 			|| nN_CNum != nO_CNum
// 			)
// 		{
// 			pFacp = m_pRefFasSysData->GetFacpByNum(nN_FNum);
// 			if (pFacp == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_UNIT;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->pRefOld = pFacp;
// 				pNF = new CDataFacp;
// 				pNF->CopyData(pFacp);
// 				pNF->SetFacpID(nN_Fid);
// 				pNF->SetFacpNum(nN_FNum);
// 				strtemp.Format(L"FACP_%02d", nN_FNum);
// 				pNF->SetFacpName(strtemp);
// 				pChgLink->pChange = pNF;
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// 			pNU = m_pRefFasSysData->GetUnitByNum(nN_FNum, nN_UNum);
// 			if (pNU == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_UNIT;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->pRefOld = nullptr;
// 				pNU = new CDataUnit;
// 				strtemp.Format(L"Unit_%02d%02d", nN_FNum,nN_UNum);
// 				//pNF->SetFacpName(strtemp);
// 				pNU->SetUnitData(nN_Fid, nN_UNum, nN_Uid
// 					, strtemp);
// 				pChgLink->pChange = pNU;
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// 			pNC = m_pRefFasSysData->GetChannelByNum(nN_FNum, nN_UNum, nN_CNum);
// 			if (pNC == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_CHN;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->pRefOld = nullptr;
// 				pChgLink->pChange = (CDataChannel*)pNewSys->GetSysData();
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// 		}
		break;
	case TTYPE_DEV_DEVICE:
		pTemp = (CDataDevice *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_RELAY_LIST "
			L"WHERE FACP_ID = %d AND UNIT_ID = %d AND CHN_ID = %d AND RLY_ID = %d "
			L"ORDER BY FACP_ID, UNIT_ID,CHN_ID, RLY_ID"
			, pTemp->GetFacpID(), pTemp->GetUnitID()
			, pTemp->GetChnID(), pTemp->GetDeviceID()
		);

// 
// 		if (nN_FNum != nO_FNum
// 			|| nN_UNum != nO_UNum
// 			|| nN_CNum != nO_CNum
// 			|| nN_DNum != nO_DNum
// 			)
// 		{
// 			pFacp = m_pRefFasSysData->GetFacpByNum(nN_FNum);
// 			if (pFacp == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_UNIT;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->pRefOld = nullptr;
// 				pNF = new CDataFacp;
// 				pNF->CopyData(pFacp);
// 				pNF->SetFacpID(nN_Fid);
// 				pNF->SetFacpNum(nN_FNum);
// 				strtemp.Format(L"FACP_%02d", nN_FNum);
// 				pNF->SetFacpName(strtemp); pChgLink->pChange = pNF;
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// 			pNU = m_pRefFasSysData->GetUnitByNum(nN_FNum, nN_UNum);
// 			if (pNU == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_UNIT;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->pRefOld = nullptr;
// 				pNU = new CDataUnit;
// 				strtemp.Format(L"Unit_%02d%02d", nN_FNum, nN_UNum);
// 				pNU->SetUnitData(nN_Fid, nN_UNum, nN_Uid
// 					, strtemp);
// 				pChgLink->pChange = pNU;
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// 			pNC = m_pRefFasSysData->GetChannelByNum(nN_FNum, nN_UNum, nN_CNum);
// 			if (pNC == nullptr)
// 			{
// 				pChgLink = new ST_CHANGE_LINKDATA;
// 				pChgLink->btInOutType = CHANGE_LK_CHN;
// 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// 				pChgLink->pRefInput = nullptr;
// 				pChgLink->pRefOld = nullptr;
// 				pNC = new CDataChannel;
// 				strtemp.Format(L"Channel_%02d%02d-%d", nN_FNum, nN_UNum, nN_CNum);
// 				pNC->SetChannelData(nN_Fid, nN_Uid, nN_Cid, nN_CNum
// 					, strtemp);
// 				pChgLink->pChange = pNC;
// 				m_ptrChangeList.AddTail(pChgLink);
// 			}
// // 			pNewDev = m_pRefFasSysData->GetDevice(nN_Fid, nN_Uid, nN_Cid, nN_Did);
// // 			if (pNewDev == nullptr)
// // 			{
// // 				pChgLink = new ST_CHANGE_LINKDATA;
// // 				pChgLink->btInOutType = CHANGE_LK_DEV;
// // 				pChgLink->btChangeType = CHANGE_ACTION_ADD;
// // 				pChgLink->pRefInput = nullptr;
// // 				pChgLink->pRefOld = pTemp;
// // 				pNewDev = new CDataDevice;
// // 				pNewDev->CopyExceptLink(pTemp);
// // 				pNewDev->SetAddressInfo(nN_Fid, nN_Uid, nN_Cid, nN_Did, nN_FNum, nN_UNum, nN_CNum, nN_DNum);
// // 				pChgLink->pChange = (CDataChannel*)pSys->GetSysData();
// // 				m_ptrChangeList.AddTail(pChgLink);
// // 			}
// 		}
		break;
	}

	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;

		if (pDB->GetFieldValue(L"FACP_ID", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"UNIT_ID", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"CHN_ID", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"RLY_ID", nDev) == FALSE)
			nDev = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
		{
			pDB->MoveNext();
			continue;
		}
		pOldDev = m_pRefFasSysData->GetDeviceByID(nFacp, nUnit, nChn, nDev);

		if (pOldDev == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

		nRet = GetChangeNumber(pOldDev, nN_FNum, nN_UNum, nN_CNum, nN_DNum, nN_Fid, nN_Uid, nN_Cid, nN_Did
			, nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);

		if (nRet <= 0) // 변경하지 않음
		{
			pDB->MoveNext(); 
			continue;
		}
		// 변경될 수신기 , 유닛,채널이 있는지 확인한다.
		if (nlf != nT_FNum || nlu != nT_UNum || nlc != nT_CNum)
		{
			if (nlf != nT_FNum)
			{
				pFacp = m_pRefFasSysData->GetFacpByNum(nT_FNum);
				if (pFacp == nullptr)
				{
					pChgLink = new ST_CHANGE_LINKDATA;
					pChgLink->btInOutType = CHANGE_LK_FACP;
					pChgLink->btChangeType = CHANGE_ACTION_ADD;
					pChgLink->pRefInput = nullptr;
					pChgLink->pRefOld = nullptr;
					pNF = new CDataFacp;
					pNF->CopyData(pFacp);
					pNF->SetFacpID(nT_Fid);
					pNF->SetFacpNum(nT_FNum);
					strtemp.Format(L"FACP_%02d", nT_FNum);
					pNF->SetFacpName(strtemp); 
					pChgLink->pChange = pNF;
					m_ptrChangeList.AddTail(pChgLink);
				}
			}

			if (nlf != nT_FNum || nlu != nT_UNum)
			{
				pNU = m_pRefFasSysData->GetUnitByNum(nT_FNum, nT_UNum);
				if (pNU == nullptr)
				{
					pChgLink = new ST_CHANGE_LINKDATA;
					pChgLink->btInOutType = CHANGE_LK_UNIT;
					pChgLink->btChangeType = CHANGE_ACTION_ADD;
					pChgLink->pRefInput = nullptr;
					pChgLink->pRefOld = nullptr;
					pNU = new CDataUnit;
					strtemp.Format(L"Unit_%02d%02d", nT_FNum, nT_UNum);
					pNU->SetUnitData(nT_Fid, nT_FNum, nT_UNum, nT_Uid
						, strtemp);
					pChgLink->pChange = pNU;
					m_ptrChangeList.AddTail(pChgLink);
				}
			}

			if (nlf != nT_FNum || nlu != nT_UNum || nlc != nT_CNum)
			{
				pNC = m_pRefFasSysData->GetChannelByNum(nT_FNum, nT_UNum, nT_CNum);
				if (pNC == nullptr)
				{
					pChgLink = new ST_CHANGE_LINKDATA;
					pChgLink->btInOutType = CHANGE_LK_CHN;
					pChgLink->btChangeType = CHANGE_ACTION_ADD;
					pChgLink->pRefInput = nullptr;
					pChgLink->pRefOld = nullptr;
					pNC = new CDataChannel;
					strtemp.Format(L"Channel_%02d%02d-%d", nT_FNum, nT_UNum, nT_CNum);
					pNC->SetChannelData(nT_Fid, nT_Uid, nT_Cid
						, nT_FNum, nT_UNum, nT_CNum
						, strtemp);
					pChgLink->pChange = pNC;
					m_ptrChangeList.AddTail(pChgLink);
				}
			}
			nlf = nT_FNum;
			nlu = nN_UNum;
			nlc = nT_CNum;
		}
		strKey = GF_GetSysDataKey(SE_RELAY, nT_FNum, nT_UNum, nT_CNum, nT_DNum);
		pNewDev = m_pRefFasSysData->GetDevice(strKey);
		if (pNewDev != nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pNewDev = new CDataDevice;
		pNewDev->CopyExceptLink(pOldDev);
		pChgLink = new ST_CHANGE_LINKDATA;
		pChgLink->btInOutType = CHANGE_LK_INPUT;
		pChgLink->btChangeType = CHANGE_ACTION_UPDATE;
		pChgLink->pRefInput = nullptr;
		pChgLink->pRefOld = pOldDev;
		pChgLink->pChange = pNewDev;
		pTemp = m_mapChangeDevice[pOldDev->GetDevKey()];
		if (pTemp == nullptr)
			m_mapChangeDevice[pOldDev->GetDevKey()] = pNewDev;
		pNewDev->SetAddressInfo(nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);
		m_ptrChangeList.AddTail(pChgLink);
		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}

int CFormRelayEdit::MakeInputLinkChangeData(ST_TREEITEM * pNewData)
{
	if (pNewData == nullptr || pNewData->pData == nullptr)
		return 0;
	//int nFid, nUid, nCid, nDid;
	//int nFNum, nUNum, nCNum, nDNum;
	int nN_Fid, nN_Uid, nN_Cid, nN_Did;
	int nN_FNum, nN_UNum, nN_CNum, nN_DNum;
	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;
	int nInType, nOutType, nEqName, nCont;

	CString strSql;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataEquip * pEq;
	CDataDevice * pDev = nullptr, *pNewDev = nullptr, *pTemp = nullptr;
	CDataSystem *pSys;
	CDataLinked *pOldLink = nullptr , * pNewLink = nullptr;
	ST_CHANGE_LINKDATA * pChgLink;
	//nFNum = nUNum = nCNum = nDNum = -1;
	pSys = (CDataSystem *)m_pCurData->pData;

	nN_Fid = nN_Uid = nN_Cid = nN_Did = -1;
	nN_FNum = nN_UNum = nN_CNum = nN_DNum = -1;

	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;

	switch (m_pCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pSys->GetSysData();
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			L"ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY "
			, LK_TYPE_RELEAY, pFacp->GetFacpID()
		);
		nN_Fid = pFacp->GetFacpID();
		nN_FNum = pFacp->GetFacpNum();
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pSys->GetSysData();
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			L"AND TGT_UNIT=%d "
			L"ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY "
			, LK_TYPE_RELEAY, pUnit->GetFacpID()
			, pUnit->GetUnitID()
		);
		nN_FNum = m_pRefFasSysData->CvtFacpIDToNum(pUnit->GetFacpID());
		nN_UNum = pUnit->GetUnitNum();
		nN_Fid = pUnit->GetFacpID();
		nN_Uid = pUnit->GetUnitID();
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pSys->GetSysData();
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			L"AND TGT_UNIT=%d AND TGT_CHN=%d "
			L"ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY "
			, LK_TYPE_RELEAY, pChn->GetFacpID()
			, pChn->GetUnitID(), pChn->GetChnID()
		);
		nN_FNum = m_pRefFasSysData->CvtFacpIDToNum(pChn->GetFacpID());
		nN_UNum = m_pRefFasSysData->CvtUnitIDToNum(pChn->GetFacpID(), pChn->GetUnitID());
		nN_CNum = pChn->GetChnNum();
		nN_Fid = pChn->GetFacpID();
		nN_Uid = pChn->GetUnitID();
		nN_Cid = pChn->GetChnID();
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pSys->GetSysData();
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			L"AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
			L"ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY "
			, LK_TYPE_RELEAY, pDev->GetFacpID()
			, pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
		);
		nN_FNum = m_pRefFasSysData->CvtFacpIDToNum(pDev->GetFacpID());
		nN_UNum = m_pRefFasSysData->CvtUnitIDToNum(pDev->GetFacpID(), pDev->GetUnitID());
		nN_CNum = m_pRefFasSysData->CvtChannelIDToNum(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID());
		nN_DNum = pDev->GetDeviceNum();

		nN_Fid = pDev->GetFacpID();
		nN_Uid = pDev->GetUnitID();
		nN_Cid = pDev->GetChnID();
		nN_Did = pDev->GetDeviceID();
		break;
	}

	int nCnt, i , nRet = 0;
	int nFacp, nUnit, nChn, nDev;
	int nlf, nlu, nlc, nld; //< 마지막 ID
	int ntf, ntu, ntc, ntd;
	BOOL bFirstAdd = FALSE;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	nlf = nlu = nlc = nld = -1;
	ntf = ntu = ntc = ntd = -1;
	nCnt = pDB->GetRecordCount();
	pDev = nullptr;
	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;
		ntf = ntu = ntc = ntd = -1;
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

		if (pDev == nullptr || pNewDev == nullptr 
			|| nlf != nFacp || nlu != nUnit || nlc != nChn || nld != nDev)
		{
			nlf = nFacp;
			nlu = nUnit;
			nlc = nChn;
			nld = nDev;
			pDev = m_pRefFasSysData->GetDeviceByID(nlf, nlu, nlc, nld);
			pNewDev = m_mapChangeDevice[pDev->GetDevKey()];
		}

		if (pNewDev == nullptr || pDev == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

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

		pOldLink = pDev->GetLink(LK_TYPE_RELEAY , ntf, ntu, ntc, ntd);
		if (pOldLink == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		
		pTemp = m_pRefFasSysData->GetDeviceByID(ntf, ntu, ntc, ntd);

		if (pTemp == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		nRet = GetChangeNumber(pTemp, nN_FNum, nN_UNum, nN_CNum, nN_DNum, nN_Fid, nN_Uid, nN_Cid, nN_Did
			, nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);

		if (nRet <= 0) // 변경하지 않음
		{
			pDB->MoveNext();
			continue;
		}

		nInType = nOutType = nEqName = nCont = -1;
		pEq = pTemp->GetEqInput();
		if (pEq)
			nInType = pEq->GetEquipID();
		pEq = pTemp->GetEqName();
		if (pEq)
			nEqName = pEq->GetEquipID();
		pEq = pTemp->GetEqOutput();
		if (pEq)
			nOutType = pEq->GetEquipID();
		pEq = pTemp->GetEqOutContents();
		if (pEq)
			nCont = pEq->GetEquipID();
		// [KHS 2020-5-18 11:25:25] 
		// 교차 감지기 일때
		// 출력 타입 - 프리액션 , 출력내용 - 밸브 일때 
		if (nOutType == OUTTYPE_PREACTION && nCont == OUTCONT_VALVE)
			bFirstAdd = TRUE;
		else
			bFirstAdd = FALSE; 

		pNewLink = new CDataLinked;
		pNewLink->CopyData(pOldLink);
		pNewLink->SetLinkData(nT_Fid, nT_Uid, nT_Cid, nT_Did 
			, pOldLink->GetLinkType() , pOldLink->GetLogicType() , pOldLink->GetLogicID()
			, nlf, nlu, nlc, nld
		);

		pChgLink = new ST_CHANGE_LINKDATA;
		pChgLink->btInOutType = CHANGE_LK_OUTPUT;
		pChgLink->btChangeType = CHANGE_ACTION_UPDATE;
		pChgLink->pRefInput = pNewDev;
		pChgLink->pRefOld = pOldLink;
		pChgLink->pChange = pNewLink;
		m_ptrChangeList.AddTail(pChgLink);
		pNewDev->AddLink(bFirstAdd,pNewLink);
		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}



int CFormRelayEdit::MakeOutputChangeData(ST_TREEITEM * pNewData, int nDefaultAction)
{
	if (pNewData == nullptr || pNewData->pData == nullptr)
		return 0;
	//int nFid, nUid, nCid, nDid;
	int nFNum, nUNum, nCNum, nDNum;
	CString strSql;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataSystem *pSys;
	ST_CHANGE_LINKDATA * pChgLink;
	CDataDevice * pDev = nullptr, *pNewDev = nullptr, *pTemp = nullptr;
	CDataLinked *pOldLink = nullptr, *pNewLink = nullptr;
	int nN_Fid, nN_Uid, nN_Cid, nN_Did;
	int nN_FNum, nN_UNum, nN_CNum, nN_DNum;
	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;

	nN_Fid = nN_Uid = nN_Cid = nN_Did = -1;
	nN_FNum = nN_UNum = nN_CNum = nN_DNum = -1;

	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;
// 	nFid = nUid = nCid = nDid = -1;
 	nFNum = nUNum = nCNum = nDNum = -1;
// 	GetIDNum(pNewData, nFid, nUid, nCid, nDid, nFNum, nUNum, nCNum, nDNum);

	pSys = (CDataSystem *)m_pCurData->pData;
	switch (m_pCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pSys->GetSysData();
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			L"ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY "
			, LK_TYPE_RELEAY, pFacp->GetFacpID()
		);
		nFNum = pFacp->GetFacpNum();
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pSys->GetSysData();
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			L"AND TGT_UNIT=%d "
			L"ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY "
			, LK_TYPE_RELEAY, pUnit->GetFacpID()
			, pUnit->GetUnitID()
		);
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(pUnit->GetFacpID());
		nUNum = pUnit->GetUnitNum();
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pSys->GetSysData();
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			L"AND TGT_UNIT=%d AND TGT_CHN=%d "
			L"ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY "
			, LK_TYPE_RELEAY, pChn->GetFacpID()
			, pChn->GetUnitID(), pChn->GetChnID()
		);
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(pChn->GetFacpID());
		nUNum = m_pRefFasSysData->CvtUnitIDToNum(pChn->GetFacpID(), pChn->GetUnitID());
		nCNum = pChn->GetChnNum();
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pSys->GetSysData();
		strSql.Format(L"SELECT * FROM TB_LINK_RELAY WHERE LINK_TYPE=%d AND TGT_FACP=%d "
			L"AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
			L"ORDER BY SRC_FACP , SRC_UNIT , SRC_CHN , SRC_RLY "
			, LK_TYPE_RELEAY, pDev->GetFacpID()
			, pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
		);
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(pDev->GetFacpID());
		nUNum = m_pRefFasSysData->CvtUnitIDToNum(pDev->GetFacpID(), pDev->GetUnitID());
		nCNum = m_pRefFasSysData->CvtChannelIDToNum(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID());
		nDNum = pDev->GetDeviceNum();
		break;
	}

	int nCnt, i,nRet;
	int nFacp, nUnit, nChn, nDev;
	int nlf,nlu,nlc,nld ; //< 마지막 ID
	int ntf, ntu, ntc, ntd;
	int nInType, nOutType, nEqName, nCont;
	CDataEquip *pEq;
	BOOL bFirstAdd = FALSE;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	nlf = nlu = nlc = nld = -1;
	ntf = ntu = ntc = ntd = -1;
	nCnt = pDB->GetRecordCount();
	pDev = nullptr;
	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;
		ntf = ntu = ntc = ntd = -1;
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

		if (pDev == nullptr || pNewData == nullptr || nlf != nFacp || nlu != nUnit || nlc != nChn || nld != nDev)
		{
			nlf = nFacp;
			nlu = nUnit;
			nlc = nChn;
			nld = nDev;
			pDev = m_pRefFasSysData->GetDeviceByID(nlf, nlu, nlc, nld);

			// 변경되는 연동데이터(CDAtaLinked)를 사용하기 때문에 변경되는 Device Map에는 안들어갈 수도 있다.
			pNewDev = m_mapChangeDevice[pDev->GetDevKey()];
			if (pNewDev == nullptr)
			{
				pNewDev = new CDataDevice;
				pNewDev->CopyExceptLink(pDev);
			}
			//m_mapChangeDevice[pDev->GetDevKey()] = pNewDev;
		}

		if (pDev == nullptr || pNewDev == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

		if (pDB->GetFieldValue(L"TGT_FACP", ntf) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"TGT_UNIT", ntu) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"TGT_CHN", ntc) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"TGT_RLY", ntd) == FALSE)
			nDev = -1;
		if (ntf < 0 || ntu < 0 || ntc < 0 || ntd < 0)
		{
			pDB->MoveNext();
			continue;
		}
		
		pOldLink = pDev->GetLink(LK_TYPE_RELEAY, ntf, ntu, ntc, ntd);
		if (pOldLink == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

		pTemp = m_pRefFasSysData->GetDeviceByID(ntf, ntu, ntc, ntd);

		if (pTemp == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		nRet = GetChangeNumber(pTemp, nN_FNum, nN_UNum, nN_CNum, nN_DNum, nN_Fid, nN_Uid, nN_Cid, nN_Did
			, nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);

		if (nRet <= 0) // 변경하지 않음
		{
			pDB->MoveNext();
			continue;
		}

		nInType = nOutType = nEqName = nCont = -1;
		pEq = pTemp->GetEqInput();
		if (pEq)
			nInType = pEq->GetEquipID();
		pEq = pTemp->GetEqName();
		if (pEq)
			nEqName = pEq->GetEquipID();
		pEq = pTemp->GetEqOutput();
		if (pEq)
			nOutType = pEq->GetEquipID();
		pEq = pTemp->GetEqOutContents();
		if (pEq)
			nCont = pEq->GetEquipID();
		// [KHS 2020-5-18 11:25:25] 
		// 교차 감지기 일때
		// 출력 타입 - 프리액션 , 출력내용 - 밸브 일때 
		if (nOutType == OUTTYPE_PREACTION && nCont == OUTCONT_VALVE)
			bFirstAdd = TRUE;
		else
			bFirstAdd = FALSE;

		pNewLink = pNewDev->GetLink(pOldLink->GetLinkType(), ntf, ntu, ntc, ntd);
		if (pNewLink == nullptr)
		{
			pNewLink = new CDataLinked;
			pNewDev->AddLink(bFirstAdd,pNewLink);
		}
		pNewLink->SetLinkData(nT_Fid, nT_Uid, nT_Cid, nT_Did
			, pOldLink->GetLinkType(), pOldLink->GetLogicType(), pOldLink->GetLogicID()
			, nlf, nlu, nlc, nld
		);
		pChgLink = new ST_CHANGE_LINKDATA;
		pChgLink->btInOutType = CHANGE_LK_OUTPUT;
		pChgLink->btChangeType = CHANGE_ACTION_UPDATE;
		pChgLink->pRefInput = pNewDev;
		pChgLink->pRefOld = pOldLink;
		pChgLink->pChange = pNewLink;
		m_ptrChangeList.AddTail(pChgLink);
		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}


int CFormRelayEdit::MakePatternChangeData(ST_TREEITEM * pNewData, int nDefaultAction)
{
	if (pNewData == nullptr || pNewData->pData == nullptr)
		return 0;
	//int nFid, nUid, nCid, nDid;
	//int nFNum, nUNum, nCNum, nDNum;
	int nN_Fid, nN_Uid, nN_Cid, nN_Did;
	int nN_FNum, nN_UNum, nN_CNum, nN_DNum;

	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;

	CString strSql;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev, *pOldDev , *pNewDev;
	CDataSystem *pSys;
	CDataPattern * pOldPtn = nullptr , *pNewPtn  = nullptr;
	CDataLinked * plnk;
	ST_CHANGE_LINKDATA * pChgLink;
	// 	nFid = nUid = nCid = nDid = -1;
	//nFNum = nUNum = nCNum = nDNum = -1;
	// 	GetIDNum(pNewData, nFid, nUid, nCid, nDid, nFNum, nUNum, nCNum, nDNum);
	nN_Fid = nN_Uid = nN_Cid = nN_Did = -1;
	nN_FNum = nN_UNum = nN_CNum = nN_DNum = -1;

	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;
	GetIDNum(pNewData, nN_Fid, nN_Uid, nN_Cid, nN_Did, nN_FNum, nN_UNum, nN_CNum, nN_DNum);

	pSys = (CDataSystem *)m_pCurData->pData;
	switch (m_pCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pSys->GetSysData();
		strSql.Format(L"SELECT  A.PT_ID,A.ITEM_TYPE, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID, B.PT_TYPE, B.PT_NAME "
			L"FROM     TB_PATTERN_ITEM AS A INNER JOIN "
			L"TB_PATTERN_MST AS B ON A.PT_ID = B.PT_ID "
			L"WHERE(A.FACP_ID = %d)  "
			L"ORDER BY A.PT_ID, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID"
			, pFacp->GetFacpID()
		);
		//nFNum = pFacp->GetFacpNum();
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pSys->GetSysData();
		strSql.Format(L"SELECT  A.PT_ID, A.ITEM_TYPE,A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID, B.PT_TYPE, B.PT_NAME "
			L"FROM     TB_PATTERN_ITEM AS A INNER JOIN "
			L"TB_PATTERN_MST AS B ON A.PT_ID = B.PT_ID "
			L"WHERE(A.FACP_ID = %d) AND(A.UNIT_ID = %d) "
			L"ORDER BY A.PT_ID, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID"
			, pUnit->GetFacpID()
			, pUnit->GetUnitID()
		);
		
		//nFNum = m_pRefFasSysData->CvtFacpIDToNum(pUnit->GetFacpID());
		//nUNum = pUnit->GetUnitNum();
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pSys->GetSysData();
		strSql.Format(L"SELECT  A.PT_ID,A.ITEM_TYPE, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID, B.PT_TYPE, B.PT_NAME "
			L"FROM     TB_PATTERN_ITEM AS A INNER JOIN "
			L"TB_PATTERN_MST AS B ON A.PT_ID = B.PT_ID "
			L"WHERE(A.FACP_ID = %d) AND(A.UNIT_ID = %d) AND(A.CHN_ID = %d)  "
			L"ORDER BY A.PT_ID, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID"
			, pChn->GetFacpID()
			, pChn->GetUnitID(), pChn->GetChnID()
		);
		//nFNum = m_pRefFasSysData->CvtFacpIDToNum(pChn->GetFacpID());
		//nUNum = m_pRefFasSysData->CvtUnitIDToNum(pChn->GetFacpID(), pChn->GetUnitID());
		//nCNum = pChn->GetChnNum();
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pSys->GetSysData();
		strSql.Format(L"SELECT  A.PT_ID, A.ITEM_TYPE,A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID, B.PT_TYPE, B.PT_NAME "
			L"FROM     TB_PATTERN_ITEM AS A INNER JOIN "
			L"TB_PATTERN_MST AS B ON A.PT_ID = B.PT_ID "
			L"WHERE(A.FACP_ID = %d) AND(A.UNIT_ID = %d) AND(A.CHN_ID = %d) AND(A.RLY_ID = %d) "
			L"ORDER BY A.PT_ID, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID"
			, pDev->GetFacpID()
			, pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
		);
		//nFNum = m_pRefFasSysData->CvtFacpIDToNum(pDev->GetFacpID());
		//nUNum = m_pRefFasSysData->CvtUnitIDToNum(pDev->GetFacpID(), pDev->GetUnitID());
		//nCNum = m_pRefFasSysData->CvtChannelIDToNum(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID());
		//nDNum = pDev->GetDeviceNum();
		break;
	}

	int nCnt, i , nRet , nType ;
	int nPtId, nlp;
	int nFacp, nUnit, nChn, nDev;
	std::shared_ptr<CManagerPattern> spManager;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	spManager = m_pRefFasSysData->GetPatternManager();
	nlp = nPtId = 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		nPtId = nFacp = nUnit = nChn = nDev = -1;
		if (pDB->GetFieldValue(L"PT_ID", nPtId) == FALSE)
		{
			nPtId = -1;
			pDB->MoveNext();
			continue;
		}

		if (pDB->GetFieldValue(L"FACP_ID", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"UNIT_ID", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"CHN_ID", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"RLY_ID", nDev) == FALSE)
			nDev = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
		{
			pDB->MoveNext();
			continue;
		}
		pDB->GetFieldValue(L"ITEM_TYPE", nType);
		if (nType != LK_TYPE_RELEAY)
		{
			pDB->MoveNext();
			continue;
		}

		if (pOldPtn == nullptr || pNewPtn == nullptr || nPtId != nlp)
		{
			nlp = nPtId;
			pOldPtn = spManager->GetPattern(nPtId);
			pNewPtn = new CDataPattern;
			pNewPtn->CopyDataExceptDevice(pOldPtn);
		}

		if (pOldPtn == nullptr || pNewPtn == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

		pOldDev = m_pRefFasSysData->GetDeviceByID(nFacp, nUnit, nChn, nDev);

		if (pOldDev == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

		nRet = GetChangeNumber(pOldDev, nN_FNum, nN_UNum, nN_CNum, nN_DNum, nN_Fid, nN_Uid, nN_Cid, nN_Did
			, nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);

		if (nRet <= 0) // 변경하지 않음
		{
			pDB->MoveNext();
			continue;
		}
		plnk = pNewPtn->GetPatternItem(LK_TYPE_RELEAY, nT_Fid, nT_Uid, nT_Cid, nT_Did);
		if(plnk == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pNewDev = m_pRefFasSysData->GetDeviceByID(nT_Fid, nT_Uid, nT_Cid, nT_Did);
		if(pNewDev == nullptr)
		{
			pNewDev = new CDataDevice;
			pNewDev->CopyExceptLink(pOldDev);
			pNewPtn->AddDeviceToPattern(pNewDev,LOGIC_ALL_AUTO);
		}
		pNewDev->SetAddressInfo(nT_Fid, nT_Uid, nT_Cid, nT_Did, nT_FNum, nT_UNum, nT_CNum, nT_DNum);
		pChgLink = new ST_CHANGE_LINKDATA;
		pChgLink->btInOutType = CHANGE_LK_PATTERN;
		pChgLink->btChangeType = CHANGE_ACTION_UPDATE;
		pChgLink->pRefInput = pNewPtn;
		pChgLink->pRefOld = pOldDev;
		pChgLink->pChange = pNewDev;
		m_ptrChangeList.AddTail(pChgLink);
		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}


int CFormRelayEdit::MakeInputDeleteData(ST_TREEITEM * pCurData)
{
	if (pCurData == nullptr || pCurData->pData == nullptr)
		return 0;

	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev, *pTemp;
	CDataSystem *pSys;
	CDataPattern * pPtn = nullptr;
	ST_CHANGE_LINKDATA * pChgLink;
	CString strSql;
	int nCnt , i;
	int nFacp, nUnit, nChn, nDev;
	pSys = (CDataSystem *)pCurData->pData;
	switch (pCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_RELAY_LIST "
			L"WHERE FACP_ID = %d  "
			L"ORDER BY FACP_ID, UNIT_ID,CHN_ID, RLY_ID"
			, pFacp->GetFacpID()
		);
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_RELAY_LIST "
			L"WHERE FACP_ID = %d AND UNIT_ID = %d "
			L"ORDER BY FACP_ID, UNIT_ID,CHN_ID, RLY_ID"
			, pUnit->GetFacpID()
			, pUnit->GetUnitID()
		);
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_RELAY_LIST "
			L"WHERE FACP_ID = %d AND UNIT_ID = %d AND CHN_ID = %d "
			L"ORDER BY FACP_ID, UNIT_ID,CHN_ID, RLY_ID"
			, pChn->GetFacpID()
			, pChn->GetUnitID(), pChn->GetChnID()
		);
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_RELAY_LIST "
			L"WHERE FACP_ID = %d AND UNIT_ID = %d AND CHN_ID = %d AND RLY_ID = %d "
			L"ORDER BY FACP_ID, UNIT_ID,CHN_ID, RLY_ID"
			, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(),pDev->GetDeviceID()
		);
		break;
	}

	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;

		if (pDB->GetFieldValue(L"FACP_ID", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"UNIT_ID", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"CHN_ID", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"RLY_ID", nDev) == FALSE)
			nDev = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
		{
			pDB->MoveNext();
			continue;
		}
		pDev = m_pRefFasSysData->GetDeviceByID(nFacp, nUnit, nChn, nDev);

		if (pDev == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pChgLink = new ST_CHANGE_LINKDATA;
		pChgLink->btInOutType = CHANGE_LK_INPUT;
		pChgLink->btChangeType = CHANGE_ACTION_DEL;
		pChgLink->pRefInput = nullptr;
		pChgLink->pRefOld = pDev;
		pChgLink->pChange = nullptr;
		pTemp = m_mapChangeDevice[pDev->GetDevKey()];
		if (pTemp == nullptr)
			m_mapChangeDevice[pDev->GetDevKey()] = pDev;
		m_ptrChangeList.AddTail(pChgLink);
		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}


int CFormRelayEdit::MakeOutputDeleteData(ST_TREEITEM * pCurData)
{
	if (pCurData == nullptr || pCurData->pData == nullptr)
		return 0;

	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev , *pSrc=nullptr;
	CDataSystem *pSys;
	CDataLinked * pLnk = nullptr;
	CDataPattern * pPtn = nullptr;
	ST_CHANGE_LINKDATA * pChgLink;
	CString strSql;
	int nCnt, i , ntype;
	int nFacp, nUnit, nChn, nDev;
	int nSrcF, nSrcU, nSrcC, nSrcD;
	int nlf, nlu, nlc, nld;
	nlf = nlu = nlc = nld = -1;
	pSys = (CDataSystem *)pCurData->pData;
	switch (pCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_LINK_RELAY "
			L"WHERE SRC_FACP = %d  "
			L"ORDER BY SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY"
			, pFacp->GetFacpID()
		);
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pSys->GetSysData();
		strSql.Format(L"SELECT    * FROM TB_LINK_RELAY "
			L"WHERE SRC_FACP = %d AND SRC_UNIT = %d "
			L"ORDER BY SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY"
			, pUnit->GetFacpID()
			, pUnit->GetUnitID()
		);
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pSys->GetSysData();
		strSql.Format(L"SELECT    * FROM TB_LINK_RELAY "
			L"WHERE SRC_FACP = %d AND SRC_UNIT = %d AND SRC_CHN = %d "
			L"ORDER BY SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY"
			, pChn->GetFacpID()
			, pChn->GetUnitID(), pChn->GetChnID()
		);
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_LINK_RELAY "
			L"WHERE SRC_FACP = %d AND SRC_UNIT = %d AND SRC_CHN = %d AND SRC_RLY = %d "
			L"ORDER BY SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY"
			, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		break;
	}

	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;
		if (pDB->GetFieldValue(L"TGT_FACP", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"TGT_UNIT", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"TGT_CHN", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"TGT_RLY", nDev) == FALSE)
			nDev = -1;

		if (pDB->GetFieldValue(L"SRC_FACP", nSrcF) == FALSE)
			nSrcF = -1;
		if (pDB->GetFieldValue(L"SRC_UNIT", nSrcU) == FALSE)
			nSrcU = -1;
		if (pDB->GetFieldValue(L"SRC_CHN", nSrcC) == FALSE)
			nSrcC = -1;
		if (pDB->GetFieldValue(L"SRC_RLY", nSrcD) == FALSE)
			nSrcD = -1;
		if (pDB->GetFieldValue(L"LINK_TYPE", ntype) == FALSE)
			ntype = -1;

		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0 || ntype < 0
			|| nSrcF < 0 || nSrcU < 0 || nSrcC < 0 || nSrcD < 0)
		{
			pDB->MoveNext();
			continue;
		}
		
		if (pSrc == nullptr || nlf != nSrcF || nlu != nSrcU || nlc != nSrcC || nld != nSrcD)
		{
			pSrc = m_pRefFasSysData->GetDeviceByID(nSrcF, nSrcU, nSrcC, nSrcD);
			if (pSrc == nullptr)
			{
				pDB->MoveNext();
				continue;
			}
			nlf = nSrcF;
			nlu = nSrcU;
			nlc = nSrcC;
			nld = nSrcD;
		}
		pLnk = pSrc->GetLink(ntype, nFacp, nUnit, nChn, nDev);
		if (pLnk == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pChgLink = new ST_CHANGE_LINKDATA;
		pChgLink->btInOutType = CHANGE_LK_OUTPUT;
		pChgLink->btChangeType = CHANGE_ACTION_DEL;
		pChgLink->pRefInput = pSrc;
		pChgLink->pRefOld = pLnk;
		pChgLink->pChange = nullptr;
		m_ptrChangeList.AddTail(pChgLink);
		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}

int CFormRelayEdit::MakeUsedLinkDeleteData(ST_TREEITEM * pCurData)
{
	if (pCurData == nullptr || pCurData->pData == nullptr)
		return 0;

	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev, *pSrc = nullptr;
	CDataSystem *pSys;
	CDataLinked * pLnk = nullptr;
	CDataPattern * pPtn = nullptr;
	ST_CHANGE_LINKDATA * pChgLink;
	CString strSql;
	int nCnt, i, ntype;
	int nFacp, nUnit, nChn, nDev;
	int nSrcF, nSrcU, nSrcC, nSrcD;
	int nlf, nlu, nlc, nld;
	pSys = (CDataSystem *)pCurData->pData;
	nlf = nlu = nlc = nld = -1;
	switch (pCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_LINK_RELAY "
			L"WHERE LINK_TYPE=%d AND TGT_FACP = %d "
			L"ORDER BY SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY, TGT_FACP, TGT_UNIT,TGT_CHN, TGT_RLY"
			, LK_TYPE_RELEAY, pFacp->GetFacpID()
		);
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_LINK_RELAY "
			L"WHERE LINK_TYPE=%d AND TGT_FACP = %d AND TGT_UNIT = %d "
			L"ORDER BY SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY, TGT_FACP, TGT_UNIT,TGT_CHN, TGT_RLY"
			, LK_TYPE_RELEAY, pUnit->GetFacpID()
			, pUnit->GetUnitID()
		);
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_LINK_RELAY "
			L"WHERE LINK_TYPE=%d AND TGT_FACP = %d AND TGT_UNIT = %d AND TGT_CHN = %d "
			L"ORDER BY SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY, TGT_FACP, TGT_UNIT,TGT_CHN, TGT_RLY"
			, LK_TYPE_RELEAY, pChn->GetFacpID()
			, pChn->GetUnitID(), pChn->GetChnID()
		);
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pSys->GetSysData();
		strSql.Format(L"SELECT   * FROM TB_LINK_RELAY "
			L"WHERE LINK_TYPE=%d AND TGT_FACP = %d AND TGT_UNIT = %d AND TGT_CHN = %d AND TGT_RLY = %d "
			L"ORDER BY SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY, TGT_FACP, TGT_UNIT,TGT_CHN, TGT_RLY"
			, LK_TYPE_RELEAY, pDev->GetFacpID(), pDev->GetUnitID()
			, pDev->GetChnID(), pDev->GetDeviceID()
		);
		break;
	}

	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		nFacp = nUnit = nChn = nDev = -1;

		if (pDB->GetFieldValue(L"TGT_FACP", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"TGT_UNIT", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"TGT_CHN", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"TGT_RLY", nDev) == FALSE)
			nDev = -1;
		if (pDB->GetFieldValue(L"SRC_FACP", nSrcF) == FALSE)
			nSrcF = -1;
		if (pDB->GetFieldValue(L"SRC_UNIT", nSrcU) == FALSE)
			nSrcU = -1;
		if (pDB->GetFieldValue(L"SRC_CHN", nSrcC) == FALSE)
			nSrcC = -1;
		if (pDB->GetFieldValue(L"SRC_RLY", nSrcD) == FALSE)
			nSrcD = -1;
		if (pDB->GetFieldValue(L"LINK_TYPE", ntype) == FALSE)
			ntype = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0 || ntype < 0
			|| nSrcF < 0 || nSrcU < 0 || nSrcC < 0 || nSrcD < 0)
		{
			pDB->MoveNext();
			continue;
		}


		pDev = m_pRefFasSysData->GetDeviceByID(nFacp, nUnit, nChn, nDev);

		if (pDev == nullptr)
		{
			pDB->MoveNext();
			continue;
		}

		if (pSrc == nullptr || nlf != nSrcF || nlu != nSrcU || nlc != nSrcC || nld != nSrcD)
		{
			pSrc = m_pRefFasSysData->GetDeviceByID(nSrcF, nSrcU, nSrcC, nSrcD);
			if (pSrc == nullptr)
			{
				pDB->MoveNext();
				continue;
			}
			nlf = nSrcF;
			nlu = nSrcU;
			nlc = nSrcC;
			nld = nSrcD;
		}
		pLnk = pSrc->GetLink(ntype, nFacp, nUnit, nChn, nDev);
		if (pLnk == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pChgLink = new ST_CHANGE_LINKDATA;
		pChgLink->btInOutType = CHANGE_LK_OUTPUT;
		pChgLink->btChangeType = CHANGE_ACTION_DEL;
		pChgLink->pRefInput = pSrc;
		pChgLink->pRefOld = pLnk;
		pChgLink->pChange = nullptr;
// 		pTemp = m_mapChangeDevice[pDev->GetDevKey()];
// 		if (pTemp == nullptr)
// 			m_mapChangeDevice[pDev->GetDevKey()] = pDev;
		m_ptrChangeList.AddTail(pChgLink);
		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}

int CFormRelayEdit::MakePatternDeleteData(ST_TREEITEM * pCurData)
{
	if (pCurData == nullptr || pCurData->pData == nullptr)
		return 0;
	//int nFid, nUid, nCid, nDid;
	//int nFNum, nUNum, nCNum, nDNum;
	int nN_Fid, nN_Uid, nN_Cid, nN_Did;
	int nN_FNum, nN_UNum, nN_CNum, nN_DNum;

	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;

	CString strSql;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev, *pTemp;
	CDataSystem *pSys;
	CDataPattern * pPtn = nullptr;
	ST_CHANGE_LINKDATA * pChgLink;
	// 	nFid = nUid = nCid = nDid = -1;
	//nFNum = nUNum = nCNum = nDNum = -1;
	// 	GetIDNum(pNewData, nFid, nUid, nCid, nDid, nFNum, nUNum, nCNum, nDNum);
	nN_Fid = nN_Uid = nN_Cid = nN_Did = -1;
	nN_FNum = nN_UNum = nN_CNum = nN_DNum = -1;

	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;
	GetIDNum(pCurData, nN_Fid, nN_Uid, nN_Cid, nN_Did, nN_FNum, nN_UNum, nN_CNum, nN_DNum);

	pSys = (CDataSystem *)pCurData->pData;
	switch (pCurData->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pSys->GetSysData();
		strSql.Format(L"SELECT  A.PT_ID,A.ITEM_TYPE, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID, B.PT_TYPE, B.PT_NAME "
			L"FROM     TB_PATTERN_ITEM AS A INNER JOIN "
			L"TB_PATTERN_MST AS B ON A.PT_ID = B.PT_ID "
			L"WHERE(A.FACP_ID = %d)  "
			L"ORDER BY A.PT_ID, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID"
			, pFacp->GetFacpID()
		);
		//nFNum = pFacp->GetFacpNum();
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pSys->GetSysData();
		strSql.Format(L"SELECT  A.PT_ID,A.ITEM_TYPE, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID, B.PT_TYPE, B.PT_NAME "
			L"FROM     TB_PATTERN_ITEM AS A INNER JOIN "
			L"TB_PATTERN_MST AS B ON A.PT_ID = B.PT_ID "
			L"WHERE(A.FACP_ID = %d) AND(A.UNIT_ID = %d) "
			L"ORDER BY A.PT_ID, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID"
			, pUnit->GetFacpID()
			, pUnit->GetUnitID()
		);

		//nFNum = m_pRefFasSysData->CvtFacpIDToNum(pUnit->GetFacpID());
		//nUNum = pUnit->GetUnitNum();
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pSys->GetSysData();
		strSql.Format(L"SELECT  A.PT_ID,A.ITEM_TYPE, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID, B.PT_TYPE, B.PT_NAME "
			L"FROM     TB_PATTERN_ITEM AS A INNER JOIN "
			L"TB_PATTERN_MST AS B ON A.PT_ID = B.PT_ID "
			L"WHERE(A.FACP_ID = %d) AND(A.UNIT_ID = %d) AND(A.CHN_ID = %d)  "
			L"ORDER BY A.PT_ID, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID"
			, pChn->GetFacpID()
			, pChn->GetUnitID(), pChn->GetChnID()
		);
		//nFNum = m_pRefFasSysData->CvtFacpIDToNum(pChn->GetFacpID());
		//nUNum = m_pRefFasSysData->CvtUnitIDToNum(pChn->GetFacpID(), pChn->GetUnitID());
		//nCNum = pChn->GetChnNum();
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pSys->GetSysData();
		strSql.Format(L"SELECT  A.PT_ID, A.ITEM_TYPE,A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID, B.PT_TYPE, B.PT_NAME "
			L"FROM     TB_PATTERN_ITEM AS A INNER JOIN "
			L"TB_PATTERN_MST AS B ON A.PT_ID = B.PT_ID "
			L"WHERE(A.FACP_ID = %d) AND(A.UNIT_ID = %d) AND(A.CHN_ID = %d) AND(A.RLY_ID = %d) "
			L"ORDER BY A.PT_ID, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID"
			, pDev->GetFacpID()
			, pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
		);
		//nFNum = m_pRefFasSysData->CvtFacpIDToNum(pDev->GetFacpID());
		//nUNum = m_pRefFasSysData->CvtUnitIDToNum(pDev->GetFacpID(), pDev->GetUnitID());
		//nCNum = m_pRefFasSysData->CvtChannelIDToNum(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID());
		//nDNum = pDev->GetDeviceNum();
		break;
	}

	int nCnt, i,nType;
	int nPtId, nlp;
	int nFacp, nUnit, nChn, nDev;
	std::shared_ptr<CManagerPattern> spManager;
	YAdoDatabase * pDB = m_pRefFasSysData->GetPrjDB();
	if (pDB->OpenQuery(strSql) == FALSE)
		return 0;

	spManager = m_pRefFasSysData->GetPatternManager();
	nlp = nPtId = 0;
	nCnt = pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		nPtId = nFacp = nUnit = nChn = nDev = -1;
		if (pDB->GetFieldValue(L"PT_ID", nPtId) == FALSE)
		{
			nPtId = -1;
			pDB->MoveNext();
			continue;
		}

		if (pDB->GetFieldValue(L"FACP_ID", nFacp) == FALSE)
			nFacp = -1;
		if (pDB->GetFieldValue(L"UNIT_ID", nUnit) == FALSE)
			nUnit = -1;
		if (pDB->GetFieldValue(L"CHN_ID", nChn) == FALSE)
			nChn = -1;
		if (pDB->GetFieldValue(L"RLY_ID", nDev) == FALSE)
			nDev = -1;
		if (nFacp < 0 || nUnit < 0 || nChn < 0 || nDev < 0)
		{
			pDB->MoveNext();
			continue;
		}
		pDB->GetFieldValue(L"ITEM_TYPE", nType);
		if (nType != LK_TYPE_RELEAY)
		{
			pDB->MoveNext();
			continue;
		}
		if (nPtId != nlp)
		{
			nlp = nPtId;
			pPtn = spManager->GetPattern(nPtId);
		}

		if (pPtn == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pTemp = m_pRefFasSysData->GetDeviceByID(nFacp, nUnit, nChn, nDev);

		if (pTemp == nullptr)
		{
			pDB->MoveNext();
			continue;
		}
		pChgLink = new ST_CHANGE_LINKDATA;
		pChgLink->btInOutType = CHANGE_LK_PATTERN;
		pChgLink->btChangeType = CHANGE_ACTION_DEL;
		pChgLink->pRefInput = pPtn;
		pChgLink->pRefOld = pTemp;
		pChgLink->pChange = nullptr;
		pDB->MoveNext();
	}
	pDB->RSClose();
	return 1;
}

BOOL CFormRelayEdit::GetIDNum(ST_TREEITEM* pTreeItem, int &nFid, int &nUid, int &nCid, int &nDevid
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


int CFormRelayEdit::SaveAffectedByChange(YAdoDatabase * pDB)
{
	CString strSql , strWhere , strCmd;
	POSITION pos;
	ST_CHANGE_LINKDATA * pChgLink;
	CDataPattern * pPtn; 
	CDataDevice *pOld, *pNew;
	CDataLocBase * pLoc;
	CDataLinked *pOldLnk, *pNewLnk;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nPtn;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	pos = m_ptrChangeList.GetHeadPosition();
	while (pos)
	{
		strSql = strWhere = strCmd = L"";
		pChgLink = (ST_CHANGE_LINKDATA*)m_ptrChangeList.GetNext(pos);
		if (pChgLink == nullptr)
			continue; 
// 		if (pChgLink->pRefInput == nullptr)
// 			continue; 

		// CHANGE_ACTION_DEL : 패터일때 삭제 , 출력일 때 삭제 , 
		if (pChgLink->btChangeType == CHANGE_ACTION_ADD)
		{
			strCmd = L"";
			strWhere = L"";
			switch (pChgLink->btInOutType)
			{
			case CHANGE_LK_PATTERN:
				break;
			case CHANGE_LK_FACP:
				pFacp = (CDataFacp *)pChgLink->pChange;
				strCmd.Format(L"INSERT INTO TB_FACP(NET_ID,FACP_ID,FACP_NUM,FACP_TYPE,FACP_NAME,ADD_USER) "
					L" VALUES(1,%d,%d,%d,'%s','%s') "
					, pFacp->GetFacpID(), pFacp->GetFacpNum(), pFacp->GetFacpType(), pFacp->GetFacpName()
					, m_pRefFasSysData->GetCurrentUser()
				);
				m_pRefFasSysData->AddSystemFacpDataByNum(
					 pFacp->GetFacpNum() ,pFacp->GetFacpID()
					, pFacp->GetFacpType(), pFacp->GetFacpName()
				);
				break;
			case CHANGE_LK_UNIT:
				pUnit = (CDataUnit *)pChgLink->pChange;
				strCmd.Format(L"INSERT INTO TB_UNIT(NET_ID,FACP_ID,UNIT_ID,UNIT_NUM,UNIT_TYPE,UNIT_NAME,ADD_USER) "
					L" VALUES(1,%d,%d,%d,%d,'%s','%s') "
					, pUnit->GetFacpID(), pUnit->GetUnitID(), pUnit->GetUnitNum(), pUnit->GetUnitType()
					, pUnit->GetUnitName()
					, m_pRefFasSysData->GetCurrentUser()
				);
				
				m_pRefFasSysData->AddSystemUnitDataByID(
					pUnit->GetFacpID(), pUnit->GetUnitNum()
					, pUnit->GetUnitID(), pUnit->GetUnitType()
					, pUnit->GetUnitName()
				);
				break;
			case CHANGE_LK_CHN:
				pChn = (CDataChannel *)pChgLink->pChange;
				strCmd.Format(L"INSERT INTO TB_CHANNEL(NET_ID,FACP_ID,UNIT_ID,CHN_ID,CHN_NUM,CHN_NAME,ADD_USER) "
					L" VALUES(1,%d,%d,%d,%d,"
					L"'%s','s') "
					, pChn->GetFacpID(), pChn->GetUnitID(), pChn->GetChnID(), pChn->GetChnNum()
					, pChn->GetChnName()
					, m_pRefFasSysData->GetCurrentUser()
				);
				m_pRefFasSysData->AddSystemChannelDataByID(
					  pChn->GetFacpID(), pChn->GetUnitID()
					, pChn->GetChnNum(), pChn->GetChnID()
					, pChn->GetChnName()
				);
				break;
			case CHANGE_LK_DEV:
				break;
			}
		}
		else if (pChgLink->btChangeType == CHANGE_ACTION_DEL)
		{
			if (pChgLink->btInOutType == CHANGE_LK_PATTERN)
			{
				pPtn = (CDataPattern*)pChgLink->pRefInput;
				pOld = (CDataDevice *)pChgLink->pRefOld;
				
				strCmd.Format(L"DELETE TB_PATTERN_ITEM ");
				strWhere.Format(L" WHERE PT_ID=%d AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d " 
					,pPtn->GetPatternID()
					, pOld->GetFacpID() , pOld->GetUnitID() , pOld->GetChnID() , pOld->GetDeviceID()
				);
			}
			else if (pChgLink->btInOutType == CHANGE_LK_OUTPUT)
			{
				pOldLnk = (CDataLinked *)pChgLink->pRefOld;
				strCmd.Format(L" DELETE TB_LINK_RELAY ");
				strWhere.Format(L" WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
					L" TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
					, pOldLnk->GetSrcFacp(), pOldLnk->GetSrcUnit(), pOldLnk->GetSrcChn(), pOldLnk->GetSrcDev()
					, pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit(), pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev()
				);
			}
			else  if (pChgLink->btInOutType == CHANGE_LK_INPUT)
			{
				pOld = (CDataDevice*)pChgLink->pRefOld;
				strCmd.Format(L"DELETE TB_RELAY_LIST ");
				strWhere.Format(L" WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
					, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID()
				);
			}
		}
		else if (pChgLink->btChangeType == CHANGE_ACTION_KEEP)
		{
			continue;
		}
		else
		{
			if (pChgLink->btInOutType == CHANGE_LK_PATTERN)
			{
				pPtn = (CDataPattern*)pChgLink->pRefInput;
				pOld = (CDataDevice *)pChgLink->pRefOld;
				pNew = (CDataDevice *)pChgLink->pChange;
				strCmd.Format(L"UPDATE TB_PATTERN_ITEM SET FACP_ID=%d , UNIT_ID=%d , CHN_ID=%d , RLY_ID=%d  "
					, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
				);
				strWhere.Format(L" WHERE PT_ID=%d AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
					, pPtn->GetPatternID()
					, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID()
				);
			}
			else if (pChgLink->btInOutType == CHANGE_LK_OUTPUT)
			{
				pOldLnk = (CDataLinked *)pChgLink->pRefOld;
				pNewLnk = (CDataLinked *)pChgLink->pChange;
				strCmd.Format(L" UPDATE TB_LINK_RELAY "
					L" SET SRC_FACP = %d , SRC_UNIT = %d , SRC_CHN = %d , SRC_RLY = %d "
					, pNewLnk->GetSrcFacp(), pNewLnk->GetSrcUnit(), pNewLnk->GetSrcChn(), pNewLnk->GetSrcDev()
					, pNewLnk->GetTgtFacp(), pNewLnk->GetTgtUnit(), pNewLnk->GetTgtChn(), pNewLnk->GetTgtDev()
				);
				strWhere.Format(L" WHERE LINK_TYPE=%d "
					L"AND SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
					L" TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
					, pOldLnk->GetLinkType()
					, pOldLnk->GetSrcFacp(), pOldLnk->GetSrcUnit(), pOldLnk->GetSrcChn(), pOldLnk->GetSrcDev()
					, pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit(), pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev()
				);
			}
			else  if (pChgLink->btInOutType == CHANGE_LK_INPUT)
			{
				pOld = (CDataDevice*)pChgLink->pRefOld;
				pNew = (CDataDevice *)pChgLink->pChange;
				
				nInput = nOut = nContents = 0;
				nBuild = nBtype = nStair = nFloor = nRoom = 0;
				nPtn = 0;
				pInput = pNew->GetEqInput();
				pOut = pNew->GetEqOutput();
				pContents = pNew->GetEqOutContents();
				pEqName = pNew->GetEqName();
				if (pInput)
					nInput = (int)pInput->GetEquipID();
				if (pOut)
					nOut = (int)pOut->GetEquipID();
				if (pContents)
					nContents = (int)pContents->GetEquipID();
				if (pEqName)
					nEqID = (int)pEqName->GetEquipID();
				pLoc = pNew->GetDevInputLocation();
				if (pLoc)
				{
					nBuild = (int)pLoc->GetBuildID();
					nBtype = (int)pLoc->GetBTypeID();
					nStair = (int)pLoc->GetStairID();
					nFloor = (int)pLoc->GetFloorID();
					nRoom = (int)pLoc->GetRoomID();
				}

				strCmd.Format(L"UPDATE TB_RELAY_LIST SET RLY_NUM=%d,MAP_KEY='%s',RPT_NUM=%d"
					L",INPUT_ID=%d,OUTPUT_ID=%d"
					L",BD_ID=%d,BTYPE_ID=%d,STAIR_ID=%d,FL_ID=%d,RM_ID=%d "
					L",OUTCONTENTS_ID=%d,EQ_ID=%d , EQ_ADD_IDX='%s' "
					L", FACP_ID=%d , UNIT_ID=%d , CHN_ID=%d , RLY_ID=%d "
					, pNew->GetDeviceNum(), pNew->GetDevKey(), pNew->GetRepeatorNum()
					, nInput, nOut
					, nBuild, nBtype, nStair, nFloor, nRoom
					, nContents, nEqID, pNew->GetEqAddIndex()
					, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
				);
				strWhere.Format(L" WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
					, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID()
				);

			}
		}

		strSql = strCmd + strWhere;
		TRACE(strSql + L"\n");
		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			pChgLink->btChangeType = CHANGE_ACTION_FAILED;
			//return 0;
		}
	}
	return 1;
}


int CFormRelayEdit::ApplyChangeSystemMemory()
{
	CString strSql, strWhere, strCmd;
	POSITION pos;
	ST_CHANGE_LINKDATA * pChgLink;
	CDataPattern *pNewPtn , * pOldPtn;
	CDataDevice *pOld, *pNew , *pTemp;
	CDataLinked *pOldLnk, *pNewLnk;
	CString strOldKey , strIDKey;
	CDataSystem * pSys;
	CDataEquip *pEq;
	int nInType, nOutType, nEqName, nCont;
	BOOL bFirstAdd = FALSE;
	std::shared_ptr<CManagerPattern> spManager;
	spManager = m_pRefFasSysData->GetPatternManager();
	CMapSystemData * pMap = m_pRefFasSysData->GetSystemData();
	if (pMap == nullptr)
		return 0; 
	pos = m_ptrChangeList.GetHeadPosition();
	while (pos)
	{
		strSql = strWhere = strCmd = L"";
		pChgLink = (ST_CHANGE_LINKDATA*)m_ptrChangeList.GetNext(pos);
		if (pChgLink == nullptr)
			continue;

		// CHANGE_ACTION_DEL : 패터일때 삭제 , 출력일 때 삭제 , 
		if (pChgLink->btChangeType == CHANGE_ACTION_ADD)
		{

		}
		else if (pChgLink->btChangeType == CHANGE_ACTION_DEL)
		{
			if (pChgLink->btInOutType == CHANGE_LK_PATTERN)
			{
				// pOld Pattern에서 삭제
				pNewPtn = (CDataPattern*)pChgLink->pRefInput;
				pOld = (CDataDevice *)pChgLink->pRefOld;
				pOldPtn = spManager->GetPattern(pNewPtn->GetPatternID());
				pOldPtn->DeleteItemByID(LK_TYPE_RELEAY , pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID());
			}
			else if (pChgLink->btInOutType == CHANGE_LK_OUTPUT)
			{
				// pOld 연동데이터에서 삭제
				pTemp = (CDataDevice *)pChgLink->pRefInput;
				pOldLnk = (CDataLinked *)pChgLink->pRefOld;
				pTemp->DeleteLinkByID(LK_TYPE_RELEAY
					, pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit()
					, pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev());
			}
			else // 디바이스의 입력 변경에서 삭제는 기존 상태 유지  - 변경은 변경된 상태로 수정
			{
				pOld = (CDataDevice *)pChgLink->pRefOld;
				m_pRefFasSysData->DeleteDeviceInMemory(pOld);
				delete pOld;
				pOld = nullptr;
			}
		}
		else if (pChgLink->btChangeType == CHANGE_ACTION_KEEP)
		{
			continue;
		}
		else
		{
			if (pChgLink->btInOutType == CHANGE_LK_PATTERN)
			{
				pNewPtn = (CDataPattern*)pChgLink->pRefInput;
				pOldPtn = spManager->GetPattern(pNewPtn->GetPatternID());
				pOld = (CDataDevice *)pChgLink->pRefOld;
				pNew = (CDataDevice *)pChgLink->pChange;
				//pOldPtn->DeleteDeviceByID(pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID());
				
				//pOldPtn->DeleteDeviceByID();
				pSys = (*pMap)[pOld->GetDevKey()];
				if (pSys == nullptr)
				{
					pSys = new CDataSystem;
				}
				strIDKey = GF_GetIDSysDataKey(SE_RELAY, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID());
				m_pRefFasSysData->RemoveSysMap(pOld->GetDevKey(), strIDKey);
				pMap->erase(pOld->GetDevKey());
				pSys->SetDataType(SE_RELAY);
				pSys->SetKey(pNew->GetDevKey());
				pSys->SetSysData(pOld);
				pOld->SetAddressInfo(
					pNew->GetFacpID(),pNew->GetUnitID(),pNew->GetChnID(),pNew->GetDeviceID()
					,pNew->GetFacpNum(),pNew->GetUnitNum(),pNew->GetChnNum(),pNew->GetDeviceNum()
				);
				strIDKey = GF_GetIDSysDataKey(SE_RELAY, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID());
				m_pRefFasSysData->AddSysMap(pNew->GetDevKey()
					, strIDKey
					, pSys
				);
				//(*pMap)[pNew->GetDevKey()] = pSys;


// 				delete pNew;
// 				pNew = nullptr;
			}
			else if (pChgLink->btInOutType == CHANGE_LK_OUTPUT)
			{
				pNew = (CDataDevice *)pChgLink->pRefInput;
				pOldLnk = (CDataLinked *)pChgLink->pRefOld;
				pNewLnk = (CDataLinked *)pChgLink->pChange;
				pOld = m_pRefFasSysData->GetDeviceByID(
					pOldLnk->GetSrcFacp(), pOldLnk->GetSrcUnit()
					, pOldLnk->GetSrcChn(), pOldLnk->GetSrcDev());
				if (pOld == nullptr)
					continue; 

				pTemp = m_pRefFasSysData->GetDeviceByID(
					pOldLnk->GetTgtFacp(), pOldLnk->GetTgtUnit()
					, pOldLnk->GetTgtChn(), pOldLnk->GetTgtDev());
				if (pTemp == nullptr)
					continue;

				nInType = nOutType = nEqName = nCont = -1;
				pEq = pTemp->GetEqInput();
				if (pEq)
					nInType = pEq->GetEquipID();
				pEq = pTemp->GetEqName();
				if (pEq)
					nEqName = pEq->GetEquipID();
				pEq = pTemp->GetEqOutput();
				if (pEq)
					nOutType = pEq->GetEquipID();
				pEq = pTemp->GetEqOutContents();
				if (pEq)
					nCont = pEq->GetEquipID();
				// [KHS 2020-5-18 11:25:25] 
				// 교차 감지기 일때
				// 출력 타입 - 프리액션 , 출력내용 - 밸브 일때 
				if (nOutType == OUTTYPE_PREACTION && nCont == OUTCONT_VALVE)
					bFirstAdd = TRUE;
				else
					bFirstAdd = FALSE;
				// 이전 연동데이터를 삭제하고 새 연동데이터를 입력
				pOld->DeleteLink(pOldLnk);
				pOld->AddLink(bFirstAdd , pNewLnk);

				// 이전에 등록되어 있는 SystemData 삭제
				strOldKey = pOld->GetDevKey();
				pSys = (*pMap)[pOld->GetDevKey()];
				if (pSys == nullptr)
				{
					pSys = new CDataSystem;
				}

				pMap->erase(pOld->GetDevKey());
				pSys->SetDataType(SE_RELAY);
				pSys->SetKey(pNew->GetDevKey());
				pSys->SetSysData(pOld);

				pOld->SetAddressInfo(
					pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
					, pNew->GetFacpNum(), pNew->GetUnitNum(), pNew->GetChnNum(), pNew->GetDeviceNum()
				);
				(*pMap)[pNew->GetDevKey()] = pSys;
// 				delete pNew;
// 				pNew = nullptr;
			}
			else  if (pChgLink->btInOutType == CHANGE_LK_INPUT)
			{
				pOld = (CDataDevice*)pChgLink->pRefOld;
				pNew = (CDataDevice *)pChgLink->pChange;

				// 이전에 등록되어 있는 SystemData 삭제
				pSys = (*pMap)[pOld->GetDevKey()];
				if (pSys == nullptr)
				{
					pSys = new CDataSystem;
				}

				pMap->erase(pOld->GetDevKey());
				pSys->SetDataType(SE_RELAY);
				pSys->SetKey(pNew->GetDevKey());
				pSys->SetSysData(pOld);

				pOld->SetAddressInfo(
					pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
					, pNew->GetFacpNum(), pNew->GetUnitNum(), pNew->GetChnNum(), pNew->GetDeviceNum()
				);
				(*pMap)[pNew->GetDevKey()] = pSys;

				delete pNew;
				pNew = nullptr;
			}
		}
	}
	//pMap->sort();
	return 1;
}

int CFormRelayEdit::DeleteAffectedByChange(YAdoDatabase * pDB)
{
	CString strSql, strWhere, strCmd;
	POSITION pos;
	ST_CHANGE_LINKDATA * pChgLink;
	CDataPattern * pPtn;
	CDataDevice *pOld, *pNew;
	CDataLocBase * pLoc;
	CDataLinked * pOLnk, *pNLnk;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nPtn;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	pos = m_ptrChangeList.GetHeadPosition();
	while (pos)
	{
		strSql = strWhere = strCmd = L"";
		pChgLink = (ST_CHANGE_LINKDATA*)m_ptrChangeList.GetNext(pos);
		if (pChgLink == nullptr)
			continue;
		if (pChgLink->pRefInput == nullptr)
			continue;

		// CHANGE_ACTION_DEL : 패터일때 삭제 , 출력일 때 삭제 , 
		if (pChgLink->btChangeType == CHANGE_ACTION_ADD)
			continue;

		if (pChgLink->btChangeType == CHANGE_ACTION_DEL)
		{
			if (pChgLink->btInOutType == CHANGE_LK_PATTERN)
			{
				pNew = (CDataDevice*)pChgLink->pChange;
				pPtn = (CDataPattern*)pChgLink->pRefInput;
				strCmd.Format(L"DELETE TB_PATTERN_ITEM ");
				strWhere.Format(L" WHERE PT_ID=%d AND FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
					, pPtn->GetPatternID()
					, pNew->GetFacpID(), pNew->GetUnitID(), pNew->GetChnID(), pNew->GetDeviceID()
				);
			}
			else if (pChgLink->btInOutType == CHANGE_LK_OUTPUT)
			{
				pOLnk = (CDataLinked *)pChgLink->pRefOld;
				pNLnk = (CDataLinked *)pChgLink->pChange;
				pOld = (CDataDevice *)pChgLink->pRefInput;
				strCmd.Format(L" DELETE TB_LINK_RELAY ");
				strWhere.Format(L" WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
					L" AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
					, pOLnk->GetSrcFacp(), pOLnk->GetSrcUnit(), pOLnk->GetSrcChn(), pOLnk->GetSrcDev()
					, pOLnk->GetTgtFacp(), pOLnk->GetTgtUnit(), pOLnk->GetTgtChn(), pOLnk->GetTgtDev()
				);
			}
			else // 디바이스의 입력 변경에서 삭제는 기존 상태 유지  - 변경은 변경된 상태로 수정
				continue;
		}
		else if (pChgLink->btChangeType == CHANGE_ACTION_KEEP)
		{
			pNew = (CDataDevice*)pChgLink->pChange;
			continue;
		}
		else
		{
			pNew = (CDataDevice*)pChgLink->pChange;
			nInput = nOut = nContents = 0;
			nBuild = nBtype = nStair = nFloor = nRoom = 0;
			nPtn = 0;
			pInput = pNew->GetEqInput();
			pOut = pNew->GetEqOutput();
			pContents = pNew->GetEqOutContents();
			pEqName = pNew->GetEqName();
			if (pInput)
				nInput = (int)pInput->GetEquipID();
			if (pOut)
				nOut = (int)pOut->GetEquipID();
			if (pContents)
				nContents = (int)pContents->GetEquipID();
			if (pEqName)
				nEqID = (int)pEqName->GetEquipID();
			pLoc = pNew->GetDevInputLocation();
			if (pLoc)
			{
				nBuild = (int)pLoc->GetBuildID();
				nBtype = (int)pLoc->GetBTypeID();
				nStair = (int)pLoc->GetStairID();
				nFloor = (int)pLoc->GetFloorID();
				nRoom = (int)pLoc->GetRoomID();
			}

			if (pChgLink->btInOutType == CHANGE_LK_INPUT)
				pOld = (CDataDevice *)pChgLink->pRefInput;
			else if (pChgLink->btInOutType == CHANGE_LK_OUTPUT)
				pOld = (CDataDevice *)pChgLink->pRefOld;
			else // 디바이스의 입력 변경에서 삭제는 기존 상태 유지  - 변경은 변경된 상태로 수정
				continue;

			strCmd.Format(L"UPDATE TB_RELAY_LIST SET RLY_NUM=%d,MAP_KEY='%s',RPT_NUM=%d"
				L",INPUT_ID=%d,OUTPUT_ID=%d"
				L",BD_ID=%d,BTYPE_ID=%d,STAIR_ID=%d,FL_ID=%d,RM_ID=%d "
				L",OUTCONTENTS_ID=%d,EQ_ID=%d , EQ_ADD_IDX='%s' "
				, pNew->GetDeviceNum(), pNew->GetDevKey(), pNew->GetRepeatorNum()
				, nInput, nOut
				, nBuild, nBtype, nStair, nFloor, nRoom
				, nContents, nEqID, pNew->GetEqAddIndex()
			);
			strWhere.Format(L" WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
				, pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID()
			);
		}

		strSql = strCmd + strWhere;
		if (pDB->ExecuteSql(strSql) == FALSE)
			return 0;
	}
	return 1;
}


int CFormRelayEdit::ApplyDeleteSystemMemory()
{
	CString strSql, strWhere, strCmd;
	POSITION pos;
	ST_CHANGE_LINKDATA * pChgLink;
	CDataPattern *pPtn;
	CDataDevice *pOld, *pNew, *pDev;
	CDataLinked * pOLnk, *pNLnk;
	CDataSystem * pSys;
	pos = m_ptrChangeList.GetHeadPosition();
	while (pos)
	{
		strSql = strWhere = strCmd = L"";
		pChgLink = (ST_CHANGE_LINKDATA*)m_ptrChangeList.GetNext(pos);
		if (pChgLink == nullptr)
			continue;
		if (pChgLink->pRefInput == nullptr)
			continue;

		// CHANGE_ACTION_DEL : 패터일때 삭제 , 출력일 때 삭제 , 
		if (pChgLink->btChangeType == CHANGE_ACTION_ADD)
			continue;

		if (pChgLink->btChangeType == CHANGE_ACTION_DEL)
		{
			if (pChgLink->btInOutType == CHANGE_LK_PATTERN)
			{
				pOld =(CDataDevice*) pChgLink->pRefOld;
				pNew = (CDataDevice*)pChgLink->pChange;
				// pOld Pattern에서 삭제
				pPtn = (CDataPattern*)pChgLink->pRefInput;
				pPtn->DeleteItemByID(LK_TYPE_RELEAY ,pOld->GetFacpID(), pOld->GetUnitID(), pOld->GetChnID(), pOld->GetDeviceID());
			}
			else if (pChgLink->btInOutType == CHANGE_LK_OUTPUT)
			{
				// pOld 연동데이터에서 삭제
				pDev = (CDataDevice *)pChgLink->pRefInput;
				pOLnk = (CDataLinked *)pChgLink->pRefOld;
				pNLnk = (CDataLinked *)pChgLink->pChange;
				pDev->DeleteLinkByID(LK_TYPE_RELEAY
					, pOLnk->GetTgtFacp(), pOLnk->GetTgtUnit()
					, pOLnk->GetTgtChn(), pOLnk->GetTgtDev());
			}
			else // 디바이스의 입력 변경에서 삭제는 기존 상태 유지  - 변경은 변경된 상태로 수정
				continue;
		}
		else if (pChgLink->btChangeType == CHANGE_ACTION_KEEP)
		{
			continue;
		}
		else
		{
// 			if (pChgLink->btInOutType == CHANGE_LK_INPUT)
// 				pOld = (CDataDevice *)pChgLink->pRefInput;
// 			else if (pChgLink->btInOutType == CHANGE_LK_OUTPUT)
// 				pOld = (CDataDevice *)pChgLink->pRefOld;
// 			else // 디바이스의 입력 변경에서 삭제는 기존 상태 유지  - 변경은 변경된 상태로 수정
// 				continue;
// 			pOld->CopyData(pNew);
		}


	}
// 	CMapSystemData *  pMap = m_pRefFasSysData->GetSystemData();
// 	if (pMap == nullptr)
// 		return 0;

	pos = m_ptrSysRefChangeList.GetHeadPosition();
	while (pos)
	{
		pSys = (CDataSystem *)m_ptrSysRefChangeList.GetNext(pos);
		if (pSys == nullptr)
			continue; 
		m_pRefFasSysData->RemoveSysMap(pSys);
		//pMap->erase(pSys->GetKey());
		pSys->DeleteData();
		delete pSys;
		pSys;
	}
	return 1;
}

LRESULT CFormRelayEdit::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	return 0;
}

void CFormRelayEdit::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


int CFormRelayEdit::IsSameAddress(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData)
{
	int nO_Fid, nO_Uid, nO_Cid, nO_Did;
	int nO_FNum, nO_UNum, nO_CNum, nO_DNum;
	int nN_Fid, nN_Uid, nN_Cid, nN_Did;
	int nN_FNum, nN_UNum, nN_CNum, nN_DNum;

	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;

	nO_Fid = nO_Uid = nO_Cid = nO_Did = -1;
	nO_FNum = nO_UNum = nO_CNum = nO_DNum = -1;
	nN_Fid = nN_Uid = nN_Cid = nN_Did = -1;
	nN_FNum = nN_UNum = nN_CNum = nN_DNum = -1;
	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;
	GetIDNum(pNewData, nN_Fid, nN_Uid, nN_Cid, nN_Did, nN_FNum, nN_UNum, nN_CNum, nN_DNum);
	GetIDNum(pCurData, nO_Fid, nO_Uid, nO_Cid, nO_Did, nO_FNum, nO_UNum, nO_CNum, nO_DNum);

	if (nN_FNum == nO_FNum 
		&& nN_UNum == nO_UNum 
		&& nN_CNum == nO_CNum 
		&& nN_DNum == nO_DNum)
		return 1;
	return 0;
}
