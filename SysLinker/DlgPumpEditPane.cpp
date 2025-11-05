// DlgPumpEditPane.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgPumpEditPane.h"
#include "afxdialogex.h"

#include "../Common/CustomList/CustomBaseItem.h"
#include "../Common/CustomList/CustomListCtrl.h"
#include "../Common/CustomList/Def_CustomCtrl.h"
#include "PumpItem.h"
#include "BtnFacp.h"
#include "DataFacp.h"
#include "DataPump.h"
#include "DataPS.h"
#include "ManagerPS.h"
#include "ManagerPump.h"
#include "ManagerFacp.h"
#include "RelayTableData.h"
#include "PumpTypeList.h"
// CDlgPumpEditPane 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgPumpEditPane, CDialogEx)

CDlgPumpEditPane::CDlgPumpEditPane(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PUMPEDIT_PANE, pParent)
	,m_strTypeName(_T(""))
	,m_strLcdMsg(_T(""))
{
	m_pCustomListCtrl = nullptr;
	m_bPumpTypeDragging = FALSE;
	m_pPumpTypeDragImage = nullptr;
	m_nPumpTypeDragItem = -1;
	m_pRefFasSysData = nullptr;
	m_pPtrRefPumpTypeList = nullptr;
	m_bModified = FALSE;
	m_pCurFacpData = nullptr;
	m_pMainForm = nullptr;
	m_pPtrRefPumpTemplete = nullptr;
}

CDlgPumpEditPane::~CDlgPumpEditPane()
{
	if(m_pCustomListCtrl)
	{
		delete m_pCustomListCtrl;
		m_pCustomListCtrl = nullptr;
	}
}

void CDlgPumpEditPane::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_PUMPTYPE_LIST,m_ctrlPumpTypeList);

	for(int i = 0; i < D_MAX_FACP_COUNT; i++)
	{
		DDX_Control(pDX,IDC_RD_FACP0 + i,m_arrFacpBtn[i]);
	}
	DDX_Control(pDX,IDC_CMB_PSTYPE,m_cmbPsType);
	DDX_Control(pDX,IDC_CMB_PUMPTYPE,m_cmbPumpType);
	DDX_Text(pDX,IDC_ED_TYPENAME,m_strTypeName);
	DDX_Text(pDX,IDC_ED_LCDMSG,m_strLcdMsg);
	DDX_Control(pDX,IDC_CMB_PUMPTEMPLETE,m_cmbPumpTemplete);
	DDX_Control(pDX,IDC_ED_TYPENAME,m_edTypeName);
	DDX_Control(pDX,IDC_ED_LCDMSG,m_edLcdMsg);
}


BEGIN_MESSAGE_MAP(CDlgPumpEditPane, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK,&CDlgPumpEditPane::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,&CDlgPumpEditPane::OnBnClickedCancel)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RD_FACP0,IDC_RD_FACP0 + D_MAX_FACP_COUNT - 1,OnRadioFacpClicked)
	ON_NOTIFY(LVN_BEGINDRAG,IDC_PUMPTYPE_LIST,&CDlgPumpEditPane::OnLvnBegindragPumptypeList)
	ON_NOTIFY(NMCL_LBUTTON_UP,IDC_CTRLLIST_VIEW,&CDlgPumpEditPane::OnClcnItemUp)
	ON_NOTIFY(NMCL_ITEM_SELCHANGED,IDC_CTRLLIST_VIEW,&CDlgPumpEditPane::OnClcnItemSelChanged)
	ON_NOTIFY(NMCL_END_DELITEM,IDC_CTRLLIST_VIEW,&CDlgPumpEditPane::OnClcnItemDelete)
	ON_NOTIFY(NMCL_END_CHANGEITEM,IDC_CTRLLIST_VIEW,&CDlgPumpEditPane::OnClcnItemEditEnd)
	ON_NOTIFY(NMCL_ITEM_ENDDRAG,IDC_CTRLLIST_VIEW,&CDlgPumpEditPane::OnClcnItemEndDrag)
	ON_BN_CLICKED(IDC_BTN_ADD,&CDlgPumpEditPane::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MODIFY,&CDlgPumpEditPane::OnBnClickedBtnModify)
	ON_BN_CLICKED(IDC_BTN_DEL,&CDlgPumpEditPane::OnBnClickedBtnDel)
	ON_CBN_SELCHANGE(IDC_CMB_PSTYPE,&CDlgPumpEditPane::OnCbnSelchangeCmbPstype)
	ON_CBN_SELCHANGE(IDC_CMB_PUMPTYPE,&CDlgPumpEditPane::OnCbnSelchangeCmbPumptype)
	ON_EN_CHANGE(IDC_ED_LCDMSG,&CDlgPumpEditPane::OnEnChangeEdLcdmsg)
	ON_NOTIFY(NM_CLICK,IDC_PUMPTYPE_LIST,&CDlgPumpEditPane::OnNMClickPumptypeList)
	ON_BN_CLICKED(IDC_BTN_INITFACP,&CDlgPumpEditPane::OnBnClickedBtnInitfacp)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_CMB_PUMPTEMPLETE,&CDlgPumpEditPane::OnCbnSelchangeCmbPumptemplete)
	ON_BN_CLICKED(IDC_BTN_ADDTEMPLETE,&CDlgPumpEditPane::OnBnClickedBtnAddtemplete)
END_MESSAGE_MAP()


// CDlgPumpEditPane 메시지 처리기입니다.


BOOL CDlgPumpEditPane::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rc;
	GetClientRect(&rc);
	rc.left = 10;
	rc.top = 10;
	rc.right = rc.right - 10;
	rc.bottom = rc.bottom - 10;

	CRuntimeClass	*pRuntimeClass;
	if(NULL != (pRuntimeClass = RUNTIME_CLASS(CCustomListCtrl)))
	{
		if(NULL != (m_pCustomListCtrl = (CCustomListCtrl *)(pRuntimeClass->CreateObject())))
		{
			m_pCustomListCtrl->Create(NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,rc,this,IDC_CTRLLIST_VIEW,NULL);
		}
	}
	m_pCustomListCtrl->SendMessage(WM_INITIALUPDATE);

	int i = 0;
	CPumpItem * pItem = nullptr;
	for(i = 0; i < D_MAX_PUMP_COUNT; i ++)
	{
		pItem = new CPumpItem(i + 1,IDB_PUMPITEM_UNASSIGN,m_pCustomListCtrl);
		pItem->CreateControl(m_pCustomListCtrl);
		pItem->ShowWindow(SW_SHOW);
		m_pCustomListCtrl->AddItem(pItem);
		pItem->InitCustomItem();
		pItem->SetSelected(FALSE);
	}

	if(m_ImageList.GetSafeHandle() == NULL)
	{
		m_ImageList.Create(62,61,ILC_COLOR24 | ILC_MASK,1,1);
	}
	CBitmap bmpImage;
	if(!bmpImage.LoadBitmap(IDB_PUMP_SYMBOL))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"),IDB_PUMP_SYMBOL);
		ASSERT(FALSE);
		return FALSE;
	}
	m_ImageList.Add(&bmpImage,RGB(52,52,52));
	m_ImageList.SetImageCount(1);

	m_ftList.CreatePointFont(130,L"Arial");
	m_ctrlPumpTypeList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FLATSB);
	m_ctrlPumpTypeList.SetFont(&m_ftList);
	m_ctrlPumpTypeList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_ctrlPumpTypeList.SetImageList(&m_ImageList,LVSIL_NORMAL);
#ifndef ENGLISH_MODE
	m_ctrlPumpTypeList.InsertColumn(0,L"펌프목록",LVCFMT_CENTER,400);
#else
	m_ctrlPumpTypeList.InsertColumn(0,L"Pump Type",LVCFMT_CENTER,400);
#endif
	m_ctrlPumpTypeList.SetEnableDrag(FALSE);
	m_ctrlPumpTypeList.SetAlwaysVisibleSelection(TRUE);
	m_ctrlPumpTypeList.ModifyStyle(0,LVS_SHOWSELALWAYS);

