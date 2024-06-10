#pragma once

#include "ExcelWrapper.h"
class CNewExcelManager : public TSingleton<CNewExcelManager>
{
public:
	CNewExcelManager();
	~CNewExcelManager();

	//변수
public:

	// GT1 추가 Sheet 존재 여부
	BOOL bExistPI;
	BOOL bExistFT;
	BOOL bExistUT;

	// 설비 정의 Sheet 존재 여부
	BOOL bExistEI;

	//매서드
public:
	// 프로젝트 정보 파싱
	BOOL ParsingProjectInfo(CExcelWrapper* xls);

	// 수신기 Type 파싱
	BOOL ParsingFacpType(CExcelWrapper* xls);

	// Unit Type 파싱
	BOOL ParsingUnitType(CExcelWrapper* xls);

	// 설비 정의 파싱
	BOOL ParsingEquipmentInfo(CExcelWrapper* xls);

	// 프로젝트 정보 갱신
	BOOL UpdateProjectInfo(CString strWin32AppProjectName);

	// 설비 정보 갱신 (일괄 갱신)
	BOOL UpdateEquipmentInfo(CString strWin32AppProjectName);

	// 설비 정의 추가 시 GT1 프로젝트라면 중계기 일람표 설비 정의에도 적용
	BOOL UpdateOneEquipmentInfo(int nType, int nIndex, CString strEquipment, CString strWin32AppProjectName);

	// 패턴 편집 시 중계기 일람표 적용
	BOOL UpdatePatternInfo(int nFacp, int nUnit, int nLoop, int nCircuit, int nEditType, CString strPatternName, CString strWin32AppProjectName);

	// 중계기 일람표 상의 해당 패턴 삭제
	BOOL DeletePatternFromModuleTableFile(CString strPatternName, std::vector<CString>& strModuleTableFileList);

	// 중계기 일람표 상의 패턴 아이템(출력회로) 추가 -> 타입 나눠서 삭제도 가능하도록 매서드 수정하면 될 듯
	BOOL UpdateLinkedCircuitInThePattern(int nFacp, int nUnit, int nLoop, int nCircuit, CString strPatternName, std::vector<CString>& strModuleTableFileList);

	// 중계기 일람표 갱신 시 새 중계기 일람표 프로젝트 폴더에 복사
	BOOL CopyModuleTable(CStringList * pStrList, CString strWin32AppProjectName);

	// 비상방송 편집 시 중계기 일람표에 적용 (F3, GT1이든 상관없음)
	BOOL UpdateOneEBInfo(int nNum, CString strRemarks, CString strCommContent, CString strWin32AppProjectName);
};

