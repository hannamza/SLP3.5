#include "stdafx.h"
#include "SysLinker.h"
#include "DataDevice.h"
#include "XMakeLink_Def.h"
#include "XDataStair.h"
#include "XDataDev.h"
#include "XDataFloor.h"
#include "XDataLogicMst.h"
#include "XDataLogicItem.h"
#include "XListFloor.h"
#include "XDataRangeLogic.h"

CXDataStair::CXDataStair()
{
	m_bInputItem = FALSE;
	m_strName = L"";
	m_nId = -1;
	m_pListFloor = nullptr;
	m_pParent = nullptr;

	m_nNameIndex = -1;
	m_nBuildIndex = -1;
	m_nBtypeIndex = -1;
}


CXDataStair::~CXDataStair()
{
	RemoveAllData();
	if(m_pListFloor)
	{
		delete m_pListFloor;
		m_pListFloor = nullptr;
	}
}

void CXDataStair::RemoveAllData()
{
	if(m_pListFloor == nullptr)
		return;
	CXDataFloor * pData;
	while(m_pListFloor->IsEmpty() == FALSE)
	{
		pData = m_pListFloor->RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

// CXDataStair 멤버 함수

int CXDataStair::MakeLocData(CXDataBtype * pParent,CXDataDev * pPnt,BOOL bInputItem /* = TRUE */)
{
	CDataLocBase * pLoc = nullptr;
	m_bInputItem = bInputItem;
	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return 0;
	m_pParent = pParent;
	m_strName = pLoc->GetStairName();
	m_nId = pLoc->GetStairID();
	return 1;
}

int CXDataStair::AddDeviceData(CXDataDev * pPnt,BOOL bInputItem)
{
	if(m_pListFloor == nullptr)
		m_pListFloor = new CXListFloor;
	m_pListFloor->SortedAddData(this,pPnt,bInputItem);
	return 1;
}


int CXDataStair::CompareData(CDataDevice * pPnt)
{
	int nRet = 0;
	CString strtemp;
	CDataLocBase * pLoc = nullptr;

	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return -1;
	strtemp = pLoc->GetStairName();
	nRet = m_strName.CompareNoCase(strtemp);
	return nRet;
}

int CXDataStair::CompareData(CXDataDev * pPnt)
{
	int nRet = 0;
	CString strtemp;
	CDataLocBase * pLoc = nullptr;

	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return -1;
	strtemp = pLoc->GetStairName();
	nRet = m_strName.CompareNoCase(strtemp);
	return nRet;
}
int CXDataStair::CompareData(int nIndex)
{
	if(m_nNameIndex == nIndex)
		return 0;
	else if(m_nNameIndex > nIndex)
		return 1;
	else
		return -1;
}

BOOL CXDataStair::CheckBasicLogicMatch(
	CXDataDev * pInDev,CXMapLink * pDevList,CXDataFloor *pFloor,CXDataLogicMst * pMst)
{
	int nTgtFlNum,nSrcFlNum;
	CXDataLogicItem * pItem; 

	if(pFloor == nullptr || pMst == nullptr)
		return FALSE;
	pItem = pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID];
	nTgtFlNum = pFloor->GetFloorNumber();
	nSrcFlNum = pInDev->GetLocFloorNumber();

	if(pItem->GetUseUnderLogic() == 0)
	{
		if(pItem->MatchBuild(pInDev,pFloor,TRUE) == 0)
			return FALSE;
		if(pItem->MatchBType(pInDev,pFloor,TRUE) == 0)
			return FALSE;
		if(pItem->MatchStair(pInDev,pFloor,TRUE) == 0)
			return FALSE;
		if(pItem->MatchFloorRange(pInDev,pFloor) == 0)
			return FALSE;
		pFloor->GetLogicOutputConditionDevice(pInDev,pDevList,pItem);
		return TRUE;
	}

	if(nSrcFlNum < -1)
	{
		if(pItem->CheckMatchLinkedBuild(pInDev,pFloor))
		{
			// 입력회로가 주차장 또는 출력이 주차장이면 
			if(nTgtFlNum > -1) return FALSE;
		}
		else
		{
			if(pItem->MatchBuild(pInDev,pFloor,FALSE) == 0)
				return FALSE;
			if(pItem->MatchBType(pInDev,pFloor,FALSE) == 0)
				return FALSE;
			if(pItem->MatchStair(pInDev,pFloor,FALSE) == 0)
				return FALSE;
			if(nTgtFlNum > -1) return FALSE;
		}
	}
	else if(nSrcFlNum == -1)
	{
		if(pItem->CheckMatchLinkedBuild(pInDev,pFloor))
		{
			if(nTgtFlNum > -1) return FALSE;
		}
		else
		{
			if(nTgtFlNum <= -1)
			{
				if(pItem->MatchBuild(pInDev,pFloor,FALSE) == 0)
					return FALSE;
				if(pItem->MatchBType(pInDev,pFloor,FALSE) == 0)
					return FALSE;
				if(pItem->MatchStair(pInDev,pFloor,FALSE) == 0)
					return FALSE;
			}
			else
			{
				// 대상 층이 1층이상
				if(pItem->MatchBuild(pInDev,pFloor,TRUE) == 0)
					return FALSE;
				if(pItem->MatchBType(pInDev,pFloor,TRUE) == 0)
					return FALSE;
				if(pItem->MatchStair(pInDev,pFloor,TRUE) == 0)
					return FALSE;
				
				if(pItem->GetUnderB1F() == 1 && nTgtFlNum == 1)
				{
				//	return TRUE;
				}
				else
				{
					if(pItem->MatchFloorRange(pInDev,pFloor) == 0)
						return FALSE;
				}
			}
		}
	}
	else if(nSrcFlNum == 1)
	{
		if(pItem->CheckMatchLinkedBuild(pInDev,pFloor))
		{
			if(nTgtFlNum > -1) return FALSE;
		}
		else
		{
			if(nTgtFlNum < 0)
			{
				if(pItem->MatchBuild(pInDev,pFloor,FALSE) == 0)
					return FALSE;
				if(pItem->MatchBType(pInDev,pFloor,FALSE) == 0)
					return FALSE;
				if(pItem->MatchStair(pInDev,pFloor,FALSE) == 0)
					return FALSE;
				if(pItem->GetUnder1F() != 1)
					return FALSE;
			}
			else
			{
				if(pItem->MatchBuild(pInDev,pFloor,TRUE) == 0)
					return FALSE;
				if(pItem->MatchBType(pInDev,pFloor,TRUE) == 0)
					return FALSE;
				if(pItem->MatchStair(pInDev,pFloor,TRUE) == 0)
					return FALSE;
				if(pItem->MatchFloorRange(pInDev,pFloor) == 0)
					return FALSE;
			}
		}
	}
	else // nSrcFlNum > 1
	{
		if(nTgtFlNum < 0)
		{
			return FALSE;
		}
		else
		{
			if(pItem->MatchBuild(pInDev,pFloor,TRUE) == 0)
				return FALSE;
			if(pItem->MatchBType(pInDev,pFloor,TRUE) == 0)
				return FALSE;
			if(pItem->MatchStair(pInDev,pFloor,TRUE) == 0)
				return FALSE;
			if(pItem->MatchFloorRange(pInDev,pFloor) == 0)
				return FALSE;
		}
	}
	pFloor->GetLogicOutputConditionDevice(pInDev,pDevList,pItem);
	return TRUE;
}

BOOL CXDataStair::CheckRangeLogicMatch(
	CXDataDev * pInDev,CXMapLink * pDevList
	,CXDataFloor *pFloor,CXDataRangeLogic * pRange,CXDataLogicMst * pMst
	,BOOL bCheckFloor
)
{
	BOOL bGround = TRUE;
	if(pInDev->GetLocFloorNumber() < 0)
		bGround = FALSE;
	if(pRange->MatchBuild(pInDev,pFloor,bGround) == FALSE)
		return FALSE;
	if(pRange->MatchStair(pInDev,pFloor,bGround) == FALSE)
		return FALSE;
	if(bCheckFloor)
	{
		if(pRange->MatchFloorRange(pInDev,pFloor) == FALSE)
			return FALSE;
	}
	
	if(pFloor->GetFloorAllOutputDevList(pDevList,pRange->GetRangeId()) == FALSE)
		return FALSE;
	return TRUE;
}

/// 변환전 함수
BOOL CXDataStair::GetLogicOutputConditionDevice(
	CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicMst * pMst)
{
	POSITION pos;
	CXDataLogicItem * pItem;
	CXDataFloor * pFloor;
	if(m_pListFloor == nullptr)
		return FALSE;
	pItem = pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID];
	pos = m_pListFloor->GetHeadPosition();
	while(pos)
	{
		pFloor = m_pListFloor->GetNext(pos);
		if(pFloor == nullptr)
			continue;
		
		if(CheckBasicLogicMatch(pDev,pDevList,pFloor,pMst) == TRUE)
		{
			pFloor->GetLogicOutputConditionDevice(pDev,pDevList,pItem);
		}
	}
	return TRUE;
}

