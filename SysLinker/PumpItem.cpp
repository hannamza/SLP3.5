// PumpItem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PumpItem.h"
#include "afxdialogex.h"
#include "DataPump.h"
#include "DataPs.h"

// CPumpItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPumpItem, CCustomBaseItem)

CPumpItem::CPumpItem(UINT uItemID,UINT uBkID,CWnd* pParent /*=NULL*/)
	: CCustomBaseItem(uItemID,uBkID,IDD_PUMP_ITEM, pParent)
	,m_strPumpLcdMsg(_T(""))
	,m_strPumpFullName(_T(""))
	,m_strPsFullName(_T("PS DATA TEST"))
	,m_bUsePs(FALSE)
	,m_strPrefix(_T(""))
	,m_strTypeName(_T(""))
{
	m_nAssigned = PUMP_UNASSIGN;
	m_pPSwitch = nullptr;
	m_pPump = nullptr;
	m_bModified = FALSE;
}

CPumpItem::~CPumpItem()
{
}

void CPumpItem::DoDataExchange(CDataExchange* pDX)
{
	CCustomBaseItem::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_ED_PUMP_LCDMSG,m_strPumpLcdMsg);
	DDX_Text(pDX,IDC_ST_PUMP_FULLNAME,m_strPumpFullName);
	DDX_Text(pDX,IDC_ST_PS_FULLNAME,m_strPsFullName);
	DDX_Control(pDX,IDC_BTN_DEL,m_btnDel);
	DDX_Check(pDX,IDC_CHK_USEPS,m_bUsePs);
	DDX_Control(pDX,IDC_ST_TYPENAME,m_ctrlTypeName);
	DDX_Text(pDX,IDC_ED_PREFIX,m_strPrefix);
	DDX_Text(pDX,IDC_ST_TYPENAME,m_strTypeName);
	DDX_Control(pDX,IDC_ST_PUMP_NUM,m_stPumpNum);
	DDX_Control(pDX,IDC_ED_PREFIX,m_edPrefix);
	DDX_Control(pDX,IDC_ED_PUMP_LCDMSG,m_edLcdMsg);
}


BEGIN_MESSAGE_MAP(CPumpItem, CCustomBaseItem)
	ON_BN_CLICKED(IDOK,&CPumpItem::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,&CPumpItem::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_DEL,&CPumpItem::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_CHK_USEPS,&CPumpItem::OnBnClickedChkUseps)
	ON_EN_CHANGE(IDC_ED_PUMP_LCDMSG,&CPumpItem::OnEnChangeEdPumpLcdmsg)
	ON_BN_CLICKED(IDC_RD_IN_FAN,&CPumpItem::OnBnClickedRdInFan)
	ON_BN_CLICKED(IDC_RD_IN_PUMP,&CPumpItem::OnBnClickedRdInPump)
	ON_BN_CLICKED(IDC_RD_IN_POWER,&CPumpItem::OnBnClickedRdInPower)
	ON_BN_CLICKED(IDC_RD_IN_GENERATOR,&CPumpItem::OnBnClickedRdInGenerator)
	ON_BN_CLICKED(IDC_RD_KEEP,&CPumpItem::OnBnClickedRdKeep)
	ON_BN_CLICKED(IDC_RD_NOKEEP,&CPumpItem::OnBnClickedRdNokeep)
	ON_EN_CHANGE(IDC_ED_PREFIX,&CPumpItem::OnEnChangeEdPrefix)
	ON_EN_SETFOCUS(IDC_ED_PREFIX,&CPumpItem::OnSetfocusEdPrefix)
	ON_EN_SETFOCUS(IDC_ED_PUMP_LCDMSG,&CPumpItem::OnSetfocusEdPumpLcdmsg)
END_MESSAGE_MAP()


// CPumpItem 메시지 처리기입니다.

