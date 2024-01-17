// DlgMakeAutoAllInput.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgMakeAutoAllInput.h"
#include "afxdialogex.h"


// CDlgMakeAutoAllInput 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMakeAutoAllInput, CDialogEx)

CDlgMakeAutoAllInput::CDlgMakeAutoAllInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_MAKEAUTO_ALLINPUT, pParent)
	, m_bInBtype(FALSE)
	, m_bInBuild(FALSE)
	, m_bInStair(FALSE)
	, m_bInFloor(FALSE)
	, m_bInRoom(FALSE)
	, m_bInInputType(FALSE)
	, m_bInEquip(FALSE)
	, m_bOutBuild(FALSE)
	, m_bOutBtype(FALSE)
	, m_bOutStair(FALSE)
	, m_bOutFloor(FALSE)
	, m_bOutRoom(FALSE)
	, m_bOutOutputType(FALSE)
	, m_bOutEquip(FALSE)
	, m_strInBtype(_T(""))
	, m_strInStair(_T(""))
	, m_strInRoom(_T(""))
	, m_strInInputType(_T(""))
	, m_strInEquip(_T(""))
	, m_strOutBtype(_T(""))
	, m_strOutStair(_T(""))
	, m_strOutRoom(_T(""))
	, m_strOutInputType(_T(""))
	, m_strOutEquip(_T(""))
	, m_bMatchBuild(FALSE)
	, m_bMatchBtype(FALSE)
	, m_bMatchEquip(FALSE)
	, m_bMatchFloor(FALSE)
	, m_bMatchRoom(FALSE)
	, m_bMatchStair(FALSE)
{

}

CDlgMakeAutoAllInput::~CDlgMakeAutoAllInput()
{
}

void CDlgMakeAutoAllInput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_IN_BUILD, m_bInBuild);
	DDX_Check(pDX, IDC_CHK_IN_BTYPE, m_bInBtype);
	DDX_Check(pDX, IDC_CHK_IN_STAIR, m_bInStair);
	DDX_Check(pDX, IDC_CHK_IN_FLOOR, m_bInFloor);
	DDX_Check(pDX, IDC_CHK_IN_ROOM, m_bInRoom);
	DDX_Check(pDX, IDC_CHK_IN_INPUTTYPE, m_bInInputType);
	DDX_Check(pDX, IDC_CHK_IN_EQUIP, m_bInEquip);
	DDX_Control(pDX, IDC_CMB_IN_FROM_BUILD, m_cmbInFromBuild);
	DDX_Control(pDX, IDC_CMB_IN_TO_BUILD, m_cmbInToBuild);
	DDX_Control(pDX, IDC_CMB_IN_FROM_FLOOR, m_cmbInFromFloor);
	DDX_Control(pDX, IDC_CMB_IN_TO_FLOOR, m_cmbInToFloor);
	DDX_Text(pDX, IDC_ED_IN_VIEW_BTYPE, m_strInBtype);
	DDX_Text(pDX, IDC_ED_IN_VIEW_STAIR, m_strInStair);
	DDX_Text(pDX, IDC_ED_IN_VIEW_ROOM, m_strInRoom);
	DDX_Text(pDX, IDC_ED_IN_VIEW_INPUTTYPE, m_strInInputType);
	DDX_Text(pDX, IDC_ED_IN_VIEW_EQUIP, m_strInEquip);

	DDX_Check(pDX, IDC_CHK_OUT_BUILD, m_bOutBuild);
	DDX_Check(pDX, IDC_CHK_OUT_BTYPE, m_bOutBtype);
	DDX_Check(pDX, IDC_CHK_OUT_STAIR, m_bOutStair);
	DDX_Check(pDX, IDC_CHK_OUT_FLOOR, m_bOutFloor);
	DDX_Check(pDX, IDC_CHK_OUT_ROOM, m_bOutRoom);
	DDX_Check(pDX, IDC_CHK_OUT_OUTPUTTYPE, m_bOutOutputType);
	DDX_Check(pDX, IDC_CHK_OUT_EQUIP, m_bOutEquip);
	DDX_Control(pDX, IDC_CMB_OUT_FROM_BUILD, m_cmbOutFromBuild);
	DDX_Control(pDX, IDC_CMB_OUT_TO_BUILD, m_cmbOutToBuild);
	DDX_Control(pDX, IDC_CMB_OUT_FROM_FLOOR, m_cmbOutFromFloor);
	DDX_Control(pDX, IDC_CMB_OUT_TO_FLOOR, m_cmbOutToFloor);
	DDX_Text(pDX, IDC_ED_OUT_VIEW_BTYPE, m_strOutBtype);
	DDX_Text(pDX, IDC_ED_OUT_VIEW_STAIR, m_strOutStair);
	DDX_Text(pDX, IDC_ED_OUT_VIEW_ROOM, m_strOutRoom);
	DDX_Text(pDX, IDC_ED_OUT_VIEW_OUTPUTTYPE, m_strOutInputType);
	DDX_Text(pDX, IDC_ED_OUT_VIEW_EQUIP, m_strOutEquip);
	DDX_Check(pDX, IDC_CHKP_MATCH_BUILD, m_bMatchBuild);
	DDX_Check(pDX, IDC_CHKP_MATCH_BTYPE, m_bMatchBtype);
	DDX_Check(pDX, IDC_CHKP_MATCH_EQUIP, m_bMatchEquip);
	DDX_Check(pDX, IDC_CHKP_MATCH_FLOOR, m_bMatchFloor);
	DDX_Check(pDX, IDC_CHKP_MATCH_ROOM, m_bMatchRoom);
	DDX_Check(pDX, IDC_CHKP_MATCH_STAIR, m_bMatchStair);
}