BOOL CXDataStair::GetStairAllDevList(CXMapDev * pDevList,BOOL bRemoveDev)
{
	CXMapDev retList;
	POSITION pos;
	CXDataFloor * pFloor;
	// [2025/8/1 8:28:43 KHS] 
	// 향후 추가될 입력 범위설정을 위해 Flag 생성
	// 아래 플래그를 CDataAutoLogic에 포함시켜 입력타입 범위를 체크한다.
	CString strStairName = L"";
	if(m_pListFloor == nullptr)
		return FALSE;
	//
	pos = m_pListFloor->GetHeadPosition();
	while(pos)
	{
		pFloor = m_pListFloor->GetNext(pos);
		if(pFloor == nullptr)
			continue;

		pFloor->GetFloorAllDevList(&retList,bRemoveDev);
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}

BOOL CXDataStair::CopyData(CXDataStair * pSrc)
{
	if(pSrc == nullptr)
		return FALSE;
	CXListFloor * pList;
	m_pParent = pSrc->GetParent();
	m_strName = pSrc->GetName();
	m_nId  = pSrc->GetId();
	m_bInputItem= pSrc->IsInputType();
	m_nNameIndex = pSrc->GetIndex();
	m_nBuildIndex = pSrc->GetBuildIndex();
	m_nBtypeIndex = pSrc->GetBtypeIndex();

	pList = pSrc->GetListFloor();
	if(pList == nullptr)
		return TRUE;

	m_pListFloor = new CXListFloor;
	m_pListFloor->CopyData(pList);
	return TRUE;
}

// 
// BOOL CXDataStair::GetOutRangeFloor(CXMapOutFloor * pMapOutFloor,CXDataRangeLogic * pRangeLogic)
// {
// 	if(m_pListFloor == nullptr)
// 		return FALSE;
// 	POSITION pos;
// 	CXDataFloor * pData;
// 	SU_OUTFLOOR_KEY suKey;
// 	RANGE_RESULT nRet;
// 	int nRangeStart,nRangeEnd,nFlNum , nPlusN;
// 	nRangeStart = pRangeLogic->GetRangeStartLevelNum();
// 	nRangeEnd = pRangeLogic->GetRangeEndLevelNum();
// 	nPlusN = pRangeLogic->GetPlusNEnd();
// 	pos = m_pListFloor->GetHeadPosition();
// 	while(pos)
// 	{
// 		pData = m_pListFloor->GetNext(pos);
// 		if(pData == nullptr)
// 			continue;
// 		nFlNum = pData->GetFloorNumber();
// 		nRet = pRangeLogic->CheckFloorPosition(nFlNum);
// 		// 출력 층의 범위는 
// 		// 범위 아래 - (+N층) ~ 범위 위 + (+N층)
// 		switch(nRet)
// 		{
// 		case RET_RANGE_BELOW:
// 			// 직상 +N층 범위가 5층 부터 이고 +N층이 4일때 2층부터 
// 			if(nRangeStart - nPlusN > nFlNum)
// 				continue; 
// 			break;
// 		case RET_RANGE_INSIDE:
// 			break;
// 		case RET_RANGE_OVER:
// 			if(nRangeEnd + nPlusN < nFlNum)
// 				continue;
// 			break;
// 		}
// 		suKey.dwKey = 0; 
// 		suKey.stKey.dwBIdx = pData->GetBuildIndex();
// 		suKey.stKey.dwTIdx = pData->GetBtypeIndex();
// 		suKey.stKey.dwSIdx = pData->GetStairIndex();
// 		suKey.stKey.dwFIdx = pData->GetIndex();
// 		(*pMapOutFloor)[suKey.dwKey] = pData;
// 	}
// 	return TRUE;
// }

BOOL CXDataStair::GetAppectingInputDev(CXMapDev * pDevList,CXDataRangeLogic * pRange , CXDataLogicItem * pItem,BOOL bAlertEqType)
{
	//CXMapDev retList;
	POSITION pos;
	CXDataFloor * pFloor;
	RANGE_RESULT nResult = RET_RANGE_OVER;
	int nFlNum;
	if(m_pListFloor == nullptr)
		return FALSE;
	//
	pos = m_pListFloor->GetHeadPosition();
	while(pos)
	{
		pFloor = m_pListFloor->GetNext(pos);
		if(pFloor == nullptr)
			continue;
		nFlNum = pFloor->GetFloorNumber();
		if(pItem->GetMatchGroundFloor() == 0)
		{
			//if(bAlertEqType == TRUE)
			nResult = pRange->CheckFloorPosition(nFlNum);
			if(bAlertEqType)
			{
				pFloor->GetFloorAllDevList(pDevList,TRUE);
			}
			else
			{
				if(nResult == RET_RANGE_INSIDE)
					pFloor->GetFloorAllDevList(pDevList,TRUE);
			}
			continue;
		}
		nResult = pRange->CheckFloorPosition(nFlNum);
		if(nResult == RET_RANGE_INSIDE)
		{
			// 해당하는 입력회로는 삭제 한다.
			// 남은 입력회로만 기본로직 적용하기 위해
			pFloor->GetFloorAllDevList(pDevList,TRUE);
		}
		else if(nResult == RET_RANGE_BELOW)
		{
			// +N층 적용 시 출력회로가 포함되면 출력범위에 영향을 주는 회로로 추가한다.
			if(nFlNum + pRange->GetPlusNEnd() >= pRange->GetRangeStartLevelNum())
			{
				if(bAlertEqType == TRUE)
					pFloor->GetFloorAllDevList(pDevList,TRUE);
			}
		}
	}
	//pDevList->insert(retList.begin(),retList.end());
	//retList.clear();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ※ 범위에서 로직에서 층의 범위를 확인 하지 않는경우
//   1) 기본 로직이 층 일치 조건 없음 , 
/// 입력회로가 범위에 영향을 줄 수 있는 회로
BOOL CXDataStair::GetRangeOutputDevice(
	CXDataDev * pInDev,CXMapLink * pMapOutDev
	,CXDataRangeLogic * pRange,CXDataLogicMst * pMst
	,BOOL bAlertTypeEq)
{
	POSITION pos;
	CXDataLogicItem * pItem;
	CXDataFloor * pFloor;
	int nSrcFl,nTargetFl,nPlusN;
	int nRangeStart,nRangeEnd;
	RANGE_RESULT nResult;
	if(m_pListFloor == nullptr)
		return FALSE;



	if(pRange->GetUseFloorRange() == 0)
	{
		pos = m_pListFloor->GetHeadPosition();
		while(pos)
		{
			pFloor = m_pListFloor->GetNext(pos);
			if(pFloor == nullptr)
				continue;
			if(bAlertTypeEq)
			{
				if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
					continue;
			}
			else
			{
				if(pRange->CheckFloorPosition(pFloor->GetFloorNumber()) == RET_RANGE_INSIDE)
				{
					if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
						continue;
				}
			}
		}
		return TRUE;
	}

	nPlusN = pRange->GetPlusNEnd();
	nRangeStart = pRange->GetRangeStartLevelNum();
	nRangeEnd = pRange->GetRangeEndLevelNum();
	pItem = pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID];

	// 입력 회로와 설정된 범위와의 관계를 확인
	nSrcFl = pInDev->GetLocFloorNumber();
	nResult = pRange->CheckFloorPosition(nSrcFl);

	pos = m_pListFloor->GetHeadPosition();
	while(pos)
	{
		pFloor = m_pListFloor->GetNext(pos);
		if(pFloor == nullptr)
			continue;

		nTargetFl = pFloor->GetFloorNumber();
		switch(nResult)
		{
		case RET_RANGE_BELOW:
			/*
			1. 입력 아래 출력
				기본로직
			2. 입력 ~ 범위 사이 출력
				1) 출력이 경보 타입이 아니면 - 옵션 사용 불가 : 주로직
				- 층일치가 없을 때 - 주로직
				- 층일치가 있을 때 - 주로직
				2) 출력이 경보 타입 이면 - 옵션 사용가능
				- 층일치가 없을 때 - 옵션에 주로직,범위로직
				- 층일치가 있을 때 - 주로직
			3. 범위 내 출력
				1) 출력이 경보 타입이 아니면 - 주로직
				- 층일치가 없을 때 - 주로직
				- 층일치가 있을 때 - 주로직
				2) 출력이 경보 타입이면
				- 층일치가 없을 때 - 범위로직
				- 층일치가 있을 때 - 범위로직
			4. 범위 초과 출력
				1) 출력이 경보 타입이 아니면 - 주로직
					- 층일치가 없을 때 - 주로직
					- 층일치가 있을 때 - 주로직
				2) 출력이 경보 타입이면 - 옵션에 따라
					- 층일치가 없을 때 - 옵션에 따라 주로직,범위로직
					- 층일치가 이을 때 - 옵션에 따라 주로직,범위로직
			*/
			if(nTargetFl < nRangeStart)
			{
				// 1. 입력 아래 출력
				// 2. 입력 ~ 범위 사이 출력
				if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
					continue;
				//if(bAlertTypeEq == FALSE)
				//{
				//	if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
				//		continue;
				//}
				//else
				//{
				//	if(pItem->GetMatchGroundFloor() == 0)
				//	{
				//		if(pRange->GetUseRangeLogicOverFloor() == 1)
				//		{
				//			if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
				//				continue;
				//		}
				//		else
				//		{
				//			if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
				//				continue;
				//		}
				//	}
				//	else
				//	{
				//		if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
				//			continue;
				//	}
				//}
			}
			else if(nRangeStart <= nTargetFl && nTargetFl <= nRangeEnd)
			{
				//3. 범위 내 출력
				//	1) 출력이 경보 타입이 아니면 - 주로직
				//		- 층일치가 없을 때 - 주로직
				//		- 층일치가 있을 때 - 주로직
				//	2) 출력이 경보 타입이면
				//		- 층일치가 없을 때 - 범위로직
				//		- 층일치가 있을 때 - 범위로직
				if(bAlertTypeEq == FALSE)
				{
					if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
						continue;
				}
				else
				{
					if(pItem->GetMatchGroundFloor() == 0)
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
							continue;
					}
					else
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
							continue;
					}
				}
			}
			else if(nRangeEnd < nTargetFl)
			{
				//4. 범위 초과 출력
				//	1) 출력이 경보 타입이 아니면 - 주로직
				//		- 층일치가 없을 때 - 주로직
				//		- 층일치가 있을 때 - 주로직
				//	2) 출력이 경보 타입이면 - 옵션에 따라
				//		- 층일치가 없을 때 - 옵션에 따라 주로직,범위로직
				//		- 층일치가 이을 때 - 옵션에 따라 주로직,범위로직
				if(bAlertTypeEq == FALSE)
				{
					if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
						continue;
				}
				else
				{
					if(pItem->GetMatchGroundFloor() == 0)
					{
						if(pRange->GetUseRangeLogicOverFloor() == 1)
						{
							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
								continue;
						}
						else
						{
							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
								continue;
						}
					}
					else
					{
						if(pRange->GetUseRangeLogicOverFloor() == 1)
						{
							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
								continue;
						}
						else
						{
							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
								continue;
						}
					}
				}
			}
			// 			
			break;
		case RET_RANGE_INSIDE:
			/*
			1. 출력이 범위 미만
				1) 출력이 경보 타입이 아니면 - 주로직
					- 층일치가 없을 때 - 범위로직(층 X)
					- 층일치가 있을 때 - 기본로직(범위밖 , 처리할 필요 없음)
				2) 출력이 경보 타입이면 - 
					- 층일치가 없을 때 - 범위로직(층 X)
					- 층일치가 있을 때 - 기본로직(범위밖, 처리할 필요 없음)
			2. 출력이 범위 내
				1) 출력이 경보 타입이 아니면 - 범위로직
					- 층일치가 없을 때 - 범위로직(층 X)
					- 층일치가 있을 때 - 범위로직(층 O)
				2) 출력이 경보 타입이면
					- 층일치가 없을 때 - 범위로직(층 X)
					- 층일치가 있을 때 - 범위로직(층 O)
			3. 출력이 범위 초과
				1) 출력이 경보 타입이 아니면 - 범위로직
					- 층일치가 없을 때 - 범위로직(층 X)
					- 층일치가 있을 때 - 범위로직(층 O)
				2) 출력이 경보 타입이면 - 옵션에 따라 
					- 층일치가 없을 때 - 옵션에 따라 범위(층 X),주 로직
					- 층일치가 있을 때 - 옵션에 따라 범위(층 O), 주로직
			*/
			if(nTargetFl < nRangeStart)
			{
				//1. 출력이 범위 미만
				//	1) 출력이 경보 타입이 아니면 - 주로직
				//	- 층일치가 없을 때 - 범위로직(층 X)
				//	- 층일치가 있을 때 - 기본로직(범위밖 , 처리할 필요 없음)
				//	2) 출력이 경보 타입이면 -
				//	- 층일치가 없을 때 - 범위로직(층 X)
				//	- 층일치가 있을 때 - 기본로직(범위밖 , 처리할 필요 없음)
				if(bAlertTypeEq == FALSE)
				{
					if(pItem->GetMatchGroundFloor() == 0)
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
							continue;
					}
					else
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
							continue;
					}
				}
				else
				{
					if(pItem->GetMatchGroundFloor() == 0)
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
							continue;
					}
					else
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
							continue;
					}
				}
			}
			else if(nRangeStart <= nTargetFl && nRangeEnd >= nTargetFl)
			{
				//2. 출력이 범위 내
				//	1) 출력이 경보 타입이 아니면 - 범위로직
				//	- 층일치가 없을 때 - 범위로직(층 X)
				//	- 층일치가 있을 때 - 범위로직(층 O)
				//	2) 출력이 경보 타입이면
				//	- 층일치가 없을 때 - 범위로직(층 X)
				//	- 층일치가 있을 때 - 범위로직(층 O)
				if(bAlertTypeEq == FALSE)
				{
					if(pItem->GetMatchGroundFloor() == 0)
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
							continue;
					}
					else
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
							continue;
					}
				}
				else
				{
					if(pItem->GetMatchGroundFloor() == 0)
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
							continue;
					}
					else
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
							continue;
					}
				}
			}
			else if(nRangeEnd < nTargetFl)
			{
				//3. 출력이 범위 초과
				//	1) 출력이 경보 타입이 아니면 - 범위로직
				//	- 층일치가 없을 때 - 범위로직(층 X)
				//	- 층일치가 있을 때 - 범위로직(층 O)
				//	2) 출력이 경보 타입이면 - 옵션에 따라
				//	- 층일치가 없을 때 - 옵션에 따라 범위(층 X),주 로직
				//	- 층일치가 있을 때 - 옵션에 따라 범위(층 O),주로직
				if(bAlertTypeEq == FALSE)
				{
					if(pItem->GetMatchGroundFloor() == 0)
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
							continue;
					}
					else
					{
						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
							continue;
					}
				}
				else
				{
					if(pItem->GetMatchGroundFloor() == 0)
					{
						if(pRange->GetUseRangeLogicOverFloor() == 1)
						{
							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
								continue;
						}
						else
						{
							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
								continue;
						}
					}
					else
					{
						if(pRange->GetUseRangeLogicOverFloor() == 1)
						{
							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
								continue;
						}
						else
						{
							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
								continue;
						}
					}
				}
			}
			break;
		case RET_RANGE_OVER:
			// 기본 로직
			if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
				continue;
			break;
		}
	}
	return TRUE;
}


