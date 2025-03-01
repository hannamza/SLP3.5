#include "stdafx.h"
#include "NewDBManager.h"


CNewDBManager::CNewDBManager()
{
	m_pDB = nullptr;
}


CNewDBManager::~CNewDBManager()
{
}

void CNewDBManager::SetDBAccessor(YAdoDatabase* pDB)
{
	m_pDB = pDB;
}

BOOL CNewDBManager::CheckAndInsertEquipmentNewInputType()
{
	if (m_pDB == nullptr)
		return FALSE;

	for (int i = NEW_EQUIPMENT_INPUT_TYPE::AN열식교차A; i <= NEW_EQUIPMENT_INPUT_TYPE::CCTV; i++)
	{
		CString strEquip = _T("");
		strEquip.Format(_T("%s"), g_lpszEquipmentInputType[i]);

		CString strQuery = _T("");
		strQuery.Format(_T("SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=1 AND EQ_ID=%d"), i);

		if (!m_pDB->OpenQuery(strQuery))
		{
			Log::Trace("TB_EQUIP_MST SELECT QUERY Failed! - Input Type : %s", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
			return FALSE;
		}

		int nRow = -1;
		nRow = m_pDB->GetRecordCount();
		if (nRow == 0)
		{
			strQuery.Format(_T("INSERT INTO TB_EQUIP_MST (EQ_ID, EQ_TYPE, EQ_NAME, EQ_DESC, EQ_SYMBOL, ADD_USER) VALUES (%d, 1, \'%s\', \'%s\', \'basic.bmp\', \'admin\')"), i, strEquip, strEquip);
			if (m_pDB->ExecuteSql(strQuery))
			{
				Log::Trace("TB_EQUIP_MST INSERT QUERY Succeeded! Input Type : %s", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
			}
			else
			{
				Log::Trace("TB_EQUIP_MST INSERT QUERY Failed! - Input Type : %s", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
				return FALSE;
			}
		}
		else if (nRow == 1)	// 만약 존재하면 업데이트하고 로그 남김
		{
			strQuery.Format(_T("UPDATE TB_EQUIP_MST SET EQ_NAME = \'%s\', EQ_DESC = \'%s\' WHERE EQ_TYPE = 1 AND EQ_ID = %d"), strEquip, strEquip, i);
			if (m_pDB->ExecuteSql(strQuery))
			{
				Log::Trace("TB_EQUIP_MST UPDATE QUERY Succeeded! - Input Type : %s", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
			}
			else
			{
				Log::Trace("TB_EQUIP_MST UPDATE QUERY Failed! - Input Type : %s", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
				return FALSE;
			}
		}
		else
		{
			Log::Trace("There is more than one row ! - Input Type : %s Please Check TB_EQUIP_MST table out", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
			return FALSE;
		}
	}

	//입력 설비명 CCTV 추가 - 기본 DB (gfs_base)에는 52번으로 넣지만 기존 DB로드 시 CCTV 이름을 추가할 경우 기존에 없으면 맨 마지막 번으로 넣음
	for (int j = NEW_EQUIPMENT_INPUT_NAME::CCTV; j <= NEW_EQUIPMENT_INPUT_NAME::CCTV; j++)
	{
		CString strName = _T("");
		strName.Format(_T("%s"), g_lpszEquipmentInputName[j]);

		CString strQuery = _T("");
		strQuery.Format(_T("SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=2 AND EQ_NAME=\'%s\' AND EQ_DESC=\'%s\'"), strName, strName);

		if (!m_pDB->OpenQuery(strQuery))
		{
			Log::Trace("TB_EQUIP_MST SELECT QUERY Failed! - Input Name : %s", CCommonFunc::WCharToChar(strName.GetBuffer(0)));
			return FALSE;
		}

		int nRow = -1;
		nRow = m_pDB->GetRecordCount();
		if (nRow == 0)
		{
			//기존에 입력명이 몇 개 있는지 확인
			strQuery.Format(_T("SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=2"));

			if (!m_pDB->OpenQuery(strQuery))
			{
				Log::Trace("TB_EQUIP_MST SELECT QUERY (EQ_TYPE=2 COUNT) Failed! - Input Name : %s", CCommonFunc::WCharToChar(strName.GetBuffer(0)));
				return FALSE;
			}

			nRow = m_pDB->GetRecordCount();

			strQuery.Format(_T("INSERT INTO TB_EQUIP_MST (EQ_ID, EQ_TYPE, EQ_NAME, EQ_DESC, EQ_SYMBOL, ADD_USER) VALUES (%d, 2, \'%s\', \'%s\', \'basic.bmp\', \'admin\')"), nRow + 1, strName, strName);
			if (!m_pDB->ExecuteSql(strQuery))
			{
				Log::Trace("TB_EQUIP_MST INSERT QUERY Failed! - Input Name : %s", CCommonFunc::WCharToChar(strName.GetBuffer(0)));
				return FALSE;
			}
		}
		else if (nRow == 1)	// 만약 존재하면 아무 것도 안함
		{
// 			int nEQID = -1;
// 			m_pDB->GetFieldValue(_T("EQ_ID"), nEQID);
// 			ASSERT(nEQID > 0);
// 
// 			strQuery.Format(_T("UPDATE TB_EQUIP_MST SET EQ_NAME = \'%s\', EQ_DESC = \'%s\' WHERE EQ_TYPE = 2 AND EQ_ID = %d"), strName, strName, nEQID);
// 			if (m_pDB->ExecuteSql(strQuery))
// 			{
// 				Log::Trace("TB_EQUIP_MST UPDATE QUERY Succeeded! - Input Name : %s", CCommonFunc::WCharToChar(strName.GetBuffer(0)));
// 			}
// 			else
// 			{
// 				Log::Trace("TB_EQUIP_MST UPDATE QUERY Failed! - Input Name : %s", CCommonFunc::WCharToChar(strName.GetBuffer(0)));
// 				return FALSE;
// 			}
		}
		else
		{
			Log::Trace("There is more than one row ! - Input Name : %s Please check TB_EQUIP_MST table out", CCommonFunc::WCharToChar(strName.GetBuffer(0)));
			return FALSE;
		}
	}

	//20241111 GBM - 여기에는 NMS 부분이 들어가야 하지만, 해당 매서드 자체가 현재 호출되지 않으므로 구현하지 않음

	return TRUE;
}

BOOL CNewDBManager::InsertDataIntoEquipmentInfoTable()
{	
	for (int i = EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_INPUT_TYPE; i <= EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_PUMP_MODULE; i++)
	{
		int nType = i - EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_NUMBER;

		for (int j = 0; j < MAX_EQUIP_INFO_ITEM_COUNT; j++)
		{
			CString strTemp = _T("");
			CString strQuery = _T("");
			CString strMsg = _T("");
			int nRowCount = -1;

			switch (i)
			{
			case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_INPUT_TYPE:
			{
				strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.inputType[j]));
				break;
			}
			case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_EQUIPMENT_NAME:
			{
				strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.equipmentName[j]));
				break;
			}
			case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_OUTPUT_TYPE:
			{
				strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.outputType[j]));
				break;
			}
			case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_OUTPUT_CIRCUIT:
			{
				strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.outputCircuit[j]));
				break;
			}
			case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_PUMP_EQUIPMENT:
			{
				strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.pumpEquipment[j]));
				break;
			}
			case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_PS_EQUIPMENT:
			{
				strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.psEquipment[j]));
				break;
			}
			case EXCEL_ENUM_EQUIPMENT_INFO::COLUMN_PUMP_MODULE:
			{
				strTemp.Format(_T("%s"), CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_ei.pumpModule[j]));
				break;
			}
			default:
				break;
			}
			
			if (strTemp.IsEmpty())
				continue;

			strQuery.Format(_T("SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=%d AND EQ_ID=%d"), nType, j + 1);	// EQ_ID는 1베이스
			if (!m_pDB->OpenQuery(strQuery))
			{
				strMsg.Format(_T("TB_EQUIP_MST SELECT QUERY (EQ_TYPE = %d, EQ_ID = %d COUNT) Failed! - Input Type : %s"), nType, j + 1, strTemp);
				Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
				return FALSE;
			}

			nRowCount = m_pDB->GetRecordCount();

			if (nRowCount > 0)
			{
				strQuery.Format(_T("UPDATE TB_EQUIP_MST SET EQ_NAME = \'%s\', EQ_DESC = \'%s\' WHERE EQ_TYPE = %d AND EQ_ID = %d"), strTemp, strTemp, nType, j + 1);
			}
			else
			{
				strQuery.Format(_T("INSERT INTO TB_EQUIP_MST (EQ_ID, EQ_TYPE, EQ_NAME, EQ_DESC, EQ_SYMBOL, ADD_USER) VALUES (%d, %d, \'%s\', \'%s\', \'basic.bmp\', \'admin\')"), j + 1, nType, strTemp, strTemp);
			}

			if (m_pDB->ExecuteSql(strQuery))
			{
				strMsg.Format(_T("TB_EQUIP_MST INSERT / UPDATE QUERY(EQ_TYPE = %d EQ_ID = %d COUNT) Succeeded!- Input Type : %s"), nType, j + 1, strTemp);
				Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
			}
			else
			{
				strMsg.Format(_T("TB_EQUIP_MST INSERT / UPDATE QUERY(EQ_TYPE = %d EQ_ID = %d COUNT) Failed!- Input Type : %s"), nType, j + 1, strTemp);
				Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CNewDBManager::DeleteEquipmentCircuitInfoFromEquipmentInfoTable()
{
	//입력타입, 설비명, 출력타입, 출력내용만 중계기 일람표에서 작성되므로 이 내용만 지움 -> 펌프설비, 압력스위치설비, 펌프모듈을 포함한 모든 설비 정의 삭제
	//CString strQuery = _T("DELETE TB_EQUIP_MST WHERE EQ_TYPE >= 1 AND EQ_TYPE <= 4");
	CString strQuery = _T("DELETE TB_EQUIP_MST");

	m_pDB->BeginTransaction();
	if (m_pDB->ExecuteSql(strQuery))
	{
		Log::Trace("[%s] Query Succeeded!", strQuery);
	}
	else
	{
		Log::Trace("[%s] Query Failed!", strQuery);
		m_pDB->RollbackTransaction();
		return FALSE;
	}
	m_pDB->CommitTransaction();

	return TRUE;
}

BOOL CNewDBManager::CheckAndCreateGT1DBTables()
{
	for (int nTable = TB_FACP_TYPE; nTable <= TB_UNIT_TYPE; nTable++)
	{
		BOOL bTableExist = FALSE;
		BOOL bTableCreate = FALSE;

		CString strTable = _T("");
		CString strQuery = _T("");

		strTable.Format(_T("%s"), g_lpszNewTable[nTable]);
		bTableExist = CheckDBTableExist(strTable);

		if (!bTableExist)
		{
			switch (nTable)
			{
			case TB_FACP_TYPE:
			{
				strQuery = GetQueryStringCreateFACPTypeTable();
				break;
			}
			case TB_UNIT_TYPE:
			{
				strQuery = GetQueryStringCreateUnitTypeTable();
				break;
			}
			case TB_PROJECT_INFO:
			{
				strQuery = GetQueryStringCreateProjectInfoTable();
				break;
			}
			default:
				break;
			}

			ASSERT(!strQuery.IsEmpty());

			m_pDB->BeginTransaction();
			bTableCreate = m_pDB->ExecuteSql(strQuery);
			if (bTableCreate)
			{
				Log::Trace("%s Table Creation Succeeded!", CCommonFunc::WCharToChar(strTable.GetBuffer(0)));
			}
			else
			{
				Log::Trace("%s Table Creation Failed!", CCommonFunc::WCharToChar(strTable.GetBuffer(0)));
				m_pDB->RollbackTransaction();
				return FALSE;
			}
			m_pDB->CommitTransaction();
		}
	}

	return TRUE;
}

BOOL CNewDBManager::CheckDBTableExist(CString strTableName)
{
	CString strQuery = _T("");
	strQuery.Format(_T("SELECT * FROM INFORMATION_SCHEMA.tables WITH(NOLOCK) WHERE TABLE_SCHEMA = 'dbo' AND TABLE_NAME = \'%s\'"), strTableName);

	if (!m_pDB->OpenQuery(strQuery))
	{
		Log::Trace("TABLE EXIST QUERY Failed! - TABLE NAME : %s", CCommonFunc::WCharToChar(strTableName.GetBuffer(0)));
		return TRUE;	// 쿼리 자체가 실패이므로 로그 남기고 TRUE 리턴 후 DB 내용 확인하도록 함
	}

	int nRow = -1;
	nRow = m_pDB->GetRecordCount();
	if (nRow == 0)
		return FALSE;

	return TRUE;
}

CString CNewDBManager::GetQueryStringCreateFACPTypeTable()
{
	CString strQuery = _T("");

	strQuery = _T("CREATE TABLE [dbo].[TB_FACP_TYPE]( ");
	strQuery += _T("[FACP_NUM] tinyint NOT NULL PRIMARY KEY, ");
	strQuery += _T("[FACP_TYPE] tinyint NOT NULL )");
	
	return strQuery;
}

CString CNewDBManager::GetQueryStringCreateUnitTypeTable()
{
	CString strQuery = _T("");

	strQuery = _T("CREATE TABLE [dbo].[TB_UNIT_TYPE]( ");
	strQuery += _T("[FACP_NUM] tinyint NOT NULL, ");
	strQuery += _T("[UNIT_NUM] tinyint NOT NULL, ");
	strQuery += _T("[UNIT_TYPE] tinyint NOT NULL, ");
	strQuery += _T("CONSTRAINT [PK_GROUP] PRIMARY KEY CLUSTERED ([FACP_NUM], [UNIT_NUM]))");

	return strQuery;
}

CString CNewDBManager::GetQueryStringCreateProjectInfoTable()
{
	CString strQuery = _T("");

	strQuery = _T("CREATE TABLE [dbo].[TB_PROJECT_INFO]( ");
	strQuery += _T("[PROJECT_NAME] varchar(100) NOT NULL, ");
	strQuery += _T("[MODULE_TABLE_VER_NUM] tinyint NOT NULL, ");
	strQuery += _T("[LINKED_DATA_VER_NUM] tinyint NOT NULL, ");
	strQuery += _T("[AUTHORIZED] tinyint NOT NULL )");

	return strQuery;
}

// F4 추가 Table에 데이터 insert
BOOL CNewDBManager::InsertDatasIntoGT1DBTables()
{
	if (!InsertDataIntoFACPTypeTable())
		return FALSE;

	if (!InsertDataIntoUnitTypeTable())
		return FALSE;

	//20240731 GBM start - 프로젝트 테이블 생성 안함
// 	if (!InsertDataIntoProjectInfoTable())
// 		return FALSE;
	//20240731 GBM end

	return TRUE;
}

BOOL CNewDBManager::InsertDataIntoFACPTypeTable()
{
	CString strQuery = _T("DELETE FROM [dbo].[TB_FACP_TYPE]");
	if (!m_pDB->ExecuteSql(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
	}

	for (int i = 0; i < MAX_FACP_COUNT; i++)
	{
		int nType = -1;
		nType = CNewInfo::Instance()->m_gi.facpType[i];

		ASSERT(nType > -1);
		if (nType == 0)	//수신기 Type은 1부터 시작, 0이라는 건 안쓰이는 수신기
		{
			Log::Trace("The total number of inserted FACP types is %d", i);
			break;
		}

		strQuery.Format(_T("INSERT INTO TB_FACP_TYPE (FACP_NUM, FACP_TYPE) VALUES (%d, %d)"), i, nType);

		if (!m_pDB->ExecuteSql(strQuery))
		{
			Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNewDBManager::InsertDataIntoUnitTypeTable()
{
	CString strQuery = _T("DELETE FROM [dbo].[TB_UNIT_TYPE]");
	if (!m_pDB->ExecuteSql(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
	}

	BOOL bExit = FALSE;
	for (int i = 0; i < MAX_FACP_COUNT; i++)
	{
		// 수신기 타입을 읽어와서 0이면 존재하지 않는 수신기이므로 이후 루프를 돌지 않음
		int nFACPType = -1;
		nFACPType = CNewInfo::Instance()->m_gi.facpType[i];
		if (nFACPType == 0)
		{
			break;
		}

		for (int j = 0; j < MAX_UNIT_COUNT; j++)
		{
			int nType = -1;

			nType = CNewInfo::Instance()->m_gi.unitType[i][j];

			ASSERT((nType > -1) || (nType <= 피난사다리));

			if(nType == 0)
			{
				Log::Trace("The total number of inserted unit types of FACP[%02d] is %d", i, j);
				break;
			}

			strQuery.Format(_T("INSERT INTO TB_UNIT_TYPE (FACP_NUM, UNIT_NUM, UNIT_TYPE) VALUES (%d, %d, %d)"), i, j, nType);

			if (!m_pDB->ExecuteSql(strQuery))
			{
				Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CNewDBManager::InsertDataIntoProjectInfoTable()
{
	//만약 모든 수신기가 F3라서 프로젝트명이 존재하지 않으면 DB Insert를 진행하지 않음
	CString strProjectName = _T("");
	strProjectName = CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_gi.projectInfo.projectName);
	if(strProjectName.IsEmpty())
	{ 
		Log::Trace("There in no project information!");
		return TRUE;
	}

	CString strQuery = _T("DELETE FROM [dbo].[TB_PROJECT_INFO]");
	if (!m_pDB->ExecuteSql(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
	}

	int nModuleTableVerNum = -1;
	int nLinkedDataVerNum = -1;
	int bAuthorized = -1;

	nModuleTableVerNum = CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum;
	ASSERT(nModuleTableVerNum != -1);

	nLinkedDataVerNum = CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum;
	ASSERT(nLinkedDataVerNum != -1);

	bAuthorized = CNewInfo::Instance()->m_gi.projectInfo.authorized;
	ASSERT(bAuthorized != -1);

	strQuery.Format(_T("INSERT INTO TB_PROJECT_INFO (PROJECT_NAME, MODULE_TABLE_VER_NUM, LINKED_DATA_VER_NUM, AUTHORIZED) VALUES (\'%s\', %d, %d, %d)"),
		strProjectName, nModuleTableVerNum, nLinkedDataVerNum, bAuthorized);

	if (!m_pDB->ExecuteSql(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
		return FALSE;
	}

	return TRUE;
}

BOOL CNewDBManager::GetDataFromGT1DBTables()
{
	if (!GetDataFromFACPTypeTable())
		return FALSE;

	if (!GetDataFromUnitTypeTable())
		return FALSE;

	//20240731 GBM start - 프로젝트 테이블 제외
// 	if (!GetDataFromProjectInfoTable())
// 		return FALSE;
	//20240731 GBM end

	return TRUE;
}

BOOL CNewDBManager::GetDataFromFACPTypeTable()
{
	CString strQuery = _T("SELECT * FROM [dbo].[TB_FACP_TYPE]");

	if (!m_pDB->OpenQuery(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
		return FALSE;
	}

	int nRecordCount = -1;
	nRecordCount = m_pDB->GetRecordCount();
	if (nRecordCount > 0)
	{
		int nFACP = -1;
		int nType = -1;

		for (int i = 0; i <= nRecordCount; i++)
		{
			m_pDB->GetFieldValue(_T("FACP_NUM"), nFACP);
			m_pDB->GetFieldValue(_T("FACP_TYPE"), nType);

			ASSERT(nFACP > -1);
			ASSERT(nType > -1);

			CNewInfo::Instance()->m_gi.facpType[nFACP] = nType;

			m_pDB->MoveNext();
		}
	}
	else
	{
// 		Log::Trace("The number of records in the TB_FACP_TYPE table is 0.");
// 		return FALSE;
	}

	return TRUE;
}

BOOL CNewDBManager::GetDataFromUnitTypeTable()
{
	CString strQuery = _T("SELECT * FROM [dbo].[TB_UNIT_TYPE]");

	if (!m_pDB->OpenQuery(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
		return FALSE;
	}

	int nRecordCount = -1;
	nRecordCount = m_pDB->GetRecordCount();
	if (nRecordCount > 0)
	{
		for (int i = 0; i < nRecordCount; i++)
		{
			int nFACP = -1;
			int nUnit = -1;
			int nType = -1;

			m_pDB->GetFieldValue(_T("FACP_NUM"), nFACP);
			m_pDB->GetFieldValue(_T("UNIT_NUM"), nUnit);
			m_pDB->GetFieldValue(_T("UNIT_TYPE"), nType);

			ASSERT(nFACP > -1);
			ASSERT(nUnit > -1);
			ASSERT(nType > 0);

			CNewInfo::Instance()->m_gi.unitType[nFACP][nUnit] = nType;

			m_pDB->MoveNext();
		}
	}
	else
	{
// 		Log::Trace("The number of records in the TB_UNIT_TYPE table is 0.");
// 		return FALSE;
	}

	return TRUE;
}

BOOL CNewDBManager::GetDataFromProjectInfoTable()
{
	CString strQuery = _T("SELECT * FROM [dbo].[TB_PROJECT_INFO]");

	if (!m_pDB->OpenQuery(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
		return FALSE;
	}

	int nRecordCount = -1;
	nRecordCount = m_pDB->GetRecordCount();
	if (nRecordCount > 0)
	{
		CString strProjectName = _T("");
		int nModuleTableVerNum = -1;
		int nLinkedDataVerNum = -1;
		int nAuthorized = -1;

		m_pDB->GetFieldValue(_T("PROJECT_NAME"), strProjectName);
		m_pDB->GetFieldValue(_T("MODULE_TABLE_VER_NUM"), nModuleTableVerNum);
		m_pDB->GetFieldValue(_T("LINKED_DATA_VER_NUM"), nLinkedDataVerNum);
		m_pDB->GetFieldValue(_T("AUTHORIZED"), nAuthorized);

		ASSERT(strProjectName.GetLength() > 0);
		ASSERT(nModuleTableVerNum > 0);
		ASSERT(nLinkedDataVerNum > -1);
		ASSERT(nAuthorized > -1);

		strcpy_s(CNewInfo::Instance()->m_gi.projectInfo.projectName, CCommonFunc::WCharToChar(strProjectName.GetBuffer(0)));
		CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum = nModuleTableVerNum;
		CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum = nLinkedDataVerNum;
		CNewInfo::Instance()->m_gi.projectInfo.authorized = nAuthorized;
	}
	else
	{
// 		Log::Trace("The number of records in the TB_PROJECT_INFO table is 0.");
// 		return FALSE;
	}

	return TRUE;
}