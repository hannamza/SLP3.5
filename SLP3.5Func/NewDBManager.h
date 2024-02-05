#pragma once

#include	"YAdoDatabase.h"

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
};

