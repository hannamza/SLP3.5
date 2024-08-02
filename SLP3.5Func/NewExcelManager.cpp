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

	// 건설사/발주사, 현장명, 건물종류, 현장주소, 대리점 문자열을 조합해서 프로젝트명 생성 후 저장

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

	strcpy_s(CNewInfo::Instance()->m_gi.projectInfo.projectName, CCommonFunc::WCharToChar(strProjectName.GetBuffer(0)));

	strTemp = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_VERSION, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);
	ASSERT(!strTemp.IsEmpty());

	AfxExtractSubString(strModuleTableVer, strTemp, 0, '.');
	AfxExtractSubString(strRomVer, strTemp, 1, '.');

	strAuthorized = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_AUTHORIZED, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);

	CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum = _wtoi(strModuleTableVer);
	CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum = _wtoi(strRomVer);

	if (strAuthorized.Compare(_T("YES")) == 0)
	{
		CNewInfo::Instance()->m_gi.projectInfo.authorized = true;
	}
	else
	{
		CNewInfo::Instance()->m_gi.projectInfo.authorized = false;
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

		//수신기 Type은 0일 수 없으므로 0이면(더이상 데이터가 없으면) 루프 종료하도록 함
		if (nFacpType == 0)
		{
			Log::Trace("There is no more FACP type information. (Total FACP Type Info Count : %d)", i - EXCEL_ENUM_FACP_TYPE::ROW_LIST_START);
			return TRUE;
		}

		CNewInfo::Instance()->m_gi.facpType[nFacpNum] = nFacpType;
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

		//Unit Type은 0일 수 없으므로 0이면(더이상 데이터가 없으면) 루프 종료하도록 함
		if (nUnitType == 0)
		{
			Log::Trace("There is no more unit type information. (Total Unit Type Info Count : %d)", i - EXCEL_ENUM_UNIT_TYPE::ROW_LIST_START);
			return TRUE;
		}

		CNewInfo::Instance()->m_gi.unitType[nFacpNum][nUnitNum] = nUnitType;
	}

	return TRUE;
}

BOOL CNewExcelManager::ParsingNewFacpUintType(CExcelWrapper* xls)
{
	int nFacpNum = -1;
	int nFacpType = -1;
	int nUnitNum = -1;
	int nUnitType = -1;
	int nRoopCount = MAX_FACP_COUNT * MAX_UNIT_COUNT;
	CString strTemp;
	
	for (int i = EXCEL_ENUM_NEW_FACP_UNIT_TYPE::ROW_LIST_START; i < nRoopCount + EXCEL_ENUM_NEW_FACP_UNIT_TYPE::ROW_LIST_START; i++)
	{
		strTemp = _T("");

		strTemp = xls->GetItemText(i, EXCEL_ENUM_NEW_FACP_UNIT_TYPE::COLUMN_FACP_NUMBER);
		//수신기 번호가 없으면 더이상 정보가 없는 것으로 판단하고 루프 종료
		if (strTemp.IsEmpty())
		{
			Log::Trace("There is no more new FACP / unit type information. (Total new FACP / Unit Type Info Count : %d)", i - EXCEL_ENUM_NEW_FACP_UNIT_TYPE::ROW_LIST_START);
			break;
		}
		nFacpNum = _wtoi(strTemp);

		strTemp = xls->GetItemText(i, EXCEL_ENUM_NEW_FACP_UNIT_TYPE::COLUMN_FACP_TYPE);
		nFacpType = _wtoi(strTemp);

		strTemp = xls->GetItemText(i, EXCEL_ENUM_NEW_FACP_UNIT_TYPE::COLUMN_UNIT_NUMBER);
		nUnitNum = _wtoi(strTemp);

		strTemp = xls->GetItemText(i, EXCEL_ENUM_NEW_FACP_UNIT_TYPE::COLUMN_UNIT_TYPE);
		nUnitType = _wtoi(strTemp);

		CNewInfo::Instance()->m_gi.facpType[nFacpNum] = nFacpType;
		CNewInfo::Instance()->m_gi.unitType[nFacpNum][nUnitNum] = nUnitType;
	}

	return TRUE;
}

