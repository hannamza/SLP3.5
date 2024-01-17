#pragma once

// CDataAutoLogic 명령 대상입니다.

class CDataAutoLogic : public CObject
{
public:
	CDataAutoLogic();
	virtual ~CDataAutoLogic();

	SIMPLE_FUNC_IMPL(int, LgId, m_nLgId);
	SIMPLE_FUNC_IMPL(int, InType, m_nInType);
	SIMPLE_FUNC_IMPL(int, OutType, m_nOutType);
	SIMPLE_FUNC_IMPL(int, EqName, m_nEqName);
	SIMPLE_FUNC_IMPL(int, OutContents, m_nOutContents);

// 	SIMPLE_FUNC_IMPL(BYTE, UseUnderFloor, m_btUseUnderFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UseAllFloor, m_btUseAllFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UseCurFloor, m_btUseCurFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UsePlusOneFloor, m_btUsePlusOneFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UseUpperFloor, m_btUseUpperFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UseOutput, m_btUseOutput);
// 	SIMPLE_FUNC_IMPL(BYTE, UseEmergency, m_btUseEmergency);
// 	SIMPLE_FUNC_IMPL(BYTE, UseRoom, m_btUseRoom);
//	SIMPLE_FUNC_IMPL(CString, ContName, m_strOutContents);
// 
// 	SIMPLE_FUNC_IMPL(BYTE, UseUnderFloor, m_btUseUnderFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UseAllFloor, m_btUseAllFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UseCurFloor, m_btUseCurFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UsePlusOneFloor, m_btUsePlusOneFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UseUpperFloor, m_btUseUpperFloor);
// 	SIMPLE_FUNC_IMPL(BYTE, UseOutput, m_btUseOutput);
// 

	SIMPLE_FUNC_IMPL(BYTE, UseAllFloor, m_btAllFloor);
	SIMPLE_FUNC_IMPL(BYTE, UseEmergency, m_btEmergency);
	SIMPLE_FUNC_IMPL(BYTE, UseOutput, m_btOutput);
	SIMPLE_FUNC_IMPL(BYTE, UsePluseNFloor, m_btPluseNFloor);
						   
	SIMPLE_FUNC_IMPL(BYTE, UseMatchBuild, m_btMatchBuild);
	SIMPLE_FUNC_IMPL(BYTE, UseMatchBType, m_btMatchBType);
	SIMPLE_FUNC_IMPL(BYTE, UseMatchStair, m_btMatchStair);
	SIMPLE_FUNC_IMPL(BYTE, UseMatchFloor, m_btMatchFloor);
	SIMPLE_FUNC_IMPL(BYTE, UseMatchRoom, m_btMatchRoom);
						   
	SIMPLE_FUNC_IMPL(BYTE, UseUnderBasic, m_btUnderBasic);
	SIMPLE_FUNC_IMPL(BYTE, UseUnderClassBuild, m_btUnderClassBuild);
	SIMPLE_FUNC_IMPL(BYTE, UseUnderClassBType, m_btUnderClassBType);
	SIMPLE_FUNC_IMPL(BYTE, UseUnderClassStair, m_btUnderClassStair);
	SIMPLE_FUNC_IMPL(BYTE, UseUnder1F, m_btUnder1F);
	SIMPLE_FUNC_IMPL(BYTE, UseUnderB1F, m_btUnderB1F);
	SIMPLE_FUNC_IMPL(BYTE, UseUnderParking, m_btUnderParking);

protected:
	int m_nLgId;
	int m_nInType;
	int m_nOutType;
	int m_nEqName;
	int m_nOutContents;
	
// 	BYTE m_btUseUnderFloor; //< 지하로직 사용
// 	BYTE m_btUseAllFloor; //< 전층에 있는 해당 출력 , 경종이면 경종만
// 	BYTE m_btUseCurFloor; //< 현재층
// 	BYTE m_btUsePlusOneFloor; //< 직상층
// 	BYTE m_btUseUpperFloor; //< N층 위까지
// 	BYTE m_btUseOutput; //< 해당 디바이스의 출력
// 	BYTE m_btUseEmergency; //< 비상방송 연동
// 	BYTE m_btUseRoom; ///< 실구분

//	CString		m_strOutContents;


	BYTE	m_btAllFloor;
	BYTE	m_btEmergency;
	BYTE	m_btOutput;
	BYTE	m_btPluseNFloor;

	BYTE	m_btMatchBuild;
	BYTE	m_btMatchBType;
	BYTE	m_btMatchStair;
	BYTE	m_btMatchFloor;
	BYTE	m_btMatchRoom;

	BYTE	m_btUnderBasic;
	BYTE	m_btUnderClassBuild;
	BYTE	m_btUnderClassBType;
	BYTE	m_btUnderClassStair;
	BYTE	m_btUnder1F;
	BYTE	m_btUnderB1F;

	BYTE	m_btUnderParking;
public:
	void SetAutoLogic(
		int nId, int nIntype, int nOuttype, int nName, int nCont
		, BYTE btEmer, BYTE btAll, BYTE btout , BYTE btNFloor
		, BYTE btMatchBuild, BYTE btMatchBType, BYTE btMatchStair, BYTE btMatchFloor, BYTE btMatchRoom
		, BYTE btUnderBasic, BYTE btUnderBuild, BYTE btUnderBType, BYTE btUnderStair, BYTE btUnder1F, BYTE btUnderB1F
		, BYTE btUnderParking
		//, CString strName = L""
	);
};


