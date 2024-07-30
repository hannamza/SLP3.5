// PropPagePattern.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPagePattern.h"
#include "afxdialogex.h"
#include "RelayTableData.h"

// CPropPagePattern 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropPagePattern, CPropertyPage)

#ifndef ENGLISH_MODE
CPropPagePattern::CPropPagePattern()
	: CPropertyPage(IDD_PROP_PAGE_PATTERN)
	, m_bEquip(FALSE)
	, m_bEquipBuild(FALSE)
	, m_bEquipLevel(FALSE)
	, m_bLevel(FALSE)
	, m_bPattern(TRUE)
	, m_bBuild(FALSE)
	, m_bLevelBuild(FALSE)
	, m_bRoom(FALSE)
	, m_bRoomBuild(FALSE)
	, m_bRoomLevel(FALSE)
	, m_bStair(FALSE)
	, m_bStairBuild(FALSE)
	, m_bBtype(FALSE)
	, m_bBtypeBuild(FALSE)
	, m_bStairBtype(FALSE)
	, m_bLevelBType(FALSE)
	, m_bLevelStair(FALSE)
	, m_bRoomBType(FALSE)
	, m_bRoomStair(FALSE)
	, m_bEquipBType(FALSE)
	, m_bEquipStair(FALSE)
{
	m_pRefFasSysData = nullptr;

}
#else
CPropPagePattern::CPropPagePattern()
	: CPropertyPage(IDD_PROP_PAGE_PATTERN_EN)
	, m_bEquip(FALSE)
	, m_bEquipBuild(FALSE)
	, m_bEquipLevel(FALSE)
	, m_bLevel(FALSE)
	, m_bPattern(TRUE)
	, m_bBuild(FALSE)
	, m_bLevelBuild(FALSE)
	, m_bRoom(FALSE)
	, m_bRoomBuild(FALSE)
	, m_bRoomLevel(FALSE)
	, m_bStair(FALSE)
	, m_bStairBuild(FALSE)
	, m_bBtype(FALSE)
	, m_bBtypeBuild(FALSE)
	, m_bStairBtype(FALSE)
	, m_bLevelBType(FALSE)
	, m_bLevelStair(FALSE)
	, m_bRoomBType(FALSE)
	, m_bRoomStair(FALSE)
	, m_bEquipBType(FALSE)
	, m_bEquipStair(FALSE)
{
	m_pRefFasSysData = nullptr;

}
#endif

CPropPagePattern::~CPropPagePattern()
{
}

void CPropPagePattern::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_PATTERN, m_bPattern);
	DDX_Check(pDX, IDC_CHKP_BUILD, m_bBuild);
	DDX_Check(pDX, IDC_CHK_BTYPE, m_bBtype);
	DDX_Check(pDX, IDC_CHKP_BTYPE_BUILD, m_bBtypeBuild);
	DDX_Check(pDX, IDC_CHKP_STAIR, m_bStair);
	DDX_Check(pDX, IDC_CHKP_STAIR_BUILD, m_bStairBuild);
	DDX_Check(pDX, IDC_CHKP_STAIR_BTYPE, m_bStairBtype);
	DDX_Check(pDX, IDC_CHK_LEVEL, m_bLevel);
	DDX_Check(pDX, IDC_CHKP_LEVEL_BUILD, m_bLevelBuild);
	DDX_Check(pDX, IDC_CHKP_LEVEL_BTYPE, m_bLevelBType);
	DDX_Check(pDX, IDC_CHKP_LEVEL_STAIR, m_bLevelStair);
	DDX_Check(pDX, IDC_CHKP_ROOM, m_bRoom);
	DDX_Check(pDX, IDC_CHKP_ROOM_BUILD, m_bRoomBuild);
	DDX_Check(pDX, IDC_CHKP_ROOM_BTYPE, m_bRoomBType);
	DDX_Check(pDX, IDC_CHKP_ROOM_STAIR, m_bRoomStair);
	DDX_Check(pDX, IDC_CHKP_ROOM_LEVEL, m_bRoomLevel);
	DDX_Check(pDX, IDC_CHK_EQUIP, m_bEquip);
	DDX_Check(pDX, IDC_CHK_EQUIP_BUILD, m_bEquipBuild);
	DDX_Check(pDX, IDC_CHKP_EQUIP_BTYPE, m_bEquipBType);
	DDX_Check(pDX, IDC_CHKP_EQUIP_STAIR, m_bEquipStair);
	DDX_Check(pDX, IDC_CHK_EQUIP_LEVEL, m_bEquipLevel);
	DDX_Control(pDX, IDC_PATTERN_LIST, m_ctrlPatternList);

}