BOOL CPumpItem::OnInitDialog()
{
	CCustomBaseItem::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bmpUnAssign.LoadBitmap(IDB_PUMPITEM_UNASSIGN);
	m_bmpAssign.LoadBitmap(IDB_PUMPITEM_ASSIGN);

	BITMAP bm;
	m_bmpAssign.GetBitmap(&bm);
	m_szBack = CSize(bm.bmWidth,bm.bmHeight);
	MoveWindow(0,0,m_szBack.cx,m_szBack.cy);

	CString str;
	str.Format(L"M%d",m_uItemID);
	//GetDlgItem(IDC_ST_PUMP_NUM)->SetWindowTextW(str);
	//m_stPumpNum.SetBackColor(RGB(255,0,0));
	//m_stPumpNum.SetTextColor(RGB(255,255,255));

	m_stPumpNum.SetBackColor(RGB(255,255,255));
	m_stPumpNum.SetTextColor(RGB(0,0,0));

	m_stPumpNum.SetFont(L"맑은 고딕");
	m_stPumpNum.SetHeight(28);
	m_stPumpNum.SetBoarder(TRUE);
	m_stPumpNum.SetAlignment(DT_CENTER | DT_SINGLELINE| DT_VCENTER);

	m_stPumpNum.SetText(str);

	SetUIInPumpType(m_nInPump);
	SetUIInPsType(m_nInPs);

	SetWindowTheme(GetDlgItem(IDC_RD_IN_FAN)->GetSafeHwnd(),_T(""),_T(""));
	SetWindowTheme(GetDlgItem(IDC_RD_IN_PUMP)->GetSafeHwnd(),_T(""),_T(""));
	SetWindowTheme(GetDlgItem(IDC_RD_IN_POWER)->GetSafeHwnd(),_T(""),_T(""));
	SetWindowTheme(GetDlgItem(IDC_RD_IN_GENERATOR)->GetSafeHwnd(),_T(""),_T(""));

	SetWindowTheme(GetDlgItem(IDC_RD_KEEP)->GetSafeHwnd(),_T(""),_T(""));
	SetWindowTheme(GetDlgItem(IDC_RD_NOKEEP)->GetSafeHwnd(),_T(""),_T(""));

	SetWindowTheme(GetDlgItem(IDC_CHK_USEPS)->GetSafeHwnd(),_T(""),_T(""));

	//m_btnDel.SetIcon(IDI_ICON6,(int)BTNST_AUTO_DARKER);
	m_btnDel.SetBitmaps(IDB_CLOSE_UNASSIGN,RGB(255,255,255));
	m_btnDel.SetTooltipText(_T("Delete Item"));
	SetUIAssign(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
CString CPumpItem::MakeFullName(BOOL bPump,int nInType,CString strPrefix,CString strTypeName)
{
	CString strRet = L"";
	CString strSuffix = L"";
	if(bPump)
	{
		// 		if(nInType == IN_PUMP_POWER)
		// 			strSuffix = SUFFIX_POWER;
		// 		else

		switch(nInType)
		{
		case IN_PUMP_FAN:
			strSuffix = SUFFIX_FAN;
			break;
			// 		case IN_PUMP_PUMP:
			// 			strSuffix = SUFFIX_FAN;
			// 			break;
		case IN_PUMP_GENERATOR:
			strSuffix = SUFFIX_GENERATOR;
			break;
		case IN_PUMP_POWER:
			strSuffix = SUFFIX_POWER;
			break;
		default:
			strSuffix = SUFFIX_PUMP;
			break;
		}
		if(strTypeName.GetLength() == 0)
			strRet = L"";
		else
			strRet.Format(L"%s %s %s",strPrefix,strTypeName,strSuffix);
	}
	else
	{
		if(strTypeName.GetLength() == 0)
		{
			//strRet = L"";
// 			if(m_pPSwitch)
// 				strRet = m_pPSwitch->GetPSwitchName();
// 			else
				strRet = L"";
		}
		else
			strRet.Format(L"%s %s %s",strPrefix,strTypeName,SUFFIX_PS);
	}
	strRet.TrimLeft();
	strRet.TrimRight();
	return strRet;
}

void CPumpItem::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CCustomBaseItem::OnOK();
}


void CPumpItem::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CCustomBaseItem::OnCancel();
}

BOOL CPumpItem::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	POINT at;
	CBitmap * pBmp,* pOldBmp;
	CRect rc;
	GetClientRect(&rc);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	switch(m_nAssigned)
	{
	case PUMP_ASSIGN:
		pBmp = &m_bmpAssign;
		break;
	case PUMP_UNASSIGN:
		pBmp = &m_bmpUnAssign;
		break;
	default:
		pBmp = &m_bmpUnAssign;
		break;
	}

	pOldBmp = memDC.SelectObject(pBmp);
	pDC->StretchBlt(0,0,rc.Width(),rc.Height(),&memDC,0,0,rc.Width(),rc.Height(),SRCCOPY);
	if(m_bSelected)
	{
		CPen pn(PS_SOLID,8,RGB(255,0,0));
		CBrush br;
		br.CreateStockObject(NULL_BRUSH);
		CBrush * pOldBr = pDC->SelectObject(&br);
		CPen* pOldPen = pDC->SelectObject(&pn);
		pDC->Rectangle(&rc);
		//pDC->FrameRect(&rc,&br);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBr);
	}
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();
	return TRUE;
}