BEGIN_MESSAGE_MAP(CDlgMakeAutoAllInput, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_INPUT, &CDlgMakeAutoAllInput::OnBnClickedBtnPreviewInput)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_OUTPUT, &CDlgMakeAutoAllInput::OnBnClickedBtnPreviewOutput)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_LINK, &CDlgMakeAutoAllInput::OnBnClickedBtnPreviewLink)
	ON_BN_CLICKED(IDC_BTN_IN_SEARCH_BTYPE, &CDlgMakeAutoAllInput::OnBnClickedBtnInSearchBtype)
	ON_BN_CLICKED(IDC_BTN_IN_SEARCH_STAIR, &CDlgMakeAutoAllInput::OnBnClickedBtnInSearchStair)
	ON_BN_CLICKED(IDC_BTN_IN_SEARCH_ROOM, &CDlgMakeAutoAllInput::OnBnClickedBtnInSearchRoom)
	ON_BN_CLICKED(IDC_BTN_IN_SEARCH_INPUTTYPE, &CDlgMakeAutoAllInput::OnBnClickedBtnInSearchInputtype)
	ON_BN_CLICKED(IDC_BTN_IN_SEARCH_EQUIP, &CDlgMakeAutoAllInput::OnBnClickedBtnInSearchEquip)
	ON_BN_CLICKED(IDC_BTN_OUT_SEARCH_BTYPE, &CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchBtype)
	ON_BN_CLICKED(IDC_BTN_OUT_SEARCH_STAIR, &CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchStair)
	ON_BN_CLICKED(IDC_BTN_OUT_SEARCH_ROOM, &CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchRoom)
	ON_BN_CLICKED(IDC_BTN_OUT_SEARCH_OUTPUTTYPE, &CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchOutputtype)
	ON_BN_CLICKED(IDC_BTN_OUT_SEARCH_EQUIP, &CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchEquip)
END_MESSAGE_MAP()


// CDlgMakeAutoAllInput 메시지 처리기입니다.


BOOL CDlgMakeAutoAllInput::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect		rcFrame;
	CPoint		pt(0, 0);
	GetDlgItem(IDC_ST_PREVIEW_FRAME)->GetWindowRect(&rcFrame);
	ClientToScreen(&pt);

	rcFrame.left -= pt.x;
	rcFrame.right -= pt.x;
	rcFrame.top -= pt.y;
	rcFrame.bottom -= pt.y;

	//m_hSelect = NULL;
	//m_iSelCol = 0;

	m_ImgDeviceIcons.Create(IDB_BMP_DEVICE_ICON, 16, 8, RGB(0, 255, 255));
	m_ImgChecks.Create(IDB_BMP_CHECKS, 16, 8, RGB(0, 255, 255));
	m_ctrlTreeList.CreateEx(
		WS_EX_CLIENTEDGE
		, TVS_HASBUTTONS | TVS_HASLINES | TVS_FULLROWSELECT | TVS_TRACKSELECT | TVS_EDITLABELS | WS_CHILD | WS_VISIBLE | WS_TABSTOP
		, rcFrame
		, this
		, IDC_TREELIST
	);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewInput()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewOutput()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewLink()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnInSearchBtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnInSearchStair()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnInSearchRoom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnInSearchInputtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnInSearchEquip()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchBtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchStair()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchRoom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchOutputtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchEquip()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
