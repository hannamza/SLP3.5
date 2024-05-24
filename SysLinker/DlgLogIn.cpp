// DlgLogIn.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgLogIn.h"
#include "afxdialogex.h"
#include "DlgVersion.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "../Common/Utils/StdioFileEx.h"
#include "RelayTableData.h"
// CDlgLogIn 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgLogIn, CDialogEx)

CDlgLogIn::CDlgLogIn(YAdoDatabase * pMainDB , CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_LOGIN, pParent)
	, m_strUser(_T(""))
	, m_strPwd(_T(""))
	, m_strPrjName(_T(""))
	, m_strCreateDate(_T(""))
	, m_strMaker(_T(""))
	, m_strPrjVersion(_T(""))
	, m_strModifyDate(_T(""))
	, m_strModifier(_T(""))
{
	m_pRefMainDB = pMainDB;
	m_nSelectPrjType = 0;
	m_strOpenPrjPath = L"";
	m_strOpenPrjName = L"";
	//m_strOpenDBName = L"";
	m_wMajor = 1;
	m_wMinor = 0;
	m_bVersionTempSelected = FALSE;
}

CDlgLogIn::~CDlgLogIn()
{
	ST_TEMP_PRJINFO * pInfo;
	while (!m_ptrList.IsEmpty())
	{
		pInfo = (ST_TEMP_PRJINFO *)m_ptrList.RemoveHead();
		if (pInfo == nullptr)
			continue; 
		delete pInfo;
		pInfo = nullptr;
	}
}

void CDlgLogIn::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_CMB_PROJECT, m_cmbProject);
	DDX_Text(pDX, IDC_ED_USER, m_strUser);
	DDX_Text(pDX, IDC_ED_PWD, m_strPwd);
	DDX_Control(pDX, IDC_PROJECT_TREE, m_ctrlProject);
	DDX_Text(pDX, IDC_ED_NAME, m_strPrjName);
	DDX_Text(pDX, IDC_ED_CREATEDATE, m_strCreateDate);
	DDX_Text(pDX, IDC_ED_MAKER, m_strMaker);
	DDX_Text(pDX, IDC_ED_PRJ_VERSION, m_strPrjVersion);
	DDX_Text(pDX, IDC_ED_MODIFYDATE, m_strModifyDate);
	DDX_Text(pDX, IDC_ED_MODIFIER, m_strModifier);
}


BEGIN_MESSAGE_MAP(CDlgLogIn, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgLogIn::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLogIn::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_CMB_PROJECT, &CDlgLogIn::OnCbnSelchangeCmbProject)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROJECT_TREE, &CDlgLogIn::OnTvnSelchangedProjectTree)
END_MESSAGE_MAP()


// CDlgLogIn 메시지 처리기입니다.