HBRUSH CPumpItem::OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC,pWnd,nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

void CPumpItem::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 즉각 적용해야 될거 같음
	// 삭제하면 정보가 없음
// 	InitPumpData();
// 	SetModifiedFlag(TRUE);

	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
	if(m_nAssigned == PUMP_UNASSIGN)
		return;


// 
// 	CString strMsg;
// 	strMsg.Format(L"%s를 삭제 하시겠습니까?" , m_strPumpFullName);
// 	if(AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) != IDYES)
// 		return; 

	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_DELETE,(WPARAM)this,0);
}


void CPumpItem::OnBnClickedChkUseps()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	SetUsePs(m_bUsePs);
	SetModifiedFlag(TRUE);
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}


void CPumpItem::OnEnChangeEdPumpLcdmsg()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CCustomBaseItem::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	SetModifiedFlag(TRUE);
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}


void CPumpItem::OnBnClickedRdInFan()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetModifiedFlag(TRUE);
	m_nInPump = IN_PUMP_FAN;
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
	//m_nInPump = IN_PUMP_FAN;
}


void CPumpItem::OnBnClickedRdInPump()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetModifiedFlag(TRUE);
	m_nInPump = IN_PUMP_PUMP;
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}


void CPumpItem::OnBnClickedRdInPower()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetModifiedFlag(TRUE);
	m_nInPump = IN_PUMP_POWER;
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}


void CPumpItem::OnBnClickedRdInGenerator()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetModifiedFlag(TRUE);
	m_nInPump = IN_PUMP_GENERATOR;
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}


void CPumpItem::OnBnClickedRdKeep()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetModifiedFlag(TRUE);
	m_nInPs = IN_PS_KEEP;
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}


void CPumpItem::OnBnClickedRdNokeep()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetModifiedFlag(TRUE);
	m_nInPs = IN_PS_NOKEEP;
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}

BOOL CPumpItem::CopyCustomItem(CCustomBaseItem * pSrcItem)
{
	if(pSrcItem == nullptr)
		return FALSE;
	CPumpItem * pSrcPmp;
	pSrcPmp = (CPumpItem *)pSrcItem;

// 	SetPumpItemData(
// 		pSrcPmp->GetTypeName(),pSrcPmp->GetPrefix()
// 		,pSrcPmp->GetInPump(),pSrcPmp->GetPumpLcdMsg()
// 		,pSrcPmp->GetUsePs(),pSrcPmp->GetInPump()
// 	);
	if(pSrcPmp->GetDataPump() == nullptr)
	{
		return FALSE;
// 		SetPumpItemData(
// 			pSrcPmp->GetTypeName(),pSrcPmp->GetPrefix()
// 			,pSrcPmp->GetInPump(),pSrcPmp->GetPumpLcdMsg()
// 			,pSrcPmp->GetUsePs(),pSrcPmp->GetInPump()
// 		);
	}
	else
	{
		SetPumpItemData(pSrcPmp->GetDataPump(),pSrcPmp->GetDataPS());
	}
	return TRUE;
}


BOOL CPumpItem::InitCustomItem()
{
	CCustomBaseItem::InitCustomItem();
	InitPumpData();
	return TRUE;
}

void CPumpItem::UpdateOldData(BOOL bSaveOldData)
{
	if(bSaveOldData)
	{
		
	}
	else
	{
// 		SetPumpLcdMsg(m_OldstrPumpLcdMsg);
// 		SetPrefix(m_OldstrPrefix);
// 		SetInPump(m_OldnInPump);
// 		SetUsePs(m_OldbUsePs);
// 		SetInPs(m_OldnInPs);
// 
// 		m_strPumpFullName = MakeFullName(TRUE,m_nInPump,m_strPrefix,m_strTypeName);
// 		m_nAssigned = PUMP_ASSIGN;
		SetPumpItemData(m_pPump,m_pPSwitch);
	}
}