/// Backup

////////////////////////////////////////////////////////////////////////////
//// ※ 범위에서 로직에서 층의 범위를 확인 하지 않는경우
////   1) 기본 로직이 층 일치 조건 없음 , 
///// 입력회로가 범위에 영향을 줄 수 있는 회로
//BOOL CXDataStair::GetRangeOutputDevice(
//	CXDataDev * pInDev,CXMapLink * pMapOutDev
//	,CXDataRangeLogic * pRange,CXDataLogicMst * pMst
//	,BOOL bAlertTypeEq)
//{
//	POSITION pos;
//	CXDataLogicItem * pItem;
//	CXDataFloor * pFloor;
//	int nSrcFl,nTargetFl,nPlusN;
//	int nRangeStart,nRangeEnd;
//	RANGE_RESULT nResult;
//	if(m_pListFloor == nullptr)
//		return FALSE;
//
//
//
//	if(pRange->GetUseFloorRange() == 0)
//	{
//		pos = m_pListFloor->GetHeadPosition();
//		while(pos)
//		{
//			pFloor = m_pListFloor->GetNext(pos);
//			if(pFloor == nullptr)
//				continue;
//			if(bAlertTypeEq)
//			{
//				if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//					continue;
//			}
//			else
//			{
//				if(pRange->CheckFloorPosition(pFloor->GetFloorNumber()) == RET_RANGE_INSIDE)
//				{
//					if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//						continue;
//				}
//			}
//		}
//		return TRUE;
//	}
//
//	nPlusN = pRange->GetPlusNEnd();
//	nRangeStart = pRange->GetRangeStartLevelNum();
//	nRangeEnd = pRange->GetRangeEndLevelNum();
//	pItem = pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID];
//
//	// 입력 회로와 설정된 범위와의 관계를 확인
//	nSrcFl = pInDev->GetLocFloorNumber();
//	nResult = pRange->CheckFloorPosition(nSrcFl);
//
//	pos = m_pListFloor->GetHeadPosition();
//	while(pos)
//	{
//		pFloor = m_pListFloor->GetNext(pos);
//		if(pFloor == nullptr)
//			continue;
//
//		nTargetFl = pFloor->GetFloorNumber();
//		switch(nResult)
//		{
//		case RET_RANGE_BELOW:
//			if(nSrcFl <= nTargetFl)
//			{
//				/*
//				1. 입력 아래 출력
//				1) 출력이 경보 타입이 아니면 - 옵션 사용 불가 : 주로직
//				- 층일치가 없을 때 - 주로직
//				- 층일치가 있을 때 - 주로직
//				2) 출력이 경보 타입이면 - 옵션 사용가능
//				- 층일치가 없을 때 - 옵션에 따라 주로직,범위로직
//				- 층일치가 있을 때 - 주로직
//				2. 입력 ~ 범위 사이 출력
//				1) 출력이 경보 타입이 아니면 - 옵션 사용 불가 : 주로직
//				- 층일치가 없을 때 - 주로직
//				- 층일치가 있을 때 - 주로직
//				2) 출력이 경보 타입 이면 - 옵션 사용가능
//				- 층일치가 없을 때 - 옵션에 주로직,범위로직
//				- 층일치가 있을 때 - 주로직
//				3. 범위 내 출력
//				1) 출력이 경보 타입이 아니면 - 주로직
//				- 층일치가 없을 때 - 주로직
//				- 층일치가 있을 때 - 주로직
//				2) 출력이 경보 타입이면
//				- 층일치가 없을 때 - 범위로직
//				- 층일치가 있을 때 - 범위로직
//				4. 범위 초과 출력
//				1) 출력이 경보 타입이 아니면 - 주로직
//				- 층일치가 없을 때 - 주로직
//				- 층일치가 있을 때 - 주로직
//				2) 출력이 경보 타입이면 - 옵션에 따라
//				- 층일치가 없을 때 - 옵션에 따라 주로직,범위로직
//				- 층일치가 이을 때 - 옵션에 따라 주로직,범위로직
//				*/
//				// 1) 범위 아래 출력은 기본로직
//				if(nTargetFl < nRangeStart)
//				{
//					// 기본 로직 적용
//					// 층일치가 없으면 Option에 따라 기본 또는 범위로직(층일치 무시)
//					if(pItem->GetMatchGroundFloor() == 1)
//					{
//						if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//							continue;
//					}
//					else
//					{
//						// 소스 - 범위 아래 , nTargetFl < nRangeStart
//						if(pRange->GetUseRangeLogicOverFloor() == 1)
//						{
//							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//								continue;
//						}
//						else
//						{
//							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//								continue;
//						}
//					}
//				}
//				else if(nRangeStart <= nTargetFl && nTargetFl <= nRangeEnd)
//				{
//					// 입력이 범위 아래 이고 , 대상이 범위 안 --> 범위 로직
//					// 기본로직에 층일치가 없으면 층 일치 무시
//					if(pItem->GetMatchGroundFloor() == 1)
//					{
//						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
//							continue;
//					}
//					else
//					{
//						// 소스 - 범위 아래 , nRangeStart <= nTargetFl && nTargetFl <= nRangeEnd
//						if(pRange->GetUseRangeLogicOverFloor() == 1)
//						{
//							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//								continue;
//						}
//						else
//						{
//							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//								continue;
//						}
//					}
//				}
//				else if(nRangeEnd < nTargetFl)
//				{
//					// 기본 로직 
//					// 기본로직의 층일치가 없으면 option따라 기본 또는 범위로직
//					if(pItem->GetMatchGroundFloor() == 1)
//					{
//						if(pRange->GetUseRangeLogicOverFloor())
//						{
//							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
//								continue;
//						}
//						else
//						{
//							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//								continue;
//						}
//					}
//					else
//					{
//						// 소스 - 범위 아래 , nRangeEnd < nTargetFl
//						if(pRange->GetUseRangeLogicOverFloor())
//						{
//							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//								continue;
//						}
//						else
//						{
//							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//								continue;
//						}
//					}
//				}
//			}
//			else if(nSrcFl > nTargetFl)
//			{
//				if(pItem->GetMatchGroundFloor() == 1)
//				{
//					if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//						continue;
//				}
//				else
//				{
//					// 소스 - 범위 아래 , nSrcFl > nTargetFl
//					if(pRange->GetUseRangeLogicOverFloor())
//					{
//						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//							continue;
//					}
//					else
//					{
//						if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//							continue;
//					}
//				}
//			}
//			// 			
//			break;
//		case RET_RANGE_INSIDE:
//			if(nTargetFl < nRangeStart)
//			{
//				// 입력 회로가 범위 안이고 출력이 범위 아래 일때 기본 로직을 사용해야되지만
//				// 기본 로직에서 층일치가 없으면 범위 로직을 사용한다
//				//  ex) 1. 조건 : 
//				//		  - 기본 로직 : 건물,계단 (층 일치 없음)
//				//		  - 범위 로직 : 조건 없음
//				//      2. 결과 
//				//		  - 범위가 101,102동일 15 ~17 층 일때
//				//		  - 101동 1계단 15층 입력 시
//				//		    -- > 101동 전체 + 102동 전체 (범위 내에서 층 구분 무시)
//				if(pItem->GetMatchGroundFloor())
//				{
//					if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//						continue;
//				}
//				else
//				{
//					// 소스 - 범위 안 , nTargetFl < nRangeStart
//					if(pRange->GetUseRangeLogicOverFloor())
//					{
//						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//							continue;
//					}
//					else
//					{
//						if(bAlertTypeEq)
//						{
//							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//								continue;
//						}
//						else
//						{
//							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//								continue;
//						}
//					}
//				}
//			}
//			if(nRangeStart <= nTargetFl && nRangeEnd >= nTargetFl)
//			{
//				// 입력이 범위 아래 이고 , 대상이 범위 안 --> 범위 로직
//				// 기본 로직에서 층일치가 없으면 범위 로직 --> 층 무시
//				if(pItem->GetMatchGroundFloor() == 1)
//				{
//					if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
//						continue;
//				}
//				else
//				{
//					// 소스 - 범위 안 , nRangeStart <= nTargetFl && nRangeEnd >= nTargetFl
//					if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//						continue;
//				}
//			}
//			else if(nRangeEnd < nTargetFl)
//			{
//				if(pItem->GetMatchGroundFloor())
//				{
//					if(pRange->GetUseRangeLogicOverFloor())
//					{
//						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,TRUE) == FALSE)
//							continue;
//					}
//					else
//					{
//						if(bAlertTypeEq)
//						{
//							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//								continue;
//						}
//						else
//						{
//							// 소스 - 범위 안 , nRangeStart <= nTargetFl && nRangeEnd >= nTargetFl
//							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//								continue;
//						}
//					}
//				}
//				else
//				{
//					// 소스 - 범위 안 , nRangeEnd < nTargetFl
//					if(pRange->GetUseRangeLogicOverFloor())
//					{
//
//						if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//							continue;
//					}
//					else
//					{
//						if(bAlertTypeEq)
//						{
//							if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//								continue;
//						}
//						else
//						{
//							if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//								continue;
//						}
//					}
//				}
//			}
//
//			break;
//		case RET_RANGE_OVER:
//			// 기본 로직
//			if(pItem->GetMatchGroundFloor())
//			{
//				if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//					continue;
//			}
//			else
//			{
//				// 소스 - 범위 밖
//				if(pRange->GetUseRangeLogicOverFloor() == 1)
//				{
//					if(CheckRangeLogicMatch(pInDev,pMapOutDev,pFloor,pRange,pMst,FALSE) == FALSE)
//						continue;
//				}
//				else
//				{
//					if(CheckBasicLogicMatch(pInDev,pMapOutDev,pFloor,pMst) == FALSE)
//						continue;
//				}
//			}
//			break;
//		}
//	}
//	return TRUE;
//}