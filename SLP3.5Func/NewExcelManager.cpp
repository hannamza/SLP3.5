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

		//수신기 Type은 0일 수 없으므로 0이면(더이상 데이터가 없으면) 루프 종료하도록 함
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

		//Unit Type은 0일 수 없으므로 0이면(더이상 데이터가 없으면) 루프 종료하도록 함
		if (nUnitType == 0)
		{
			Log::Trace("There is no more unit type information. (Total Unit Type Info Count : %d)", i - EXCEL_ENUM_UNIT_TYPE::ROW_LIST_START);
			return TRUE;
		}

		CNewInfo::Instance()->m_fi.unitType[nFacpNum][nUnitNum] = nUnitType;
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
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_fi.projectInfo.projectName));

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
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_fi.projectInfo.projectName));

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

BOOL CNewExcelManager::AddEquipment(int nType, int nIndex, CString strEquipment)
{
	BOOL bRet = FALSE;
	bRet = IsExistingEquipment(nType, strEquipment);
	if (!bRet)
	{
		switch (nType)
		{
		case ET_INPUTTYPE:
		{
			strcpy_s(CNewInfo::Instance()->m_ei.inputType[nIndex], CCommonFunc::WCharToChar(strEquipment.GetBuffer(0)));
			break;
		}
		case ET_EQNAME:
		{
			strcpy_s(CNewInfo::Instance()->m_ei.equipmentName[nIndex], CCommonFunc::WCharToChar(strEquipment.GetBuffer(0)));
			break;
		}
		case ET_OUTPUTTYPE:
		{
			strcpy_s(CNewInfo::Instance()->m_ei.outputType[nIndex], CCommonFunc::WCharToChar(strEquipment.GetBuffer(0)));
			break;
		}
		case ET_OUTCONTENTS:
		{
			strcpy_s(CNewInfo::Instance()->m_ei.outputCircuit[nIndex], CCommonFunc::WCharToChar(strEquipment.GetBuffer(0)));
			break;
		}
		default:
			break;
		}
	}

	return bRet;
}

BOOL CNewExcelManager::IsExistingEquipment(int nType, CString strEquipment)
{
	CString strTemp;

	for (int i = 0; i < MAX_EQUIP_INFO_ITEM_COUNT; i++)
	{
		strTemp = _T("");
		switch (nType)
		{
		case ET_INPUTTYPE:
		{
			strTemp.Format(_T("%s"), CNewInfo::Instance()->m_ei.inputType[i]);
			break;
		}
		case ET_EQNAME:
		{
			strTemp.Format(_T("%s"), CNewInfo::Instance()->m_ei.equipmentName[i]);
			break;
		}
		case ET_OUTPUTTYPE:
		{
			strTemp.Format(_T("%s"), CNewInfo::Instance()->m_ei.outputType[i]);
			break;
		}
		case ET_OUTCONTENTS:
		{
			strTemp.Format(_T("%s"), CNewInfo::Instance()->m_ei.outputCircuit[i]);
			break;
		}
		default:
			break;
		}
		
		if (strTemp.Compare(strEquipment) == 0)
			return TRUE;
	}

	return FALSE;
}

BOOL CNewExcelManager::CopyModuleTable(CStringList * pStrList, CString strWin32AppProjectName)
{
	////중계기 일람표 경로 문자열 조합, 중계기 일람표 파일명은 중계기 일람표 상의 프로젝트명으로 찾고, 폴더는 SLP3에서 설정된 프로젝트명으로 찾음, 추후에는 일치시키는 방향으로 진행할 예정
	CString strProjectName;
	strProjectName.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_fi.projectInfo.projectName));

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