BOOL CNewExcelManager::ParsingEquipmentInfo(CExcelWrapper* xls)
{
	int nNumber = -1;
	CString strTemp = _T("");
	for (int i = EXCEL_ENUM_EQUIPMENT_INFO::ROW_LIST_START; i < MAX_EQUIP_INFO_ITEM_COUNT + EXCEL_ENUM_EQUIPMENT_INFO::ROW_LIST_START; i++)
	{
		strTemp = xls->GetItemText(i, EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_NUMBER);
		nNumber = _wtoi(strTemp);
		ASSERT(nNumber > 0);
		nNumber -= 1;	//번호는 1베이스, 인덱스는 0베이스
		strTemp = xls->GetItemText(i, EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_INPUT_TYPE);
		strcpy_s(CNewInfo::Instance()->m_ei.inputType[nNumber], CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
		strTemp = xls->GetItemText(i, EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_EQUIPMENT_NAME);
		strcpy_s(CNewInfo::Instance()->m_ei.equipmentName[nNumber], CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
		strTemp = xls->GetItemText(i, EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_OUTPUT_TYPE);
		strcpy_s(CNewInfo::Instance()->m_ei.outputType[nNumber], CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
		strTemp = xls->GetItemText(i, EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_OUTPUT_CIRCUIT);
		strcpy_s(CNewInfo::Instance()->m_ei.outputCircuit[nNumber], CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
	}

	return TRUE;
}

BOOL CNewExcelManager::UpdateProjectInfo(CString strWin32AppProjectName)
{
	//중계기 일람표 경로 문자열 조합, 중계기 일람표 파일명은 중계기 일람표 상의 프로젝트명으로 찾고, 폴더는 SLP3에서 설정된 프로젝트명으로 찾음, 추후에는 일치시키는 방향으로 진행할 예정
	CString strProjectName;
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_gi.projectInfo.projectName));

	CString strModuleTablePath;
	strModuleTablePath.Format(_T("C:\\Ficon3\\Project\\%s\\%s\\%s"), strWin32AppProjectName, F3_VERSIONTEMPFOLDER_NAME, F3_PRJ_DIR_RELAYTABLE);

	//중계기 일람표 리스트 얻음
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
				//리스트 개수만큼 루프돌면서 프로젝트 번호 업데이트

				int nModuleTableVerNum = -1;
				int nLinkedDataVerNum = -1;
				bool bAuthorized = false;
				CString strProjectVerNum = _T("");
				CString strAuthorized = _T("");

				nModuleTableVerNum = CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum;
				nLinkedDataVerNum = CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum;
				bAuthorized = CNewInfo::Instance()->m_gi.projectInfo.authorized;

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

			//모든 Sheet를 검색했는데 project Sheet를 못찾았다면 프로젝트 버전 번호 갱신 실패
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

BOOL CNewExcelManager::UpdateEquipmentInfo(CString strWin32AppProjectName)
{
	//중계기 일람표 경로 문자열 조합, 중계기 일람표 파일명은 중계기 일람표 상의 프로젝트명으로 찾고, 폴더는 SLP3에서 설정된 프로젝트명으로 찾음, 추후에는 일치시키는 방향으로 진행할 예정
	CString strProjectName;
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_gi.projectInfo.projectName));

	CString strModuleTablePath;
	strModuleTablePath.Format(_T("C:\\Ficon3\\Project\\%s\\%s\\%s"), strWin32AppProjectName, F3_VERSIONTEMPFOLDER_NAME, F3_PRJ_DIR_RELAYTABLE);

	//중계기 일람표 리스트 얻음
	std::vector<CString> strModuleTableFileList;
	strModuleTableFileList = CCommonFunc::GetFullPathFileListIntheFolder(strModuleTablePath, strProjectName);

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

			if (strSheetName.CompareNoCase(EXCEL_SHEET_EQUIPMENT_INFO) == 0)
			{
				for (int nCol = EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_INPUT_TYPE; nCol <= EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_OUTPUT_CIRCUIT; nCol++)
				{
					for (int nRow = EXCEL_ENUM_EQUIPMENT_INFO::ROW_LIST_START; nRow < MAX_EQUIP_INFO_ITEM_COUNT + EXCEL_ENUM_EQUIPMENT_INFO::ROW_LIST_START; nRow++)
					{
						CString strTemp = _T("");
						int nIndex = nRow - EXCEL_ENUM_EQUIPMENT_INFO::ROW_LIST_START;

						switch (nCol)
						{
						case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_INPUT_TYPE:
						{
							strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.inputType[nIndex]));
							break;
						}
						case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_EQUIPMENT_NAME:
						{
							strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.equipmentName[nIndex]));
							break;
						}
						case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_OUTPUT_TYPE:
						{
							strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.outputType[nIndex]));
							break;
						}
						case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_OUTPUT_CIRCUIT:
						{
							strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.outputCircuit[nIndex]));
							break;
						}
						default:
							break;
						}

						xls.SetItemText(nRow, nCol, strTemp);
					}
				}
			}
		}

		xls.SavaAs(strModuleTableFile);
		xls.Close();
	}

	return TRUE;
}

