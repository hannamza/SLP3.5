// PropPageLocation.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPageLocation.h"
#include "afxdialogex.h"

#include "DataLocation.h"
#include "RelayTableData.h"
#include "DataLocBase.h"

// CPropPageLocation ��ȭ �����Դϴ�.

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


// CPropPageLocation �޽��� ó�����Դϴ�.


BOOL CPropPageLocation::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_ImgLocation.Create(IDB_BMP_DEVICE_ICON, 16, 8, RGB(0, 255, 255));
	m_ctrlLocTree.SetImageList(&m_ImgLocation, TVSIL_NORMAL);


	m_ctrlDeviceList.InsertColumn(0, _T("������/�߰��"), LVCFMT_CENTER, 120);
	m_ctrlDeviceList.InsertColumn(1, _T("���ű�"), LVCFMT_CENTER, 50);
	m_ctrlDeviceList.InsertColumn(2, _T("����"), LVCFMT_CENTER, 50);
	m_ctrlDeviceList.InsertColumn(3, _T("����"), LVCFMT_CENTER, 50);
	m_ctrlDeviceList.InsertColumn(4, _T("ȸ��"), LVCFMT_CENTER, 50);
	m_ctrlDeviceList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CPropPageLocation::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CPropPageLocation::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


BOOL CPropPageLocation::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CPropertyPage::OnWizardNext();
}


void CPropPageLocation::OnNMDblclkLocTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	*pResult = 0;
}

// 
// void CPropPageLocation::OnTvnItemChangedLocTree(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
// 	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
