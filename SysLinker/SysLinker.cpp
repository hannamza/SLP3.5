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
	m_strVersion = GFSP_PRODUCT_UPDATE_DATE;
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
	//ON_UPDATE_COMMAND_UI(ID_FACP_CREATE_LINK, &CSysLinkerApp::OnUpdateFacpCreateLink)
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

	//20240118 GBM start - github test
	int a = 0;
	//20240118 GBM end

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("GFS\\FICON3\\SLP3"));
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
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
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
	//20240202 GBM end

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
	Log::Cleanup();
	//20240202 GBM end

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
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
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
	m_pTempleMakeLink = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CSysLinkerDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CSysLinkerView));
	if (!m_pTempleMakeLink)
		return FALSE;
	AddDocTemplate(m_pTempleMakeLink);

	m_pTempleMakePtn = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormPattern));
	if (!m_pTempleMakePtn)
		return FALSE;
	AddDocTemplate(m_pTempleMakePtn);



	m_pTemplePSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormPSwitch));
	if (!m_pTemplePSwitch)
		return FALSE;
	AddDocTemplate(m_pTemplePSwitch);



	m_pTempleSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormSwitch));
	if (!m_pTempleSwitch)
		return FALSE;
	AddDocTemplate(m_pTempleSwitch);



	m_pTempleLinkView = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLinkView));
	if (!m_pTempleLinkView)
		return FALSE;
	AddDocTemplate(m_pTempleLinkView);



	m_pTemplePump = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormPump));
	if (!m_pTemplePump)
		return FALSE;
	AddDocTemplate(m_pTemplePump);


	m_pTempleEquip = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEquip));
	if (!m_pTempleEquip)
		return FALSE;
	AddDocTemplate(m_pTempleEquip);


	m_pTempleUser = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormUser));
	if (!m_pTempleUser)
		return FALSE;
	AddDocTemplate(m_pTempleUser);

	m_pTempleAccess = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormAccess));
	if (!m_pTempleAccess)
		return FALSE;
	AddDocTemplate(m_pTempleAccess);

	m_pTempleEmergency = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEmergency));
	if (!m_pTempleEmergency)
		return FALSE;
	AddDocTemplate(m_pTempleEmergency);

	m_pTempleLocation = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLocation));
	if (!m_pTempleLocation)
		return FALSE;
	AddDocTemplate(m_pTempleLocation);

	m_pTempleFacp = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormFacp));
	if (!m_pTempleFacp)
		return FALSE;
	AddDocTemplate(m_pTempleFacp);

	m_pTempleUnit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormUnit));
	if (!m_pTempleUnit)
		return FALSE;

	m_pTempleRelayEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormRelayEdit));
	if (!m_pTempleRelayEdit)
		return FALSE;

	m_pTempleAutoMake = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormAutoMake));
	if (!m_pTempleAutoMake)
		return FALSE;
	m_pTempleLogicEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormEditLogic));
	if (!m_pTempleLogicEdit)
		return FALSE;
	m_pTempleLoadRealy = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormLoadRelayTable));
	if (!m_pTempleLoadRealy)
		return FALSE;

	m_pTempleErrorCheck = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CFormErrorCheck));
	if(!m_pTempleErrorCheck)
		return FALSE;

	AddDocTemplate(m_pTempleUnit);
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
		strTitle = L"연동데이터 편집";
	}
	else if (iStyle == FV_MAKEPATTERN)
	{
		pDocTemp = m_pTempleMakePtn;
		strTitle = L"패턴데이터 편집";
	}
	else if (iStyle == FV_PSWITCH)
	{
		pDocTemp = m_pTemplePSwitch;
		strTitle = L"압력스위치 편집";
	}
	else if (iStyle == FV_SWITCH)
	{
		pDocTemp = m_pTempleSwitch;
		strTitle = L"스위치 편집";
	}
	else if (iStyle == FV_LINKVIEW)
	{
		pDocTemp = m_pTempleLinkView;
		strTitle = L"연동데이터 보기";
	}
	else if (iStyle == FV_PUMP)
	{
		pDocTemp = m_pTemplePump;
		strTitle = L"펌프데이터 편집";
	}
	else if (iStyle == FV_EQUIP)
	{
		pDocTemp = m_pTempleEquip;
		strTitle = L"설비정보 편집";
	}
	else if (iStyle == FV_USER)
	{
		pDocTemp = m_pTempleUser;
		strTitle = L"사용자/사용자그룹 편집";
	}
	else if (iStyle == FV_ACCESS)
	{
		pDocTemp = m_pTempleAccess;
		strTitle = L"Access권한 편집";
	}
	else if (iStyle == FV_EMERGENCY)
	{
		pDocTemp = m_pTempleEmergency;
		strTitle = L"비상방송 편집";
	}
	else if (iStyle == FV_LOCATION)
	{
		pDocTemp = m_pTempleLocation;
		strTitle = L"위치정보 편집";
	}
	else if (iStyle == FV_FACP)
	{
		pDocTemp = m_pTempleFacp;
		strTitle = L"수신기정보 편집";
	}
	else if (iStyle == FV_UNIT)
	{
		pDocTemp = m_pTempleUnit;
		strTitle = L"유닛정보 편집";
	}
	else if (iStyle == FV_RELAYEDIT)
	{
		pDocTemp = m_pTempleRelayEdit;
		strTitle = L"회로 편집";
	}
	else if (iStyle == FV_AUTOMAKE)
	{
		pDocTemp = m_pTempleAutoMake;
		strTitle = L"연동데이터 자동생성";
	}
	else if (iStyle == FV_LOGICEDIT)
	{
		pDocTemp = m_pTempleLogicEdit;
		strTitle = L"연동데이터 자동생성 로직";
	}
	else if (iStyle == FV_LOADRELAYTABLE)
	{
		pDocTemp = m_pTempleLoadRealy;
		strTitle = L"중계기 일람표";
	}
	else if(iStyle == FV_ERRORCHECK)
	{
		pDocTemp = m_pTempleErrorCheck;
		strTitle = L"오류 검사";
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
	if (AfxMessageBox(L"현재 프로젝트를 닫으시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	CloseProject();
}

void CSysLinkerApp::OnHomeProjectNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CDlgNewProjectWizard dlg;

	if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
	{
		if (AfxMessageBox(L"새 프로젝트를 생성하시려면 현재 프로젝트를 닫아야 합니다\n현재 프로젝트를 닫으시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
		CloseProject();
	}

	CPropSheetNewProject psNewProject(L"New Project");
	if (m_pFasSysData == nullptr)
	{
		m_pFasSysData = new CRelayTableData;
	}

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

// 	if (m_spInputEquipManager == nullptr && m_spOutputEquipManager == nullptr
// 		&& m_spContentsEquipManager == nullptr && m_spPumpEquipManager == nullptr
// 		&& m_spPSEquipManager == nullptr && m_spEqNameManager == nullptr)
	{
		LoadEquipBaseData();
	}
	m_pFasSysData->InitFasSysData(m_spInputEquipManager, m_spEqNameManager
		, m_spOutputEquipManager , m_spContentsEquipManager
		, m_spPumpEquipManager, m_spPSEquipManager
		, m_spPmpNameEquipManager
	);
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

	PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_NEW, 0);
}

void CSysLinkerApp::OnHomeProjectOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
	{
		if (AfxMessageBox(L"프로젝트 열기를 하시려면 현재 프로젝트를 닫아야 합니다\n현재 프로젝트를 닫으시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
		CloseProject();
	}

	CDlgLogIn dlg(m_pMainDb);

	if (dlg.DoModal() != IDOK)
		return;
	//CloseProject();
	COleDateTime dtCur;
	DWORD_PTR dwStart, dwEnd;
	dwStart = GetTickCount();
	dtCur = COleDateTime::GetCurrentTime();
	GF_AddDebug(L"Start 시간 : %s", dtCur.Format(L"%H:%M:%S"));
	if (OpenProject(dlg.GetOpenProjectName(), dlg.GetOpenProjectPath()
		, dlg.GetOpenProjectVersion(), dlg.IsSelectedVersionTemp()) <= 0)
	{
		GF_AddLog(L"프로젝트를 여는데 실패 했습니다.");
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
	GF_AddLog(L"프로젝트를 여는데 성공 했습니다.");
	dwEnd = GetTickCount();
	GF_AddDebug(L"OnHomeProjectOpen : %d", dwEnd - dwStart);
	dtCur = COleDateTime::GetCurrentTime();
	GF_AddDebug(L"End 시간 : %s", dtCur.Format(L"%H:%M:%S"));
	CString strName;
	strName.Format(L"프로젝트명 - %s", m_pFasSysData->GetPrjName());
	AfxGetMainWnd()->SetWindowTextW(strName);

	if(g_bRequirePtnManualCheck)
	{
		AfxMessageBox(L"패턴을 수동으로 생성했는지 일람표로 생성했는지 체크되지 않았습니다.\n'수동으로 생성' 또는 '일람표로 생성'을 선택해 주시기 바랍니다.");
		CDlgPatternProperty dlg;
		dlg.m_pRefFasSysData = m_pFasSysData;
		dlg.DoModal();
	}
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
		AfxMessageBox(L"프로젝트가 열려 있지 않습니다.");
		GF_AddLog(L"프로젝트가 열려 있지 않습니다.");
		return;
	}
	// Database 용량 줄이기
	if (m_pFasSysData->ReduceDatabase() == 0)
	{
		AfxMessageBox(L"프로젝트를 저장하는데 실패 했습니다.");
		return; 
	}
	//////////////////////////////////////////////////////////////////////////
	// 1. 편집되고 있는 프로젝트(Version_temp) 정보 변경
	CString strPrjPath , strFullPath , strDBPath;
	m_pFasSysData->VersionUp();
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
	if (GF_IsExistFile(strDBPath) == FALSE)
	{
		CString strError;
		strError.Format(L"프로젝트를 저장하는데 실패 했습니다.(데이터베이스 Backup 실패)\r\n"
			L"'%s'에 같은 이름으로 파일 있습니다. 이 파일을 삭제하고 다시 저장해 주십시요"
			, strDBPath
		);
		AfxMessageBox(strError);
		return ;
	}
	if (m_pMainDb->BackupDatabase(m_pFasSysData->GetDBName(), strDBPath) == FALSE)
	{
//		AfxMessageBox(L"프로젝트를 저장하는데 실패 했습니다.\n데이터베이스 Backup 실패");
		GF_AddLog(L"프로젝트를 저장하는데 실패 했습니다.\n데이터베이스 Backup 실패");
// 		if (m_pMainDb->BackupDatabase(m_pFasSysData->GetDBName(), strDBPath) == FALSE)
// 		{
// 			GF_AddLog(L"프로젝트를 저장하는데 실패 했습니다.\n데이터베이스 Backup 실패");
// 		}
//		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 4. 
	m_pFasSysData->SetChangeFlag(FALSE);
	GF_AddLog(L"프로젝트를 저장하는데 성공했습니다");
	AfxMessageBox(L"프로젝트를 저장하는데 성공했습니다");

}

void CSysLinkerApp::OnHomeProjectTable()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_pFasSysData == nullptr)
	{
		GF_AddLog(L"프로젝트가 열려 있지 않습니다.");
		AfxMessageBox(L"프로젝트가 열려 있지 않습니다.");
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
	GF_AddLog(L"프로그램 환경설정을 저장했습니다.");
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
	OpenFormView(FV_LOGICEDIT);

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
	OpenFormView(FV_MAKEPATTERN);
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
		if (AfxMessageBox(L"프로젝트가 열려있습니다. 기존 프로젝트를 닫고 역변환을 시작하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
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
		GF_AddLog(L"ROM 파일로부터 프로젝트를 생성하는데 실패 했습니다.");
		AfxMessageBox(L"ROM 파일로부터 프로젝트를 생성하는데 실패 했습니다.");
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
	AfxMessageBox(L"ROM 파일로부터 프로젝트를 생성하는데 성공 했습니다.");
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
		//20240312 GBM start - 메뉴 비활성화
		pCmdUI->Enable(FALSE);
		//pCmdUI->Enable(m_bProgramLogin);
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
		GF_AddLog(L"데이터베이스 테이블을 여는데 실패했습니다. : TB_EQUIP_MST open failed");
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

	nRet = ::GetPrivateProfileString(L"PROJECT", L"PATH", L"C:\\FICON3\\PROJECT\\", buff, _MAX_PATH, (LPCTSTR)strPath);
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
	
	nRet = CreateProjectDatabase();
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
	CString strPrjSymbolFolder, strPrjResFolder, strPrjDBFolder, strPrjRelayTableFolder, strPrjMapFolder, strPrjEtcFolder;
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
		strError.Format(L"DBMS에 데이터 파일을 설정하는데 실패 했습니다.ErrorCode:%ld" , GetLastError());
		GF_AddLog(strError + L" -- " + strTarget + L" -- "  + strFile);
		AfxMessageBox(strError);
		AfxMessageBox(strTarget + L"---" + strFile);
		return 0;
	}

	return 1;
}

int CSysLinkerApp::CreateProjectDatabase()
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

	//20240222 GBM start - 중계기 일람표 파싱이 끝나고 기존 DB가 준비된 시점에 F4 추가 테이블에 Data Insert
	CNewDBManager::Instance()->SetDBAccessor(m_pFasSysData->m_pDB);
	//F4 추가 입력 타입은 gfs_base에 미리 넣어 프로젝트 DB를 복사하면 적용하고 F4 추가 테이블은 중계기 일람표 상에서 F4 추가 정보가 존재할 때만 F4 추가 테이블을 생성
	BOOL bRet = FALSE;

	// F4추가 테이블의 경우는 변경된 중계기 일람표가 F4 추가 정보가 있는 경우에만 생성, 당연히 해당 Sheet 중 하나가 있으면 다 있겠지만 혹시 완벽하지 않으면 DB에 넣지 않도록 함
	bRet = CNewExcelManager::Instance()->bExistFT && CNewExcelManager::Instance()->bExistUT && CNewExcelManager::Instance()->bExistPI;
	if (bRet)
	{
		bRet = CNewDBManager::Instance()->CheckAndCreateF4DBTables();
		if (bRet)
		{
			GF_AddLog(L"F4 정보 테이블 (프로젝트, 수신기 TYPE, UNIT TYPE) 생성이 성공했습니다.");
			Log::Trace("Inserting new DB table succeeded!");
		}
		else
		{
			GF_AddLog(L"F4 정보 테이블 (프로젝트, 수신기 TYPE, UNIT TYPE) 생성이 실패했습니다, DB를 확인하세요.");
			Log::Trace("Inserting new DB table failed!");
		}


		//20240222 GBM start - 중계기 일람표 파싱이 끝난 시점에 F4 추가 테이블에 Data Insert
		bRet = CNewDBManager::Instance()->InsertDatasIntoF4DBTables();
		if (bRet)
		{
			GF_AddLog(L"F4 정보 테이블 (프로젝트, 수신기 TYPE, UNIT TYPE) 데이터 추가에 성공했습니다.");
			Log::Trace("F4 DB table insertion succeeded!");
		}
		else
		{
			GF_AddLog(L"F4 정보 테이블 (프로젝트, 수신기 TYPE, UNIT TYPE) 데이터 추가에 실패했습니다, DB를 확인하세요.");
			Log::Trace("F4 DB table insertion failed!");
		}
		//20240222 GBM end
	}
	//20240222 GBM end

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
		GF_AddLog(L"프로젝트 파일을 생성하는데 실패 했습니다.\n");
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
//	file.WriteString(m_pFasSysData->GetPrjPath() + L"\n");
	file.WriteString(m_pFasSysData->GetDBName() + L"\n");
	file.WriteString(m_pFasSysData->GetDBUser() + L"\n");
	file.WriteString(m_pFasSysData->GetDBPwd() + L"\n");
	file.WriteString(m_pFasSysData->GetPrjCreateDate().Format(L"%Y-%m-%d %H:%M:%S") + L"\n");
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
		GF_AddLog(L"프로젝트 파일을 생성하는데 실패 했습니다.\n");
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
		GF_AddLog(L"프로젝트 파일을 생성하는데 실패 했습니다.\n");
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
		GF_AddLog(L"프로젝트 파일을 생성하는데 실패 했습니다.\n");
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
		strError.Format(L"프로그램 기초 데이터베이스(%s)를 등록하는데 실패 했습니다.\n%s"
			, g_stConfig.szDBName, pDB->GetLastErrorString());
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
				GF_AddLog(L"기초정보 데이터베이스를 서버에 등록하는데 실패했습니다.");
				AfxMessageBox(L"기초정보 데이터베이스를 서버에 등록하는데 실패했습니다.\r\nMSSQL Server를 설치하거나 서비스틀 실행해주시기 바랍니다.");
				return 0;
			}
			if (m_pMainDb->DBOpen() == FALSE)
			{
				GF_AddLog(L"기초정보 데이터베이스 접속에 실패했습니다.");
				AfxMessageBox(L"데이터베이스 접속에 실패했습니다.");
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
			GF_AddLog(L"기초정보 데이터베이스를 여는데 실패 했습니다.");
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
			strError.Format(L"프로젝트 데이터베이스(%s)를 분리하는데 실패 했습니다.\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
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
			strError.Format(L"프로젝트 데이터베이스(%s)를 등록하는데 실패 했습니다.\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
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
			strError.Format(L"프로젝트 데이터베이스(%s)를 복원하는데 실패 했습니다.\n%s"
				, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
			GF_AddLog(strError);
			AfxMessageBox(strError);
			return -3;
		}
	}
	
	if (pFasSysData->OpenPrjDatabase(g_stConfig.szSource, m_pFasSysData->GetDBName()
		, 1433, g_stConfig.szDBUser, g_stConfig.szDBPass) <= 0)
	{
		CString strError;
		strError.Format(L"프로젝트 데이터베이스(%s)를 연결하는데 실패 했습니다.\n%s"
			, pFasSysData->GetDBName(), m_pMainDb->GetLastErrorString());
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
			GF_AddLog(L"프로젝트 정보를 가져오는데 실패 했습니다.");
			AfxMessageBox(L"프로젝트 정보를 가져오는데 실패 했습니다.");
			return -1;
		}
	}

	if (m_pFasSysData == nullptr)
	{
		m_pFasSysData = new CRelayTableData;
	}

	OpenProjectInfoFile(m_pFasSysData , strPrjFullPath, strPrjName);
	OpenVersionInfoFile(m_pFasSysData, HIWORD(dwVer), LOWORD(dwVer), strPrjFullPath, strPrjName,bVerTemp);
	if (OpenProjectDatabase(m_pFasSysData) < 0)
	{
		GF_AddLog(L"프로젝트 데이터베이스를 여는데 실패 했습니다.");
		return 0;
	}

	//20240311 GBM start - DB Open 후 DB에서 데이터를 가져오기 전에 F4 추가 입력 타입을 추가
	CNewDBManager::Instance()->SetDBAccessor(theApp.m_pFasSysData->m_pDB);

	BOOL bRet = FALSE;

	bRet = CNewDBManager::Instance()->CheckAndInsertEquipmentNewInputType();
	if (bRet)
	{
		GF_AddLog(L"F4 입력 타입 자동 추가에 성공했습니다.");
		Log::Trace("Inserting a new input type of equipment succeeded!");
	}
	else
	{
		GF_AddLog(L"F4 입력 타입 자동 추가에 실패했습니다. 사용 중인 입력 타입을 확인하세요.");
		Log::Trace("Inserting a new input type of equipment failed!");
	}
	//20240311 GBM end

	if (m_pFasSysData->LoadProjectDatabase() == 0)
	{
		GF_AddLog(L"프로젝트를 불러오는데 실패 했습니다.");
		return 0;
	}
	m_pFasSysData->SetProjectOpened(TRUE);
	GF_AddLog(L"프로젝트를 불러오는데 성공 했습니다.");

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
		if (AfxMessageBox(L"변경사항이 있습니다.\n프로젝트를 저장하시겠습니까?"
			, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnHomeProjectSave();
		}
	}

	m_pMainDb->DetachMSDB(m_pFasSysData->GetDBName());
	delete m_pFasSysData;
	m_pFasSysData = nullptr;

	//RemoveTemplate();

	//20240307 GBM start - F4 추가 정보 메모리 초기화
	memset(&CNewInfo::Instance()->m_fi, NULL, sizeof(F4APPENDIX_INFO));
	//20240307 GBM end

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
			GF_AddLog(L"버전 폴더를 생성하는데 실패 했습니다.");
			AfxMessageBox(L"버전 폴더를 생성하는데 실패 했습니다.");
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
		GF_AddLog(L"버전정보 파일을 이동하는데 실패 했습니다.");
		AfxMessageBox(L"버전정보 파일을 이동하는데 실패 했습니다.");
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
			GF_AddLog(L"데이터베이스 폴더를 생성하는데 실패 했습니다.");
			AfxMessageBox(L"데이터베이스 폴더를 생성하는데 실패 했습니다.");
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
			GF_AddLog(L"RELEASE 폴더를 생성하는데 실패 했습니다.");
			AfxMessageBox(L"RELEASE 폴더를 생성하는데 실패 했습니다.");
			return 0;
		}
	}
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
