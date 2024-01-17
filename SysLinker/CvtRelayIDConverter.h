#pragma once

class CRelayTableData;
class YAdoDatabase;
class CDataDevice;

class CTempLkItem
{
public:
	CDataDevice		* m_pRefDev;
	byte			m_btLinkType;
	byte			m_btLogicType;
	int				m_nLogicID;
};

class CTempDevData
{
public:
	CDataDevice * m_pOldRefDev;
	CPtrList		m_ptrLinkList; ///< 연동데이터의 Old Device Pointer List
};

class CTempLinkData
{
public:
	LPVOID 		m_pOldData;
	int			m_nType;
	CPtrList		m_ptrLinkList; ///< 연동데이터의 Old Device Pointer List
};

class CCvtRelayIDConverter
{
public:
	CCvtRelayIDConverter(CRelayTableData * pTable);
	~CCvtRelayIDConverter();

	CRelayTableData *		m_pOldTable;


	CPtrList		m_ptrTempDevList;
	CPtrList		m_ptrTempPtnList;
	CPtrList		m_ptrTempPumpList;
	CPtrList		m_ptrTempPsList;

	int				m_arrChangedFacpID[D_MAX_FACP_COUNT];
	
	// 1. 기존 데이터 copy - 수신기,유닛,채널,회로
	// 2. 연동데이터 copy
	// 3. Pattern copy
	// 4. 수신기,유닛,채널,회로 아이디 재 생성
	// 5. Old Map 다시 생성
	// 6. Database 초기화
	// 7. Database 입력

	int RemoveTempDevList();
	int RemoveTempPtnList();
	int RemoveTempPumpList();
	int RemoveTempPsList();

	// 1. 기존 데이터 copy - 수신기,유닛,채널,회로
	// 2. 연동데이터 copy
	int MakeTempDevData();
	
	// 3. Pattern copy
	int MakeTempPtnData();
	int MakeTempPumpData();
	int MakeTempPSData();


	// 4. 수신기,유닛,채널,회로 아이디 재 생성
	int ChangeID();

	int ChangeMap();
	
	
	int ChangeMemory();

	int ConvertRelayID();
	// Database Change
	int ChangeDatabase(YAdoDatabase * pDB);
	int ChangeRelayDB(YAdoDatabase * pDB);
	int ChangePatternDB(YAdoDatabase * pDB);
	int ChangePumpDB(YAdoDatabase * pDB);
	int ChangePSDB(YAdoDatabase * pDB);
	int ChangeFacpContactDB(YAdoDatabase * pDB);
};

