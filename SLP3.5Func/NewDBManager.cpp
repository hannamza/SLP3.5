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

	for (int i = AN광전교차A; i <= 광센서감지기; i++)
	{
		CString strEquip = _T("");
		strEquip.Format(_T("%s"), g_lpszNewEquipmentInputType[i]);

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
			if (!m_pDB->ExecuteSql(strQuery))
			{
				Log::Trace("TB_EQUIP_MST INSERT QUERY Failed! - Input Type : %s", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CNewDBManager::CheckAndCreateF4DBTables()
{
	for (int nTable = TB_FACP_TYPE; nTable <= TB_PROJECT_INFO; nTable++)
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
			case TB_CCTV_INFO:
			{
				strQuery = GetQueryStringCreateCCTVInfoTable();
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

CString CNewDBManager::GetQueryStringCreateCCTVInfoTable()
{
	CString strQuery = _T("");

	strQuery = _T("CREATE TABLE [dbo].[TB_CCTV_INFO]( ");
	strQuery += _T("[NO] int NOT NULL PRIMARY KEY, ");
	strQuery += _T("[CCTV_TYPE] tinyint NOT NULL, ");
	strQuery += _T("[COMPANY_TYPE] tinyint NOT NULL, ");
	strQuery += _T("[IP] varchar(16), ");
	strQuery += _T("[PORT] int, ");
	strQuery += _T("[URL] varchar(2084), ");
	strQuery += _T("[CAMERA_COUNT] int, ");
	strQuery += _T("[ID] varchar(20), ");
	strQuery += _T("[PASSWORD] varchar(20), ");
	strQuery += _T("[RESERVED] bigint )");

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
BOOL CNewDBManager::InsertDatasIntoF4DBTables()
{
	if (!InsertDataIntoFACPTypeTable())
		return FALSE;

	if (!InsertDataIntoUnitTypeTable())
		return FALSE;

	if (!InsertDataIntoCCTVInfoTable())
		return FALSE;

	if (!InsertDataIntoProjectInfoTable())
		return FALSE;

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
		nType = CNewInfo::Instance()->m_fi.facpType[i];

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
		nFACPType = CNewInfo::Instance()->m_fi.facpType[i];
		if (nFACPType == 0)
		{
			break;
		}

		for (int j = 0; j < MAX_UNIT_COUNT; j++)
		{
			int nType = -1;

			nType = CNewInfo::Instance()->m_fi.unitType[i][j];

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

BOOL CNewDBManager::InsertDataIntoCCTVInfoTable()
{
	CString strQuery = _T("DELETE FROM [dbo].[TB_CCTV_INFO]");
	if (!m_pDB->ExecuteSql(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
	}

	for (int i = 0; i < MAX_CCTV_COUNT; i++)
	{
		int nNum = -1;
		int nCCTVType = 0;
		int nCompanyType = 0;
		CString strIP = _T("");
		int nPort = -1;
		CString strURL = _T("");
		int nCameraCount = -1;
		CString strID = _T("");
		CString strPassword = _T("");
		double nReserved = 0;

		//번호는 1베이스
		nNum = i + 1;

		nCCTVType = CNewInfo::Instance()->m_fi.cctvInfo[i].cctvType;
		if(nCCTVType == 0)
		{
			Log::Trace("The total number of inserted CCTV information is %d", i);
			break;
		}

		nCompanyType = CNewInfo::Instance()->m_fi.cctvInfo[i].companyType;

		strIP = CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_fi.cctvInfo[i].ip);

		nPort = CNewInfo::Instance()->m_fi.cctvInfo[i].port;

		strURL = CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_fi.cctvInfo[i].url);

		nCameraCount = CNewInfo::Instance()->m_fi.cctvInfo[i].cameraCount;

		strID = CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_fi.cctvInfo[i].id);

		strPassword = CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_fi.cctvInfo[i].password);

		nReserved = CNewInfo::Instance()->m_fi.cctvInfo[i].reserved;

		strQuery.Format(_T("INSERT INTO TB_CCTV_INFO (NO, CCTV_TYPE, COMPANY_TYPE, IP, PORT, URL, CAMERA_COUNT, ID, PASSWORD, RESERVED) VALUES (%d, %d, %d, \'%s\', %d, \'%s\', %d, \'%s\', \'%s\', %d)"), 
			nNum, nCCTVType, nCompanyType, strIP, nPort, strURL, nCameraCount, strID, strPassword, nReserved);

		if (!m_pDB->ExecuteSql(strQuery))
		{
			Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNewDBManager::InsertDataIntoProjectInfoTable()
{
	CString strQuery = _T("DELETE FROM [dbo].[TB_PROJECT_INFO]");
	if (!m_pDB->ExecuteSql(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
	}

	CString strProjectName = _T("");
	int nModuleTableVerNum = -1;
	int nLinkedDataVerNum = -1;
	int bAuthorized = -1;

	strProjectName = CCommonFunc::CharToWCHAR(CNewInfo::Instance()->m_pi.projectName);

	nModuleTableVerNum = CNewInfo::Instance()->m_pi.moduleTableVerNum;
	ASSERT(nModuleTableVerNum != -1);

	nLinkedDataVerNum = CNewInfo::Instance()->m_pi.linkedDataVerNum;
	ASSERT(nLinkedDataVerNum != -1);

	bAuthorized = CNewInfo::Instance()->m_pi.authorized;
	ASSERT(bAuthorized != -1);

	strQuery.Format(_T("INSERT INTO TB_PROJECT_INFO (PROJECT_NAME, MODULE_TABLE_VER_NUM, LINKED_DATA_VER_NUM, AUTHORIZED) VALUES (\'%s\', %d, %d, %d)"),
		strProjectName, nModuleTableVerNum, nLinkedDataVerNum, bAuthorized);

	if (!m_pDB->ExecuteSql(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
		return FALSE;
	}
}

BOOL CNewDBManager::GetDataFromF4DBTables()
{
	if (!GetDataFromFACPTypeTable())
		return FALSE;

	if (!GetDataFromUnitTypeTable())
		return FALSE;

	if (!GetDataFromCCTVInfoTable())
		return FALSE;

	if (!GetDataFromProjectInfoTable())
		return FALSE;

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

			CNewInfo::Instance()->m_fi.facpType[nFACP] = nType;

			m_pDB->MoveNext();
		}
	}
	else
	{
		Log::Trace("The number of records in the TB_FACP_TYPE table is 0.");
		return FALSE;
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

			CNewInfo::Instance()->m_fi.unitType[nFACP][nUnit] = nType;

			m_pDB->MoveNext();
		}
	}
	else
	{
		Log::Trace("The number of records in the TB_UNIT_TYPE table is 0.");
		return FALSE;
	}

	return TRUE;
}

BOOL CNewDBManager::GetDataFromCCTVInfoTable()
{
	CString strQuery = _T("SELECT * FROM [dbo].[TB_CCTV_INFO]");

	if (!m_pDB->OpenQuery(strQuery))
	{
		Log::Trace("[%s] query failed!", CCommonFunc::WCharToChar(strQuery.GetBuffer(0)));
		return FALSE;
	}

	int nRecordCount = -1;
	nRecordCount = m_pDB->GetRecordCount();
	if (nRecordCount > 0)	// CCTV는 없을 수도 있음
	{
		for (int i = 0; i < nRecordCount; i++)
		{
			int nNum = -1;
			int nCCTVType = 0;
			int nCompanyType = 0;
			CString strIP = _T("");
			int nPort = 0;
			CString strURL = _T("");
			int nCameraCount = 0;
			CString strID = _T("");
			CString strPassword = _T("");
			double nReserved = 0;

			m_pDB->GetFieldValue(_T("NO"), nNum);
			ASSERT(nNum > -1);
			m_pDB->GetFieldValue(_T("CCTV_TYPE"), nCCTVType);
			m_pDB->GetFieldValue(_T("COMPANY_TYPE"), nCompanyType);
			m_pDB->GetFieldValue(_T("IP"), strIP);
			m_pDB->GetFieldValue(_T("PORT"), nPort);
			m_pDB->GetFieldValue(_T("URL"), strURL);
			m_pDB->GetFieldValue(_T("CAMERA_COUNT"), nCameraCount);
			m_pDB->GetFieldValue(_T("ID"), strID);
			m_pDB->GetFieldValue(_T("PASSWORD"), strPassword);
			m_pDB->GetFieldValue(_T("RESERVED"), nReserved);


			ASSERT(nNum > 0);
			nNum -= 1;	// 번호는 1베이스, 인덱스는 0베이스
			CNewInfo::Instance()->m_fi.cctvInfo[nNum].cctvType = nCCTVType;
			CNewInfo::Instance()->m_fi.cctvInfo[nNum].companyType = nCompanyType;
			strcpy_s(CNewInfo::Instance()->m_fi.cctvInfo[nNum].ip, CCommonFunc::WCharToChar(strIP.GetBuffer(0)));
			CNewInfo::Instance()->m_fi.cctvInfo[nNum].port = nPort;
			strcpy_s(CNewInfo::Instance()->m_fi.cctvInfo[nNum].url, CCommonFunc::WCharToChar(strURL.GetBuffer(0)));
			CNewInfo::Instance()->m_fi.cctvInfo[nNum].cameraCount = nCameraCount;
			strcpy_s(CNewInfo::Instance()->m_fi.cctvInfo[nNum].id, CCommonFunc::WCharToChar(strID.GetBuffer(0)));
			strcpy_s(CNewInfo::Instance()->m_fi.cctvInfo[nNum].password, CCommonFunc::WCharToChar(strPassword.GetBuffer(0)));
			CNewInfo::Instance()->m_fi.cctvInfo[nNum].reserved = nReserved;

			m_pDB->MoveNext();
		}
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

		strcpy_s(CNewInfo::Instance()->m_pi.projectName, CCommonFunc::WCharToChar(strProjectName.GetBuffer(0)));
		CNewInfo::Instance()->m_pi.moduleTableVerNum = nModuleTableVerNum;
		CNewInfo::Instance()->m_pi.linkedDataVerNum = nLinkedDataVerNum;
		CNewInfo::Instance()->m_pi.authorized = nAuthorized;
	}
	else
	{
		Log::Trace("The number of records in the TB_PROJECT_INFO table is 0.");
		return FALSE;
	}

	return TRUE;
}