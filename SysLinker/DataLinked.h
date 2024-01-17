#pragma once

// CDataLinked 명령 대상입니다.
class CDataEmBc;
class CDataDevice;
class CDataPattern;
class CDataLinked : public CObject
{
public:
	CDataLinked();
	virtual ~CDataLinked();

	SIMPLE_FUNC_IMPL(int, SrcFacp, m_nSrcFacp);
	SIMPLE_FUNC_IMPL(int, SrcUnit, m_nSrcUnit);
	SIMPLE_FUNC_IMPL(int, SrcChn, m_nSrcChn);
	SIMPLE_FUNC_IMPL(int, SrcDev, m_nSrcDev);
	SIMPLE_FUNC_IMPL(int, TgtFacp, m_nTgtFacp);
	SIMPLE_FUNC_IMPL(int, TgtUnit, m_nTgtUnit);
	SIMPLE_FUNC_IMPL(int, TgtChn, m_nTgtChn);
	SIMPLE_FUNC_IMPL(int, TgtDev, m_nTgtDev);
	SIMPLE_FUNC_IMPL(byte, LinkType, m_btLinkType);
	SIMPLE_FUNC_IMPL(int, LogicID, m_nLogicID);
	SIMPLE_FUNC_IMPL(byte, LogicType, m_btLogicType);

	int m_nSrcFacp; //< ID
	int m_nSrcUnit;
	int m_nSrcChn;
	int m_nSrcDev;

	byte m_btLinkType; //<1 : 패턴 , 2:중계기

	byte m_btLogicType; //< 로직 타입 , 전체자동, 개별자동, 수동 (LOGIC_MANUAL,LOGIC_ALL_AUTO,LOGIC_ONE_AUTO
	//< 패턴일 때 1: 수동추가됨 ,2: 개별 자동 , 3: 전체 자동(일람표 비고란에 패턴이름으로 생성시)
	int m_nLogicID; //< 로직 아이디 , 수동일때는 0

	int m_nTgtFacp; //< 패턴 아이디 또는 수신기 아이디 
	int m_nTgtUnit;
	int m_nTgtChn;
	int m_nTgtDev;


	void SetLinkData(int nTgtFacp, int nTgtUnit, int nTgtChn, int nTgtDev
		, byte btLinkType, byte btLogicType, int nLogicID
		, int nSrcFacp = -1, int nSrcUnit = -1, int nSrcChn = -1, int nSrcDev = -1
	);
	void SetLinkData(CDataEmBc * pEm
		,  byte btLogicType, int nLogicID
		, CDataDevice * pSrc);
	void SetLinkData(CDataPattern * pPtn
		, byte btLogicType, int nLogicID
		, CDataDevice * pSrc);
	BOOL IsEqual(CDataLinked* pLink);
	int CopyData(CDataLinked * pSrc);
};