#pragma once

#include "ExcelWrapper.h"
class CNewExcelManager : public TSingleton<CNewExcelManager>
{
public:
	CNewExcelManager();
	~CNewExcelManager();

	//매서드
public:
	// 프로젝트 정보 파싱
	BOOL ParsingProjectInfo(CExcelWrapper* xls);

	// 수신기 Type 파싱
	BOOL ParsingFacpType(CExcelWrapper* xls);

	// Unit Type 파싱
	BOOL ParsingUnitType(CExcelWrapper* xls);

	// CCTV 정보 파싱
	BOOL ParsingCCTVInfo(CExcelWrapper* xls);
};

