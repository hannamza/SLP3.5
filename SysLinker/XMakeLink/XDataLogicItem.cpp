#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XDataLogicMst.h"
#include "XDataDev.h"
#include "XDataLogicItem.h"
#include "XDataFloor.h"
#include "XDataEm.h"
CXDataLogicItem::CXDataLogicItem()
{
	m_nLgId = 0;
	m_btPriority = MAX_LOGIC_PRIORITY;

	m_btUseLevelRange = 0;
	m_nInEndLevelNum = 0;
	m_nInStartLevelNum = 0;
	m_btInToOperator = L_OP_NONE;
	m_btInFromOperator = L_OP_NONE;

	m_nOutStartLevelNum = 0;
	m_nOutEndLevelNum = 0;
	m_btOutToOperator = L_OP_NONE;
	m_btOutFromOperator = L_OP_NONE;

	m_btEmergency = 0;
	m_btSameAddrOutput = 0;
	m_nPlusNStart = 0;
	m_nPlusNEnd = 0;

	m_suMatchUnder.btLoctype = 0;
	m_suMatchGround.btLoctype = 0;
	m_suMatchPark.btLoctype = 0;

	m_btUseRangeLogicOverFloor = FALSE;
}


CXDataLogicItem::~CXDataLogicItem()
{
	m_vtInputBuildIds.clear();
	m_vtInputStairs.clear();
	m_arrBuildName.RemoveAll();
	m_arrStairName.RemoveAll();
}

void CXDataLogicItem::CopyArray(CStringArray * pTarget,CStringArray * pSource)
{

	int nSize;
	int i;
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
		pTarget->Add(str);
	}
}

