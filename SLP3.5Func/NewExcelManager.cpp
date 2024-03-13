#include "stdafx.h"
#include "NewExcelManager.h"


CNewExcelManager::CNewExcelManager()
{
}


CNewExcelManager::~CNewExcelManager()
{
}

BOOL CNewExcelManager::ParsingProjectInfo(CExcelWrapper* xls)
{
	CString strTemp = _T("");
	CString strProjectName = _T("");
	CString strModuleTableVer = _T("");
	CString strRomVer = _T("");
	CString strAuthorized = _T("");

	// �Ǽ���/���ֻ�, �����, �ǹ�����, �����ּ�, �븮�� ���ڿ��� �����ؼ� ������Ʈ�� ���� �� ����

	strTemp = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_CONSTRUCTION_COMPANY, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);
	ASSERT(!strTemp.IsEmpty());
	strProjectName = strTemp + _T("_");

	strTemp = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_SITE_NAME, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);
	ASSERT(!strTemp.IsEmpty());
	strProjectName += strTemp;

	strTemp = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_BUILDING_TYPE, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);
	strProjectName += _T(" ") + strTemp;

	strTemp = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_SITE_ADDRESS, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);
	ASSERT(!strTemp.IsEmpty());
	strProjectName += _T("(") + strTemp + _T(")");

	strTemp = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_RETAIL_STORE, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);
	ASSERT(!strTemp.IsEmpty());
	strProjectName += _T("_") + strTemp;

	strcpy_s(CNewInfo::Instance()->m_fi.projectInfo.projectName, CCommonFunc::WCharToChar(strProjectName.GetBuffer(0)));

	strTemp = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_VERSION, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);
	ASSERT(!strTemp.IsEmpty());

	AfxExtractSubString(strModuleTableVer, strTemp, 0, '.');
	AfxExtractSubString(strRomVer, strTemp, 1, '.');

	strAuthorized = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_AUTHORIZED, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);

	CNewInfo::Instance()->m_fi.projectInfo.moduleTableVerNum = _wtoi(strModuleTableVer);
	CNewInfo::Instance()->m_fi.projectInfo.linkedDataVerNum = _wtoi(strRomVer);

	if (strAuthorized.Compare(_T("YES")) == 0)
	{
		CNewInfo::Instance()->m_fi.projectInfo.authorized = true;
	}
	else
	{
		CNewInfo::Instance()->m_fi.projectInfo.authorized = false;
	}

	return TRUE;
}

BOOL CNewExcelManager::ParsingFacpType(CExcelWrapper* xls)
{
	int nFacpNum = 0;
	int nFacpType = 0;

	CString strTemp = _T("");
	int nRoopCount = MAX_FACP_COUNT;
	for (int i = EXCEL_ENUM_FACP_TYPE::ROW_LIST_START; i < nRoopCount + EXCEL_ENUM_FACP_TYPE::ROW_LIST_START; i++)
	{
		strTemp = xls->GetItemText(i, EXCEL_ENUM_FACP_TYPE::COLUMN_FACP_NUM);
		nFacpNum = _wtoi(strTemp);
		strTemp = xls->GetItemText(i, EXCEL_ENUM_FACP_TYPE::COLUMN_FACP_TYPE);
		nFacpType = _wtoi(strTemp);

		//���ű� Type�� 0�� �� �����Ƿ� 0�̸�(���̻� �����Ͱ� ������) ���� �����ϵ��� ��
		if (nFacpType == 0)
		{
			Log::Trace("There is no more FACP type information. (Total FACP Type Info Count : %d)", i - EXCEL_ENUM_FACP_TYPE::ROW_LIST_START);
			return TRUE;
		}

		CNewInfo::Instance()->m_fi.facpType[nFacpNum] = nFacpType;
	}

	return TRUE;
}

BOOL CNewExcelManager::ParsingUnitType(CExcelWrapper* xls)
{
	int nFacpNum = 0;
	int nUnitNum = 0;
	int nUnitType = 0;

	CString strTemp = _T("");
	int nRoopCount = MAX_FACP_COUNT * MAX_UNIT_COUNT;
	for (int i = EXCEL_ENUM_UNIT_TYPE::ROW_LIST_START; i < nRoopCount + EXCEL_ENUM_UNIT_TYPE::ROW_LIST_START; i++)
	{
		strTemp = xls->GetItemText(i, EXCEL_ENUM_UNIT_TYPE::COLUMN_FACP_NUM);
		nFacpNum = _wtoi(strTemp);
		strTemp = xls->GetItemText(i, EXCEL_ENUM_UNIT_TYPE::COLUMN_UNIT_NUM);
		nUnitNum = _wtoi(strTemp);
		strTemp = xls->GetItemText(i, EXCEL_ENUM_UNIT_TYPE::COLUMN_UNIT_TYPE);
		nUnitType = _wtoi(strTemp);

		//Unit Type�� 0�� �� �����Ƿ� 0�̸�(���̻� �����Ͱ� ������) ���� �����ϵ��� ��
		if (nUnitType == 0)
		{
			Log::Trace("There is no more unit type information. (Total Unit Type Info Count : %d)", i - EXCEL_ENUM_UNIT_TYPE::ROW_LIST_START);
			return TRUE;
		}

		CNewInfo::Instance()->m_fi.unitType[nFacpNum][nUnitNum] = nUnitType;
	}

	return TRUE;
}