BOOL CDlgLogIn::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// 프로젝트 Path에서 현재 있는 프로젝트 검색
	CString strPath = g_stConfig.szPrjPath;
	FillProjectTree(strPath);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgLogIn::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	CString strDBPath = L"",strtemp;
	ST_TEMP_PRJINFO * pInfo;

	HTREEITEM hItem;
	hItem = m_ctrlProject.GetSelectedItem();
	if (hItem == nullptr)
	{
		GF_AddLog(L"선택된 버전이 없습니다.");
		AfxMessageBox(L"선택된 버전이 없습니다.");
		return;
	}

	pInfo = (ST_TEMP_PRJINFO*)m_ctrlProject.GetItemData(hItem);
	if (pInfo == nullptr)
	{
		GF_AddLog(L"선택된 버전의 데이터가 없습니다.");
		AfxMessageBox(L"선택된 버전의 데이터가 없습니다.");
		return;
	}

	if (m_strUser == L"")
	{
		GF_AddLog(L"사용자 이름이 없습니다.");
		AfxMessageBox(L"사용자 이름이 없습니다.");
		return;
	}

	if (m_strPwd == L"")
	{
		GF_AddLog(L"비밀번호가 없습니다.");
		AfxMessageBox(L"비밀번호가 없습니다.");
		return;
	}

	m_strOpenPrjName = pInfo->strName;
	m_strOpenPrjPath = pInfo->strPrjPath;
	m_wMajor = pInfo->wMajor;
	m_wMinor = pInfo->wMinor;
	m_bVersionTempSelected = pInfo->bVerTemp;

	CString strPath = g_stConfig.szPrjPath;
	if (strPath.Right(1) != '\\')
		strPath += L"\\";
	if (pInfo->bVerTemp)
	{
		strDBPath.Format(L"%s%s\\%s\\%s\\"
			, strPath, m_strOpenPrjName
			, F3_VERSIONTEMPFOLDER_NAME
			, F3_PRJ_DIR_DATABASE
		);
	}
	else
	{
		strDBPath.Format(L"%s%s\\%s\\%s\\"
			, strPath, m_strOpenPrjName
			, GF_GetVersionFolderName(m_wMajor, m_wMinor)
			, F3_PRJ_DIR_DATABASE
		);
	}
	

	if (CheckLogin(strDBPath ,m_strUser , m_strPwd) <= 0)
		return;

	//20240523 GBM start - 아래 행정은 CheckLogin에서 이미 하고 있으므로 불필요한 행정으로 주석처리
	// DB 연결 해제
	//m_pRefMainDB->DetachMSDB(m_strOpenPrjName);	
	//20240523 GBM end

	CDialogEx::OnOK();
}


void CDlgLogIn::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}

// 
// int CDlgLogIn::PutProjectInCombo(CString strPath)
// {
// 	m_cmbProject.InsertString(0, L"새 프로젝트");
// 	ST_TEMP_PRJINFO * pInfo;
// 	int nIdx = 1 , nTemp; 
// 	CString strFile;
// 	CString strFindCriteria;
// 	CFileFind find , fsub;
// 	BOOL bSub = FALSE;
// 	CString strSubFind , strSubFile , strPrjName , strSubPath;
// 	if (strPath.Right(1) != '\\')
// 		strPath += L"\\";
// 	strFindCriteria.Format(L"%s*.*", strPath);
// 	BOOL bResult = find.FindFile(strFindCriteria);
// 	while (bResult)
// 	{
// 		bResult = find.FindNextFile();
// 		strFile = find.GetFilePath();
// 		if (find.IsDots())
// 			continue; 
// 		if (find.IsDirectory())
// 		{
// 			strSubFind = strFile;
// 			if (strSubFind.Right(1) != '\\')
// 				strSubFind += L"\\";
// 			strSubFind += L"*.";
// 			strSubFind += F3_PROJECT_EXT;
// 			bSub = fsub.FindFile(strSubFind);
// 			while (bSub)
// 			{
// 				bSub = fsub.FindNextFile();
// 				if (fsub.IsDots())
// 					continue;
// 				if (fsub.IsDirectory())
// 				{
// 					// Version 폴더 - Project명만 가져오기 때문에 버전정보는 가져오지 않는다.
// 					continue;
// 				}
// 				strSubFile = fsub.GetFileName();
// 				strSubPath = fsub.GetFilePath();
// 				nTemp = strSubFile.ReverseFind('.');
// 				strPrjName = strSubFile.Mid(nTemp - 1);
// 				m_cmbProject.InsertString(nIdx, strPrjName);
// 				pInfo = new ST_TEMP_PRJINFO;
// 				pInfo->wMajor = 1;
// 				pInfo->wMinor = 0;
// 				pInfo->strPath = strSubPath;
// 				pInfo->strName = strPrjName;
// 				m_cmbProject.SetItemData(nIdx, (DWORD_PTR)pInfo);
// 				m_ptrList.AddTail(pInfo);
// 				nIdx++;
// 			}
// 		}
// 	}
// 	m_cmbProject.SetCurSel(0);
// 	return 1;
// }


