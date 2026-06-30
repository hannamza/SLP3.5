#pragma once
#include <map>
class CXDataEm;
class CXDataRangeLogic : public CObject
{
public:
	CXDataRangeLogic();
	virtual ~CXDataRangeLogic();

	SIMPLE_FUNC_IMPL(int,RangeId,m_nRangeId);
	SIMPLE_FUNC_IMPL(BYTE,Priority,m_btPriority);
	SIMPLE_FUNC_IMPL(BYTE,UseRangeLogicOverFloor,m_btUseRangeLogicOverFloor);
	SIMPLE_FUNC_IMPL(BYTE,UseBuildRange,m_btUseBuildRange);
	SIMPLE_FUNC_IMPL(BYTE,UseStairRange,m_btUseStairRange);
	SIMPLE_FUNC_IMPL(BYTE,UseFloorRange,m_btUseFloorRange);



	SIMPLE_FUNC_IMPL(int,RangeStartLevelNum,m_nRangeStartLevelNum);
	SIMPLE_FUNC_IMPL(int,RangeEndLevelNum,m_nRangeEndLevelNum);


	SIMPLE_FUNC_IMPL(BYTE,MatchGroundCondition,m_suMatchGround.btLoctype);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundBuild,m_suMatchGround.stLoctype.btBuild);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundBType,m_suMatchGround.stLoctype.btBType);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundStair,m_suMatchGround.stLoctype.btStair);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundFloor,m_suMatchGround.stLoctype.btFloor);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundRoom,m_suMatchGround.stLoctype.btRoom);
	SIMPLE_FUNC_IMPL(int,PlusNStart,m_nPlusNStart);
	SIMPLE_FUNC_IMPL(int,PlusNEnd,m_nPlusNEnd);

	//출력 조건
	SIMPLE_FUNC_IMPL(BYTE,UseEmergency,m_btEmergency);
	SIMPLE_FUNC_IMPL(BYTE,UseSameAddrOutput,m_btSameAddrOutput);
	SIMPLE_FUNC_IMPL(BYTE,Under1F,m_btUnder1F);
	SIMPLE_FUNC_IMPL(BYTE,UnderB1F,m_btUnderB1F);
protected:
	// DB에는 여러 조건이 있지만 실제 사용되지 않고 예약만 되어 있는 필드가 있다
	// 실제 사용되는 데이터
	int							m_nRangeId;
	BYTE						m_btPriority; // 로직안에서 Logic Item의 우선순위
	CString						m_strRangeName;

	BYTE						m_btUseBuildRange;
	BYTE						m_btUseStairRange;
	BYTE						m_btUseFloorRange;

	CStringArray				m_arrBuildName;
	CStringArray				m_arrStairName;
	std::vector<int>			m_vtInputBuildIds;
	std::vector<int>			m_vtInputStairs;

	CString						m_strRangeStartFloor;// DB에 있는 층 숫자 String('11')
	CString						m_strRangeEndFloor;// DB에 있는 층 숫자 String('11')
	int							m_nRangeStartLevelNum; // 0 일때 사용안함 
	int							m_nRangeEndLevelNum; // 0 일때 사용안함

	SU_LOCTYPEBIT				m_suMatchGround;
	BYTE						m_btUseRangeLogicOverFloor;// 1. +N층으로 인해 출력범위가 입력범위를 초과하는 부분에 대한 로직 적용우선순위
														   // TRUE일때 범위로직 우선 적용
														   // 2. 출력이 범위를 걸쳤을 때 (ex:층일치 없음 - 건물 또는 계단 일치)
														   //    101동 1계단 15층 입력 시 101동 + 102동 전체 출력

	// DB 필드 있음,UI 편집기에서 데이터 설정하지 않음
	// 프로그램에서 필요에 의해 기본 로직의 값을 설정 - MakeLinkList에서 while loop 돌면서 기본로직의 값을 바꿔준다.
	int							m_nPlusNStart;
	int							m_nPlusNEnd;

	// 사용되지 않는 DB 필드
	BYTE						m_btMatchAll;
	BYTE						m_btSameAddrOutput; // 입력과 같은 어드레스

	BYTE						m_btUnder1F; // 1층 화재시 지하 전체 - 1층이 들어갈 이
	BYTE						m_btUnderB1F; // 지하 1층 화재시 1층 출력


	SU_LOCTYPEBIT				m_suMatchUnder; //
	SU_LOCTYPEBIT				m_suMatchPark;

	BYTE						m_btEmergency;
	BYTE						m_btUseUnderLogic;
	BYTE						m_btUseParkLogic;


public:
	void SetRangeID(int nId,BYTE btPriority,BYTE btUseRangeOverFloor
		,BYTE btUseBuildRange,BYTE btUseStairRange,BYTE btUseFloorRange
	);
	void SetLogicInputLoc(CStringArray * pArrBuild,CStringArray * pArrStair
		,int nStartLevelNum,int nEndLevelNum
	);
	void SetLogicOutputCondition(
		BYTE btEmer,BYTE btSameout,int nNFloorStart,int nNFloorEnd
		,BYTE btUseUnderLogic,BYTE btUseParkLogic
		,BYTE btUnder1F,BYTE btUnderB1F
	);

	void SetAutoLogic(
		int nId
		,CStringArray * pArrBuild,CStringArray * pArrStair
		,int nStartLevelNum,int nEndLevelNum
		,BYTE btEmer,BYTE btSameout,int nNFloorStart,int nNFloorEnd
		,BYTE btUseUnderLogic,BYTE btUseParkLogic
		,BYTE btMatchGround,BYTE btMatchUnder,BYTE btMatchPark
		,BYTE btUnder1F,BYTE btUnderB1F
	);

	void SetAutoLogic(
		int nId
		,CStringArray * pArrBuild,CStringArray * pArrStair
		,int nStartLevelNum,int nEndLevelNum
		,BYTE btEmer,BYTE btSameout,int nNFloorStart,int nNFloorEnd
		,BYTE btUseUnderLogic,BYTE btUseParkLogic
		,BYTE btMatchGroundBuild,BYTE btMatchGroundBType,BYTE btMatchGroundStair,BYTE btMatchGroundFloor,BYTE btMatchGroundRoom
		,BYTE btMatchUnderBuild,BYTE btMatchUnderBType,BYTE btMatchUnderStair,BYTE btMatchUnderFloor,BYTE btMatchUnderRoom
		,BYTE btMatchParkBuild,BYTE btMatchParkBType,BYTE btMatchParkStair,BYTE btMatchParkFloor,BYTE btMatchParkRoom
		,BYTE btUnder1F,BYTE btUnderB1F
	);

	BYTE SetMatchCondition(BYTE btType,BYTE btMatchBuild,BYTE btMatchBType,BYTE btMatchStair,BYTE btMatchFloor,BYTE btMatchRoom);

	void CopyArray(CStringArray * pTarget,CStringArray * pSource,BOOL bBuild);
	BOOL CheckInputRangeBuild(int nBuildIdx);
	BOOL CheckInputRangeStair(int nStairIndex);
	RANGE_RESULT CheckFloorPosition(int nFloor);

	BOOL MatchBuild(CXDataDev* src,CXDataFloor* tgt,BOOL ground);
	BOOL MatchStair(CXDataDev* src,CXDataFloor* tgt,BOOL ground);
	BOOL MatchFloorRange(CXDataDev* src,CXDataFloor* tgt);

	BOOL InRange(CXDataDev * pInDev);
	BOOL MatchEmergency(CXDataDev * pInDev,CXDataEm * pEm);
};