BOOL CNewExcelManager::UpdateOneEquipmentInfo(int nType, int nIndex, CString strEquipment, CString strWin32AppProjectName)
{
	//중계기 일람표 경로 문자열 조합, 중계기 일람표 파일명은 중계기 일람표 상의 프로젝트명으로 찾고, 폴더는 SLP3에서 설정된 프로젝트명으로 찾음, 추후에는 일치시키는 방향으로 진행할 예정
	CString strProjectName;
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_gi.projectInfo.projectName));

	CString strModuleTablePath;
	strModuleTablePath.Format(_T("C:\\Ficon3\\Project\\%s\\%s\\%s"), strWin32AppProjectName, F3_VERSIONTEMPFOLDER_NAME, F3_PRJ_DIR_RELAYTABLE);

	//중계기 일람표 리스트 얻음
	std::vector<CString> strModuleTableFileList;
	strModuleTableFileList = CCommonFunc::GetFullPathFileListIntheFolder(strModuleTablePath, strProjectName);

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

			if (strSheetName.CompareNoCase(EXCEL_SHEET_EQUIPMENT_INFO) == 0)
			{
				int nCol = nType + EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_NUMBER;
				int nRow = nIndex + EXCEL_ENUM_EQUIPMENT_INFO::ROW_HEADER;
				xls.SetItemText(nRow, nCol, strEquipment);

				break;
			}
		}

		xls.SavaAs(strModuleTableFile);
		xls.Close();
	}

	return TRUE;
}

BOOL CNewExcelManager::DeletePatternFromModuleTableFile(CString strPatternName, std::vector<CString>& strModuleTableFileList)
{
	std::vector<CString>::iterator iter;
	iter = strModuleTableFileList.begin();
	for (; iter != strModuleTableFileList.end(); iter++)
	{
		CString strModuleTableFile = _T("");
		strModuleTableFile = *iter;
		CExcelWrapper xls;
		if (xls.Open(strModuleTableFile) == false)
		{
			Log::Trace("Failed to open file [%s].", CCommonFunc::WCharToChar(strModuleTableFile.GetBuffer(0)));
			return FALSE;
		}

		BOOL bUnitSheetFound = FALSE;
		int nSheetCount = xls.GetSheetCount();

		for (int j = 0; j < nSheetCount; j++)
		{
			if (xls.SetWorkSheetChange(j + 1) == FALSE)
				continue;

			CString strSheetName = _T("");
			strSheetName = xls.GetSheetName(j + 1);

			if (strSheetName.Find(_T("Unit")) != -1)
			{
				bUnitSheetFound = TRUE;

				for (int nLoop = 0; nLoop < MAX_LOOP_COUNT; nLoop++)
				{
					int nLoopInterval = nLoop * EXCEL_ENUM_CIRCUIT_INFO::COLUMN_OUTPUT_CONTENT;
					for (int nRow = EXCEL_ENUM_CIRCUIT_INFO::ROW_LIST_START; nRow < EXCEL_ENUM_CIRCUIT_INFO::ROW_LIST_START + MAX_CIRCUIT_COUNT; nRow++)
					{
						CString strPattern = _T("");
						strPattern = xls.GetItemText(nRow, EXCEL_ENUM_CIRCUIT_INFO::COLUMN_PATTERN_NAME + nLoopInterval);

						if (strPatternName.Compare(strPattern) == 0)
						{
							xls.SetItemText(nRow, EXCEL_ENUM_CIRCUIT_INFO::COLUMN_PATTERN_NAME + nLoopInterval, _T(""));
						}
					}
				}
			}
		}

		if (!bUnitSheetFound)
		{
			xls.Close();
			Log::Trace("There is no [UnitXX] excel sheet.");
			return FALSE;
		}

		xls.SavaAs(strModuleTableFile);
		xls.Close();
	}

	return TRUE;
}