#ifndef ENGLISH_MODE
	m_cmbPsType.InsertString(0,L"지속");
	m_cmbPsType.InsertString(1,L"비지속");

	m_cmbPumpType.InsertString(0,L"휀입력");
	m_cmbPumpType.InsertString(1,L"펌프입력");
	m_cmbPumpType.InsertString(2,L"한전전원");
	m_cmbPumpType.InsertString(3,L"비상발전기");
#else
	m_cmbPsType.InsertString(0,L"Keep");
	m_cmbPsType.InsertString(1,L"NoKeep");

	m_cmbPumpType.InsertString(0,L"FAN");
	m_cmbPumpType.InsertString(1,L"PUMP");
	m_cmbPumpType.InsertString(2,L"Alw.Pwr");
	m_cmbPumpType.InsertString(3,L"Generator");
#endif
//	MakeFacpButton();
	ResizeControl();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgPumpEditPane::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgPumpEditPane::OnSize(UINT nType,int cx,int cy)
{
	CDialogEx::OnSize(nType,cx,cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ResizeControl();
}


void CDlgPumpEditPane::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


void CDlgPumpEditPane::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void CDlgPumpEditPane::OnMouseMove(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_bPumpTypeDragging && m_pPumpTypeDragImage)
	{
		CRect rcScreen;
		CPoint ptScreen;
		ptScreen = point;
		ClientToScreen(&ptScreen);
		m_pPumpTypeDragImage->DragMove(ptScreen);
		m_pCustomListCtrl->GetWindowRect(&rcScreen);
		if(ptScreen.y <= rcScreen.top)
			m_pCustomListCtrl->ScrollUpDownList(TRUE,TRUE);
		else if(rcScreen.top < ptScreen.y && ptScreen.y < rcScreen.top + 20)
			m_pCustomListCtrl->ScrollUpDownList(TRUE,FALSE);
		else if(rcScreen.bottom - 20 < ptScreen.y && ptScreen.y < rcScreen.bottom)
			m_pCustomListCtrl->ScrollUpDownList(FALSE,FALSE);
		else if(rcScreen.bottom <= ptScreen.y)
			m_pCustomListCtrl->ScrollUpDownList(FALSE,TRUE);
	}
	CDialogEx::OnMouseMove(nFlags,point);
}

void CDlgPumpEditPane::OnLButtonUp(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ReleaseCapture();
	if(m_bPumpTypeDragging)
	{
		m_bPumpTypeDragging = FALSE;
		if(m_pPumpTypeDragImage == nullptr)
		{
			CDialogEx::OnLButtonUp(nFlags,point);
			return;
		}
		CPoint ptScreen;

		int nHit,nIdx;
		ptScreen = point;
		ClientToScreen(&ptScreen);
		m_pPumpTypeDragImage->DragLeave(GetDesktopWindow());
		m_pPumpTypeDragImage->EndDrag();
		delete m_pPumpTypeDragImage;
		m_pPumpTypeDragImage = nullptr;
		nHit = m_pCustomListCtrl->IsScrollArea(ptScreen);
		if(nHit != HT_IN_ITEM)
		{
			CDialogEx::OnLButtonUp(nFlags,point);
			return;
		}
		nIdx = m_pCustomListCtrl->PointToIndex(ptScreen);
		if(nIdx < 0)
		{
			CDialogEx::OnLButtonUp(nFlags,point);
			return;
		}
		if(m_pCurFacpData == nullptr)
		{
			AfxMessageBox(L"수신기가 설정되지 않았습니다.");
			CDialogEx::OnLButtonUp(nFlags,point);
			return;
		}

		CString strMsg;
		CPumpItem * pItem = nullptr;
		ST_PUMPTYPE * pTypeData = nullptr;
		POSITION pos;
		int nListIdx = -1;
		int i = 0 , nStartIndex = 0 ,nSelCnt = 0 ;
		BOOL bOverWrite = FALSE;
		LRESULT lRet = 0;

		nSelCnt = m_ctrlPumpTypeList.GetSelectedCount();
		if(nSelCnt == 0)
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(L"선택된 펌프 타입이 없습니다.");
#else
			AfxMessageBox(L"No pump type selected.");
#endif
			CDialogEx::OnLButtonUp(nFlags,point);
			return;
		}
		nStartIndex = nIdx;
		if(nSelCnt > 1)
		{
			CDataPump * pPmp = nullptr;
			CDataPS * pPs = nullptr;
			nStartIndex = nIdx;

#ifndef ENGLISH_MODE
			if(AfxMessageBox(L"기존에 이미 할당된 펌프가 있으면 덮어씌우시겠습니까?",MB_YESNO | MB_ICONQUESTION) == IDYES)
				bOverWrite = TRUE;
			else
				bOverWrite = FALSE;
#else
			if(AfxMessageBox(L"If there is an existing pump assigned, would you like to overwrite it?",MB_YESNO | MB_ICONQUESTION) == IDYES)
				bOverWrite = TRUE;
			else
				bOverWrite = FALSE;
#endif
			pos = m_ctrlPumpTypeList.GetFirstSelectedItemPosition();
			while(pos)
			{
				nListIdx = m_ctrlPumpTypeList.GetNextSelectedItem(pos);
				if(nListIdx < 0)
					continue;
				pTypeData = (ST_PUMPTYPE*)m_ctrlPumpTypeList.GetItemData(nListIdx);
				if(pTypeData == nullptr)
					continue;

				for(i = nIdx; i < D_MAX_PUMP_COUNT; i ++)
				{
					pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(i);
					if(pItem == nullptr)
						continue;

					// 이미 할당된 아이템에 대해 덮어씌울건지 확인
					if(pItem->GetAssign() == PUMP_ASSIGN)
					{
						if(bOverWrite == FALSE)
							continue;
						else
						{
							lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_DEL,nIdx),(DWORD_PTR)pItem);
							if(lRet != 0)
							{
#ifndef ENGLISH_MODE
								strMsg.Format(L"대상이 되는 펌프 %s를 삭제하는데 실패 했습니다.",pItem->GetPumpFullName());
#else
								strMsg.Format(L"Failed to delete target pump %s.",pItem->GetPumpFullName());
#endif
								AfxMessageBox(strMsg);
								CDialogEx::OnLButtonUp(nFlags,point);
								return;
							}
						}
					}

					// 이미 할당된 항목에 Overwrite 허용이면 기존 항목 삭제하고 for Loop를 빠져나온다.
					// Overwrite 허용하지 않으면 다음 번호로 이동
					break;
				}
				if(pItem == nullptr || i == D_MAX_PUMP_COUNT)
					continue;

				pPmp = new CDataPump;
				pPs = nullptr;
				pPmp->SetPumpData(m_pCurFacpData->GetFacpID(),i,pTypeData->nPumpType
					,pTypeData->strTypeName,pTypeData->strLcdMsg,L"",pTypeData->strTypeName,pTypeData->nPsUse);
				if(pTypeData->nPsUse == PUMP_PS_USE)
				{
					pPs = new CDataPS;
					pPs->SetPSwitchData(m_pCurFacpData->GetFacpID(),i,pTypeData->nPsType
						,pTypeData->strTypeName,pTypeData->strLcdMsg,L"",pTypeData->strTypeName,1
					);
				}
				pItem->SetPumpItemData(pPmp,pPs);

				lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_ADD,0),(DWORD_PTR)pItem);
				if(lRet != 0)
				{
#ifndef ENGLISH_MODE
					AfxMessageBox(L"펌프를 추가하는데 실패 했습니다.");
#else
					AfxMessageBox(L"Failed to add pump.");
#endif
					pItem->InitPumpData();
					delete pPmp;
					pPmp = nullptr;
					return;
				}
				nIdx = i;
				nIdx ++; // customlist에서 인덱스 증가
			}
		}
		else
		{
			CPumpItem * pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(nIdx);
			if(pItem == nullptr)
			{
			 	CDialogEx::OnLButtonUp(nFlags,point);
			 	return;
			}
			if(pItem->GetAssign() == PUMP_ASSIGN)
			{
			 	// 기존 데이터 데이터 삭제
			 	CString strMsg;
			 	LRESULT lRet = 1;
#ifndef ENGLISH_MODE
				strMsg.Format(L"대상이 되는 펌프가 이미 할당 되어 있습니다.\n대상 펌프를 삭제하고 새로 추가 하시겠습니까?\nNo를 선택 시 이 명령이 취소 됩니다.");
#else
				strMsg.Format(L"The target pump is already assigned.\nDo you want to delete the target pump and add a new one?\nSelecting No will cancel this command.");
#endif
				if(AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_DEL,nIdx),(DWORD_PTR)pItem);
					if(lRet != 0)
					{
#ifndef ENGLISH_MODE
						strMsg.Format(L"대상이 되는 펌프 %s를 삭제하는데 실패 했습니다.",pItem->GetPumpFullName());
#else
						strMsg.Format(L"Failed to delete target pump %s.",pItem->GetPumpFullName());
#endif
						AfxMessageBox(strMsg);
						return;
					}
				}
				else
					return; 
			}
			
			CDataPump * pPmp = new CDataPump;
			CDataPS * pPs = nullptr;
			ST_PUMPTYPE * pData = (ST_PUMPTYPE*)m_ctrlPumpTypeList.GetItemData(m_nPumpTypeDragItem);
			if(pData == nullptr)
			{
			 	CDialogEx::OnLButtonUp(nFlags,point);
			 	return;
			}
		
			LRESULT lRet = 0; 

			pPmp->SetPumpData(m_pCurFacpData->GetFacpID(),pItem->GetItemID(),pData->nPumpType
			 	,pData->strTypeName,pData->strLcdMsg,L"",pData->strTypeName,pData->nPsUse);
			
			if(pData->nPsUse == PUMP_PS_USE)
			{
				pPs = new CDataPS;
				pPs->SetPSwitchData(m_pCurFacpData->GetFacpID(),pItem->GetItemID(),pData->nPsType
					,pData->strTypeName,pData->strLcdMsg,L"",pData->strTypeName,1
				);
			}

			pItem->SetPumpItemData(pPmp,pPs);
			 
			// [2025/9/30 9:34:46 KHS] 
			// Database 입력
			lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_ADD,0),(DWORD_PTR)pItem);
			if(lRet != 0)
			{
#ifndef ENGLISH_MODE
				AfxMessageBox(L"펌프를 추가하는데 실패 했습니다.");
#else
				AfxMessageBox(L"Failed to add pump.");
#endif
				pItem->InitPumpData();
			 	delete pPmp;
			 	pPmp = nullptr;
			 	return; 
			}
		}
		
		m_pCustomListCtrl->SetCurSelIndex(nStartIndex);
	}
	CDialogEx::OnLButtonUp(nFlags,point);
}



