// FormLocation.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormLocation.h"

#include "DataDevice.h"
#include "DataLinked.h"
#include "MapSystemData.h"
#include "MapLocation.h"
#include "DataLocation.h"
#include "RelayTableData.h"
#include "DataLocBase.h"
#include "DataLocFloor.h"
#include "DataLocStair.h"
#include "../Common/Utils/YAdoDatabase.h"
// CFormLocation

IMPLEMENT_DYNCREATE(CFormLocation, CFormView)

CFormLocation::CFormLocation()
	: CFormView(IDD_FORMLOCATION)
{
	m_bModified = FALSE;
}

CFormLocation::~CFormLocation()
{
}

void CFormLocation::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICE_LIST, m_ctrlDeviceList);
	DDX_Control(pDX, IDC_LOCATION_TREE, m_ctrlLocTree);
}

BEGIN_MESSAGE_MAP(CFormLocation, CFormView)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_LOCATION_TREE, &CFormLocation::OnTvnSelchangedLocationTree)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_BTN_SAVE,&CFormLocation::OnBnClickedBtnSave)
	ON_NOTIFY(LVN_XL_EDIT_END,IDC_DEVICE_LIST,OnEditEdnDeviceList)
	ON_NOTIFY(NM_CLICK,IDC_LOCATION_TREE,&CFormLocation::OnNMClickLocationTree)
END_MESSAGE_MAP()


// CFormLocation 진단입니다.

#ifdef _DEBUG
void CFormLocation::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormLocation::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormLocation 메시지 처리기입니다.


void CFormLocation::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_ImgLocation.Create(IDB_BMP_DEVICE_ICON, 16, 8, RGB(0, 255, 255));
	m_ctrlLocTree.SetImageList(&m_ImgLocation, TVSIL_NORMAL);


	m_ctrlDeviceList.InsertColumn(0, L"층이름", LVCFMT_LEFT, 200);
	m_ctrlDeviceList.InsertColumn(1, L"층번호", LVCFMT_LEFT, 80);

	m_ctrlDeviceList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ctrlDeviceList.SetSortType(0,SRT_STRING);
	m_ctrlDeviceList.SetSortType(1,SRT_INT);
	CRelayTableData * pRelayTable = theApp.GetRelayTableData();

	if (pRelayTable != nullptr)
		pRelayTable->FillLocationTree2(&m_ctrlLocTree,LT_STAIR);

	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	m_bModified = FALSE;
}
// 
// int CFormLocation::FillDeviceList(CDataLocBase* pLoc)
// {
// 	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
// 
// 	if (pRelayTable == nullptr)
// 		return 0; 
// 
// 	int nType, nIdx = 0;
// 	CDataSystem * pData;
// 	CDataDevice * pDev;
// 	CDataLocBase * pDevLoc;
// 	CMapSystemData::iterator it;
// 	CMapSystemData * pRefMap = nullptr;
// 	CString strBuild, strBtype, strStair, strFloor, strRoom, strtemp;
// 
// 	m_ctrlDeviceList.DeleteAllItems();
// 
// 
// 
// 	strBuild = strBtype = strStair = strFloor = strRoom = L"";
// 	CString str;
// 	std::vector<CString> vt;
// // 	int nSize;
// // 	vt = GF_SplitString(strtemp, L".");
// // 	nSize = vt.size();
// // 	if (nSize < D_MAX_LOCKEY_SPLIT_COUNT)
// // 	{
// // 		m_ctrlDeviceList.SetRedraw();
// // 		return nSize;
// // 	}
// 	strBuild = pLoc->GetBuildName();
// 	strBtype = pLoc->GetBTypeName();
// 	strStair = pLoc->GetStairName();
// 	strFloor = pLoc->GetFloorName();
// 	strRoom = pLoc->GetRoomName();
// 	nType = pLoc->GetLocType();
// 
// 	pRefMap = pRelayTable->GetSystemData();
// 	if (pRefMap == nullptr)
// 	{
// 		m_ctrlDeviceList.SetRedraw();
// 		return 0;
// 	}
// 
// 	for (it = pRefMap->begin(); it != pRefMap->end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 
// 		if (pData->GetDataType() != SE_RELAY)
// 			continue;
// 		if (pData->GetSysData() == nullptr)
// 			continue;
// 
// 		pDev = (CDataDevice*)pData->GetSysData();
// 
// 		pDevLoc = pDev->GetDevInputLocation();
// 		if (pDevLoc == nullptr)
// 			continue;
// 
// 		if (pDevLoc->GetBuildName() != strBuild)
// 			continue;
// 		if (nType >= LT_BTYPE && pDevLoc->GetBTypeName() != strBtype)
// 			continue;
// 		if (nType >= LT_STAIR && pDevLoc->GetStairName() != strStair)
// 			continue;
// 		if (nType >= LT_FLOOR && pDevLoc->GetFloorName() != strFloor)
// 			continue;
// 
// 		if (nType >= LT_ROOM && pDevLoc->GetRoomName() != strRoom)
// 			continue;
// 
// 		m_ctrlDeviceList.InsertItem(nIdx, pDev->GetOutContentsFullName());
// 		m_ctrlDeviceList.SetItemText(nIdx, 1, pDev->GetInputTypeName());
// 		m_ctrlDeviceList.SetItemText(nIdx, 2, pDev->GetOutputTypeName());
// 		m_ctrlDeviceList.SetItemText(nIdx, 3, pDev->GetEquipName());
// 		m_ctrlDeviceList.SetItemText(nIdx, 4, pDev->GetEqAddIndex());
// 		m_ctrlDeviceList.SetItemText(nIdx, 5, pDev->GetOutContentsName());
// 		m_ctrlDeviceList.SetItemText(nIdx, 6, pDev->GetOutputLocationName());
// 		m_ctrlDeviceList.SetItemText(nIdx, 7, L"");
// 		m_ctrlDeviceList.SetItemText(nIdx, 8, L"감지기/중계기");
// 		m_ctrlDeviceList.SetItemData(nIdx, (DWORD_PTR)pDev);
// 		nIdx++;
// 	}
// 	m_ctrlDeviceList.SetRedraw();
// 
// 	return nIdx;
// }


