#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XDataRangeLogic.h"
#include "XDataDev.h"
#include "XDataFloor.h"

CXDataRangeLogic::CXDataRangeLogic()
{
	m_nRangeId = 0;
	m_btPriority = MAX_LOGIC_PRIORITY;

	m_nRangeStartLevelNum = 0;
	m_nRangeEndLevelNum = 0;

	m_btEmergency = 0;
	m_btSameAddrOutput = 0;
	m_nPlusNStart = 0;
	m_nPlusNEnd = 0;

	m_suMatchUnder.btLoctype = 0;
	m_suMatchGround.btLoctype = 0;
	m_suMatchPark.btLoctype = 0;

	m_btUseRangeLogicOverFloor = FALSE;
}


CXDataRangeLogic::~CXDataRangeLogic()
{
	m_vtInputBuildIds.clear();
	m_vtInputStairs.clear();
	m_arrBuildName.RemoveAll();
	m_arrStairName.RemoveAll();
}

void CXDataRangeLogic::CopyArray(CStringArray * pTarget,CStringArray * pSource,BOOL bBuild)
{

	int nSize;
	int i,nIdx = 0;
	CString str;

	if(pTarget == nullptr || pSource == nullptr)
		return;

	if(pSource->GetSize() <= 0)
		return;
	nSize = pSource->GetSize();
	for(i = 0; i < nSize; i++)
	{
		str = pSource->GetAt(i);
		if(str.GetLength() <= 0)
			continue;
		if(bBuild)
		{
			nIdx = g_MapIdxBuild[str];
			m_vtInputBuildIds.push_back(nIdx);
		}
		else
		{
			nIdx = g_MapIdxStair[str];
			m_vtInputStairs.push_back(nIdx);
		}
		pTarget->Add(str);
	}
}

void CXDataRangeLogic::SetRangeID(int nId,BYTE btPriority,BYTE btUseRangeOverFloor
	,BYTE btUseBuildRange,BYTE btUseStairRange,BYTE btUseFloorRange
)
{
	m_nRangeId = nId; 
	m_btPriority = btPriority; 
	m_btUseRangeLogicOverFloor = btUseRangeOverFloor;
	m_btUseBuildRange = btUseBuildRange;
	m_btUseStairRange = btUseStairRange;
	m_btUseFloorRange = btUseFloorRange;

}

BYTE CXDataRangeLogic::SetMatchCondition(
	BYTE btType
	,BYTE btMatchBuild,BYTE btMatchBType,BYTE btMatchStair,BYTE btMatchFloor,BYTE btMatchRoom
)
{
	SU_LOCTYPEBIT suData;
	suData.btLoctype = 0;
	suData.stLoctype.btBuild = btMatchBuild;
	suData.stLoctype.btBType = btMatchBType;
	suData.stLoctype.btStair = btMatchStair;
	// +N층일 때는 무조건 층일치 들어가야된다.
// 	if(m_nPlusNStart != 0 || m_nPlusNEnd != 0)
// 		suData.stLoctype.btFloor = 1;
// 	else
		suData.stLoctype.btFloor = btMatchFloor;

	// 	if(m_nPluseNFloor != 0)
	// 		suData.stLoctype.btFloor = 1;
	// 	else
	// 		suData.stLoctype.btFloor = btMatchFloor;
	suData.stLoctype.btRoom = btMatchRoom;
	switch(btType)
	{
	case MATCH_GROUND:
		m_suMatchGround.btLoctype = suData.btLoctype;
		break;
	case MATCH_UNDER:
		m_suMatchUnder.btLoctype = suData.btLoctype;
		break;
	case MATCH_PARK:
		m_suMatchPark.btLoctype = suData.btLoctype;
		break;
	}
	return suData.btLoctype;
}

