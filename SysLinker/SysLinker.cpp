// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// SysLinker.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "SysLinker.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "SysLinkerDoc.h"
#include "SysLinkerView.h"


#include "FormDoc.h"
#include "FormAccess.h"
#include "FormPattern.h"
#include "FormPSwitch.h"
#include "FormLinkView.h"
#include "FormSwitch.h"
#include "FormPump.h"
#include "FormEquip.h"
#include "FormUser.h"
#include "FormEmergency.h"
#include "FormLocation.h"
#include "FormFacp.h"
#include "FormUnit.h"
#include "FormRelayEdit.h"
#include "FormAutoMake.h"
#include "FormEditLogic.h"
#include "FormLoadRelayTable.h"
#include "FormPSwitchEdit.h"
#include "FormErrorCheck.h"

#include "FormPumpEdit.h"

#include "PropSheetNewProject.h"
#include "PropPageProjectInfo.h"

#include "../Common/Utils/YAdoDatabase.h"
#include "../Common/Utils/StdioFileEx.h"


#include "RelayTableData.h"
#include "DlgEnv.h"

#include "DlgLogIn.h"
#include "DlgMakeAutoAllInput.h"

#include "DlgLogIn.h"
#include "DlgProgramLogin.h"
#include "DataEquip.h"

#include "DlgProgress.h"

#include "DlgMakePwd.h"
#include "DlgLoadRom.h"

#include "OutputWnd.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "DlgPatternProperty.h"
#include "../Version/Version.h"

#include "DlgWebLogin.h"

UINT ThreadCloseProject(LPVOID pParam)
{
	BOOL bRet = FALSE;
	theApp.m_bThreadSucceeded = TRUE;

	bRet = theApp.CloseProject();
	if (!bRet)
		theApp.m_bThreadSucceeded = FALSE;

	theApp.m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(theApp.m_hThreadHandle);
	return 0;
}

UINT ThreadGetDefaultEquipmentDefinition(LPVOID pParam)
{
	//20240822 GBM start - 기본 DB가 프로젝트 생성 시 설비 정의가 중계기 일람표 상 설비 정의로 overwrite 되므로 기본 설비 정의 엑셀 파일을 두어서 새 프로젝트 생성 시마다 초기화
	theApp.m_bThreadSucceeded = TRUE;
	CString strEquipmentDefinitionFile = _T("");
	CString strFolder = _T("");
	CString strFile = _T("");
	std::vector<CString> fileVec;
	BOOL bRet = FALSE;

	strFolder.Format(_T("C:\\%s\\%s"), ROOT_FOLDER, F3_PRJ_DIR_DATABASE);
	strFile.Format(_T("%s"), EQUIPMENT_INFO_EXCEL_FILE_NAME);
	fileVec = CCommonFunc::GetFullPathFileListIntheFolder(strFolder, strFile);	// 경로를 얻는 매서드를 파일이 존재하는 지 여부로 활용
	strEquipmentDefinitionFile.Format(_T("%s\\%s"), strFolder, strFile);
	theApp.m_strEquipmentDefinitionFile = strEquipmentDefinitionFile;
	if (fileVec.size() > 0)
	{
		bRet = theApp.InitEquipmentInfoTable(strEquipmentDefinitionFile);
		if (!bRet)
		{
			theApp.m_bThreadSucceeded = FALSE;

			CString strMsg = _T("");
#ifndef ENGLISH_MODE
			strMsg.Format(_T("기본 설비 정의 파일 [%s] 열기에 실패했습니다."), strEquipmentDefinitionFile);
#else
			strMsg.Format(_T("Failed to open the default equipment definition file [%s]."), strEquipmentDefinitionFile);
#endif
			GF_AddLog(strMsg);
			strMsg.Format(_T("Failed to open default equipment definition file [%s]."), strEquipmentDefinitionFile);
			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
		}
	}
	else
	{
		theApp.m_bThreadSucceeded = FALSE;

		CString strMsg = _T("");
#ifndef ENGLISH_MODE
		strMsg.Format(_T("기본 설비 정의 파일 [%s]이 없어서 적용하지 않습니다."), strEquipmentDefinitionFile);
#else
		strMsg.Format(_T("Not applied because the default equipment definition file [%s] doesn't exist."), strEquipmentDefinitionFile);
#endif
		GF_AddLog(strMsg);
		strMsg.Format(_T("The default equipmemt definition file [%s] does not exist and will not be applied."), strEquipmentDefinitionFile);
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));	
	}
	
	theApp.m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(theApp.m_hThreadHandle);
	//20240822 GBM end

	return 0;
}

UINT ThreadCreateNewProject(LPVOID pParam)
{
	theApp.m_bThreadSucceeded = TRUE;
	if (theApp.m_pFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 생성에 실패했습니다.");
#else
		GF_AddLog(L"Failed to create a project.");
#endif
		theApp.CloseProject();
		theApp.m_bThreadSucceeded = FALSE;
		theApp.m_pProgressBarDlg->PostMessage(WM_CLOSE);
		SetEvent(theApp.m_hThreadHandle);
		return 0;
	}

	/************************************************************************/
	/*
	1. Project Path 설정 :
	- 폴더 생성
	- File 생성
	2. Database 생성
	- Database 생성
	- Table 및 데이터 생성
	3. View 설정
	- DialogBar : Input , output ,

	*/
	/************************************************************************/
	if (theApp.CreateProject() <= 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 생성에 실패했습니다.");
#else
		GF_AddLog(L"Failed to create a project.");
#endif
		theApp.CloseProject();
		theApp.m_bThreadSucceeded = FALSE;
	}

	theApp.m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(theApp.m_hThreadHandle);
	return 0;
}

UINT ThreadOpenProjectDatabase(LPVOID pParam)
{
	theApp.m_bThreadSucceeded = TRUE;
	if (theApp.OpenProjectDatabase(theApp.m_pFasSysData) < 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 데이터베이스를 여는데 실패했습니다.");
#else
		GF_AddLog(L"Failed to open the project database.");
#endif
		theApp.m_bThreadSucceeded = FALSE;
		theApp.m_pProgressBarDlg->PostMessage(WM_CLOSE);
		SetEvent(theApp.m_hThreadHandle);
		return 0;
	}

	if (theApp.m_pFasSysData->LoadProjectDatabase() == 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 데이터베이스에서 데이터를 가져오는 데에 실패했습니다.");
#else
		GF_AddLog(L"Failed to retrieve the data from the project database.");
#endif
		theApp.m_bThreadSucceeded = FALSE;
	}

	theApp.m_pFasSysData->SetProjectOpened(TRUE);

	theApp.m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(theApp.m_hThreadHandle);

	return 0;
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
ST_VIEWFORM_SELECTITEM g_stViewSelectItem;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()

public:
	//	afx_msg void OnSortAddress();
//	afx_msg void OnChkViewdkoutput();
	CString m_strVersion;
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
,m_strVersion(_T(""))
{
#ifndef SLP4_MODE
	m_strVersion = GFSP_PRODUCT_UPDATE_DATE;
#else
	m_strVersion.Format(_T("SLP4 Program Version Number : [%g]\nLast Build Time : [%s]"), SLP4_VERSION_NUMBER, CCommonFunc::GetProgramVersion());
#endif
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_ST_VERSION,m_strVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

	//	ON_COMMAND(ID_SORT_ADDRESS, &CAboutDlg::OnSortAddress)
//	ON_COMMAND(ID_CHK_VIEWDKOUTPUT, &CAboutDlg::OnChkViewdkoutput)
END_MESSAGE_MAP()


// CSysLinkerApp

BEGIN_MESSAGE_MAP(CSysLinkerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSysLinkerApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)

	ON_COMMAND(ID_HOME_LOGIN, &CSysLinkerApp::OnHomeLogin)
	ON_COMMAND(ID_HOME_PROJECT_CLOSE, &CSysLinkerApp::OnHomeProjectClose)
	ON_COMMAND(ID_HOME_PROJECT_NEW, &CSysLinkerApp::OnHomeProjectNew)
	ON_COMMAND(ID_HOME_PROJECT_OPEN, &CSysLinkerApp::OnHomeProjectOpen)
	ON_COMMAND(ID_HOME_PROJECT_SAVE, &CSysLinkerApp::OnHomeProjectSave)
	ON_COMMAND(ID_BASIC_ENVSET, &CSysLinkerApp::OnBasicEnvset)
	ON_COMMAND(ID_BASIC_SET_ACCESSLEVEL, &CSysLinkerApp::OnBasicSetAccesslevel)
	ON_COMMAND(ID_BASIC_SET_DRAW, &CSysLinkerApp::OnBasicSetDraw)
	ON_COMMAND(ID_BASIC_SET_EQUIP, &CSysLinkerApp::OnBasicSetEquip)
	ON_COMMAND(ID_BASIC_SET_FACP, &CSysLinkerApp::OnBasicSetFacp)
	ON_COMMAND(ID_BASIC_SET_INOUT_TYPE, &CSysLinkerApp::OnBasicSetInoutType)
	ON_COMMAND(ID_BASIC_SET_PUMP, &CSysLinkerApp::OnBasicSetPump)
	ON_COMMAND(ID_BASIC_SET_UNIT, &CSysLinkerApp::OnBasicSetUnit)
	ON_COMMAND(ID_BASIC_SET_USER, &CSysLinkerApp::OnBasicSetUser)
	ON_COMMAND(ID_BASIC_SET_USERGROUP, &CSysLinkerApp::OnBasicSetUsergroup)
	ON_COMMAND(ID_BASIC_SET_LOGIC_EDIT, &CSysLinkerApp::OnBasicSetLogicEdit)
	ON_COMMAND(ID_BASIC_SET_ENV, &CSysLinkerApp::OnBasicSetEnv)
	//ON_COMMAND(ID_FACP_CREATE_LINK, &CSysLinkerApp::OnFacpCreateLink)
	ON_COMMAND(ID_FACP_EDIT_LINK, &CSysLinkerApp::OnFacpEditLink)
	ON_COMMAND(ID_FACP_EMMERGENCY, &CSysLinkerApp::OnFacpEmmergency)
	ON_COMMAND(ID_FACP_LINK_PS, &CSysLinkerApp::OnFacpLinkPs)
	ON_COMMAND(ID_FACP_LINK_PUMP, &CSysLinkerApp::OnFacpLinkPump)
	ON_COMMAND(ID_FACP_LOCATION, &CSysLinkerApp::OnFacpLocation)
	ON_COMMAND(ID_FACP_PATTERN, &CSysLinkerApp::OnFacpPattern)
	ON_COMMAND(ID_FACP_PREVIEW_LINK, &CSysLinkerApp::OnFacpPreviewLink)
	ON_COMMAND(ID_FACP_REVERSE_LINK, &CSysLinkerApp::OnFacpReverseLink)
	ON_COMMAND(ID_FACP_SET_FACPPWD, &CSysLinkerApp::OnFacpSetFacppwd)
	ON_UPDATE_COMMAND_UI(ID_HOME_LOGIN, &CSysLinkerApp::OnUpdateHomeLogin)
	ON_UPDATE_COMMAND_UI(ID_HOME_PROJECT_CLOSE, &CSysLinkerApp::OnUpdateHomeProjectClose)
	ON_UPDATE_COMMAND_UI(ID_HOME_PROJECT_NEW, &CSysLinkerApp::OnUpdateHomeProjectNew)
	ON_UPDATE_COMMAND_UI(ID_HOME_PROJECT_OPEN, &CSysLinkerApp::OnUpdateHomeProjectOpen)
	ON_UPDATE_COMMAND_UI(ID_HOME_PROJECT_SAVE, &CSysLinkerApp::OnUpdateHomeProjectSave)
	ON_UPDATE_COMMAND_UI(ID_BASIC_ENVSET, &CSysLinkerApp::OnUpdateBasicEnvset)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_ACCESSLEVEL, &CSysLinkerApp::OnUpdateBasicSetAccesslevel)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_DRAW, &CSysLinkerApp::OnUpdateBasicSetDraw)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_EQUIP, &CSysLinkerApp::OnUpdateBasicSetEquip)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_FACP, &CSysLinkerApp::OnUpdateBasicSetFacp)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_INOUT_TYPE, &CSysLinkerApp::OnUpdateBasicSetInoutType)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_PUMP, &CSysLinkerApp::OnUpdateBasicSetPump)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_UNIT, &CSysLinkerApp::OnUpdateBasicSetUnit)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_USER, &CSysLinkerApp::OnUpdateBasicSetUser)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_USERGROUP, &CSysLinkerApp::OnUpdateBasicSetUsergroup)
	ON_UPDATE_COMMAND_UI(ID_BASIC_SET_LOGIC_EDIT, &CSysLinkerApp::OnUpdateBasicSetEditLogic)
	ON_UPDATE_COMMAND_UI(ID_FACP_CREATE_LINK, &CSysLinkerApp::OnUpdateFacpCreateLink)
	ON_UPDATE_COMMAND_UI(ID_FACP_EDIT_LINK, &CSysLinkerApp::OnUpdateFacpEditLink)
	ON_UPDATE_COMMAND_UI(ID_FACP_EMMERGENCY, &CSysLinkerApp::OnUpdateFacpEmmergency)
	ON_UPDATE_COMMAND_UI(ID_FACP_LINK_PS, &CSysLinkerApp::OnUpdateFacpLinkPs)
	ON_UPDATE_COMMAND_UI(ID_FACP_LINK_PUMP, &CSysLinkerApp::OnUpdateFacpLinkPump)
	ON_UPDATE_COMMAND_UI(ID_FACP_LOCATION, &CSysLinkerApp::OnUpdateFacpLocation)
	ON_UPDATE_COMMAND_UI(ID_FACP_PATTERN, &CSysLinkerApp::OnUpdateFacpPattern)
	ON_UPDATE_COMMAND_UI(ID_FACP_PREVIEW_LINK, &CSysLinkerApp::OnUpdateFacpPreviewLink)
	ON_UPDATE_COMMAND_UI(ID_FACP_REVERSE_LINK, &CSysLinkerApp::OnUpdateFacpReverseLink)
	ON_UPDATE_COMMAND_UI(ID_FACP_SET_FACPPWD, &CSysLinkerApp::OnUpdateFacpSetFacppwd)
	ON_COMMAND(ID_FACP_RELAYEDIT, &CSysLinkerApp::OnFacpRelayedit)
	ON_UPDATE_COMMAND_UI(ID_FACP_RELAYEDIT, &CSysLinkerApp::OnUpdateFacpRelayedit)
	ON_COMMAND(ID_FILE_CLOSE, &CSysLinkerApp::OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, &CSysLinkerApp::OnUpdateFileClose)
	ON_COMMAND(ID_HOME_PROJECT_TABLE, &CSysLinkerApp::OnHomeProjectTable)
	ON_UPDATE_COMMAND_UI(ID_HOME_PROJECT_TABLE, &CSysLinkerApp::OnUpdateHomeProjectTable)
	ON_COMMAND(ID_ERROR_CHECK,&CSysLinkerApp::OnErrorCheck)
	ON_UPDATE_COMMAND_UI(ID_ERROR_CHECK,&CSysLinkerApp::OnUpdateErrorCheck)
	ON_UPDATE_COMMAND_UI(ID_HOME_ERROR_CHECK, &CSysLinkerApp::OnUpdateSimpleErrorCheck)
END_MESSAGE_MAP()


// CSysLinkerApp 생성

CSysLinkerApp::CSysLinkerApp()
{
	
	m_bHiColorIcons = TRUE;

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("SysLinker.AppID.NoVersion"));
	m_pTempleMakeLink = nullptr;
	m_pTempleMakePtn = nullptr;
	m_pTemplePSwitch = nullptr;
	m_pTempleSwitch = nullptr;
	m_pTempleLinkView = nullptr;
	m_pTemplePump = nullptr;
	m_pTempleEquip = nullptr;
	m_pTempleUser = nullptr;
	m_pTempleAccess = nullptr;
	m_pTempleEmergency = nullptr;
	m_pTempleLocation = nullptr;
	m_pTempleFacp = nullptr;
	m_pTempleUnit = nullptr;
	m_pTempleAutoMake = nullptr;
	m_pTempleLogicEdit = nullptr;

	m_pFasSysData = nullptr;
	m_spInputEquipManager = nullptr;
	m_spEqNameManager = nullptr;
	m_spOutputEquipManager = nullptr;
	m_spContentsEquipManager = nullptr;
	m_spPumpEquipManager = nullptr;
	m_spPSEquipManager = nullptr;
	m_spPmpNameEquipManager = nullptr;
	m_pMainDb = nullptr;

	m_strPrjRootFolder = L"";
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.

	m_strProgramLoginUser = L"";
	m_bProgramLogin = FALSE;

	m_pDlgProg = nullptr;

	m_bModuleTableChanged = FALSE;
}

// 유일한 CSysLinkerApp 개체입니다.

CSysLinkerApp theApp;


// CSysLinkerApp 초기화

BOOL CSysLinkerApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.

#ifndef SLP4_MODE
	SetRegistryKey(_T("GFS\\FICON3\\SLP3"));
#else
	CString key = _T("");
	key.Format(_T("GFS\\%s\\SLP4"), ROOT_FOLDER);
	SetRegistryKey(key);
#endif

	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	SetSystemPath();

	ConfigLoad();

// 	if (OpenBaseDatabase() == 0)
// 	{
// 		AfxMessageBox(L"데이터베이스에 연결 할 수 없습니다. 환경설정을 다시 설정해 주시기 바람니다.");
// 	}
// 	else
// 		OnHomeLogin();

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	//CMultiDocTemplate* pDocTemplate;
	DocumentTemplateInit();

	// 주 MDI 프레임 창을 만듭니다.
	CMainFrame* pMainFrame = new CMainFrame;
#ifndef ENGLISH_MODE
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
#else
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME_EN))
	{
		delete pMainFrame;
		return FALSE;
	}
#endif
	m_pMainWnd = pMainFrame;


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	m_pTempleMakeLink->OpenDocumentFile(nullptr);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
// 	if (!ProcessShellCommand(cmdInfo))
// 		return FALSE;
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	//20240202 GBM start - 새 기능 클래스 초기화
	CNewInfo::New();
	CString strProgramName = _T("");
	strProgramName = CCommonFunc::GetFileNameOnly(CCommonFunc::GetProgramPath());
	int nProgramNameLen = -1;
	nProgramNameLen = strProgramName.GetLength();
	Log::Setup(strProgramName.Left(nProgramNameLen - 4));
	CNewDBManager::New();
	CNewExcelManager::New();
	CHelpMsgManager::New();
	CWebConnManager::New();
	CWinCredManager::New();
	//20240202 GBM end

	CManualLinkManager::New();	//20250617 GBM - 수동 연동데이터 일괄 편집 기능

	//20260126 GBM start - 여기서 로그인 창을 띄워서 계정 확인 후 계정 확인되면 진행, 그렇지 않으면 FALSE 리턴해서 종료
	if (!WebServerLogin())
	{
		return FALSE;
	}
	//20260126 GBM end

	OnHomeLogin();
	
	return TRUE;
}

int CSysLinkerApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);
	GdiplusShutdown(m_gdiplusToken);

	CloseProject();
	if (m_pMainDb)
	{
		delete m_pMainDb;
		m_pMainDb = nullptr;
	}