int CFormLocation::FillLevelList(CDataLocBase* pLoc)
{
	m_ctrlDeviceList.DeleteAllItems();
	CRelayTableData * pRelayTable = theApp.GetRelayTableData();

	if(pRelayTable == nullptr)
		return 0;
	
	if(pLoc->GetLocType() != LT_STAIR)
		return 0; 

	int i = 0; 
	CString strName,strtemp ;
	CDataLocFloor * pData;
	CMapLocFloor * pMapFloor;
	CMapLocFloor::iterator it;
	pMapFloor =&(((CDataLocStair*)pLoc)->m_mapFloor);
	for(it = pMapFloor->begin(); it != pMapFloor->end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		strName = pData->GetFloorName();
		m_ctrlDeviceList.InsertItem(i,strName);
		strtemp.Format(L"%d",pData->GetFloorNumber());
		m_ctrlDeviceList.SetItemText(i,1,strtemp);
		strName.Remove(' ');
		if(strName != "-")
			m_ctrlDeviceList.SetEdit(i,1,true,true);
		m_ctrlDeviceList.SetItemData(i,(LPARAM)pData);
	}
	m_ctrlDeviceList.Sort(1,TRUE);
	return 0;
}
BOOL CFormLocation::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::PreCreateWindow(cs);
}

int CFormLocation::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CFormLocation::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnClose();
}


void CFormLocation::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_ctrlLocTree.GetSafeHwnd() == nullptr || m_ctrlDeviceList.GetSafeHwnd() == nullptr)
		return;
	CRect rc, rcTree, rcList;
	GetClientRect(&rc);
	rcTree.left = 5;
	rcTree.top = 5;
	rcTree.bottom = cy - 5;
	rcTree.right = 300;
	m_ctrlLocTree.MoveWindow(&rcTree);

	rcList.left = rcTree.right + 5;
	rcList.right = cx - 5;
	rcList.top = 45;
	rcList.bottom = cy - 5;
	m_ctrlDeviceList.MoveWindow(&rcList);
}


void CFormLocation::OnTvnSelchangedLocationTree(NMHDR *pNMHDR, LRESULT *pResult)
{
 	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
 	HTREEITEM hSel = pNMTreeView->itemNew.hItem;
 	if (hSel == nullptr)
 		return;
 
//  	if(m_bModified)
//  	{
//  		if(AfxMessageBox(L"변경 사항이 있습니다 저장하시겠습니까?",MB_YESNO | MB_ICONQUESTION) == IDYES)
//  		{
//  			// SAVE
//  			if(SaveLevelInfo() == 0)
//  			{
//  				*pResult = 1;
//  				return;
//  			}
//  		}
//  		
//  	}
//  
 	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
 	m_bModified = FALSE;
 	CDataLocBase * pLoc;
 	pLoc = (CDataLocBase *)m_ctrlLocTree.GetItemData(hSel);
 	if (pLoc == nullptr)
 		return;
 
 	FillLevelList(pLoc);
	*pResult = 0;
}

