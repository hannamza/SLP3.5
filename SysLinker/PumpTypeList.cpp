#include "stdafx.h"
#include "PumpTypeList.h"
#include <algorithm>


CPumpTypeList::CPumpTypeList()
{
}


CPumpTypeList::~CPumpTypeList()
{
}

BOOL CPumpTypeList::AddSoted(ST_PUMPTYPE * pNew)
{
	int nCnt;
	ST_PUMPTYPE * pData;
	POSITION pos;
	nCnt = (int)GetCount();
	if(nCnt == 0)
	{
		AddTail(pNew);
		return TRUE;
	}


	int nCurPos,nComp,nStart,nEnd;
	nEnd = nCnt;
	nCurPos = nCnt - 1;
	nStart = 0;
	do
	{
		pos = FindIndex(nCurPos);
		if(pos == nullptr)
			return FALSE;

		pData = (ST_PUMPTYPE*)GetAt(pos);
		nComp = pData->CompareTypeName(TRUE,pNew);
		if(nComp != 0)
		{
			if(nComp < 0)
				nStart = nCurPos + 1;
			else// if(nComp > 0)
				nEnd = nCurPos;
			nCurPos = (nEnd + nStart) / 2;
		}
		else
		{
			return FALSE;
		}
	} while((nStart != nEnd) && (nComp != 0));

	if(nComp < 0)
	{
		InsertAfter(pos,pNew);
	}
	else if(nComp > 0)
		InsertBefore(pos,pNew);
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CPumpTypeList::AddSoted(int nPmpType,CString strTypeName,CString strLcdName,int nPsType,int nPsUse)
{
	int nCnt;
	ST_PUMPTYPE * pData,* pNew;
	POSITION pos;
	nCnt = (int)GetCount();
	if(nCnt == 0)
	{
		pNew = new ST_PUMPTYPE;
		pNew->SetPumpType(nPmpType,strTypeName,strLcdName,nPsType,nPsUse);
		AddTail(pNew);
		return TRUE;
	}

	
	int nCurPos,nComp,nStart,nEnd;
	nEnd = nCnt;
	nCurPos = nCnt - 1;
	nStart = 0;
	pNew = new ST_PUMPTYPE;
	pNew->SetPumpType(nPmpType,strTypeName,strLcdName,nPsType,nPsUse);

	do
	{
		pos = FindIndex(nCurPos);
		if(pos == nullptr)
			return FALSE;

		pData = (ST_PUMPTYPE*)GetAt(pos);
		nComp = pData->CompareTypeName(TRUE,pNew);
		if(nComp != 0)
		{
			if(nComp < 0)
				nStart = nCurPos + 1;
			else// if(nComp > 0)
				nEnd = nCurPos;
			nCurPos = (nEnd + nStart) / 2;
		}
		else
		{
			return FALSE;
		}
	} while((nStart != nEnd) && (nComp != 0));
	
	if(nComp < 0)
	{
		InsertAfter(pos,pNew);
	}
	else if(nComp > 0)
		InsertBefore(pos,pNew);
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CPumpTypeList::Sort(BOOL bAsc)
{
	ST_PUMPTYPE * pData;
	std::vector<ST_PUMPTYPE*> vt;
	POSITION pos;
	int i;
	pos = GetHeadPosition();
	while(pos)
	{
		pData = (ST_PUMPTYPE *)GetNext(pos);
		if(pData == nullptr)
			continue; 

		vt.push_back(pData);
	}
	RemoveAll();
	if(bAsc)
		sort(vt.begin(),vt.end(),CompAsc);
	else
		sort(vt.begin(),vt.end(),CompDsc);

	for(i = (int)vt.size() - 1; i >= 0; i --)
	{
		pData = vt[i];
		AddTail(pData);
	}
	return TRUE;
}

bool CPumpTypeList::CompAsc(ST_PUMPTYPE * p1,ST_PUMPTYPE * p2)
{
	if(p1 == nullptr || p2 == nullptr)
		return false;
	return p1->CompareTypeName(TRUE , p2);
}

bool CPumpTypeList::CompDsc( ST_PUMPTYPE * p1, ST_PUMPTYPE * p2)
{
	if(p1 == nullptr || p2 == nullptr)
		return false;
	return p1->CompareTypeName(FALSE,p2);
	//return p1->strTypeName.CompareNoCase(p2->strTypeName) < 0;
}