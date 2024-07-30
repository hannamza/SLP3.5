// DlgMakeAutoAllInput.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgMakeAutoAllInput.h"
#include "afxdialogex.h"
#include "RelayTableData.h"

// CDlgMakeAutoAllInput 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMakeAutoAllInput, CDialogEx)

#ifndef ENGLISH_MODE
CDlgMakeAutoAllInput::CDlgMakeAutoAllInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_MAKEAUTO_ALLINPUT, pParent)
	, m_bMatchBuild(FALSE)
	, m_bMatchBtype(FALSE)
	, m_bMatchEquip(FALSE)
	, m_bMatchFloor(FALSE)
	, m_bMatchRoom(FALSE)
	, m_bMatchStair(FALSE)
{
	m_pRefFasSysData = nullptr;
	m_pPtrInputCondList = nullptr;
	m_pPtrOutputCondList = nullptr;
	m_pPtrSelOutput = nullptr;
	m_pPtrSelInput = nullptr;
}
#else
CDlgMakeAutoAllInput::CDlgMakeAutoAllInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_MAKEAUTO_ALLINPUT_EN, pParent)
	, m_bMatchBuild(FALSE)
	, m_bMatchBtype(FALSE)
	, m_bMatchEquip(FALSE)
	, m_bMatchFloor(FALSE)
	, m_bMatchRoom(FALSE)
	, m_bMatchStair(FALSE)
{
	m_pRefFasSysData = nullptr;
	m_pPtrInputCondList = nullptr;
	m_pPtrOutputCondList = nullptr;
	m_pPtrSelOutput = nullptr;
	m_pPtrSelInput = nullptr;
}
#endif

CDlgMakeAutoAllInput::~CDlgMakeAutoAllInput()
{
	RemoveTreeListData(m_pPtrOutputCondList);
	RemoveTreeListData(m_pPtrInputCondList);
	delete m_pPtrOutputCondList;
	m_pPtrOutputCondList = nullptr;
	delete m_pPtrInputCondList;
	m_pPtrInputCondList = nullptr;

}

void CDlgMakeAutoAllInput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHKP_MATCH_BUILD, m_bMatchBuild);
	DDX_Check(pDX, IDC_CHKP_MATCH_BTYPE, m_bMatchBtype);
	DDX_Check(pDX, IDC_CHKP_MATCH_EQUIP, m_bMatchEquip);
	DDX_Check(pDX, IDC_CHKP_MATCH_FLOOR, m_bMatchFloor);
	DDX_Check(pDX, IDC_CHKP_MATCH_ROOM, m_bMatchRoom);
	DDX_Check(pDX, IDC_CHKP_MATCH_STAIR, m_bMatchStair);
	DDX_Control(pDX, IDC_IN_TREE, m_ctrlInTree);
	DDX_Control(pDX, IDC_OUT_TREE, m_ctrlOutTree);
}


BEGIN_MESSAGE_MAP(CDlgMakeAutoAllInput, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_INPUT, &CDlgMakeAutoAllInput::OnBnClickedBtnPreviewInput)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_OUTPUT, &CDlgMakeAutoAllInput::OnBnClickedBtnPreviewOutput)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_LINK, &CDlgMakeAutoAllInput::OnBnClickedBtnPreviewLink)
	ON_BN_CLICKED(IDC_BTN_IN_SEARCH_INPUTTYPE, &CDlgMakeAutoAllInput::OnBnClickedBtnInSearchInputtype)
	ON_BN_CLICKED(IDC_BTN_IN_SEARCH_EQUIP, &CDlgMakeAutoAllInput::OnBnClickedBtnInSearchEquip)
	ON_BN_CLICKED(IDC_BTN_OUT_SEARCH_OUTPUTTYPE, &CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchOutputtype)
	ON_BN_CLICKED(IDC_BTN_OUT_SEARCH_EQUIP, &CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchEquip)
END_MESSAGE_MAP()


// CDlgMakeAutoAllInput 메시지 처리기입니다.

int CDlgMakeAutoAllInput::RemoveTreeListData(CPtrList * pList)
{
	if (pList == nullptr)
		return 0;

	ST_TREEITEM * pItem;
	while (!pList->IsEmpty())
	{
		pItem = (ST_TREEITEM *)pList->RemoveHead();
		if (pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
	return 1;
}


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
	
	m_pPtrInputCondList = new CPtrList;
	m_pPtrOutputCondList = new CPtrList;

	if (m_pRefFasSysData)
	{
		m_pRefFasSysData->FillConditionTree(&m_ctrlInTree , m_pPtrInputCondList , TRUE);
		m_pRefFasSysData->FillConditionTree(&m_ctrlOutTree, m_pPtrOutputCondList, FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewInput()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pPtrSelInput == nullptr)
		m_pPtrSelInput = new CPtrList;

	m_pPtrSelInput->RemoveAll();
	// [KHS 2020-2-7 16:41:14] 
			// GetRootItem --> TVI_ROOT 변환
			// GetCheckItem(&m_ctrlInTree, m_pPtrSelInput, m_ctrlInTree.GetRootItem());
	GetCheckItem(&m_ctrlInTree, m_pPtrSelInput, TVI_ROOT);
	
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewOutput()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewLink()
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


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchOutputtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchEquip()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



int CDlgMakeAutoAllInput::GetTreeCheckList(CTreeCtrl* pCtrl)
{

	return 0;
}


int CDlgMakeAutoAllInput::GetCheckItem(CTreeCtrl* pCtrl, CPtrList* pCheckList, HTREEITEM hCurrent)
{
	ST_TREEITEM * pItem = nullptr;
	HTREEITEM hChild, hSibling; 
	hChild = hSibling = nullptr;

	while (hCurrent)
	{
		if (pCtrl->GetCheck(hCurrent))
		{
			pItem = (ST_TREEITEM*)pCtrl->GetItemData(hCurrent);
			if (pItem)
				pCheckList->AddTail(pItem);
		}

		if (pCtrl->ItemHasChildren(hCurrent))
		{
			hChild = pCtrl->GetChildItem(hCurrent);
			GetCheckItem(pCtrl, pCheckList, hChild);
		}
		hCurrent = pCtrl->GetNextSiblingItem(hCurrent);
	}
	return 1;
}
