// ManagerPump.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ManagerPump.h"
#include "DataPump.h"

// CManagerPump

CManagerPump::CManagerPump()
{
}

CManagerPump::~CManagerPump()
{
	RemoveAllData();
}

// CManagerPump 멤버 함수
CDataPump * CManagerPump::GetPumpData(int nFacp, int nPump)
{
	CDataPump *pData;
	POSITION pos;
	pos = GetHeadPosition();
	while (pos)
	{
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetFacpID() == nFacp
			&& pData->GetPumpID() == nPump)
			return pData;
	}
	return nullptr;
}

void CManagerPump::RemoveAllData()
{
	CDataPump * pData;
	while (!IsEmpty())
	{
		pData = RemoveHead();
		if (pData == nullptr)
			continue;

		delete pData;
		pData = nullptr;
	}
}

void CManagerPump::RemoveData(CDataPump* pData)
{
	CDataPump *pTemp;
	POSITION pos , savePos;
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
