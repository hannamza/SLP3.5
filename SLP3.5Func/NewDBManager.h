#pragma once

#include	"YAdoDatabase.h"

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
};