int CDlgLogIn::GetPrjVersionCount(CString strPath)
{
	bool bIsVersionTemp = false;
	int nCnt =0;
	CString strFile;
	CString strUpperFile, strTempUpper;
	CString strFindCriteria;
	CFileFind find;
	if (strPath.Right(1) != '\\')
		strPath += L"\\";
	strFindCriteria.Format(L"%s*.*", strPath);
	BOOL bResult = find.FindFile(strFindCriteria);
	while (bResult)
	{
		bResult = find.FindNextFile();
		strFile = find.GetFilePath();
		if (find.IsDots())
			continue;
		if (find.IsDirectory())
		{
			strUpperFile = strFile;
			strUpperFile.MakeUpper();
			if (strUpperFile.Find(L"VERSION") >= 0)
			{
				// VERSION_TEMP 일때 COUNT 무시
				if (strUpperFile.Find(L"TEMP") < 0)
					nCnt++;
				else
					bIsVersionTemp = true;
			}
		}
	}
	// Version_TEMP 폴더만 있을때 --> 프로그램 생성하고 저장하지 않는경우
	if (nCnt == 0 && bIsVersionTemp)
		return -1;
	return nCnt;
}


void CDlgLogIn::OnCbnSelchangeCmbProject()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


int CDlgLogIn::CheckLogin(CString strDBPath, CString strUser, CString strPwd)
{
	// 1. DB 등록
	// 2. Loging
	// 3. DB 분리
	YAdoDatabase * pDB = nullptr;
	int nCnt = 0; 
	CString strPath, strBkName;
	CString strDB, strlog;
	CString strSql;
	strPath = strDBPath;

	if (strPath.Right(1) != '\\')
		strPath += L"\\";
	if (m_pRefMainDB == nullptr)
		return -1;
	if (m_pRefMainDB->IsAttachedDatabase(m_strOpenPrjName) == TRUE)
	{
		if (m_pRefMainDB->DetachMSDB(m_strOpenPrjName) == FALSE)
		{
			CString strError;
			strError.Format(L"프로젝트 데이터베이스(%s)를 분리하는데 실패 했습니다.\n%s"
				, m_strOpenPrjName, m_pRefMainDB->GetLastErrorString());
			GF_AddLog(strError);
			AfxMessageBox(strError);
			return -1;
		}
	}

	strDB.Format(L"%s_DATA.MDF", m_strOpenPrjName);
	if (GF_FindFile(strPath, strDB) == TRUE) // DB 등록 필요
	{
		strDB.Format(L"%s%s_DATA.MDF", strPath, m_strOpenPrjName);
		strlog.Format(L"%s%s_LOG.LDF", strPath, m_strOpenPrjName);
		//ATTACH
		if (m_pRefMainDB->AttachMSDB(m_strOpenPrjName, strDB, strlog) == FALSE)
		{

			CString strError;
			strError.Format(L"프로젝트 데이터베이스(%s)를 등록하는데 실패 했습니다.\n%s"
				, m_strOpenPrjName, m_pRefMainDB->GetLastErrorString());
			GF_AddLog(strError);
			AfxMessageBox(strError);
			return 0;
		}
	}
	else // Restore 필요
	{
		strBkName.Format(L"%s.BAK", m_strOpenPrjName);
		if (m_pRefMainDB->RestoreDatabase(m_strOpenPrjName, strPath, strBkName) == FALSE)
		{

			CString strError;
			strError.Format(L"프로젝트 데이터베이스(%s)를 복원하는데 실패 했습니다.\n%s"
				, m_strOpenPrjName, m_pRefMainDB->GetLastErrorString());
			GF_AddLog(strError);
			AfxMessageBox(strError);
			return 0;
		}
	}
	
	pDB = new YAdoDatabase;
	pDB->MSSqlInitialize(g_stConfig.szDBPass, g_stConfig.szDBUser, m_strOpenPrjName, g_stConfig.szSource);
	if (pDB->DBOpen() == FALSE)
	{
		CString strError;
		strError.Format(L"프로젝트 데이터베이스(%s)를 접속하는데 실패 했습니다.\n%s"
			, m_strOpenPrjName, pDB->GetLastErrorString());
		GF_AddLog(strError);
		AfxMessageBox(strError);
		delete pDB;
		return 0;
	}
	strSql.Format(L"SELECT  A.USER_ID, A.USER_NAME, A.GRP_ID, A.USER_DESC, A.USER_PWD "
		L", A.PHONE1, A.PHONE2, A.PHONE3, A.ENABLE, A.FIRST_SCREEN "
		L", B.ACCESS_ID, B.GRP_DESC "
		L" FROM     TB_USER AS A INNER JOIN "
		L" TB_USER_GROUP AS B ON A.GRP_ID = B.GRP_ID"
		L" AND A.USER_ID='%s' AND A.USER_PWD='%s' "
		, strUser, strPwd
	);

	if (pDB->OpenQuery(strSql) == FALSE)
	{
		CString strError;
		strError.Format(L"사용자 정보를 가져오는데 실패 했습니다.\n%s"
			, pDB->GetLastErrorString());
		GF_AddLog(strError);
		AfxMessageBox(strError);
		pDB->DBClose();
		delete pDB;
		return 0;
	}

	nCnt = pDB->GetRecordCount();
	pDB->DBClose();
	pDB->RSClose();
	delete pDB;

	if (nCnt <= 0)
	{
		GF_AddLog(L"아이디 또는 패스워드가 틀립니다.");
		AfxMessageBox(L"아이디 또는 패스워드가 틀립니다.");
		return -1;
	}

	/*
	SELECT  A.USER_ID, A.USER_NAME, A.GRP_ID, A.USER_DESC, A.USER_PWD
			, A.PHONE1, A.PHONE2, A.PHONE3, A.ENABLE, A.FIRST_SCREEN
			, B.ACCESS_ID, B.GRP_DESC
	FROM     TB_USER AS A INNER JOIN
			 TB_USER_GROUP AS B ON A.GRP_ID = B.GRP_ID
	*/

	

	return 1;
}


