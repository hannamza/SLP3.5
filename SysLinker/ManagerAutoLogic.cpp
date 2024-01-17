// ManagerAutoLogic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ManagerAutoLogic.h"
#include "DataAutoLogic.h"

// CManagerAutoLogic

CManagerAutoLogic::CManagerAutoLogic()
{
}

CManagerAutoLogic::~CManagerAutoLogic()
{
}


// CManagerAutoLogic 멤버 함수
CDataAutoLogic * CManagerAutoLogic::RemoveLogic(int nID)
{
	CDataAutoLogic * pData = nullptr;
	POSITION pos = FindLogic(nID);
	if (pos == nullptr)
		return nullptr;
	pData = GetAt(pos);
	RemoveAt(pos);
	return pData;

}

POSITION CManagerAutoLogic::FindLogic(int nID)
{
	CDataAutoLogic* pData;
	POSITION pos, posSave;
	pos = GetHeadPosition();
	while (pos)
	{
		posSave = pos;
		pData = GetNext(pos);
		if (pData == nullptr)
			continue; 
		if (pData->GetLgId() == nID)
			return posSave;
	}
	return nullptr;
}
CDataAutoLogic * CManagerAutoLogic::GetLogic(int nID)
{
	CDataAutoLogic* pData;
	POSITION pos, posSave;
	pos = GetHeadPosition();
	while (pos)
	{
		posSave = pos;
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetLgId() == nID)
			return pData;
	}
	return nullptr;
}

void CManagerAutoLogic::RemoveAllLogic()
{
	CDataAutoLogic* pData;
	while (!IsEmpty())
	{
		pData = RemoveHead();
		if (pData == nullptr)
			continue; 
		delete pData;
		pData = nullptr;
	}
}
