#pragma once

#include "YAdoDatabase.h"
#include "ExcelWrapper.h"

class CNewDBManager : public TSingleton<CNewDBManager>
{
public:
	CNewDBManager();
	~CNewDBManager();

public:
	// ����

	// DB Accessor
	YAdoDatabase* m_pDB;

	// �ż���

	// DB Accessor set
	void SetDBAccessor(YAdoDatabase* pDB);

	// �� �Է� Ÿ�� Table�� ���� ���� Ȯ�� �� ������ Insert
	BOOL CheckAndInsertEquipmentNewInputType();

	// ���� ���� ������ insert
	BOOL InsertDataIntoEquipmentInfoTable();

	// GT1 �߰� Table ���� ���� Ȯ�� �� ������ Create
	BOOL CheckAndCreateGT1DBTables();

	BOOL CheckDBTableExist(CString strTableName);

	CString GetQueryStringCreateFACPTypeTable();

	CString GetQueryStringCreateUnitTypeTable();

	CString GetQueryStringCreateProjectInfoTable();

	// GT1 �߰� Table�� ������ insert
	BOOL InsertDatasIntoGT1DBTables();

	BOOL InsertDataIntoFACPTypeTable();

	BOOL InsertDataIntoUnitTypeTable();

	BOOL InsertDataIntoProjectInfoTable();

	// ���� ������Ʈ ���� �� GT1 �߰� Table���� ������ �ε�
	BOOL GetDataFromGT1DBTables();

	BOOL GetDataFromFACPTypeTable();

	BOOL GetDataFromUnitTypeTable();

	BOOL GetDataFromProjectInfoTable();
};

