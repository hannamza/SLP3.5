// YAdoDatabase.cpp: implementation of the YAdoDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "YAdoDatabase.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//IMPLEMENT_DYNAMIC(YAdoDatabase, YAdoDatabase)
// size of the chunk
#define YADO_CHUNKSIZE	2000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void WriteAdoLog(const WCHAR * szFmt, ...)
{
	CString     strLog;
	va_list     argptr;
	int         cnt;
	SYSTEMTIME  tm;
	FILE        *fpLog = NULL;
	WCHAR        szLog[30240] = { 0 };
	return;
	GetLocalTime(&tm);
	strLog.Format(L"C:\\FAS\\Log\\AdoLog%04d%02d%02d.log",tm.wYear, tm.wMonth, tm.wDay);

	va_start(argptr, szFmt);
	cnt = vswprintf_s(szLog, szFmt, argptr);
	va_end(argptr);

	//fpLog = fopen(strLog, "a+");
	//if (fpLog)
	//{
	//	fprintf(fpLog, "[%02d:%02d:%02d] %s\n",
	//		tm.wHour, tm.wMinute, tm.wSecond,
	//		szLog);

	//	fclose(fpLog);
	//}

	int nError = _wfopen_s(&fpLog, strLog, L"a+, ccs=UTF-8");
	if (fpLog != NULL)
	{
		fwprintf(fpLog, L"[%02d:%02d:%02d.%03d] %s\n",
			tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds,
			szLog);

		fclose(fpLog);
	}
}

CString IntToStr(int nVal)
{
	CString strRet;
//	char buff[10];

//	itoa(nVal, buff, 10);
//	strRet = buff;
	strRet.Format ( _T("%d") , nVal );
	return strRet;
}

CString LongToStr(long lVal)
{
	CString strRet;
//	char buff[20];

//	ltoa(lVal, buff, 10);
//	strRet = buff;
	strRet.Format ( _T("%ld") , lVal );
	return strRet;
}

CString ULongToStr(unsigned long ulVal)
{
	CString strRet;
//	char buff[20];

//	ultoa(ulVal, buff, 10);
//	strRet = buff;
	strRet.Format ( _T("%ld") , ulVal );
	return strRet;
}


CString DblToStr(double dblVal, int ndigits)
{
	CString strRet;
	char buff[50];

	_gcvt_s(buff , dblVal, ndigits);
	strRet = buff;
	return strRet;
}

CString DblToStr(float fltVal)
{
	CString strRet = _T("");
	char buff[50];

	_gcvt_s(buff , fltVal, 10);
	strRet = buff;
	return strRet;
}

YAdoDatabase::YAdoDatabase()
{
    ::CoInitialize(NULL);

    m_enDatabase		= enODBC;
    m_dwLastError		= S_OK;

    m_pConnection		= NULL;
    m_pCommand			= NULL;
    m_pRecordSet		= NULL;
    m_pRecordBinding	= NULL;

    m_strProvider		= S_DB_MDB_PROVIDER;
    m_strUserId			= S_DB_USER_ID;
    m_strUserPasswd		= S_DB_USER_PASSWD;
    
    m_strConnectString  = _T("");
    m_strDataSource     = _T("");
    m_strDatabaseName   = _T("");
    m_strDatabaseString = _T("");

    m_bConnection		= FALSE;
    m_bRecordset		= FALSE;
    m_bCommand			= FALSE;
    m_bOpen				= FALSE;
	m_bRSOpen			= FALSE;

	m_nFields			= 0;
	m_nRecords			= 0;
}

YAdoDatabase::~YAdoDatabase()
{
    ::CoUninitialize();
    DBClose();
}

#ifdef _DEBUG
void YAdoDatabase::AssertValid() const
{
	CObject::AssertValid();
}

void YAdoDatabase::Dump(CDumpContext& dc) const
{	
	CObject::Dump(dc);
}
#endif //_DEBUG

void YAdoDatabase::SetDatabaseType ( EN_YADO_DATABASE enDatabase /*= enMDB*/ )
{
    m_enDatabase	= enDatabase;
	switch (enDatabase)
	{
	case enMDB:
		m_strProvider = S_DB_MDB_PROVIDER;
		break;
	case enSQL:
		m_strProvider = S_DB_SQL_PROVIDER;
		break;
	case enODBC:
		//m_strProvider = (enMDB == m_enDatabase) ? S_DB_MDB_PROVIDER : S_DB_SQL_PROVIDER;
		break;
	case enACCDB:
		m_strProvider = S_DB_ACCDB_PROVIDER;
		break;
	}
}

void YAdoDatabase::SetDatabaseName ( LPCTSTR lpszDatabaseName )
{
    m_strDatabaseName = lpszDatabaseName;
    
    if ( m_enDatabase == enMDB || m_enDatabase == enACCDB )
	{
        m_strDataSource = _T("Data Source=") + m_strDatabaseName + _T(";");
	}
    m_strDatabaseString = _T("Database=") + m_strDatabaseName + _T(";");

    MakeConnectString();
}

void YAdoDatabase::SetProvider ( LPCTSTR lpszProvider ) 
{
    CString strProvider = lpszProvider;
    m_strProvider		= _T("Provider=") + strProvider + _T(";");

    MakeConnectString();
}

void YAdoDatabase::SetDataSource ( LPCTSTR lpszDataSource ) 
{
    CString strDataSource	= lpszDataSource;
    m_strDataSource			= _T("Data Source=") + strDataSource + _T(";");

    MakeConnectString();
}


void YAdoDatabase::MakeConnectString()
{
    m_strConnectString = m_strProvider + m_strDataSource;
    
    if ( enSQL == m_enDatabase )
    {
        m_strConnectString += m_strDatabaseString + m_strUserId + m_strUserPasswd;
    }
}

void YAdoDatabase::ODBCInitialize(CString strDsn, CString strDB, CString strId, CString strPassword)
{
	CString strTemp;
	m_strConnectString = _T("");

	strTemp.Format ( _T("DSN=%s;") , strDsn );
	m_strConnectString += strTemp;

	if (!strDB.IsEmpty())
	{
		strTemp.Format(_T("DATABASE=%s;"), strDB);
		m_strConnectString += strTemp;
	}

	if ( !strId.IsEmpty() )
	{
		strTemp.Format ( _T("UID=%s;") , strId );
		m_strConnectString += strTemp;
	}

	if ( !strPassword.IsEmpty() )
	{
		strTemp.Format ( _T("PWD=%s;") , strPassword );
		m_strConnectString += strTemp;
	}
	SetDatabaseType(enODBC);

}

