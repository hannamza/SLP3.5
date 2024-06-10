#pragma once

#include "ExcelWrapper.h"
class CNewExcelManager : public TSingleton<CNewExcelManager>
{
public:
	CNewExcelManager();
	~CNewExcelManager();

	//����
public:

	// GT1 �߰� Sheet ���� ����
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

	// ���� ���� �߰� �� GT1 ������Ʈ��� �߰�� �϶�ǥ ���� ���ǿ��� ����
	BOOL UpdateOneEquipmentInfo(int nType, int nIndex, CString strEquipment, CString strWin32AppProjectName);

	// ���� ���� �� �߰�� �϶�ǥ ����
	BOOL UpdatePatternInfo(int nFacp, int nUnit, int nLoop, int nCircuit, int nEditType, CString strPatternName, CString strWin32AppProjectName);

	// �߰�� �϶�ǥ ���� �ش� ���� ����
	BOOL DeletePatternFromModuleTableFile(CString strPatternName, std::vector<CString>& strModuleTableFileList);

	// �߰�� �϶�ǥ ���� ���� ������(���ȸ��) �߰� -> Ÿ�� ������ ������ �����ϵ��� �ż��� �����ϸ� �� ��
	BOOL UpdateLinkedCircuitInThePattern(int nFacp, int nUnit, int nLoop, int nCircuit, CString strPatternName, std::vector<CString>& strModuleTableFileList);

	// �߰�� �϶�ǥ ���� �� �� �߰�� �϶�ǥ ������Ʈ ������ ����
	BOOL CopyModuleTable(CStringList * pStrList, CString strWin32AppProjectName);

	// ����� ���� �� �߰�� �϶�ǥ�� ���� (F3, GT1�̵� �������)
	BOOL UpdateOneEBInfo(int nNum, CString strRemarks, CString strCommContent, CString strWin32AppProjectName);
};

