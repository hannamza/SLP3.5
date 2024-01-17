// ManagerEquip.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ManagerEquip.h"
#include "DataEquip.h"

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
