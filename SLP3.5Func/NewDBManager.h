#pragma once

#include "YAdoDatabase.h"
#include "ExcelWrapper.h"

class CNewDBManager : public TSingleton<CNewDBManager>
{
public:
	CNewDBManager();
	~CNewDBManager();

public:
	// 변수

	// DB Accessor
	YAdoDatabase* m_pDB;

	// 매서드

	// DB Accessor set
	void SetDBAccessor(YAdoDatabase* pDB);

	// 새 입력 타입 Table에 존재 여부 확인 후 없으면 Insert
	BOOL CheckAndInsertEquipmentNewInputType();

	// 설비 정의 데이터 insert
	BOOL InsertDataIntoEquipmentInfoTable();

	// GT1 추가 Table 존재 여부 확인 후 없으면 Create
	BOOL CheckAndCreateGT1DBTables();

	BOOL CheckDBTableExist(CString strTableName);

	CString GetQueryStringCreateFACPTypeTable();

	CString GetQueryStringCreateUnitTypeTable();

	CString GetQueryStringCreateProjectInfoTable();

	// GT1 추가 Table에 데이터 insert
	BOOL InsertDatasIntoGT1DBTables();

	BOOL InsertDataIntoFACPTypeTable();

	BOOL InsertDataIntoUnitTypeTable();

	BOOL InsertDataIntoProjectInfoTable();

	// 기존 프로젝트 열기 시 GT1 추가 Table에서 데이터 로드
	BOOL GetDataFromGT1DBTables();

	BOOL GetDataFromFACPTypeTable();

	BOOL GetDataFromUnitTypeTable();

	BOOL GetDataFromProjectInfoTable();
};

