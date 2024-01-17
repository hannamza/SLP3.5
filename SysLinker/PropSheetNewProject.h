#pragma once


#include "PropPageProjectInfo.h"
#include "PropPageDevice.h"
#include "PropPageFacp.h"
#include "PropPageLocation.h"
#include "PropPagePattern.h"
#include "PropPageEmergency.h"
// CPropSheetNewProject

class CRelayTableData;
class CPropSheetNewProject : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropSheetNewProject)

public:
	CPropSheetNewProject(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropSheetNewProject(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPropSheetNewProject();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CPropPageProjectInfo		m_pgPrjInfo;
	CPropPageDevice				m_pgDevice;
	CPropPageFacp				m_pgFacp;
	CPropPageLocation			m_pgLocation;
	CPropPagePattern			m_pgPattern;
	CPropPageEmergency			m_pgEmergency;


	CRelayTableData		*		m_pFasSysData; /// 중계기 일람표의 모든데이터
	int InitPage();
	int ProcessDeviceTable();
	void SetFasSysData(CRelayTableData * pData);
	CRelayTableData * GetFasSysData() {
		return m_pFasSysData;
	};
};


