#pragma once

#include "ExcelWrapper.h"
class CNewExcelManager : public TSingleton<CNewExcelManager>
{
public:
	CNewExcelManager();
	~CNewExcelManager();

	//변수
public:

	// F4 추가 Sheet 존재 여부
	BOOL bExistPI;
	BOOL bExistFT;
	BOOL bExistUT;

	//매서드
public:
	// 프로젝트 정보 파싱
	BOOL ParsingProjectInfo(CExcelWrapper* xls);

	// 수신기 Type 파싱
	BOOL ParsingFacpType(CExcelWrapper* xls);

	// Unit Type 파싱
	BOOL ParsingUnitType(CExcelWrapper* xls);

	// 프로젝트 정보 갱신
	BOOL UpdateProjectInfo(CString strWin32AppProjectName);
};