BOOL CNewExcelManager::UpdateLinkedCircuitInThePattern(int nFacp, int nUnit, int nLoop, int nCircuit, CString strPatternName, std::vector<CString>& strModuleTableFileList)
{
	CString strModuleTableFile = _T("");
	std::vector<CString>::iterator iter;
	iter = strModuleTableFileList.begin();
	for (; iter != strModuleTableFileList.end(); iter++)
	{
		int nFacpNum = -1;
		CString strFacpNum = _T("");
		CString strFile = _T("");
		strFile = *iter;

		strFacpNum = strFile.Mid(strFile.GetLength() - 7, 2);	// XX.xlsx
		nFacpNum = _wtoi(strFacpNum);
		ASSERT(nFacpNum > -1);

		if (nFacp == nFacpNum)
		{
			strModuleTableFile = strFile;
			break;
		}
	}

	if (strModuleTableFile.IsEmpty())
	{
		Log::Trace("There is no Module Table file [FACP number : %d] in the [%s] folder.", nFacp, CCommonFunc::WCharToChar(strModuleTableFile.GetBuffer(0)));
		return FALSE;
	}

	CExcelWrapper xls;
	if (xls.Open(strModuleTableFile) == false)
	{
		Log::Trace("Failed to open file [%s].", CCommonFunc::WCharToChar(strModuleTableFile.GetBuffer(0)));
		return FALSE;
	}

	BOOL bUnitSheetFound = FALSE;
	CString strUnitSheet = _T("");
	strUnitSheet.Format(_T("%02dUnit"), nUnit);
	int nSheetCount = xls.GetSheetCount();

	for (int j = 0; j < nSheetCount; j++)
	{
		if (xls.SetWorkSheetChange(j + 1) == FALSE)
			continue;

		CString strSheetName = _T("");
		strSheetName = xls.GetSheetName(j + 1);

		if (strSheetName.CompareNoCase(strUnitSheet) == 0)
		{
			bUnitSheetFound = TRUE;

			//회로 정보가 있는 지 확인 (입력타입은 회로가 있다면 반드시 존재하므로 입력타입 문자열이 비어있는 지로 판단)
			int nColumn = nLoop * EXCEL_ENUM_CIRCUIT_INFO::COLUMN_OUTPUT_CONTENT + EXCEL_ENUM_CIRCUIT_INFO::COLUMN_INPUT_TYPE;
			int nRow = EXCEL_ENUM_CIRCUIT_INFO::ROW_LIST_START + nCircuit - 1;	//회로번호 1베이스

			CString strInputType = _T("");
			strInputType = xls.GetItemText(nRow, nColumn);
			if (strInputType.IsEmpty())
			{
				xls.Close();
				CString strMsg = _T("");
				strMsg.Format(_T("There is no circuit [%02d%02d%d%03d]."), nFacp, nUnit, nLoop, nCircuit);
				Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
				return FALSE;
			}

			//회로 정보가 있다면 수정
			nColumn = nLoop * EXCEL_ENUM_CIRCUIT_INFO::COLUMN_OUTPUT_CONTENT + EXCEL_ENUM_CIRCUIT_INFO::COLUMN_PATTERN_NAME;
			xls.SetItemText(nRow, nColumn, strPatternName);

			break;
		}
	}

	if (!bUnitSheetFound)
	{
		xls.Close();
		CString strMsg = _T("");
		strMsg.Format(_T("There is no excel sheet [Unit%02d]."), nUnit);
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
		return FALSE;
	}

	xls.SavaAs(strModuleTableFile);
	xls.Close();

	return TRUE;
}

BOOL CNewExcelManager::UpdatePatternInfo(int nFacp, int nUnit, int nLoop, int nCircuit, int nEditType, CString strPatternName, CString strWin32AppProjectName)
{
	BOOL bSucceeded = FALSE;

	//중계기 일람표 경로 문자열 조합, 중계기 일람표 파일명은 중계기 일람표 상의 프로젝트명으로 찾고, 폴더는 SLP3에서 설정된 프로젝트명으로 찾음, 추후에는 일치시키는 방향으로 진행할 예정
	CString strProjectName;
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_gi.projectInfo.projectName));

	CString strModuleTablePath;
	strModuleTablePath.Format(_T("C:\\Ficon3\\Project\\%s\\%s\\%s"), strWin32AppProjectName, F3_VERSIONTEMPFOLDER_NAME, F3_PRJ_DIR_RELAYTABLE);

	//중계기 일람표 리스트 얻음
	std::vector<CString> strModuleTableFileList;
	strModuleTableFileList = CCommonFunc::GetFullPathFileListIntheFolder(strModuleTablePath, strProjectName);

	int nModuleTableCount = -1;
	nModuleTableCount = strModuleTableFileList.size();

	if (nModuleTableCount == 0)
	{
		Log::Trace("There is no Module Table file in the [%s] folder.", CCommonFunc::WCharToChar(strModuleTablePath.GetBuffer(0)));
		return FALSE;
	}

	// 편집 타입 분기
	switch (nEditType)
	{
	case PATTERN_DELETE:
	{
		bSucceeded = DeletePatternFromModuleTableFile(strPatternName, strModuleTableFileList);
		break;
	}
	case PATTERN_ITEM_ADD:
	case PATTERN_ITEM_DELETE:
	{
		bSucceeded = UpdateLinkedCircuitInThePattern(nFacp, nUnit, nLoop, nCircuit, strPatternName, strModuleTableFileList);
		break;
	}
	default:
		break;
	}

	return bSucceeded;
}