void CPumpItem::SetSelected(BOOL bSel)
{
	m_bSelected = bSel;
	if(m_bSelected)
	{
		SetFocus();
		GetDlgItem(IDC_ED_PREFIX)->SetFocus();
	}
	if(m_nAssigned == PUMP_ASSIGN)
	{
		GetDlgItem(IDC_ED_PREFIX)->EnableWindow(m_bSelected);
		GetDlgItem(IDC_ED_PUMP_LCDMSG)->EnableWindow(m_bSelected);

		GetDlgItem(IDC_RD_IN_FAN)->EnableWindow(m_bSelected);
		GetDlgItem(IDC_RD_IN_PUMP)->EnableWindow(m_bSelected);
		GetDlgItem(IDC_RD_IN_POWER)->EnableWindow(m_bSelected);
		GetDlgItem(IDC_RD_IN_GENERATOR)->EnableWindow(m_bSelected);

		GetDlgItem(IDC_BTN_DEL)->EnableWindow(m_bSelected);

		GetDlgItem(IDC_CHK_USEPS)->EnableWindow(m_bSelected);

		if(m_pPump != nullptr)
		{
			if(m_pPump->GetUsePS() == PUMP_PS_NOTAVAILABLE)
			{
				GetDlgItem(IDC_CHK_USEPS)->EnableWindow(FALSE);
				GetDlgItem(IDC_RD_KEEP)->EnableWindow(FALSE);
				GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(FALSE);
			}
			else
			{
				if(m_bUsePs)
				{
					GetDlgItem(IDC_RD_KEEP)->EnableWindow(m_bSelected);
					GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(m_bSelected);
				}
				else
				{
					GetDlgItem(IDC_RD_KEEP)->EnableWindow(FALSE);
					GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(FALSE);
				}
			}
		}
	}
	else
	{
		GetDlgItem(IDC_ED_PREFIX)->EnableWindow(FALSE);
		GetDlgItem(IDC_ED_PUMP_LCDMSG)->EnableWindow(FALSE);

		GetDlgItem(IDC_RD_IN_FAN)->EnableWindow(FALSE);
		GetDlgItem(IDC_RD_IN_PUMP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RD_IN_POWER)->EnableWindow(FALSE);
		GetDlgItem(IDC_RD_IN_GENERATOR)->EnableWindow(FALSE);

		GetDlgItem(IDC_BTN_DEL)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHK_USEPS)->EnableWindow(FALSE);

		GetDlgItem(IDC_RD_KEEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(FALSE);
	
	}
	
	Invalidate();
}


int CPumpItem::SetPumpItemData(CDataPump* pPmp,CDataPS* pPs)
{
	// 펌프가 없으면 빈 UI
	if(pPmp == nullptr)
	{
		SetUIAssign(FALSE);
		return 0; 
	}
	
	SetUIInPumpType(pPmp->GetPumpType());
	SetTypeName(pPmp->GetTypeName());
	SetPrefix(pPmp->GetPrefix());

	m_strPumpLcdMsg = pPmp->GetPumpLcd();
	m_strPumpFullName = MakeFullName(TRUE,m_nInPump,m_strPrefix,m_strTypeName);
	if(m_strPumpFullName.GetLength() == 0)
		m_strPumpFullName = pPmp->GetPumpName();
	m_nAssigned = PUMP_ASSIGN;
	
	if(pPmp->GetUsePS() == PUMP_PS_NOTAVAILABLE)
		GetDlgItem(IDC_CHK_USEPS)->EnableWindow(FALSE);
	else if(pPmp->GetUsePS() == PUMP_PS_USE)
	{
		GetDlgItem(IDC_CHK_USEPS)->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_CHK_USEPS))->SetCheck(TRUE);
		m_bUsePs = TRUE;
	}

	SetUsePs(pPs != nullptr);

	if(m_bUsePs)
	{
		m_strPsFullName = MakeFullName(FALSE,m_nInPs,m_strPrefix,m_strTypeName);
		if(m_strPsFullName.GetLength() == 0)
		{
			if(pPs)
				m_strPsFullName = pPs->GetPSwitchName();
		}
		if(pPs)
			SetUIInPsType(pPs->GetPSwitchType());
		//GetDlgItem(IDC_ST_PS_FULLNAME)->SetWindowTextW(m_strPsFullName);
	}
	m_pPSwitch = pPs;
	m_pPump = pPmp;

	UpdateOldData(TRUE);
	UpdateData(FALSE);
	SetUIAssign(TRUE);
	RedrawWindow();
	return 0;
}


