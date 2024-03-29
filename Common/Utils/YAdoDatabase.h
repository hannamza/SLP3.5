// YAdoDatabase.h: interface for the YAdoDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YADODATABASE_H__639A8D2A_A57D_4650_AB38_C18BF2E39542__INCLUDED_)
#define AFX_YADODATABASE_H__639A8D2A_A57D_4650_AB38_C18BF2E39542__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>
#include <afxole.h>         // MFC OLE classes
#include <afxdb.h>			// MFC ODBC database classes


#define S_DB_MDB_PROVIDER   _T("Provider=Microsoft.Jet.OLEDB.4.0;")
#define S_DB_SQL_PROVIDER   _T("Provider=SQLOLEDB;")
#define S_DB_ACCDB_PROVIDER   _T("Provider=Microsoft.ACE.OLEDB.12.0;")

#define S_DB_PROVIDER       _T("Provider=")
#define S_DB_DATASOURCE     _T("Data Source=")
#define S_DB_DATABASE_NAME  _T("Database=")

#define S_DB_ODBC_DSN		_T("DSN=")
#define	S_DB_ODBC_ID		_T("UID=")
#define	S_DB_ODBC_PASSWD	_T("PWD=")

#define S_DB_USER_ID        _T("User Id=sa;")
#define S_DB_USER_PASSWD    _T("Password=sa;")

#define Y_DB_ID				_T("sa")
#define Y_DB_PASSWORD		_T("sa_sa")
#define Y_DB_SOURCE			_T("127.0.0.1")
#define Y_DB_NAME			_T("IBSServer")


#pragma warning (disable: 4146)
/*
// ADO library needs to be used in the #import statement
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename_namespace("ADOCG") rename("EOF", "EndOfFile")
//using namespace ADOCG;
#import "C:\Program Files\Common Files\System\ole db\Oledb32.dll" no_namespace

inline void TESTHR(HRESULT x) {if FAILED(x) _com_issue_error(x);};
*/
// CG : In order to use this code against a different version of ADO, the appropriate
// ADO library needs to be used in the #import statement
//#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename("EOF", "adoEOF")
//#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename("EOF", "adoEOF")
//using namespace ADOCG;
//#import "C:\Program Files\Common Files\System\ole db\Oledb32.dll" no_namespace
//#import "C:\Program Files\Common Files\System\ado\MSJRO.DLL" no_namespace

// 만약 개발 PC가 64비트면 아래와 같이
// #import "C:\Program Files (x86)\Common Files\System\ADO\msado15.dll" rename("EOF", "adoEOF")
// #import "c:\Program Files (x86)\Common Files\system\ado\msadox.dll"
// 32비트라면 아래와 같이 
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename("EOF", "adoEOF")
#import "c:\Program Files\Common Files\system\ado\msadox.dll"
using namespace ADOX;
using namespace ADODB;

#pragma warning (default: 4146)
#include "icrsint.h"

struct CADOFieldInfo
{
	char m_strName[30]; 
	short m_nType;
	long m_lSize; 
	long m_lDefinedSize;
	long m_lAttributes;
	short m_nOrdinalPosition;
	BOOL m_bRequired;   
	BOOL m_bAllowZeroLength; 
	long m_lCollatingOrder;  
};


typedef enum EN_YADO_DATABASE
{
    enMDB = 0,
    enSQL = 1,
	enODBC =2,
	enACCDB=3,
	enMySQL=4,

} EN_YADO_DATABASE;


enum cadoOpenEnum
{
	openUnknown = 0,
	openQuery = 1,
	openTable = 2,
	openStoredProc = 3
};
/*
enum cadoEditEnum
{
	dbEditNone = 0,
	dbEditNew = 1,
	dbEdit = 2
};
*/
enum cadoPositionEnum
{

	positionUnknown = -1,
	positionBOF = -2,
	positionEOF = -3
};

enum cadoSearchEnum
{	
	searchForward = 1,
	searchBackward = -1
};