void CXDataRangeLogic::SetAutoLogic(
	int nId
	,CStringArray * pArrBuild,CStringArray * pArrStair
	,int nStartLevelNum,int nEndLevelNum
	,BYTE btEmer,BYTE btSameout,int nNFloorStart,int nNFloorEnd
	,BYTE btUseUnderLogic,BYTE btUseParkLogic
	,BYTE btMatchGround,BYTE btMatchUnder,BYTE btMatchPark
	,BYTE btUnder1F,BYTE btUnderB1F
)
{
	m_nRangeId = nId;

	CopyArray(&m_arrBuildName,pArrBuild,TRUE);
	CopyArray(&m_arrStairName,pArrStair,FALSE);

	m_nRangeEndLevelNum = nEndLevelNum;
	m_nRangeStartLevelNum = nStartLevelNum;


	m_btEmergency = btEmer;

	m_btSameAddrOutput = btSameout;
	m_nPlusNStart = nNFloorStart;
	m_nPlusNEnd = nNFloorEnd;

	//m_btUseUnderLogic = btUseUnderLogic;
	m_btUnder1F = btUnder1F;
	m_btUnderB1F = btUnderB1F;

	m_suMatchGround.btLoctype = btMatchGround;
	m_suMatchUnder.btLoctype = btMatchUnder;
	m_suMatchPark.btLoctype = btMatchPark;

	m_btUseUnderLogic = btUseUnderLogic;
	m_btUseParkLogic = btUseParkLogic;
}


void CXDataRangeLogic::SetAutoLogic(
	int nId
	,CStringArray * pArrBuild,CStringArray * pArrStair
	,int nStartLevelNum,int nEndLevelNum
	,BYTE btEmer,BYTE btSameout,int nNFloorStart,int nNFloorEnd
	,BYTE btUseUnderLogic,BYTE btUseParkLogic
	,BYTE btMatchGroundBuild,BYTE btMatchGroundBType,BYTE btMatchGroundStair,BYTE btMatchGroundFloor,BYTE btMatchGroundRoom
	,BYTE btMatchUnderBuild,BYTE btMatchUnderBType,BYTE btMatchUnderStair,BYTE btMatchUnderFloor,BYTE btMatchUnderRoom
	,BYTE btMatchParkBuild,BYTE btMatchParkBType,BYTE btMatchParkStair,BYTE btMatchParkFloor,BYTE btMatchParkRoom
	,BYTE btUnder1F,BYTE btUnderB1F
)
{
	m_nRangeId = nId;

	CopyArray(&m_arrBuildName,pArrBuild,TRUE);
	CopyArray(&m_arrStairName,pArrStair,FALSE);

	m_nRangeEndLevelNum = nEndLevelNum;
	m_nRangeStartLevelNum = nStartLevelNum;

	m_btEmergency = btEmer;

	m_btSameAddrOutput = btSameout;
	m_nPlusNStart = nNFloorStart;
	m_nPlusNEnd = nNFloorEnd;

	//m_btUseUnderLogic = btUseUnderLogic;
	m_btUnder1F = btUnder1F;
	m_btUnderB1F = btUnderB1F;

	SetMatchCondition(MATCH_GROUND,btMatchGroundBuild,btMatchGroundBType,btMatchGroundStair,btMatchGroundFloor,btMatchGroundRoom);
	SetMatchCondition(MATCH_UNDER,btMatchUnderBuild,btMatchUnderBType,btMatchUnderStair,btMatchUnderFloor,btMatchUnderRoom);
	SetMatchCondition(MATCH_PARK,btMatchParkBuild,btMatchParkBType,btMatchParkStair,btMatchParkFloor,btMatchParkRoom);

	m_btUseUnderLogic = btUseUnderLogic;
	m_btUseParkLogic = btUseParkLogic;
}

void CXDataRangeLogic::SetLogicInputLoc(CStringArray * pArrBuild,CStringArray * pArrStair
	,int nStartLevelNum,int nEndLevelNum
)
{
	CopyArray(&m_arrBuildName,pArrBuild,TRUE);
	CopyArray(&m_arrStairName,pArrStair,FALSE);

	m_nRangeEndLevelNum = nEndLevelNum;
	m_nRangeStartLevelNum = nStartLevelNum;
	if(m_nRangeStartLevelNum == 0 && m_nRangeEndLevelNum == 0)
		m_btUseFloorRange = 0; 
}


