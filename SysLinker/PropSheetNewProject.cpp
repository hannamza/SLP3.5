// PropSheetNewProject.cpp : 구현 파일입니다.
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


// CPropSheetNewProject 메시지 처리기입니다.


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
	if (m_pFasSysData == nullptr)
	{
		m_pFasSysData = new CRelayTableData;
	}

	int nCnt, i; 
	CString str;
	nCnt = m_pgDevice.m_lstTable.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pgDevice.m_lstTable.GetText(i, str);
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
