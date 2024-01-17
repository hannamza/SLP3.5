#include "stdafx.h"
#include "AllocRelayIndex.h"
#include "RelayTableData.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataDevice.h"
#include "DataSystem.h"
#include "DataLinked.h"
#include "DataPattern.h"
#include "DataPump.h"
#include "DataPS.h"
#include "DataLocation.h"
#include "ManagerPump.h"
#include "ManagerPS.h"
#include "MapSystemData.h"
#include "ManagerFacpRelay.h"
#include "../Common/Utils/YAdoDatabase.h"

CAllocRelayIndex::CAllocRelayIndex(CRelayTableData * pTable, int nAllocType)
{
	m_nAllocType = nAllocType;
	m_pTable = pTable;
	m_nLastIndex = 0;
}


CAllocRelayIndex::~CAllocRelayIndex()
{
}


int CAllocRelayIndex::ProcessAlloc(YAdoDatabase * pDB)
{
	if (m_pTable == nullptr)
		return 0;

	CDataSystem *pOldSys;
	CDataDevice *pOldDev;
	CMapSystemData::iterator it;
	CMapSystemData * pMapOld;
	CString strSql;
	pMapOld = m_pTable->GetSystemData();
	for (it = pMapOld->begin(); it != pMapOld->end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pOldSys = (CDataSystem *)it->second;
		if (pOldSys->GetSysData() == nullptr || pOldSys->GetDataType() != SE_RELAY)
			continue;
		pOldDev = (CDataDevice*)pOldSys->GetSysData();

		if (m_nAllocType == RIDX_ALLOC_PART)
		{
			if (pOldDev->GetRelayIndex() <= 0)
			{
				pOldDev->SetRelayIndex(m_pTable->GetNewRelayIndex());
				m_pTable->IncreaseRelayIndex();
			}
		}
		else
		{
			m_nLastIndex++;
			pOldDev->SetRelayIndex(m_nLastIndex);
		}

		strSql.Format(L"UPDATE TB_RELAY_LIST SET RIDX=%d "
			L" WHERE FACP_ID=%d AND UNIT_ID=%d AND CHN_ID=%d AND RLY_ID=%d "
			, pOldDev->GetRelayIndex()
			, pOldDev->GetFacpID(), pOldDev->GetUnitID(), pOldDev->GetChnID(), pOldDev->GetDeviceID()
		);

		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			USERLOG(L"RelayIndex Update Failed:%s", strSql);
			continue;
		}
	}
	if (m_nAllocType < RIDX_ALLOC_PART)
		m_pTable->SetLastRelayIndex(m_nLastIndex);
	return 1;
}