int CFormLocation::SaveLevelInfo()
{
	BOOL bDuplicate = FALSE;
	int i = 0 , nCnt,nFNum; 
	CDataLocFloor * pLoc;
	CString strPrev = L"",strCur = L"", strError = L"",strtemp,strSql;
	YAdoDatabase * pDBUtil = nullptr;
	CRelayTableData * pRelayTable = theApp.GetRelayTableData();

	if(pRelayTable == nullptr)
		return 0; 

	m_ctrlDeviceList.Sort(1,TRUE);

	nCnt = m_ctrlDeviceList.GetItemCount();

	for(i = 0; i < nCnt; i++)
	{
		strCur = m_ctrlDeviceList.GetItemText(i,1);
		if(strCur == strPrev)
		{
			bDuplicate = TRUE;
			strtemp.Format(L"   - '%s'의 층번호[%s]가 이전 층번호[%s]와 중복됩니다.\n"
				,m_ctrlDeviceList.GetItemText(i,0),strCur,strPrev
			);
			strError += strtemp;
		}
		strPrev = strCur;
	}

	if(bDuplicate)
	{
		strtemp.Format(L"중복되는 층번호가 있습니다.\n%s"
			,strError
		);
		AfxMessageBox(strtemp,MB_OK | MB_ICONWARNING);
		//return 0; 
	}

	pDBUtil = new YAdoDatabase;
	pDBUtil->MSSqlInitialize(g_stConfig.szDBPass,g_stConfig.szDBUser
		,pRelayTable->GetPrjName(),g_stConfig.szSource);

	if(pDBUtil->DBOpen() == FALSE)
	{
		GF_AddDebug(L"층번 저장 오류 : 데이터베이스를 연결하는데 실패했습니다.");
		AfxMessageBox(L"데이터베이스 접속에 실패했습니다.");
		return 0;

	}
	
	pDBUtil->BeginTransaction();
	for(i = 0; i < nCnt; i++)
	{
		pLoc = (CDataLocFloor*)m_ctrlDeviceList.GetItemData(i);
		if(pLoc == nullptr)
			continue; 

		strtemp = m_ctrlDeviceList.GetItemText(i,1);
		nFNum = _wtoi(strtemp);
		strSql.Format(
			L"SELECT * FROM TB_LOC_FLOOR "
			L" WHERE BD_ID=%d AND BTYPE_ID=%d AND STAIR_ID=%d AND FL_ID=%d"
			,pLoc->GetBuildID(),pLoc->GetBTypeID(),pLoc->GetStairID(),pLoc->GetFloorID()
		);
		if((pDBUtil->OpenQuery(strSql) == FALSE)
			|| (pDBUtil->GetRecordCount() <= 0))
		{
			strSql.Format(L"INSERT INTO TB_LOC_FLOOR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,FL_NAME,FL_NUM,FL_MIDDLE,ADD_USER) "
				L" VALUES(1, %d,%d,%d,%d,'%s',%d,%d,'%s') "
				, pLoc->GetBuildID(),pLoc->GetBTypeID(),pLoc->GetStairID()
				, pLoc->GetFloorID() ,pLoc->GetFloorName(),pLoc->GetFloorNumber() ,pLoc->GetMiddleFloor()
				, pRelayTable->GetCurrentUser()
			);
		}
		else
		{
			strSql.Format(
				L" UPDATE TB_LOC_FLOOR SET FL_NAME='%s' , FL_NUM=%d ,ADD_USER='%s'"
				L" WHERE BD_ID=%d AND BTYPE_ID=%d AND STAIR_ID=%d AND FL_ID=%d "
				,m_ctrlDeviceList.GetItemText(i,0),nFNum , pRelayTable->GetCurrentUser()
				,pLoc->GetBuildID(),pLoc->GetBTypeID(),pLoc->GetStairID(),pLoc->GetFloorID()
			);
		}

		if(pDBUtil->ExecuteSql(strSql) == FALSE)
		{
			pDBUtil->RollbackTransaction();
			pDBUtil->DBClose();
			delete pDBUtil;
			pDBUtil = nullptr;
			strError.Format(L"'%s'의 층번호[%d]를 변경중 데이터베이스 오류가 발생했습니다."
				,m_ctrlDeviceList.GetItemText(i,0),nFNum
			);
			strtemp.Format(L"층번호를 변경 중에 오류가 발생했습니다.\n%s"
				,strError
			);
			AfxMessageBox(strtemp,MB_OK | MB_ICONERROR);
			return 0; 
		}
	}
	pDBUtil->CommitTransaction();
	pDBUtil->DBClose();
	delete pDBUtil;
	pDBUtil = nullptr;

	for(i = 0; i < nCnt; i++)
	{
		pLoc = (CDataLocFloor*)m_ctrlDeviceList.GetItemData(i);
		if(pLoc == nullptr)
			continue;
		strtemp = m_ctrlDeviceList.GetItemText(i,1);
		nFNum = _wtoi(strtemp);
		pLoc->SetFloorNumber(nFNum);
		pLoc->SetFloorName(m_ctrlDeviceList.GetItemText(i,0));
	}
	m_bModified = FALSE;
	return 1; 
}
LRESULT CFormLocation::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
// 	if (m_ctrlRelayList.GetSafeHwnd())
// 		m_ctrlRelayList.DeleteAllItems();
// 	if (m_ctrlPatternList)
// 		m_ctrlPatternList.DeleteAllItems();
	if (wp == FORM_PRJ_OPEN || wp == FORM_PRJ_NEW)
	{
		CRelayTableData * pRelayTable = theApp.GetRelayTableData();

		if (pRelayTable != nullptr)
			pRelayTable->FillLocationTree(&m_ctrlLocTree);
	}
	return 1;
}