// void CDlgPumpEditPane::OnLButtonUp(UINT nFlags,CPoint point)
// {
// 	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
// 	ReleaseCapture();
// 	if(m_bPumpTypeDragging)
// 	{
// 		m_bPumpTypeDragging = FALSE;
// 		if(m_pPumpTypeDragImage == nullptr)
// 		{
// 			CDialogEx::OnLButtonUp(nFlags,point);
// 			return;
// 		}
// 		CPoint ptScreen;
// 
// 		int nHit,nIdx;
// 		ptScreen = point;
// 		ClientToScreen(&ptScreen);
// 		m_pPumpTypeDragImage->DragLeave(GetDesktopWindow());
// 		m_pPumpTypeDragImage->EndDrag();
// 		delete m_pPumpTypeDragImage;
// 		m_pPumpTypeDragImage = nullptr;
// 		nHit = m_pCustomListCtrl->IsScrollArea(ptScreen);
// 		if(nHit != HT_IN_ITEM)
// 		{
// 			CDialogEx::OnLButtonUp(nFlags,point);
// 			return;
// 		}
// 		nIdx = m_pCustomListCtrl->PointToIndex(ptScreen);
// 		if(nIdx < 0)
// 		{
// 			CDialogEx::OnLButtonUp(nFlags,point);
// 			return;
// 		}
// 
// 		CPumpItem * pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(nIdx);
// 		if(pItem == nullptr)
// 		{
// 			CDialogEx::OnLButtonUp(nFlags,point);
// 			return;
// 		}
// 		if(pItem->GetAssign() == PUMP_ASSIGN)
// 		{
// 			// 기존 데이터 데이터 삭제
// 			CString strMsg;
// 			LRESULT lRet = 1;
// 			strMsg.Format(L"대상이 되는 펌프가 이미 할당 되어 있습니다.\n대상 펌프를 삭제하시겠습니까?");
// 			if(AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) == IDYES)
// 			{
// 				lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_DEL,nIdx),(DWORD_PTR)pItem);
// 				if(lRet != 0)
// 				{
// 					strMsg.Format(L"대상이 되는 펌프 %s를 삭제하는데 실패 했습니다.",pItem->GetPumpFullName());
// 					AfxMessageBox(strMsg);
// 					return;
// 				}
// 			}
// 		}
// 
// 		ST_PUMPTYPE * pData = (ST_PUMPTYPE*)m_ctrlPumpTypeList.GetItemData(m_nPumpTypeDragItem);
// 		if(pData == nullptr)
// 		{
// 			CDialogEx::OnLButtonUp(nFlags,point);
// 			return;
// 		}
// 
// 		if(m_pCurFacpData == nullptr)
// 		{
// 			AfxMessageBox(L"수신기가 설정되지 않았습니다.");
// 			CDialogEx::OnLButtonUp(nFlags,point);
// 			return;
// 		}
// 
// 		LRESULT lRet = 0; 
// 		CDataPump * pPmp = new CDataPump;
// 		pPmp->SetPumpData(m_pCurFacpData->GetFacpID(),pItem->GetItemID(),pData->nPumpType
// 			,pData->strTypeName,pData->strLcdMsg,L"",pData->strTypeName);
// 		pItem->SetPumpItemData(pPmp,nullptr);
// 
// 		// [2025/9/30 9:34:46 KHS] 
// 		// Database 입력
// 		lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_ADD,0),(DWORD_PTR)pItem);
// 		if(lRet != 0)
// 		{
// 			AfxMessageBox(L"펌프를 추가하는데 실패 했습니다.");
// 			pItem->InitPumpData();
// 			delete pPmp;
// 			pPmp = nullptr;
// 			return; 
// 		}
// 		m_pCustomListCtrl->SetCurSelIndex(nIdx);
// 	}
// 	CDialogEx::OnLButtonUp(nFlags,point);
// }


void CDlgPumpEditPane::OnLvnBegindragPumptypeList(NMHDR *pNMHDR,LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	m_bPumpTypeDragging = TRUE;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_nPumpTypeDragItem = pNMListView->iItem;
	//m_pDragImage = CreateDragImageEx(&pt);
	m_pPumpTypeDragImage = m_ctrlPumpTypeList.CreateDragImageWithText(m_nPumpTypeDragItem);
	if(m_pPumpTypeDragImage)
	{
		CPoint pt(pNMListView->ptAction);
		ClientToScreen(&pt);
		m_pPumpTypeDragImage->BeginDrag(0,CPoint(0,0));
		m_pPumpTypeDragImage->DragEnter(GetDesktopWindow(),pt);
		SetCapture();
		m_bPumpTypeDragging = TRUE;
	}
}

BOOL CDlgPumpEditPane::CreateDlg(CWnd * pParent)
{
	return Create(IDD_PUMPEDIT_PANE,pParent);
}

