// PropSheetNewProject.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropSheetNewProject.h"

#include "RelayTableData.h"
// CPropSheetNewProject

IMPLEMENT_DYNAMIC(CPropSheetNewProject, CPropertySheet)

CPropSheetNewProject::CPropSheetNewProject(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pFasSysData = nullptr;
}

CPropSheetNewProject::CPropSheetNewProject(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

CPropSheetNewProject::~CPropSheetNewProject()
{
}


BEGIN_MESSAGE_MAP(CPropSheetNewProject, CPropertySheet)
END_MESSAGE_MAP()


// CPropSheetNewProject �޽��� ó�����Դϴ�.


int CPropSheetNewProject::InitPage()
{
	AddPage(&m_pgPrjInfo);
	AddPage(&m_pgDevice);
	AddPage(&m_pgFacp);
	AddPage(&m_pgLocation);
	AddPage(&m_pgPattern);
	//AddPage(&m_pgEmergency);
	//SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT | PSWIZB_FINISH | PSWIZB_CANCEL);
	SetWizardMode();

	return 0;
}

int CPropSheetNewProject::ProcessDeviceTable()
{
	//AfxMessageBox(L"ReadDeviceTable");

	//20240527 GBM start - �޸� ���� ����
#if 1
	if (m_pFasSysData == nullptr)
	{
		m_pFasSysData = new CRelayTableData;
	}
	else
	{
		//�ڷ� ���ٰ� �ٽ� ������ �� ��� �޸� ����
		m_pFasSysData->m_strFileNameList.RemoveAll();
		m_pFasSysData->RemoveAllData();
		memset(&CNewInfo::Instance()->m_ei, NULL, sizeof(EQUIPMENT_INFO));
		memset(&CNewInfo::Instance()->m_gi, NULL, sizeof(GT1APPENDIX_INFO));
	}
#else
	if (m_pFasSysData == nullptr)
	{
		m_pFasSysData = new CRelayTableData;
	}
#endif
	//20240527 GBM end

	int nCnt, i; 
	CString str;
	nCnt = m_pgDevice.m_ctrlListCtrl.GetItemCount();
	for (i = 0; i < nCnt; i++)
	{
		str = m_pgDevice.m_ctrlListCtrl.GetItemText(i, 0);
		m_pFasSysData->AddDeviceTable(str);
	}
	m_pFasSysData->ProcessDeviceTableList();

	m_pgFacp.InitFacpList();
	return 0;
}

void CPropSheetNewProject::SetFasSysData(CRelayTableData * pData)
{
	m_pFasSysData = pData;
	m_pgPrjInfo.SetFasSysData(pData);
	m_pgDevice.SetFasSysData(pData);
	m_pgFacp.SetFasSysData(pData);
	m_pgLocation.SetFasSysData(pData);
	m_pgPattern.SetFasSysData(pData);
	m_pgEmergency.SetFasSysData(pData);
}


BOOL CPropSheetNewProject::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// TODO:  ���⿡ Ư��ȭ�� �ڵ带 �߰��մϴ�.
#ifdef ENGLISH_MODE
	CWnd* pBackBtn = GetDlgItem(ID_WIZBACK);
	CWnd* pNextBtn = GetDlgItem(ID_WIZNEXT);
	CWnd* pCancelBtn = GetDlgItem(IDCANCEL);

	if (pBackBtn != nullptr)
	{
		pBackBtn->SetWindowText(_T("Back"));
	}

	if (pNextBtn != nullptr)
	{
		pNextBtn->SetWindowText(_T("Next"));
	}

	if (pCancelBtn != nullptr)
	{
		pCancelBtn->SetWindowText(_T("Cancel"));
	}
#endif

	return bResult;
}
