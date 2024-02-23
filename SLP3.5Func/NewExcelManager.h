#pragma once

#include "ExcelWrapper.h"
class CNewExcelManager : public TSingleton<CNewExcelManager>
{
public:
	CNewExcelManager();
	~CNewExcelManager();

	//�ż���
public:
	// ������Ʈ ���� �Ľ�
	BOOL ParsingProjectInfo(CExcelWrapper* xls);

	// ���ű� Type �Ľ�
	BOOL ParsingFacpType(CExcelWrapper* xls);

	// Unit Type �Ľ�
	BOOL ParsingUnitType(CExcelWrapper* xls);

	// CCTV ���� �Ľ�
	BOOL ParsingCCTVInfo(CExcelWrapper* xls);
};