int CDlgPumpEditPane::ResizeControl()
{
	CRect rcWnd,rcbtn,rc;
	int nFacpArea = 50;
	int nTypeWidth = 320;
	int nItemHeight = 30;
	int nBtnWidth = 80;
	GetClientRect(&rcWnd);
	nFacpArea = ResizeFacpArea(rcWnd.Width(),rcWnd.Height());

	if(m_pCustomListCtrl && m_pCustomListCtrl->GetSafeHwnd())
	{
		m_pCustomListCtrl->MoveWindow(0,nFacpArea,rcWnd.Width() - nTypeWidth - 0,rcWnd.Height() - nFacpArea - 0);
	}

	if(GetDlgItem(IDC_CMB_PUMPTEMPLETE)->GetSafeHwnd())
	{
		GetDlgItem(IDC_CMB_PUMPTEMPLETE)->MoveWindow(rcWnd.Width() - nTypeWidth + 5,nFacpArea + 4,nTypeWidth - 10 - nBtnWidth - 4 ,nItemHeight);
	}

	if(GetDlgItem(IDC_BTN_ADDTEMPLETE)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_ADDTEMPLETE)->MoveWindow(rcWnd.Width() - nBtnWidth,nFacpArea,nBtnWidth - 4,nItemHeight);
	}

	if(m_ctrlPumpTypeList.GetSafeHwnd())
	{
		m_ctrlPumpTypeList.MoveWindow(rcWnd.Width() - nTypeWidth + 5,nFacpArea + nItemHeight ,nTypeWidth - 10,rcWnd.Height() - nFacpArea - nItemHeight - 0 - 200);
	}


	ResizeAddArea(rcWnd.Width(),rcWnd.Height());


	return 0;
}


int CDlgPumpEditPane::ResizeFacpArea(int nCx,int nCy)
{
	int nStartTop , nStartLeft , nPaneCx;
	int nItemWidth,nItemHeight;
	nItemWidth = 80;
	nItemHeight = 30;
	nStartTop = 2;
	nStartLeft = 2;
	nPaneCx = nCx - nItemWidth * 2;
	int nCntPerLine , i , nRow,nCol , nTopPos,nLeftPos , nIdx; 
	nIdx = 0; 
	nRow = nCol = nTopPos = nLeftPos = 0;
	nCntPerLine = (int)((float)(nPaneCx - 4) / (float)nItemWidth) + 1;
	for(i = 0; i < D_MAX_FACP_COUNT; i ++)
	{
		if(m_arrFacpBtn[i].GetFacpData() == nullptr)
			continue; 
		nRow = nIdx / nCntPerLine;
		nCol = nIdx % nCntPerLine;
		nTopPos = nStartTop + (nItemHeight * nRow);
		nLeftPos = nStartLeft + (nItemWidth * nCol);
		m_arrFacpBtn[i].MoveWindow(nLeftPos,nTopPos,nItemWidth,nItemHeight);
		nIdx ++;
	}

	if(GetDlgItem(IDC_BTN_INITFACP)->GetSafeHwnd())
		GetDlgItem(IDC_BTN_INITFACP)->MoveWindow(nPaneCx + nItemWidth ,nStartTop , nItemWidth - 4,nItemHeight);

	return nTopPos + nItemHeight;
}

int CDlgPumpEditPane::ResizeAddArea(int nCx , int nCy)
{
	int nLeftPos,nTopPos,nStWidth,nItemWidth,nHeight;
	nStWidth = 80;
	nItemWidth = 200;
	nHeight = 30;
	nLeftPos = nCx - (nStWidth + nItemWidth) - 20;

	if(GetDlgItem(IDC_ST_TYPENAME)->GetSafeHwnd() != nullptr)
	{
		nTopPos = nCy - 20 - nHeight * 5;
		GetDlgItem(IDC_ST_TYPENAME)->MoveWindow(nLeftPos,nTopPos,nStWidth,nHeight);
	}

	if(GetDlgItem(IDC_ST_PSTYPE)->GetSafeHwnd() != nullptr)
	{
		nTopPos = nCy - 20 - nHeight * 4;
		GetDlgItem(IDC_ST_PSTYPE)->MoveWindow(nLeftPos,nTopPos,nStWidth,nHeight);
	}

	if(GetDlgItem(IDC_ST_PUMPTYPE)->GetSafeHwnd() != nullptr)
	{
		nTopPos = nCy - 20 - nHeight * 3 ;
		GetDlgItem(IDC_ST_PUMPTYPE)->MoveWindow(nLeftPos,nTopPos,nStWidth,nHeight);
	}

	if(GetDlgItem(IDC_ST_LCDMSG)->GetSafeHwnd() != nullptr)
	{
		nTopPos = nCy - 20 - nHeight * 2;
		GetDlgItem(IDC_ST_LCDMSG)->MoveWindow(nLeftPos,nTopPos,nStWidth,nHeight);
	}

	// Edit control
	nLeftPos = nCx - (nItemWidth) - 4;
	if(GetDlgItem(IDC_ED_TYPENAME)->GetSafeHwnd() != nullptr)
	{
		nTopPos = nCy - 20 - nHeight * 5;
		GetDlgItem(IDC_ED_TYPENAME)->MoveWindow(nLeftPos,nTopPos,nItemWidth,nHeight - 4);
	}

	if(GetDlgItem(IDC_CMB_PSTYPE)->GetSafeHwnd() != nullptr)
	{
		nTopPos = nCy - 20 - nHeight * 4 ;
		GetDlgItem(IDC_CMB_PSTYPE)->MoveWindow(nLeftPos,nTopPos,nItemWidth,nHeight);
	}

	if(GetDlgItem(IDC_CMB_PUMPTYPE)->GetSafeHwnd() != nullptr)
	{
		nTopPos = nCy - 20 - nHeight * 3 ;
		GetDlgItem(IDC_CMB_PUMPTYPE)->MoveWindow(nLeftPos,nTopPos,nItemWidth,nHeight);
	}

	if(GetDlgItem(IDC_ED_LCDMSG)->GetSafeHwnd() != nullptr)
	{
		nTopPos = nCy - 20 - nHeight * 2;
		GetDlgItem(IDC_ED_LCDMSG)->MoveWindow(nLeftPos,nTopPos,nItemWidth,nHeight - 4);
	}


	nTopPos = nCy - 20 - nHeight * 1;
	if(GetDlgItem(IDC_BTN_ADD)->GetSafeHwnd() != nullptr)
	{
		nLeftPos = nCx - 10 - 100 * 3;
		GetDlgItem(IDC_BTN_ADD)->MoveWindow(nLeftPos,nTopPos,100,nHeight);
	}

	if(GetDlgItem(IDC_BTN_MODIFY)->GetSafeHwnd() != nullptr)
	{
		nLeftPos = nCx - 10 - 100 * 2;
		GetDlgItem(IDC_BTN_MODIFY)->MoveWindow(nLeftPos,nTopPos,100,nHeight);
	}

	if(GetDlgItem(IDC_BTN_DEL)->GetSafeHwnd() != nullptr)
	{
		nLeftPos = nCx - 10 - 100 * 1;
		GetDlgItem(IDC_BTN_DEL)->MoveWindow(nLeftPos,nTopPos,100,nHeight);
	}

	return 0;
}

int CDlgPumpEditPane::InitPumpTypeList()
{
	if(m_pPtrRefPumpTypeList == nullptr)
		return 0; 
	int nIdx = 0;
	ST_PUMPTYPE * pData;
	POSITION pos;
	CString strtemp;
	m_ctrlPumpTypeList.DeleteAllItems();
	pos = m_pPtrRefPumpTypeList->GetHeadPosition();
	while(pos)
	{
		pData = (ST_PUMPTYPE *)m_pPtrRefPumpTypeList->GetNext(pos);
		if(pData == nullptr)
			continue; 
		strtemp = pData->strTypeName;
		strtemp.TrimRight();
		strtemp.TrimLeft();
		if(strtemp.GetLength() <= 0)
			continue; 

		m_ctrlPumpTypeList.InsertItem(nIdx,strtemp);
		m_ctrlPumpTypeList.SetItemImage(nIdx,0,0);
		m_ctrlPumpTypeList.SetItemData(nIdx,(DWORD_PTR)pData);
		nIdx ++;
	}
	//m_ctrlPumpTypeList.Sort(0,TRUE);
	return 0;
}