void YAdoDatabase::MSSqlInitialize(LPCTSTR lpszUserPswd, LPCTSTR lpszUserName, LPCTSTR lpszDbName, LPCTSTR lpszDbServer,UINT nPort)
{
	if(nPort == 1433)
		m_strConnectString.Format(_T("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s"),
			lpszUserPswd,lpszUserName, lpszDbName ,lpszDbServer);
	else
		m_strConnectString.Format(
			_T("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s,%d"),
			lpszUserPswd,lpszUserName,lpszDbName,lpszDbServer,nPort);
	m_strDatabaseName = lpszDbName;
	m_strDataSource = lpszDbServer;
	m_strUserId = lpszUserName;
	m_strUserPasswd = lpszUserPswd;
	SetDatabaseType(enSQL);
}

void YAdoDatabase::MySqlInitialize(LPCTSTR lpszUserPswd, LPCTSTR lpszUserName
	, LPCTSTR lpszDbName, LPCTSTR lpszDbServer, UINT nPort)
{
	m_strConnectString.Format(_T("Driver={MariaDB ODBC 3.1 Driver};TCPIP=1;Server=%s;Port=%d;Database=%s;UID=%s;PWD=%s;Option=2; ")
		//_T("Database=%s;UID=%s;PWD=%s;Option=2;")
		, lpszDbServer, nPort, lpszDbName, lpszUserName, lpszUserPswd
	);
	m_strDatabaseName = lpszDbName;
	m_strDataSource = lpszDbServer;
	m_strUserId = lpszUserName;
	m_strUserPasswd = lpszUserPswd;
	SetDatabaseType(enMySQL);

}

void YAdoDatabase::MdbInitialize ( LPCTSTR lpszFileName , LPCTSTR lpszPassword )
{
	if ( lpszPassword )
	{
		m_strConnectString.Format ( _T("Provider=Microsoft.Jet.OLEDB.4.0; Data Source=%s; Password=\"%s\"; ") ,
																			lpszFileName , lpszPassword );
	}
	else
	{
		m_strConnectString.Format ( _T("Provider=Microsoft.Jet.OLEDB.4.0; Data Source=%s;") , lpszFileName );
	}

	SetDatabaseType(enMDB);

}


void YAdoDatabase::MdbAccdbInitialize(LPCTSTR lpszFileName, LPCTSTR lpszPassword)
{
	if (lpszPassword)
	{
		m_strConnectString.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0; Data Source=%s;")
			_T("Jet OLEDB:Database Password=\"%s\""),
			lpszFileName, lpszPassword);
	}
	else
	{
		m_strConnectString.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0; Data Source=%s;")
			_T("Persist Security Info=False;"), lpszFileName);
	}
	SetDatabaseType(enACCDB);

}


BOOL YAdoDatabase::DBOpen()
{
    m_bConnection = FALSE;
    m_bRecordset  = FALSE;
    m_bCommand    = FALSE;

    if ( m_pConnection )
    {
        if ( m_pConnection->State != adStateOpen )
        {
            CloseConnection();
        }
    }

    if ( !OpenConnection() )
	{
        return FALSE;
	}
    m_bOpen = TRUE;

    return TRUE;
}





CString YAdoDatabase::BSTRtoCString ( BSTR bstrTemp )
{
    CString strConvert;
 
    if(bstrTemp == NULL)
        return strConvert;  // empty for NULL BSTR
 
    #ifdef UNICODE
       strConvert = bstrTemp;
    #else
       LPSTR pStr = strConvert.GetBuffer(SysStringLen(bstrTemp) + 1);
 
       ::WideCharToMultiByte(CP_ACP,   // ANSI Code Page
               0,                 // no flags
               bstrTemp,      // source widechar string
               -1,                // assume NUL-terminated
               pStr,             // target buffer
               SysStringLen(bstrTemp)+1,  // target buffer length
               NULL,           // use system default char
               NULL);          // don't care if default used
       strConvert.ReleaseBuffer();
    #endif
 
 
 
    return strConvert;
}

BOOL YAdoDatabase::OpenConnection()
{
    BOOL bRet = TRUE;

    HRESULT hr = S_OK;

    try
    {
        hr = m_pConnection.CreateInstance( __uuidof ( Connection ) );
        if ( FAILED ( hr ) )
		{
            _com_issue_error ( hr );
		}
        m_pConnection->ConnectionTimeout = 30;
		//m_pConnection->Mode = adModeShareDenyNone;
        hr = m_pConnection->Open ( _bstr_t(m_strConnectString) , _bstr_t(L"") , _bstr_t(L"") ,
									adConnectUnspecified );
        if ( FAILED ( hr ) )
		{
            _com_issue_error ( hr );
		}
        bstr_t str = m_pConnection->GetConnectionString();
        //PropertyPtr ptr = m_pConnection->GetProperties();
        m_bConnection = TRUE;

        hr = m_pCommand.CreateInstance ( __uuidof ( Command ) );
        if ( FAILED ( hr ) )
		{
            _com_issue_error ( hr );
        }

        try
        {
            m_pCommand->ActiveConnection = m_pConnection;
        }
        catch(...)
        {
            _com_issue_error ( hr );
        }

        m_bCommand = TRUE;

        hr = m_pRecordSet.CreateInstance ( __uuidof ( Recordset ) );
        if ( FAILED ( hr ) )
		{
            _com_issue_error ( hr );
		}

        hr = m_pRecordSet.QueryInterface ( __uuidof ( IADORecordBinding ) , (LPVOID*)&m_pRecordBinding );
        if ( FAILED ( hr ) )
		{
            _com_issue_error ( hr );
		}
        m_bRecordset = TRUE;
    }
    catch ( const _com_error& e )
    {
        bRet = FALSE;
        SetLastError ( (DWORD)e.Error() );
//        SetLastErrorString ( (LPCTSTR)e.Description() );
		m_strErrorString.Format (_T("열기 오류[%08X] :") , (DWORD)e.Error() );
		m_strErrorString += (LPCTSTR)e.Description();
        TRACE ( _T("Open Failed: %s\n") , m_strErrorString );
		//AfxMessageBox ( m_strErrorString );
#ifdef g_LogFile
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "Open Failed: %s\n") , m_strErrorString );
#endif

        if ( m_bConnection )
		{
            CloseConnection();
		}
    }

    return bRet;
}

void YAdoDatabase::DBClose()
{
    if ( m_bConnection )
    {
        CloseConnection();
        m_bConnection	= FALSE;
        ::CoUninitialize();
    }

    m_pConnection.Detach();
	m_pCommand.Detach();
	m_pRecordSet.Detach();

    m_bOpen = FALSE;
}


