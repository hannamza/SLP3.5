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

	// F4 �߰� Table ���� ���� Ȯ�� �� ������ Create
	BOOL CheckAndCreateF4DBTables();

	BOOL CheckDBTableExist(CString strTableName);

	CString GetQueryStringCreateFACPTypeTable();

	CString GetQueryStringCreateUnitTypeTable();

	CString GetQueryStringCreateProjectInfoTable();

	// F4 �߰� Table�� ������ insert
	BOOL InsertDatasIntoF4DBTables();

	BOOL InsertDataIntoFACPTypeTable();

	BOOL InsertDataIntoUnitTypeTable();

	BOOL InsertDataIntoProjectInfoTable();

	// ���� ������Ʈ ���� �� F4 �߰� Table���� ������ �ε�
	BOOL GetDataFromF4DBTables();

	BOOL GetDataFromFACPTypeTable();

	BOOL GetDataFromUnitTypeTable();

	BOOL GetDataFromProjectInfoTable();
};