void CFormLocation::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CFormLocation::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(SaveLevelInfo() == 0)
	{
		//AfxMessageBox(L"층번호를 수정하는데 실패했습니다.",MB_OK | MB_ICONERROR);
		if(AfxMessageBox(L"변경 되기 전의 층 정보를 다시 가져올까요?",MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			m_bModified = FALSE;
			GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
			m_bModified = FALSE;
			CDataLocBase * pLoc;
			HTREEITEM hSel = m_ctrlLocTree.GetSelectedItem();
			pLoc = (CDataLocBase *)m_ctrlLocTree.GetItemData(hSel);
			if(pLoc == nullptr)
				return;

			FillLevelList(pLoc);
		}
		else
		{

		}
	}
	else
	{
		AfxMessageBox(L"층번호를 수정하는데 성공했습니다.",MB_OK|MB_ICONINFORMATION);
	}
}


void CFormLocation::OnEditEdnDeviceList(NMHDR *pNMHDR,LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if(pNMHDR == nullptr)
		return;

	XNOTIFY_EDIT_END *pxnData = reinterpret_cast<XNOTIFY_EDIT_END*>(pNMHDR);
	if(pxnData == nullptr)
		return;
	CDataLocFloor * pLoc;
	CString str;
	int nFNum = 0; 
	int nIdx = pxnData->dispinfo.item.iItem;
	if(nIdx < 0)
		return;
	pLoc = (CDataLocFloor*)m_ctrlDeviceList.GetItemData(nIdx);
	if(pLoc == nullptr)
		return; 
	str = m_ctrlDeviceList.GetItemText(nIdx,1);
	nFNum = _wtoi(str);
	if(pLoc->GetFloorNumber() != nFNum)
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
		m_bModified = TRUE;
	}
}


void CFormLocation::OnNMClickLocationTree(NMHDR *pNMHDR,LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hSel = m_ctrlLocTree.GetSelectedItem();
 	if(hSel == nullptr)
	{
		*pResult = 1;
		return;
	}
 
 	if(m_bModified)
 	{
 		if(AfxMessageBox(L"변경 사항이 있습니다 저장하시겠습니까?",MB_YESNO | MB_ICONQUESTION) == IDYES)
 		{
 			// SAVE
 			if(SaveLevelInfo() == 0)
 			{
				AfxMessageBox(L"변경 사항을 저장하는데 실패했습니다.");
 				*pResult = 1;
 				return;
 			}
 		}
		m_bModified = FALSE;
 	}
 
//  	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
//  	m_bModified = FALSE;
//  	CDataLocBase * pLoc;
//  	pLoc = (CDataLocBase *)m_ctrlLocTree.GetItemData(hSel);
//  	if(pLoc == nullptr)
//  		return;
//  
//  	FillLevelList(pLoc);

}

int CFormLocation::SelectItem(int nItemType,DWORD_PTR dwData)
{
	HTREEITEM hFind = nullptr;
	hFind = GF_FindTreeByData(&m_ctrlLocTree,m_ctrlLocTree.GetRootItem(),dwData);
	if(hFind == nullptr)
		return 0;

	m_ctrlLocTree.SelectItem(hFind);
	return 1;
}