int CDlgLogIn::OneTimeOpenDatabase(CString strDBPath)
{
	/*
	1. Database연결
	2. Login 확인
	3. Database 분리
	*/
	return 0;
}


void CDlgLogIn::OnTvnSelchangedProjectTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	HTREEITEM hSel = m_ctrlProject.GetSelectedItem();
	if (hSel == nullptr)
		return;
	ST_TEMP_PRJINFO * pItem = (ST_TEMP_PRJINFO*)m_ctrlProject.GetItemData(hSel);
	if (pItem == nullptr)
		return; 

	DisplayPrjInfo(pItem);
}


int CDlgLogIn::FillProjectTree(CString strPath)
{
	ST_TEMP_PRJINFO * pInfo;
	int nIdx = 1, nTemp;
	CString strFile;
	CString strFindCriteria;
	CFileFind find, fsub;
	HTREEITEM hItem;
	BOOL bSub = FALSE;
	CString strSubFind, strSubFile, strPrjName, strSubPath , strtemp;
	if (strPath.Right(1) != '\\')
		strPath += L"\\";
	strFindCriteria.Format(L"%s*.*", strPath);
	BOOL bResult = find.FindFile(strFindCriteria);
	while (bResult)
	{
		bResult = find.FindNextFile();
		strFile = find.GetFilePath();
		if (find.IsDots())
			continue;
		// Project Folder인지 확인
		if (find.IsDirectory())
		{
			strSubFind = strFile;
			if (strSubFind.Right(1) != '\\')
				strSubFind += L"\\";
			strSubFind += L"*.";
			strSubFind += F3_PROJECT_EXT;
			bSub = fsub.FindFile(strSubFind);
			while (bSub)
			{
				bSub = fsub.FindNextFile();
				strSubFile = fsub.GetFileName();
				strtemp = fsub.GetFilePath();
				if (fsub.IsDots())
					continue;
				if (fsub.IsDirectory())
				{
					// Version 폴더
					//
					continue;
				}
// 				strSubFile = fsub.GetFileName();
// 				strSubPath = fsub.GetFilePath();
				nTemp = strtemp.ReverseFind('\\');
				strSubPath = strtemp.Mid(0,nTemp);
				nTemp = strSubFile.ReverseFind('.');
				strPrjName = strSubFile.Mid(0, nTemp);
				hItem = m_ctrlProject.InsertItem(strPrjName, 0, 0, TVI_ROOT); 
				pInfo = SetProjectInfo(strSubPath ,strPrjName);
				pInfo->hItem = hItem;
				pInfo->hParent = TVI_ROOT;
				pInfo->wMajor = 1;
				pInfo->wMinor = 0;
				pInfo->bVerTemp = 1;
				m_ctrlProject.SetItemData(hItem, (DWORD_PTR)pInfo);
				AddVersion(strSubPath , pInfo);
				m_ptrList.AddTail(pInfo);
				nIdx++;
			}
		}
	}
	return 0;
}