BEGIN_MESSAGE_MAP(CPropPagePattern, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CPropPagePattern::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropPagePattern::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, &CPropPagePattern::OnBnClickedBtnPreview)
END_MESSAGE_MAP()


// CPropPagePattern 메시지 처리기입니다.
BOOL CPropPagePattern::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
#ifndef ENGLISH_MODE
	m_ctrlPatternList.InsertColumn(0, _T("순번"), LVCFMT_CENTER, 50);
	m_ctrlPatternList.InsertColumn(1, _T("패턴 이름"), LVCFMT_CENTER, 200);
	m_ctrlPatternList.InsertColumn(2, _T("종류"), LVCFMT_CENTER, 80);
#else
	m_ctrlPatternList.InsertColumn(0, _T("ORDER"), LVCFMT_CENTER, 50);
	m_ctrlPatternList.InsertColumn(1, _T("PATTERN NAME"), LVCFMT_CENTER, 200);
	m_ctrlPatternList.InsertColumn(2, _T("TYPE"), LVCFMT_CENTER, 80);
#endif
	m_ctrlPatternList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPropPagePattern::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPropPagePattern::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

BOOL CPropPagePattern::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	//pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	pSheet->SetWizardButtons(PSWIZB_FINISH | PSWIZB_BACK);
	return CPropertyPage::OnSetActive();
}

void CPropPagePattern::OnBnClickedBtnPreview()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pRefFasSysData == nullptr)
		return;
	UpdateData();
	m_pRefFasSysData->ProcessingPattern(
		nullptr
		,m_bBuild
		, m_bBtype, m_bBtypeBuild
		, m_bStair, m_bStairBuild, m_bStairBtype
		, m_bLevel, m_bLevelBuild, m_bLevelBType, m_bLevelStair
		, m_bRoom, m_bRoomBuild, m_bRoomBType, m_bRoomStair, m_bRoomLevel
		, m_bEquip, m_bEquipBuild, m_bEquipBType, m_bEquipStair, m_bEquipLevel
	);
	m_pRefFasSysData->FillPatternList(&m_ctrlPatternList);
}


LRESULT CPropPagePattern::OnWizardBack()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CPropertyPage::OnWizardBack();
}


BOOL CPropPagePattern::OnWizardFinish()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UpdateData();
	m_pRefFasSysData->ProcessingPattern(
		nullptr
		,m_bBuild
		, m_bBtype, m_bBtypeBuild
		, m_bStair, m_bStairBuild, m_bStairBtype
		, m_bLevel, m_bLevelBuild, m_bLevelBType, m_bLevelStair
		, m_bRoom, m_bRoomBuild, m_bRoomBType, m_bRoomStair, m_bRoomLevel
		, m_bEquip, m_bEquipBuild, m_bEquipBType, m_bEquipStair, m_bEquipLevel
	);


	return CPropertyPage::OnWizardFinish();
}


LRESULT CPropPagePattern::OnWizardNext()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CPropertyPage::OnWizardNext();
}

int CPropPagePattern::MakePattern()
{
	return 0; 
}

DWORD CPropPagePattern::Thread_MakeProc(LPVOID lpData)
{
	CPropPagePattern	*me;
	int nRet = 0;
	me = (CPropPagePattern *)lpData;
	if((CPropPagePattern *)NULL == me)
	{
		me->SendMessage(CSWM_PROGRESS_STEP,0,0);
		return 0;
	}

	try
	{
		//nRet = me->GenerateAutoLinkData();
		nRet = me->MakePattern();
		//nRet = me->GenerateAutoLinkData3();
	}
	catch(...)
	{
	}
	// 	if(nRet == 1)
	// 		me->SendMessage(CSWM_PROGRESS_STEP, 100, PROG_RESULT_FINISH);
	// 	else
	// 		me->SendMessage(CSWM_PROGRESS_STEP, 100, PROG_RESULT_ERROR);
	/*
	*  Thread가 종료 하였음을 설정.
	*/
	return 0;
}