BOOL YAdoDatabase::IsExistTable(CString strTable)
{
	BOOL bRet = FALSE;
	CString strSql;
	HRESULT	hr = E_FAIL;
	_bstr_t table_name;
	try
	{
		m_pRecordSet = m_pConnection->OpenSchema(adSchemaTables);
		strSql.Format(L"TABLE_NAME LIKE '%s'", strTable);
		hr = m_pRecordSet->Find(_bstr_t(strSql), 0, ADODB::adSearchForward, L"");
		if (SUCCEEDED(hr))
		{
			while (!(m_pRecordSet->GetadoEOF()))
			{
				table_name = m_pRecordSet->Fields->GetItem("TABLE_NAME")->Value;
				if (strTable.CompareNoCase(table_name) == 0)
				{
					bRet = TRUE;
					break;
				}
				m_pRecordSet->MoveNext();
			}
		}
		m_pRecordSet->Close();
		
	}
	catch (_com_error &e)
	{
		CString strErr;
		SetLastError((DWORD)e.Error());
		SetLastErrorString((LPCTSTR)e.Description());
		strErr.Format(_T("IsExistTable[LINE-%d] Error[%d]:%s\n"), __LINE__,
			e.Error(), (LPTSTR)(LPCTSTR)e.Description());

		TRACE(strErr);
		WriteAdoLog(strErr);

#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage(LOG_FILE_ERROR, _T("%s\r\n"), strErr);
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE(_T("Schema Open Exception...\n"));
		return FALSE;
	}
	return bRet;
}


BOOL YAdoDatabase::OpenTableInfo()
{
	try
	{
		m_pRecordSet = m_pConnection->OpenSchema ( adSchemaTables );

		m_nRecords = m_pRecordSet->GetRecordCount();

		if(m_nRecords == -1)
		{
			if(m_pRecordSet->adoEOF != VARIANT_TRUE)
			{
				m_pRecordSet->MoveFirst();
				m_nRecords = 0;
				while(m_pRecordSet->adoEOF != VARIANT_TRUE)
				{
					m_nRecords++;
					m_pRecordSet->MoveNext();
				}
				m_pRecordSet->MoveFirst();
			}
		}
/*
		while ( !(pRecordset->GetadoEOF() ) )
		{
			_bstr_t table_name = pRecordset->Fields->GetItem("TABLE_NAME")->Value;
			TRACE ("Table Name: %s\n",(LPCSTR) table_name);
			_bstr_t table_type = pRecordset->Fields->GetItem("TABLE_TYPE")->Value;
			TRACE ("Table type: %s\n",(LPCSTR) table_type);

			pRecordset->MoveNext();
		}
		pRecordset->Close();
		*/
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format ( _T("OpenTableInfo[LINE-%d] Error[%d]:%s\n") , __LINE__ , 
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);

#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , _T("%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Schema Open Exception...\n") );
		return FALSE;
	}
	return m_pRecordSet != NULL;
}

void YAdoDatabase::CloseConnection()
{
    //ASSERT(m_pConnection != NULL);

    HRESULT hr = 0;
    try
    {
        if ( m_bRecordset )
        {
            if ( m_pRecordSet->State != adStateClosed )
            {
                hr = m_pRecordSet->Close();
                if ( FAILED ( hr ) )
                    _com_issue_error ( hr );
            }
        }

        if ( m_pConnection->State != adStateClosed )
        {
            hr = m_pConnection->Close();
            if ( FAILED ( hr ) )
			{
                _com_issue_error ( hr );
			}
        }

    }
    catch ( const _com_error& e )
    {
        SetLastError ( (DWORD)e.Error() );
        SetLastErrorString ( (LPCTSTR)e.Description() );
        AfxMessageBox ( GetLastErrorString() );
    }

    m_bConnection = FALSE;
    m_bRecordset  = FALSE;
    m_bCommand    = FALSE;
}

BOOL YAdoDatabase::ExecuteSql(LPCTSTR lpszSql, BOOL bCheckCnt)
{
    CString strSql;
    strSql = lpszSql;

    return ExecuteSql(strSql, bCheckCnt);
}

BOOL YAdoDatabase::ExecuteSql ( CString &strSql , BOOL bCheckCnt )
{
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.lVal= 0;
	_RecordsetPtr pRet = nullptr;
    try
    {
        if ( NULL != m_pConnection )
        {
            if ( m_pConnection->State == adStateClosed )
            {
                if ( !OpenConnection() )
				{
                    return FALSE;
				}
            }
        }

//        m_pCommand->CommandText = _bstr_t ( strSql );
//		BSTR str = ConvertStringToBSTR ( (LPSTR)(LPCSTR)strSql );
		//BSTR str = strSql.AllocSysString();// ConvertStringToBSTR ( (LPSTR)(LPCSTR)strSql );
        //m_pCommand->CommandText = str;
		m_pCommand->CommandText = _bstr_t ( strSql );
		pRet = m_pCommand->Execute (&vtFld, NULL , adCmdText );
		if (bCheckCnt == TRUE)
		{
			if (vtFld.lVal <= 0)
			{
				return FALSE;
			}
		}
		
		// 메모리 해제 해야 하나? 확인하자.  SysFreeString ( str ); 

    }
    catch ( _com_error &e )
    {
        CString strErr;        
        SetLastError ( (DWORD)e.Error() );

        SetLastErrorString ( (LPCTSTR)e.Description() );
        strErr.Format ( _T("Error[%d]:%s\n%s\n")
			, e.Error(), (LPTSTR)(LPCTSTR)e.Description() 
			, strSql);
		//TRACE(strErr);
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
	g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
        return FALSE;
    }

	return TRUE;
}

BOOL YAdoDatabase::ExecuteSqlTimeOut(LPCTSTR lpszSql, DWORD dwTimeOut, BOOL bCheckCnt)
{
	CString strSql;
	strSql = lpszSql;

	return ExecuteSqlTimeOut(strSql, dwTimeOut, bCheckCnt);
}

BOOL YAdoDatabase::ExecuteSqlTimeOut(CString &strSql, DWORD dwTimeOut, BOOL bCheckCnt)
{
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.lVal = 0;
	_RecordsetPtr pRet = nullptr;
	try
	{
		if (NULL != m_pConnection)
		{
			if (m_pConnection->State == adStateClosed)
			{
				if (!OpenConnection())
				{
					return FALSE;
				}
			}
		}

		//        m_pCommand->CommandText = _bstr_t ( strSql );
		//		BSTR str = ConvertStringToBSTR ( (LPSTR)(LPCSTR)strSql );
		//BSTR str = strSql.AllocSysString();// ConvertStringToBSTR ( (LPSTR)(LPCSTR)strSql );
		//m_pCommand->CommandText = str;
		m_pCommand->CommandTimeout = dwTimeOut;
		m_pCommand->CommandText = _bstr_t(strSql);
		pRet = m_pCommand->Execute(&vtFld, NULL, adCmdText);
		if (bCheckCnt == TRUE)
		{
			if (vtFld.lVal <= 0)
			{
				return FALSE;
			}
		}

		// 메모리 해제 해야 하나? 확인하자.  SysFreeString ( str ); 

	}
	catch (_com_error &e)
	{
		CString strErr;
		SetLastError((DWORD)e.Error());

		SetLastErrorString((LPCTSTR)e.Description());
		strErr.Format(_T("Error[%d]:%s\n%s\n")
			, e.Error(), (LPTSTR)(LPCTSTR)e.Description()
			, strSql);
		TRACE(strErr);
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage(LOG_FILE_ERROR, "%s\r\n"), strErr );
#endif
		return FALSE;
	}

	return TRUE;
}

