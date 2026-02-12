#pragma once

class CXDataDev;
class CXDataFloor;
class CXDataEm;
class CXDataLogicItem : public CObject
{
public:
	CXDataLogicItem();
	virtual ~CXDataLogicItem();
public:
	SIMPLE_FUNC_IMPL(int,LgId,m_nLgId);
	SIMPLE_FUNC_IMPL(int,InType,m_nInType);
	SIMPLE_FUNC_IMPL(int,OutType,m_nOutType);
	SIMPLE_FUNC_IMPL(int,EqName,m_nEqName);
	SIMPLE_FUNC_IMPL(int,OutContents,m_nOutContents);
	//입력 조건
	SIMPLE_FUNC_IMPL(BYTE,Priority,m_btPriority);
	SIMPLE_FUNC_IMPL(BYTE,UseLevelRange,m_btUseLevelRange);

	SIMPLE_FUNC_IMPL(BYTE,InFromOperator,m_btInFromOperator);
	SIMPLE_FUNC_IMPL(BYTE,InToOperator,m_btInToOperator);
	SIMPLE_FUNC_IMPL(int,InStartLevelNum,m_nInStartLevelNum);
	SIMPLE_FUNC_IMPL(int,InEndLevelNum,m_nInEndLevelNum);
	SIMPLE_FUNC_IMPL(CString,StartLevelName,m_strStartLevelName);
	SIMPLE_FUNC_IMPL(CString,EndLevelName,m_strEndLevelName);


	SIMPLE_FUNC_IMPL(BYTE,OutFromOperator,m_btOutFromOperator);
	SIMPLE_FUNC_IMPL(BYTE,OutToOperator,m_btOutToOperator);
	SIMPLE_FUNC_IMPL(int,OutStartLevelNum,m_nOutStartLevelNum);
	SIMPLE_FUNC_IMPL(int,OutEndLevelNum,m_nOutEndLevelNum);

	// 	SIMPLE_FUNC_IMPL(std::vector<int>,InputBuildIds,m_vtInputBuildIds);
	// 	SIMPLE_FUNC_IMPL(CStringArray,BuildName,m_arrBuildName);
	// 	SIMPLE_FUNC_IMPL(std::vector<int>,InputStairs,m_vtInputStairs);
	// 	SIMPLE_FUNC_IMPL(CStringArray,StairName,m_arrStairName);


	//출력 조건
	SIMPLE_FUNC_IMPL(BYTE,UseEmergency,m_btEmergency);
	SIMPLE_FUNC_IMPL(BYTE,UseSameAddrOutput,m_btSameAddrOutput);
	SIMPLE_FUNC_IMPL(int,UsePluseNFloor,m_nPluseNFloor);
	SIMPLE_FUNC_IMPL(BYTE,Under1F,m_btUnder1F);
	SIMPLE_FUNC_IMPL(BYTE,UnderB1F,m_btUnderB1F);


	SIMPLE_FUNC_IMPL(BYTE,MatchGroundCondition,m_suMatchGround.btLoctype);