// 	if (m_pFasSysData)
// 	{
// 		delete m_pFasSysData;
// 		m_pFasSysData = nullptr;
// 	}
	//RemoveTemplate();

	//20240202 GBM start - 새 기능 클래스 정리
	CNewInfo::Delete();
	CNewDBManager::Delete();
	CNewExcelManager::Delete();
	CHelpMsgManager::Delete();
	CWebConnManager::Delete();
	CWinCredManager::Delete();
	Log::Cleanup();

	//_CrtDumpMemoryLeaks();
	//20240202 GBM end

	CManualLinkManager::Delete();	//20250617 GBM - 수동 연동데이터 일괄 편집 기능

	return CWinAppEx::ExitInstance();
}

// CSysLinkerApp 메시지 처리기


// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CSysLinkerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSysLinkerApp 사용자 지정 로드/저장 메서드

void CSysLinkerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;

#ifndef ENGLISH_MODE
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
#else
	bNameValid = strName.LoadString(IDS_EDIT_MENU_EN);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT_EN);
	bNameValid = strName.LoadString(IDS_EXPLORER_EN);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER_EN);
#endif
}

void CSysLinkerApp::LoadCustomState()
{
}

void CSysLinkerApp::SaveCustomState()
{
}

// CSysLinkerApp 메시지 처리기






//void CAboutDlg::OnSortAddress()
//{
//	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//}


int CSysLinkerApp::SetSystemPath()
{
	CString strPath;

	TCHAR pszPathName[_MAX_PATH];
	::GetModuleFileName(::AfxGetInstanceHandle(), pszPathName, _MAX_PATH);
	PathRemoveFileSpec(pszPathName);

	g_strAppPath = pszPathName;
	if (g_strAppPath.Right(1) != L'\\')
		g_strAppPath += L'\\';
	return 0;
}


int CSysLinkerApp::DocumentTemplateInit()
{
#ifndef ENGLISH_MODE
	m_pTempleMakeLink = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CSysLinkerDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CSysLinkerView));
#else
	m_pTempleMakeLink = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CSysLinkerDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CSysLinkerView));
#endif
	if (!m_pTempleMakeLink)
		return FALSE;
	AddDocTemplate(m_pTempleMakeLink);

#ifndef ENGLISH_MODE
	m_pTempleMakePtn = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormPattern));
#else
	m_pTempleMakePtn = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormPattern));
#endif
	if (!m_pTempleMakePtn)
		return FALSE;
	AddDocTemplate(m_pTempleMakePtn);

#ifndef ENGLISH_MODE
	m_pTemplePSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormPSwitch));
#else
	m_pTemplePSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormPSwitch));
#endif
	if (!m_pTemplePSwitch)
		return FALSE;
	AddDocTemplate(m_pTemplePSwitch);

#ifndef ENGLISH_MODE
	m_pTempleSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormSwitch));
#else
	m_pTempleSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormSwitch));
#endif
	if (!m_pTempleSwitch)
		return FALSE;
	AddDocTemplate(m_pTempleSwitch);

#ifndef ENGLISH_MODE
	m_pTempleLinkView = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLinkView));
#else
	m_pTempleLinkView = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLinkView));
#endif
	if (!m_pTempleLinkView)
		return FALSE;
	AddDocTemplate(m_pTempleLinkView);
// 
// #ifndef ENGLISH_MODE
// 	m_pTemplePump = new CMultiDocTemplate(IDR_SysLinkerTYPE,
// 		RUNTIME_CLASS(CFormDoc),
// 		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
// 		RUNTIME_CLASS(CFormPump));
// #else
// 	m_pTemplePump = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
// 		RUNTIME_CLASS(CFormDoc),
// 		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
// 		RUNTIME_CLASS(CFormPump));
// #endif

#ifndef ENGLISH_MODE
	m_pTemplePump = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.

		//20251219 GBM start - 테스트를 위한 기존 펌프 화면 로드 -> 원복
		RUNTIME_CLASS(CFormPumpEdit));
		//RUNTIME_CLASS(CFormPump));
		//20251219 GBM end
#else
	m_pTemplePump = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormPumpEdit));
#endif

	if (!m_pTemplePump)
		return FALSE;
	AddDocTemplate(m_pTemplePump);

#ifndef ENGLISH_MODE
	m_pTempleEquip = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEquip));
#else
	m_pTempleEquip = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEquip));
#endif
	if (!m_pTempleEquip)
		return FALSE;
	AddDocTemplate(m_pTempleEquip);

#ifndef ENGLISH_MODE
	m_pTempleUser = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormUser));
#else
	m_pTempleUser = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormUser));
#endif
	if (!m_pTempleUser)
		return FALSE;
	AddDocTemplate(m_pTempleUser);

#ifndef ENGLISH_MODE
	m_pTempleAccess = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormAccess));
#else
	m_pTempleAccess = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormAccess));
#endif
	if (!m_pTempleAccess)
		return FALSE;
	AddDocTemplate(m_pTempleAccess);

#ifndef ENGLISH_MODE
	m_pTempleEmergency = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEmergency));
#else
	m_pTempleEmergency = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEmergency));
#endif
	if (!m_pTempleEmergency)
		return FALSE;
	AddDocTemplate(m_pTempleEmergency);

#ifndef ENGLISH_MODE
	m_pTempleLocation = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLocation));
#else
	m_pTempleLocation = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLocation));
#endif
	if (!m_pTempleLocation)
		return FALSE;
	AddDocTemplate(m_pTempleLocation);

#ifndef ENGLISH_MODE
	m_pTempleFacp = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormFacp));
#else
	m_pTempleFacp = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormFacp));
#endif
	if (!m_pTempleFacp)
		return FALSE;
	AddDocTemplate(m_pTempleFacp);

#ifndef ENGLISH_MODE
	m_pTempleUnit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormUnit));
#else
	m_pTempleUnit = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormUnit));
#endif
	if (!m_pTempleUnit)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleRelayEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormRelayEdit));
#else
	m_pTempleRelayEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormRelayEdit));
#endif
	if (!m_pTempleRelayEdit)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleAutoMake = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormAutoMake));
#else
	m_pTempleAutoMake = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormAutoMake));
#endif
	if (!m_pTempleAutoMake)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleLogicEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEditLogic));
#else
	m_pTempleLogicEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEditLogic));
#endif
	if (!m_pTempleLogicEdit)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleLoadRealy = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLoadRelayTable));
#else
	m_pTempleLoadRealy = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLoadRelayTable));
#endif
	if (!m_pTempleLoadRealy)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleErrorCheck = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormErrorCheck));
#else
	m_pTempleErrorCheck = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormErrorCheck));
#endif
	if(!m_pTempleErrorCheck)
		return FALSE;

	AddDocTemplate(m_pTempleUnit);

	//20240909 GBM start - 메모리 누수 오류 수정 : 누락 코드 반영
	AddDocTemplate(m_pTempleRelayEdit);
	AddDocTemplate(m_pTempleAutoMake);
	AddDocTemplate(m_pTempleLogicEdit);
	AddDocTemplate(m_pTempleLoadRealy);
	AddDocTemplate(m_pTempleErrorCheck);
	//20240909 GBM end

	return 0;
}

CView* CSysLinkerApp::OpenFormView(FormViewStyle iStyle)
{
	CString strTitle = L"";
	CMultiDocTemplate* pDocTemp = NULL;
	CView* pView = NULL;
	if (iStyle == FV_MAKELINKER)
	{
		pDocTemp = m_pTempleMakeLink;
#ifndef ENGLISH_MODE
		strTitle = L"연동데이터 편집";
#else
		strTitle = L"EDIT SITE LOGIC DATA";
#endif
	}
	else if (iStyle == FV_MAKEPATTERN)
	{
		pDocTemp = m_pTempleMakePtn;
#ifndef ENGLISH_MODE
		strTitle = L"패턴데이터 편집";
#else
		strTitle = L"EDIT PATTERN DATA";
#endif
	}
	else if (iStyle == FV_PSWITCH)
	{
		pDocTemp = m_pTemplePSwitch;
#ifndef ENGLISH_MODE
		strTitle = L"압력스위치 편집";
#else
		strTitle = L"EDIT PRESSURE SWITCH";
#endif
	}
	else if (iStyle == FV_SWITCH)
	{
		pDocTemp = m_pTempleSwitch;
#ifndef ENGLISH_MODE
		strTitle = L"스위치 편집";
#else
		strTitle = L"EDIT SWITCH";
#endif
	}
	else if (iStyle == FV_LINKVIEW)
	{
		pDocTemp = m_pTempleLinkView;
#ifndef ENGLISH_MODE
		strTitle = L"연동데이터 보기";
#else
		strTitle = L"VIEW SITE LOGIC DATA";
#endif
	}
	else if (iStyle == FV_PUMP)
	{
		pDocTemp = m_pTemplePump;
#ifndef ENGLISH_MODE
		strTitle = L"펌프데이터 편집";
#else
		strTitle = L"EDIT PUMP DATA";
#endif
	}
	else if (iStyle == FV_EQUIP)
	{
		pDocTemp = m_pTempleEquip;
#ifndef ENGLISH_MODE
		strTitle = L"설비정보 편집";
#else
		strTitle = L"EDIT EQUIPMENT INFORMATION";
#endif
	}
	else if (iStyle == FV_USER)
	{
		pDocTemp = m_pTempleUser;
#ifndef ENGLISH_MODE
		strTitle = L"사용자/사용자그룹 편집";
#else
		strTitle = L"EDIT USER/USER GROUP";
#endif
	}
	else if (iStyle == FV_ACCESS)
	{
		pDocTemp = m_pTempleAccess;
#ifndef ENGLISH_MODE
		strTitle = L"Access 권한 편집";
#else
		strTitle = L"EDIT ACCESS PERMISSION";
#endif
	}
	else if (iStyle == FV_EMERGENCY)
	{
		pDocTemp = m_pTempleEmergency;
#ifndef ENGLISH_MODE
		strTitle = L"비상방송 편집";
#else
		strTitle = L"EDIT PUBLIC ADDRESS";
#endif
	}
	else if (iStyle == FV_LOCATION)
	{
		pDocTemp = m_pTempleLocation;
#ifndef ENGLISH_MODE
		strTitle = L"위치정보 편집";
#else
		strTitle = L"EDIT GEOLOCATION INFORMATION";
#endif
	}
	else if (iStyle == FV_FACP)
	{
		pDocTemp = m_pTempleFacp;
#ifndef ENGLISH_MODE
		strTitle = L"수신기정보 편집";
#else
		strTitle = L"EDIT FACP INFORMATION";
#endif
	}
	else if (iStyle == FV_UNIT)
	{
		pDocTemp = m_pTempleUnit;
#ifndef ENGLISH_MODE
		strTitle = L"유닛정보 편집";
#else
		strTitle = L"EDIT UNIT INFORMATION";
#endif
	}
	else if (iStyle == FV_RELAYEDIT)
	{
		pDocTemp = m_pTempleRelayEdit;
#ifndef ENGLISH_MODE
		strTitle = L"회로 편집";
#else
		strTitle = L"EDIT CIRCUIT";
#endif
	}
	else if (iStyle == FV_AUTOMAKE)
	{
		pDocTemp = m_pTempleAutoMake;
#ifndef ENGLISH_MODE
		strTitle = L"연동데이터 자동생성";
#else
		strTitle = L"AUTOGENERATE SITE LOGIC DATA";
#endif
	}
	else if (iStyle == FV_LOGICEDIT)
	{
		pDocTemp = m_pTempleLogicEdit;
#ifndef ENGLISH_MODE
		strTitle = L"연동데이터 자동생성 로직";
#else
		strTitle = L"SITE LOGIC DATA AUTOGENERATION LOGIC";
#endif
	}
	else if (iStyle == FV_LOADRELAYTABLE)
	{
		pDocTemp = m_pTempleLoadRealy;
#ifndef ENGLISH_MODE
		strTitle = L"중계기 일람표";
#else
		strTitle = L"MODULE TABLE";
#endif
	}
	else if(iStyle == FV_ERRORCHECK)
	{
		pDocTemp = m_pTempleErrorCheck;
#ifndef ENGLISH_MODE
		strTitle = L"오류 검사";
#else
		strTitle = L"CHECK FOR ERROR";
#endif
	}

	if (pDocTemp == NULL)
		return NULL;
	if (GetSysLinkerDoc(pDocTemp) == NULL)
	{
		CDocument* pDoc = pDocTemp->OpenDocumentFile(NULL);
		if (pDoc)
			pDoc->SetTitle(strTitle);
	}
	pView = GetSysLinkerView(pDocTemp);
	CChildFrame * pChild = nullptr;
	if (pView != NULL)
	{
		pChild = (CChildFrame*)pView->GetParent();
		if(pChild)
		{ 
			pChild->SetWindowTextW(strTitle);
			SetActiveView(pChild);
			return pView;
		}
		
// 		(pView->GetParent())->SetActiveView(pView);
// 		pView->SetFocus();
	}

	
	
	return pView;
}
BOOL CSysLinkerApp::PostMessageAllView(UINT uMsg, WPARAM wp, LPARAM lp)
{
	::PostMessage(HWND_BROADCAST, uMsg, wp, lp);
	PostMessageFormView(FV_MAKELINKER, uMsg,wp, lp);
	PostMessageFormView(FV_MAKEPATTERN, uMsg, wp, lp);
	PostMessageFormView(FV_PSWITCH, uMsg, wp, lp);
	PostMessageFormView(FV_SWITCH, uMsg, wp, lp);
	PostMessageFormView(FV_LINKVIEW, uMsg, wp, lp);
	PostMessageFormView(FV_PUMP, uMsg, wp, lp);
	PostMessageFormView(FV_EQUIP, uMsg, wp, lp);
	PostMessageFormView(FV_USER, uMsg, wp, lp);
	PostMessageFormView(FV_ACCESS, uMsg, wp, lp);
	PostMessageFormView(FV_EMERGENCY, uMsg, wp, lp);
	PostMessageFormView(FV_LOCATION, uMsg, wp, lp);
	PostMessageFormView(FV_FACP, uMsg, wp, lp);
	PostMessageFormView(FV_UNIT, uMsg, wp, lp);
	PostMessageFormView(FV_RELAYEDIT, uMsg, wp, lp);
	PostMessageFormView(FV_AUTOMAKE, uMsg, wp, lp);
	PostMessageFormView(FV_LOGICEDIT, uMsg, wp, lp);
	PostMessageFormView(FV_LOGICEDIT, uMsg, wp, lp);
	PostMessageFormView(FV_LOADRELAYTABLE, uMsg, wp, lp);
	return TRUE;
}
BOOL CSysLinkerApp::PostMessageFormView(FormViewStyle iStyle, UINT uMsg, WPARAM wp, LPARAM lp)
{
	CString strTitle = L"";
	CMultiDocTemplate* pDocTemp = NULL;
	CView* pView = NULL;
	if (iStyle == FV_MAKELINKER)
	{
		pDocTemp = m_pTempleMakeLink;
	}
	else if (iStyle == FV_MAKEPATTERN)
	{
		pDocTemp = m_pTempleMakePtn;
	}
	else if (iStyle == FV_PSWITCH)
	{
		pDocTemp = m_pTemplePSwitch;
	}
	else if (iStyle == FV_SWITCH)
	{
		pDocTemp = m_pTempleSwitch;
	}
	else if (iStyle == FV_LINKVIEW)
	{
		pDocTemp = m_pTempleLinkView;
	}
	else if (iStyle == FV_PUMP)
	{
		pDocTemp = m_pTemplePump;
	}
	else if (iStyle == FV_EQUIP)
	{
		pDocTemp = m_pTempleEquip;
	}
	else if (iStyle == FV_USER)
	{
		pDocTemp = m_pTempleUser;
	}
	else if (iStyle == FV_ACCESS)
	{
		pDocTemp = m_pTempleAccess;
	}
	else if (iStyle == FV_EMERGENCY)
	{
		pDocTemp = m_pTempleEmergency;
	}
	else if (iStyle == FV_LOCATION)
	{
		pDocTemp = m_pTempleLocation;
	}
	else if (iStyle == FV_FACP)
	{
		pDocTemp = m_pTempleFacp;
	}
	else if (iStyle == FV_UNIT)
	{
		pDocTemp = m_pTempleUnit;
	}
	else if (iStyle == FV_RELAYEDIT)
	{
		pDocTemp = m_pTempleRelayEdit;
	}
	else if (iStyle == FV_AUTOMAKE)
	{
		pDocTemp = m_pTempleAutoMake;
	}
	else if (iStyle == FV_LOGICEDIT)
	{
		pDocTemp = m_pTempleLogicEdit;
	}
	else if (iStyle == FV_LOADRELAYTABLE)
	{
		pDocTemp = m_pTempleLoadRealy;
	}
	else if(iStyle == FV_ERRORCHECK)
	{
		pDocTemp = m_pTempleErrorCheck;
	}
	else
		return FALSE;

	pView = GetSysLinkerView(pDocTemp);
	if (pView == nullptr)
		return FALSE;
	pView->PostMessage(uMsg, wp, lp);
	return TRUE;
}


BOOL CSysLinkerApp::CloseFormView(FormViewStyle iStyle)
{
	CString strTitle = L"";
	CMultiDocTemplate* pDocTemp = NULL;
	CView* pView = NULL;
	if (iStyle == FV_MAKELINKER)
	{
		pDocTemp = m_pTempleMakeLink;
	}
	else if (iStyle == FV_MAKEPATTERN)
	{
		pDocTemp = m_pTempleMakePtn;
	}
	else if (iStyle == FV_PSWITCH)
	{
		pDocTemp = m_pTemplePSwitch;
	}
	else if (iStyle == FV_SWITCH)
	{
		pDocTemp = m_pTempleSwitch;
	}
	else if (iStyle == FV_LINKVIEW)
	{
		pDocTemp = m_pTempleLinkView;
	}
	else if (iStyle == FV_PUMP)
	{
		pDocTemp = m_pTemplePump;
	}
	else if (iStyle == FV_EQUIP)
	{
		pDocTemp = m_pTempleEquip;
	}
	else if (iStyle == FV_USER)
	{
		pDocTemp = m_pTempleUser;
	}
	else if (iStyle == FV_ACCESS)
	{
		pDocTemp = m_pTempleAccess;
	}
	else if (iStyle == FV_EMERGENCY)
	{
		pDocTemp = m_pTempleEmergency;
	}
	else if (iStyle == FV_LOCATION)
	{
		pDocTemp = m_pTempleLocation;
	}
	else if (iStyle == FV_FACP)
	{
		pDocTemp = m_pTempleFacp;
	}
	else if (iStyle == FV_UNIT)
	{
		pDocTemp = m_pTempleUnit;
	}
	else if (iStyle == FV_RELAYEDIT)
	{
		pDocTemp = m_pTempleRelayEdit;
	}
	else if (iStyle == FV_AUTOMAKE)
	{
		pDocTemp = m_pTempleAutoMake;
	}
	else if (iStyle == FV_LOGICEDIT)
	{
		pDocTemp = m_pTempleLogicEdit;
	}
	else if (iStyle == FV_LOADRELAYTABLE)
	{
		pDocTemp = m_pTempleLoadRealy;
	}
	else if(iStyle == FV_ERRORCHECK)
	{
		pDocTemp = m_pTempleErrorCheck;
	}
	else
		return FALSE;

	pView = GetSysLinkerView(pDocTemp);
	CChildFrame * pChild = nullptr;
	if (pView != NULL)
	{
		pChild = (CChildFrame*)pView->GetParent();
		if (pChild)
		{
			pChild->SetWindowTextW(strTitle);
			pChild->PostMessage(WM_CLOSE, 0, 0);
		}
	}
	return TRUE;
}


