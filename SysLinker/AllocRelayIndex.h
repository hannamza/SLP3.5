#pragma once

enum RIDX_ALLOC_TYPE
{
	RIDX_ALLOC_NONE = 0,
	RIDX_ALLOC_ALL = 1,
	RIDX_ALLOC_PART = 2,
};

class CRelayTableData;
class YAdoDatabase;
class CAllocRelayIndex
{
public:
	/*
	nAllocType : 
		- �Ҵ����� ���� : 0
		- RelayIndex�� ��ü ���� �Ҵ� : 1
		- ���ȣ�� ���� �Ҵ� : 2
	*/
	CAllocRelayIndex(CRelayTableData * pTable, int nAllocType);
	~CAllocRelayIndex();

	int m_nAllocType; 
	CRelayTableData * m_pTable;
	int m_nLastIndex;
	int ProcessAlloc(YAdoDatabase * pDB);

};

