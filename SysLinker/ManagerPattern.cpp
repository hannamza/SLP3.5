// ManagerPattern.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ManagerPattern.h"
#include "DataPattern.h"

// CManagerPattern

CManagerPattern::CManagerPattern()
{
}

CManagerPattern::~CManagerPattern()
{
}


BOOL CManagerPattern::SortedInsertAfterPosition(CDataPattern * pPtn)
{
	CDataPattern * pTemp;
	POSITION pos;
	int nCurPos,nComp , nStart,nEnd;
	int nCnt = GetCount();
	if(nCnt == 0)
	{
		AddTail(pPtn);
		return TRUE;
	}
// 	pos = FindIndex(nCnt - 1);
// 	
// 	if(pos == nullptr)
// 		return FALSE;
// 	pTemp = GetAt(pos);
// 	if(pTemp->GetPatternID() <= pPtn->GetPatternID())
// 	{
// 		AddTail(pPtn);
// 		return TRUE;
// 	}

	nEnd = nCnt; 
	nCurPos = nCnt - 1;
	nStart = 0;
	do
	{
		pos = FindIndex(nCurPos);
		if(pos == nullptr)
			return FALSE;

		pTemp = GetAt(pos);
		nComp = pTemp->ComparePtn(pPtn);
		if(nComp != 0)
		{
			if(nComp < 0)
				nStart = nCurPos + 1;
			else// if(nComp > 0)
				nEnd = nCurPos;
			nCurPos = (nEnd + nStart) / 2;
		}
	} while((nStart != nEnd) && (nComp != 0));

	if(nComp < 0)
	{
		InsertAfter(pos,pPtn);
	}
	else if(nComp > 0)
		InsertBefore(pos,pPtn);
	else
	{
		return FALSE;
	}
	return TRUE;
}
// CManagerPattern 멤버 함수
CDataPattern * CManagerPattern::AddPattern(CString strPattern,int nType,BYTE * pLocMake,int nManualMake,int nPatternID)
{
	CDataPattern * pData = nullptr;
	CString strtemp,strRepName;
	BOOL bFind = FALSE;
	strtemp = strPattern;
	strtemp.Remove(' ');
	if(strtemp == L"")
		return nullptr;
	USERLOG(L"Request Pattern List Add : %s\n",strPattern);
	pData = FindPatternData(strPattern);
	// [2022/11/14 18:48:54 KHS] 
	// 중복시 프로그램에서 표시 되지 않아서 운영자가 중복 패턴이 있는지 인지 하지 못한다.
	// 중복되는 패턴은 중복 되었다고 표시 해주어야 한다.
	// 아래는 코드는 중복 표시 안하는 부분
  	if(pData == nullptr)
  	{
  		USERLOG(L"New Pattern List Add : %s\n",strPattern);
  		pData = new CDataPattern;
  		if(nPatternID > 0)
  		{
  			pData->SetPatternID(nPatternID);
  		}
  		else
  		{
  			pData->SetPatternID(GetWholePatternID());
  		}
  		pData->SetPatternName(strPattern);
		pData->SetPatternType(nType);
		pData->SetManualMake(nManualMake);
  		if(pLocMake != nullptr)
  			pData->SetMakeLocArray(pLocMake);
  		
  		// Sort 해서 넣는다.
  		//AddTail(pData);
  		SortedInsertAfterPosition(pData);
  	}
	else
	{
		// [2022/11/15 10:09:42 KHS] 
		// 중복 확인 - DB에서 데이터를 가져올때 이름이 같은 패턴은 List에 입력하지 않아 사용자가 중복을 알 수 없었는데
		// 이름은 같고 아이디가 틀린것을 표현하기 위해 넣음
		if(nPatternID > 0)
		{
			strRepName.Format(L"★패턴이름중복:%03d★ %s",pData->GetPatternID(),strPattern);
			USERLOG(L"중복 패턴 아이템 : %s\n",strRepName);
			pData = new CDataPattern;
			pData->SetPatternID(nPatternID);
			pData->SetPatternName(strRepName);
			pData->SetPatternType(nType);
			pData->SetManualMake(nManualMake);
			if(pLocMake != nullptr)
				pData->SetMakeLocArray(pLocMake);

			// Sort 해서 넣는다.
			//AddTail(pData);
			SortedInsertAfterPosition(pData);

		}
	}
	return pData;
}