int CDlgLogIn::AddVersion(CString strPath, ST_TEMP_PRJINFO * pInfo)
{
	bool bIsVersionTemp = false;
	int nCnt = 0;
	CString strFile;
	CString strUpperFile, strTempUpper;
	CString strFindCriteria;
	CFileFind find;
	if (strPath.Right(1) != '\\')
		strPath += L"\\";
	strFindCriteria.Format(L"%s*.*", strPath);
	BOOL bResult = find.FindFile(strFindCriteria);
	while (bResult)
	{
		bResult = find.FindNextFile();
		strFile = find.GetFilePath();
		if (find.IsDots())
			continue;
		if (find.IsDirectory())
		{
			strUpperFile = strFile;
			strUpperFile.MakeUpper();
			if (strUpperFile.Find(L"VERSION") >= 0)
			{
				// VERSION_TEMP 일때 COUNT 무시
				nCnt++;
				if (strUpperFile.Find(L"TEMP") < 0)
				{
					SetVersionInfo(pInfo , strFile,strPath );
					//hItem = m_ctrlProject.InsertItem()
				}
				else
				{
					bIsVersionTemp = true;
					SetVersionInfo(pInfo, strFile, strPath , TRUE);
				}
			}
		}
	}
	// Version_TEMP 폴더만 있을때 --> 프로그램 생성하고 저장하지 않는경우
	if (nCnt == 0 && bIsVersionTemp)
		return -1;
	return 0;
}


