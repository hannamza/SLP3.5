// ManagerEquip.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ManagerEquip.h"
#include "DataEquip.h"

#ifndef ENGLISH_MODE
TCHAR *g_szInputType[] = {
	L"��Ž������",
	L"A����������",
	L"B����������",
	L"�Ϲ�  ������",
	L"ȭ���  ����",
	L"���ľƳ��α�",
	L"���ľƳ��α�",
	L"����   �ּ���",
	L"����   �ּ���",
	L"����   ������",
	L"����      ����",
	L"�Ϲ�      Ȯ��",
	L"����      Ȯ��",
	L"����      ����",
	L"������",
	L"AN���� ����A",
	L"AN���� ����B",
	L"AN���� ����A",
	L"AN���� ����B",
	NULL
};
#else
TCHAR *g_szInputType[] = {
	L"SELF-DETECTOR",
	L"A CROSS DETECTOR",
	L"B CROSS DETECTOR",
	L"GENERAL DETECTOR",
	L"FIRE RELAY CONTACT",
	L"HEAT-BASED ANALOG",
	L"SMOKE-BASED ANALOG",
	L"HEAT-BASED ADDRESS TYPE",
	L"SMOKE-BASED ADDRESS TYPE",
	L"SUPERVISORY (UNLATCH)",
	L"SUPERVISORY (LATCH)",
	L"CONFIRM",
	L"CONFIRM POWER",
	L"GAS LEAKS",
	L"NOT USED",
	L"AN HEAT-BASED CROSS A",
	L"AN HEAT-BASED CROSS B",
	L"AN SMOKE-BASED CROSS A",
	L"AN SMOKE-BASED CROSS B",
	NULL
};
#endif

#ifndef ENGLISH_MODE
TCHAR *g_szOutType[] = { 
	L"�����",
	L"�� �� ��",
	L"�� �� ��",
	L"�����׼�",
	L"�� �� â",
	L"�� ȭ ��",
	L"��ȭ����",
	L"��������",
	L"�����ޱ���",
	L"���������",
	L"��ȭ����",
	L"���ǵ���",
	L"����������",
	NULL
};
#else
TCHAR *g_szOutType[] = {
	L"PUBLIC ADDRESS",
	L"ZONE BELL",
	L"SIREN",
	L"PRE-ACTION VALVE",
	L"SMOKE VENT WINDOW",
	L"FIRE DOOR",
	L"FIRE SHUTTER",
	L"SMOKE DAMPER",
	L"SUPPLY FAN",
	L"EXHAUST FAN",
	L"FIRE HYDRANT",
	L"SMOKE CONTROL DAMPER",
	L"EMERGENCY EXIT SIGN OFF",
	NULL
};
#endif

#ifndef ENGLISH_MODE
TCHAR *g_szOutContents[] = { 
	L"����",
	L"�� �� ��",
	L"�����׼�",
	L"�ޱ����",
	L"������",
	L"SMD",
	L"�ð��溸",
	L"����",
	L"P / L",
	L"���Թ�",
	L"�ڵ����",
	L"â�����",
	L"�迬â",
	L"�ޱ�FAN",
	L"���FAN",
	L"1�����",
	L"2�����",
	L"��ȭ��",
	L"�ڵ���",
	L"����",
	L"�����",
	L"�κ���",
NULL };
#else
TCHAR *g_szOutContents[] = {
	L"BELL",
	L"SIREN",
	L"PRE-ACTION VALVE",
	L"SUPPLY SMOKE DAMPER",
	L"EXHAUST DAMPER",
	L"SMD",
	L"STROBE",
	L"EMERGENCY LIGHT",
	L"P / L",
	L"DOOR",
	L"AUTOMATIC DOOR CLOSER",
	L"SMOKE WINDOW CLOSE",
	L"SMOKE WINDOW",
	L"SUPPLY FAN",
	L"EXHAUST FAN",
	L"1ST CLOSURE",
	L"2ND CLOSURE",
	L"FIRE DOOR",
	L"AUTOMATIC DOOR",
	L"ROOF DOOR",
	L"EMERGENCY OPEN/CLOSE",
	L"LOBBY PHONE",
	NULL };
#endif

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


// CManagerEquip ��� �Լ�


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