int CDlgPumpEditPane::InitPumpTemplet()
{
	if(m_pPtrRefPumpTemplete == nullptr)
		return 0;
	int nIdx = 0;
	ST_PUMPTEMPLETE * pData;
	POSITION pos;
	CString strtemp;
	m_cmbPumpTemplete.ResetContent();
	pos = m_pPtrRefPumpTemplete->GetHeadPosition();
	while(pos)
	{
		pData = (ST_PUMPTEMPLETE *)m_pPtrRefPumpTemplete->GetNext(pos);
		if(pData == nullptr)
			continue;
		strtemp = pData->strTempleteName;
		strtemp.TrimRight();
		strtemp.TrimLeft();
		if(strtemp.GetLength() <= 0)
			continue;

		nIdx = m_cmbPumpTemplete.AddString(strtemp);
		if(nIdx < 0)
			continue; 
		m_cmbPumpTemplete.SetItemData(nIdx,(LPARAM)pData);
	}
	if(nIdx >= 0)
		m_cmbPumpTemplete.SetCurSel(0);
	return 0;
}

void CDlgPumpEditPane::SetPumpTypeList(CPumpTypeList * ptrTypeList)
{
	m_pPtrRefPumpTypeList = ptrTypeList;
	InitPumpTypeList();
}

void CDlgPumpEditPane::SetPumpTemplete(CPtrList * pPtrTemplete)
{
	m_pPtrRefPumpTemplete = pPtrTemplete;
	InitPumpTemplet();
}

int CDlgPumpEditPane::InitDlg()
{
	if(AfxGetMainWnd())
		m_pRefFasSysData = theApp.GetRelayTableData();
	MakeFacpButton();

// 	if(m_nPaneType == 1)
// 		FillPSwitchTree();
// 	else
// 		FillPumpTree();
	return 1;
}

void CDlgPumpEditPane::OnClcnItemUp(NMHDR *pNMHDR,LRESULT *pResult)
{
	// Pump Selected

	CLNOTIFY_INFO * pClData = (CLNOTIFY_INFO*)pNMHDR;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if(pClData == nullptr || pClData->dwItemPtr == 0)
		return;

	CPumpItem * pItem = (CPumpItem*)pClData->dwItemPtr;
	CString strLcd = pItem->GetPumpLcdMsg();

	CString strMsg;
	strMsg.Format(L"%s - %d",strLcd,pClData->nIndex);
	//AfxMessageBox(strMsg);

}

int CDlgPumpEditPane::MakeFacpButton()
{
	InitFacpBtn();
	std::map<int,CDataFacp *> * pMapFacp;
	std::map<int,CDataFacp*>::iterator it;
	CDataFacp * pFacp;
	int i = 0; 
	if(m_pRefFasSysData == nullptr)
		return 0; 

	pMapFacp = m_pRefFasSysData->GetFacpMap();
	pMapFacp = m_pRefFasSysData->GetFacpMap();
	if(pMapFacp == nullptr)
		return 0;
	for(it = pMapFacp->begin(); it != pMapFacp->end(); it++)
	{
		pFacp = it->second;
		if(pFacp == nullptr)
			continue;
		m_arrFacpBtn[i].SetWindowTextW(pFacp->GetFacpName());
		m_arrFacpBtn[i].MoveWindow(10 + 100 * i,5,100,40);
		m_arrFacpBtn[i].SetFacpData(pFacp);
		m_arrFacpBtn[i].ShowWindow(SW_SHOW);
		i++;
	}
	m_arrFacpBtn[0].SetCheck(1);
	OnRadioFacpClicked(IDC_RD_FACP0);
	return 0;
}

ST_PUMPTYPE * CDlgPumpEditPane::SetTypeNPrefix(CDataPump * pPmp,CDataPS * pPs)
{
	CString strPmpName,strUpper , strPrefix;
	int nFind;
	POSITION pos;
	ST_PUMPTYPE * pData;
	// 내림차순으로 정렬되어 있기 때문에 뒤에서 부터 검색하면 긴 이름 먼저 체크한다.
	// ex) "한전감시-1"이 "한전감시"보다 먼저 나온다
	// SP 주펌프 , 소화전 주펌프
	pos = m_pPtrRefPumpTypeList->GetTailPosition();
	while(pos)
	{
		pData = (ST_PUMPTYPE *)m_pPtrRefPumpTypeList->GetPrev(pos);
		if(pData == nullptr)
			continue; 
		strPmpName = pPmp->GetPumpName();
		strUpper = pData->strTypeName;
		nFind = strPmpName.Find(strUpper);
		if(nFind < 0)
			continue; 
		strPrefix = strPmpName.Mid(0,nFind - 1);
		strPrefix.TrimLeft();
		strPrefix.TrimRight();

		pPmp->SetTypeName(pData->strTypeName);
		pPmp->SetPrefix(strPrefix);
		if(pPs != nullptr)
		{
			pPs->SetTypeName(pData->strTypeName);
			pPs->SetPrefix(strPrefix);
		}
		return pData;
	}
	return nullptr;
}

void CDlgPumpEditPane::OnRadioFacpClicked(UINT nID)
{
	CString strName;
	CPumpItem * pItem;
	CDataFacp * pFacp = nullptr;
	CDataPump * pPmp;
	CDataPS		* pPs;
	POSITION pos;
	if(m_pCustomListCtrl->IsItemModified())
	{
#ifndef ENGLISH_MODE
		if(AfxMessageBox(L"변경된 펌프데이터가 있습니다.\n저장 하시겠습니까?",MB_YESNO|MB_ICONQUESTION) == IDYES)
#else
		if(AfxMessageBox(L"There is changed pump data.\nDo you want to save it?",MB_YESNO | MB_ICONQUESTION) == IDYES)
#endif
		{
			int i = 0;
			CPumpItem * pItem;
			LRESULT lRet = 1;
			for(i = 0; i < D_MAX_PUMP_COUNT; i ++)
			{
				pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(i);
				if(pItem == nullptr)
					continue; 
				if(pItem->GetAssign() != PUMP_ASSIGN)
					continue; 

				if(pItem->IsModifiedFlag() == FALSE)
					continue; 

				lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE
					,MAKELPARAM(DATA_SAVE,0),(LPARAM)pItem);
				if(lRet != 0)
				{
					// 변경 실패
#ifndef ENGLISH_MODE
					AfxMessageBox(L"변경된 펌프 목록을 저장 중 오류가 발생했습니다.\n프로그램을 반드시 재시작 하시길 바랍니다.");
#else
					AfxMessageBox(L"An error occurred while saving the changed pump list.\nYou must restart the program.");
#endif
					return;
				}
				pItem->SetModifiedFlag(FALSE);
			}
		}
		else
		{

		}
	}

	m_pCustomListCtrl->InitAllItemData();

	pFacp = m_arrFacpBtn[nID - IDC_RD_FACP0].GetFacpData();
	if(pFacp == nullptr)
		return;

	m_pCurFacpData = pFacp;
	std::shared_ptr<CManagerPump> spPump;
	std::shared_ptr<CManagerPS> spPs;
	spPump = m_pRefFasSysData->GetPumpManager();
	if(spPump == nullptr)
		return;
	spPs = m_pRefFasSysData->GetPSwitchManager();
	if(spPs == nullptr)
		return;

	pos = spPump->GetHeadPosition();
	while(pos)
	{
		pPmp = spPump->GetNext(pos);
		if(pPmp == nullptr)
			continue;
		if(pPmp->GetFacpID() != pFacp->GetFacpID())
			continue; 
		// Pump ID는 1부터 시작하고 , GetItem은 0 Base
		pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(pPmp->GetPumpID() -1);

		// pItem은 m_pCustomListCtrl생성 할 때 이미 생성
		// 단지 데이터 할당을 했는지 안했는지 문제
		// pItem == nullptr일때 펌프 아이디가 맞는지 확인
		if(pItem == nullptr)
		{
// 			if(pPmp->GetPumpID() <= 0 || pPmp->GetPumpID() >= D_MAX_PUMP_COUNT)
// 			{
// 
// 			}
// 			else
// 			{
// 
// 			}
			continue; 
		}

		pPs = spPs->GetPSData(pPmp->GetFacpID() , pPmp->GetPumpID());
		if(pPmp->GetTypeName() == L"")
		{
			SetTypeNPrefix(pPmp,pPs);
		}
		// Pump Type 찾기 - 이전 프로젝트에서는 pump type이 없기 때문에 csv파일 읽어올때 
		pItem->SetPumpItemData(pPmp,pPs);
		pItem->SetSelected(FALSE);
		//m_pCustomListCtrl->SetItemData(pPmp->GetPumpID(),(LPARAM)pPmp);
	}


	m_pCustomListCtrl->SetCurSelIndex(0);
}