int CSysLinkerApp::SetActiveView(CChildFrame * pChild)
{
	CMainFrame * pMainWnd = (CMainFrame*)AfxGetMainWnd();
	//pMainWnd->SetActiveFrame(pView);
	CMFCTabCtrl* tab;
	int nCnt = 0,nIdx = 0;
	const CObList& TabGrps = pMainWnd->GetMDITabGroups();
	POSITION pos = TabGrps.GetHeadPosition();
	while (pos)
	{
		tab = (CMFCTabCtrl*)TabGrps.GetNext(pos);
		if (tab == nullptr)
			continue;
		nIdx = tab->GetTabFromHwnd(pChild->GetSafeHwnd());
		if (nIdx >= 0)
		{
			tab->ActivateMDITab(nIdx);
			return nIdx;
		}
	}
	return -1;
}

// Get the Active Station Document
CDocument* CSysLinkerApp::GetSysLinkerDoc(CMultiDocTemplate* pDocTemp)
{
	if (pDocTemp == NULL)
		return NULL;
	POSITION Pos = pDocTemp->GetFirstDocPosition();
	if (Pos == NULL)
		return NULL;
	return pDocTemp->GetNextDoc(Pos);
}

// Get the Active Station View
CView* CSysLinkerApp::GetSysLinkerView(CMultiDocTemplate* pDocTemp)
{
	CDocument* pDoc = GetSysLinkerDoc(pDocTemp);
	if (pDoc == NULL)
		return NULL;
	POSITION Pos = pDoc->GetFirstViewPosition();
	if (Pos == NULL)
		return NULL;
	return pDoc->GetNextView(Pos);
}


void CSysLinkerApp::OnHomeLogin()
{
	if (m_pMainDb == nullptr || m_pMainDb->IsOpen() == FALSE)
		OpenBaseDatabase();

	//20240306 GBM start - 무의미한 로그인 삭제
#if 0
	CDlgProgramLogin dlg(m_pMainDb);
	if (dlg.DoModal() != IDOK)
		return; 
	m_strProgramLoginUser = dlg.m_strUser;
	GF_AddLog(L"프로그램 로그인에 성공했습니다.");
	AfxMessageBox(L"프로그램 로그인에 성공했습니다.");
#endif
	//20240306 GBM end

	m_bProgramLogin = TRUE;
	//OpenBaseDatabase();
	//m_pMainDb->RestoreDatabase(L"연동", L"D:\\04. Ficon3\\51. Project\\연동", L"연동.bak");
// 	CDlgLogIn dlg(m_pMainDb);
// 
// 	if (dlg.DoModal() != IDOK)
// 		return;


// 	// 새프로젝트
// 	if (dlg.GetSelectProjectType() == 0)
// 	{
// 		OnHomeProjectNew();
// 		return;
// 	}
// 	

	// 이전 프로젝트 


	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 방식2 : DSN 이용안함, 대신 Server명, Port등을 지정해 주어야 한다.
	//MySQL ODBC Driver를 사용해도 작동함    conn.ConnectionString = "Driver={MySQL ODBC 5.3 Unicode Driver};Server=192.168.65.128;Port=3306;Database=test;User=root;Password=x1234;Option=2;"
	//"Driver={MariaDB ODBC 3.1 Driver};Server=192.168.65.128;Port=3306;Database=test;User=root;Password=x1234;Option=2;"
	// 	CString strCon;
	// 	strCon.Format(L"Driver={MariaDB ODBC 3.1 Driver};Server=%s;Port=%d;"
	// 		L"Database=%s;User=%s;Password=%s;Option=2;"
	// 		, L"127.0.0.1", 3306
	// 		, L"gfs_base", L"root", L"rnd@GFS111"
	// 	);
	// 
	// 	YAdoDatabase db;
	// 	//db.ODBCInitialize(L"FICON3", L"gfs_base", L"root", L"rnd@GFS111");
	// 	db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	if (db.DBOpen() == FALSE)
	// 	{
	// 		//AfxMessageBox(L"오류");
	// 	}
	// 	else
	// 	{
	// 		AfxMessageBox(L"접속1");
	// 	}
	// 	db.DBClose();
	// 
	// 	db.ODBCInitialize(L"FICON3", L"gfs_base", L"root", L"rnd@GFS111");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"한글데이터베이스", L"127.0.0.1");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	if (db.DBOpen() == FALSE)
	// 	{
	// 		//AfxMessageBox(L"오류");
	// 	}
	// 	else
	// 	{
	// 		AfxMessageBox(L"접속2");
	// 	}
	// 	db.DBClose();


	// 
	// 	YAdoDatabase db;
	// 	db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	if (db.DBOpen() == FALSE)
	// 	{
	// 	 	//AfxMessageBox(L"오류");
	// 	}
	// 	else
	// 	{
	// 	 	//AfxMessageBox(L"접속1");
	// 	}
	// 	CString strSql;
	// 	strSql.Format(L"CREATE DATABASE %s", L"제기동GFS");
	// 	if (db.ExecuteSql(strSql) == FALSE)
	// 	{
	// 		AfxMessageBox(L"데이터베이스를 생성하는데 실패 했습니다.");
	// 	}
	// 
	// 	db.DBClose();
	// 
	// 	db.MySqlInitialize(L"rnd@GFS111", L"root", L"제기동GFS", L"127.0.0.1");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	if (db.DBOpen() == FALSE)
	// 	{
	// 		//AfxMessageBox(L"오류");
	// 	}
	// 	else
	// 	{
	// 		//AfxMessageBox(L"접속1");
	// 	}
	// 
	// 
	// 	
	// 	CString strFile;
	// 	CString strFindCriteria;
	// 	CFileFind find;
	// 	strFindCriteria.Format(L"%sDB\\*.sql", g_strAppPath);
	// 	BOOL bResult = find.FindFile(strFindCriteria);
	// 	while (bResult)
	// 	{
	// 		bResult = find.FindNextFile();
	// 		strFile = find.GetFilePath();
	// 		std::wstring wSql = LoadUtf8FileToString(strFile.operator LPCWSTR());
	// 		strSql = wSql.c_str();
	// 		db.ExecuteSql(strSql);
	// 
	// 	}
	// 	db.DBClose();

}


void CSysLinkerApp::OnHomeProjectClose()
{
	//////////////////////////////////////////////////////////////////////////
	// 1. 프로젝트 저장
	// 2. 화면 초기화
	// 3. DB분리 , 데이터 할당 해제
	// 4. 데이터 삭제
#ifndef ENGLISH_MODE
	if (AfxMessageBox(L"현재 프로젝트를 닫으시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
#else
	if (AfxMessageBox(L"Do you want to close the current project?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
#endif

	//20251125 GBM start - 만약 로직 편집기가 열려 있다면 죽임
	CString strLogicEditor = _T("LogicEditor.exe");
#ifndef ENGLISH_MODE
	CString strLogicEditorCaption = L"로직 편집기";
#else
	CString strLogicEditorCaption = L"Logic Editor";
#endif
	HWND hwnd = FindWindow(NULL, strLogicEditorCaption);
	if (hwnd != nullptr)
	{
		CCommonFunc::KillProcess(strLogicEditor);
	}
	//20251125 GBM end

	//20240527 GBM start - 스레드로 전환
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("프로젝트를 닫는 중입니다. 잠시 기다려 주세요.");
#else
	CString strMsg = _T("Closing the project. Wait for a moment.");
#endif
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadCloseProject, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트를 닫는 데에 성공했습니다.");
#else
		GF_AddLog(L"Successfully closed the project.");
#endif
		Log::Trace("The project was successfully closed.");
	}
	else
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트를 닫는 데에 실패했습니다.");
#else
		GF_AddLog(L"Failed to close the project.");
#endif
		Log::Trace("Failed to close the project.");
	}

	m_pProgressBarDlg = nullptr;

	CString strName = _T("");
	AfxGetMainWnd()->SetWindowTextW(strName);
#else
	CloseProject();

	CString strName = _T("");
	AfxGetMainWnd()->SetWindowTextW(strName);

	GF_AddLog(L"프로젝트를 닫았습니다.");
	Log::Trace("Project Closed!");
#endif

	//20240527 GBM end
}

void CSysLinkerApp::OnHomeProjectNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CDlgNewProjectWizard dlg;

	if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
	{
#ifndef ENGLISH_MODE
		if (AfxMessageBox(L"새 프로젝트를 생성하시려면 현재 프로젝트를 닫아야 합니다.\n현재 프로젝트를 닫으시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#else
		if (AfxMessageBox(L"To create a new project, you need to close the current project.\nDo you want to close the current project? ", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#endif

		//20240527 GBM start - 스레드로 전환
#if 1
		m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
		CString strMsg = _T("프로젝트를 닫는 중입니다. 잠시 기다려 주세요.");
#else
		CString strMsg = _T("Closing the project. Wait for a moment.");
#endif
		CProgressBarDlg dlg(strMsg);
		m_pProgressBarDlg = &dlg;

		CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadCloseProject, this);
		dlg.DoModal();

		DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
		if (dw != WAIT_OBJECT_0)
		{
			Log::Trace("스레드 대기 실패! dw : %d", dw);
		}

		if (m_bThreadSucceeded)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"프로젝트를 닫는 데에 성공했습니다.");
#else
			GF_AddLog(L"Successfully closed the project.");
#endif
			Log::Trace("The project was successfully closed.");
		}
		else
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"프로젝트를 닫는 데에 실패했습니다.");
#else
			GF_AddLog(L"Failed to close the project.");
#endif
			Log::Trace("Failed to close the project.");
		}

		m_pProgressBarDlg = nullptr;

		CString strName = _T("");
		AfxGetMainWnd()->SetWindowTextW(strName);
		//
#else
		CloseProject();

		CString strName = _T("");
		AfxGetMainWnd()->SetWindowTextW(strName);
 		GF_AddLog(L"프로젝트를 닫았습니다.");
 		Log::Trace("Project Closed!");
#endif
		//20240527 GBM end
	}

	//20240527 GBM start - 스레드로 전환
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("기본 설비 정의를 로드 중입니다. 잠시 기다려 주세요.");
#else
	CString strMsg = _T("Loading the default equipment definition. Wait for a moment.");
#endif
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadGetDefaultEquipmentDefinition, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"기본 설비 정의 로드에 성공했습니다.");
#else
		GF_AddLog(L"Successfully loaded the default equipment definition.");
#endif
		Log::Trace("Loading of the default equipment definition was successful.");
	}

	m_pProgressBarDlg = nullptr;
	//20240527 GBM end

	CPropSheetNewProject psNewProject(L"New Project");
	if (m_pFasSysData == nullptr)
	{
		m_pFasSysData = new CRelayTableData;
	}

	//20240408 GBM start - 여기서는 아래를 진행하지 않고 중계기 일람표 경로가 확정되어 파일을 열어 설비 정의를 기본 DB에 적용 후 거기서 아래 내용을 진행해 적용하도록 함
#if 0
	if (m_spInputEquipManager)
		m_spInputEquipManager->RemoveAllEquip();
	if (m_spEqNameManager)
		m_spEqNameManager->RemoveAllEquip();
	if (m_spOutputEquipManager)
		m_spOutputEquipManager->RemoveAllEquip();
	if (m_spContentsEquipManager)
		m_spContentsEquipManager->RemoveAllEquip();
	if (m_spPumpEquipManager)
		m_spPumpEquipManager->RemoveAllEquip();
	if (m_spPSEquipManager)
		m_spPSEquipManager->RemoveAllEquip();
	if (m_spPmpNameEquipManager)
		m_spPmpNameEquipManager->RemoveAllEquip();

	if (m_spInputEquipManager == nullptr && m_spOutputEquipManager == nullptr
		&& m_spContentsEquipManager == nullptr && m_spPumpEquipManager == nullptr
		&& m_spPSEquipManager == nullptr && m_spEqNameManager == nullptr)
	{
		LoadEquipBaseData();
	}
	m_pFasSysData->InitFasSysData(m_spInputEquipManager, m_spEqNameManager
		, m_spOutputEquipManager , m_spContentsEquipManager
		, m_spPumpEquipManager, m_spPSEquipManager
		, m_spPmpNameEquipManager
	);
#endif
	//20240408 GBM end

	psNewProject.SetFasSysData(m_pFasSysData);
	psNewProject.InitPage();

	INT_PTR nRet = psNewProject.DoModal();
	if (nRet != ID_WIZFINISH)
	{
		delete m_pFasSysData;
		m_pFasSysData = nullptr;
		return;
	}
	m_pFasSysData = psNewProject.GetFasSysData();

	//20240523 GBM start - 아래 행정을 스레드로 대체
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	strMsg = _T("        새 프로젝트를 생성 중입니다. 잠시 기다려 주세요.");
#else
	strMsg = _T("        Creating a new project. Wait for a moment.");
#endif
	CProgressBarDlg dlg2(strMsg);
	m_pProgressBarDlg = &dlg2;

	CWinThread* pThread2 = AfxBeginThread((AFX_THREADPROC)ThreadCreateNewProject, this);
	dlg2.DoModal();

	DWORD dw2 = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw2 != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"새 프로젝트 생성에 성공했습니다.");
#else
		GF_AddLog(L"Successfully created a new project.");
#endif
		Log::Trace("New project creation was successful.");
	}
	else
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"새 프로젝트 생성에 실패했습니다.");
#else
		GF_AddLog(L"Failed to create a new project.");
#endif
		Log::Trace("New project creation was failed.");
	}

	CMainFrame * pMainWnd = nullptr;
	pMainWnd = (CMainFrame *)AfxGetMainWnd();

	if (pMainWnd)
		pMainWnd->SetRelayTable(m_pFasSysData);

	CSysLinkerView * pMakLinkView = (CSysLinkerView*)GetSysLinkerView(m_pTempleMakeLink);
	pMakLinkView->SetRelayTableData(m_pFasSysData);
	m_pFasSysData->SetProjectOpened(TRUE);

	CString strName;
	CString strVersion;
	strVersion.Format(_T("(ver%d.%d)"), m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum());
#ifndef ENGLISH_MODE
	strName.Format(L"프로젝트명 - %s %s", m_pFasSysData->GetPrjName(), strVersion);
#else
	strName.Format(L"Project Name - %s %s", m_pFasSysData->GetPrjName(), strVersion);
#endif
	AfxGetMainWnd()->SetWindowTextW(strName);

#ifndef ENGLISH_MODE
	GF_AddLog(L"[%s %s] 프로젝트를 생성했습니다.", m_pFasSysData->GetPrjName(), strVersion);
#else
	GF_AddLog(L"You have created the project [%s].", m_pFasSysData->GetPrjName(), strVersion);
#endif
	Log::Trace("[%s %s] Project Created!", CCommonFunc::WCharToChar(m_pFasSysData->GetPrjName().GetBuffer(0)));

	PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_NEW, 0);

#else
	if (m_pFasSysData == nullptr)
	{
		AfxMessageBox(L"프로젝트 생성에 실패 했습니다.");
		CloseProject();
		return;
	}

	/************************************************************************/
	/*
	1. Project Path 설정 :
	- 폴더 생성
	- File 생성
	2. Database 생성
	- Database 생성
	- Table 및 데이터 생성
	3. View 설정
	- DialogBar : Input , output ,

	*/
	/************************************************************************/
	if(CreateProject() <= 0)
	{
		AfxMessageBox(L"프로젝트를 생성하는데 실패 했습니다.");
		CloseProject();
		return ;
	}

	CMainFrame * pMainWnd = nullptr;
	pMainWnd = (CMainFrame *)AfxGetMainWnd();
	
	if (pMainWnd)
		pMainWnd->SetRelayTable(m_pFasSysData);

	CSysLinkerView * pMakLinkView = (CSysLinkerView*)GetSysLinkerView(m_pTempleMakeLink);
	pMakLinkView->SetRelayTableData(m_pFasSysData);
	m_pFasSysData->SetProjectOpened(TRUE);

	CString strName;
	strName.Format(L"프로젝트명 - %s", m_pFasSysData->GetPrjName());
	AfxGetMainWnd()->SetWindowTextW(strName);

	GF_AddLog(L"[%s] 프로젝트를 생성했습니다.", m_pFasSysData->GetPrjName());
	Log::Trace("[%s] Project Created!", CCommonFunc::WCharToChar(m_pFasSysData->GetPrjName().GetBuffer(0)));

	PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_NEW, 0);

#endif
	//20240523 GBM end

	//20250804 GBM start - 도움말 파일 로드 : 프로그램 실행 시 최초 한번 실행
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->GetHelpMessage();
	//20250804 GBM end
}

void CSysLinkerApp::OnHomeProjectOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
	{
#ifndef ENGLISH_MODE
		if (AfxMessageBox(L"프로젝트 열기를 하시려면 현재 프로젝트를 닫아야 합니다.\n현재 프로젝트를 닫으시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#else
		if (AfxMessageBox(L"To open a project, you need to close the current project.\nDo you want to close the current project ? ", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#endif

		//20240527 GBM start - 스레드로 전환
#if 1
		m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
		CString strMsg = _T("프로젝트를 닫는 중입니다. 잠시 기다려 주세요.");
#else
		CString strMsg = _T("Closing the project. Wait for a moment.");
#endif
		CProgressBarDlg dlg(strMsg);
		m_pProgressBarDlg = &dlg;

		CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadCloseProject, this);
		dlg.DoModal();

		DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
		if (dw != WAIT_OBJECT_0)
		{
			Log::Trace("스레드 대기 실패! dw : %d", dw);
		}

		if (m_bThreadSucceeded)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"프로젝트를 닫는 데에 성공했습니다.");
#else
			GF_AddLog(L"Successfully closed the project.");
#endif
			Log::Trace("The project was successfully closed.");
		}
		else
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"프로젝트를 닫는 데에 실패했습니다.");
#else
			GF_AddLog(L"Failed to close the project.");
#endif
			Log::Trace("Failed to close the project.");
		}

		m_pProgressBarDlg = nullptr;

		CString strName = _T("");
		AfxGetMainWnd()->SetWindowTextW(strName);
		//
#else
		CloseProject();

		CString strName = _T("");
		AfxGetMainWnd()->SetWindowTextW(strName);
		GF_AddLog(L"프로젝트를 닫았습니다.");
		Log::Trace("Project Closed!");
#endif
		//20240527 GBM end
	}

	CDlgLogIn dlg(m_pMainDb);

	if (dlg.DoModal() != IDOK)
		return;
	//CloseProject();
	COleDateTime dtCur;
	DWORD_PTR dwStart, dwEnd;
	dwStart = GetTickCount();
	dtCur = COleDateTime::GetCurrentTime();
#ifndef ENGLISH_MODE
	GF_AddDebug(L"Start 시간 : %s", dtCur.Format(L"%H:%M:%S"));
#else
	GF_AddDebug(L"Start Time : %s", dtCur.Format(L"%H:%M:%S"));
