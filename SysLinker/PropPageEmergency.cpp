// PropPageEmergency.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPageEmergency.h"
#include "afxdialogex.h"
#include "RelayTableData.h"

// CPropPageEmergency 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropPageEmergency, CPropertyPage)

#ifndef ENGLISH_MODE
CPropPageEmergency::CPropPageEmergency()
	: CPropertyPage(IDD_PROP_PAGE_EMERGENCY)
	, m_bBuild(FALSE)
	, m_bBtype(FALSE)
	, m_bBtypeBuild(FALSE)
	, m_bStair(FALSE)
	, m_bStairBuild(FALSE)
	, m_bStairBtype(FALSE)
	, m_bLevel(FALSE)
	, m_bLevelBuild(FALSE)
	, m_bLevelBType(FALSE)
	, m_bLevelStair(FALSE)
	, m_bRoom(FALSE)
	, m_bRoomBuild(FALSE)
	, m_bRoomBType(FALSE)
	, m_bRoomStair(FALSE)
	, m_bRoomLevel(FALSE)
{
	m_pRefFasSysData = nullptr;
}
#else
CPropPageEmergency::CPropPageEmergency()
	: CPropertyPage(IDD_PROP_PAGE_EMERGENCY_EN)
	, m_bBuild(FALSE)
	, m_bBtype(FALSE)
	, m_bBtypeBuild(FALSE)
	, m_bStair(FALSE)
	, m_bStairBuild(FALSE)
	, m_bStairBtype(FALSE)
	, m_bLevel(FALSE)
	, m_bLevelBuild(FALSE)
	, m_bLevelBType(FALSE)
	, m_bLevelStair(FALSE)
	, m_bRoom(FALSE)
	, m_bRoomBuild(FALSE)
	, m_bRoomBType(FALSE)
	, m_bRoomStair(FALSE)
	, m_bRoomLevel(FALSE)
{
	m_pRefFasSysData = nullptr;
}
#endif

CPropPageEmergency::~CPropPageEmergency()
{
}

void CPropPageEmergency::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_BUILD, m_bBuild);
	DDX_Check(pDX, IDC_CHKE_BTYPE, m_bBtype);
	DDX_Check(pDX, IDC_CHKE_BTYPE_BUILD, m_bBtypeBuild);

	DDX_Check(pDX, IDC_CHKE_STAIR, m_bStair);
	DDX_Check(pDX, IDC_CHKE_STAIR_BUILD, m_bStairBuild);
	DDX_Check(pDX, IDC_CHKE_STAIR_BTYPE, m_bStairBtype);
	
	DDX_Check(pDX, IDC_CHK_LEVEL, m_bLevel);
	DDX_Check(pDX, IDC_CHK_LEVEL_BUILD, m_bLevelBuild);
	DDX_Check(pDX, IDC_CHK_LEVEL_BTYPE, m_bLevelBType);
	DDX_Check(pDX, IDC_CHK_LEVEL_STAIR, m_bLevelStair);

	DDX_Check(pDX, IDC_CHK_ROOM, m_bRoom);
	DDX_Check(pDX, IDC_CHK_ROOM_BUILD, m_bRoomBuild);
	DDX_Check(pDX, IDC_CHK_ROOM_BTYPE, m_bRoomBType);
	DDX_Check(pDX, IDC_CHK_ROOM_STAIR, m_bRoomStair);
	DDX_Check(pDX, IDC_CHK_ROOM_LEVEL, m_bRoomLevel);
	DDX_Control(pDX, IDC_EMB_LIST, m_ctrlItemList);
}


BEGIN_MESSAGE_MAP(CPropPageEmergency, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CPropPageEmergency::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropPageEmergency::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, &CPropPageEmergency::OnBnClickedBtnPreview)
END_MESSAGE_MAP()


// CPropPageEmergency 메시지 처리기입니다.


void CPropPageEmergency::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPropPageEmergency::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CPropPageEmergency::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK|PSWIZB_FINISH);
	return CPropertyPage::OnSetActive();
}
BOOL CPropPageEmergency::OnWizardFinish()
{
	CString report = _T("You have selected the following options:\n");
	CPropertySheet* sheet = (CPropertySheet*)GetParent();
	int count = sheet->GetPageCount();
	//AfxMessageBox(report);

	return CPropertyPage::OnWizardFinish();
}

void CPropPageEmergency::OnBnClickedBtnPreview()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pRefFasSysData == nullptr)
		return;

	UpdateData();

	m_pRefFasSysData->ProcessingEmergency(
		m_bBuild
		, m_bBtype, m_bBtypeBuild
		, m_bStair, m_bStairBuild, m_bStairBtype
		, m_bLevel, m_bLevelBuild, m_bLevelBType, m_bLevelStair
		, m_bRoom, m_bRoomBuild, m_bRoomBType, m_bRoomStair, m_bRoomLevel
	);
}


BOOL CPropPageEmergency::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