int CDlgPumpEditPane::InitFacpBtn()
{
	for(int i = 0; i < D_MAX_FACP_COUNT; i ++)
	{
		m_arrFacpBtn[i].ShowWindow(SW_HIDE);
		m_arrFacpBtn[i].SetFacpData(nullptr);
		m_arrFacpBtn[i].SetCheck(0);
	}
	return 0;
}


void CDlgPumpEditPane::OnClcnItemEndDrag(NMHDR *pNMHDR,LRESULT *pResult)
{
	CLNOTIFY_DRAGINFO * pClData = (CLNOTIFY_DRAGINFO*)pNMHDR;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if(pClData == nullptr || pClData->dwDragItemPtr == 0 || pClData->dwTargetItemPtr == 0)
		return;

	if(m_pMainForm == nullptr)
		return; 

	CString strMsg;
	LRESULT lRet = 0; 
	CPumpItem * pDragItem,* pTargetItem;

	pDragItem = (CPumpItem*)pClData->dwDragItemPtr;
	pTargetItem = (CPumpItem*)pClData->dwTargetItemPtr;

	if(pTargetItem->GetAssign() == TRUE)
	{
		// 기존 데이터 데이터 삭제
#ifndef ENGLISH_MODE
		strMsg.Format(L"대상이 되는 펌프가 이미 할당 되어 있습니다.\n대상 펌프를 삭제하고 새로 추가 하시겠습니까?\nNo를 선택 시 이 명령이 취소 됩니다.");
#else
		strMsg.Format(L"The target pump is already assigned.\nDo you want to delete the target pump and add a new one?\nIf you select No, this command will be canceled.");
#endif
		if(AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_DEL,pClData->nTargetIndex),(DWORD_PTR)pTargetItem);
			if(lRet != 0)
			{
#ifndef ENGLISH_MODE
				strMsg.Format(L"대상이 되는 펌프 %s를 삭제하는데 실패 했습니다.",pTargetItem->GetPumpFullName());
#else
				strMsg.Format(L"Failed to delete target pump %s.",pTargetItem->GetPumpFullName());
#endif
				AfxMessageBox(strMsg);
				*pResult = 1;
				return; 
			}
		}
		else
			return;
	}
	CString strLcd = pDragItem->GetPumpLcdMsg();

	// [2025/10/16 8:43:00 KHS] 
	// Drag Index는 Vector에서
	strMsg.Format(L"%s - %d",strLcd,pClData->nDragIndex);
	//AfxMessageBox(strMsg);

	lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_MOVE,pClData->nTargetIndex),(DWORD_PTR)pDragItem);
	if(lRet != 0)
	{
#ifndef ENGLISH_MODE
		strMsg.Format(L"펌프 %s를 추가하는데 실패 했습니다.",pDragItem->GetPumpFullName());
#else
		strMsg.Format(L"Failed to add pump %s.",pDragItem->GetPumpFullName());
#endif
		AfxMessageBox(strMsg);
		*pResult = 1;
		return;
	}
	pTargetItem->SetPumpItemData(pDragItem->GetDataPump() , pDragItem->GetDataPS());
	pDragItem->SetPumpItemData(nullptr,nullptr);
	m_pCustomListCtrl->SetCurSelIndex(pClData->nTargetIndex);
}

void CDlgPumpEditPane::OnClcnItemDelete(NMHDR *pNMHDR,LRESULT *pResult)
{
	CLNOTIFY_INFO * pClData = (CLNOTIFY_INFO*)pNMHDR;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 1;
	LRESULT lRet = 1;
	if(pClData == nullptr || pClData->dwItemPtr == 0)
		return;

	CPumpItem * pItem = (CPumpItem*)pClData->dwItemPtr;
	CString strLcd = pItem->GetPumpLcdMsg();
	CString strMsg;
#ifndef ENGLISH_MODE
	strMsg.Format(L"선택된 %s를 삭제하시겠습니까?",pItem->GetPumpFullName());
#else
	strMsg.Format(L"Do you want to delete the selected [%s]?",m_pCurrentData->GetPumpName());
#endif
	if(AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
	if(m_pMainForm)
	{
		lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_DEL,0),(DWORD_PTR)pItem);

	}
	*pResult = lRet;
}

void CDlgPumpEditPane::OnClcnItemSelChanged(NMHDR *pNMHDR,LRESULT *pResult)
{
	CLNOTIFY_SELCHANGEINFO * pClData = (CLNOTIFY_SELCHANGEINFO*)pNMHDR;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	if(pClData == nullptr || pClData->dwSelItemPtr == 0 || m_pMainForm== nullptr)
		return;
	CPumpItem * pOldItem = (CPumpItem *)pClData->dwOldItemPtr;
	CPumpItem * pItem = (CPumpItem*)pClData->dwSelItemPtr;
	CString strLcd = pItem->GetPumpLcdMsg();
	
	if(pOldItem != nullptr && pOldItem->IsModifiedFlag())
	{
		if(AfxMessageBox(L"이전 아이템이 변경됐습니다. 이전 아이템을 저장하시겠습니까?"
			,MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			pOldItem->UpdateOldData(FALSE);
			pOldItem->SetModifiedFlag(FALSE);
			m_pMainForm->SendMessage(UWM_CHILDPANE_SELECTCHANGE,(WPARAM)pOldItem,(LPARAM)pItem);
			return; 
		}
		LRESULT lRet = 0;
		lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_SAVE ,0),(LPARAM)pOldItem);
		if(lRet != 0)
		{
			// 변경 실패
			return; 
		}
		pOldItem->SetModifiedFlag(FALSE);
	}

	m_pMainForm->SendMessage(UWM_CHILDPANE_SELECTCHANGE,(WPARAM)pOldItem,(LPARAM)pItem);
}


void CDlgPumpEditPane::OnClcnItemEditEnd(NMHDR *pNMHDR,LRESULT *pResult)
{
	CLNOTIFY_INFO * pClData = (CLNOTIFY_INFO*)pNMHDR;
 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
 	*pResult = 0;
 	if(pClData == nullptr || pClData->dwItemPtr == 0 || m_pMainForm == nullptr)
 		return;
 	CPumpItem * pItem = (CPumpItem*)pClData->dwItemPtr;
 	CString strLcd = pItem->GetPumpLcdMsg();
	LRESULT lRet = 0;
	lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_SAVE,0),(LPARAM)pItem);
	if(lRet != 0)
	{
		// 변경 실패
		return;
	}
	pItem->SetModifiedFlag(FALSE);
}