#endif
	//20240730 GBM start - 별도의 DB가 아닌 프로젝트 정보 파일로 프로젝트 정보를 넣음
	memset(&CNewInfo::Instance()->m_gi.projectInfo, NULL, sizeof(PROJECT_INFO));
	sprintf(CNewInfo::Instance()->m_gi.projectInfo.projectName, CCommonFunc::WCharToChar(dlg.GetOpenProjectName().GetBuffer(0)));
	CString strVersion, strMajor, strMinor;
	strVersion = dlg.GetOpenProjectVersionString();
	AfxExtractSubString(strMajor, strVersion, 0, '.');
	AfxExtractSubString(strMinor, strVersion, 1, '.');
	CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum = _wtoi(strMajor);
	CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum = _wtoi(strMinor);
	CNewInfo::Instance()->m_gi.projectInfo.authorized = false;
	//20240730 GBM end

	if (OpenProject(dlg.GetOpenProjectName(), dlg.GetOpenProjectPath()
		, dlg.GetOpenProjectVersion(), dlg.IsSelectedVersionTemp()) <= 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"[%s] 프로젝트를 여는데 실패 했습니다.", dlg.GetOpenProjectName());
#else
		GF_AddLog(L"Failed to open the [%s] project.", dlg.GetOpenProjectName());
#endif
		return;
	}

	m_pFasSysData->SetCurrentUser(dlg.GetLogInUser());
	CMainFrame * pMainWnd = nullptr;
	pMainWnd = (CMainFrame *)AfxGetMainWnd();

	if (pMainWnd)
		pMainWnd->SetRelayTable(m_pFasSysData);

	CSysLinkerView * pMakLinkView = (CSysLinkerView*)GetSysLinkerView(m_pTempleMakeLink);
	if (pMakLinkView)
		pMakLinkView->SetRelayTableData(m_pFasSysData);

	PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_OPEN, 0);

	CString strProject;
	strProject.Format(_T("%s (ver%s)"), dlg.GetOpenProjectName(), strVersion);
#ifndef ENGLISH_MODE
	GF_AddLog(L"[%s] 프로젝트를 여는데 성공 했습니다.", strProject);
#else
	GF_AddLog(L"Successfully opened the project [%s].", strProject);
#endif
	Log::Trace("[%s] Project Opened!", CCommonFunc::WCharToChar(dlg.GetOpenProjectName().GetBuffer(0)));
	dwEnd = GetTickCount();
	GF_AddDebug(L"OnHomeProjectOpen : %d", dwEnd - dwStart);
	dtCur = COleDateTime::GetCurrentTime();
#ifndef ENGLISH_MODE
	GF_AddDebug(L"End 시간 : %s", dtCur.Format(L"%H:%M:%S"));
#else
	GF_AddDebug(L"End Time : %s", dtCur.Format(L"%H:%M:%S"));
#endif
	CString strName;
#ifndef ENGLISH_MODE
	strName.Format(L"프로젝트명 - %s", strProject);
#else
	strName.Format(L"Project Name - %s", strProject);
#endif
	AfxGetMainWnd()->SetWindowTextW(strName);

	if(g_bRequirePtnManualCheck)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"패턴을 수동으로 생성했는지 일람표로 생성했는지 체크되지 않았습니다.\n'수동으로 생성' 또는 '일람표로 생성'을 선택해 주시기 바랍니다.");
#else
		AfxMessageBox(L"You have not checked whether the pattern was created manually or from a table.\nPlease select 'Create Manually' or 'Create from Table'.");
#endif
		CDlgPatternProperty dlg;
		dlg.m_pRefFasSysData = m_pFasSysData;
		dlg.DoModal();
	}

	//20250804 GBM start - 도움말 파일 로드 : 프로그램 실행 시 최초 한번 실행
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->GetHelpMessage();
	//20250804 GBM end
}

void CSysLinkerApp::OnHomeProjectSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 1. Version 변경 : VER.PVI파일에 버전 변경 , 변경자 , 이름 수정
	// 2. Folder 복사
	// 3. DB Backup
	// 4.
	if (m_pFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트가 열려있지 않습니다.");
		GF_AddLog(L"프로젝트가 열려있지 않습니다.");
#else
		AfxMessageBox(L"The project is not open.");
		GF_AddLog(L"The project is not open.");
#endif
		return;
	}

	// [2025/10/22 16:48:28 KHS] 
	// Pump의 변경된 정보 저장 명령 전송
	CView * pView = GetSysLinkerView(m_pTemplePump);
	if(pView != nullptr)
	{
		pView->SendMessage(UWM_PROJECT_SAVE,0,0);
	}

	// Database 용량 줄이기
	if (m_pFasSysData->ReduceDatabase() == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트를 저장하는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to save the project.");
#endif
		return; 
	}
	//////////////////////////////////////////////////////////////////////////
	// 1. 편집되고 있는 프로젝트(Version_temp) 정보 변경
	CString strPrjPath , strFullPath , strDBPath;

	if (!m_bModuleTableChanged)
		m_pFasSysData->VersionUp();
	else
		m_pFasSysData->VersionUp(TRUE);

	//20240730 GBM start - 프로젝트 저장 시 Minor 번호 증가 적용
	CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum = m_pFasSysData->m_wPrjMajorNum;
	CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum = m_pFasSysData->m_wPrjMinorNum;
	//20240730 GBM end

	m_pFasSysData->SetPrjModifier(m_pFasSysData->GetCurrentUser());
	m_pFasSysData->SetPrjModifyDate(COleDateTime::GetCurrentTime());

	strPrjPath = g_stConfig.szPrjPath;
	if (strPrjPath.Right(1) != '\\')
		strPrjPath += "\\";

	strFullPath = strPrjPath + m_pFasSysData->GetPrjName();
	SaveProjectInfoFile(strFullPath);
	SaveVersionInfoFile(m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum(), strFullPath, TRUE);

	//////////////////////////////////////////////////////////////////////////
	// 2. Folder 복사
	CopyVersionTempToVersion(m_pFasSysData->GetPrjName(), strFullPath, m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum());

	//////////////////////////////////////////////////////////////////////////
	// 3. DB Backup --> 프로젝트Path\\프로젝트명\\버전폴더\\DB\\프로젝트명.bak
	strDBPath.Format(L"%s\\%s\\%s\\%s.BAK" 
		, strFullPath
		,GF_GetVersionFolderName(m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum())
		, F3_PRJ_DIR_DATABASE
		, m_pFasSysData->GetDBName()
	);
	if (GF_IsExistFile(strDBPath) == TRUE)
	{
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"프로젝트를 저장하는데 실패했습니다.(데이터베이스 Backup 실패)\r\n"
			L"'%s'에 같은 이름으로 파일 있습니다. 이 파일을 삭제하고 다시 저장해 주십시오."
			, strDBPath
		);
#else
		strError.Format(L"Failed to save the project (database backup failed)\nA file with the same name exists in '%s'.Please delete this file and save it again."
			, strDBPath
		);
#endif
		AfxMessageBox(strError);
		return ;
	}
	if (m_pMainDb->BackupDatabase(m_pFasSysData->GetDBName(), strDBPath) == FALSE)
	{
//		AfxMessageBox(L"프로젝트를 저장하는데 실패 했습니다.\n데이터베이스 Backup 실패");
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트를 저장하는데 실패했습니다.\n데이터베이스 Backup 실패");
#else
		GF_AddLog(L"Failed to save the project.\nDatabase Backup Failure");
#endif
// 		if (m_pMainDb->BackupDatabase(m_pFasSysData->GetDBName(), strDBPath) == FALSE)
// 		{
// 			GF_AddLog(L"프로젝트를 저장하는데 실패 했습니다.\n데이터베이스 Backup 실패");
// 		}
//		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 4. 
	m_pFasSysData->SetChangeFlag(FALSE);

	CString strVersion;
	strVersion.Format(_T("%s (ver%d.%d)"), m_pFasSysData->GetPrjName(), m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum());
#ifndef ENGLISH_MODE
	GF_AddLog(L"[%s] 프로젝트를 저장하는데 성공했습니다.", strVersion);
#else
	GF_AddLog(L"Successfully saved the project [%s].", strVersion);
#endif
	//AfxMessageBox(L"프로젝트를 저장하는데 성공했습니다");

	CString strName;
#ifndef ENGLISH_MODE
	strName.Format(L"프로젝트명 - %s", strVersion);
#else
	strName.Format(L"Project Name - %s", strVersion);
#endif
	AfxGetMainWnd()->SetWindowTextW(strName);

}

void CSysLinkerApp::OnHomeProjectTable()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_pFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트가 열려있지 않습니다.");
		AfxMessageBox(L"프로젝트가 열려있지 않습니다.");
#else
		GF_AddLog(L"The project is not open.");
		AfxMessageBox(L"The project is not open.");
#endif
		return;
	}
	OpenFormView(FV_LOADRELAYTABLE);
}

void CSysLinkerApp::OnBasicEnvset()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgEnv dlg;
	if (dlg.DoModal() != IDOK)
		return;
	ConfigSave();
#ifndef ENGLISH_MODE
	GF_AddLog(L"프로그램 환경설정을 저장했습니다.");
#else
	GF_AddLog(L"You've saved your program environment setup.");
#endif
}


void CSysLinkerApp::OnBasicSetAccesslevel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_ACCESS);
}


void CSysLinkerApp::OnBasicSetDraw()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CSysLinkerApp::OnBasicSetEquip()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//OpenFormView(FV_EQUIP);
}


void CSysLinkerApp::OnBasicSetFacp()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CSysLinkerApp::OnBasicSetInoutType()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_EQUIP);

}


void CSysLinkerApp::OnBasicSetPump()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CSysLinkerApp::OnBasicSetUnit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CSysLinkerApp::OnBasicSetUser()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CSysLinkerApp::OnBasicSetUsergroup()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_USER);

}

void CSysLinkerApp::OnBasicSetLogicEdit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	//20250805 GBM start - 별도의 로직 편집기 프로그램을 사용하는 방식으로 변경
#ifdef SLP4_MODE
	CString strPath = CCommonFunc::GetCurrentPath();
	CString strExe = _T("LogicEditor.exe");
	strPath += _T("\\") + strExe;
	HANDLE hHandle = nullptr;
	BOOL bFind = FALSE;
	//bFind = CCommonFunc::FindProcess(strExe, hHandle);	// FindWindow보다 확실한 방법이지만 실행 중인 전체 프로세스 스냅샷을 찍다보니 시간이 걸림
#ifndef ENGLISH_MODE
	CString strLogicEditorCaption = L"로직 편집기";
#else
	CString strLogicEditorCaption = L"Logic Editor";
#endif
	HWND hwnd = FindWindow(NULL, strLogicEditorCaption);		//캡션명으로 해야 할 듯
	if (hwnd != nullptr)
		bFind = TRUE;
	if (!bFind)
	{
		// 실행 중이 아니라면 실행
		CString strProjectName;
		strProjectName = m_pFasSysData->GetPrjName();
		CString strCaption;
		m_pMainWnd->GetWindowTextW(strCaption);
		strCaption = _T("\"") + strCaption + _T("\"");
		ShellExecuteW(NULL, L"open", strPath, strCaption, NULL, SW_SHOWNORMAL);
	}
	else
	{
#ifndef ENGLISH_MODE
		CString strMsg = strLogicEditorCaption + "가 이미 실행 중입니다.";
#else
		CString strMsg = "The " + strLogicEditorCaption + "is already running.";
#endif
		AfxMessageBox(strMsg);
	}
#else
	OpenFormView(FV_LOGICEDIT);
#endif
	//20250805 GBM end
}
// 
// void CSysLinkerApp::OnFacpCreateLink()
// {
// 	// TODO: 여기에 명령 처리기 코드를 추가합니다.
// 	if(AfxMessageBox(L"연동데이터를 현재 상태로 컴파일 합니다.",MB_YESNO | MB_ICONQUESTION) != IDYES)
// 		return; 
// // 	if (m_pDlgProg == nullptr)
// // 	{
// // 		m_pDlgProg = new CDlgProgress;
// // 		m_pDlgProg->Create(IDD_DLG_PROGRESS);
// // 	}
// // 	m_pDlgProg->ShowWindow(SW_SHOW);
// // 	m_pDlgProg->InitProgress();
// 
// // 	BYTE	btRomBuff[0x7FFFFF] = { 0 };
// // 	BYTE	btMsgBuff[0x27FFFF] = { 0 };
// 
// 	int nRet = 0; 
// 	CString strPath;
// 	CRelayTableData * pRefTable = theApp.GetRelayTableData();
// 	if (pRefTable == nullptr)
// 	{
// 		GF_AddLog(L"프로젝트가 닫힌 상태 입니다. 프로젝트를 여시고 다시 시도하여주십시요");
// 		AfxMessageBox(L"프로젝트가 닫힌 상태 입니다. 프로젝트를 여시고 다시 시도하여주십시요");
// 		return;
// 	}
// 	CString strPrjPath, strFullPath, strDBPath;
// 
// 	strPrjPath = g_stConfig.szPrjPath;
// 	if (strPrjPath.Right(1) != '\\')
// 		strPrjPath += "\\";
// 
// 	strFullPath = strPrjPath + m_pFasSysData->GetPrjName();
// 	strPath.Format(L"%s\\%s\\Release"
// 		, strFullPath, F3_VERSIONTEMPFOLDER_NAME
// 	);
// 	GF_CreateFolder(strPath);
// 	nRet = pRefTable->MakeLinkData(strPath);
// 	if(nRet > 0)
// 	{
// 		GF_AddLog(L"프로젝트를 컴파일 하는데 성공했습니다.(연동데이터 생성 성공)");
// 	//	AfxMessageBox(L"프로젝트를 컴파일 하는데 성공했습니다.");
// 	}
// 	else
// 	{
// 		if(strPath.Right(1) != '\\')
// 			strPath += L"\\";
// 		strPath += L"*.*";
// 		//GF_DeleteDir(strPath);
// 		GF_AddLog(L"프로젝트를 컴파일 하는데 실패했습니다.(연동데이터 생성 실패)");
// 		AfxMessageBox(L"프로젝트를 컴파일 하는데 실패했습니다." , MB_OK|MB_ICONSTOP);
// 		return;
// 		//AfxMessageBox(L"프로젝트를 컴파일 하는데 실패했습니다.");
// 	}
// 	nRet = pRefTable->MakeConstructorTable(strPath);
// 	if (nRet > 0)
// 	{
// 		GF_AddLog(L"프로젝트를 컴파일 하는데 성공했습니다.(연동 출력표생성 성공)");
// 		AfxMessageBox(L"프로젝트를 컴파일 하는데 성공했습니다.",MB_OK|MB_ICONINFORMATION);
// 	}
// 	else
// 	{
// 		if (strPath.Right(1) != '\\')
// 			strPath += L"\\";
// 		strPath += L"*.*";
// 		//GF_DeleteDir(strPath);
// 		GF_AddLog(L"프로젝트를 컴파일 하는데 실패했습니다.(연동 출력표생성 실패)");
// 		AfxMessageBox(L"프로젝트를 컴파일 하는데 실패했습니다.",MB_OK | MB_ICONSTOP);
// 	}
// }


void CSysLinkerApp::OnFacpEditLink()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_AUTOMAKE);
// 	CDlgMakeAutoAllInput dlg;
// 	dlg.m_pRefFasSysData = m_pFasSysData;
// 	if (dlg.DoModal() != IDOK)
// 		return;
}


void CSysLinkerApp::OnFacpEmmergency()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_EMERGENCY);
}


void CSysLinkerApp::OnFacpLinkPs()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_PSWITCH);
}


void CSysLinkerApp::OnFacpLinkPump()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_PUMP);
}


void CSysLinkerApp::OnFacpLocation()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_LOCATION);
}


void CSysLinkerApp::OnFacpPattern()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFormPattern * pPtnView = (CFormPattern*)OpenFormView(FV_MAKEPATTERN);
	CSysLinkerView * pView = (CSysLinkerView*)GetSysLinkerView(m_pTempleMakeLink);
	if (pView != nullptr && pPtnView != nullptr)
		pPtnView->RegisterPatternDropWnd(&pView->m_ctrlPatternList);
}


void CSysLinkerApp::OnFacpPreviewLink()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_LINKVIEW);
}


void CSysLinkerApp::OnFacpReverseLink()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_pFasSysData != nullptr)
	{
#ifndef ENGLISH_MODE
		if (AfxMessageBox(L"프로젝트가 열려있습니다. 기존 프로젝트를 닫고 역변환을 시작하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#else
		if (AfxMessageBox(L"The project is open. Do you want to close the existing project and start the inverse transformation?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#endif
		CloseProject();
	}

	m_pFasSysData = new CRelayTableData;

	if (m_spInputEquipManager == nullptr && m_spOutputEquipManager == nullptr
		&& m_spContentsEquipManager == nullptr && m_spPumpEquipManager == nullptr
		&& m_spPSEquipManager == nullptr && m_spEqNameManager == nullptr
		&& m_spPmpNameEquipManager == nullptr)
	{
		LoadEquipBaseData();
	}
	m_pFasSysData->InitFasSysData(m_spInputEquipManager, m_spEqNameManager
		, m_spOutputEquipManager, m_spContentsEquipManager
		, m_spPumpEquipManager, m_spPSEquipManager, m_spPmpNameEquipManager);

	CDlgLoadRom dlg; 
	dlg.SetRefRelayTable(m_pFasSysData);
	if (dlg.DoModal() != IDOK)
	{
		delete m_pFasSysData;
		m_pFasSysData = nullptr;
		return;
	}

	if (CreateProjectFromRom(dlg.m_strRomPath, &dlg.m_ptrList , dlg.m_bFoundRvFile) <= 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"ROM 파일로부터 프로젝트를 생성하는데 실패했습니다.");
		AfxMessageBox(L"ROM 파일로부터 프로젝트를 생성하는데 실패했습니다.");
#else
		GF_AddLog(L"Failed to create a project from the ROM file.");
		AfxMessageBox(L"Failed to create a project from the ROM file.");
#endif
		return;
	}

	CMainFrame * pMainWnd = nullptr;
	pMainWnd = (CMainFrame *)AfxGetMainWnd();

	if (pMainWnd)
		pMainWnd->SetRelayTable(m_pFasSysData);

	CSysLinkerView * pMakLinkView = (CSysLinkerView*)GetSysLinkerView(m_pTempleMakeLink);
	pMakLinkView->SetRelayTableData(m_pFasSysData);
	m_pFasSysData->SetProjectOpened(TRUE);
	PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_NEW, 0);
#ifndef ENGLISH_MODE
	AfxMessageBox(L"ROM 파일로부터 프로젝트를 생성하는데 성공했습니다.");
#else
	AfxMessageBox(L"Successfully created a project from the ROM file.");
#endif
}


void CSysLinkerApp::OnFacpSetFacppwd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgMakePwd dlg;
	dlg.DoModal();
}

void CSysLinkerApp::OnFacpRelayedit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_RELAYEDIT);

}



void CSysLinkerApp::OnUpdateHomeLogin(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->Enable(!m_bProgramLogin);
}


