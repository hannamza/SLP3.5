#pragma once

class YAdoDatabase;

class CPrjVerionInfo : public CObject
{
public:
	CPrjVerionInfo();
	virtual ~CPrjVerionInfo();

	SIMPLE_FUNC_IMPL(WORD, PrjMajorNum, m_wPrjMajorNum);
	SIMPLE_FUNC_IMPL(WORD, PrjMinorNum, m_wPrjMinorNum);
	SIMPLE_FUNC_IMPL(COleDateTime, PrjModifyDate, m_dtModify);
	SIMPLE_FUNC_IMPL(CString, PrjModifier, m_strPrjModifier);

	CStringList * GetRelayTableFileList() {
		return &m_strFileNameList;
	}

protected:
	WORD			m_wPrjMajorNum;
	WORD			m_wPrjMinorNum;
	CStringList		m_strFileNameList;
	COleDateTime	m_dtModify;
	CString			m_strPrjModifier;
};

typedef CTypedPtrList<CObList, CPrjVerionInfo *> CManagerVersion;

class CProjectInfo : public CObject
{
public:
	CProjectInfo();
	virtual ~CProjectInfo();

	SIMPLE_FUNC_IMPL(CString, PrjName, m_strPrjName);
	SIMPLE_FUNC_IMPL(CString, SiteName, m_strSiteName);
	SIMPLE_FUNC_IMPL(CString, SitePhone, m_strSitePhone);
	SIMPLE_FUNC_IMPL(CString, PrjMaker, m_strPrjMaker);
	SIMPLE_FUNC_IMPL(CString, PrjPath, m_strPrjPath);
	SIMPLE_FUNC_IMPL(CString, DBName, m_strDBName);
	SIMPLE_FUNC_IMPL(CString, DBPwd, m_strDBPwd);
	SIMPLE_FUNC_IMPL(CString, DBUser, m_strDBUser);
	SIMPLE_FUNC_IMPL(COleDateTime, PrjCreateDate, m_dtCreate);
// Project 

protected:

	// Project Info
	CString			m_strPrjName;
	CString			m_strSiteName;
	CString			m_strSitePhone;
	CString			m_strPrjMaker;
	CStringList		m_strFileNameList;

	// path
	CString			m_strPrjPath;

	//Database
	CString			m_strDBName;
	CString			m_strDBPwd;
	CString			m_strDBUser;

	// Date
	COleDateTime	m_dtCreate;
	CManagerVersion	m_MngVersion;


// 	WORD			m_wPrjMajorNum;
// 	WORD			m_wPrjMinorNum;
// 	DWORD			m_dwPrjVersion;
	
	
// 	COleDateTime	m_dtModify;
// 	CString			m_strPrjModifier;
// 
// 	// Current Project Path
// 	CString							m_strProjectPath;
// 	CString							m_strProjectFile;;
// 
// 	// Project Info
// 	ULONG_PTR						m_uPrjVersion;
// 	CString							m_strPrjName;
// 	CString							m_strSiteName;
// 	CString							m_strPrjMaker;
// 	CString							m_strPrjDesc;
// 
// 	COleDateTime					m_dtCreate;
// 	COleDateTime					m_dtModify;
// 
// 	int								m_nDBPort;
// 	CString							m_strDBName;
// 	CString							m_strDBAddr;
// 	CString							m_strDBUser;
// 	CString							m_strDBPwd;
// 
// 	YAdoDatabase *					m_pPrjDataDB; // Project에서 사용되는 전체 데이터가 정장된 DB

public:

public:
// 
// 	int NewProject(LPCTSTR lpszPathName
// 					, LPCTSTR lpszPrjName
// 					, LPCTSTR lpszSiteName
// 					, LPCTSTR lpszMaker
// 					, LPCTSTR lpszDesc
// 	);
// 	int OpenProject(LPCTSTR lpszPathName);
// 	int SaveProject();
// 	int SaveAsProject(LPCTSTR lpszOldPathName, LPCTSTR lpszNewPathName);
// 
// 	int CreatePrjDataTable();
// 	int InsertDefaultPrjData();
// 	bool CreateProjectDirectory(CString strPrjRootPath);

};