void CDlgPumpEditPane::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 같은 이름의 tYPE NAME 찾기
	UpdateData();

	int nCnt; 
	int nPsType,nPumpType,nTemp;
	ST_PUMPTYPE * pData;
	POSITION pos;
	CString str1,str2,strError;
	str2 = m_strTypeName;
	str2.TrimLeft();
	str2.TrimRight();
	pos = m_pPtrRefPumpTypeList->GetHeadPosition();
	while(pos)
	{
		pData = (ST_PUMPTYPE *)m_pPtrRefPumpTypeList->GetNext(pos);
		if(pData == nullptr)
			continue; 
		str1 = pData->strTypeName;
		str1.TrimLeft();
		str1.TrimRight();

		

		if(str1.CompareNoCase(str2) == 0)
		{
#ifndef ENGLISH_MODE
			strError.Format(L"이미 '%s'(으)로 펌프 타입이 있습니다",m_strTypeName);
#else
			strError.Format(L"There is already a pump type of '%s'",m_strTypeName);
#endif
			AfxMessageBox(strError);
			return;
		}
	}

	nTemp = m_cmbPsType.GetCurSel();
	if(nTemp < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"PS Type이 선택되지 않았습니다.");
#else
		AfxMessageBox(L"PS Type is not selected.");
#endif
		return; 
	}
	nPsType = nTemp + 1;
	nTemp = m_cmbPumpType.GetCurSel();
	if(nTemp < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"Pump Type이 선택되지 않았습니다.");
#else
		AfxMessageBox(L"Pump Type not selected");
#endif	
		return;
	}
	nPumpType = nTemp + 1;
	pData = new ST_PUMPTYPE;
	pData->strTypeName = str2;
	str1 = m_strLcdMsg.TrimLeft();
	str1 = m_strLcdMsg.TrimRight();
	pData->strLcdMsg = str1;
	pData->nPsType = nPsType;
	pData->nPumpType = nPumpType;
	m_pPtrRefPumpTypeList->AddTail(pData);
	nCnt = m_ctrlPumpTypeList.GetItemCount();
	m_ctrlPumpTypeList.InsertItem(nCnt,pData->strTypeName);
	m_ctrlPumpTypeList.SetItemImage(nCnt,0,0);
	m_ctrlPumpTypeList.SetItemData(nCnt,(DWORD_PTR)pData);
#ifndef ENGLISH_MODE
	AfxMessageBox(L"Pump Type을 추가하는데 성공했습니다");
#else
	AfxMessageBox(L"Successfully added Pump Type");
#endif
}


void CDlgPumpEditPane::OnBnClickedBtnModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	BOOL bFind = FALSE;
	int nPsType,nPumpType,nTemp;
	ST_PUMPTYPE * pData;
	POSITION pos;
	CString str1,str2,strError;
	str2 = m_strTypeName;
	str2.TrimLeft();
	str2.TrimRight();
	pos = m_pPtrRefPumpTypeList->GetHeadPosition();
	while(pos)
	{
		pData = (ST_PUMPTYPE *)m_pPtrRefPumpTypeList->GetNext(pos);
		if(pData == nullptr)
			continue;
		str1 = pData->strTypeName;
		str1.TrimLeft();
		str1.TrimRight();

		if(str1.CompareNoCase(str2) == 0)
		{
// 			strError.Format(L"이미 '%s'(으)로 펌프 타입이 있습니다",m_strTypeName);
// 			AfxMessageBox(strError);
			bFind = TRUE;
			break;
		}
	}

	if(bFind == FALSE)
	{
#ifndef ENGLISH_MODE
		strError.Format(L"'%s'의 펌프 타입이 없습니다",m_strTypeName);
#else
		strError.Format(L"Pump type '%s' does not exist",m_strTypeName);
#endif
		AfxMessageBox(strError);
		return;
	}
	nTemp = m_cmbPsType.GetCurSel();
	if(nTemp < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"PS Type이 선택되지 않았습니다.");
#else
		AfxMessageBox(L"PS Type is not selected.");
#endif

		return;
	}
	nPsType = nTemp + 1;
	nTemp = m_cmbPumpType.GetCurSel();
	if(nTemp < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"Pump Type이 선택되지 않았습니다.");
#else
		AfxMessageBox(L"Pump Type not selected.");
#endif
		return;
	}
	nPumpType = nTemp + 1;
	pData->strTypeName = str2;
	str1 = m_strLcdMsg.TrimLeft();
	str1 = m_strLcdMsg.TrimRight();
	pData->strLcdMsg = str1;
	pData->nPsType = nPsType;
	pData->nPumpType = nPumpType;
#ifndef ENGLISH_MODE
	AfxMessageBox(L"Pump Type을 수정하는데 성공했습니다");
#else
	AfxMessageBox(L"Successfully modified the Pump Type");
#endif
}


void CDlgPumpEditPane::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ST_PUMPTYPE * pData = nullptr;
	CString strTypeName = L"";
	CString strError;
	int nIdx;
	nIdx = m_ctrlPumpTypeList.GetNextItem(-1,LVNI_SELECTED);
	if(nIdx < 0)
		return;

	pData = (ST_PUMPTYPE *)m_ctrlPumpTypeList.GetItemData(nIdx);
	if(pData == nullptr)
	{
		strTypeName = m_ctrlPumpTypeList.GetItemText(nIdx,0);
	}
	else
		strTypeName = pData->strTypeName;

#ifndef ENGLISH_MODE
	strError.Format(L"Pump Type %s(을)를 삭제하시겠습니까?",strTypeName);
#else
	strError.Format(L"Are you sure you want to delete Pump Type %s?",strTypeName);
#endif
	if(AfxMessageBox(strError,MB_YESNO | MB_ICONQUESTION) != IDYES)
		return; 

	m_ctrlPumpTypeList.DeleteItem(nIdx);
	if(pData != nullptr)
	{
		POSITION pos = m_pPtrRefPumpTypeList->Find(pData);
		if(pos)
			m_pPtrRefPumpTypeList->RemoveAt(pos);
		delete pData;
		pData = nullptr;
	}

#ifndef ENGLISH_MODE
	AfxMessageBox(L"삭제하는데 성공했습니다");
#else
	AfxMessageBox(L"Deletion was successful");
#endif
}


void CDlgPumpEditPane::OnCbnSelchangeCmbPstype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgPumpEditPane::OnCbnSelchangeCmbPumptype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgPumpEditPane::OnEnChangeEdLcdmsg()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgPumpEditPane::OnNMClickPumptypeList(NMHDR *pNMHDR,LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	ST_PUMPTYPE * pData = nullptr;
	int nIdx; 
	nIdx = m_ctrlPumpTypeList.GetNextItem(-1,LVNI_SELECTED);
	if(nIdx < 0)
		return; 

	pData = (ST_PUMPTYPE *)m_ctrlPumpTypeList.GetItemData(nIdx);
	DisplayType(pData);
}


int CDlgPumpEditPane::DisplayType(ST_PUMPTYPE* pType)
{
	if(pType == nullptr)
		return 0; 
	m_strTypeName = pType->strTypeName;
	m_strLcdMsg = pType->strLcdMsg;
	m_cmbPsType.SetCurSel(pType->nPsType - 1);
	m_cmbPumpType.SetCurSel(pType->nPumpType - 1);
	UpdateData(FALSE);
	return 0;
}


void CDlgPumpEditPane::OnBnClickedBtnInitfacp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_pCurFacpData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"선택된 수신기가 없습니다.");
#else
		AfxMessageBox(L"No receiver selected");
#endif
		return; 
	}

	CString strMsg;

#ifndef ENGLISH_MODE
	strMsg.Format(
		L"펌프를 출력으로 사용하는 모든 입력에서 펌프를 삭제됩니다.\n"
		L"삭제하시겠습니까?\n"
	);
#else
	strMsg.Format(
		L"Remove the pump from all inputs that use the pump as an output..\n"
		L"Would you like to delete it?\n"
	);
#endif
	if(AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return; 
	}

// 	for(i = 0; i < D_MAX_FACP_COUNT; i ++)
// 	{
// 		pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(i);
// 		if(pItem == nullptr)
// 			continue; 
// 
// 		if(pItem->GetAssign() != PUMP_ASSIGN)
// 			continue; 
// 
// 
// 
// 
// 	}

	LRESULT lRet = 0;
	lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_ALLDEL,0),(LPARAM)m_pCurFacpData->GetFacpID());
	if(lRet != 0)
	{
		// 변경 실패
		return;
	}

	m_pCustomListCtrl->InitAllItemData();
}