void CSysLinkerApp::OnUpdateHomeProjectClose(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateHomeProjectNew(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->Enable(m_bProgramLogin);
}


void CSysLinkerApp::OnUpdateHomeProjectOpen(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->Enable(m_bProgramLogin);
}


void CSysLinkerApp::OnUpdateHomeProjectSave(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateHomeProjectTable(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

void CSysLinkerApp::OnUpdateBasicEnvset(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	if (pCmdUI)
// 	{
// 		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
// 			pCmdUI->Enable(TRUE);
// 		else
// 			pCmdUI->Enable(FALSE);
// 	}
}


void CSysLinkerApp::OnUpdateBasicSetAccesslevel(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->Enable(FALSE);
// 	if (pCmdUI)
// 	{
// 		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
// 			pCmdUI->Enable(TRUE);
// 		else
// 			pCmdUI->Enable(FALSE);
// 	}
}



void CSysLinkerApp::OnUpdateErrorCheck(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(pCmdUI)
	{
		if(m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}



void CSysLinkerApp::OnUpdateBasicSetDraw(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->Enable(FALSE);
}


void CSysLinkerApp::OnUpdateBasicSetEquip(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateBasicSetFacp(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateBasicSetInoutType(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.

	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}

}


void CSysLinkerApp::OnUpdateBasicSetPump(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateBasicSetUnit(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateBasicSetUser(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifndef _DEBUG
	if (pCmdUI)
		pCmdUI->Enable(FALSE);
#endif
}


void CSysLinkerApp::OnUpdateBasicSetUsergroup(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#ifndef _DEBUG
	if (pCmdUI)
		pCmdUI->Enable(FALSE);
#endif
}


void CSysLinkerApp::OnUpdateBasicSetEditLogic(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// #ifdef _DEBUG
// 	return; 
// #endif
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

// void CSysLinkerApp::OnUpdateFacpCreateLink(CCmdUI *pCmdUI)
// {
// 	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
//  	if (pCmdUI)
//  	{
//  		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
//  			pCmdUI->Enable(TRUE);
//  		else
//  			pCmdUI->Enable(FALSE);
//  	}
// }


void CSysLinkerApp::OnUpdateFacpEditLink(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
 	if (pCmdUI)
 	{
 		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
 			pCmdUI->Enable(TRUE);
 		else
 			pCmdUI->Enable(FALSE);
 	}
}


void CSysLinkerApp::OnUpdateFacpEmmergency(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

void CSysLinkerApp::OnUpdateFacpLinkPs(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateFacpLinkPump(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateFacpLocation(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateFacpPattern(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateFacpPreviewLink(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}


void CSysLinkerApp::OnUpdateFacpReverseLink(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		//20240312 GBM start - 메뉴 비활성화 -> 20240618 GBM start - 다시 활성화
		//pCmdUI->Enable(FALSE);
		pCmdUI->Enable(m_bProgramLogin);
		//20240312 GBM end
}


void CSysLinkerApp::OnUpdateFacpRelayedit(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
	{
// 		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
// 			pCmdUI->Enable(TRUE);
// 		else
			pCmdUI->Enable(FALSE);
	}
}
void CSysLinkerApp::OnUpdateFacpSetFacppwd(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
// 	if (pCmdUI)
// 	{
// 		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
// 			pCmdUI->Enable(TRUE);
// 		else
// 			pCmdUI->Enable(FALSE);
// 	}
}



void CSysLinkerApp::OnErrorCheck()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OpenFormView(FV_ERRORCHECK);
}

void CSysLinkerApp::OnBasicSetEnv()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgEnv dlg;
	if (dlg.DoModal() != IDOK)
		return;

	ConfigSave();
}


int CSysLinkerApp::LoadEquipBaseData()
{
	if (m_spInputEquipManager == nullptr)
	{
		m_spInputEquipManager = std::make_shared<CManagerEquip>();
	}

	if (m_spEqNameManager == nullptr)
	{
		m_spEqNameManager = std::make_shared<CManagerEquip>();
	}


	if (m_spOutputEquipManager == nullptr)
	{
		m_spOutputEquipManager = std::make_shared<CManagerEquip>();
	}

	if (m_spContentsEquipManager == nullptr)
	{
		m_spContentsEquipManager = std::make_shared<CManagerEquip>();
	}

	if (m_spPumpEquipManager == nullptr)
	{
		m_spPumpEquipManager = std::make_shared<CManagerEquip>();
	}

	if (m_spPSEquipManager == nullptr)
	{
		m_spPSEquipManager = std::make_shared<CManagerEquip>();
	}


	if (m_spPmpNameEquipManager == nullptr)
	{
		m_spPmpNameEquipManager = std::make_shared<CManagerEquip>();
	}

/*	m_spEquipManager->LoadEquipData();*/

	CString strSql, strType, strName,strFile;
	CDataEquip * pEq;
	int nCnt = 0, nValue = 0, i;
	int nEtype, nEid;
	if (m_pMainDb == nullptr || m_pMainDb->IsOpen() == FALSE)
	{
		GF_AddLog(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  A.*, B.EQ_NAME AS EQ_TYPE_NAME "
		L"FROM     TB_EQUIP_MST AS A INNER JOIN "
		L"TB_EQUIP_TYPE AS B ON A.EQ_TYPE = B.EQ_TYPE "
		L"ORDER BY A.EQ_TYPE, A.EQ_ID");
	if (m_pMainDb->OpenQuery(strSql) == FALSE)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"데이터베이스 테이블을 여는데 실패했습니다. : TB_EQUIP_MST open failed.");
#else
		GF_AddLog(L"Failed to open the database table. : TB_EQUIP_MST open failed.");
#endif
		return 0;
	}
	nCnt = m_pMainDb->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pMainDb->GetFieldValue(L"EQ_ID", nValue);
		nEid = nValue;
		m_pMainDb->GetFieldValue(L"EQ_TYPE", nValue);
		nEtype = nValue;
		m_pMainDb->GetFieldValue(L"EQ_TYPE_NAME", strType);
		m_pMainDb->GetFieldValue(L"EQ_NAME", strName);
		strName.Remove(' ');
		m_pMainDb->GetFieldValue(L"EQ_SYMBOL", strFile);
		pEq = new CDataEquip;
		pEq->SetData(nEid,(int)nEtype, strName, strName,strFile);
		switch (nEtype)
		{
		case ET_INPUTTYPE:
			m_spInputEquipManager->AddTail(pEq);
			break;
		case ET_EQNAME:
			m_spEqNameManager->AddTail(pEq);
			break;
		case ET_OUTPUTTYPE:
			m_spOutputEquipManager->AddTail(pEq);
			break;
		case ET_OUTCONTENTS:
			m_spContentsEquipManager->AddTail(pEq);
			break;
		case ET_PUMPTYPE:
			m_spPumpEquipManager->AddTail(pEq);
			break;
		case ET_PSTYPE:
			m_spPSEquipManager->AddTail(pEq);
			break;
		case ET_PMPNAME:
			m_spPmpNameEquipManager->AddTail(pEq);
			break;
		}
		m_pMainDb->MoveNext();
	}
	return 1;
}

int CSysLinkerApp::ConfigLoad()
{
	CString strPath = g_strAppPath;
	if (strPath.Right(1) != '\\')
		strPath += "\\";
	strPath += "Config\\SysLinker.INI";

	TCHAR	buff[_MAX_PATH + 1] = { 0 };
	int nRet = 0 , nSize , i , nTemp;
	vector<CString> vtView;
	memset(&g_stConfig, 0, sizeof(ST_CONFIG));

	CString strDefaultPath = _T("");
	strDefaultPath.Format(_T("C:\\%s\\PROJECT\\"), ROOT_FOLDER);
	nRet = ::GetPrivateProfileString(L"PROJECT", L"PATH", strDefaultPath, buff, _MAX_PATH, (LPCTSTR)strPath);
	if (nRet)
	{
		wcscpy_s(g_stConfig.szPrjPath, sizeof(g_stConfig.szPrjPath) / sizeof(TCHAR), buff);
	}

	nRet = GetPrivateProfileInt(L"DATABASE", L"DBPORT", 1433, (LPCTSTR)strPath);
	g_stConfig.nDBPort = nRet;

	nRet = ::GetPrivateProfileString(L"DATABASE", L"SERVER", L"127.0.0.1", buff, _MAX_PATH, (LPCTSTR)strPath);
	if (nRet)
	{
		wcscpy_s(g_stConfig.szSource, sizeof(g_stConfig.szSource) / sizeof(TCHAR), buff);
	}
	nRet = ::GetPrivateProfileString(L"DATABASE", L"DBNAME", L"gfs_base", buff, _MAX_PATH, (LPCTSTR)strPath);
	if (nRet)
	{
		wcscpy_s(g_stConfig.szDBName, sizeof(g_stConfig.szDBName) / sizeof(TCHAR), buff);
	}

	nRet = ::GetPrivateProfileString(L"DATABASE", L"DBUSER", L"root", buff, _MAX_PATH, (LPCTSTR)strPath);
	if (nRet)
	{
		wcscpy_s(g_stConfig.szDBUser, sizeof(g_stConfig.szDBUser) / sizeof(TCHAR), buff);
	}
	nRet = ::GetPrivateProfileString(L"DATABASE", L"DBPASS", L"root", buff, _MAX_PATH, (LPCTSTR)strPath);
	if (nRet)
	{
		wcscpy_s(g_stConfig.szDBPass, sizeof(g_stConfig.szDBPass) / sizeof(TCHAR), buff);
	}

	nRet = GetPrivateProfileInt(L"DATABASE",L"QUERY_TIMEOUT",7200,(LPCTSTR)strPath);
	g_stConfig.dwTimeOut = nRet;

	nRet = GetPrivateProfileInt(L"SERIAL", L"COMM_PORT", 1, strPath);
	if (nRet > 0)
	{
		g_stConfig.nSerialPort = nRet;
	}

	nRet = GetPrivateProfileString(L"SERIAL", L"COMM_BAUDRATE", L"19200", buff, D_MAX_NAME, strPath);
	if (nRet > 0)
	{
		//g_stConfig.nLcbServerPort = nRet;
		g_stConfig.nBaudrate = _wtoi(buff);
	}

	nRet = GetPrivateProfileString(L"SERIAL", L"COMM_DATABIT", L"19200", buff, D_MAX_NAME, strPath);
	if (nRet > 0)
	{
		//g_stConfig.nLcbServerPort = nRet;
		g_stConfig.nDataBit = _wtoi(buff);
	}

	nRet = GetPrivateProfileString(L"SERIAL", L"COMM_STOPBIT", L"0", buff, D_MAX_NAME, strPath);
	if (nRet > 0)
	{
		g_stConfig.nStopbit = _wtoi(buff);
	}

	nRet = GetPrivateProfileString(L"SERIAL", L"COMM_PARITYBIT", L"0", buff, D_MAX_NAME, strPath);
	if (nRet > 0)
	{
		g_stConfig.nParitybit = _wtoi(buff);
	}

	g_tvInput.clear();
	nRet = GetPrivateProfileString(L"TREEVIEW", L"INPUT", L"", buff, D_MAX_NAME, strPath);
	if (nRet > 0)
	{
		wcscpy_s(g_stConfig.szInput, sizeof(g_stConfig.szInput) / sizeof(TCHAR), buff);
		vtView = GF_SplitString(g_stConfig.szInput, L".");
		nSize = (int)vtView.size();
		g_tvInput.resize(nSize);
		for (i = 0; i < nSize; i++)
		{
			try
			{
				nTemp = _wtoi(vtView[i]);
			}
			catch (...)
			{
				nTemp = 0; 
				continue;
			}
			g_tvInput[i] = nTemp;
		}
	}
	else
		memset(g_stConfig.szInput ,0 , sizeof(g_stConfig.szInput));


	g_tvOutput.clear();
	nRet = GetPrivateProfileString(L"TREEVIEW", L"OUTPUT", L"", buff, D_MAX_NAME, strPath);
	if (nRet > 0)
	{
		wcscpy_s(g_stConfig.szOutput, sizeof(g_stConfig.szOutput) / sizeof(TCHAR), buff);
		vtView = GF_SplitString(g_stConfig.szOutput, L".");
		nSize = (int)vtView.size();
		g_tvOutput.resize(nSize);
		for (i = 0; i < nSize; i++)
		{
			try
			{
				nTemp = _wtoi(vtView[i]);
			}
			catch (...)
			{
				nTemp = 0;
				continue;
			}
			g_tvOutput[i] = nTemp;
		}
	}
	else
		memset(g_stConfig.szOutput, 0, sizeof(g_stConfig.szOutput));

	//20260126 GBM start - 로그인 창 설정값 추가
	nRet = GetPrivateProfileInt(L"LOGIN", L"ACCOUNT_SAVE", 0, (LPCTSTR)strPath);
	if (nRet > 0)
	{
		g_stConfig.nAccountSave = nRet;
	}
	//20260126 GBM end

	return 1;
}


int CSysLinkerApp::ConfigSave()
{
	CString str;
	CString strPath = g_strAppPath;
	if (strPath.Right(1) != '\\')
		strPath += "\\";
	strPath += "Config\\SysLinker.INI";
	TCHAR	buff[_MAX_PATH + 1] = { 0 };
	BOOL bRet = 0;

	bRet = WritePrivateProfileString(L"PROJECT", L"PATH", g_stConfig.szPrjPath, (LPCTSTR)strPath);
	if (bRet == FALSE)
		return FALSE;

	bRet = WritePrivateProfileString(L"DATABASE", L"SERVER", g_stConfig.szSource, (LPCTSTR)strPath);
	if (bRet == FALSE)
		return FALSE;

	bRet = WritePrivateProfileString(L"DATABASE", L"DBNAME", g_stConfig.szDBName, (LPCTSTR)strPath);
	if (bRet == FALSE)
		return FALSE;

	bRet = WritePrivateProfileString(L"DATABASE", L"DBUSER", g_stConfig.szDBUser, (LPCTSTR)strPath);
	if (bRet == FALSE)
		return FALSE;

	bRet = WritePrivateProfileString(L"DATABASE", L"DBPASS", g_stConfig.szDBPass, (LPCTSTR)strPath);
	if (bRet == FALSE)
		return FALSE;

	str.Format(L"%d", g_stConfig.nDBPort);
	bRet = WritePrivateProfileString(L"DATABASE", L"DBPORT", str,(LPCTSTR)strPath);
	if (bRet == FALSE)
		return FALSE;

	str.Format(L"%d",g_stConfig.dwTimeOut);
	bRet = WritePrivateProfileString(L"DATABASE",L"QUERY_TIMEOUT",str,(LPCTSTR)strPath);
	if(bRet == FALSE)
		return FALSE;

	str.Format(L"%d" , g_stConfig.nSerialPort);
	bRet = WritePrivateProfileString(L"SERIAL", L"COMM_PORT", str, strPath);
	if (bRet == FALSE)
		return FALSE;

	str.Format(L"%d", g_stConfig.nBaudrate);
	bRet = WritePrivateProfileString(L"SERIAL", L"COMM_BAUDRATE", str, strPath);
	if (bRet == FALSE)
		return FALSE;

	str.Format(L"%d", g_stConfig.nDataBit);
	bRet = WritePrivateProfileString(L"SERIAL", L"COMM_DATABIT", str, strPath);
	if (bRet == FALSE)
		return FALSE;
	str.Format(L"%d", g_stConfig.nStopbit);
	bRet = WritePrivateProfileString(L"SERIAL", L"COMM_STOPBIT",str , strPath);
	if (bRet == FALSE)
		return FALSE;

	str.Format(L"%d", g_stConfig.nParitybit);
	bRet = WritePrivateProfileString(L"SERIAL", L"COMM_PARITYBIT", str, strPath);
	if (bRet == FALSE)
		return FALSE;

	MakeCustomTreeViewItem(FALSE);
	bRet = WritePrivateProfileString(L"TREEVIEW", L"INPUT", g_stConfig.szInput, strPath);
	if (bRet == FALSE)
		return FALSE;

	MakeCustomTreeViewItem(TRUE);
	bRet = WritePrivateProfileString(L"TREEVIEW", L"OUTPUT", g_stConfig.szOutput, strPath);
	if (bRet == FALSE)
		return FALSE;

	//20260126 GBM start - 로그인 창 설정값 추가
	str.Format(L"%d", g_stConfig.nAccountSave);
	bRet = WritePrivateProfileString(L"LOGIN", L"ACCOUNT_SAVE", str, strPath);
	if (bRet == FALSE)
		return FALSE;
	//20260126 GBM end

	return 1;
}


int CSysLinkerApp::CreateProjectFromRom(CString strRomPath , CPtrList *pPtrList, BOOL bUseRvFile)
{

 	//1. Create Folder
 	int nRet = 0;
 	//////////////////////////////////////////////////////////////////////////
 	// 파일 생성 : Project 정보 파일 생성 , Symbol 복사해오기 , 중계기 테이블
 	//
 	nRet = CreateProjectFolder();
	if (nRet <= 0)
		return 0;
 	//////////////////////////////////////////////////////////////////////////
 	// Data 입력 - mssql에서 DB 통째로 복사해서 넣는다. - 테이블 , 기초정보 , 
//  	nRet = CreateProjectDatabase();
// 	if (nRet <= 0)
// 		return 0;

	m_pFasSysData->CreateFromRom(strRomPath , pPtrList , bUseRvFile);
	
	nRet = CreateProjectDatabase(TRUE);
// 	m_pFasSysData->InsertPrjBasePatternDB();
// 	m_pFasSysData->InsertPrjBaseLocationDB();
// 	m_pFasSysData->InsertPrjBaseRelayTable();
// 	m_pFasSysData->LoadAutMakeLogic();

	return nRet;
}

int CSysLinkerApp::CreateProject()
{
	//1. Create Folder
	int nRet = 0;


	//////////////////////////////////////////////////////////////////////////
	// 파일 생성 : Project 정보 파일 생성 , Symbol 복사해오기 , 중계기 테이블
	//
	nRet = CreateProjectFolder();
	
	//////////////////////////////////////////////////////////////////////////
	// Data 입력 - mssql에서 DB 통째로 복사해서 넣는다. - 테이블 , 기초정보 , 
	nRet = CreateProjectDatabase();

	return nRet;
}

/*!
* @fn         CreateProjectFolder
* @version    1.0
* @date       2019-12-20 10:15:56
* @param
* @brief      새로운 프로젝트를 만든다.
* @return    성공 시 1 , 실패시 0
* @note
1. 프로젝트 명으로 폴더 생성
2. 프로젝트 명안에 VERSION_TEMP 생성 (버전 저장시에 VERSION 1.0 폴더 생성)
*/
int CSysLinkerApp::CreateProjectFolder()
{
	CString strPrjPath, strVersion, strLastPath;
	CString strPrjNameFolder, strPrjVerFolder;
	CString strPrjSymbolFolder, strPrjResFolder, strPrjDBFolder, strPrjRelayTableFolder, strPrjMapFolder, strPrjEtcFolder, strPrjLogicFolder, strPrjOupputContentImages;
	strPrjPath = g_stConfig.szPrjPath;
	if (strPrjPath.Right(1) != '\\')
		strPrjPath += "\\";

	//////////////////////////////////////////////////////////////////////////
	//CreateProjectFolder 
	strPrjNameFolder = strPrjPath + m_pFasSysData->GetPrjName();
	m_strPrjRootFolder = strPrjNameFolder;
	CreateDirectory(strPrjNameFolder, nullptr);

	// project verstion_temp 폴더 생성
	strVersion = F3_VERSIONTEMPFOLDER_NAME;
	strPrjVerFolder = strPrjNameFolder + L"\\" + strVersion;
	CreateDirectory(strPrjVerFolder, nullptr);

	strPrjSymbolFolder.Format(L"%s\\%s", strPrjVerFolder, F3_PRJ_DIR_SYMBOL);
	CreateDirectory(strPrjSymbolFolder, nullptr);

	strPrjResFolder.Format(L"%s\\%s", strPrjVerFolder, F3_PRJ_DIR_RESOURCES);
	CreateDirectory(strPrjResFolder, nullptr);

	strPrjDBFolder.Format(L"%s\\%s", strPrjVerFolder, F3_PRJ_DIR_DATABASE);
	CreateDirectory(strPrjDBFolder, nullptr);

	strPrjRelayTableFolder.Format(L"%s\\%s", strPrjVerFolder, F3_PRJ_DIR_RELAYTABLE);
	CreateDirectory(strPrjRelayTableFolder, nullptr);

	strPrjMapFolder.Format(L"%s\\%s", strPrjVerFolder, F3_PRJ_DIR_MAP);
	CreateDirectory(strPrjMapFolder, nullptr);

	strPrjEtcFolder.Format(L"%s\\%s", strPrjVerFolder, F3_PRJ_DIR_ETC);
	CreateDirectory(strPrjEtcFolder, nullptr);

	strPrjEtcFolder.Format(L"%s\\%s", strPrjVerFolder, F3_PRJ_DIR_RELEASE);
	CreateDirectory(strPrjEtcFolder, nullptr);

	//20251014 GBM start - 로직 편집 정보가 들어갈 폴더 생성
	strPrjLogicFolder.Format(L"%s\\%s", strPrjVerFolder, F3_PRJ_DIR_LOGIC);
	CreateDirectory(strPrjLogicFolder, nullptr);
	//20251014 GBM end

	CopyBaseFile(m_pFasSysData->GetPrjName(),strPrjSymbolFolder, strPrjRelayTableFolder, strPrjDBFolder);
	SaveProjectInfoFile(strPrjNameFolder);
	SaveVersionInfoFile(1, 0, strPrjNameFolder);
	return 1;
}

int CSysLinkerApp::CopyBaseFile(CString strPrjName, CString strTargetSymbolPath , CString strTargetRelayTablePath, CString strTargetDBPath)
{
	CString strFrom, strTo, strFile , strError=L"";
	SHFILEOPSTRUCT shStruct;
	TCHAR pszFrom[4096] = { 0 }, pszTo[4096] = { 0 };
	ZeroMemory(&shStruct, sizeof(SHFILEOPSTRUCT));

	strFrom = g_strAppPath + F3_PRJ_DIR_SYMBOL;

	//////////////////////////////////////////////////////////////////////////
	// 1. C:\\FICON3\\SYMBOLE 에서 Version folder안에 복사.
	_tcscpy_s(pszTo, 4096, strTargetSymbolPath);
	_tcscpy_s(pszFrom, 4096, strFrom);
	pszTo[strTargetSymbolPath.GetLength()] = '\0';
	pszTo[strTargetSymbolPath.GetLength() + 1] = '\0';
	pszFrom[strFrom.GetLength()] = '\0';
	pszFrom[strFrom.GetLength() + 1] = '\0';

	shStruct.fFlags = FOF_NOCONFIRMATION | FOF_MULTIDESTFILES | FOF_SILENT;
	//shStruct.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	shStruct.hwnd = NULL;
	shStruct.pTo = pszTo;
	shStruct.pFrom = pszFrom;
	shStruct.wFunc = FO_COPY;
	shStruct.fAnyOperationsAborted = NULL;
	shStruct.hNameMappings = NULL;
	shStruct.lpszProgressTitle = NULL;
	int nRet = SHFileOperation(&shStruct);
	if (nRet != 0)
	{
		strError = GF_GetSHErrorString(nRet);
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return 0; 
	}

	//////////////////////////////////////////////////////////////////////////
	// 2. 중계기 테이블
	int nIdx = 0;
	POSITION pos;
	CString strPath = L"";
	CStringList * pStrList;
	pStrList = m_pFasSysData->GetRelayTableList();
	pos = pStrList->GetHeadPosition();
	//nCnt = pStrList->GetCount();
	while (pos)
	{
		strPath = pStrList->GetNext(pos);
		if (strPath == L"")
			continue;

		ZeroMemory(&shStruct, sizeof(SHFILEOPSTRUCT));
		nIdx = strPath.ReverseFind('\\');
		if (nIdx < 0)
			continue;
		strFile = strPath.Mid(nIdx + 1);
		strTo.Format(L"%s\\%s", strTargetRelayTablePath, strFile);
		memset(pszTo, 0, sizeof(pszTo));
		memset(pszFrom, 0, sizeof(pszFrom));
		_tcscpy_s(pszTo, 4096, strTo);
		_tcscpy_s(pszFrom, 4096, strPath);
		pszTo[strTo.GetLength()] = '\0';
		pszTo[strTo.GetLength() + 1] = '\0';
		pszFrom[strPath.GetLength()] = '\0';
		pszFrom[strPath.GetLength() + 1] = '\0';

		USERLOG(L"%s\n", pszFrom);
		USERLOG(L"%s\n", pszTo);
		shStruct.fFlags = FOF_NOCONFIRMATION | FOF_FILESONLY | FOF_SILENT;
		shStruct.hwnd = NULL;
		shStruct.pTo = pszTo;
		shStruct.pFrom = pszFrom;
		shStruct.wFunc = FO_COPY;
		shStruct.fAnyOperationsAborted = NULL;
		shStruct.hNameMappings = NULL;
		shStruct.lpszProgressTitle = NULL;
		nRet = SHFileOperation(&shStruct);
		if (nRet != 0)
		{
			strError = GF_GetSHErrorString(nRet);
			GF_AddLog(strError);
			AfxMessageBox(strError);
			return 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// MS-SQL DB 복사 : program root\db\base.dat , base.log --> Project\version_temp\db
	CString strTarget;
	strFile.Format(L"%s%s\\%s", g_strAppPath, F3_PRJ_DIR_DATABASE, FAS_BASE_BACKUP_FILE);
	if (strTargetDBPath.Right(1) != '\\')
		strTargetDBPath += L"\\";

	strTarget.Format(L"%s%s.BAK", strTargetDBPath,strPrjName);
	if (::CopyFile(strFile, strTarget, FALSE) == FALSE)
		//if (::CopyFile(strFile, strTarget, FALSE) == FALSE)
	{
#ifndef ENGLISH_MODE
		strError.Format(L"DBMS에 데이터 파일을 설정하는데 실패했습니다.ErrorCode:%ld" , GetLastError());
#else
		strError.Format(L"Failed to set up the data file in DBMS. Error Code : %ld", GetLastError());
#endif
		GF_AddLog(strError + L" -- " + strTarget + L" -- "  + strFile);
		AfxMessageBox(strError);
		AfxMessageBox(strTarget + L"---" + strFile);
		return 0;
	}

	return 1;
}

int CSysLinkerApp::CreateProjectDatabase(BOOL bReverse)
{
	//////////////////////////////////////////////////////////////////////////
	// Database 생성 
	// 1. Main Database 연결
	// 2. Project Database 등록 , 연결
// 	CString strSql;
// 	CString strDBPath, strPath;
// 	
// 	if (m_pMainDb == nullptr)
// 	{
// 		m_pMainDb = new YAdoDatabase;
// 	}
// 
// 	if (m_pMainDb->IsOpen() == FALSE)
// 	{
// 		m_pMainDb->MSSqlInitialize(g_stConfig.szDBPass, g_stConfig.szDBUser
// 			, g_stConfig.szDBName, g_stConfig.szSource);
// 
// 		if (m_pMainDb->DBOpen() == FALSE)
// 		{
// 			AfxMessageBox(L"메인 데이터베이스 접속에 실패했습니다.\n환경설정을 다시 하고 시도해 주십요");
// 			return -1;
// 		}
// 	}
// 	strPath = g_stConfig.szPrjPath;
// 	if (strPath.Right(1) != '\\')
// 		strPath += L"\\";
// 	strDBPath.Format(L"%s%s\\%s\\%s\\"
// 		, strPath, m_pFasSysData->GetPrjName()
// 		, F3_VERSIONTEMPFOLDER_NAME
// 		, F3_PRJ_DIR_DATABASE
// 	);
// 
// 	if (m_pMainDb->IsAttachedDatabase(m_pFasSysData->GetPrjName()) == FALSE)
// 	{
// 		CString strDB, strlog;
// 		strDB.Format(L"%s%s_DATA.MDF", strDBPath, m_pFasSysData->GetPrjName());
// 		strlog.Format(L"%s%s_LOG.LDF", strDBPath, m_pFasSysData->GetPrjName());
// 		//ATTACH
// 		if (m_pMainDb->AttachMSDB(m_pFasSysData->GetPrjName(), strDB, strlog) == FALSE)
// 		{
// 
// 			CString strError;
// 			strError.Format(L"프로젝트 데이터베이스(%s)를 등록하는데 실패 했습니다.\n%s"
// 				, m_pFasSysData->GetPrjName(), m_pMainDb->GetLastErrorString());
// 			AfxMessageBox(strError);
// 			return -2;
// 		}
// 	}
// 
// 	if (m_pFasSysData->OpenPrjDatabase(g_stConfig.szSource, m_pFasSysData->GetPrjName()
// 		, 1433, g_stConfig.szDBUser, g_stConfig.szDBPass) <= 0)
// 	{
// 		CString strError;
// 		strError.Format(L"프로젝트 데이터베이스(%s)를 연결하는데 실패 했습니다.\n%s"
// 			, m_pFasSysData->GetPrjName(), m_pMainDb->GetLastErrorString());
// 		AfxMessageBox(strError);
// 		return -3;
// 	}
// 	
	//m_pFasSysData->CreatePrjTable();
	if (OpenProjectDatabase(m_pFasSysData) <= 0)
		return 0; 
	if(m_pFasSysData->InsertPrjBaseData() <= 0)
		return 0;

	//20240808 GBM start - 테이블 생성하지 않고 기존 TB_FACP, TB_UNIT을 활용
#if 0
	//20240222 GBM start - 중계기 일람표 파싱이 끝나고 기존 DB가 준비된 시점에 GT1 추가 테이블에 Data Insert
	CNewDBManager::Instance()->SetDBAccessor(m_pFasSysData->m_pDB);
	//GT1 추가 입력 타입은 gfs_base에 미리 넣어 프로젝트 DB를 복사하면 적용하고 GT1 추가 테이블은 중계기 일람표 상에서 GT1 추가 정보가 존재할 때만 GT1 추가 테이블을 생성
	BOOL bRet = FALSE;

	//역변환일 경우 수신기 타입 정보를 보고 GT1 프로젝트인지 판단
	if (bReverse)
	{
		BOOL bGT1Type = FALSE;

		for (int nFacp = 0; nFacp < MAX_FACP_COUNT; nFacp++)
		{
			if (CNewInfo::Instance()->m_gi.facpType[nFacp] == GT1)
			{
				bGT1Type = TRUE;
				break;
			}
		}
		bRet = bGT1Type;
	}
	// 일반적인 새 프로젝트 생성일 경우는 중계기 일람표를 읽으므로 GT1 관련 시트가 존재하는 지 여부를 보고 판단
	else
	{
		// GT1추가 테이블의 경우는 변경된 중계기 일람표가 GT1 추가 정보가 있는 경우에만 생성, 당연히 해당 Sheet 중 하나가 있으면 다 있겠지만 혹시 완벽하지 않으면 DB에 넣지 않도록 함
		// -> 수신기 타입과 유닛 타입만 사용
		bRet = CNewExcelManager::Instance()->bExistFT && CNewExcelManager::Instance()->bExistUT;
	}
	
	if (bRet)
	{
		BOOL bRet = FALSE;
		bRet = CNewDBManager::Instance()->CheckAndCreateGT1DBTables();
		if (bRet)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"GT1 정보 테이블 (수신기 TYPE, UNIT TYPE) 생성이 성공했습니다.");
#else
			GF_AddLog(L"Successfully created the GT1 information table (FACP TYPE, UNIT TYPE).");
#endif
			Log::Trace("Inserting new DB table succeeded!");
		}
		else
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"GT1 정보 테이블 (수신기 TYPE, UNIT TYPE) 생성이 실패했습니다, DB를 확인하세요.");
#else
			GF_AddLog(L"Failed to create the GT1 information table (FACP TYPE, UNIT TYPE). Please check the DB.");
#endif
			Log::Trace("Inserting new DB table failed!");
		}

		//20240222 GBM start - 중계기 일람표 파싱이 끝난 시점에 GT1 추가 테이블에 Data Insert
		bRet = CNewDBManager::Instance()->InsertDatasIntoGT1DBTables();
		if (bRet)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"GT1 정보 테이블 (수신기 TYPE, UNIT TYPE) 데이터 추가에 성공했습니다.");
#else
			GF_AddLog(L"Successfully added the GT1 information table (FACP TYPE, UNIT TYPE) data.");
#endif
			Log::Trace("GT1 DB table insertion succeeded!");
		}
		else
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"GT1 정보 테이블 (수신기 TYPE, UNIT TYPE) 데이터 추가에 실패했습니다, DB를 확인하세요.");
#else
			GF_AddLog(L"Failed to add the GT1 information table (FACP TYPE, UNIT TYPE) data. Please check the DB.");
#endif
			Log::Trace("GT1 DB table insertion failed!");
		}
		//20240222 GBM end

		//20240703 GBM start - 중계기 일람표 편집 기능 Disable
#ifdef MODULE_TABLE_UPDATE_MODE
		//20240422 GBM start
		//GT1과 F3이 혼재되어 있을 경우 불일치되는 설비 정의가 메모리에 추가 반영되어 위(m_pFasSysData->InsertPrjBaseData())에서 DB에 써지지만
		//중계기 일람표 Sheet에는 적용되지 않으므로 이를 적용, 추가된 설비 정보는 CRelayTableData::AddNewEquip(CString strEquipName, int nType)에서 
		//중계기 일람표 설비 정의 메모리에 반영되지 않음(새 중계기 일람표 갱신 시 비교데이터가 아니므로), 
		//이 시점에서 한번이라도 새 설비 정의가 추가되었다면 프로젝트 폴더의 중계기 일람표 폴더에 이미 중계기 일람표 복사가 완료되었으므로 직접 수정
		//이 작업은 중계기 일람표를 다시 다 열어야 하므로 파일이 크면 오래 걸리는 작업이므로 새 정의가 추가되었을 때만 수행
		if (m_pFasSysData->GetNewEquipmentTypeAdded())
		{
			POSITION pos;
			std::shared_ptr <CManagerEquip> spManager;
			CDataEquip * pEq;
			int nID;
			CString strTemp;
			for (int i = ET_INPUTTYPE; i <= ET_OUTCONTENTS; i++)
			{
				spManager = m_pFasSysData->GetEquipManager(i);
				if (spManager == nullptr)
					return -1;

				pos = spManager->GetHeadPosition();
				while (pos)
				{
					nID = -1;
					strTemp = _T("");
					pEq = spManager->GetNext(pos);
					if (pEq == nullptr)
						continue;

					nID = pEq->GetEquipID();
					ASSERT(nID > 0);
					nID--;	//번호는 1베이스 인덱스는 0베이스
					strTemp = pEq->GetEquipName();
					strTemp.Remove(' ');
					strTemp.MakeUpper();

					switch (i)
					{
					case ET_INPUTTYPE:
					{
						sprintf_s(CNewInfo::Instance()->m_ei.inputType[nID], CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
						break;
					}
					case ET_EQNAME:
					{
						sprintf_s(CNewInfo::Instance()->m_ei.equipmentName[nID], CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
						break;
					}
					case ET_OUTPUTTYPE:
					{
						sprintf_s(CNewInfo::Instance()->m_ei.outputType[nID], CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
						break;
					}
					case ET_OUTCONTENTS:
					{
						sprintf_s(CNewInfo::Instance()->m_ei.outputCircuit[nID], CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
						break;
					}
					default:
						break;
					}
				}
			}

			bRet = CNewExcelManager::Instance()->UpdateEquipmentInfo(m_pFasSysData->GetPrjName());
			if (bRet)
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"설비 정의 Sheet에 없어서 추가된 설비 정의를 중계기 일람표에 저장하는 데에 성공했습니다.");
#else
				GF_AddLog(L"Successfully saved the added equipment definition to the module table because it was not found in the equipment definition sheet.");
#endif
				Log::Trace("We succeeded in saving the added equipment definition to the equipment definition sheet in module table file because it was not in the equipment definition sheet.");
			}
			else
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"설비 정의 Sheet에 없어서 추가된 설비 정의를 중계기 일람표에 저장하는 데에 실패했습니다.");
#else
				GF_AddLog(L"Failed to save the added equipment definition to the module table because it was not found in the equipment definition sheet.");
#endif
				Log::Trace("We failed to save the added equipment definition to the equipment definition sheet in module table file because it was not in the equipment definition sheet.");
			}
		}
#endif
		//20240703 GBM end

	}
	//20240222 GBM end

#endif
	//20240808 GBM end

	return 1;
}
// 
// CSysLinkerView* CSysLinkerApp::GetActiveLinkView()
// {
// 	POSITION pos = theApp.GetFirstDocTemplatePosition();
// 	CDocTemplate* pDocTemplate = nullptr;
// 	POSITION doc_pos = nullptr;
// 	CDocument* pDoc = nullptr;
// 	POSITION view_pos = nullptr;
// 	CView* pView = nullptr;
// 
// 	pDocTemplate = theApp.m_pTempleMakeLink;
// 	for (doc_pos = pDocTemplate->GetFirstDocPosition(); doc_pos != NULL; )
// 	{
// 		pDoc = pDocTemplate->GetNextDoc(doc_pos);
// 		for (view_pos = pDoc->GetFirstViewPosition(); view_pos != NULL; )
// 		{
// 			pView = pDoc->GetNextView(view_pos);
// 			if (pView->IsKindOf(RUNTIME_CLASS(CSysLinkerView)))
// 			{
// 				return (CSysLinkerView*)pView;
// 			}
// 		}
// 	}
// 	return nullptr;
// }



int CSysLinkerApp::SaveProjectInfoFile(CString strCurrentPrjRootFolder)
{
	CStdioFileEx file;
	CString strtemp, strPath;
	strPath.Format(L"%s\\%s.%s", strCurrentPrjRootFolder, m_pFasSysData->GetPrjName(), F3_PROJECT_EXT);
	if (!file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CStdioFileEx::modeWriteUnicode))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 파일을 생성하는데 실패했습니다.\n");
#else
		GF_AddLog(L"Failed to create the project file.\n");
#endif
		return 0;
	}

	// 	CString			m_strPrjName;
	// 	CString			m_strSiteName;
	// 	CString			m_strSitePhone;
	// 	CString			m_strPrjMaker;
	// 	INT_PTR			m_nPrjMajorNum;
	// 	INT_PTR			m_nPrjMinorNum;
	// 	CString			m_strPrjPath;
	// 	CString			m_strDBName;
	// 	CString			m_strDBPwd;
	// 	CString			m_strDBUser;
	// 	COleDateTime	m_dtCreate;
	file.WriteString(m_pFasSysData->GetPrjName() + L"\n");
	file.WriteString(m_pFasSysData->GetSiteName() + L"\n");
	file.WriteString(m_pFasSysData->GetSitePhone() + L"\n");
	file.WriteString(m_pFasSysData->GetPrjMaker() + L"\n");
	strtemp.Format(L"%d.%d", m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum());
	file.WriteString(strtemp + L"\n");
	//file.WriteString(m_pFasSysData->GetPrjPath() + L"\n");	
	file.WriteString(m_pFasSysData->GetDBName() + L"\n");
	file.WriteString(m_pFasSysData->GetDBUser() + L"\n");
	file.WriteString(m_pFasSysData->GetDBPwd() + L"\n");
	file.WriteString(m_pFasSysData->GetPrjCreateDate().Format(L"%Y-%m-%d %H:%M:%S") + L"\n");

	//20240731 GBM - 프로젝트 수정 시간 적용 (주석 해제) 
	//-> 다시 주석처리 : 기존 SLP3 프로젝트와의 호환성을 위해 수정 시간 사용하지 않음, 
	//
// 	file.WriteString(m_pFasSysData->GetPrjModifyDate().Format(L"%Y-%m-%d %H:%M:%S") + L"\n");	
// 	file.WriteString(m_pFasSysData->GetPrjModifier() + L"\n");
	file.Close();

	return 1;
}


int CSysLinkerApp::SaveVersionInfoFile(WORD wMajor, WORD wMinor, CString strCurrentPrjRootFolder, BOOL bVerTemp)
{
	CStdioFileEx file;
	CString strtemp, strPath;
	if (strCurrentPrjRootFolder.Right(1) != '\\')
		strCurrentPrjRootFolder += L"\\";
	if(bVerTemp)
		strPath.Format(L"%s%s\\%s"
			, strCurrentPrjRootFolder
			, F3_VERSIONTEMPFOLDER_NAME
			, F3_VERSIONFINFO_FILE
		);
	else
		strPath.Format(L"%s%s\\%s"
			, strCurrentPrjRootFolder
			, GF_GetVersionFolderName(wMajor,wMinor)
			, F3_VERSIONFINFO_FILE
		);

	if (!file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CStdioFileEx::modeWriteUnicode))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 파일을 생성하는데 실패했습니다.\n");
#else
		GF_AddLog(L"Failed to create the project file.\n");
#endif
		return 0;
	}
	strtemp.Format(L"%d.%d", m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum());
	file.WriteString(strtemp + L"\n");
	file.WriteString(m_pFasSysData->GetPrjModifyDate().Format(L"%Y-%m-%d %H:%M:%S") + L"\n");
	file.WriteString(m_pFasSysData->GetPrjModifier() + L"\n");
	file.Close();
	return 1;
}



