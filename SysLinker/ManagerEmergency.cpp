#include "stdafx.h"
#include "DataEmBc.h"
#include "ManagerEmergency.h"


CManagerEmergency::CManagerEmergency()
{
}


CManagerEmergency::~CManagerEmergency()
{
}


CDataEmBc* CManagerEmergency::GetEmergency(CString strBuild, CString strStair, CString strFloor)
{
	CDataEmBc * pEm = nullptr;
	POSITION pos; 
	CString sB, sS, sF;
	CString tB, tS, tF;
	tB = strBuild;
	tS = strStair;
	tF = strFloor;
	tB.Remove(' ');
	tS.Remove(' ');
	tF.Remove(' ');

	pos = GetHeadPosition();
	while (pos)
	{
		pEm = GetNext(pos);
		if (pEm == nullptr)
			continue;

		sB = pEm->GetBuildName();
		sB.Remove(' ');
		sS = pEm->GetStairName();
		sS.Remove(' ');
		sF = pEm->GetFloorName();
		sF.Remove(' ');

		if (sB.CompareNoCase(tB) != 0)
			continue;
		if (sS.CompareNoCase(tS) != 0)
			continue;
		if (sF.CompareNoCase(tF) != 0)
			continue;

		// 		if ((strBuild.CompareNoCase(pEm->GetBuildName()) == 0)
		// 			&& (strStair.CompareNoCase(pEm->GetStairName()) == 0)
		// 			&& (strFloor.CompareNoCase(pEm->GetFloorName()) == 0))
		return pEm;
	}
	return nullptr;
}


CDataEmBc* CManagerEmergency::RemoveEmergency(int nID)
{
	CDataEmBc * pData;
	POSITION pos = FindEmergency(nID);
	if (pos == nullptr)
		return nullptr;
	pData = GetAt(pos);
	RemoveAt(pos);
	return pData;
}


CDataEmBc* CManagerEmergency::GetEmergency(int nID)
{
	CDataEmBc * pData;
	POSITION pos;
	pos = GetHeadPosition();
	while (pos)
	{
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetEmID() == nID)
			return pData;
	}
	return nullptr;
}



POSITION CManagerEmergency::FindEmergency(int nID)
{
	CDataEmBc * pData;
	POSITION pos, posSave;
	pos = GetHeadPosition();
	while (pos)
	{
		posSave = pos;
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetEmID() == nID)
			return posSave;
	}
	return nullptr;
}



void CManagerEmergency::RemoveAllEmergency()
{
	CDataEmBc * pData;
	while (!IsEmpty())
	{
		pData = RemoveHead();
		if (pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}