void CDlgPumpEditPane::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_pCustomListCtrl == nullptr || m_pCustomListCtrl->GetSafeHwnd() == nullptr)
		return; 
	SaveCustomControlChangeData();

	if(m_pCustomListCtrl != nullptr)
	{
		delete m_pCustomListCtrl;
		m_pCustomListCtrl = nullptr;
	}

// 
// 	if(m_pCustomListCtrl->IsItemModified() == FALSE)
// 		return; 


// 	if(AfxMessageBox(L"변경된 펌프데이터가 있습니다.\n저장 하시겠습니까?",MB_YESNO | MB_ICONQUESTION) != IDYES)
// 		return; 
// 
// 	int i = 0,nSize;
// 	CPumpItem * pItem;
// 	LRESULT lRet = 1;
// 	for(i = 0; i < D_MAX_PUMP_COUNT; i ++)
// 	{
// 		pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(i);
// 		if(pItem == nullptr)
// 			continue;
// 		if(pItem->GetAssign() != PUMP_ASSIGN)
// 			continue;
// 
// 		if(pItem->IsModifiedFlag() == FALSE)
// 			continue;
// 
// 		lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE
// 			,MAKELPARAM(DATA_SAVE,0),(LPARAM)pItem);
// 		if(lRet != 0)
// 		{
// 			// 변경 실패
// 			AfxMessageBox(L"변경된 펌프 목록을 저장 중 오류가 발생했습니다.\n프로그램을 반드시 재시작 하시길 바랍니다.");
// 			return;
// 		}
// 		pItem->SetModifiedFlag(FALSE);
// 	}
}

BOOL CDlgPumpEditPane::SaveCustomControlChangeData()
{
	if(m_pCustomListCtrl == nullptr)
		return FALSE;
	if(m_pCustomListCtrl->IsItemModified() == FALSE)
		return TRUE;

#ifndef ENGLISH_MODE
	if(AfxMessageBox(L"변경된 펌프데이터가 있습니다.\n저장 하시겠습니까?",MB_YESNO | MB_ICONQUESTION) != IDYES)
#else
	if(AfxMessageBox(L"There is changed pump data.\nDo you want to save it?",MB_YESNO | MB_ICONQUESTION) != IDYES)
#endif
		return TRUE;

	int i = 0;
	CPumpItem * pItem;
	LRESULT lRet = 1;
	for(i = 0; i < D_MAX_PUMP_COUNT; i ++)
	{
		pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(i);
		if(pItem == nullptr)
			continue;
		if(pItem->GetAssign() != PUMP_ASSIGN)
			continue;

		if(pItem->IsModifiedFlag() == FALSE)
			continue;

		lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE
			,MAKELPARAM(DATA_SAVE,0),(LPARAM)pItem);
		if(lRet != 0)
		{
			// 변경 실패
#ifndef ENGLISH_MODE
			AfxMessageBox(L"변경된 펌프 목록을 저장 중 오류가 발생했습니다.\n프로그램을 반드시 재시작 하시길 바랍니다.");
#else
			AfxMessageBox(L"An error occurred while saving the changed pump list.\nYou must restart the program..");
#endif
			return FALSE;
		}
		pItem->SetModifiedFlag(FALSE);
	}


	return TRUE;
}


void CDlgPumpEditPane::OnCbnSelchangeCmbPumptemplete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgPumpEditPane::OnBnClickedBtnAddtemplete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = -1 , i , x,nIdx , nStartIdx = -1;
	ST_PUMPTEMPLETE * pTemp;
	CPumpItem * pItem;
	CDataPump * pPmp;
	BOOL bOverWrite = FALSE;
	LRESULT lRet = 1;
	CString strMsg;
	nSel = m_cmbPumpTemplete.GetCurSel();
	if(nSel < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"선택된 Templete이 없습니다.");
#else
		AfxMessageBox(L"There is no Templete selected.");
#endif
		return;
	}
	pTemp = (ST_PUMPTEMPLETE *)m_cmbPumpTemplete.GetItemData(nSel);
	if(pTemp == nullptr)
		return;
	if(m_pCurFacpData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"수신기가 설정되지 않았습니다.");
#else
		AfxMessageBox(L"Receiver not set up.");
#endif
		return;
	}

	nIdx = m_pCustomListCtrl->GetCurSelIndex();
	if(nIdx < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"템플릿이 입력될 최초 펌프의 번호를 선택해야합니다.");
#else
		AfxMessageBox(L"You must select the number of the first pump to which the template will be input.");
#endif
		return;
	}

#ifndef ENGLISH_MODE
	strMsg.Format(
		L"기존에 이미 할당된 펌프가 있으면 덮어씌우시겠습니까?\n"
		L"\tYes를 선택 시 기존 펌프를 덮어씌웁니다.\n"
		L"\tNo를 선택 시 빈공간에 추가 됩니다."
	);
	
#else
	strMsg.Format(
		L"If there's an existing pump assigned, would you like to overwrite it?\n"
		L"\tIf you select Yes, the existing pump will be overwritten.\n"
		L"When you select \tNo, it will be added to the blank space."
	);
#endif
	if(AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) == IDYES)
		bOverWrite = TRUE;
	else
		bOverWrite = FALSE;

	for(i = 0; i < D_MAX_TEMPLETE_ITEM_CNT; i ++)
	{
		if(pTemp->pArrPumpType[i] == nullptr)
			continue; 

		for(x = nIdx; x < D_MAX_PUMP_COUNT; x ++)
		{
			pItem = (CPumpItem*)m_pCustomListCtrl->GetItem(x);
			if(pItem == nullptr)
				continue;

			// 이미 할당된 아이템에 대해 덮어씌울건지 확인
			if(pItem->GetAssign() == PUMP_ASSIGN)
			{
				if(bOverWrite == FALSE)
					continue;
				else
				{
					lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_DEL,x),(DWORD_PTR)pItem);
					if(lRet != 0)
					{
#ifndef ENGLISH_MODE
						strMsg.Format(L"대상이 되는 펌프 %s를 삭제하는데 실패 했습니다.",pItem->GetPumpFullName());
#else
						strMsg.Format(L"대상이 되는 펌프 %s를 삭제하는데 실패 했습니다.",pItem->GetPumpFullName());
#endif
						AfxMessageBox(strMsg);
						return;
					}
				}
			}

			// 이미 할당된 항목에 Overwrite 허용이면 기존 항목 삭제하고 for Loop를 빠져나온다.
			// Overwrite 허용하지 않으면 다음 번호로 이동
			break;
		}

		if(pItem == nullptr || x == D_MAX_PUMP_COUNT)
			continue;

		pPmp = new CDataPump;
		pPmp->SetPumpData(
			m_pCurFacpData->GetFacpID(),x + 1,pTemp->pArrPumpType[i]->nPumpType
			,pTemp->pArrPumpType[i]->strTypeName,pTemp->pArrPumpType[i]->strLcdMsg
			,L"",pTemp->pArrPumpType[i]->strTypeName,pTemp->pArrPumpType[i]->nPsUse
		);
		pItem->SetPumpItemData(pPmp,nullptr);

		lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_CURDATACHANGE,MAKELPARAM(DATA_ADD,0),(DWORD_PTR)pItem);
		if(lRet != 0)
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(L"펌프를 추가하는데 실패 했습니다.");
#else
			AfxMessageBox(L"Failed to add pump.");
#endif
			pItem->InitPumpData();
			delete pPmp;
			pPmp = nullptr;
			return;
		}
		if(nStartIdx < 0)
			nStartIdx = x;
		nIdx = x;
		nIdx ++; // customlist에서 인덱스 증가
	}
	m_pCustomListCtrl->SetCurSelIndex(nStartIdx);
#ifndef ENGLISH_MODE
	AfxMessageBox(L"템플릿을 추가하는데 성공했습니다.");
#else
	AfxMessageBox(L"Successfully added the template.");
#endif
}