BYTE CXDataLogicItem::SetMatchCondition(
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
	if(m_nPlusNStart != 0 || m_nPlusNEnd != 0)
		suData.stLoctype.btFloor = 1;
	else
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

void CXDataLogicItem::SetAutoLogic(
	int nId,int nInType,int nOutType,int nEqName,int nOutCond
	,CStringArray * pArrBuild,CStringArray * pArrStair
	,int nStartLevelNum,int nEndLevelNum,BYTE btFromOperator,BYTE btToOperator
	,BYTE btEmer,BYTE btSameout,int nNFloorStart,int nNFloorEnd
	,BYTE btUseUnderLogic,BYTE btUseParkLogic
	,BYTE btMatchGround,BYTE btMatchUnder,BYTE btMatchPark
	,BYTE btUnder1F,BYTE btUnderB1F
)
{
	m_nLgId = nId;
	m_nInType = nInType;
	m_nOutType = nOutType;
	m_nEqName = nEqName;
	m_nOutContents = nOutCond;

	CopyArray(&m_arrBuildName,pArrBuild);
	CopyArray(&m_arrStairName,pArrStair);

	m_nInEndLevelNum = nEndLevelNum;
	m_nInStartLevelNum = nStartLevelNum;
	m_btInToOperator = btToOperator;
	m_btInFromOperator = btFromOperator;

	if(m_nInStartLevelNum == 0 && m_nInEndLevelNum == 0)
		m_btUseLevelRange = 0;
	else
		m_btUseLevelRange = 1;




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

	if(m_btUnder1F == TRUE)
	{
		// 출력 대상 : 지하 전체
		// START =0 , END = -1 :  대상 <= END
		m_nOutEndLevelNum = -1;
		m_nOutStartLevelNum = 0;
		m_btOutToOperator = L_OP_LESSEQUAL; // 지하 1층 이하 
		m_btOutFromOperator = L_OP_LESSEQUAL;
	}

	if(m_btUnderB1F == TRUE)
	{
		// 출력 대상 : 지상 1층
		// START =1 , END = 1 :  START == 대상 == END
		m_nOutEndLevelNum = 1;
		m_nOutStartLevelNum = 1;
		m_btOutToOperator = L_OP_EQUALEQUAL;
		m_btOutFromOperator = L_OP_EQUALEQUAL;
	}
}


void CXDataLogicItem::SetAutoLogic(
	int nId,int nInType,int nOutType,int nEqName,int nOutCond
	,CStringArray * pArrBuild,CStringArray * pArrStair
	,int nStartLevelNum,int nEndLevelNum,BYTE btFromOperator,BYTE btToOperator
	,BYTE btEmer,BYTE btSameout,int nNFloorStart,int nNFloorEnd
	,BYTE btUseUnderLogic,BYTE btUseParkLogic
	,BYTE btMatchGroundBuild,BYTE btMatchGroundBType,BYTE btMatchGroundStair,BYTE btMatchGroundFloor,BYTE btMatchGroundRoom
	,BYTE btMatchUnderBuild,BYTE btMatchUnderBType,BYTE btMatchUnderStair,BYTE btMatchUnderFloor,BYTE btMatchUnderRoom
	,BYTE btMatchParkBuild,BYTE btMatchParkBType,BYTE btMatchParkStair,BYTE btMatchParkFloor,BYTE btMatchParkRoom
	,BYTE btUnder1F,BYTE btUnderB1F
)
{
	m_nLgId = nId;
	m_nInType = nInType;
	m_nOutType = nOutType;
	m_nEqName = nEqName;
	m_nOutContents = nOutCond;

	CopyArray(&m_arrBuildName,pArrBuild);
	CopyArray(&m_arrStairName,pArrStair);

	m_nInEndLevelNum = nEndLevelNum;
	m_nInStartLevelNum = nStartLevelNum;
	m_btInToOperator = btToOperator;
	m_btInFromOperator = btFromOperator;

	if(m_nInStartLevelNum == 0 && m_nInEndLevelNum == 0)
		m_btUseLevelRange = 0;
	else
		m_btUseLevelRange = 1;




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
	if(m_btUnder1F == TRUE)
	{
		// 출력 대상 : 지하 전체
		// START =0 , END = -1 :  대상 <= END
		m_nOutEndLevelNum = -1;
		m_nOutStartLevelNum = 0;
		m_btOutToOperator = L_OP_LESSEQUAL; // 지하 1층 이하 
		m_btOutFromOperator = L_OP_LESSEQUAL;
	}

	if(m_btUnderB1F == TRUE)
	{
		// 출력 대상 : 지상 1층
		// START =1 , END = 1 :  START == 대상 == END
		m_nOutEndLevelNum = 1;
		m_nOutStartLevelNum = 1;
		m_btOutToOperator = L_OP_EQUALEQUAL;
		m_btOutFromOperator = L_OP_EQUALEQUAL;
	}
}

void CXDataLogicItem::SetLogicMst(int nId,int nInType,int nOutType,int nEqName,int nOutCond,BYTE btUseRangeLogicOverfloor)
{
	m_nLgId = nId;
	m_nInType = nInType;
	m_nOutType = nOutType;
	m_nEqName = nEqName;
	m_nOutContents = nOutCond;
	m_btUseRangeLogicOverFloor = btUseRangeLogicOverfloor;
}

void CXDataLogicItem::SetLogicInputLoc(CStringArray * pArrBuild,CStringArray * pArrStair
	,int nStartLevelNum,int nEndLevelNum,BYTE btFromOperator,BYTE btToOperator
	)
{
	CopyArray(&m_arrBuildName,pArrBuild);
	CopyArray(&m_arrStairName,pArrStair);

	m_nInEndLevelNum = nEndLevelNum;
	m_nInStartLevelNum = nStartLevelNum;
	m_btInToOperator = btToOperator;
	m_btInFromOperator = btFromOperator;

	if(m_nInStartLevelNum == 0 && m_nInEndLevelNum == 0)
		m_btUseLevelRange = 0;
	else
		m_btUseLevelRange = 1;
}


void CXDataLogicItem::SetLogicOutputCondition(
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
	// 	if(m_btUnder1F == TRUE)
	// 	{
	// 		// 출력 대상 : 지하 전체
	// 		// START =0 , END = -1 :  대상 <= END
	// 		m_nOutEndLevelNum = -1;
	// 		m_nOutStartLevelNum = 0;
	// 		m_btOutToOperator = L_OP_LESSEQUAL; // 지하 1층 이하 
	// 		m_btOutFromOperator = L_OP_LESSEQUAL;
	// 	}
	// 
	// 	if(m_btUnderB1F == TRUE)
	// 	{
	// 		// 출력 대상 : 지상 1층
	// 		// START =1 , END = 1 :  START == 대상 == END
	// 		m_nOutEndLevelNum = 1;
	// 		m_nOutStartLevelNum = 1;
	// 		m_btOutToOperator = L_OP_EQUALEQUAL;
	// 		m_btOutFromOperator = L_OP_EQUALEQUAL;
	// 	}
}

BYTE CXDataLogicItem::IsNeedCheckMatchRoom(CXDataDev * pInputDev)
{
	if(pInputDev == nullptr)
		return 0;
	int nFNum = pInputDev->GetLocFloorNumber();
	if(m_btUseUnderLogic == 0 || nFNum > 1)
		return GetMatchGroundRoom();

	if(nFNum == 1)
	{
		if(m_btUnder1F)
			return 0;
		else
			return GetMatchGroundRoom();
	}
	else if(nFNum > 1)
		return GetMatchGroundRoom();

	return 0;
}

BYTE CXDataLogicItem::CheckMatchLinkedBuild(CXDataDev* pInputDev,CXDataFloor* tgt)
{
	if(pInputDev == nullptr || tgt == nullptr)
		return 0;

	int nInBuild,nOutBuild,nParkBuild;
	std::vector<int> vtTarget;
	nInBuild = pInputDev->GetBuildIndex();
	nOutBuild = tgt->GetBuildIndex();

	// [2026/5/7 12:56:14 KHS] 
	// 주차장일 때 처리 - 연결 건물 처리방안 ? 
// 	nParkBuild = g_MapIdxBuild[L"주차장"];
// 	if(GetUseParkLogic() == 1
// 		&& (nInBuild == nParkBuild || nOutBuild == nParkBuild))
// 	{
// 		return 1;
// 	}

	if(nInBuild <= 0 || nOutBuild <= 0)
		return 0; 
	// 1. 입력이 연계건물의 Source , 출력이 연계건물의 대상 일 때 
	//     -->출력건물에 대상건물이 포함되는지 확인 : 포함될 때 Return 1
	// 2. 입력이 연계건물의 Source , 출력이 연계건물의 대상이 아닐 때
	//     --> 출력 건물에 대상 건물이 포함 되는지 확인 : 포함될 때 Return 1
	// 3. 입력이 연계건물의 Source가 아니고 , 출력이 연계건물의 대상 일 때
	// 4. 입력이 연계건물의 Source가 아니고 , 출력이 연계건물의 대상 아닐 때

	if(g_MapIdxLinkedBuild.find(nInBuild) != g_MapIdxLinkedBuild.end())
	{
		// 1. 입력이 연계건물의 Source , 출력이 연계건물의 대상 일 때 
		//     -->출력건물에 대상건물이 포함되는지 확인
		// 2. 입력이 연계건물의 Source , 출력이 연계건물의 대상이 아닐 때
		//     --> 출력 건물에 대상 건물이 포함 되는지 확인
		vtTarget = g_MapIdxLinkedBuild[nInBuild];
		// 대상건물에 출력 건물이 포함 되는지 확인
		if(std::find(vtTarget.begin(),vtTarget.end(),nOutBuild) != vtTarget.end())
			return 1;
		else
			return 0; 
	}

	if(g_MapIdxLinkedBuild.find(nOutBuild) != g_MapIdxLinkedBuild.end())
	{
		// 3. 입력이 연계건물의 Source가 아니고 , 출력이 연계건물의 대상 일 때
		// 4. 입력이 연계건물의 Source가 아니고 , 출력이 연계건물의 대상 아닐 때
		vtTarget = g_MapIdxLinkedBuild[nOutBuild];
		// 대상건물에 출력 건물이 포함 되는지 확인
		if(std::find(vtTarget.begin(),vtTarget.end(),nInBuild) != vtTarget.end())
			return 1;
		else
			return 0;
	}

	return 0;
}

BOOL CXDataLogicItem::MatchBuild(CXDataDev* src,CXDataFloor* tgt,BOOL ground)
{
	if(ground)
		return GetMatchGroundBuild() ? (src->GetBuildIndex() == tgt->GetBuildIndex()) : true;
	else
		return GetMatchUnderBuild() ? (src->GetBuildIndex() == tgt->GetBuildIndex()) : true;
}

BOOL CXDataLogicItem::MatchBType(CXDataDev* src,CXDataFloor* tgt,BOOL ground)
{
	if(ground)
		return GetMatchGroundBType() ? (src->GetBTypeIndex() == tgt->GetBtypeIndex()) : true;
	else
		return GetMatchUnderBType() ? (src->GetBTypeIndex() == tgt->GetBtypeIndex()) : true;
}

BOOL CXDataLogicItem::MatchStair(CXDataDev* src,CXDataFloor* tgt,BOOL ground)
{
	if(ground)
		return GetMatchGroundStair() ? (src->GetStairIndex() == tgt->GetStairIndex()) : true;
	else
		return GetMatchUnderStair() ? (src->GetStairIndex() == tgt->GetStairIndex()) : true;
}

BOOL CXDataLogicItem::MatchFloorRange(CXDataDev* src,CXDataFloor* tgt)
{
	// [2026/6/10 10:07:16 KHS] 
	// 발생 층이 지하 일때도 범위 적용
	// 지하 로직 - 사용 때 : ?
	//          - 사용하지 않을 때 : ?
	BOOL bRet = FALSE;
	int nSrcFlNum = src->GetLocFloorNumber();
	int nTgtFlNum = tgt->GetFloorNumber();
	if(GetMatchGroundFloor() == 1)
	{
		if(nTgtFlNum >= nSrcFlNum + m_nPlusNStart
			&& nTgtFlNum <= nSrcFlNum + m_nPlusNEnd)
		{
			bRet = TRUE;
		}
		else 
			bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}
	return bRet;
}

BOOL CXDataLogicItem::MatchEmergency(CXDataDev* src,CXDataEm* tgt)
{
	int n1,n2,n3;
	int nBuild,nStair,nFloor;
	int nSrcFlNum = src->GetLocFloorNumber();
	nBuild = nStair = nFloor = 0;
	if(m_btEmergency == 0)
		return FALSE;
	if(m_btUseUnderLogic == 0 || nSrcFlNum > 0)
	{
		n1 = GetMatchGroundBuild() ? (src->GetBuildIndex() == tgt->GetBuildIndex()) : 1;
		n2 = GetMatchGroundStair() ? (src->GetStairIndex() == tgt->GetStairIndex()) : 1;
		// [2026/5/7 17:11:29 KHS] 
		// 비상방송
		if(m_btPriority != MAINLOGIC_PRIORITYID) // 범위로직일때 비상방송은 무조건 층일치 BY 표종완
			n3 = (src->GetFloorIndex() == tgt->GetFloorIndex());
		else
			n3 = GetMatchGroundFloor() ? (src->GetFloorIndex() == tgt->GetFloorIndex()) : 1;
	}
	else
	{
		n1 = GetMatchUnderBuild() ? (src->GetBuildIndex() == tgt->GetBuildIndex()) : 1;
		n2 = GetMatchUnderStair() ? (src->GetStairIndex() == tgt->GetStairIndex()) : 1;
		// [2026/5/7 17:11:29 KHS] 
		// 비상방송
		if(m_btPriority != MAINLOGIC_PRIORITYID) // 범위로직일때 비상방송은 무조건 층일치 BY 표종완
			n3 = (src->GetFloorIndex() == tgt->GetFloorIndex());
		else
			n3 = GetMatchUnderFloor() ? (src->GetFloorIndex() == tgt->GetFloorIndex()) : 1;
	}
	if(n1 == 1 && n2 == 1 && n3 == 1)
		return TRUE;
	return FALSE;
}

BOOL CXDataLogicItem::CheckInputRangeBuild(int nBuildIdx)
{
	int nSize,i;
	CString strBuildName;
	CXLocStrMap::iterator it;
	
	nSize = m_arrBuildName.GetSize();
	if(nSize <= 0)
		return TRUE;
	for(i = 0; i < nSize; i++)
	{
		strBuildName = m_arrBuildName.GetAt(i);
		if(strBuildName.GetLength() <= 0)
			continue;

		it = g_MapIdxBuild.find(strBuildName);
		if(it == g_MapIdxBuild.end())
			continue;

		if(nBuildIdx == it->second)
			return TRUE;
	}
	return FALSE;
}


BOOL CXDataLogicItem::CheckInputRangeStair(int nStairIndex)
{
	int nSize,i;
	CString strStairName;
	CXLocStrMap::iterator it;
	nSize = m_arrStairName.GetSize();
	if(nSize <= 0)
		return TRUE;
	for(i = 0; i < nSize; i++)
	{
		strStairName = m_arrStairName.GetAt(i);
		if(strStairName.GetLength() <= 0)
			continue;

		it = g_MapIdxStair.find(strStairName);
		if(it == g_MapIdxStair.end())
			continue;

		if(nStairIndex == it->second)
			return TRUE;
	}
	return FALSE;
}