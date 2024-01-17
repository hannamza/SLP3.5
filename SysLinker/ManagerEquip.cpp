// ManagerEquip.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ManagerEquip.h"
#include "DataEquip.h"

TCHAR *g_szInputType[] = {
	L"자탐감지기",
	L"A교차감지기",
	L"B교차감지기",
	L"일반  감지기",
	L"화재용  접점",
	L"열식아나로그",
	L"연식아나로그",
	L"열식   주소형",
	L"연식   주소형",
	L"감시   비지속",
	L"감시      지속",
	L"일반      확인",
	L"전원      확인",
	L"가스      누출",
	L"사용안함",
	L"AN열식 교차A",
	L"AN열식 교차B",
	L"AN연식 교차A",
	L"AN연식 교차B",
	NULL
};

TCHAR *g_szOutType[] = { 
	L"비상방송",
	L"지 구 벨",
	L"싸 이 렌",
	L"프리액션",
	L"배 연 창",
	L"방 화 문",
	L"방화셧터",
	L"제연뎀퍼",
	L"제연급기휀",
	L"제연배기휀",
	L"소화설비",
	L"전실뎀퍼",
	L"유도등정지",
	NULL
};


TCHAR *g_szOutContents[] = { 
	L"경종",
	L"싸 이 렌",
	L"프리액션",
	L"급기댐퍼",
	L"배기댐퍼",
	L"SMD",
	L"시각경보",
	L"비상등",
	L"P / L",
	L"출입문",
	L"자동폐쇄",
	L"창문폐쇄",
	L"배연창",
	L"급기FAN",
	L"배기FAN",
	L"1차폐쇄",
	L"2차폐쇄",
	L"방화문",
	L"자동문",
	L"옥상문",
	L"비상개폐",
	L"로비폰",
NULL };

// CManagerEquip

CManagerEquip::CManagerEquip()
{
}

CManagerEquip::~CManagerEquip()
{
	CDataEquip * pData;
	while (!IsEmpty())
	{
		pData = RemoveHead();
		if (pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}


// CManagerEquip 멤버 함수


int CManagerEquip::LoadEquipTestData()
{
	CDataEquip * pData = nullptr;
	int i = 0 , nIdx=1; 
	for (i = 0; g_szInputType[i] != NULL; i++)
	{
		if (g_szInputType[i] == NULL)
			continue; 
		pData = new CDataEquip; 
		pData->SetData(nIdx,(int)ET_INPUTTYPE, g_szInputType[i] , L"",L"");

		AddTail(pData);
		nIdx++;
	}

	for (i = 0; g_szOutType[i] != NULL; i++)
	{
		if (g_szOutType[i] == NULL)
			continue;
		pData = new CDataEquip;
		pData->SetData(nIdx, (int)ET_OUTPUTTYPE, g_szOutType[i], L"", L"");
		AddTail(pData);
		nIdx++;
	}

	for (i = 0; g_szOutContents[i] != NULL; i++)
	{
		if (g_szOutType[i] == NULL)
			continue;
		pData = new CDataEquip;
		pData->SetData(nIdx, (int)ET_OUTCONTENTS, g_szOutContents[i], L"", L"");
		AddTail(pData);
		nIdx++;
	}
	return 0;
}


CDataEquip *  CManagerEquip::GetEquip(int nID)
{
	CDataEquip * pData;
	POSITION pos;
	pos = GetHeadPosition();
	while (pos)
	{
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetEquipID() == nID)
			return pData;
	}
	return nullptr;
}


int CManagerEquip::GetEquipCount(int nType)
{
	CDataEquip * pData;
	POSITION pos;
	int nCnt = 0; 
	pos = GetHeadPosition();
	while (pos)
	{
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetEquipID() == nType)
			nCnt++;
	}
	return nCnt;
}


POSITION  CManagerEquip::FindEquip(int nID)
{
	CDataEquip * pData;
	POSITION pos, posSave;
	pos = GetHeadPosition();
	while (pos)
	{
		posSave = pos;
		pData = GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetEquipID() == nID)
			return posSave;
	}
	return nullptr;
}

void  CManagerEquip::SetEquip(int nID
	, int nType
	, CString strName, CString strDesc
	, CString strFileName)
{
	CDataEquip * pData;
	pData = GetEquip(nID);
	if (pData == nullptr)
	{
		pData = new CDataEquip;
		AddTail(pData);
	}
	pData->SetData(nID,nType, strName, strDesc, strFileName);
}


void  CManagerEquip::SetEquip(int nID, CDataEquip *pNew)
{
	CDataEquip * pData;
	pData = GetEquip(nID);
	if (pData == nullptr)
	{
		pData = new CDataEquip;
		AddTail(pData);
	}
	pData->CopyData(pNew);
}

CDataEquip *  CManagerEquip::RemoveEquip(int nID)
{
	CDataEquip * pData;
	POSITION pos = FindEquip(nID);
	if (pos == nullptr)
		return nullptr;
	pData = GetAt(pos);
	RemoveAt(pos);
	return pData;
}


void CManagerEquip::RemoveAllEquip()
{
	CDataEquip * pData;
	while (!IsEmpty())
	{
		pData = RemoveHead();
		if (pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}