int CPumpItem::InitPumpData()
{
	m_strPrefix = L"";
	m_strTypeName = L"";

	SetUIInPumpType(0);
	m_strPumpLcdMsg = L"";
	m_strPumpFullName = L"";

	m_bUsePs = FALSE;
	SetUIInPsType(0);
	m_strPsFullName = L"";
	m_nAssigned = PUMP_UNASSIGN;
	m_pPump = nullptr;
	m_pPSwitch = nullptr;
	UpdateData(FALSE);
	RedrawWindow();
	m_btnDel.Invalidate();
	SetUIAssign(FALSE);

	UpdateOldData(TRUE);
	return 0;
}

int CPumpItem::SetUIInPumpType(int nInType)
{
	m_nInPump = nInType;
	switch(m_nInPump)
	{
	case IN_PUMP_FAN:
		((CButton*)GetDlgItem(IDC_RD_IN_FAN))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RD_IN_PUMP))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_POWER))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_GENERATOR))->SetCheck(0);
		break;
	case IN_PUMP_PUMP:
		((CButton*)GetDlgItem(IDC_RD_IN_FAN))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_PUMP))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RD_IN_POWER))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_GENERATOR))->SetCheck(0);
		break;
	case IN_PUMP_POWER:
		((CButton*)GetDlgItem(IDC_RD_IN_FAN))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_PUMP))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_POWER))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RD_IN_GENERATOR))->SetCheck(0);
		break;
	default:
		((CButton*)GetDlgItem(IDC_RD_IN_FAN))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_PUMP))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_POWER))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_IN_GENERATOR))->SetCheck(1);
		break;
	}
	return 0;
}

int CPumpItem::SetUIInPsType(int nInType)
{
	m_nInPs = nInType;

	if(nInType == 0)
		m_nInPs = IN_PS_KEEP;

	if(m_nInPs == IN_PS_KEEP)
	{
		((CButton*)GetDlgItem(IDC_RD_KEEP))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RD_NOKEEP))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RD_KEEP))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RD_NOKEEP))->SetCheck(1);
	}
	return 0;
}

void CPumpItem::SetUsePs(BOOL bUsePs)
{
	m_bUsePs = bUsePs;
	if(m_bSelected)
	{
		((CButton*)GetDlgItem(IDC_RD_KEEP))->EnableWindow(m_bUsePs);
		((CButton*)GetDlgItem(IDC_RD_NOKEEP))->EnableWindow(m_bUsePs);
		//(GetDlgItem(IDC_ST_PS_FULLNAME))->EnableWindow(m_bUsePs);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RD_KEEP))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RD_NOKEEP))->EnableWindow(FALSE);
		//(GetDlgItem(IDC_ST_PS_FULLNAME))->EnableWindow(FALSE);
	}
	
	if(m_bUsePs)
	{
		m_strPsFullName = MakeFullName(FALSE,m_nInPs,m_strPrefix,m_strTypeName);
		if(m_strPsFullName.GetLength() <= 0)
		{
			if(m_pPSwitch)
				m_strPsFullName = m_pPSwitch->GetPSwitchName();
		}
		GetDlgItem(IDC_ST_PS_FULLNAME)->SetWindowTextW(m_strPsFullName);
	}
	else
		GetDlgItem(IDC_ST_PS_FULLNAME)->SetWindowTextW(L"");
	RedrawWindow();
}