BOOL CNewExcelManager::UpdateProjectInfo(CString strWin32AppProjectName)
{
	//�߰�� �϶�ǥ ��� ���ڿ� ����, �߰�� �϶�ǥ ���ϸ��� �߰�� �϶�ǥ ���� ������Ʈ������ ã��, ������ SLP3���� ������ ������Ʈ������ ã��, ���Ŀ��� ��ġ��Ű�� �������� ������ ����
	CString strProjectName;
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_fi.projectInfo.projectName));

	CString strModuleTablePath;
	strModuleTablePath.Format(_T("C:\\Ficon3\\Project\\%s\\%s\\%s"), strWin32AppProjectName, F3_VERSIONTEMPFOLDER_NAME, F3_PRJ_DIR_RELAYTABLE);

	//�߰�� �϶�ǥ ����Ʈ ����
	std::vector<CString> strModuleTableFileList;
	strModuleTableFileList =  CCommonFunc::GetFullPathFileListIntheFolder(strModuleTablePath, strProjectName);

	int nModuleTableCount = -1;
	nModuleTableCount = strModuleTableFileList.size();

	if (nModuleTableCount == 0)
	{
		Log::Trace("There is no Module Table file in the [%s] folder.", CCommonFunc::WCharToChar(strModuleTablePath.GetBuffer(0)));
		return FALSE;
	}

	for (int i = 0; i < nModuleTableCount; i++)
	{
		CString strModuleTableFile;
		strModuleTableFile = strModuleTableFileList[i];

		CExcelWrapper xls;
		if (xls.Open(strModuleTableFile) == false)
		{
			Log::Trace("Failed to open file [%s].", CCommonFunc::WCharToChar(strModuleTableFile.GetBuffer(0)));
			return FALSE;
		}

		int nSheetCount = 0;
		nSheetCount = xls.GetSheetCount();

		for (int j = 0; j < nSheetCount; j++)
		{
			if (xls.SetWorkSheetChange(j + 1) == FALSE)
				continue;

			CString strSheetName = _T("");
			strSheetName = xls.GetSheetName(j + 1);

			if (strSheetName.CompareNoCase(EXCEL_SHEET_PROJECT_INFO) == 0)
			{
				//
				//����Ʈ ������ŭ �������鼭 ������Ʈ ��ȣ ������Ʈ

				int nModuleTableVerNum = -1;
				int nLinkedDataVerNum = -1;
				bool bAuthorized = false;
				CString strProjectVerNum = _T("");
				CString strAuthorized = _T("");

				nModuleTableVerNum = CNewInfo::Instance()->m_fi.projectInfo.moduleTableVerNum;
				nLinkedDataVerNum = CNewInfo::Instance()->m_fi.projectInfo.linkedDataVerNum;
				bAuthorized = CNewInfo::Instance()->m_fi.projectInfo.authorized;

				strProjectVerNum.Format(_T("%02d.%02d"), nModuleTableVerNum, nLinkedDataVerNum);
				if (bAuthorized)
				{
					strAuthorized = _T("YES");
				}
				else
				{
					strAuthorized = _T("NO");
				}

				xls.SetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_VERSION, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT, strProjectVerNum);
				xls.SetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_AUTHORIZED, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT, strAuthorized);
				//

				break;
			}

			//��� Sheet�� �˻��ߴµ� project Sheet�� ��ã�Ҵٸ� ������Ʈ ���� ��ȣ ���� ����
			if (j == nSheetCount - 1)
			{
				CString strLog = _T("Failed to update the project number in file [%s] because it failed to find the [%s] sheet.", strModuleTableFile, EXCEL_SHEET_PROJECT_INFO);
				Log::Trace("%s", CCommonFunc::WCharToChar(strLog.GetBuffer(0)));
			}
		}
		xls.SavaAs(strModuleTableFile);
		xls.Close();

	}
	return TRUE;
}