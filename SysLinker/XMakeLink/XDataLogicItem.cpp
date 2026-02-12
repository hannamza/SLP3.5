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
	m_strEndLevelName = L"";
	m_strStartLevelName = L"";

	m_nOutStartLevelNum = 0;
	m_nOutEndLevelNum = 0;
	m_btOutToOperator = L_OP_NONE;
	m_btOutFromOperator = L_OP_NONE;

	m_btEmergency = 0;
	m_btSameAddrOutput = 0;
	m_nPluseNFloor = 0;

	m_suMatchUnder.btLoctype = 0;
	m_suMatchGround.btLoctype = 0;
	m_suMatchPark.btLoctype = 0;
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
	if(m_nPluseNFloor != 0)
		suData.stLoctype.btFloor = 1;
	else
		suData.stLoctype.btFloor = btMatchFloor;
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
	,CString strStartLevelName,CString strEndLevelName
	,BYTE btEmer,BYTE btSameout,int nNFloor
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
	m_strEndLevelName = strEndLevelName;
	m_strStartLevelName = strStartLevelName;
	m_btInToOperator = btToOperator;
	m_btInFromOperator = btFromOperator;

	if(m_nInStartLevelNum == 0 && m_nInEndLevelNum == 0)
		m_btUseLevelRange = 0;
	else
		m_btUseLevelRange = 1;




	m_btEmergency = btEmer;

	m_btSameAddrOutput = btSameout;
	m_nPluseNFloor = nNFloor;

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
	,CString strStartLevelName,CString strEndLevelName
	,BYTE btEmer,BYTE btSameout,int nNFloor
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
	m_strEndLevelName = strEndLevelName;
	m_strStartLevelName = strStartLevelName;
	m_btInToOperator = btToOperator;
	m_btInFromOperator = btFromOperator;

	if(m_nInStartLevelNum == 0 && m_nInEndLevelNum == 0)
		m_btUseLevelRange = 0;
	else
		m_btUseLevelRange = 1;




	m_btEmergency = btEmer;

	m_btSameAddrOutput = btSameout;
	m_nPluseNFloor = nNFloor;

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

void CXDataLogicItem::SetLogicMst(int nId,int nInType,int nOutType,int nEqName,int nOutCond)
{
	m_nLgId = nId;
	m_nInType = nInType;
	m_nOutType = nOutType;
	m_nEqName = nEqName;
	m_nOutContents = nOutCond;
}

void CXDataLogicItem::SetLogicInputLoc(CStringArray * pArrBuild,CStringArray * pArrStair
	,int nStartLevelNum,int nEndLevelNum,BYTE btFromOperator,BYTE btToOperator
	,CString strStartLevelName,CString strEndLevelName)
{
	CopyArray(&m_arrBuildName,pArrBuild);
	CopyArray(&m_arrStairName,pArrStair);

	m_nInEndLevelNum = nEndLevelNum;
	m_nInStartLevelNum = nStartLevelNum;
	m_strEndLevelName = strEndLevelName;
	m_strStartLevelName = strStartLevelName;
	m_btInToOperator = btToOperator;
	m_btInFromOperator = btFromOperator;

	if(m_nInStartLevelNum == 0 && m_nInEndLevelNum == 0)
		m_btUseLevelRange = 0;
	else
		m_btUseLevelRange = 1;
}