ST_TEMP_PRJINFO * CDlgLogIn::SetVersionInfo(ST_TEMP_PRJINFO * pParentInfo
	, CString strVersionFolder, CString strPrjPath, BOOL bVerTemp)
{
	std::vector<CString> vtArr;
	CStdioFileEx file;
	HTREEITEM hItem;
	DWORD dwData = 0;
	CString strVerFile;
	WORD wMajor, wMinor;
	ST_TEMP_PRJINFO * pInfo = nullptr;
	CString strtemp, strVersion;
	if (strVersionFolder.Right(1) != '\\')
		strVersionFolder += L"\\";
	strVerFile.Format(L"%s%s", strVersionFolder, F3_VERSIONFINFO_FILE);
	if (!file.Open(strVerFile, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
		USERLOG(L"프로젝트 파일을 생성하는데 실패 했습니다.\n");
		return nullptr;
	}

	file.ReadString(strtemp); //GetPrjMajorNum GetPrjMinorNum
	strVersion = strtemp;
	vtArr = GF_SplitString(strtemp, L".");
	if (vtArr.size() < 2)
		return nullptr;
	wMajor = _wtoi(vtArr[0]);
	wMinor = _wtoi(vtArr[1]);
	strtemp = GF_GetVersionFolderName(wMajor, wMinor);
	if (bVerTemp)
		strtemp += L"_TEMP";
	hItem = m_ctrlProject.InsertItem(strtemp, 0, 0, pParentInfo->hItem);
	pInfo = new ST_TEMP_PRJINFO;
	pInfo->strCreator = pParentInfo->strCreator;
	pInfo->strDateCreate = pParentInfo->strDateCreate;
	pInfo->hItem = hItem;
	pInfo->hParent = pInfo->hItem;
	pInfo->wMajor = wMajor;
	pInfo->wMinor = wMinor;
	pInfo->strPrjPath = strPrjPath;
	pInfo->strFullPath = strVersionFolder;
	pInfo->strName = pParentInfo->strName;
	file.ReadString(strtemp); //GetPrjModifyDate
	pInfo->strDateModify = strtemp;
	file.ReadString(strtemp); //GetPrjModifier
	pInfo->strModifier = strtemp;
	pInfo->bVerTemp = bVerTemp;
	m_ctrlProject.SetItemData(hItem, (DWORD_PTR)pInfo);
	m_ptrList.AddTail(pInfo);
	file.Close();
	return pInfo;
}

ST_TEMP_PRJINFO *   CDlgLogIn::SetProjectInfo(CString strPrjPath , CString strPrjName)
{
	ST_TEMP_PRJINFO * pInfo;
	CStdioFileEx file;
	CString strtemp, strPath;
	if (strPrjPath.Right(1) != '\\')
		strPrjPath += L"\\";
	strPath.Format(L"%s%s.%s"
		, strPrjPath, strPrjName, F3_PROJECT_EXT);
	if (!file.Open(strPath, CFile::modeRead| CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
		USERLOG(L"프로젝트 파일을 생성하는데 실패 했습니다.\n");
		return 0;
	}
	pInfo = new ST_TEMP_PRJINFO;
// 	pInfo->hItem = hItem;
// 	pInfo->hParent = hParent;
// 	pInfo->wMajor = 1;
// 	pInfo->wMinor = 0;
	pInfo->strPrjPath = strPrjPath;
	pInfo->strFullPath = strPrjPath;
	pInfo->strName = strPrjName;
	pInfo->strDateCreate;

	file.ReadString(strtemp); //GetPrjName
	file.ReadString(strtemp); //GetSiteName
	file.ReadString(strtemp); //GetSitePhone
	file.ReadString(strtemp); //GetPrjMaker
	pInfo->strCreator = strtemp;
	file.ReadString(strtemp); //GetPrjMajorNum GetPrjMinorNum
	file.ReadString(strtemp); //GetPrjPath
	file.ReadString(strtemp); //GetDBName
	file.ReadString(strtemp); //GetDBUser
	file.ReadString(strtemp); //GetDBPwd
	file.ReadString(strtemp); //GetPrjCreateDate
	pInfo->strDateCreate = strtemp;
	pInfo->strModifier = pInfo->strCreator;
	pInfo->strDateModify = pInfo->strDateCreate;
	// 	file.ReadString(strtemp); //GetPrjModifyDate
// 	file.ReadString(strtemp); //GetPrjModifier
	file.Close();
	return pInfo;
}


int CDlgLogIn::DisplayPrjInfo(ST_TEMP_PRJINFO * pInfo)
{
	m_strPrjName = pInfo->strName;
	m_strCreateDate = pInfo->strDateCreate;
	m_strMaker = pInfo->strCreator;
	//m_strPrjVersion.Format(L"%d.%d" , pInfo->wMajor , pInfo->wMinor);		//20240306 GBM - 표시되는 버전이 항상 1.0이므로 의미가 없어 표시를 안하게 함
	m_strModifyDate = pInfo->strDateModify;
	m_strModifier = pInfo->strDateModify;
	UpdateData(FALSE);
	return 0;
}