void CPumpItem::SetUIAssign(BOOL bAssigned)
{
// 	GetDlgItem(IDC_ED_PREFIX)->EnableWindow(bAssigned);
// 
// 	GetDlgItem(IDC_ED_PUMP_LCDMSG)->EnableWindow(bAssigned);
// 
// 	GetDlgItem(IDC_RD_IN_FAN)->EnableWindow(bAssigned);
// 	GetDlgItem(IDC_RD_IN_PUMP)->EnableWindow(bAssigned);
// 	GetDlgItem(IDC_RD_IN_POWER)->EnableWindow(bAssigned);
// 	GetDlgItem(IDC_RD_IN_GENERATOR)->EnableWindow(bAssigned);
// 
// 	GetDlgItem(IDC_CHK_USEPS)->EnableWindow(bAssigned);
//	SetUsePs(m_bUsePs);
	// 	GetDlgItem(IDC_RD_KEEP)->EnableWindow(bAssigned);
	// 	GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(bAssigned);
	// 	GetDlgItem(IDC_ST_PS_FULLNAME)->EnableWindow(bAssigned);
// 	if(bAssigned == FALSE)
// 	{
// 		GetDlgItem(IDC_ST_PS_FULLNAME)->SetWindowTextW(L"");
// 		m_btnDel.SetBitmaps(IDB_CLOSE_UNASSIGN,RGB(255,255,255));
// 	}
// 	else
// 	{
// 		m_btnDel.SetBitmaps(IDB_CLOSE_ASSIGN,RGB(255,255,255));
// 	}

	if(m_nAssigned == PUMP_ASSIGN)
	{
		m_btnDel.SetBitmaps(IDB_CLOSE_ASSIGN,RGB(255,255,255));
		GetDlgItem(IDC_ED_PREFIX)->EnableWindow(m_bSelected);
		GetDlgItem(IDC_ED_PUMP_LCDMSG)->EnableWindow(m_bSelected);

		GetDlgItem(IDC_RD_IN_FAN)->EnableWindow(m_bSelected);
		GetDlgItem(IDC_RD_IN_PUMP)->EnableWindow(m_bSelected);
		GetDlgItem(IDC_RD_IN_POWER)->EnableWindow(m_bSelected);
		GetDlgItem(IDC_RD_IN_GENERATOR)->EnableWindow(m_bSelected);

		GetDlgItem(IDC_BTN_DEL)->EnableWindow(m_bSelected);

		GetDlgItem(IDC_CHK_USEPS)->EnableWindow(m_bSelected);

		if(m_pPump != nullptr)
		{
			if(m_pPump->GetUsePS() == PUMP_PS_NOTAVAILABLE)
			{
				GetDlgItem(IDC_CHK_USEPS)->EnableWindow(FALSE);
				GetDlgItem(IDC_RD_KEEP)->EnableWindow(FALSE);
				GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(FALSE);
			}
			else
			{
				if(m_bUsePs)
				{
					GetDlgItem(IDC_RD_KEEP)->EnableWindow(m_bSelected);
					GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(m_bSelected);
				}
				else
				{
					GetDlgItem(IDC_RD_KEEP)->EnableWindow(FALSE);
					GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(FALSE);
				}
			}
		}
	}
	else
	{
		m_btnDel.SetBitmaps(IDB_CLOSE_UNASSIGN,RGB(255,255,255));
		GetDlgItem(IDC_ED_PREFIX)->EnableWindow(FALSE);
		GetDlgItem(IDC_ED_PUMP_LCDMSG)->EnableWindow(FALSE);

		GetDlgItem(IDC_RD_IN_FAN)->EnableWindow(FALSE);
		GetDlgItem(IDC_RD_IN_PUMP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RD_IN_POWER)->EnableWindow(FALSE);
		GetDlgItem(IDC_RD_IN_GENERATOR)->EnableWindow(FALSE);

		GetDlgItem(IDC_BTN_DEL)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHK_USEPS)->EnableWindow(FALSE);

		GetDlgItem(IDC_RD_KEEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RD_NOKEEP)->EnableWindow(FALSE);

	}
	//SetSelected(FALSE);
}

CString CPumpItem::GetSuffix(int nInPump)
{
	CString strRet;
	switch(nInPump)
	{
	case IN_PUMP_FAN:
		strRet = SUFFIX_FAN;
		break;
		// 		case IN_PUMP_PUMP:
		// 			strSuffix = SUFFIX_FAN;
		// 			break;
	case IN_PUMP_GENERATOR:
		strRet = SUFFIX_GENERATOR;
		break;
	case IN_PUMP_POWER:
		strRet = SUFFIX_POWER;
		break;
	default:
		strRet = SUFFIX_PUMP;
		break;
	}
	return strRet;
}