void CXDataLogicItem::SetLogicOutputCondition(
	BYTE btEmer,BYTE btSameout,int nNFloor
	,BYTE btUseUnderLogic,BYTE btUseParkLogic
	,BYTE btUnder1F,BYTE btUnderB1F
)
{
	m_btEmergency = btEmer;

	m_btSameAddrOutput = btSameout;
	m_nPluseNFloor = nNFloor;

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


BYTE CXDataLogicItem::IsNeedCheckMatchBuild(CXDataDev * pInputDev)
{
	if(pInputDev == nullptr)
		return 0;
	int nFNum = pInputDev->GetLocFloorNumber();
	// 	CString strPark;
	// 	strPark = pInputDev->GetLocRoomName();

	// 	if(m_btUseUnderLogic && m_btUseParkLogic && nFNum <= 1)
	// 		return 0; 

	//지하 주차장 로직 - 입력이 지하 주차장이면 지하 전체 
	// 계단에서 층조건 검색 - 건물,계단 상관없이 조건에 대한 판단은 계단에서 실행 
	// 	if(strPark.Find(PARKING_SUBSCRIPT) >= 0)
	// 	{
	// 		if(m_btUseUnderLogic && m_btUseParkLogic)
	// 			return 0; 
	// 	}
	// 건물명을 체크 할 수 없는 경우 - 대상이 되는 출력 회로의 정보가 없다 
	// 1. 지상1F로직 : 출력이 지상부분은 건물일치가 체크 , 지하 부분은 건물일치가 체크되지 않을 경우
	//                건물명 일치하는지는 건물정보에서는 체크 할 수없다.
	// 2. 지하1F로직 : 위와 같은 이유
	// 3. 주차장 : 위와 같은 이유
	// 따라서 이부분은 층정보에서 확인해야된다.
	// 지상1F로직,지하1F로직,주차장 로직이 있으면 비교할 내용이 많아진다.
	// 각 단계(건물,종류,계단)에서 확인 할 경우 비교 내용이 줄어들 수도 있다

	// 입력이 지상 1F일때 지하 전체 출력이 있을 때
	// 지하 1층 발화 시 지상1층 출력 있을 때
	if(nFNum <= 1)
	{
		// 대상이 되는 출력회로의 위치가 정해지지 않았기 때문에 
		// 지하로직,주차장 로직이 설정되어 있으면 
		// 건물 정보를 체크하지 않는다
		if(m_btUseUnderLogic)
		{
			//return 0; // 건물정보 체크 할 수 없음
			if(m_btUseParkLogic)
			{
				// 2개 이상 확인해야되서
				return 1;
			}
			else
				return GetMatchUnderBuild();
		}
		else
			return GetMatchGroundBuild();
	}
	else
		return GetMatchGroundBuild();

	return 0;
}


BYTE CXDataLogicItem::IsNeedCheckMatchBType(CXDataDev * pInputDev)
{
	if(pInputDev == nullptr)
		return 0;
	int nFNum = pInputDev->GetLocFloorNumber();
	// 	CString strPark;
	// 	strPark = pInputDev->GetLocRoomName();
	// 
	// 	//지하 주차장 로직 - 입력이 지하 주차장이면 지하 전체 
	// 	// 계단에서 층조건 검색 - 건물,계단 상관없이 조건에 대한 판단은 계단에서 실행 
	// 	if(strPark.Find(PARKING_SUBSCRIPT) >= 0)
	// 	{
	// 		if(m_btUseUnderLogic && m_btUseParkLogic)
	// 			return 0;
	// 	}

	if(nFNum <= 1)
	{
		// 대상이 되는 출력회로의 위치가 정해지지 않았기 때문에 
		// 지하로직,주차장 로직이 설정되어 있으면 
		// 체크하지 않는다
		if(m_btUseUnderLogic)
		{
			//return 0; // 건물정보 체크 할 수 없음
			if(m_btUseParkLogic)
				return GetMatchParkBType();
			else
				return GetMatchUnderBType();
		}
		else
			return GetMatchGroundBType();
	}
	else
		return GetMatchGroundBType();
	return 0;
}

BYTE CXDataLogicItem::IsNeedCheckMatchStair(CXDataDev * pInputDev)
{
	if(pInputDev == nullptr)
		return 0;
	int nFNum = pInputDev->GetLocFloorNumber();
	// 	CString strPark;
	// 	strPark = pInputDev->GetLocRoomName();
	// 
	// 	//지하 주차장 로직 - 입력이 지하 주차장이면 지하 전체 
	// 	// 계단에서 층조건 검색 - 건물,계단 상관없이 조건에 대한 판단은 계단에서 실행 
	// 	if(strPark.Find(PARKING_SUBSCRIPT) >= 0)
	// 	{
	// 		if(m_btUseUnderLogic && m_btUseParkLogic)
	// 			return 0;
	// 	}

	if(nFNum <= 1)
	{
		// 대상이 되는 출력회로의 위치가 정해지지 않았기 때문에 
		// 지하로직,주차장 로직이 설정되어 있으면 
		// 체크하지 않는다
		if(m_btUseUnderLogic)
		{
			if(m_btUseParkLogic)
				return GetMatchParkStair();
			else
				return GetMatchUnderStair();
		}
		else
			return GetMatchGroundStair();
	}
	else
		return GetMatchGroundStair();
	return 0;
}

BYTE CXDataLogicItem::IsNeedCheckMatchFloor(CXDataDev * pInputDev)
{
	if(pInputDev == nullptr)
		return 0;
	int nFNum = pInputDev->GetLocFloorNumber();
	// 	CString strPark;
	// 	strPark = pInputDev->GetLocRoomName();
	// 
	if(m_btUseUnderLogic == 0 || nFNum > 1)
	{
		// +N층 , 건물정보 매칭
		// +N층 일때 자동으로 층 일치가 들어간다 --> 조건이 있다
		return GetMatchGroundFloor();
	}

	// 조건 체크 필요
	if(m_btUnderB1F || m_btUnder1F)
		return 1;

	if(m_btUseUnderLogic)
		return 1;
	return 0;
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

// 
BOOL CXDataLogicItem::CheckOutputRange(CXDataFloor * pFloor)
{
	int nFlNum;
	int nStart,nEnd;
	BYTE btOpStart,btOpEnd;
	BOOL (*leftOp)(int,int) = nullptr; // A >= X ?
	BOOL (*rightOp)(int,int) = nullptr;   // A <= Y ?

	nEnd = GetOutEndLevelNum();
	nStart = GetOutStartLevelNum();
	btOpStart = GetOutFromOperator();
	btOpEnd = GetOutToOperator();
	nFlNum = pFloor->GetFloorNumber();


	// 대상 범위가 설정되지 않음 (전체 출력)
	if(nEnd == 0 && nStart == 0)
		return TRUE;

	switch(btOpStart)
	{
	case L_OP_EQUALEQUAL:
		leftOp = Equal_equal;
		break;
	case L_OP_LESSEQUAL:
		leftOp = Equal_equal;
		break;
	case L_OP_GREATEREQUAL:
		leftOp = Equal_equal;
		break;
	case L_OP_LESS:
		leftOp = Equal_equal;
		break;
	case L_OP_GREATER:
		leftOp = Equal_equal;
		break;
	default:
		leftOp = None_none;
		break;
	}

	switch(btOpStart)
	{
	case L_OP_EQUALEQUAL:
		rightOp = Equal_equal;
		break;
	case L_OP_LESSEQUAL:
		rightOp = Equal_equal;
		break;
	case L_OP_GREATEREQUAL:
		rightOp = Equal_equal;
		break;
	case L_OP_LESS:
		rightOp = Equal_equal;
		break;
	case L_OP_GREATER:
		rightOp = Equal_equal;
		break;
	default:
		rightOp = None_none;
		break;
	}

	if(leftOp(nFlNum,nStart) && rightOp(nFlNum,nEnd))
		return TRUE;
	return FALSE;
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

	BOOL bRet = FALSE;
	int nSrcFlNum = src->GetLocFloorNumber();
	int nTgtFlNum = tgt->GetFloorNumber();
	if(GetMatchGroundFloor() == 1)
	{
		if(m_nPluseNFloor > 0)
			bRet = (nSrcFlNum <= nTgtFlNum && nSrcFlNum + m_nPluseNFloor >= nTgtFlNum) ? TRUE : FALSE;
		else if(m_nPluseNFloor < 0)
			bRet = (nSrcFlNum >= nTgtFlNum && nSrcFlNum + m_nPluseNFloor <= nTgtFlNum) ? TRUE : FALSE;
		else
			bRet = src->GetFloorIndex() == tgt->GetIndex() ? TRUE : FALSE;
	}
	else
		bRet = TRUE;
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
		n3 = GetMatchGroundFloor() ? (src->GetFloorIndex() == tgt->GetFloorIndex()) : 1;
	}
	else
	{
		n1 = GetMatchUnderBuild() ? (src->GetBuildIndex() == tgt->GetBuildIndex()) : 1;
		n2 = GetMatchUnderStair() ? (src->GetStairIndex() == tgt->GetStairIndex()) : 1;
		n3 = GetMatchUnderFloor() ? (src->GetFloorIndex() == tgt->GetFloorIndex()) : 1;
	}
	if(n1 == 1 && n2 == 1 && n3 == 1)
		return TRUE;
	return FALSE;
}