	SIMPLE_FUNC_IMPL(BYTE,MatchGroundBuild,m_suMatchGround.stLoctype.btBuild);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundBType,m_suMatchGround.stLoctype.btBType);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundStair,m_suMatchGround.stLoctype.btStair);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundFloor,m_suMatchGround.stLoctype.btFloor);
	SIMPLE_FUNC_IMPL(BYTE,MatchGroundRoom,m_suMatchGround.stLoctype.btRoom);

	SIMPLE_FUNC_IMPL(BYTE,MatchUnderCondition,m_suMatchUnder.btLoctype);

	SIMPLE_FUNC_IMPL(BYTE,MatchUnderBuild,m_suMatchUnder.stLoctype.btBuild);
	SIMPLE_FUNC_IMPL(BYTE,MatchUnderBType,m_suMatchUnder.stLoctype.btBType);
	SIMPLE_FUNC_IMPL(BYTE,MatchUnderStair,m_suMatchUnder.stLoctype.btStair);
	SIMPLE_FUNC_IMPL(BYTE,MatchUnderFloor,m_suMatchUnder.stLoctype.btFloor);
	SIMPLE_FUNC_IMPL(BYTE,MatchUnderRoom,m_suMatchUnder.stLoctype.btRoom);

	SIMPLE_FUNC_IMPL(BYTE,MatchParkCondition,m_suMatchPark.btLoctype);

	SIMPLE_FUNC_IMPL(BYTE,MatchParkBuild,m_suMatchPark.stLoctype.btBuild);
	SIMPLE_FUNC_IMPL(BYTE,MatchParkBType,m_suMatchPark.stLoctype.btBType);
	SIMPLE_FUNC_IMPL(BYTE,MatchParkStair,m_suMatchPark.stLoctype.btStair);
	SIMPLE_FUNC_IMPL(BYTE,MatchParkFloor,m_suMatchPark.stLoctype.btFloor);
	SIMPLE_FUNC_IMPL(BYTE,MatchParkRoom,m_suMatchPark.stLoctype.btRoom);

	SIMPLE_FUNC_IMPL(BYTE,UseUnderLogic,m_btUseUnderLogic);
	SIMPLE_FUNC_IMPL(BYTE,UseParkLogic,m_btUseParkLogic);

	CStringArray * GetBuildArray() { return &m_arrBuildName; }
	CStringArray * GetStairArray() { return &m_arrStairName; }

protected:
	int m_nLgId;
	int		m_nInType;
	int		m_nOutType;
	int		m_nEqName;
	int		m_nOutContents;
	// [2025/8/7 12:54:55 KHS] 
	// 입력 조건
	// m_btPriority --> 
	// 99은 기본 조건 : 건물,층,계단,지하에 대한 입력범위조건을 다 적용하고 남은 부분에 적용되는 범위
	BYTE						m_btPriority; // 로직안에서 Logic Item의 우선순위
	std::vector<int>			m_vtInputBuildIds;
	CStringArray				m_arrBuildName;
	std::vector<int>			m_vtInputStairs;
	CStringArray				m_arrStairName;
	BYTE						m_btUseLevelRange; // 입력 범위 사용
	BYTE						m_btInFromOperator; // 이상 >= , 초과 >
	BYTE						m_btInToOperator; // 이하 <= , 미만 <
	int							m_nInStartLevelNum; // 0 일때 사용안함 , 기본적으로 층번호(PIT층)
	int							m_nInEndLevelNum; // 0 일때 사용안함
												  // 1. m_nStartLevel == 0 이고 m_nEndLevel <= 5 일때 5층이하 모두
												  // 2. m_nStartLevel >= 1 이고 m_nEndLevel == 0일때 1층이상 모두
												  // 3. m_nStartLevel,m_nEndLevel == 0일때 부등호는 무시
												  // 4. 작은 값이 m_nStartLevel ,
												  // 5. m_nEndLevel이하 

												  // [2025/8/19 8:36:00 KHS] 
												  // 현재는 사용하지 않지만 향후 층이름으로 검색할 수 있도록 변수만 지정한다
	CString						m_strStartLevelName; // 층이름을 층번호로 바꾼다.
	CString						m_strEndLevelName;

	// [2025/8/19 8:36:56 KHS] 
	// 로직의 조건에 따라 출력 대상을 프로그램적으로 설정
	// 기본적으로 출력대상은 입력 대상과 동일
	// 단. 1층발화 시 지하 전체의 경우 출력 대상은 지하 전체
	// 층 출력 조건이 하나도 없을 시 실 조건으로 이동한다.
	// 층 조건 : 층Match , +(-)N층
	// 1층로직 : 대상은 지하 전체 , 화재시 전체 출력(해당 층 전체:건물,종류,계단 확인)
	// 지하1층 로직 : 대상은 1층 전체(해당 층 전체:건물,종류,계단 확인)

	BYTE						m_btOutFromOperator; // 이상 >= , 초과 >
	BYTE						m_btOutToOperator; // 이하 <= , 미만 <
	int							m_nOutStartLevelNum; // 0 일때 사용안함 , 기본적으로 층번호(PIT층)
	int							m_nOutEndLevelNum; // 0 일때 사용안함

	int		m_nPluseNFloor;
	BYTE	m_btEmergency;
	BYTE	m_btSameAddrOutput; // 입력과 같은 어드레스

	BYTE	m_btUnder1F; // 1층 화재시 지하 전체 - 1층이 들어갈 이
	BYTE	m_btUnderB1F; // 지하 1층 화재시 1층 출력

	SU_LOCTYPEBIT	m_suMatchGround;
	SU_LOCTYPEBIT	m_suMatchUnder; //
	SU_LOCTYPEBIT	m_suMatchPark;

	BYTE	m_btUseUnderLogic;
	BYTE	m_btUseParkLogic;


	// [2025/8/20 13:32:11 KHS] 
	// 지하 로직 일때 주차장 로직 필요
	// 주차장은 건물명에 주차장@ 또는 @PK 포함
	// 1. 지하로직
	// 지하 로직 일때 주차장로직 설정 시 주차장건물이 있는지 확인
	// 있으면 주차장 추가

	// 2. 주차장 로직 : 주차장 체크 시
	// 주차장 로직에 맞는 옵션 

	//BYTE	m_btUseParkingLogic; // 주차장로직