void CPumpItem::OnEnChangeEdPrefix()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CCustomBaseItem::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	if(m_pPump == nullptr)
		return; 

	m_strTypeName;
	int nFind;
	CString strUp1 , strUp2,strUp3,strsuffix ,strtemp; 

	if(m_strTypeName.GetLength() == 0)
	{
		strtemp = m_pPump->GetPumpName();
		strUp1 = strtemp;

		strUp2 = GetSuffix(m_nInPump);
		strUp1.MakeUpper();
		strUp2.MakeUpper();
		nFind = strUp1.Find(strUp2);
		if(nFind >= 0)
		{
			// '0번 펌프1 기동확인'에서 '기동확인'제거
			strtemp.Delete(nFind,m_pPump->GetPumpName().GetLength() - nFind);
			strtemp.TrimRight();
			strUp1 = strtemp;
			strUp1.MakeUpper();
		}
		strUp3 = m_strPrefix;
		strUp3.MakeUpper();
		nFind = strUp1.Find(strUp3);
		if(nFind >= 0)
		{
			strtemp.Delete(0,nFind + strUp3.GetLength());
			strtemp.TrimLeft();
		}

		m_strTypeName = strtemp;
	}

	m_strPumpFullName = MakeFullName(TRUE,m_nInPump,m_strPrefix,m_strTypeName);
	if(m_bUsePs)
	{
		m_strPsFullName = MakeFullName(FALSE,m_nInPs,m_strPrefix,m_strTypeName);
		if(m_strPsFullName.GetLength() == 0)
		{
			if(m_pPSwitch)
				m_strPsFullName = m_pPSwitch->GetPSwitchName();
		}
	}
	UpdateData(FALSE);
	SetModifiedFlag(TRUE);
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
	RedrawWindow();
	// 	GetDlgItem(IDC_ST_PUMP_FULLNAME)->RedrawWindow();
	// 	if(m_bUsePs)
	// 		GetDlgItem(IDC_ST_PS_FULLNAME)->RedrawWindow();

}

void CPumpItem::OnSetfocusEdPrefix()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}


void CPumpItem::OnSetfocusEdPumpLcdmsg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(GetParent())
		GetParent()->PostMessageW(CIM_ITEM_SELECTED,(WPARAM)this,0);
}

BOOL CPumpItem::SetNewPs(CDataPS * pPs)
{
	if(pPs == nullptr)
		return FALSE;

	// [2025/10/14 8:25:48 KHS] 
	// 압력스위치 LCD 이름 사용하지 않음 --> 전체 이름으로 표시
	m_pPSwitch = pPs;
	m_pPSwitch->SetPSwitchData(
		m_pPump->GetFacpID()
		,m_pPump->GetPumpID()
		,m_nInPs
		,MakeFullName(FALSE,m_nInPs,m_strPrefix,m_strTypeName)
		,MakeFullName(FALSE,m_nInPs,m_strPrefix,m_strTypeName)
		,m_strPrefix
		,m_strTypeName
		,0
	);

	SetModifiedFlag(TRUE);
	return TRUE;
}


BOOL CPumpItem::UpdatePumpData()
{
	if(m_pPump == nullptr)
		return FALSE;
	m_pPump->SetPumpData(
		m_pPump->GetFacpID()
		,m_uItemID
		,m_nInPump
		,m_strPumpFullName
		,m_strPumpLcdMsg
		,m_strPrefix
		,m_strTypeName
		,m_pPump->GetUsePS()
	);
	return TRUE;
}

BOOL CPumpItem::UpdatePsData()
{
	if(m_pPSwitch == nullptr)
		return FALSE;
	m_pPSwitch->SetPSwitchData(
		m_pPump->GetFacpID()
		,m_uItemID
		,m_nInPs
		,m_strPsFullName
		,L""
		,m_strPrefix
		,m_strTypeName
		,0
	);
	return TRUE;
}

BOOL CPumpItem::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		// Enter 입력
		if(pMsg->wParam == VK_RETURN)
		{
			if(m_nAssigned == PUMP_UNASSIGN)
				return TRUE;

			if(GetParent())
				GetParent()->PostMessageW(CIM_ITEM_EDITEND,(WPARAM)this,0);
			return TRUE;
		}
		// Escape 취소
		if(pMsg->wParam == VK_DELETE)
		{
			CWnd* pFocusWnd = GetFocus();
			if(pFocusWnd->GetSafeHwnd() == m_edLcdMsg.GetSafeHwnd()
				|| pFocusWnd->GetSafeHwnd() == m_edPrefix.GetSafeHwnd())
				return CCustomBaseItem::PreTranslateMessage(pMsg);


			if(m_nAssigned == PUMP_UNASSIGN)
				return TRUE;
			
			if(GetParent())
				GetParent()->PostMessageW(CIM_ITEM_DELETE,(WPARAM)this,0);
			return TRUE;
		}
	}
	return CCustomBaseItem::PreTranslateMessage(pMsg);
}
