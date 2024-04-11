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

	// ���� ���� Sheet ���� ����
	BOOL bExistEI;

	//�ż���
public:
	// ������Ʈ ���� �Ľ�
	BOOL ParsingProjectInfo(CExcelWrapper* xls);

	// ���ű� Type �Ľ�
	BOOL ParsingFacpType(CExcelWrapper* xls);

	// Unit Type �Ľ�
	BOOL ParsingUnitType(CExcelWrapper* xls);

	// ���� ���� �Ľ�
	BOOL ParsingEquipmentInfo(CExcelWrapper* xls);

	// ������Ʈ ���� ����
	BOOL UpdateProjectInfo(CString strWin32AppProjectName);

	// ���� ���� ���� (�ϰ� ����)
	BOOL UpdateEquipmentInfo(CString strWin32AppProjectName);

	// ���� ���� �߰� �� F4 ������Ʈ��� �߰�� �϶�ǥ ���� ���ǿ��� ����
	BOOL AddOneEquipment(int nType, int nIndex, CString strEquipment, CString strWin32AppProjectName);

	// �߰�� �϶�ǥ ���� �� �� �߰�� �϶�ǥ ������Ʈ ������ ����
	BOOL CopyModuleTable(CStringList * pStrList, CString strWin32AppProjectName);
};

