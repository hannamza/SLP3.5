// DlgMakeAutoAllInput.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgMakeAutoAllInput.h"
#include "afxdialogex.h"
#include "RelayTableData.h"

// CDlgMakeAutoAllInput ��ȭ �����Դϴ�.

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


// CDlgMakeAutoAllInput �޽��� ó�����Դϴ�.

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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewInput()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_pPtrSelInput == nullptr)
		m_pPtrSelInput = new CPtrList;

	m_pPtrSelInput->RemoveAll();
	// [KHS 2020-2-7 16:41:14] 
			// GetRootItem --> TVI_ROOT ��ȯ
			// GetCheckItem(&m_ctrlInTree, m_pPtrSelInput, m_ctrlInTree.GetRootItem());
	GetCheckItem(&m_ctrlInTree, m_pPtrSelInput, TVI_ROOT);
	
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewOutput()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnPreviewLink()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgMakeAutoAllInput::OnBnClickedBtnInSearchInputtype()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnInSearchEquip()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchOutputtype()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CDlgMakeAutoAllInput::OnBnClickedBtnOutSearchEquip()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