int CSysLinkerApp::OpenProjectInfoFile(CRelayTableData * pFasSysData , CString strPrjRootFolder, CString strPrjName)
{
	CStdioFileEx file;
	CString strtemp, strPath;
	CString strSiteName, strSitePhone, strPrjMaker, strPrjPath;
	CString strDBName, strDBUser, strDBPwd, strCreateDate;
	COleDateTime dtCreate;
	if (strPrjRootFolder.Right(1) != '\\')
		strPrjRootFolder += L"\\";
	strPath.Format(L"%s\\%s.%s"
		, strPrjRootFolder, strPrjName, F3_PROJECT_EXT);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 파일을 생성하는데 실패했습니다.\n");
#else
		GF_AddLog(L"Failed to create the project file.\n");
#endif
		return 0;
	}
	// Project Version Temp Folder 생성
	// DB 생성 : Main DB class에서 Version DB생성하고 Project Name_temp
	// 저장시 이전 버전 File로 저장
	file.ReadString(strtemp); //GetPrjName
	strPrjName = strtemp;
	file.ReadString(strtemp); //GetSiteName
	strSiteName = strtemp;
	file.ReadString(strtemp); //GetSitePhone
	strSitePhone = strtemp;
	file.ReadString(strtemp); //GetPrjMaker
	strPrjMaker = strtemp;
	file.ReadString(strtemp); //GetPrjMajorNum GetPrjMinorNum
	file.ReadString(strtemp); //GetDBName
	strDBName = strtemp;
	file.ReadString(strtemp); //GetDBUser
	strDBUser = strtemp;
	file.ReadString(strtemp); //GetDBPwd
	strDBPwd = strtemp;
	file.ReadString(strtemp); //GetPrjCreateDate
	strCreateDate = strtemp;
	dtCreate.ParseDateTime(strCreateDate);
	m_pFasSysData->SetProjectInfo(strPrjName, strSiteName, dtCreate
		, strPrjMaker, strSitePhone, strDBName);

	file.Close();
	return 1;
}