BOOL CNewExcelManager::CopyModuleTable(CStringList * pStrList, CString strWin32AppProjectName)
{
	////중계기 일람표 경로 문자열 조합, 중계기 일람표 파일명은 중계기 일람표 상의 프로젝트명으로 찾고, 폴더는 SLP3에서 설정된 프로젝트명으로 찾음, 추후에는 일치시키는 방향으로 진행할 예정
	CString strProjectName;
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_gi.projectInfo.projectName));

	CString strModuleTablePath;
	strModuleTablePath.Format(_T("C:\\Ficon3\\Project\\%s\\%s\\%s"), strWin32AppProjectName, F3_VERSIONTEMPFOLDER_NAME, F3_PRJ_DIR_RELAYTABLE);

	int nModuleTableCount = -1;
	nModuleTableCount = pStrList->GetSize();

	if (nModuleTableCount == 0)
	{
		Log::Trace("There is no Module Table file in the [%s] folder.", CCommonFunc::WCharToChar(strModuleTablePath.GetBuffer(0)));
		return FALSE;
	}

	POSITION pos;
	pos = pStrList->GetHeadPosition();

	while (pos)
	{
		CString strSource = _T("");
		CString strTarget = _T("");
		CString strFileName = _T("");
		CString strMsg = _T("");
		int nFilePos = -1;

		strSource = pStrList->GetNext(pos);
		nFilePos = strSource.ReverseFind('\\');
		strFileName = strSource.Mid(nFilePos + 1);
		strTarget = strModuleTablePath + '\\' + strFileName;

		if (CopyFile(strSource, strTarget, FALSE))
		{
			strMsg.Format(_T("Sucessfully copied the module table file to [%s]"), strTarget);
			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
		}
		else
		{
			strMsg.Format(_T("Failed to copying the module table file to [%s]"), strTarget);
			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNewExcelManager::UpdateOneEBInfo(int nNum, CString strRemarks, CString strCommContent, CString strWin32AppProjectName)
{
	//중계기 일람표 경로 문자열 조합, 중계기 일람표 파일명은 중계기 일람표 상의 프로젝트명으로 찾고, 폴더는 SLP3에서 설정된 프로젝트명으로 찾음, 추후에는 일치시키는 방향으로 진행할 예정
	CString strProjectName;
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_gi.projectInfo.projectName));

	CString strModuleTablePath;
	strModuleTablePath.Format(_T("C:\\Ficon3\\Project\\%s\\%s\\%s"), strWin32AppProjectName, F3_VERSIONTEMPFOLDER_NAME, F3_PRJ_DIR_RELAYTABLE);

	//중계기 일람표 리스트 얻음
	std::vector<CString> strModuleTableFileList;
	strModuleTableFileList = CCommonFunc::GetFullPathFileListIntheFolder(strModuleTablePath, strProjectName);

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

			if (strSheetName.CompareNoCase(_T("eb")) == 0)
			{
				int nCol = EXCEL_ENUM_EB_INFO::COLUMN_REMARKS;
				int nRow = nNum + EXCEL_ENUM_EB_INFO::ROW_HEADER;
				xls.SetItemText(nRow, nCol, strRemarks);

				nCol = EXCEL_ENUM_EB_INFO::COLUMN_COMM_CONTENT;
				xls.SetItemText(nRow, nCol, strCommContent);

				break;
			}
		}

		xls.SavaAs(strModuleTableFile);
		xls.Close();
	}

	return TRUE;
}