void CXDataRangeLogic::SetLogicOutputCondition(
	BYTE btEmer,BYTE btSameout,int nNFloorStart,int nFloorEnd
	,BYTE btUseUnderLogic,BYTE btUseParkLogic
	,BYTE btUnder1F,BYTE btUnderB1F
)
{
	m_btEmergency = btEmer;

	m_btSameAddrOutput = btSameout;

	m_nPlusNStart = nNFloorStart;
	m_nPlusNEnd = nFloorEnd;

	//m_btUseUnderLogic = btUseUnderLogic;
	m_btUnder1F = btUnder1F;
	m_btUnderB1F = btUnderB1F;

	m_btUseUnderLogic = btUseUnderLogic;
	m_btUseParkLogic = btUseParkLogic;
}

BOOL CXDataRangeLogic::CheckInputRangeBuild(int nBuildIdx)
{
	size_t sz;

	if(m_btUseBuildRange == 0)
		return TRUE;
	sz = m_vtInputBuildIds.size();
	// 입력 범위 로직의 건물 목록이 없으면 모든 건물 포함
	if(sz <= 0)
		return TRUE;
	for(auto x : m_vtInputBuildIds)
	{
		if(x == nBuildIdx)
			return TRUE;
	}
	return FALSE;
}


BOOL CXDataRangeLogic::CheckInputRangeStair(int nStairIndex)
{
	size_t sz;
	if(m_btUseStairRange == 0)
		return TRUE;
	sz = m_vtInputStairs.size();
	// 입력 범위 로직의 건물 목록이 없으면 모든 건물 포함
	if(sz <= 0)
		return TRUE;
	for(auto x : m_vtInputStairs)
	{
		if(x == nStairIndex)
			return TRUE;
	}
	return FALSE;
}

RANGE_RESULT CXDataRangeLogic::CheckFloorPosition(int nFloor)
{
	// [2026/6/23 17:10:55 KHS] 
	// 범위에 층이 설정되지 않았으면
	if((m_btUseFloorRange == 0) ||(m_nRangeStartLevelNum == 0 && m_nRangeEndLevelNum == 0))
		return RET_RANGE_INSIDE;

	// 범위 아래
	if(nFloor < m_nRangeStartLevelNum)
		return RET_RANGE_BELOW;
	if(nFloor >= m_nRangeStartLevelNum && nFloor <= m_nRangeEndLevelNum)
		return RET_RANGE_INSIDE;
	if(nFloor > m_nRangeEndLevelNum)
		return RET_RANGE_OVER;
}


BOOL CXDataRangeLogic::MatchBuild(CXDataDev* src,CXDataFloor* tgt,BOOL ground)
{
	if(m_btUseBuildRange == FALSE)
		return TRUE;
	if(ground)
		return GetMatchGroundBuild() ? (src->GetBuildIndex() == tgt->GetBuildIndex()) : TRUE;
	else
		return TRUE;
}

BOOL CXDataRangeLogic::MatchStair(CXDataDev* src,CXDataFloor* tgt,BOOL ground)
{
	if(m_btUseStairRange == FALSE)
		return TRUE;
	if(ground)
		return GetMatchGroundStair() ? (src->GetStairIndex() == tgt->GetStairIndex()) : TRUE;
	else
		return TRUE;
}

BOOL CXDataRangeLogic::MatchFloorRange(CXDataDev* src,CXDataFloor* tgt)
{
	// [2026/6/10 10:07:16 KHS] 
	// 발생 층이 지하 일때도 범위 적용
	// 지하 로직 - 사용 때 : ?
	//          - 사용하지 않을 때 : ?
	BOOL bRet = FALSE;
	int nSrcFlNum = src->GetLocFloorNumber();
	int nTgtFlNum = tgt->GetFloorNumber();

	if(m_btUseFloorRange == FALSE)
		return TRUE;
	// [2026/6/16 15:20:08 KHS] 
	// 범위로직에 층이 있으면 층일치 조건 1
	//if(GetMatchGroundFloor() == 1)
	{
		bRet = TRUE;
		if(nTgtFlNum >= nSrcFlNum
			&& nTgtFlNum <= (nSrcFlNum + m_nPlusNEnd))
		{
			bRet = TRUE;
		}
		else
			bRet = FALSE;
	}
// 	else
// 	{
// 		bRet = TRUE;
// 	}
	return bRet;
}