int CSysLinkerApp::OpenVersionInfoFile(CRelayTableData * pFasSysData , WORD wMajor, WORD wMinor, CString strPrjRootFolder
	, CString strPrjName, BOOL bVerTemp)
{
	CStdioFileEx file;
	std::vector<CString> vtArr;
	CString strtemp, strPath;
	WORD w1, w2;
	COleDateTime dtModify;
	if (strPrjRootFolder.Right(1) != '\\')
		strPrjRootFolder += L"\\";
// 	strPath.Format(L"%s\\%s %d.%d\\%s"
// 		, strPrjRootFolder
// 		, F3_VERSIONFOLDER_NAME, wMajor, wMinor
// 		, F3_VERSIONFINFO_FILE
// 	);

	if (bVerTemp)
		strPath.Format(L"%s%s\\%s"
			, strPrjRootFolder
			, F3_VERSIONTEMPFOLDER_NAME
			, F3_VERSIONFINFO_FILE
		);
	else
		strPath.Format(L"%s%s\\%s"
			, strPrjRootFolder
			, GF_GetVersionFolderName(wMajor, wMinor)
			, F3_VERSIONFINFO_FILE
		);

	if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 파일을 생성하는데 실패했습니다.\n");
#else
		GF_AddLog(L"Failed to create the project file.\n");
#endif
		return 0;
	}
	file.ReadString(strtemp); //GetPrjMajorNum GetPrjMinorNum
	vtArr = GF_SplitString(strtemp, L".");
	if (vtArr.size() < 2)
		return 0;
	w1 = _wtoi(vtArr[0]);
	w2 = _wtoi(vtArr[1]);
	file.ReadString(strtemp); //GetPrjModifyDate
	dtModify.ParseDateTime(strtemp);
	file.ReadString(strtemp); //GetPrjModifier

	pFasSysData->SetPrjVersion(w1, w2);
	pFasSysData->SetPrjModifyDate(dtModify);
	pFasSysData->SetPrjModifier(strtemp);
	file.Close();
	return 1;
}

int CSysLinkerApp::RegisterBasicDB()
{
	CString strDB, strlog,strAppPath;
	YAdoDatabase * pDB = new YAdoDatabase;
	pDB->MSSqlInitialize(g_stConfig.szDBPass, g_stConfig.szDBUser
		, L"master", g_stConfig.szSource);
	if (pDB->DBOpen() == FALSE)
	{
		delete pDB;
		return 0;
	}
	strAppPath = g_strAppPath;
	if (strAppPath.Right(1) != L'\\')
		strAppPath += L'\\';
#ifdef _DEBUG
	strDB = L"D:\\04. Ficon3\\11. Base Database\\MSSQL_BASE_DATABASE\\GFS_BASE_Data.mdf";
	strlog = L"D:\\04. Ficon3\\11. Base Database\\MSSQL_BASE_DATABASE\\GFS_BASE_log.ldf";
#else
	strDB.Format(L"%sDB\\%s_DATA.MDF", strAppPath, g_stConfig.szDBName);
	strlog.Format(L"%sDB\\%s_LOG.LDF", strAppPath, g_stConfig.szDBName);
#endif
	//ATTACH
	if (pDB->AttachMSDB(g_stConfig.szDBName, strDB, strlog) == FALSE)
	{
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"프로그램 기초 데이터베이스(%s)를 등록하는데 실패했습니다.\n%s"
			, g_stConfig.szDBName, pDB->GetLastErrorString());
#else
		strError.Format(L"Failed to register the default program database (%s).\n%s"
			, g_stConfig.szDBName, pDB->GetLastErrorString());
#endif
		GF_AddLog(strError);
		AfxMessageBox(strError);
		pDB->DBClose();
		delete pDB;
		return 0;
	}

	pDB->DBClose();
	delete pDB;
	return 1;
}

int CSysLinkerApp::OpenBaseDatabase()
{
// 	if (m_pFasSysData == nullptr)
// 		m_pFasSysData = new CRelayTableData;
	
	if (m_pMainDb == nullptr )
		m_pMainDb = new YAdoDatabase;
	if (m_pMainDb->IsOpen() == FALSE)
	{
		m_pMainDb->MSSqlInitialize(g_stConfig.szDBPass, g_stConfig.szDBUser
			, g_stConfig.szDBName, g_stConfig.szSource);

		if (m_pMainDb->DBOpen() == FALSE)
		{
			// 기초정보 등록
			// MainDB 재접속
			if (RegisterBasicDB() <= 0)
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"기초정보 데이터베이스를 서버에 등록하는데 실패했습니다.");
				AfxMessageBox(L"기초정보 데이터베이스를 서버에 등록하는데 실패했습니다.\r\nMSSQL Server를 설치하거나 서비스틀 실행해주시기 바랍니다.");
#else
				GF_AddLog(L"Failed to register the default information database with the server.");
				AfxMessageBox(L"Failed to register the default information database with the server.\nPlease install MSSQL Server or start the service.");
#endif
				return 0;
			}
			if (m_pMainDb->DBOpen() == FALSE)
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"기초정보 데이터베이스 접속에 실패했습니다.");
				AfxMessageBox(L"데이터베이스 접속에 실패했습니다.");
#else
				GF_AddLog(L"Failed to access the default information database.");
				AfxMessageBox(L"Failed to connect to the database.");
#endif
				return 0;
			}
		}
	}
	return 1;
}

int CSysLinkerApp::OpenProjectDatabase(CRelayTableData * pFasSysData)
{
	//////////////////////////////////////////////////////////////////////////
	// Database 생성 
	// 1. Main Database 연결
	// 2. Project Database 등록 , 연결
	CString strSql;
	CString strDBPath, strPath;
	CString strBkName , strDB , strLog;
	int nRet = 0;
	if (m_pMainDb == nullptr)
	{
		if (OpenBaseDatabase() == 0)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"기초정보 데이터베이스를 여는데 실패했습니다.");
#else
			GF_AddLog(L"Failed to open the default information database.");
#endif
			return 0;
		}
	}

	strPath = g_stConfig.szPrjPath;
	if (strPath.Right(1) != '\\')
		strPath += L"\\";
	strDBPath.Format(L"%s%s\\%s\\%s\\"
		, strPath, m_pFasSysData->GetDBName()
		, F3_VERSIONTEMPFOLDER_NAME
		, F3_PRJ_DIR_DATABASE
	);

	// 이미 등록되어 있으면 DB분리
	if (m_pMainDb->IsAttachedDatabase(pFasSysData->GetDBName()) == TRUE)
	{
		if (m_pMainDb->DetachMSDB(pFasSysData->GetDBName()) == FALSE)
		{
			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"프로젝트 데이터베이스(%s)를 분리하는데 실패했습니다.\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
#else
			strError.Format(L"Failed to separate the project database (%s).\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
#endif
			GF_AddLog(strError);
			AfxMessageBox(strError);
			return -1;
		}
	}
	strDB.Format(L"%s_DATA.MDF", pFasSysData->GetDBName());
	if (GF_FindFile(strDBPath, strDB) == TRUE) // DB 등록 필요
	{
		strDB.Format(L"%s%s_DATA.MDF", strDBPath, pFasSysData->GetDBName());
		strLog.Format(L"%s%s_LOG.LDF", strDBPath, pFasSysData->GetDBName());
		//ATTACH
		if (m_pMainDb->AttachMSDB(pFasSysData->GetDBName(), strDB, strLog) == FALSE)
		{

			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"프로젝트 데이터베이스(%s)를 등록하는데 실패했습니다.\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
#else
			strError.Format(L"Failed to register the project database (%s).\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
#endif
			GF_AddLog(strError);
			AfxMessageBox(strError);
			return -2;
		}
	}
	else // Restore 필요
	{
		strBkName.Format(L"%s.BAK", pFasSysData->GetDBName());
		if (m_pMainDb->RestoreDatabase(pFasSysData->GetDBName(), strDBPath, strBkName) == FALSE)
		{

			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"프로젝트 데이터베이스(%s)를 복원하는데 실패했습니다.\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
#else
			strError.Format(L"Failed to restore the project database (%s).\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
#endif
			GF_AddLog(strError);
			AfxMessageBox(strError);
			return -3;
		}
	}
	
	if (pFasSysData->OpenPrjDatabase(g_stConfig.szSource, m_pFasSysData->GetDBName()
		, 1433, g_stConfig.szDBUser, g_stConfig.szDBPass) <= 0)
	{
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"프로젝트 데이터베이스(%s)를 연결하는데 실패했습니다.\n%s"
			, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
#else
		strError.Format(L"Failed to connect the project database (%s).\n%s"
			, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
#endif
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return -3;
	}
	return 1;
}

int CSysLinkerApp::OpenProject(CString strPrjName, CString strPrjFullPath, DWORD dwVer, BOOL bVerTemp)
{
	// Project 복사 
	// 로그인 프로젝트 --> Version_Temp
	if (strPrjFullPath.Right(1) != '\\')
		strPrjFullPath += L"\\";

	if(bVerTemp == FALSE)
	{
		if (CopyProjectVersionTemp(strPrjName, strPrjFullPath, dwVer) <= 0)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"프로젝트 정보를 가져오는데 실패했습니다.");
			AfxMessageBox(L"프로젝트 정보를 가져오는데 실패했습니다.");
#else
			GF_AddLog(L"Failed to retrieve the project information.");
			AfxMessageBox(L"Failed to retrieve the project information.");
#endif
			return -1;
		}
	}

	if (m_pFasSysData == nullptr)
	{
		m_pFasSysData = new CRelayTableData;
	}

	OpenProjectInfoFile(m_pFasSysData , strPrjFullPath, strPrjName);
	OpenVersionInfoFile(m_pFasSysData, HIWORD(dwVer), LOWORD(dwVer), strPrjFullPath, strPrjName,bVerTemp);

	//20240523 GBM start - 프로젝트 로드 시 가장 시간이 많이 걸리는 아래 행정을 스레드로 대체
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("        프로젝트 DB를 여는 중입니다. 잠시 기다려 주세요.");
#else
	CString strMsg = _T("        Opening the project DB. Wait for a moment.");
#endif
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadOpenProjectDatabase, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 DB 열기에 성공했습니다.");
#else
		GF_AddLog(L"Successfully opened the project DB.");
#endif
		Log::Trace("Successfully opened the project DB.");
	}
	else
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트 DB 열기에 실패했습니다.");
#else
		GF_AddLog(L"Failed to open the project DB.");
#endif
		Log::Trace("Failed to open project DB.");
	}

#else
	if (OpenProjectDatabase(m_pFasSysData) < 0)
	{
		GF_AddLog(L"프로젝트 데이터베이스를 여는데 실패 했습니다.");
		return 0;
	}

	//20240327 GBM start - DB Open 후 DB에서 데이터를 가져오기 전에 GT1 추가 입력 타입을 추가 -> 중계기 일람표(WEB)에서 회로 관련 설비 정의를 가지고 오기 때문에 이 행정을 하지 않음

	// 	CNewDBManager::Instance()->SetDBAccessor(theApp.m_pFasSysData->m_pDB);
	// 
	// 	BOOL bRet = FALSE;
	// 
	// 	bRet = CNewDBManager::Instance()->CheckAndInsertEquipmentNewInputType();
	// 	if (bRet)
	// 	{
	// 		GF_AddLog(L"GT1 입력 타입 자동 추가에 성공했습니다.");
	// 		Log::Trace("Inserting a new input type of equipment succeeded!");
	// 	}
	// 	else
	// 	{
	// 		GF_AddLog(L"GT1 입력 타입 자동 추가에 실패했습니다. 사용 중인 입력 타입을 확인하세요.");
	// 		Log::Trace("Inserting a new input type of equipment failed!");
	// 	}

	//20240327 GBM end

	if (m_pFasSysData->LoadProjectDatabase() == 0)
	{
		GF_AddLog(L"프로젝트 데이터베이스에서 데이터를 가져오는 데에 실패 했습니다.");
		return 0;
	}
	m_pFasSysData->SetProjectOpened(TRUE);
	//GF_AddLog(L"프로젝트를 불러오는데 성공 했습니다.");	//20240424 GBM - 중복 메세지 삭제
#endif
	//20240327 GBM end

	return 1;
}