enum cadoDataType
{
	typeEmpty = ADODB::adEmpty,
	typeTinyInt = ADODB::adTinyInt,
	typeSmallInt = ADODB::adSmallInt,
	typeInteger = ADODB::adInteger,
	typeBigInt = ADODB::adBigInt,
	typeUnsignedTinyInt = ADODB::adUnsignedTinyInt,
	typeUnsignedSmallInt = ADODB::adUnsignedSmallInt,
	typeUnsignedInt = ADODB::adUnsignedInt,
	typeUnsignedBigInt = ADODB::adUnsignedBigInt,
	typeSingle = ADODB::adSingle,
	typeDouble = ADODB::adDouble,
	typeCurrency = ADODB::adCurrency,
	typeDecimal = ADODB::adDecimal,
	typeNumeric = ADODB::adNumeric,
	typeBoolean = ADODB::adBoolean,
	typeError = ADODB::adError,
	typeUserDefined = ADODB::adUserDefined,
	typeVariant = ADODB::adVariant,
	typeIDispatch = ADODB::adIDispatch,
	typeIUnknown = ADODB::adIUnknown,
	typeGUID = ADODB::adGUID,
	typeDate = ADODB::adDate,
	typeDBDate = ADODB::adDBDate,
	typeDBTime = ADODB::adDBTime,
	typeDBTimeStamp = ADODB::adDBTimeStamp,
	typeBSTR = ADODB::adBSTR,
	typeChar = ADODB::adChar,
	typeVarChar = ADODB::adVarChar,
	typeLongVarChar = ADODB::adLongVarChar,
	typeWChar = ADODB::adWChar,
	typeVarWChar = ADODB::adVarWChar,
	typeLongVarWChar = ADODB::adLongVarWChar,
	typeBinary = ADODB::adBinary,
	typeVarBinary = ADODB::adVarBinary,
	typeLongVarBinary = ADODB::adLongVarBinary,
	typeChapter = ADODB::adChapter,
	typeFileTime = ADODB::adFileTime,
	typePropVariant = ADODB::adPropVariant,
	typeVarNumeric = ADODB::adVarNumeric,
	typeArray = ADODB::adVariant
};

enum cadoSchemaType 
{
	schemaSpecific = adSchemaProviderSpecific,	
	schemaAsserts = adSchemaAsserts,
	schemaCatalog = adSchemaCatalogs,
	schemaCharacterSet = adSchemaCharacterSets,
	schemaCollections = adSchemaCollations,
	schemaColumns = adSchemaColumns,
	schemaConstraints = adSchemaCheckConstraints,
	schemaConstraintColumnUsage = adSchemaConstraintColumnUsage,
	schemaConstraintTableUsage  = adSchemaConstraintTableUsage,
	shemaKeyColumnUsage = adSchemaKeyColumnUsage,
	schemaTableConstraints = adSchemaTableConstraints,
	schemaColumnsDomainUsage = adSchemaColumnsDomainUsage,
	schemaIndexes = adSchemaIndexes,
	schemaColumnPrivileges = adSchemaColumnPrivileges,
	schemaTablePrivileges = adSchemaTablePrivileges,
	schemaUsagePrivileges = adSchemaUsagePrivileges,
	schemaProcedures = adSchemaProcedures,
	schemaTables =adSchemaTables,
	schemaProviderTypes = adSchemaProviderTypes,
	schemaViews = adSchemaViews,
	schemaProcedureParameters = adSchemaProcedureParameters,
	schemaForeignKeys = adSchemaForeignKeys,
	schemaPrimaryKeys = adSchemaPrimaryKeys,
	schemaProcedureColumns = adSchemaProcedureColumns,
	schemaDBInfoKeywords = adSchemaDBInfoKeywords,
	schemaDBInfoLiterals = adSchemaDBInfoLiterals,
	schemaCubes = adSchemaCubes,
	schemaDimensions = adSchemaDimensions,
	schemaHierarchies  = adSchemaHierarchies, 
	schemaLevels = adSchemaLevels,
	schemaMeasures = adSchemaMeasures,
	schemaProperties = adSchemaProperties,
	schemaMembers = adSchemaMembers,
}; 



#define YDC_CHUNK_SIZE		2048

class YDynamicChunk
{
public:
	YDynamicChunk()
	{
		m_pBuffer = NULL;
		m_nBufferSize = 0;
		m_nUsedSize = 0;
	}

	~YDynamicChunk()
	{
		if ( m_pBuffer )
		{
			delete [] m_pBuffer;
			m_pBuffer = NULL;
		}
	}