#if 0
BOOL YAdoDatabase::OpenQuery(LPCTSTR lpstrExec, int nOption)
{	
	RSClose();
	CString strQuery;
	if ( strcmp ( lpstrExec , _T("") ) != 0 )
		strQuery = lpstrExec;

	ASSERT(!strQuery.IsEmpty());

	strQuery.TrimLeft();
	BOOL bIsSelect = strQuery.Mid(0, strlen("Select ")).CompareNoCase("select ") == 0 && nOption == openUnknown;

	try
	{
		m_pRecordSet->CursorType = adOpenStatic;
		m_pRecordSet->CursorLocation = adUseClient;
		if(bIsSelect || nOption == openQuery || nOption == openUnknown)
			m_pRecordSet->Open((LPCSTR)strQuery, _variant_t((IDispatch*)m_pConnection, TRUE), 
			adOpenStatic, adLockOptimistic, adCmdUnknown);
		else if(nOption == openTable)
			m_pRecordSet->Open((LPCSTR)strQuery, _variant_t((IDispatch*)m_pConnection, TRUE), 
			adOpenKeyset, adLockOptimistic, adCmdTable);
		else if(nOption == openStoredProc)
		{
			m_pCommand->ActiveConnection = m_pConnection;
			m_pCommand->CommandText = _bstr_t(strQuery);
			m_pCommand->CommandType = adCmdStoredProc;
			m_pConnection->CursorLocation = adUseClient;

			m_pRecordSet = m_pCommand->Execute(NULL, NULL, adCmdText);
		}
		else
		{
			TRACE( _T("Unknown parameter. %d", nOption);
			return FALSE;
		}

		m_nRecords = m_pRecordSet->GetRecordCount();

		if(m_nRecords == -1)
		{
			if(m_pRecordSet->adoEOF != VARIANT_TRUE)
			{
				m_pRecordSet->MoveFirst();
				m_nRecords = 0;
				while(m_pRecordSet->adoEOF != VARIANT_TRUE)
				{
					m_nRecords++;
					m_pRecordSet->MoveNext();
				}
				m_pRecordSet->MoveFirst();
			}
		}
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format ( _T("OpenQuery[LINE-%d] Error[%d]:%s\n") , __LINE__ , 
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
	ASSERT ( FALSE );
	return m_pRecordSet != NULL;
}
#endif

BOOL YAdoDatabase::OpenQuery ( CString strSql , CADORecordBinding* pRecordBinding )
{
	HRESULT hr;
	
    try
    {
        if ( m_pRecordSet->State != adStateClosed )		
		{
            hr = m_pRecordSet->Close();
		}

		// SQL IS NULL TABLE adCmdTable
		if ( strSql.IsEmpty() )
		{
			hr = m_pRecordSet->Open ( _variant_t ( m_strRecordSource ) , _variant_t((IDispatch *)m_pConnection) , 
										adOpenStatic , adLockOptimistic , adCmdTable );
		}
		else
		{
			
			// SQL IS NULL TABLE adCmdTable
			hr = m_pRecordSet->Open ( _variant_t ( strSql ) , _variant_t((IDispatch *)m_pConnection) , 
										adOpenStatic , adLockOptimistic , adCmdText );
		}

        if ( SUCCEEDED ( hr ) )
		{
			if ( pRecordBinding )
			{
				if ( pRecordBinding && m_pRecordBinding )
				{
					m_pRecordBinding->BindToRecordset ( pRecordBinding );
				}
			}
			m_nRecords = m_pRecordSet->GetRecordCount();
			
			if(m_nRecords == -1)
			{
				if(m_pRecordSet->adoEOF != VARIANT_TRUE)
				{
					m_pRecordSet->MoveFirst();
					m_nRecords = 0;
					while(m_pRecordSet->adoEOF != VARIANT_TRUE)
					{
						m_nRecords++;
						m_pRecordSet->MoveNext();
					}
					m_pRecordSet->MoveFirst();
				}
			}

			m_nFields = m_pRecordSet->Fields->Count;
			m_bRSOpen = TRUE;
			return TRUE;
		}
		else
		{
			m_nRecords = 0;							
			return FALSE;
		}

//		m_nRecords = m_pRecordSet->GetRecordCount();
    }
    catch ( _com_error &e )
    {
        CString strErr;
        SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
        strErr.Format ( _T("OpenQuery[LINE-%d] Error[%d]:%s\n") , __LINE__ , 
						e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);

#ifdef	__YLOGFILE_H__
 		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
    }
    catch (...)
    {
        TRACE ( _T("Table Open Exception...\n") );
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "Table Open Exception...\n") );
#endif
        return FALSE;
    }

	return TRUE;
}


BOOL YAdoDatabase::OpenQueryTimeOut(CString strSql,DWORD dwTimeOut, CADORecordBinding* pRecordBinding)
{
	HRESULT hr;

	try
	{
		if (m_pRecordSet->State != adStateClosed)
		{
			hr = m_pRecordSet->Close();
		}

		// SQL IS NULL TABLE adCmdTable
		if (strSql.IsEmpty())
		{
			hr = m_pRecordSet->Open(_variant_t(m_strRecordSource), _variant_t((IDispatch *)m_pConnection),
				adOpenStatic, adLockOptimistic, adCmdTable);
		}
		else
		{

			// SQL IS NULL TABLE adCmdTable
			hr = m_pRecordSet->Open(_variant_t(strSql), _variant_t((IDispatch *)m_pConnection),
				adOpenStatic, adLockOptimistic, adCmdText);
		}

		if (SUCCEEDED(hr))
		{
			if (pRecordBinding)
			{
				if (pRecordBinding && m_pRecordBinding)
				{
					m_pRecordBinding->BindToRecordset(pRecordBinding);
				}
			}
			m_nRecords = m_pRecordSet->GetRecordCount();

			if (m_nRecords == -1)
			{
				if (m_pRecordSet->adoEOF != VARIANT_TRUE)
				{
					m_pRecordSet->MoveFirst();
					m_nRecords = 0;
					while (m_pRecordSet->adoEOF != VARIANT_TRUE)
					{
						m_nRecords++;
						m_pRecordSet->MoveNext();
					}
					m_pRecordSet->MoveFirst();
				}
			}

			m_nFields = m_pRecordSet->Fields->Count;
			m_bRSOpen = TRUE;
			return TRUE;
		}
		else
		{
			m_nRecords = 0;
			return FALSE;
		}

		//		m_nRecords = m_pRecordSet->GetRecordCount();
	}
	catch (_com_error &e)
	{
		CString strErr;
		SetLastError((DWORD)e.Error());
		SetLastErrorString((LPCTSTR)e.Description());
		strErr.Format(_T("OpenQuery[LINE-%d] Error[%d]:%s\n"), __LINE__,
			e.Error(), (LPTSTR)(LPCTSTR)e.Description());

		TRACE(strErr);
		WriteAdoLog(strErr);

#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage(LOG_FILE_ERROR, "%s\r\n"), strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE(_T("Table Open Exception...\n"));
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage(LOG_FILE_ERROR, "Table Open Exception...\n") );
#endif
		return FALSE;
	}

	return TRUE;
}

BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, double& dbValue)
{	
	double val = (double)NULL;
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:
			val = vtFld.fltVal;
			break;
		case VT_R8:
			val = vtFld.dblVal;
			break;
		case VT_DECIMAL:
			//Corrected by Jos?Carlos Mart?ez Gal?
			val = vtFld.decVal.Lo32;
			val *= (vtFld.decVal.sign == 128)? -1 : 1;
			val /= pow(10.0, vtFld.decVal.scale); 
			break;
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_I2:
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.dblVal;
		}
		dbValue = val;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format ( _T("GetFieldValue[LINE-%d(%s)] Error[%d]:[%s] %s\n") , __LINE__, lpFieldName,
			e.Error() , lpFieldName,(LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif

		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "Table Open Exception...\n") );
#endif
		return FALSE;
	}
}


BOOL YAdoDatabase::GetFieldValue(int nIndex, double& dbValue)
{	
	double val = (double)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:
			val = vtFld.fltVal;
			break;
		case VT_R8:
			val = vtFld.dblVal;
			break;
		case VT_DECIMAL:
			//Corrected by Jos?Carlos Mart?ez Gal?
			val = vtFld.decVal.Lo32;
			val *= (vtFld.decVal.sign == 128)? -1 : 1;
			val /= pow((double)10, vtFld.decVal.scale); 
			break;
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_I2:
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = 0;
		}
		dbValue = val;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format ( _T("GetFieldValue(%d, double& dbValue)[LINE-%d] Error[%d]:%s\n"), nIndex, __LINE__ ,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
#ifdef	__YLOGFILE_H__
		TRACE ( _T("Table Open Exception...\n") );
#endif
		return FALSE;
	}
}


BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, long& lValue)
{
	long val = (long)NULL;
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.lVal;
		lValue = val;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format ( _T("GetFieldValue(LPCTSTR %s, long& lValue)[LINE-%d] Error[%d]:%s\n"), lpFieldName, __LINE__ ,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(int nIndex, long& lValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.lVal;
		lValue = val;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format ( _T("GetFieldValue(int %d, long& lValue)[LINE-%d] Error[%d]:%s\n"),nIndex , __LINE__ , 
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}


BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, unsigned long& ulValue)
{
	long val = (long)NULL;
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.ulVal;
		ulValue = val;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(LPCTSTR %s, unsigned long& ulValue)[LINE-%d] Error[%d]:%s\n"), lpFieldName, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(int nIndex, unsigned long& ulValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.ulVal;
		ulValue = val;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format ( _T("GetFieldValue(int %d, unsigned long& ulValue)[LINE-%d] Error[%d]:%s\n"),nIndex , __LINE__ , 
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}
BOOL YAdoDatabase::IsNullFieldValue(LPCTSTR lpFieldName)
{
	_variant_t vtFld;
	vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
	if(vtFld.vt == VT_NULL || vtFld.vt == VT_EMPTY)
		return TRUE;
	return FALSE;
}

BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, int& nValue)
{
	int val = NULL;
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_UI1:
			val = vtFld.bVal;
			break;
		case VT_UI2:
			val = vtFld.uiVal;
			break;
		case VT_UI4:
			val = vtFld.ulVal;
			break;
		case VT_I1:
			val = vtFld.cVal;
			break;
		case VT_I2:
			val = vtFld.iVal;
			break;
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_UINT:
			val = vtFld.uintVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.iVal;
		}	
		nValue = val;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(LPCTSTR %s, int& nValue)[LINE-%d] Error[%d]:%s\n"), lpFieldName, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}


BOOL YAdoDatabase::GetFieldValue(int nIndex, int& nValue)
{
	int val = (int)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_UI1:
			val = vtFld.bVal;
			break;
		case VT_UI2:
			val = vtFld.uiVal;
			break;
		case VT_UI4:
			val = vtFld.ulVal;
			break;
		case VT_I1:
			val = vtFld.cVal;
			break;
		case VT_I2:
			val = vtFld.iVal;
			break;
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_UINT:
			val = vtFld.uintVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.iVal;
		}	
		nValue = val;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(int %d, int& nValue)[LINE-%d] Error[%d]:%s\n"), nIndex, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, BYTE& btValue)
{
	BYTE val = NULL;
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		switch (vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_UI1:
			val = vtFld.bVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.iVal;
		}
		btValue = val;
		return TRUE;
	}
	catch (_com_error &e)
	{
		CString strErr;
		SetLastError((DWORD)e.Error());
		SetLastErrorString((LPCTSTR)e.Description());
		strErr.Format(_T("GetFieldValue(LPCTSTR %s, BYTE& btValue)[LINE-%d] Error[%d]:%s\n"), lpFieldName, __LINE__,
			e.Error(), (LPTSTR)(LPCTSTR)e.Description());

		TRACE(strErr);
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage(LOG_FILE_ERROR, "%s\r\n"), strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE(_T("Table Open Exception...\n"));
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(int nIndex, BYTE& btValue)
{
	BYTE val = 0;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		switch (vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_UI1:
			val = vtFld.bVal;
			break;
		case VT_I1:
			val = vtFld.cVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.iVal;
		}
		btValue = val;
		return TRUE;
	}
	catch (_com_error &e)
	{
		CString strErr;
		SetLastError((DWORD)e.Error());
		SetLastErrorString((LPCTSTR)e.Description());
		strErr.Format(_T("GetFieldValue(int %d, BYTE& btValue)[LINE-%d] Error[%d]:%s\n"), nIndex, __LINE__,
			e.Error(), (LPTSTR)(LPCTSTR)e.Description());

		TRACE(strErr);
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage(LOG_FILE_ERROR, "%s\r\n"), strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE(_T("Table Open Exception...\n"));
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, CString& strValue, CString strDateFormat)
{
	CString str = _T("");
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_R4:
			str = DblToStr(vtFld.fltVal);
			break;
		case VT_R8:
			str = DblToStr((float)vtFld.dblVal);
			break;
		case VT_BSTR:
			str = vtFld.bstrVal;
			break;
		case VT_I2:
		case VT_UI1:
			str = IntToStr(vtFld.iVal);
			break;
		case VT_INT:
			str = IntToStr(vtFld.intVal);
			break;
		case VT_I4:
			str = LongToStr(vtFld.lVal);
			break;
		case VT_UI4:
			str = ULongToStr(vtFld.ulVal);
			break;
		case VT_DECIMAL:
			{
				//Corrected by Jos?Carlos Mart?ez Gal?
				double val = vtFld.decVal.Lo32;
				val *= (vtFld.decVal.sign == 128)? -1 : 1;
				val /= pow((double)10.0, vtFld.decVal.scale); 
				str = DblToStr((float)val);
			}
			break;
		case VT_DATE:
			{
				COleDateTime dt(vtFld);

				if(strDateFormat.IsEmpty())
					strDateFormat = _T("%Y-%m-%d %H:%M:%S");
				str = dt.Format(strDateFormat);
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			str.Empty();
			break;
		case VT_BOOL:
			str = vtFld.boolVal == VARIANT_TRUE? 'T':'F';
			break;
		default:
			str.Empty();
			return FALSE;
		}
		strValue = str;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(LPCTSTR %s, CString& strValue, CString strDateFormat)[LINE-%d] Error[%X]:%s\n")
			, lpFieldName, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(int nIndex, CString& strValue, CString strDateFormat)
{
	CString str = _T("");
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_R4:
			str = DblToStr(vtFld.fltVal);
			break;
		case VT_R8:
			str = DblToStr((float)vtFld.dblVal);
			break;
		case VT_BSTR:
			str = vtFld.bstrVal;
			break;
		case VT_I2:
		case VT_UI1:
			str = IntToStr(vtFld.iVal);
			break;
		case VT_INT:
			str = IntToStr(vtFld.intVal);
			break;
		case VT_I4:
			str = LongToStr(vtFld.lVal);
			break;
		case VT_UI4:
			str = ULongToStr(vtFld.ulVal);
			break;
		case VT_DECIMAL:
			{
				//Corrected by Jos?Carlos Mart?ez Gal?
				double val = vtFld.decVal.Lo32;
				val *= (vtFld.decVal.sign == 128)? -1 : 1;
				val /= pow((double)10, vtFld.decVal.scale); 
				str = DblToStr((float)val);
			}
			break;
		case VT_DATE:
			{
				COleDateTime dt(vtFld);

				if(strDateFormat.IsEmpty())
					strDateFormat = _T("%Y-%m-%d %H:%M:%S");
				str = dt.Format(strDateFormat);
			}
			break;
		case VT_BOOL:
			str = vtFld.boolVal == VARIANT_TRUE? 'T':'F';
			break;
		case VT_EMPTY:
		case VT_NULL:
			str.Empty();
			break;
		default:
			str.Empty();
			return FALSE;
		}
		strValue = str;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(int %d, CString& strValue, CString strDateFormat)[LINE-%d] Error[%d]:%s\n")
			, nIndex, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, COleDateTime& time)
{
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_DATE:
			{
				COleDateTime dt(vtFld);
				time = dt;
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			time.SetStatus(COleDateTime::null);
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format ( _T("GetFieldValue(LPCTSTR %s, COleDateTime& time)[LINE-%d] Error[%d]:%s\n")
			, lpFieldName, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(int nIndex, COleDateTime& time)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_DATE:
			{
				COleDateTime dt(vtFld);
				time = dt;
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			time.SetStatus(COleDateTime::null);
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(int %d, COleDateTime& time)[LINE-%d] Error[%d]:%s\n")
			, nIndex, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, bool& bValue)
{
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_BOOL:
			bValue = vtFld.boolVal == VARIANT_TRUE? true: false;
			break;
		case VT_EMPTY:
		case VT_NULL:
			bValue = false;
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(LPCTSTR %s, bool& bValue)[LINE-%d] Error[%d]:%s\n")
			, lpFieldName, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(int nIndex, bool& bValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_BOOL:
			bValue = vtFld.boolVal == VARIANT_TRUE? true: false;
			break;
		case VT_EMPTY:
		case VT_NULL:
			bValue = false;
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(int %d, bool& bValue)[LINE-%d] Error[%d]:%s\n")
			, nIndex, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, COleCurrency& cyValue)
{
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_CY:
			cyValue = (CURRENCY)vtFld.cyVal;
			break;
		case VT_EMPTY:
		case VT_NULL:
			{
				cyValue = COleCurrency();
				cyValue.m_status = COleCurrency::null;
			}
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(LPCTSTR %s, COleCurrency& cyValue)[LINE-%d] Error[%d]:%s\n")
			, lpFieldName, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(int nIndex, COleCurrency& cyValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_CY:
			cyValue = (CURRENCY)vtFld.cyVal;
			break;
		case VT_EMPTY:
		case VT_NULL:
			{
				cyValue = COleCurrency();
				cyValue.m_status = COleCurrency::null;
			}
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(int %d, COleCurrency& cyValue)[LINE-%d] Error[%d]:%s\n")
			, nIndex, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(LPCTSTR lpFieldName, _variant_t& vtValue)
{
	try
	{
		vtValue = m_pRecordSet->Fields->GetItem(lpFieldName)->Value;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(LPCTSTR %s, _variant_t& vtValue)[LINE-%d] Error[%d]:%s\n")
			, lpFieldName, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}

BOOL YAdoDatabase::GetFieldValue(int nIndex, _variant_t& vtValue)
{
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtValue = m_pRecordSet->Fields->GetItem(vtIndex)->Value;
		return TRUE;
	}
	catch ( _com_error &e )
	{
		CString strErr;
		SetLastError ( (DWORD)e.Error() );
		SetLastErrorString ( (LPCTSTR)e.Description() );
		strErr.Format(_T("GetFieldValue(int %d, _variant_t& vtValue)[LINE-%d] Error[%d]:%s\n")
			, nIndex, __LINE__,
			e.Error() , (LPTSTR)(LPCTSTR)e.Description() );

		TRACE ( strErr );
		WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
		g_LogFile.TimeMessage ( LOG_FILE_ERROR , "%s\r\n") , strErr );
#endif
		return FALSE;
	}
	catch (...)
	{
		TRACE ( _T("Table Open Exception...\n") );
		return FALSE;
	}
}


BOOL YAdoDatabase::ImageToVariant ( BYTE* pImage , int nImageLength , VARIANT &varArray )
{
	BYTE *pByte;
	BOOL bRet = FALSE;
	try
	{
			SAFEARRAY FAR* psa;
			SAFEARRAYBOUND rgsabound[1];
			rgsabound[0].lLbound = 0;	
			rgsabound[0].cElements = nImageLength;

			// create a single dimensional byte array
			psa = SafeArrayCreate ( VT_I1 , 1 , rgsabound );
			
			// set the data of the array with data in the edit box
			if ( SafeArrayAccessData ( psa , (void **)&pByte ) == NOERROR )
			{
				memcpy ( (LPVOID)pByte , (LPVOID)pImage , nImageLength );
				bRet = TRUE;
			}
			SafeArrayUnaccessData ( psa );

			varArray.vt = VT_ARRAY | VT_UI1;
			varArray.parray = psa;
	}
	catch (_com_error &e)
	{
		DumpError(e);
	}
	return bRet;
}

void YAdoDatabase::DumpError ( _com_error& e )
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	CString str;
	str.Format(_T("\tCode = %08lx"), e.Error());
	str += _T(" Msg: ");	str += e.ErrorMessage();
	str += _T(" Source: "); str += (char *)bstrSource;
	str += _T(" Description: "); str += (char *)bstrDescription;
	AfxMessageBox(str);
	
}

BOOL YAdoDatabase::UpdateChunk ( BYTE* pImage , int nImageLength , long nFieldIndex )
{
	if ( m_pRecordSet == NULL )
	{
		TRACE ( _T("ERROR : m_pRecordSet is a Null Pointer\n") );
		return FALSE;
	}

	VARIANT varImage;

	BOOL bRet = FALSE;
	try
	{
		// call this function to get the blob data as a variant which is present in one of the controls
		ImageToVariant ( pImage , nImageLength , varImage );
		// set the value of the field
		m_pRecordSet->Fields->Item[nFieldIndex]->AppendChunk ( varImage );
		m_pRecordSet->Update();
		
		VariantClear ( &varImage );
		bRet = TRUE;
	}
	catch ( _com_error e )
	{
		DumpError ( e );
	}			
	return bRet;
}

BOOL YAdoDatabase::UpdateFile ( CString strFileName , long nFieldIndex )
{
	CFile file;
	CFileException ex;
	if ( file.Open ( strFileName , CFile::modeRead , &ex ) == FALSE )
	{
		// complain if an error happened
		// no need to delete the ex object

		TCHAR szError[1024];
		ex.GetErrorMessage ( szError , 1024 );
		TRACE ( _T("Couldn't open source file: %s\n") , szError );

		return FALSE;
	}

	long lDataLength = (long)file.GetLength();

	if ( lDataLength <= 0 )
	{
		return FALSE;
	}
	BYTE* pBuff = new BYTE [ lDataLength + 1 ];
	memset ( pBuff , 0 , lDataLength + 1 );
	file.Read ( pBuff , lDataLength );
	file.Close();

	BOOL bRet = UpdateChunk ( pBuff , lDataLength , nFieldIndex );

	delete [] pBuff;	

	return bRet;
}


BOOL YAdoDatabase::GetChunk ( BYTE** pBuffer , long& nBufferLength , long nFieldIndex )
{
	if ( m_pRecordSet == NULL )
	{
		TRACE ( _T("ERROR : m_pRecordSet is a Null Pointer\n") );
		return FALSE;
	}

	BOOL bRet = TRUE;
	_variant_t varImage;
	long lDataLength = 0;
	BYTE* pBuf = NULL;
	int nDataLenRetrieved = 0;
	
	YDynamicChunk	DynamicChunk;

	try
	{
		// get the actual length of the data
		lDataLength = m_pRecordSet->Fields->Item[nFieldIndex]->ActualSize;
		if ( lDataLength > 0 )
		{
			VariantInit ( &varImage );
			// get the chunk of data 
			varImage = m_pRecordSet->Fields->Item[nFieldIndex]->GetChunk ( lDataLength );
			
			// If the data retrieved is array of bytes then get data from the array and set the 
			// data value variable of the edit box 
			if ( varImage.vt == ( VT_ARRAY | VT_UI1 ) )
			{
				SafeArrayAccessData ( varImage.parray , (void **)&pBuf );

				*pBuffer = new BYTE [ lDataLength + 1 ];

				memset ( *pBuffer , 0x0 , lDataLength + 1 );
				memcpy ( *pBuffer , pBuf , lDataLength );

				SafeArrayUnaccessData ( varImage.parray );
			
				nBufferLength = lDataLength;
			}

		}
		// some of the providers return -1 as the data length 
		// in that case loop through the get the data in chunks
		else if ( lDataLength  < 0 )
		{
			do
			{
				VariantInit ( &varImage );
				varImage = m_pRecordSet->Fields->Item[nFieldIndex]->GetChunk ( (long)YADO_CHUNKSIZE );
				
				// If the data retrieved is array of bytes then get data from the array and set the 
				// data value variable of the edit box 
				if ( varImage.vt == (VT_ARRAY | VT_UI1) )
				{
					SafeArrayAccessData ( varImage.parray , (void **)&pBuf );
					// get the length of chunk retrieved
					nDataLenRetrieved = varImage.parray->rgsabound[0].cElements;

					DynamicChunk.AddChunk ( pBuf , nDataLenRetrieved );
					SafeArrayUnaccessData(varImage.parray);
				}
			}
			while ( nDataLenRetrieved == YADO_CHUNKSIZE );  // loop through to get the in chunks of data of CHUNKSIZE size
			
			nBufferLength = DynamicChunk.GetDataLength();
			*pBuffer = new BYTE [ nBufferLength + 1 ];
			memset ( *pBuffer , 0x0 , nBufferLength + 1 );		
			memcpy ( *pBuffer , DynamicChunk.GetBuffer() , nBufferLength );
		}

		// update the controls witht the data in the member variables
	}
	catch(_com_error e)
	{
		DumpError ( e );
		bRet = FALSE;
	}
	return bRet;
}


BOOL YAdoDatabase::GetChunk ( BYTE** pBuffer , long& nBufferLength , long& nActualLength , long nFieldIndex )
{
//	ASSERT ( *pBuffer );
	if ( m_pRecordSet == NULL )
	{
		TRACE ( _T("ERROR : m_pRecordSet is a Null Pointer\n") );
		return FALSE;
	}

	BOOL bRet = TRUE;
	_variant_t varImage;
	long lDataLength = 0;
	BYTE* pBuf = NULL;
	int nDataLenRetrieved = 0;
	
	YDynamicChunk	DynamicChunk;

	try
	{
		// get the actual length of the data
		lDataLength = m_pRecordSet->Fields->Item[nFieldIndex]->ActualSize;
		if ( lDataLength > 0 )
		{
			VariantInit ( &varImage );
			// get the chunk of data 
			varImage = m_pRecordSet->Fields->Item[nFieldIndex]->GetChunk ( lDataLength );
			
			// If the data retrieved is array of bytes then get data from the array and set the 
			// data value variable of the edit box 
			if ( varImage.vt == ( VT_ARRAY | VT_UI1 ) )
			{
				SafeArrayAccessData ( varImage.parray , (void **)&pBuf );
				
				nActualLength = lDataLength;
				if ( nBufferLength < lDataLength + 1 )
				{
					nBufferLength = lDataLength + 1;

					delete [] *pBuffer;
					*pBuffer = new BYTE [ lDataLength + 1 ];
				}

				memset ( *pBuffer , 0x0 , lDataLength + 1 );
				memcpy ( *pBuffer , pBuf , lDataLength );

				SafeArrayUnaccessData ( varImage.parray );
			
			}

		}
		// some of the providers return -1 as the data length 
		// in that case loop through the get the data in chunks
		else if ( lDataLength  < 0 )
		{
			do
			{
				VariantInit ( &varImage );
				varImage = m_pRecordSet->Fields->Item[nFieldIndex]->GetChunk ( (long)YADO_CHUNKSIZE );
				
				// If the data retrieved is array of bytes then get data from the array and set the 
				// data value variable of the edit box 
				if ( varImage.vt == (VT_ARRAY | VT_UI1) )
				{
					SafeArrayAccessData ( varImage.parray , (void **)&pBuf );
					// get the length of chunk retrieved
					nDataLenRetrieved = varImage.parray->rgsabound[0].cElements;

					DynamicChunk.AddChunk ( pBuf , nDataLenRetrieved );
					SafeArrayUnaccessData(varImage.parray);
				}
			}
			while ( nDataLenRetrieved == YADO_CHUNKSIZE );  // loop through to get the in chunks of data of CHUNKSIZE size

				
			nActualLength = DynamicChunk.GetDataLength();
			if ( nBufferLength < nActualLength )
			{
				nBufferLength = nActualLength;

				delete [] *pBuffer;
				*pBuffer = new BYTE [ nActualLength ];
			}
			
			memcpy ( *pBuffer , DynamicChunk.GetBuffer() , nBufferLength );
		}

		// update the controls witht the data in the member variables
	}
	catch(_com_error e)
	{
		DumpError ( e );
		bRet = FALSE;
	}
	return bRet;
}

BOOL YAdoDatabase::OpenTable ( CString strTable , CADORecordBinding* pRecordBinding ) 
{
    try 
    {
		HRESULT hr = m_pRecordSet->Open ( (LPCTSTR)strTable , _variant_t((IDispatch*)m_pConnection , TRUE ) , adOpenKeyset , adLockOptimistic , adCmdTable );
		hr = m_pRecordBinding->BindToRecordset ( pRecordBinding );
	}
    catch ( _com_error &e )
    {
        // Notify the user of errors if any.
        // Pass a connection pointer accessed from the Connection.
        PrintProviderError ( m_pConnection );
        PrintComError ( e );
		return FALSE;
    }
	return TRUE;
}

BOOL YAdoDatabase::AddNew ( CADORecordBinding* pRecordBinding ) 
{ 
	if ( NULL == m_pRecordBinding )
	{
		return FALSE;
	}

	HRESULT hr;	
/*
	if (GetRecordCount() == 0) 
	{
//		ClearBind();
	}
*/
    try 
    {
		hr = m_pRecordBinding->AddNew ( pRecordBinding ); 
		if ( hr == S_OK ) 
		{
			TRACE ( _T("AddNew Success\n") );
		}
		else
		{
			return FALSE;
		}
	}
    catch ( _com_error &e )
    {
        // Notify the user of errors if any.
        // Pass a connection pointer accessed from the Connection.
        PrintProviderError ( m_pConnection );
        PrintComError ( e );
		return FALSE;
    }

	return TRUE;
}

BOOL YAdoDatabase::Update ( CADORecordBinding* pRecordBinding ) 
{ 
	if ( NULL == m_pRecordBinding )
	{
		return S_FALSE;
	}
	HRESULT hr;	
    try 
    {
		hr = m_pRecordBinding->Update ( pRecordBinding ); 
		if ( hr == S_OK )
		{
			TRACE ( _T("AddNew Success\n") );
		}
		else
		{
//            _com_issue_error ( hr );www
		}
	}
    catch ( _com_error &e )
    {
        // Notify the user of errors if any.
        // Pass a connection pointer accessed from the Connection.
        PrintProviderError ( m_pConnection );
        PrintComError ( e );
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////
//                                                      //
//       PrintProviderError Function                    //
//                                                      //
//////////////////////////////////////////////////////////
void YAdoDatabase::PrintProviderError ( _ConnectionPtr pConnection )
{
    // Print Provider Errors from Connection object.
    // pErr is a record object in the Connection's Error collection.
    ErrorPtr    pErr  = NULL;

    if( (pConnection->Errors->Count) > 0)
    {
        long nCount = pConnection->Errors->Count;
        // Collection ranges from 0 to nCount -1.
        for(long i = 0; i < nCount; i++)
        {
            pErr = pConnection->Errors->GetItem(i);
            TRACE("Error number: %x\t%s\n", pErr->Number, 
                (LPCSTR) pErr->Description);
        }
    }
}

void YAdoDatabase::PrintComError ( _com_error &e )
{

    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());
        
    // Print Com errors.  
    TRACE("Error\n");
    TRACE("\tCode = %08lx\n", e.Error());
    TRACE("\tCode meaning = %s\n", e.ErrorMessage());
    TRACE("\tSource = %s\n", (LPCSTR) bstrSource);
    TRACE("\tDescription = %s\n", (LPCSTR) bstrDescription);

	//WriteAdoLog(strErr);
#ifdef	__YLOGFILE_H__
	g_LogFile.TimeMessage ( LOG_FILE_ERROR , "Com Error Code meaning = %s\n", e.ErrorMessage() );
#endif
}


BOOL YAdoDatabase::BeginTransaction()
{
	if ( m_pConnection )
	{
		m_pConnection->BeginTrans();
		return TRUE;
	}
	return FALSE;
}

BOOL YAdoDatabase::CommitTransaction()
{
	if ( m_pConnection )
	{
		m_pConnection->CommitTrans();
		return TRUE;
	}
	return FALSE;
}

BOOL YAdoDatabase::RollbackTransaction()
{
	if ( m_pConnection )
	{
		m_pConnection->RollbackTrans();
		return TRUE;
	}
	return FALSE;
}

BOOL YAdoDatabase::ReduceDatabase(CString strDBName)
{
	CString strSql;
	strSql.Format(L"DBCC SHRINKDATABASE (%s, TRUNCATEONLY)" , strDBName);
	return ExecuteSql(strSql);;
}
