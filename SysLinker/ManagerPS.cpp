// ManagerPS.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ManagerPS.h"
#include "DataPS.h"
// CManagerPS

CManagerPS::CManagerPS()
{
}

CManagerPS::~CManagerPS()
{
	RemoveAllData();
}


// CManagerPS 멤버 함수
CDataPS * CManagerPS::GetPSData(int nFacp, int nPS)
{
	CDataPS *pData;
	POSITION pos;
	pos = GetHeadPosition();
	while (pos)
	{
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetFacpID() == nFacp
			&& pData->GetPSwitchID() == nPS)
			return pData;
	}
	return nullptr;
}

void CManagerPS::RemoveAllData()
{
	CDataPS * pData;
	while (!IsEmpty())
	{
		pData = RemoveHead();
		if (pData == nullptr)
			continue;

		delete pData;
		pData = nullptr;
	}
}

void CManagerPS::RemoveData(CDataPS* pData)
{
// 	POSITION pos = Find(pData);
// 	if (pos == nullptr)
// 		return;
// 	RemoveAt(pos);
	CDataPS *pTemp;
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
