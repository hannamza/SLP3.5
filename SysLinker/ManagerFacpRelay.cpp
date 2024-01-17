#include "stdafx.h"
#include "DataFacpRelay.h"
#include "ManagerFacpRelay.h"


CManagerFacpRelay::CManagerFacpRelay()
{
}


CManagerFacpRelay::~CManagerFacpRelay()
{
	RemoveAllData();
}


// CManagerFacpRelay ¸â¹ö ÇÔ¼ö
CDataFacpRelay * CManagerFacpRelay::GetFacpRelay(int nFacp, int nRID)
{
	CDataFacpRelay *pData;
	POSITION pos;
	pos = GetHeadPosition();
	while (pos)
	{
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetFacpID() == nFacp
			&& pData->GetRelayID() == nRID)
			return pData;
	}
	return nullptr;
}

void CManagerFacpRelay::RemoveAllData()
{
	CDataFacpRelay * pData;
	while (!IsEmpty())
	{
		pData = RemoveHead();
		if (pData == nullptr)
			continue;

		delete pData;
		pData = nullptr;
	}
}


void CManagerFacpRelay::RemoveData(CDataFacpRelay* pData)
{
	CDataFacpRelay *pTemp;
	POSITION pos, savePos;
	pos = GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pTemp = GetNext(pos);
		if (pTemp == nullptr)
			continue;
		if (pTemp->IsEqual(pData))
		{
			RemoveAt(savePos);
		}
	}
}
