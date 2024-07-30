// PropPagePattern.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPagePattern.h"
#include "afxdialogex.h"
#include "RelayTableData.h"

// CPropPagePattern ��ȭ �����Դϴ�.

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


// CPropPagePattern �޽��� ó�����Դϴ�.
BOOL CPropPagePattern::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
#ifndef ENGLISH_MODE
	m_ctrlPatternList.InsertColumn(0, _T("����"), LVCFMT_CENTER, 50);
	m_ctrlPatternList.InsertColumn(1, _T("���� �̸�"), LVCFMT_CENTER, 200);
	m_ctrlPatternList.InsertColumn(2, _T("����"), LVCFMT_CENTER, 80);
#else
	m_ctrlPatternList.InsertColumn(0, _T("ORDER"), LVCFMT_CENTER, 50);
	m_ctrlPatternList.InsertColumn(1, _T("PATTERN NAME"), LVCFMT_CENTER, 200);
	m_ctrlPatternList.InsertColumn(2, _T("TYPE"), LVCFMT_CENTER, 80);
#endif
	m_ctrlPatternList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CPropPagePattern::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CPropPagePattern::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

BOOL CPropPagePattern::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	//pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	pSheet->SetWizardButtons(PSWIZB_FINISH | PSWIZB_BACK);
	return CPropertyPage::OnSetActive();
}

void CPropPagePattern::OnBnClickedBtnPreview()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CPropertyPage::OnWizardBack();
}


BOOL CPropPagePattern::OnWizardFinish()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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
	*  Thread�� ���� �Ͽ����� ����.
	*/
	return 0;
}
