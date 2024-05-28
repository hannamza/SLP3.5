// PropPageLocation.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPageLocation.h"
#include "afxdialogex.h"

#include "DataLocation.h"
#include "RelayTableData.h"
#include "DataLocBase.h"

// CPropPageLocation 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropPageLocation, CPropertyPage)

CPropPageLocation::CPropPageLocation()
	: CPropertyPage(IDD_PROP_PAGE_LOCATION)
{
	m_pRefFasSysData = nullptr;

}

CPropPageLocation::~CPropPageLocation()
{
}

void CPropPageLocation::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICE_LIST, m_ctrlDeviceList);
	DDX_Control(pDX, IDC_LOC_TREE, m_ctrlLocTree);
}


BEGIN_MESSAGE_MAP(CPropPageLocation, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CPropPageLocation::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropPageLocation::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LOC_TREE, &CPropPageLocation::OnNMClickLocTree)
	ON_NOTIFY(NM_DBLCLK, IDC_LOC_TREE, &CPropPageLocation::OnNMDblclkLocTree)
	//ON_NOTIFY(TVN_ITEMCHANGED, IDC_LOC_TREE, &CPropPageLocation::OnTvnItemChangedLocTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LOC_TREE, &CPropPageLocation::OnTvnSelchangedLocTree)
END_MESSAGE_MAP()


// CPropPageLocation 메시지 처리기입니다.


BOOL CPropPageLocation::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ImgLocation.Create(IDB_BMP_DEVICE_ICON, 16, 8, RGB(0, 255, 255));
	m_ctrlLocTree.SetImageList(&m_ImgLocation, TVSIL_NORMAL);


	m_ctrlDeviceList.InsertColumn(0, _T("감지기/중계기"), LVCFMT_CENTER, 120);
	m_ctrlDeviceList.InsertColumn(1, _T("수신기"), LVCFMT_CENTER, 50);
	m_ctrlDeviceList.InsertColumn(2, _T("유닛"), LVCFMT_CENTER, 50);
	m_ctrlDeviceList.InsertColumn(3, _T("계통"), LVCFMT_CENTER, 50);
	m_ctrlDeviceList.InsertColumn(4, _T("회로"), LVCFMT_CENTER, 50);
	m_ctrlDeviceList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPropPageLocation::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPropPageLocation::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CPropPageLocation::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	FillLocationTree();
	return CPropertyPage::OnSetActive();
}


int CPropPageLocation::FillLocationTree()
{
	m_bInit = FALSE;

	if (m_pRefFasSysData)
		m_pRefFasSysData->FillLocationTree(&m_ctrlLocTree);

	m_bInit = TRUE;
	return 0;
}


void CPropPageLocation::OnNMClickLocTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	HTREEITEM hSel = m_ctrlLocTree.GetSelectedItem();
// 	if (hSel == nullptr)
// 		return;
// 
// 	if (m_pRefFasSysData==nullptr)
// 		return;
// 	CDataLocBase * pLoc; 
// 	pLoc = (CDataLocBase *)m_ctrlLocTree.GetItemData(hSel);
// 	if (pLoc == nullptr)
// 		return;
// 
// 	m_pRefFasSysData->FillDeviceList(&m_ctrlDeviceList, pLoc);
	*pResult = 0;
}


int CPropPageLocation::FillDeviceList(CDataLocBase* pLoc)
{
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceList(&m_ctrlDeviceList, pLoc);
	return 0;
}


LRESULT CPropPageLocation::OnWizardNext()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CPropertyPage::OnWizardNext();
}


void CPropPageLocation::OnNMDblclkLocTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	*pResult = 0;
}

// 
// void CPropPageLocation::OnTvnItemChangedLocTree(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	HTREEITEM hSel = m_ctrlLocTree.GetSelectedItem();
// 	if (hSel == nullptr)
// 		return;
// 
// 	if (m_pRefFasSysData == nullptr)
// 		return;
// 	CDataLocBase * pLoc;
// 	pLoc = (CDataLocBase *)m_ctrlLocTree.GetItemData(hSel);
// 	if (pLoc == nullptr)
// 		return;
// 
// 	m_pRefFasSysData->FillDeviceList(&m_ctrlDeviceList, pLoc);
// 	*pResult = 0;
// }


void CPropPageLocation::OnTvnSelchangedLocTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!m_bInit)
		return;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hSel = pNMTreeView->itemNew.hItem;
	if (hSel == nullptr)
		return;

	if (m_pRefFasSysData == nullptr)
		return;
	CDataLocBase * pLoc;
	pLoc = (CDataLocBase *)m_ctrlLocTree.GetItemData(hSel);
	if (pLoc == nullptr)
		return;

	m_pRefFasSysData->FillDeviceList(&m_ctrlDeviceList, pLoc);

	*pResult = 0;
}