	BYTE* GetBuffer()
	{
		return m_pBuffer;
	}

	long GetDataLength()
	{
		return m_nUsedSize;
	}

	void AddChunk ( BYTE* pBuffer , int nBufferSize )
	{
		if ( m_pBuffer == NULL )
		{
			if ( YDC_CHUNK_SIZE < nBufferSize )
			{
				m_nBufferSize = nBufferSize + 4;
			}
			else
			{
				m_nBufferSize = YDC_CHUNK_SIZE;
			}
			m_pBuffer = new BYTE [ m_nBufferSize ];
//			memset ( m_pBuffer , 0x0 , m_nBufferSize );
			memcpy ( m_pBuffer , pBuffer , nBufferSize );
		
			m_nUsedSize += nBufferSize;
			return;
		}

		if ( (m_nBufferSize - m_nUsedSize) < nBufferSize )
		{
			m_nBufferSize += nBufferSize + YDC_CHUNK_SIZE;
			BYTE* pTempBuffer = new BYTE [ m_nBufferSize ];

			memset ( pTempBuffer , 0x0 , m_nBufferSize );

			memcpy ( pTempBuffer , m_pBuffer , m_nBufferSize );
			memcpy ( pTempBuffer + m_nUsedSize , pBuffer , nBufferSize );
			delete [] m_pBuffer;

			m_pBuffer = pTempBuffer;

		}
		
	}

public:
	BYTE*	m_pBuffer;
	long	m_nBufferSize;
	long	m_nUsedSize;
};

class YAdoDatabase : public CObject
{
 //   DECLARE_DYNAMIC(YAdoDatabase)

public:
	YAdoDatabase();
	virtual ~YAdoDatabase();

	void		ODBCInitialize ( CString strDsn , CString strDB , CString strId = _T("") , CString strPassword = _T("") );
	void		MSSqlInitialize(LPCTSTR lpszUserPswd, LPCTSTR lpszUserName, LPCTSTR lpszDbName, LPCTSTR lpszDbServer,UINT nPort = 1433);
	void		MySqlInitialize(LPCTSTR lpszUserPswd, LPCTSTR lpszUserName, LPCTSTR lpszDbName, LPCTSTR lpszDbServer , UINT nPort = 3306);
	void		MdbInitialize ( LPCTSTR lpszFileName , LPCTSTR lpszPassword = NULL );
	void		MdbAccdbInitialize(LPCTSTR lpszFileName, LPCTSTR lpszPassword = NULL);
    BOOL		IsOpen() { return m_bOpen; }
	BOOL		IsRSOpen() { return m_bRSOpen; }
    void		SetConnectString ( CString &strConnect ) { m_strConnectString = strConnect; }
    void		SetConnectString ( LPCTSTR lpszConnect ) { m_strConnectString = lpszConnect; }
    void		SetLastErrorString ( LPCTSTR lpszErrorString ) { m_strErrorString = lpszErrorString; }
    void		SetLastError ( DWORD dwError ) { m_dwLastError = dwError; }

	BOOL		ExecuteSql ( CString &strSql, BOOL bCheckCnt = FALSE);
	BOOL		ExecuteSql ( LPCTSTR lpszSql, BOOL bCheckCnt = FALSE);
	BOOL		ExecuteSqlTimeOut(CString &strSql, DWORD dwTimeOut, BOOL bCheckCnt = FALSE);
	BOOL		ExecuteSqlTimeOut(LPCTSTR lpszSql, DWORD dwTimeOut, BOOL bCheckCnt = FALSE);
	void		CloseConnection();
	void		DBClose();
	BOOL		DBOpen();
	BOOL		OpenConnection();
	void		MakeConnectString();
    void		SetDatabaseName ( LPCTSTR lpszDatabaseName );
    void		SetDatabaseType ( EN_YADO_DATABASE enDatabase = enMDB );
    void		SetProvider ( LPCTSTR lpszProvider );
    void		SetDataSource ( LPCTSTR lpszDataSource );

	BOOL		BeginTransaction();
	BOOL		CommitTransaction();
	BOOL		RollbackTransaction();


