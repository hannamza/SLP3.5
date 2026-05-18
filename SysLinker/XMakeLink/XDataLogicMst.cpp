#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XDataLogicMst.h"
#include "XDataLogicItem.h"
#include "XDataDev.h"
#include "XDataFloor.h"


CXDataLogicMst::CXDataLogicMst()
{
	m_nLgId = 0;
	m_nInType = 0;
	m_nOutType = 0;
	m_nEqName = 0;
	m_nOutContents = 0;
	m_bHaveRange = FALSE;
	m_bRangeOverUseBasicCondition = FALSE;
	for(int i = 0; i < MAX_LOGIC_ITEM_CNT + 1; i++)
	{
		m_pArrLgItem[i] = nullptr;
	}
}


CXDataLogicMst::~CXDataLogicMst()
{
	for(int i = 0; i < MAX_LOGIC_ITEM_CNT + 1; i++)
	{
		delete m_pArrLgItem[i];
		m_pArrLgItem[i] = nullptr;
	}
}

// CzDataLogicMst 멤버 함수
void CXDataLogicMst::SetLogicMst(int nLgId,int nInType,int nOutType,int nEqName,int nOutCond)
{
	m_nLgId = nLgId;
	m_nInType = nInType;
	m_nOutType = nOutType;
	m_nEqName = nEqName;
	m_nOutContents = nOutCond;
}

int CXDataLogicMst::GetEmptyIdx()
{
	for(int i = 1; i < MAX_LOGIC_ITEM_CNT + 1; i++)
	{
		if(m_pArrLgItem[i] == nullptr)
			return i;
	}
	return -1;
}

// BOOL CXDataLogicMst::CheckRangeBuild(CXDataDev * pInput,CXDataFloor * pFloor,CXDataLogicItem * pItem)
// {
// 	if(pInput->GetBuildIndex() != pFloor->GetBuildIndex())
// 		return FALSE;
// 
// 	return pItem->CheckInputRangeBuild(pFloor->GetBuildIndex());
// }
// 
// BOOL CXDataLogicMst::CheckRangeStair(CXDataDev * pInput,CXDataFloor * pFloor,CXDataLogicItem * pItem)
// {
// 	if(pInput->GetStairIndex() != pFloor->GetStairIndex())
// 		return FALSE;
// 
// 	return pItem->Get
// }

BOOL CXDataLogicMst::IsRangeIncluded(int nStartFloor,int nNfloor,CXDataLogicItem * pItem)
{
	return (pItem->GetInStartLevelNum() >= nStartFloor && pItem->GetInEndLevelNum() <= nStartFloor + nNfloor);
}


BOOL CXDataLogicMst::HasRangeIntersection(int nStartFloor,int nNfloor,CXDataLogicItem * pItem)
{
	return (((nStartFloor + nNfloor) >= pItem->GetInStartLevelNum()) && (pItem->GetInEndLevelNum() >= nStartFloor));
	//return (pItem->GetInStartLevelNum() >= nStartFloor && pItem->GetInEndLevelNum() <= nStartFloor + nNfloor);
}


RANGE_RESULT CXDataLogicMst::CheckFloorPosition(int nFloor,CXDataLogicItem * pItem)
{
	// 범위 아래
	if(nFloor < pItem->GetInStartLevelNum())
		return RET_RANGE_BELOW;

	if(nFloor > pItem->GetInEndLevelNum())
		return RET_RANGE_OVER;

	return RET_RANGE_INSIDE;
}

CXDataLogicItem* CXDataLogicMst::GetFloorLogicItem(CXDataDev * pInput,CXDataFloor * pFloor)
{
	// 1. 범위로직이 있는지 확인
	// 2. 
	// +N층 확인

	if(m_pArrLgItem[MAINLOGIC_PRIORITYID] == nullptr)
		return nullptr;
	// 범위로직이 없으면 주로직 사용
	if(m_bHaveRange == FALSE)
		return m_pArrLgItem[MAINLOGIC_PRIORITYID];
	
	int nStartFloor,nEndFloor;
	RANGE_RESULT ret;

	// 입력회로가 범위내이면 포함되는 부분은 검사할 필요 없음

	// +N층에서 N층이 0보다 작을 때 입력 회로 - N층이 범위로직에 들어가는것을 방지 하기 위해 0으로 변환
	nStartFloor = m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetPlusNStart() < 0 ? pInput->GetLocFloorNumber() : pInput->GetLocFloorNumber() + m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetPlusNStart();
	nEndFloor = nStartFloor + m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetPlusNEnd();

	for(int i = 1; i < MAX_LOGIC_ITEM_CNT + 1; i++)
	{
		if(i == MAINLOGIC_PRIORITYID)
			continue; 
		// 범위로직일 때만 확인 
		if(m_pArrLgItem[i] == nullptr)
			continue;

// 		// 입력된 건물이 입력 범위에 있는 건물 목록과 같은가
// 		if(m_pArrLgItem[i]->CheckInputRangeBuild(pFloor->GetBuildIndex()) == FALSE)
// 			continue;
// 
// 		if(m_pArrLgItem[i]->CheckInputRangeStair(pFloor->GetStairIndex()) == FALSE)
// 			continue;

		// 입력층 ~ +N층까지 안에 범위 로직을 전체(또는 일부분)을 포함하는가?
		if(HasRangeIntersection(nStartFloor,nEndFloor,m_pArrLgItem[i]))
		{
			// 범위의 건물,계단 확인
			// 입력층 ~ +N층까지 안에 범위 로직이 있는것만 
			ret = CheckFloorPosition(pFloor->GetFloorNumber(),m_pArrLgItem[i]);
			if(ret == RET_RANGE_BELOW)
				return m_pArrLgItem[MAINLOGIC_PRIORITYID];
			else if(ret == RET_RANGE_INSIDE)
			{
				//범위 로직인지 아직 모름
				if(m_pArrLgItem[i]->CheckInputRangeBuild(pFloor->GetBuildIndex()))
				{
					if(m_pArrLgItem[i]->CheckInputRangeStair(pFloor->GetStairIndex()))
					{
						return m_pArrLgItem[i];
					}
				}
				return m_pArrLgItem[MAINLOGIC_PRIORITYID];
				
			}
			else
			{
				//범위 로직인지 아직 모름
				if(m_pArrLgItem[i]->CheckInputRangeBuild(pFloor->GetBuildIndex()))
				{
					if(m_pArrLgItem[i]->CheckInputRangeStair(pFloor->GetStairIndex()))
					{
						// +N층으로 인해 범위를 초과한 경우 기본 로직 사용
						if(m_pArrLgItem[i]->GetUseRangeLogicOverFloor() == 0) // 
						{
							return m_pArrLgItem[MAINLOGIC_PRIORITYID];
						}
						return m_pArrLgItem[i];
					}
				}
				return m_pArrLgItem[MAINLOGIC_PRIORITYID];
			}
		}
	}
	return m_pArrLgItem[MAINLOGIC_PRIORITYID];
}