int CSysLinkerApp::CloseProject()
{
	PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_CLOSE, 0);
	CloseFormView(FV_LINKVIEW);
	CloseFormView(FV_MAKEPATTERN);
	CloseFormView(FV_PSWITCH);
	CloseFormView(FV_SWITCH);
	CloseFormView(FV_PUMP);
	CloseFormView(FV_EQUIP);
	CloseFormView(FV_USER);
	CloseFormView(FV_ACCESS);
	CloseFormView(FV_EMERGENCY);
	CloseFormView(FV_LOCATION);
	CloseFormView(FV_FACP);
	CloseFormView(FV_UNIT);
	CloseFormView(FV_RELAYEDIT);
	CloseFormView(FV_AUTOMAKE);
	CloseFormView(FV_LOGICEDIT);
	CloseFormView(FV_LOADRELAYTABLE);

	//AfxGetApp()->CloseAllDocuments(TRUE);
	if (m_pFasSysData == nullptr)
		return 0;

	if (m_pFasSysData->GetChangeFlag() == TRUE)
	{
#ifndef ENGLISH_MODE
		if (AfxMessageBox(L"변경사항이 있습니다.\n프로젝트를 저장하시겠습니까?"
			, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnHomeProjectSave();
		}
#else
		if (AfxMessageBox(L"There have been changes.\nDo you want to save the project ?"
			, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnHomeProjectSave();
		}
#endif
	}

	m_pMainDb->DetachMSDB(m_pFasSysData->GetDBName());

	delete m_pFasSysData;
	m_pFasSysData = nullptr;

	//20240307 GBM start - GT1 추가 정보 메모리 초기화
	memset(&CNewInfo::Instance()->m_gi, NULL, sizeof(GT1APPENDIX_INFO));
	memset(&CNewInfo::Instance()->m_ei, NULL, sizeof(EQUIPMENT_INFO));
	//20240307 GBM end

	//20250617 GBM start - 수동 연동데이터 일괄 편집 기능
	//수동복사 입력회로 및 연동데이터 정보 초기화
	CManualLinkManager::Instance()->InitManualCopyData();
	//20250617 GBM end

	return 1;
}


int CSysLinkerApp::RemoveTemplate()
{
	if (m_pTempleMakeLink != nullptr)
	{
		delete m_pTempleMakeLink;
		m_pTempleMakeLink = nullptr;
	}

	if (m_pTempleMakePtn != nullptr)
	{
		delete m_pTempleMakePtn;
		m_pTempleMakePtn = nullptr;
	}

	if (m_pTemplePSwitch != nullptr)
	{
		delete m_pTemplePSwitch;
		m_pTemplePSwitch = nullptr;
	}

	if (m_pTempleSwitch != nullptr)
	{
		delete m_pTempleSwitch;
		m_pTempleSwitch = nullptr;
	}

	if (m_pTempleLinkView != nullptr)
	{
		delete m_pTempleLinkView;
		m_pTempleLinkView = nullptr;
	}

	if (m_pTemplePump != nullptr)
	{
		delete m_pTemplePump;
		m_pTemplePump = nullptr;
	}

	if (m_pTempleEquip != nullptr)
	{
		delete m_pTempleEquip;
		m_pTempleEquip = nullptr;
	}

	if (m_pTempleUser != nullptr)
	{
		delete m_pTempleUser;
		m_pTempleUser = nullptr;
	}

	if (m_pTempleAccess != nullptr)
	{
		delete m_pTempleAccess;
		m_pTempleAccess = nullptr;
	}

	if (m_pTempleEmergency != nullptr)
	{
		delete m_pTempleEmergency;
		m_pTempleEmergency = nullptr;
	}

	if (m_pTempleLocation != nullptr)
	{
		delete m_pTempleLocation;
		m_pTempleLocation = nullptr;
	}

	if (m_pTempleFacp != nullptr)
	{
		delete m_pTempleFacp;
		m_pTempleFacp = nullptr;
	}

	if (m_pTempleUnit != nullptr)
	{
		delete m_pTempleUnit;
		m_pTempleUnit = nullptr;
	}

	if (m_pTempleAutoMake != nullptr)
	{
		delete m_pTempleAutoMake;
		m_pTempleAutoMake = nullptr;
	}

	if (m_pTempleLogicEdit != nullptr)
	{
		delete m_pTempleLogicEdit;
		m_pTempleLogicEdit = nullptr;
	}

	if (m_pTempleLoadRealy != nullptr)
	{
		delete m_pTempleLoadRealy;
		m_pTempleLoadRealy = nullptr;
	}

	if(m_pTempleErrorCheck != nullptr)
	{
		delete m_pTempleErrorCheck;
		m_pTempleErrorCheck = nullptr;
	}
	return 1;
}
int CSysLinkerApp::CopyProjectVersionTemp(CString strPrjName, CString strPrjFullPath, DWORD dwVer)
{
	CString strFrom, strTo, strFile, strError = L"";
	//CString strPath = g_stConfig.szPrjPath;
	SHFILEOPSTRUCT shStruct;

	TCHAR pszFrom[4096] = { 0 }, pszTo[4096] = { 0 };
	ZeroMemory(&shStruct, sizeof(SHFILEOPSTRUCT));

	if (strPrjFullPath.Right(1) != '\\')
		strPrjFullPath += L"\\";
	strFrom.Format(L"%s%s\\"
		, strPrjFullPath, GF_GetVersionFolderName(HIWORD(dwVer), LOWORD(dwVer)));
	strTo.Format(L"%s%s\\"
		, strPrjFullPath,F3_VERSIONTEMPFOLDER_NAME
	);
	//////////////////////////////////////////////////////////////////////////
	// 1. C:\\FICON3\\SYMBOLE 에서 Version folder안에 복사.
	_tcscpy_s(pszTo, 4096, strTo);
	_tcscpy_s(pszFrom, 4096, strFrom);
	pszTo[strTo.GetLength()] = '\0';
	pszTo[strTo.GetLength() + 1] = '\0';
	pszFrom[strFrom.GetLength()] = '\0';
	pszFrom[strFrom.GetLength() + 1] = '\0';

	shStruct.fFlags = FOF_NOCONFIRMATION| FOF_MULTIDESTFILES | FOF_SILENT;
	//shStruct.fFlags = FOF_MULTIDESTFILES;
	//shStruct.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	shStruct.hwnd = NULL;
	shStruct.pTo = pszTo;
	shStruct.pFrom = pszFrom;
	shStruct.wFunc = FO_COPY;
	shStruct.fAnyOperationsAborted = NULL;
	shStruct.hNameMappings = NULL;
	shStruct.lpszProgressTitle = NULL;
	int nRet = SHFileOperation(&shStruct);
	if (nRet != 0)
	{
		strError = GF_GetSHErrorString(nRet);
		AfxMessageBox(strError);
		return 0;
	}
	return 1;
}

int CSysLinkerApp::CopyVersionTempToVersion(CString strPrjName, CString strPrjFullPath, WORD wMajor, WORD wMinor)
{
	CString strFrom, strTo, strFile, strError = L"";
	SHFILEOPSTRUCT shStruct;
	TCHAR pszFrom[4096] = { 0 }, pszTo[4096] = { 0 };
	ZeroMemory(&shStruct, sizeof(SHFILEOPSTRUCT));

	//////////////////////////////////////////////////////////////////////////
	// 1. Version_Temp\\ver.pvi 이동
	// 2. ETC 폴더 이동
	// 3. MAP 폴덩 이동
	// 4. RELAYTABLE 
	// 5. RESOURCE
	// 6. SYMBOL
	// 7. DB폴더 생성

	if (strPrjFullPath.Right(1) != '\\')
		strPrjFullPath += L"\\";
	
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_VERSIONFINFO_FILE
	);
	//////////////////////////////////////////////////////////////////////////
	// 1. Version folder 생성 Version_Temp\\ver.pvi 이동
	strTo.Format(L"%s%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
	);
	if (CreateDirectory(strTo,nullptr) == FALSE)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"버전 폴더를 생성하는데 실패했습니다.");
			AfxMessageBox(L"버전 폴더를 생성하는데 실패했습니다.");
#else
			GF_AddLog(L"Failed to create a version folder.");
			AfxMessageBox(L"Failed to create a version folder.");
#endif
			return 0;
		}
	}

	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_VERSIONFINFO_FILE
	);
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_VERSIONFINFO_FILE
	);
	if (CopyFile(strFrom , strTo, FALSE) == FALSE)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"버전정보 파일을 이동하는데 실패했습니다.");
		AfxMessageBox(L"버전정보 파일을 이동하는데 실패했습니다.");
#else
		GF_AddLog(L"Failed to move the version information file.");
		AfxMessageBox(L"Failed to move the version information file.");
#endif
		return 0; 
	}
	//////////////////////////////////////////////////////////////////////////
	// 2. ETC 폴더 이동
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_ETC
	);
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_PRJ_DIR_ETC
	);
	strError = GF_CopyDir(strTo, strFrom);
	if (strError != L"")
	{
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// 3. MAP 폴덩 이동
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_MAP
	);
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_PRJ_DIR_MAP
	);
	strError = GF_CopyDir(strTo, strFrom);
	if (strError != L"")
	{
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	// 4. RELAYTABLE 
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_RELAYTABLE
	);
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_PRJ_DIR_RELAYTABLE
	);
	strError = GF_CopyDir(strTo, strFrom);
	if (strError != L"")
	{
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	// 5. RESOURCE
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_RESOURCES
	);
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_PRJ_DIR_RESOURCES
	);
	strError = GF_CopyDir(strTo, strFrom);
	if (strError != L"")
	{
		AfxMessageBox(strError);
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	// 6. SYMBOL
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_SYMBOL
	);
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_PRJ_DIR_SYMBOL
	);
	strError = GF_CopyDir(strTo, strFrom);
	if (strError != L"")
	{
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// 7. DB폴더 생성
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_DATABASE
	);
	if (CreateDirectory(strTo, nullptr) == FALSE)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"데이터베이스 폴더를 생성하는데 실패했습니다.");
			AfxMessageBox(L"데이터베이스 폴더를 생성하는데 실패했습니다.");
#else
			GF_AddLog(L"Failed to create a database folder.");
			AfxMessageBox(L"Failed to create a database folder.");
#endif
			return 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 8. RELEASE 폴더 생성
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_RELEASE
	);
	if (CreateDirectory(strTo, nullptr) == FALSE)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"RELEASE 폴더를 생성하는데 실패했습니다.");
			AfxMessageBox(L"RELEASE 폴더를 생성하는데 실패했습니다.");
#else
			GF_AddLog(L"Failed to create a RELEASE folder.");
			AfxMessageBox(L"Failed to create a RELEASE folder.");
#endif
			return 0;
		}
	}

	//20251014 GBM start - 로직 편집 정보와 출력 내용 이미지들이 들어갈 폴더 추가
	//////////////////////////////////////////////////////////////////////////
	// 9. LOGIC
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_LOGIC
	);
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_PRJ_DIR_LOGIC
	);
	strError = GF_CopyDir(strTo, strFrom);
	if (strError != L"")
	{
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return 0;
	}
	//20251014 GBM end

	return 1;
}


//void CAboutDlg::OnChkViewdkoutput()
//{
//	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//}


void CSysLinkerApp::OnFileClose()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CSysLinkerApp::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


int CSysLinkerApp::MakeCustomTreeViewItem(BOOL bOutView)
{
	CString str,strView;
	TVORDER *pTo;
	int i, nSize;
	if (bOutView)
		pTo = &g_tvOutput;
	else
		pTo = &g_tvInput;
	nSize = (int)pTo->size();
	strView = L"";
	for (i = 0; i < nSize; i++)
	{
		str.Format(L"%d.", (*pTo)[i]);
		strView += str;
	}
	if (strView.Right(1) == '.')
		strView.Delete(strView.GetLength() - 1, 1);

	if (bOutView)
		wcscpy_s(g_stConfig.szOutput, sizeof(g_stConfig.szOutput) / sizeof(TCHAR), strView);
	else
		wcscpy_s(g_stConfig.szInput, sizeof(g_stConfig.szInput) / sizeof(TCHAR), strView);

	return 1;
}

int CSysLinkerApp::SaveCustomTreeViewItem(BOOL bOutView)
{
	CString str;
	CString strPath = g_strAppPath;
	if (strPath.Right(1) != '\\')
		strPath += "\\";
	strPath += "Config\\SysLinker.INI";
	BOOL bRet = 0;
	MakeCustomTreeViewItem(bOutView);
	if (bOutView)
	{
		bRet = WritePrivateProfileString(L"TREEVIEW", L"OUTPUT", g_stConfig.szOutput, strPath);
		if (bRet == FALSE)
			return 0;
	}
	else
	{
		bRet = WritePrivateProfileString(L"TREEVIEW", L"INPUT", g_stConfig.szInput, strPath);
		if (bRet == FALSE)
			return 0;
	}
	return 1;
}
int CSysLinkerApp::ViewSelectItem()
{
	if (g_stViewSelectItem.bInit == FALSE)
		return 0; 
	int i = 0; 
	CView * pView = nullptr;
	for (i = 0; i < 30; i++)
	{
		if (g_stViewSelectItem.bOpenComplete == TRUE)
			break;
		Sleep(1000);
	}
	CMultiDocTemplate * pDocTemp = nullptr;
	if (g_stViewSelectItem.nViewType == FV_MAKELINKER)
	{
		pDocTemp = m_pTempleMakeLink;
	}
	else if (g_stViewSelectItem.nViewType == FV_MAKEPATTERN)
	{
		pDocTemp = m_pTempleMakePtn;
	}
	else if (g_stViewSelectItem.nViewType == FV_PSWITCH)
	{
		pDocTemp = m_pTemplePSwitch;
	}
	else if (g_stViewSelectItem.nViewType == FV_SWITCH)
	{
		pDocTemp = m_pTempleSwitch;
	}
	else if (g_stViewSelectItem.nViewType == FV_LINKVIEW)
	{
		pDocTemp = m_pTempleLinkView;
	}
	else if (g_stViewSelectItem.nViewType == FV_PUMP)
	{
		pDocTemp = m_pTemplePump;
	}
	else if (g_stViewSelectItem.nViewType == FV_EQUIP)
	{
		pDocTemp = m_pTempleEquip;
	}
	else if (g_stViewSelectItem.nViewType == FV_USER)
	{
		pDocTemp = m_pTempleUser;
	}
	else if (g_stViewSelectItem.nViewType == FV_ACCESS)
	{
		pDocTemp = m_pTempleAccess;
	}
	else if (g_stViewSelectItem.nViewType == FV_EMERGENCY)
	{
		pDocTemp = m_pTempleEmergency;
	}
	else if (g_stViewSelectItem.nViewType == FV_LOCATION)
	{
		pDocTemp = m_pTempleLocation;
	}
	else if (g_stViewSelectItem.nViewType == FV_FACP)
	{
		pDocTemp = m_pTempleFacp;
	}
	else if (g_stViewSelectItem.nViewType == FV_UNIT)
	{
		pDocTemp = m_pTempleUnit;
	}
	else if (g_stViewSelectItem.nViewType == FV_RELAYEDIT)
	{
		pDocTemp = m_pTempleRelayEdit;
	}
	else if (g_stViewSelectItem.nViewType == FV_AUTOMAKE)
	{
		pDocTemp = m_pTempleAutoMake;
	}
	else if (g_stViewSelectItem.nViewType == FV_LOGICEDIT)
	{
		pDocTemp = m_pTempleLogicEdit;
	}
	if (pDocTemp == NULL)
		return 0;

	pView = GetSysLinkerView(pDocTemp);
	if (pView == nullptr)
		return 0;
	switch (g_stViewSelectItem.nViewType)
	{
	case FV_MAKELINKER:
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY, TTYPE_DEV_LINKED_TARGETITEM, (LPARAM)g_stViewSelectItem.dwData);
		break;
	case FV_MAKEPATTERN:
		((CFormPattern*)pView)->SelectItem(g_stViewSelectItem.nItemType, g_stViewSelectItem.dwData);
		break;
	case FV_PSWITCH:
		//((CFormPSwitch*)pView)->SelectItem((CDataLinked*)pItem);
		break;
	case FV_PUMP:
		//((CFormPump*)pView)->SelectItem((CDataLinked*)pItem);
		break;
	case FV_EMERGENCY:
		((CFormEmergency*)pView)->SelectItem(g_stViewSelectItem.nItemType, g_stViewSelectItem.dwData);
		break;
	}
	return 1; 
}

DWORD CSysLinkerApp::Thread_ViewSelectItem(LPVOID lpData)
{
	CSysLinkerApp	*me;
	int nRet = 0; 
	me = (CSysLinkerApp *)lpData;
	if ((CSysLinkerApp *)NULL == me)
	{
		return 0;
	}

	try
	{
		nRet = me->ViewSelectItem();
	}
	catch (...)
	{
	}

	return 0;
}

int CSysLinkerApp::ViewFormSelectItem(FormViewStyle nViewType , int nItemType , DWORD_PTR dwData)
{
	CView * pView = nullptr;
	memset((void*)&g_stViewSelectItem, 0, sizeof(ST_VIEWFORM_SELECTITEM));
	g_stViewSelectItem.bInit = TRUE;
	g_stViewSelectItem.dwData = dwData;
	g_stViewSelectItem.nItemType = nItemType;
	g_stViewSelectItem.nViewType = nViewType;
	g_stViewSelectItem.bOpenComplete = FALSE;
	pView = OpenFormView(nViewType);
	if (pView == nullptr)
	{
		AfxBeginThread((AFX_THREADPROC)Thread_ViewSelectItem, (LPVOID)this);
		return 0;
	}
	
	switch (nViewType)
	{
	case FV_MAKELINKER:
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY, TTYPE_DEV_LINKED_TARGETITEM, (LPARAM)dwData);
		break;
	case FV_MAKEPATTERN:
		((CFormPattern*)pView)->SelectItem(nItemType,dwData);
		break;
	case FV_PSWITCH:
		//((CFormPSwitch*)pView)->SelectItem((CDataLinked*)pItem);
		break;
	case FV_PUMP:
		//((CFormPump*)pView)->SelectItem((CDataLinked*)pItem);
		break;
	case FV_EMERGENCY:
		((CFormEmergency*)pView)->SelectItem(nItemType , dwData);
		break;
	case FV_LOCATION:
		((CFormLocation*)pView)->SelectItem(nItemType,dwData);
		break;
	}

	return 0;
}


void CSysLinkerApp::InsertLog(CString strLog)
{
#if _USE_OUTPUTWND_
	if (g_pLogWnd == nullptr)
		return;
	g_pLogWnd->InsertLogList(strLog);
#else
	return; 
#endif


}

void CSysLinkerApp::InsertDebug(CString strLog)
{
#if _USE_OUTPUTWND_
	if (g_pLogWnd == nullptr)
		return;
	g_pLogWnd->InsertDebugList(strLog);
#else
	return;
#endif

}

BOOL CSysLinkerApp::InitEquipmentInfoTable(CString strEquipmentDefinitionFile)
{
	CExcelWrapper xls;
	if (!xls.Open(strEquipmentDefinitionFile))
	{
		return FALSE;
	}

	CNewExcelManager::Instance()->ParsingEquipmentInfo(&xls);
	CNewDBManager::Instance()->SetDBAccessor(m_pMainDb);
	CNewDBManager::Instance()->DeleteEquipmentCircuitInfoFromEquipmentInfoTable();
	CNewDBManager::Instance()->InsertDataIntoEquipmentInfoTable();
	memset(&CNewInfo::Instance()->m_ei, NULL, sizeof(EQUIPMENT_INFO));
	xls.Close();
	
	return TRUE;
}

void CSysLinkerApp::OnUpdateFacpCreateLink(CCmdUI *pCmdUI)
{
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

void CSysLinkerApp::OnUpdateSimpleErrorCheck(CCmdUI *pCmdUI)
{
	if (pCmdUI)
	{
		if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

void CSysLinkerApp::SetAccountInfo(CString strEmail)
{
	CString strRole = CWebConnManager::Instance()->m_lri.role;

	CWebConnManager::Instance()->m_ai.email = strEmail;

	int nGrade = NO_GRADE;
	for (int i = GRADE_OPERATOR; i <= GRADE_CLIENT; i++)
	{
		if (strRole == g_szUserGrade[i])
		{
			nGrade = i;
			break;
		}
	}

	CWebConnManager::Instance()->m_ai.grade = nGrade;
}

BOOL CSysLinkerApp::WebServerLogin()
{
	BOOL bRet = FALSE;

	if (!CWebConnManager::Instance()->CheckInternetConnected())
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("인터넷이 연결되어 있지 않습니다.\r\n인터넷을 확인하고 프로그램을 다시 실행해 주세요."));
		Log::Trace("인터넷이 연결되어 있지 않습니다.\r\n인터넷을 확인하고 프로그램을 다시 실행해 주세요.");
#else
		AfxMessageBox(_T("The Internet is not connected.\r\nPlease check the Internet and run the program again."));
		Log::Trace("The Internet is not connected.\r\nPlease check the Internet and run the program again.");
#endif
		return FALSE;
	}

	CDlgWebLogin dlg;
	if (dlg.DoModal() == IDOK)
	{
		bRet = TRUE;

#ifndef ENGLISH_MODE
		AfxMessageBox(_T("로그인에 성공했습니다."), MB_OK | MB_ICONINFORMATION);
		Log::Trace("로그인에 성공했습니다.");
#else
		AfxMessageBox(_T("Login was successful.", MB_OK | MB_ICONINFORMATION));
		Log::Trace("Login was successful.");
#endif

		// 로그인이 성공했을 때 계정 정보를 넣어 둠
		SetAccountInfo(dlg.m_strEditEmail);

		// 로그인 정보 메인 출력 창에 표시
		CString strAccount = _T("");

#ifdef SLP4_MODE
#ifndef ENGLISH_MODE
		strAccount.Format(_T("[%s] 계정으로 로그인 되었습니다."), CWebConnManager::Instance()->m_ai.email);
#else
		strAccount.Format(_T("You have been logged in with account [%s]"), CWebConnManager::Instance()->m_ai.email);
#endif
		GF_AddLog(strAccount);
		Log::Trace("%s", CCommonFunc::WCharToChar(strAccount.GetBuffer(0)));
#endif
	}
	else
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("로그인을 취소했기 때문에 프로그램을 종료합니다."));
		Log::Trace("로그인을 취소했기 때문에 프로그램을 종료합니다.");
#else
		AfxMessageBox(_T("The program will close because you canceled your login."));
		Log::Trace("The program will close because you canceled your login.");
#endif
	}

	return bRet;
}