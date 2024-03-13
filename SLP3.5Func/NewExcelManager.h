#pragma once

#include "ExcelWrapper.h"
class CNewExcelManager : public TSingleton<CNewExcelManager>
{
public:
	CNewExcelManager();
	~CNewExcelManager();

	//����
public:

	// F4 �߰� Sheet ���� ����
	BOOL bExistPI;
	BOOL bExistFT;
	BOOL bExistUT;

	//�ż���
public:
	// ������Ʈ ���� �Ľ�
	BOOL ParsingProjectInfo(CExcelWrapper* xls);

	// ���ű� Type �Ľ�
	BOOL ParsingFacpType(CExcelWrapper* xls);

	// Unit Type �Ľ�
	BOOL ParsingUnitType(CExcelWrapper* xls);

	// ������Ʈ ���� ����
	BOOL UpdateProjectInfo(CString strWin32AppProjectName);
};