    LPCTSTR				GetDatabaseName()	{	return m_strDatabaseName;		}
    EN_YADO_DATABASE	GetDatabaseType()	{	return m_enDatabase;			}
    LPCTSTR				GetConnectString()	{	return m_strConnectString;		}
    LPCTSTR				GetProvider()		{	return m_strProvider;			}
    LPCTSTR				GetDataSource()		{	return m_strDataSource;			}
    DWORD				GetLastError()		{	return m_dwLastError;			}
    LPCTSTR				GetLastErrorString(){	return (LPCTSTR)m_strErrorString; }

	BOOL				OpenQuery(CString strSql, CADORecordBinding* pRecordBinding = NULL);
	BOOL				OpenQueryTimeOut(CString strSql , DWORD dwTimeOut, CADORecordBinding* pRecordBinding = NULL);
//	BOOL				OpenQuery( LPCTSTR lpstrExec, int nOption );

	BOOL				ImageToVariant ( BYTE* pImage , int nImageLength , VARIANT &varArray );
	void				DumpError ( _com_error& e );

	BOOL				UpdateChunk ( BYTE* pImage , int nImageLength , long nFieldIndex );
	BOOL				UpdateFile ( CString strFileName , long nFieldIndex );

	BOOL				GetChunk ( BYTE** pBuffer , long& nBufferLength , long nFieldIndex );
	BOOL				GetChunk ( BYTE** pBuffer , long& nBufferLength , long& nActualLength , long nFieldIndex );

	BOOL				AddNew ( CADORecordBinding* pRecordBinding );
//	BOOL				AddNew ( CADORecordBinding* pRecordBinding , CString strTable );

	BOOL				Update ( CADORecordBinding* pRecordBinding );

	void				PrintProviderError ( _ConnectionPtr pConnection );
	void				PrintComError ( _com_error &e );

	BOOL				OpenTableInfo();

	BOOL				OpenTable ( CString strTable , CADORecordBinding* pRecordBinding );

	// [KHS 2020-11-19 14:38:34] 
	// Database 용량 줄이기
	BOOL				ReduceDatabase(CString strDBName);
	// [KHS 2018-7-18 10:30:21] 
	// Table Exist Check
	BOOL				IsExistTable(CString strTable);
	HRESULT RSClose()
	{    
		m_bRSOpen = FALSE;
		try
		{
			if ( m_pRecordSet->State != adStateClosed )		
			{
				return m_pRecordSet->Close();
			}
		}
		catch ( _com_error &e )
		{
			CString strErr;
			SetLastError ( (DWORD)e.Error() );
			SetLastErrorString ( (LPCTSTR)e.Description() );
			strErr.Format ( _T("RSClose[LINE-%d] Error[%d]:%s\n") , __LINE__ , 
				e.Error() , (char *)(LPCTSTR)e.Description() );

			TRACE ( strErr );
			return E_FAIL;
		}
		catch (...)
		{
			TRACE ( _T("Table Open Exception...\n") );
			return E_FAIL;
		}
		return E_FAIL;
	}

	HRESULT MoveFirst() 
	{ 
		if ( !m_pRecordSet->GetBOF() ) 
		{
			return m_pRecordSet->MoveFirst();
		}
		else 
		{
			return E_FAIL; 
		}
	};

	HRESULT MovePrevious() 
	{ 
		if ( !m_pRecordSet->GetBOF() ) 
		{
			return m_pRecordSet->MovePrevious();
		}
		else 
		{
			return E_FAIL;
		}
	};

	HRESULT MoveNext() 
	{ 
		if ( !m_pRecordSet->GetadoEOF() ) 
		{
			return m_pRecordSet->MoveNext();
		}
		else 
		{
			return E_FAIL; 
		}
	};

	HRESULT MoveLast() 
	{ 
		if ( !m_pRecordSet->GetadoEOF() ) 
		{
			return m_pRecordSet->MoveLast();
		}
		else 
		{
			return E_FAIL; 
		}
	};

