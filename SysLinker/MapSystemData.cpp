// MapSystemData.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "MapSystemData.h"
#include "DataDevice.h"

// CMapSystemData

CMapSystemData::CMapSystemData()
{
}

CMapSystemData::~CMapSystemData()
{
}


// CMapSystemData 멤버 함수

// 
// CDataDevice * CMapSystemData::GetDeviceData(
// 	int nType
// 	,int nFacpID, int nUnitID, int nChnID, int nRelayID
// )
// {
// 	CDataSystem * pData;
// 	CDataDevice * pDev;
// 	CMapSystemData::iterator it;
// 	for (it = begin(); it != end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 
// 		if (pData->GetDataType() != SE_RELAY)
// 			continue;
// 		if (pData->GetSysData() == nullptr)
// 			continue;
// 
// 		pDev = (CDataDevice *)pData->GetSysData();
// 
// 		if (pDev->GetUnitID() == nUnitID && pDev->GetFacpID() == nFacpID
// 			&& pDev->GetChnID() == nChnID && pDev->GetDeviceID() == nRelayID)
// 			return pDev;
// 	}
// 	return nullptr;
// }

CMapIDSystemData::CMapIDSystemData()
{
}

CMapIDSystemData::~CMapIDSystemData()
{
}