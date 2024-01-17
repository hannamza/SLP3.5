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
	CPtrList		m_ptrLinkList; ///< ������������ Old Device Pointer List
};

class CTempLinkData
{
public:
	LPVOID 		m_pOldData;
	int			m_nType;
	CPtrList		m_ptrLinkList; ///< ������������ Old Device Pointer List
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
	
	// 1. ���� ������ copy - ���ű�,����,ä��,ȸ��
	// 2. ���������� copy
	// 3. Pattern copy
	// 4. ���ű�,����,ä��,ȸ�� ���̵� �� ����
	// 5. Old Map �ٽ� ����
	// 6. Database �ʱ�ȭ
	// 7. Database �Է�

	int RemoveTempDevList();
	int RemoveTempPtnList();
	int RemoveTempPumpList();
	int RemoveTempPsList();

	// 1. ���� ������ copy - ���ű�,����,ä��,ȸ��
	// 2. ���������� copy
	int MakeTempDevData();
	
	// 3. Pattern copy
	int MakeTempPtnData();
	int MakeTempPumpData();
	int MakeTempPSData();


	// 4. ���ű�,����,ä��,ȸ�� ���̵� �� ����
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