// CDataPattern * CManagerPattern::AddPattern(CString strPattern, int nType, BYTE * pLocMake, int nPatternID)
// {
// 	CDataPattern * pData = nullptr;
// 	CString strtemp;
// 	POSITION pos;
// 	strtemp = strPattern;
// 	strtemp.Remove(' ');
// 	if (strtemp == L"")
// 		return nullptr;
// 	pos = FindPatternData(nPatternID);
// 	if (pos == nullptr)
// 	{
// 		USERLOG(L"Pattern List Add : %s\n", strPattern);
// 		pData = new CDataPattern;
// 		if (nPatternID > 0)
// 		{
// 			pData->SetPatternID(nPatternID);
// 		}
// 		else
// 		{
// 			pData->SetPatternID(GetWholePatternID());
// 		}
// 		pData->SetPatternName(strPattern);
// 		pData->SetPatternType(nType);
// 		if(pLocMake != nullptr)
// 			pData->SetMakeLocArray(pLocMake);
// 		AddTail(pData);
// 	}
// 
// 	return pData;
// }

CDataPattern * CManagerPattern::FindPatternData(CString strPattern)
{
	CDataPattern * pPtn;
	POSITION pos;
	pos = GetHeadPosition();
	while (pos)
	{
		pPtn = GetNext(pos);
		if (pPtn == nullptr)
			continue;
		if (strPattern.CompareNoCase(pPtn->GetPatternName()) != 0)
			continue;
		return pPtn;
	}
	return nullptr;
}


int CManagerPattern::GetWholePatternID()
{
	int nPrev, nNew , nRet =0;
	CDataPattern * pPtn = nullptr;
	POSITION pos;
	nPrev = nNew = 0;
	pos = GetHeadPosition();
	while (pos)
	{
		pPtn = GetNext(pos);
		if (pPtn == nullptr)
			continue;
		nNew = (int)pPtn->GetPatternID();
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	return nRet;
}


int CManagerPattern::GetMaxPatternID()
{
	int nID , nLastID = 0;
	CDataPattern * pPtn = nullptr;
	POSITION pos;
	pos = GetHeadPosition();
	nID = nLastID = 0;
	while (pos)
	{
		pPtn = GetNext(pos);
		if (pPtn == nullptr)
			continue;
		nID = (int)pPtn->GetPatternID();
		if (nLastID < nID)
			nLastID = nID;
	}
	return nLastID + 1;
}


POSITION CManagerPattern::FindPatternData(int nPtnID)
{
	CDataPattern * pPtn;
	POSITION pos , savePos;
	pos = GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pPtn = GetNext(pos);
		if (pPtn == nullptr)
			continue;
		if(pPtn->GetPatternID() == nPtnID)
			return savePos;
	}
	return nullptr;
}

CDataPattern * CManagerPattern::GetPattern(int nPtnID)
{
	POSITION pos;
	pos = FindPatternData(nPtnID);
	if (pos == nullptr)
		return nullptr;
	
	return GetAt(pos);
}

BOOL CManagerPattern::DeletePattern(CDataPattern * pPtn)
{
	POSITION pos;
	pos = FindPatternData((int)pPtn->GetPatternID());
	if (pos == nullptr)
		return FALSE;

	RemoveAt(pos);
	
	delete pPtn;
	pPtn = nullptr;
	return TRUE;
}


int CManagerPattern::GetAllPatternItemCount()
{
	int nRet = 0 , nTemp;
	CDataPattern * pPtn = nullptr;
	POSITION pos;
	pos = GetHeadPosition();
	while (pos)
	{
		pPtn = GetNext(pos);
		if (pPtn == nullptr)
			continue;
		nTemp = pPtn->GetItemCount();
		nRet += nTemp;
	}
	return nRet;
}