	inline BSTR ConvertStringToBSTR(const char* pSrc)
	{
		if(!pSrc) return NULL;

		DWORD cwch;

		BSTR wsOut(NULL);

		if(cwch = ::MultiByteToWideChar(CP_ACP, 0, pSrc, 
			 -1, NULL, 0))//get size minus NULL terminator
		{
					cwch--;
				wsOut = ::SysAllocStringLen(NULL, cwch);

			if(wsOut)
			{
				if(!::MultiByteToWideChar(CP_ACP, 
						 0, pSrc, -1, wsOut, cwch))
				{
					if(ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
						return wsOut;
					::SysFreeString(wsOut);//must clean up
					wsOut = NULL;
				}
			}

		};

		return wsOut;
	};


	int	GetRecordCount()
	{
		return m_nRecords;
	}

	BOOL IsEof()
	{return m_pRecordSet->adoEOF == VARIANT_TRUE;};

	long GetFieldCount()
	{return m_pRecordSet->Fields->GetCount();};

	BOOL GetFieldInfo(int nIndex, CADOFieldInfo* fldInfo)
	{
		_variant_t vtIndex;

		vtIndex.vt = VT_I2;
		vtIndex.iVal = nIndex;

		FieldPtr pField = m_pRecordSet->Fields->GetItem(vtIndex);

		return GetFieldInfo(pField, fldInfo);
	}

	BOOL GetFieldInfo(FieldPtr pField, CADOFieldInfo* fldInfo)
	{
		memset(fldInfo, 0, sizeof(CADOFieldInfo));

//		strcpy_s(fldInfo->m_strName, (LPCTSTR)pField->GetName());
		strcpy_s (fldInfo->m_strName , sizeof ( fldInfo->m_strName )/sizeof(TCHAR) - 1 , pField->GetName ());
		fldInfo->m_lDefinedSize = pField->GetDefinedSize();
		fldInfo->m_nType = pField->GetType();
		fldInfo->m_lAttributes = pField->GetAttributes();
		if(!IsEof())
			fldInfo->m_lSize = pField->GetActualSize();
		return TRUE;
	}
	CString BSTRtoCString ( BSTR bstrTemp );
	BOOL IsNullFieldValue(LPCTSTR lpFieldName);

	BOOL GetFieldValue(LPCTSTR lpFieldName, int& nValue);
	BOOL GetFieldValue(int nIndex, int& nValue);
	BOOL GetFieldValue(LPCTSTR lpFieldName, long& lValue);
	BOOL GetFieldValue(int nIndex, long& lValue);
	BOOL GetFieldValue(LPCTSTR lpFieldName, unsigned long& ulValue);
	BOOL GetFieldValue(int nIndex, unsigned long& ulValue);
	BOOL GetFieldValue(LPCTSTR lpFieldName, double& dbValue);
	BOOL GetFieldValue(int nIndex, double& dbValue);
	BOOL GetFieldValue(LPCTSTR lpFieldName, CString& strValue, CString strDateFormat = _T(""));
	BOOL GetFieldValue(int nIndex, CString& strValue, CString strDateFormat = _T(""));
	BOOL GetFieldValue(LPCTSTR lpFieldName, COleDateTime& time);
	BOOL GetFieldValue(int nIndex, COleDateTime& time);
	BOOL GetFieldValue(int nIndex, bool& bValue);
	BOOL GetFieldValue(LPCTSTR lpFieldName, bool& bValue);
	BOOL GetFieldValue(int nIndex, COleCurrency& cyValue);
	BOOL GetFieldValue(LPCTSTR lpFieldName, COleCurrency& cyValue);
	BOOL GetFieldValue(int nIndex, _variant_t& vtValue);
	BOOL GetFieldValue(LPCTSTR lpFieldName, _variant_t& vtValue);

	BOOL GetFieldValue(LPCTSTR lpFieldName, BYTE& btValue);
	BOOL GetFieldValue(int nIndex, BYTE& btValue);

	

//protected:
    BOOL                m_bOpen;
	BOOL				m_bRSOpen;
    BOOL                m_bConnection;
    BOOL                m_bRecordset;
    BOOL                m_bCommand;

    CString             m_strErrorString;
    DWORD               m_dwLastError;
    EN_YADO_DATABASE	m_enDatabase;

    CString             m_strConnectString;
    CString             m_strProvider;
    CString             m_strDataSource;
    CString             m_strDatabaseName;
    CString             m_strDatabaseString;
    CString             m_strUserId;
    CString             m_strUserPasswd;

	CString				m_strRecordSource;	    // 테이블 이름
	
	int					m_nFields;
	int					m_nRecords;

    _ConnectionPtr      m_pConnection;		// DB Connection Pointer
    _CommandPtr         m_pCommand;			// Command Pointer
    _RecordsetPtr       m_pRecordSet;		// Recordset Pointer
    IADORecordBinding*	m_pRecordBinding;   // Record Binding Pointer

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//  [5/20/2021 KHS]
// 	BOOL IsExistTable(CString strTableName)
// 	{
// 		CString strSql;
// 		int nCnt;
// 		strSql.Format(L"SELECT * FROM INFORMATION_SCHEMA.TABLES "
// 			L"TABLE_NAME = '%s'	"
// 			,strTableName);
// 		if(OpenQuery(strSql) == FALSE)
// 			return FALSE;
// 
// 		nCnt = GetRecordCount();
// 		if(nCnt > 0)
// 			return TRUE;
// 		return FALSE;
// 	}
// 	BOOL MakeTable(CString strTable ,BOOL bDrop)
// 	{
// 		CString strSql;
// 		if(bDrop)
// 		{
// 			strSql.Format(L"DROP TABLE %s" , strTable);
// 			if(ExecuteSql(strSql) == FALSE)
// 				return FALSE;
// 		}
// 		else
// 		{
// 			if(IsExistTable(strTable) == TRUE)
// 				return TRUE;
// 		}
// 		
// 
// 		CString strSql;
// 		int nCnt;
// 		strSql.Format(L"SELECT * FROM INFORMATION_SCHEMA.TABLES "
// 			L"TABLE_NAME = '%s'	"
// 			,strTableName);
// 		if(OpenQuery(strSql) == FALSE)
// 			return FALSE;
// 
// 		nCnt = GetRecordCount();
// 		if(nCnt > 0)
// 			return TRUE;
// 		return FALSE;
// 	}
	//  [5/20/2021 KHS]
	BOOL CreateMSSqlDB(CString strDBName,CString strPath)
	{
		CString strSql;
		strSql.Format(L"CREATE DATABASE %s ON PRIMARY "
			L"(NAME = '%s_Data', "
			L"FILENAME = '%s\\%s_Data.MDF', "
			L"SIZE = 5MB, FILEGROWTH = 1)"
			L"LOG ON (NAME = %s_Log, "
			L"FILENAME = '%s\\%s_LOG.LDF', "
			L"SIZE = 1MB, "
			L"FILEGROWTH = 1)"
			,strDBName
			, strDBName
			,strPath,strDBName
			,strDBName
			,strPath,strDBName
		);
		TRACE(strSql + L"\n");
		return ExecuteSql(strSql);
	}

	//[2012/12/2:KHS] Database 생성
	BOOL CreateDB(CString strConString , EN_YADO_DATABASE enDatabase = enMDB)
	{
		HRESULT hr = S_OK;
		ADOX::_CatalogPtr pCatalog = NULL;
		try
		{	
			hr = pCatalog.CreateInstance(__uuidof(ADOX::Catalog));
			if (FAILED(hr))
				_com_issue_error(hr);

			/*
			* 데이터베이스 생성
			*/
			pCatalog->Create(_bstr_t(strConString));
				/*
			* 데이터베이스 접속
			*/
			pCatalog->PutActiveConnection(_bstr_t(strConString));

			pCatalog.Release();

			SetDatabaseType(enDatabase);
			SetConnectString(strConString);
			return DBOpen();
		}
		catch(const _com_error& e)
		{
			CString strError;

			if (e.Error() == 0x80040E17l/*DB_E_DUPLICATEDATASOURCE*/)
			{			
				strError.Format(_T("%s"), (char*)(LPCTSTR)e.Description());
				AfxMessageBox(strError);
			}
			else
			{
				strError.Format(_T("데이터베이스 생성 에러[%X]:%s"), e.Error(), (char*)(LPCTSTR)e.Description());
				AfxMessageBox(strError);
			}

			return FALSE;
		}
		return TRUE ; 
	}

	// [KHS 2019-12-22 12:59:18] 
	// MSSQL DB 등록
	BOOL AttachMSDB(CString strDBName , CString strDataFile , CString strLogFile)
	{
		if (enSQL != m_enDatabase)
			return FALSE;
		CString strSql;
		//strSql.Format(L"use master\n  EXEC sp_detach_db '한글데이터베이스' , 'true'");
		strSql.Format(
			L"exec sp_attach_db @dbname = '%s',"
			L"@filename1 = '%s',"
			L"@filename2 = '%s'"
			,strDBName 
			, strDataFile
			, strLogFile
		);
		TRACE(strSql+L"");
		BOOL bRet = FALSE;
// 		if (OpenQuery(strSql) == FALSE)
// 			return FALSE;
// 
// 		int nCnt = GetRecordCount();
// 		RSClose();
// 		if (nCnt > 0)
// 			return TRUE;
// 		return FALSE;
		return ExecuteSql(strSql);
	}

	BOOL DetachMSDB(CString strDBName)
	{
		if (enSQL != m_enDatabase)
			return FALSE;
		CString strSql;
		strSql.Format(
			L"ALTER DATABASE %s "
			L"SET SINGLE_USER WITH ROLLBACK IMMEDIATE"
			, strDBName
		);
		ExecuteSql(strSql);
		strSql.Format(
			L"EXEC sp_detach_db '%s' , 'true'"
			, strDBName
		);
		return ExecuteSql(strSql);
	}

	BOOL DetachMSDBAsMaster(CString strDBName)
	{
		if(enSQL != m_enDatabase)
			return FALSE;

		if(m_strDatabaseName.CompareNoCase(L"Master") != 0)
			return FALSE;

		CString strSql;
		strSql.Format(
			L" DECLARE @Spid INT  \n"
			L" DECLARE @ExecSQL VARCHAR(255) \n"
			L" DECLARE KillCursor CURSOR LOCAL STATIC READ_ONLY FORWARD_ONLY \n"
			L" FOR \n"
			L" SELECT  DISTINCT SPID \n"
			L" FROM    MASTER..SysProcesses \n"
			L" WHERE   DBID = DB_ID('%s') \n"
			L" OPEN    KillCursor \n"
			L" FETCH   NEXT \n"
			L" FROM    KillCursor \n"
			L" INTO    @Spid \n"
			L" WHILE   @@FETCH_STATUS = 0 \n"
			L" BEGIN \n"
			L" SET     @ExecSQL = 'KILL ' + CAST(@Spid AS VARCHAR(50)) \n"
			L" EXEC    (@ExecSQL) \n"
			L" 	FETCH   NEXT \n"
			L" 	FROM    KillCursor \n"
			L" 	INTO    @Spid \n"
			L" 	END \n"
			L" 	CLOSE   KillCursor \n"
			L" 	DEALLOCATE  KillCursor \n"
			//L" 	EXEC sp_detach_db '%s', 'true';  "
			,strDBName
			//,strDBName
		);
		TRACE(L"%s\n" , strSql);
		ExecuteSql(strSql);
		strSql.Format(
			L"EXEC sp_detach_db '%s' , 'true'"
			,strDBName
		);
		return ExecuteSql(strSql);
	}

	BOOL IsAttachedDatabase(CString strDBName)
	{
		if (enSQL != m_enDatabase)
			return FALSE;
		int nCnt;
		CString strSql;
		strSql.Format(L"Select * FROM SYS.DATABASES "
					L"WHERE NAME = '%s' "
			, strDBName
		);
		if (OpenQuery(strSql) == FALSE)
			return FALSE;

		nCnt = GetRecordCount();
		RSClose();
		if (nCnt > 0)
			return TRUE;
		return FALSE;
	}

	int GetCountDBConnect(CString strDBName)
	{
		if (enSQL != m_enDatabase)
			return -1;
		CString strSql;
		int nCnt;
		strSql.Format(L"SELECT DB_NAME(DBID) , COUNT(DBID) as CONCNT "
			L" FROM sys.sysprocesses "
			L"WHERE DBID > 0 AND DBID LIKE '%s' "
			L"GROUP BY DBID"
			, strDBName
		);
		if (OpenQuery(strSql) == FALSE)
			return -1;

		nCnt = GetRecordCount();
		RSClose();
		return nCnt;
	}

	BOOL BackupDatabase(CString strDBName, CString strPath)
	{
		CString strSql;
		strSql.Format(L"BACKUP DATABASE %s TO DISK='%s' "
			, strDBName , strPath
		);
		BOOL bRet;
		bRet = ExecuteSql(strSql);
		return bRet;
	}
	/*
	RESTORE FILELISTONLY FROM DISK='D:\04. Ficon3\51. Project\연동\연동.bak' ;
	RESTORE DATABASE 연동 FROM DISK='D:\04. Ficon3\51. Project\연동\연동.bak'
	WITH
	MOVE  'GFS_BASE_DATA' TO 'D:\04. Ficon3\51. Project\연동\연동_DATA.MDF' ,
	MOVE  'GFS_BASE_LOG' TO 'D:\04. Ficon3\51. Project\연동\연동_LOG.LDF'
	*/
	BOOL RestoreDatabase(CString strDBName, CString strLocation , CString strFile)
	{
		CString strSql,strPath , strOrData , strOrLog ;
		int nCnt ;
		if (strLocation.Right(1) != '\\')
			strLocation += L"\\";
		strPath.Format(L"%s%s" , strLocation, strFile);
		strSql.Format(L"RESTORE FILELISTONLY FROM DISK='%s'", strPath);
		if (OpenQuery(strSql) == FALSE)
			return FALSE;
		nCnt = GetRecordCount();
		if (nCnt < 2)
			return FALSE;
		GetFieldValue(L"LogicalName", strOrData);
		MoveNext();
		GetFieldValue(L"LogicalName", strOrLog);
		strSql.Format(L"RESTORE DATABASE %s FROM DISK='%s' "
			L"WITH "
			L"MOVE '%s' TO '%s%s_DATA.MDF' ,"
			L"MOVE '%s' TO '%s%s_LOG.LDF' "
			, strDBName, strPath
			, strOrData, strLocation, strDBName
			, strOrLog, strLocation, strDBName
		);
		BOOL bRet = ExecuteSql(strSql);
		return bRet;
	}

	BOOL DropCleanBuffer()
	{
		CString strSql;
		strSql.Format(L"DBCC DROPCLEANBUFFERS");
		BOOL bRet = ExecuteSql(strSql);
		return bRet;
	}

	BOOL GetDBState(CString strDBName,CString &strState,int &nState)
	{
		int nCnt,nAccess;
		CString strSql;
		strSql.Format(L"SELECT user_access,user_access_desc,state,state_desc"
			L" FROM sys.databases WHERE name = '%s'",strDBName);
		if(OpenQuery(strSql) == FALSE)
			return FALSE;
		nCnt = GetRecordCount();
		if(nCnt <= 0)
			return FALSE;
		GetFieldValue(L"state",nState); // 0일때 ONLINE
		GetFieldValue(L"state_desc",strState); // ONLINE , OFFLINE,RESTORING,RECOVERING,RECOVERY PENDING,SUSPECT,EMERGENCY
		GetFieldValue(L"user_access",nAccess);
		GetFieldValue(L"user_access_desc",strState);
		return TRUE;
	}

	BOOL SetOffLine(CString strDBName)
	{
		if(enSQL != m_enDatabase)
			return FALSE;
		CString strSql;
		strSql.Format(
			L"ALTER DATABASE %s "
			L"SET OFFLINE WITH ROLLBACK IMMEDIATE"
			,strDBName
		);
		return ExecuteSql(strSql);
	}

};

class YAdoTransaction
{
public:
	YAdoTransaction ( YAdoDatabase* pDatabase )
	{
		m_bRollback = FALSE;
		m_pDatabase = pDatabase;
		m_pDatabase->BeginTransaction();
	}
	~YAdoTransaction()
	{
		Do();
	}

	void Rollback()
	{
		m_bRollback = TRUE;
	}

	void Do()
	{
		if ( m_pDatabase )
		{
			if ( m_bRollback )
			{
				m_pDatabase->RollbackTransaction();
			}
			else
			{
				m_pDatabase->CommitTransaction();
			}
			m_pDatabase = NULL;
		}
	}

	YAdoDatabase*	m_pDatabase;
	BOOL			m_bRollback;
};

void WriteAdoLog(const WCHAR * szFmt, ...);

#endif // !defined(AFX_YADODATABASE_H__639A8D2A_A57D_4650_AB38_C18BF2E39542__INCLUDED_)