public:

	// 조건이 맞는지가 아니고 조건이 있는지 확인
	// 조건이 없으면 다음 조건(계단,층...)으로 이동
	BYTE IsNeedCheckMatchBuild(CXDataDev * pInputDev);
	BYTE IsNeedCheckMatchBType(CXDataDev * pInputDev);
	BYTE IsNeedCheckMatchStair(CXDataDev * pInputDev);

	BYTE IsNeedCheckMatchFloor(CXDataDev * pInputDev);
	BYTE IsNeedCheckMatchRoom(CXDataDev * pInputDev);

	// 주어진 층이 로직 범위에 있는지 확인한다.
	BOOL CheckOutputRange(CXDataFloor * pFloor);

	//BOOL CheckFloorCondition(CXDataDev * pInputDev,CXDataFloor * pFloor);

	void SetLogicMst(int nId,int nInType,int nOutType,int nEqName,int nOutCond);
	void SetLogicInputLoc(CStringArray * pArrBuild,CStringArray * pArrStair
		,int nStartLevelNum,int nEndLevelNum,BYTE btFromOperator,BYTE btToOperator
		,CString strStartLevelName,CString strEndLevelName);
	void SetLogicOutputCondition(
		BYTE btEmer,BYTE btSameout,int nNFloor
		,BYTE btUseUnderLogic,BYTE btUseParkLogic
		,BYTE btUnder1F,BYTE btUnderB1F
	);

	void SetAutoLogic(
		int nId,int nInType,int nOutType,int nEqName,int nOutCond
		,CStringArray * pArrBuild,CStringArray * pArrStair
		,int nStartLevelNum,int nEndLevelNum,BYTE btFromOperator,BYTE btToOperator
		,CString strStartLevelName,CString strEndLevelName
		,BYTE btEmer,BYTE btSameout,int nNFloor
		,BYTE btUseUnderLogic,BYTE btUseParkLogic
		,BYTE btMatchGround,BYTE btMatchUnder,BYTE btMatchPark
		,BYTE btUnder1F,BYTE btUnderB1F
	);

	void SetAutoLogic(
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
	);

	BYTE SetMatchCondition(BYTE btType,BYTE btMatchBuild,BYTE btMatchBType,BYTE btMatchStair,BYTE btMatchFloor,BYTE btMatchRoom);

	void CopyArray(CStringArray * pTarget,CStringArray * pSource);

	BOOL MatchBuild(CXDataDev* src,CXDataFloor* tgt,BOOL ground);
	BOOL MatchBType(CXDataDev* src,CXDataFloor* tgt,BOOL ground);
	BOOL MatchStair(CXDataDev* src,CXDataFloor* tgt,BOOL ground);
	BOOL MatchFloorRange(CXDataDev* src,CXDataFloor* tgt);

	BOOL MatchEmergency(CXDataDev* src,CXDataEm* tgt);
};

