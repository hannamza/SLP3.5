// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// SysLinker.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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
	//20240516 GBM start - �⺻ DB�� ������Ʈ ���� �� ���� ���ǰ� �߰�� �϶�ǥ �� ���� ���Ƿ� overwrite�ǹǷ� �⺻ ���� ���� ���� ������ �ξ �� ������Ʈ ���� �ø��� �ʱ�ȭ
	theApp.m_bThreadSucceeded = TRUE;
	CString strEquipmentDefinitionFile = _T("");
	CString strFolder = _T("");
	CString strFile = _T("");
	std::vector<CString> fileVec;
	BOOL bRet = FALSE;

	strFolder.Format(_T("C:\\Ficon3\\%s"), F3_PRJ_DIR_DATABASE);
	strFile.Format(_T("%s"), EQUIPMENT_INFO_EXCEL_FILE_NAME);
	fileVec = CCommonFunc::GetFullPathFileListIntheFolder(strFolder, strFile);	// ��θ� ��� �ż��带 ������ �����ϴ� �� ���η� Ȱ��
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
			strMsg.Format(_T("�⺻ ���� ���� ���� [%s] ���⿡ �����߽��ϴ�."), strEquipmentDefinitionFile);
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
		strMsg.Format(_T("�⺻ ���� ���� ���� [%s]�� ��� �������� �ʽ��ϴ�."), strEquipmentDefinitionFile);
#else
		strMsg.Format(_T("Not applied because the default equipment definition file [%s] doesn't exist."), strEquipmentDefinitionFile);
#endif
		GF_AddLog(strMsg);
		strMsg.Format(_T("The default equipmemt definition file [%s] does not exist and will not be applied."), strEquipmentDefinitionFile);
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));	
	}
	
	theApp.m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(theApp.m_hThreadHandle);
	//20240516 GBM end

	return 0;
}

UINT ThreadCreateNewProject(LPVOID pParam)
{
	theApp.m_bThreadSucceeded = TRUE;
	if (theApp.m_pFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ ������ �����߽��ϴ�.");
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
	1. Project Path ���� :
	- ���� ����
	- File ����
	2. Database ����
	- Database ����
	- Table �� ������ ����
	3. View ����
	- DialogBar : Input , output ,

	*/
	/************************************************************************/
	if (theApp.CreateProject() <= 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ ������ �����߽��ϴ�.");
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
		GF_AddLog(L"������Ʈ �����ͺ��̽��� ���µ� �����߽��ϴ�.");
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
		GF_AddLog(L"������Ʈ �����ͺ��̽����� �����͸� �������� ���� �����߽��ϴ�.");
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

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
ST_VIEWFORM_SELECTITEM g_stViewSelectItem;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

														// �����Դϴ�.
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
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
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


// CSysLinkerApp ����

CSysLinkerApp::CSysLinkerApp()
{
	
	m_bHiColorIcons = TRUE;

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
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
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.

	m_strProgramLoginUser = L"";
	m_bProgramLogin = FALSE;

	m_pDlgProg = nullptr;

	m_bModuleTableChanged = FALSE;
}

// ������ CSysLinkerApp ��ü�Դϴ�.

CSysLinkerApp theApp;


// CSysLinkerApp �ʱ�ȭ

BOOL CSysLinkerApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("GFS\\FICON3\\SLP3"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.

	SetSystemPath();

	ConfigLoad();

// 	if (OpenBaseDatabase() == 0)
// 	{
// 		AfxMessageBox(L"�����ͺ��̽��� ���� �� �� �����ϴ�. ȯ�漳���� �ٽ� ������ �ֽñ� �ٶ��ϴ�.");
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

	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	//CMultiDocTemplate* pDocTemplate;
	DocumentTemplateInit();

	// �� MDI ������ â�� ����ϴ�.
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


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	m_pTempleMakeLink->OpenDocumentFile(nullptr);

	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
// 	if (!ProcessShellCommand(cmdInfo))
// 		return FALSE;
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	//20240202 GBM start - �� ��� Ŭ���� �ʱ�ȭ
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
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
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

	//20240202 GBM start - �� ��� Ŭ���� ����
	CNewInfo::Delete();
	CNewDBManager::Delete();
	CNewExcelManager::Delete();
	Log::Cleanup();

	//_CrtDumpMemoryLeaks();
	//20240202 GBM end

	return CWinAppEx::ExitInstance();
}

// CSysLinkerApp �޽��� ó����


// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CSysLinkerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSysLinkerApp ����� ���� �ε�/���� �޼���

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

// CSysLinkerApp �޽��� ó����






//void CAboutDlg::OnSortAddress()
//{
//	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CSysLinkerView));
#else
	m_pTempleMakeLink = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CSysLinkerDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CSysLinkerView));
#endif
	if (!m_pTempleMakeLink)
		return FALSE;
	AddDocTemplate(m_pTempleMakeLink);

#ifndef ENGLISH_MODE
	m_pTempleMakePtn = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormPattern));
#else
	m_pTempleMakePtn = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormPattern));
#endif
	if (!m_pTempleMakePtn)
		return FALSE;
	AddDocTemplate(m_pTempleMakePtn);

#ifndef ENGLISH_MODE
	m_pTemplePSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormPSwitch));
#else
	m_pTemplePSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormPSwitch));
#endif
	if (!m_pTemplePSwitch)
		return FALSE;
	AddDocTemplate(m_pTemplePSwitch);

#ifndef ENGLISH_MODE
	m_pTempleSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormSwitch));
#else
	m_pTempleSwitch = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormSwitch));
#endif
	if (!m_pTempleSwitch)
		return FALSE;
	AddDocTemplate(m_pTempleSwitch);

#ifndef ENGLISH_MODE
	m_pTempleLinkView = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormLinkView));
#else
	m_pTempleLinkView = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormLinkView));
#endif
	if (!m_pTempleLinkView)
		return FALSE;
	AddDocTemplate(m_pTempleLinkView);

#ifndef ENGLISH_MODE
	m_pTemplePump = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormPump));
#else
	m_pTemplePump = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormPump));
#endif
	if (!m_pTemplePump)
		return FALSE;
	AddDocTemplate(m_pTemplePump);

#ifndef ENGLISH_MODE
	m_pTempleEquip = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormEquip));
#else
	m_pTempleEquip = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormEquip));
#endif
	if (!m_pTempleEquip)
		return FALSE;
	AddDocTemplate(m_pTempleEquip);

#ifndef ENGLISH_MODE
	m_pTempleUser = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormUser));
#else
	m_pTempleUser = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormUser));
#endif
	if (!m_pTempleUser)
		return FALSE;
	AddDocTemplate(m_pTempleUser);

#ifndef ENGLISH_MODE
	m_pTempleAccess = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormAccess));
#else
	m_pTempleAccess = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormAccess));
#endif
	if (!m_pTempleAccess)
		return FALSE;
	AddDocTemplate(m_pTempleAccess);

#ifndef ENGLISH_MODE
	m_pTempleEmergency = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormEmergency));
#else
	m_pTempleEmergency = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormEmergency));
#endif
	if (!m_pTempleEmergency)
		return FALSE;
	AddDocTemplate(m_pTempleEmergency);

#ifndef ENGLISH_MODE
	m_pTempleLocation = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormLocation));
#else
	m_pTempleLocation = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormLocation));
#endif
	if (!m_pTempleLocation)
		return FALSE;
	AddDocTemplate(m_pTempleLocation);

#ifndef ENGLISH_MODE
	m_pTempleFacp = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormFacp));
#else
	m_pTempleFacp = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormFacp));
#endif
	if (!m_pTempleFacp)
		return FALSE;
	AddDocTemplate(m_pTempleFacp);

#ifndef ENGLISH_MODE
	m_pTempleUnit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormUnit));
#else
	m_pTempleUnit = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormUnit));
#endif
	if (!m_pTempleUnit)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleRelayEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormRelayEdit));
#else
	m_pTempleRelayEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormRelayEdit));
#endif
	if (!m_pTempleRelayEdit)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleAutoMake = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormAutoMake));
#else
	m_pTempleAutoMake = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormAutoMake));
#endif
	if (!m_pTempleAutoMake)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleLogicEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormEditLogic));
#else
	m_pTempleLogicEdit = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormEditLogic));
#endif
	if (!m_pTempleLogicEdit)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleLoadRealy = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormLoadRelayTable));
#else
	m_pTempleLoadRealy = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormLoadRelayTable));
#endif
	if (!m_pTempleLoadRealy)
		return FALSE;

#ifndef ENGLISH_MODE
	m_pTempleErrorCheck = new CMultiDocTemplate(IDR_SysLinkerTYPE,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormErrorCheck));
#else
	m_pTempleErrorCheck = new CMultiDocTemplate(IDR_SysLinkerTYPE_EN,
		RUNTIME_CLASS(CFormDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CFormErrorCheck));
#endif
	if(!m_pTempleErrorCheck)
		return FALSE;

	AddDocTemplate(m_pTempleUnit);

	//20240318 GBM start - �޸� ���� ���� ���� : ���� �ڵ� �ݿ�
	AddDocTemplate(m_pTempleRelayEdit);
	AddDocTemplate(m_pTempleAutoMake);
	AddDocTemplate(m_pTempleLogicEdit);
	AddDocTemplate(m_pTempleLoadRealy);
	AddDocTemplate(m_pTempleErrorCheck);
	//20240318 GBM end

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
		strTitle = L"���������� ����";
#else
		strTitle = L"EDIT SITE LOGIC DATA";
#endif
	}
	else if (iStyle == FV_MAKEPATTERN)
	{
		pDocTemp = m_pTempleMakePtn;
#ifndef ENGLISH_MODE
		strTitle = L"���ϵ����� ����";
#else
		strTitle = L"EDIT PATTERN DATA";
#endif
	}
	else if (iStyle == FV_PSWITCH)
	{
		pDocTemp = m_pTemplePSwitch;
#ifndef ENGLISH_MODE
		strTitle = L"�з½���ġ ����";
#else
		strTitle = L"EDIT PRESSURE SWITCH";
#endif
	}
	else if (iStyle == FV_SWITCH)
	{
		pDocTemp = m_pTempleSwitch;
#ifndef ENGLISH_MODE
		strTitle = L"����ġ ����";
#else
		strTitle = L"EDIT SWITCH";
#endif
	}
	else if (iStyle == FV_LINKVIEW)
	{
		pDocTemp = m_pTempleLinkView;
#ifndef ENGLISH_MODE
		strTitle = L"���������� ����";
#else
		strTitle = L"VIEW SITE LOGIC DATA";
#endif
	}
	else if (iStyle == FV_PUMP)
	{
		pDocTemp = m_pTemplePump;
#ifndef ENGLISH_MODE
		strTitle = L"���������� ����";
#else
		strTitle = L"EDIT PUMP DATA";
#endif
	}
	else if (iStyle == FV_EQUIP)
	{
		pDocTemp = m_pTempleEquip;
#ifndef ENGLISH_MODE
		strTitle = L"�������� ����";
#else
		strTitle = L"EDIT EQUIPMENT INFORMATION";
#endif
	}
	else if (iStyle == FV_USER)
	{
		pDocTemp = m_pTempleUser;
#ifndef ENGLISH_MODE
		strTitle = L"�����/����ڱ׷� ����";
#else
		strTitle = L"EDIT USER/USER GROUP";
#endif
	}
	else if (iStyle == FV_ACCESS)
	{
		pDocTemp = m_pTempleAccess;
#ifndef ENGLISH_MODE
		strTitle = L"Access ���� ����";
#else
		strTitle = L"EDIT ACCESS PERMISSION";
#endif
	}
	else if (iStyle == FV_EMERGENCY)
	{
		pDocTemp = m_pTempleEmergency;
#ifndef ENGLISH_MODE
		strTitle = L"����� ����";
#else
		strTitle = L"EDIT PUBLIC ADDRESS";
#endif
	}
	else if (iStyle == FV_LOCATION)
	{
		pDocTemp = m_pTempleLocation;
#ifndef ENGLISH_MODE
		strTitle = L"��ġ���� ����";
#else
		strTitle = L"EDIT GEOLOCATION INFORMATION";
#endif
	}
	else if (iStyle == FV_FACP)
	{
		pDocTemp = m_pTempleFacp;
#ifndef ENGLISH_MODE
		strTitle = L"���ű����� ����";
#else
		strTitle = L"EDIT FACP INFORMATION";
#endif
	}
	else if (iStyle == FV_UNIT)
	{
		pDocTemp = m_pTempleUnit;
#ifndef ENGLISH_MODE
		strTitle = L"�������� ����";
#else
		strTitle = L"EDIT UNIT INFORMATION";
#endif
	}
	else if (iStyle == FV_RELAYEDIT)
	{
		pDocTemp = m_pTempleRelayEdit;
#ifndef ENGLISH_MODE
		strTitle = L"ȸ�� ����";
#else
		strTitle = L"EDIT CIRCUIT";
#endif
	}
	else if (iStyle == FV_AUTOMAKE)
	{
		pDocTemp = m_pTempleAutoMake;
#ifndef ENGLISH_MODE
		strTitle = L"���������� �ڵ�����";
#else
		strTitle = L"AUTOGENERATE SITE LOGIC DATA";
#endif
	}
	else if (iStyle == FV_LOGICEDIT)
	{
		pDocTemp = m_pTempleLogicEdit;
#ifndef ENGLISH_MODE
		strTitle = L"���������� �ڵ����� ����";
#else
		strTitle = L"SITE LOGIC DATA AUTOGENERATION LOGIC";
#endif
	}
	else if (iStyle == FV_LOADRELAYTABLE)
	{
		pDocTemp = m_pTempleLoadRealy;
#ifndef ENGLISH_MODE
		strTitle = L"�߰�� �϶�ǥ";
#else
		strTitle = L"MODULE TABLE";
#endif
	}
	else if(iStyle == FV_ERRORCHECK)
	{
		pDocTemp = m_pTempleErrorCheck;
#ifndef ENGLISH_MODE
		strTitle = L"���� �˻�";
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

	//20240306 GBM start - ���ǹ��� �α��� ����
#if 0
	CDlgProgramLogin dlg(m_pMainDb);
	if (dlg.DoModal() != IDOK)
		return; 
	m_strProgramLoginUser = dlg.m_strUser;
	GF_AddLog(L"���α׷� �α��ο� �����߽��ϴ�.");
	AfxMessageBox(L"���α׷� �α��ο� �����߽��ϴ�.");
#endif
	//20240306 GBM end

	m_bProgramLogin = TRUE;
	//OpenBaseDatabase();
	//m_pMainDb->RestoreDatabase(L"����", L"D:\\04. Ficon3\\51. Project\\����", L"����.bak");
// 	CDlgLogIn dlg(m_pMainDb);
// 
// 	if (dlg.DoModal() != IDOK)
// 		return;


// 	// ��������Ʈ
// 	if (dlg.GetSelectProjectType() == 0)
// 	{
// 		OnHomeProjectNew();
// 		return;
// 	}
// 	

	// ���� ������Ʈ 


	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	// ���2 : DSN �̿����, ��� Server��, Port���� ������ �־�� �Ѵ�.
	//MySQL ODBC Driver�� ����ص� �۵���    conn.ConnectionString = "Driver={MySQL ODBC 5.3 Unicode Driver};Server=192.168.65.128;Port=3306;Database=test;User=root;Password=x1234;Option=2;"
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
	// 		//AfxMessageBox(L"����");
	// 	}
	// 	else
	// 	{
	// 		AfxMessageBox(L"����1");
	// 	}
	// 	db.DBClose();
	// 
	// 	db.ODBCInitialize(L"FICON3", L"gfs_base", L"root", L"rnd@GFS111");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"�ѱ۵����ͺ��̽�", L"127.0.0.1");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	if (db.DBOpen() == FALSE)
	// 	{
	// 		//AfxMessageBox(L"����");
	// 	}
	// 	else
	// 	{
	// 		AfxMessageBox(L"����2");
	// 	}
	// 	db.DBClose();


	// 
	// 	YAdoDatabase db;
	// 	db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	if (db.DBOpen() == FALSE)
	// 	{
	// 	 	//AfxMessageBox(L"����");
	// 	}
	// 	else
	// 	{
	// 	 	//AfxMessageBox(L"����1");
	// 	}
	// 	CString strSql;
	// 	strSql.Format(L"CREATE DATABASE %s", L"���⵿GFS");
	// 	if (db.ExecuteSql(strSql) == FALSE)
	// 	{
	// 		AfxMessageBox(L"�����ͺ��̽��� �����ϴµ� ���� �߽��ϴ�.");
	// 	}
	// 
	// 	db.DBClose();
	// 
	// 	db.MySqlInitialize(L"rnd@GFS111", L"root", L"���⵿GFS", L"127.0.0.1");
	// 	//db.MySqlInitialize(L"rnd@GFS111", L"root", L"gfs_base", L"127.0.0.1");
	// 	if (db.DBOpen() == FALSE)
	// 	{
	// 		//AfxMessageBox(L"����");
	// 	}
	// 	else
	// 	{
	// 		//AfxMessageBox(L"����1");
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
	// 1. ������Ʈ ����
	// 2. ȭ�� �ʱ�ȭ
	// 3. DB�и� , ������ �Ҵ� ����
	// 4. ������ ����
#ifndef ENGLISH_MODE
	if (AfxMessageBox(L"���� ������Ʈ�� �����ðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
#else
	if (AfxMessageBox(L"Do you want to close the current project?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
#endif

	//20240527 GBM start - ������� ��ȯ
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("������Ʈ�� �ݴ� ���Դϴ�. ��� ��ٷ� �ּ���.");
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
		Log::Trace("������ ��� ����! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ�� �ݴ� ���� �����߽��ϴ�.");
#else
		GF_AddLog(L"Successfully closed the project.");
#endif
		Log::Trace("The project was successfully closed.");
	}
	else
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ�� �ݴ� ���� �����߽��ϴ�.");
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

	GF_AddLog(L"������Ʈ�� �ݾҽ��ϴ�.");
	Log::Trace("Project Closed!");
#endif
	//20240527 GBM end
}

void CSysLinkerApp::OnHomeProjectNew()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	//CDlgNewProjectWizard dlg;

	if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
	{
#ifndef ENGLISH_MODE
		if (AfxMessageBox(L"�� ������Ʈ�� �����Ͻ÷��� ���� ������Ʈ�� �ݾƾ� �մϴ�.\n���� ������Ʈ�� �����ðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#else
		if (AfxMessageBox(L"To create a new project, you need to close the current project.\nDo you want to close the current project? ", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#endif

		//20240527 GBM start - ������� ��ȯ
#if 1
		m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
		CString strMsg = _T("������Ʈ�� �ݴ� ���Դϴ�. ��� ��ٷ� �ּ���.");
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
			Log::Trace("������ ��� ����! dw : %d", dw);
		}

		if (m_bThreadSucceeded)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"������Ʈ�� �ݴ� ���� �����߽��ϴ�.");
#else
			GF_AddLog(L"Successfully closed the project.");
#endif
			Log::Trace("The project was successfully closed.");
		}
		else
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"������Ʈ�� �ݴ� ���� �����߽��ϴ�.");
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
 		GF_AddLog(L"������Ʈ�� �ݾҽ��ϴ�.");
 		Log::Trace("Project Closed!");
#endif
		//20240527 GBM end
	}

	//20240527 GBM start - ������� ��ȯ
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("�⺻ ���� ���Ǹ� �ε� ���Դϴ�. ��� ��ٷ� �ּ���.");
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
		Log::Trace("������ ��� ����! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"�⺻ ���� ���� �ε忡 �����߽��ϴ�.");
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

	//20240408 GBM start - ���⼭�� �Ʒ��� �������� �ʰ� �߰�� �϶�ǥ ��ΰ� Ȯ���Ǿ� ������ ���� ���� ���Ǹ� �⺻ DB�� ���� �� �ű⼭ �Ʒ� ������ ������ �����ϵ��� ��
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

	//20240523 GBM start - �Ʒ� ������ ������� ��ü
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	strMsg = _T("        �� ������Ʈ�� ���� ���Դϴ�. ��� ��ٷ� �ּ���.");
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
		Log::Trace("������ ��� ����! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"�� ������Ʈ ������ �����߽��ϴ�.");
#else
		GF_AddLog(L"Successfully created a new project.");
#endif
		Log::Trace("New project creation was successful.");
	}
	else
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"�� ������Ʈ ������ �����߽��ϴ�.");
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
#ifndef ENGLISH_MODE
	strName.Format(L"������Ʈ�� - %s", m_pFasSysData->GetPrjName());
#else
	strName.Format(L"Project Name - %s", m_pFasSysData->GetPrjName());
#endif
	AfxGetMainWnd()->SetWindowTextW(strName);

#ifndef ENGLISH_MODE
	GF_AddLog(L"[%s] ������Ʈ�� �����߽��ϴ�.", m_pFasSysData->GetPrjName());
#else
	GF_AddLog(L"You have created the project [%s].", m_pFasSysData->GetPrjName());
#endif
	Log::Trace("[%s] Project Created!", CCommonFunc::WCharToChar(m_pFasSysData->GetPrjName().GetBuffer(0)));

	PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_NEW, 0);

#else
	if (m_pFasSysData == nullptr)
	{
		AfxMessageBox(L"������Ʈ ������ ���� �߽��ϴ�.");
		CloseProject();
		return;
	}

	/************************************************************************/
	/*
	1. Project Path ���� :
	- ���� ����
	- File ����
	2. Database ����
	- Database ����
	- Table �� ������ ����
	3. View ����
	- DialogBar : Input , output ,

	*/
	/************************************************************************/
	if(CreateProject() <= 0)
	{
		AfxMessageBox(L"������Ʈ�� �����ϴµ� ���� �߽��ϴ�.");
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
	strName.Format(L"������Ʈ�� - %s", m_pFasSysData->GetPrjName());
	AfxGetMainWnd()->SetWindowTextW(strName);

	GF_AddLog(L"[%s] ������Ʈ�� �����߽��ϴ�.", m_pFasSysData->GetPrjName());
	Log::Trace("[%s] Project Created!", CCommonFunc::WCharToChar(m_pFasSysData->GetPrjName().GetBuffer(0)));

	PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_NEW, 0);

#endif
	//20240523 GBM end
}

void CSysLinkerApp::OnHomeProjectOpen()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	if (m_pFasSysData != nullptr && m_pFasSysData->GetProjectOpened())
	{
#ifndef ENGLISH_MODE
		if (AfxMessageBox(L"������Ʈ ���⸦ �Ͻ÷��� ���� ������Ʈ�� �ݾƾ� �մϴ�.\n���� ������Ʈ�� �����ðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#else
		if (AfxMessageBox(L"To open a project, you need to close the current project.\nDo you want to close the current project ? ", MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
#endif

		//20240527 GBM start - ������� ��ȯ
#if 1
		m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
		CString strMsg = _T("������Ʈ�� �ݴ� ���Դϴ�. ��� ��ٷ� �ּ���.");
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
			Log::Trace("������ ��� ����! dw : %d", dw);
		}

		if (m_bThreadSucceeded)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"������Ʈ�� �ݴ� ���� �����߽��ϴ�.");
#else
			GF_AddLog(L"Successfully closed the project.");
#endif
			Log::Trace("The project was successfully closed.");
		}
		else
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"������Ʈ�� �ݴ� ���� �����߽��ϴ�.");
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
		GF_AddLog(L"������Ʈ�� �ݾҽ��ϴ�.");
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
	GF_AddDebug(L"Start �ð� : %s", dtCur.Format(L"%H:%M:%S"));
#else
	GF_AddDebug(L"Start Time : %s", dtCur.Format(L"%H:%M:%S"));
#endif
	//20240730 GBM start - ������ DB�� �ƴ� ������Ʈ ���� ���Ϸ� ������Ʈ ������ ����
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
		GF_AddLog(L"[%s] ������Ʈ�� ���µ� ���� �߽��ϴ�.", dlg.GetOpenProjectName());
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
#ifndef ENGLISH_MODE
	GF_AddLog(L"[%s] ������Ʈ�� ���µ� ���� �߽��ϴ�.", dlg.GetOpenProjectName());
#else
	GF_AddLog(L"Successfully opened the project [%s].", dlg.GetOpenProjectName());
#endif
	Log::Trace("[%s] Project Opened!", CCommonFunc::WCharToChar(dlg.GetOpenProjectName().GetBuffer(0)));
	dwEnd = GetTickCount();
	GF_AddDebug(L"OnHomeProjectOpen : %d", dwEnd - dwStart);
	dtCur = COleDateTime::GetCurrentTime();
#ifndef ENGLISH_MODE
	GF_AddDebug(L"End �ð� : %s", dtCur.Format(L"%H:%M:%S"));
#else
	GF_AddDebug(L"End Time : %s", dtCur.Format(L"%H:%M:%S"));
#endif
	CString strName;
#ifndef ENGLISH_MODE
	strName.Format(L"������Ʈ�� - %s", m_pFasSysData->GetPrjName());
#else
	strName.Format(L"Project Name - %s", m_pFasSysData->GetPrjName());
#endif
	AfxGetMainWnd()->SetWindowTextW(strName);

	if(g_bRequirePtnManualCheck)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"������ �������� �����ߴ��� �϶�ǥ�� �����ߴ��� üũ���� �ʾҽ��ϴ�.\n'�������� ����' �Ǵ� '�϶�ǥ�� ����'�� ������ �ֽñ� �ٶ��ϴ�.");
#else
		AfxMessageBox(L"You have not checked whether the pattern was created manually or from a table.\nPlease select 'Create Manually' or 'Create from Table'.");
#endif
		CDlgPatternProperty dlg;
		dlg.m_pRefFasSysData = m_pFasSysData;
		dlg.DoModal();
	}
}

void CSysLinkerApp::OnHomeProjectSave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	// 1. Version ���� : VER.PVI���Ͽ� ���� ���� , ������ , �̸� ����
	// 2. Folder ����
	// 3. DB Backup
	// 4.
	if (m_pFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"������Ʈ�� �������� �ʽ��ϴ�.");
		GF_AddLog(L"������Ʈ�� �������� �ʽ��ϴ�.");
#else
		AfxMessageBox(L"The project is not open.");
		GF_AddLog(L"The project is not open.");
#endif
		return;
	}
	// Database �뷮 ���̱�
	if (m_pFasSysData->ReduceDatabase() == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"������Ʈ�� �����ϴµ� �����߽��ϴ�.");
#else
		AfxMessageBox(L"Failed to save the project.");
#endif
		return; 
	}
	//////////////////////////////////////////////////////////////////////////
	// 1. �����ǰ� �ִ� ������Ʈ(Version_temp) ���� ����
	CString strPrjPath , strFullPath , strDBPath;

	if (!m_bModuleTableChanged)
		m_pFasSysData->VersionUp();
	else
		m_pFasSysData->VersionUp(TRUE);

	//20240730 GBM start - ������Ʈ ���� �� Minor ��ȣ ���� ����
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
	// 2. Folder ����
	CopyVersionTempToVersion(m_pFasSysData->GetPrjName(), strFullPath, m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum());

	//////////////////////////////////////////////////////////////////////////
	// 3. DB Backup --> ������ƮPath\\������Ʈ��\\��������\\DB\\������Ʈ��.bak
	strDBPath.Format(L"%s\\%s\\%s\\%s.BAK" 
		, strFullPath
		,GF_GetVersionFolderName(m_pFasSysData->GetPrjMajorNum(), m_pFasSysData->GetPrjMinorNum())
		, F3_PRJ_DIR_DATABASE
		, m_pFasSysData->GetDBName()
	);
	if (GF_IsExistFile(strDBPath) == FALSE)
	{
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"������Ʈ�� �����ϴµ� �����߽��ϴ�.(�����ͺ��̽� Backup ����)\r\n"
			L"'%s'�� ���� �̸����� ���� �ֽ��ϴ�. �� ������ �����ϰ� �ٽ� ������ �ֽʽÿ�."
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
//		AfxMessageBox(L"������Ʈ�� �����ϴµ� ���� �߽��ϴ�.\n�����ͺ��̽� Backup ����");
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ�� �����ϴµ� �����߽��ϴ�.\n�����ͺ��̽� Backup ����");
#else
		GF_AddLog(L"Failed to save the project.\nDatabase Backup Failure");
#endif
// 		if (m_pMainDb->BackupDatabase(m_pFasSysData->GetDBName(), strDBPath) == FALSE)
// 		{
// 			GF_AddLog(L"������Ʈ�� �����ϴµ� ���� �߽��ϴ�.\n�����ͺ��̽� Backup ����");
// 		}
//		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 4. 
	m_pFasSysData->SetChangeFlag(FALSE);
#ifndef ENGLISH_MODE
	GF_AddLog(L"������Ʈ�� �����ϴµ� �����߽��ϴ�.");
#else
	GF_AddLog(L"Successfully saved the project.");
#endif
	//AfxMessageBox(L"������Ʈ�� �����ϴµ� �����߽��ϴ�");

}

void CSysLinkerApp::OnHomeProjectTable()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	if (m_pFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ�� �������� �ʽ��ϴ�.");
		AfxMessageBox(L"������Ʈ�� �������� �ʽ��ϴ�.");
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CDlgEnv dlg;
	if (dlg.DoModal() != IDOK)
		return;
	ConfigSave();
#ifndef ENGLISH_MODE
	GF_AddLog(L"���α׷� ȯ�漳���� �����߽��ϴ�.");
#else
	GF_AddLog(L"You've saved your program environment setup.");
#endif
}


void CSysLinkerApp::OnBasicSetAccesslevel()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_ACCESS);
}


void CSysLinkerApp::OnBasicSetDraw()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CSysLinkerApp::OnBasicSetEquip()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	//OpenFormView(FV_EQUIP);
}


void CSysLinkerApp::OnBasicSetFacp()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CSysLinkerApp::OnBasicSetInoutType()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_EQUIP);

}


void CSysLinkerApp::OnBasicSetPump()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CSysLinkerApp::OnBasicSetUnit()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CSysLinkerApp::OnBasicSetUser()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CSysLinkerApp::OnBasicSetUsergroup()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_USER);

}

void CSysLinkerApp::OnBasicSetLogicEdit()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_LOGICEDIT);

}
// 
// void CSysLinkerApp::OnFacpCreateLink()
// {
// 	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
// 	if(AfxMessageBox(L"���������͸� ���� ���·� ������ �մϴ�.",MB_YESNO | MB_ICONQUESTION) != IDYES)
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
// 		GF_AddLog(L"������Ʈ�� ���� ���� �Դϴ�. ������Ʈ�� ���ð� �ٽ� �õ��Ͽ��ֽʽÿ�");
// 		AfxMessageBox(L"������Ʈ�� ���� ���� �Դϴ�. ������Ʈ�� ���ð� �ٽ� �õ��Ͽ��ֽʽÿ�");
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
// 		GF_AddLog(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�.(���������� ���� ����)");
// 	//	AfxMessageBox(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�.");
// 	}
// 	else
// 	{
// 		if(strPath.Right(1) != '\\')
// 			strPath += L"\\";
// 		strPath += L"*.*";
// 		//GF_DeleteDir(strPath);
// 		GF_AddLog(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�.(���������� ���� ����)");
// 		AfxMessageBox(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�." , MB_OK|MB_ICONSTOP);
// 		return;
// 		//AfxMessageBox(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�.");
// 	}
// 	nRet = pRefTable->MakeConstructorTable(strPath);
// 	if (nRet > 0)
// 	{
// 		GF_AddLog(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�.(���� ���ǥ���� ����)");
// 		AfxMessageBox(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�.",MB_OK|MB_ICONINFORMATION);
// 	}
// 	else
// 	{
// 		if (strPath.Right(1) != '\\')
// 			strPath += L"\\";
// 		strPath += L"*.*";
// 		//GF_DeleteDir(strPath);
// 		GF_AddLog(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�.(���� ���ǥ���� ����)");
// 		AfxMessageBox(L"������Ʈ�� ������ �ϴµ� �����߽��ϴ�.",MB_OK | MB_ICONSTOP);
// 	}
// }


void CSysLinkerApp::OnFacpEditLink()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_AUTOMAKE);
// 	CDlgMakeAutoAllInput dlg;
// 	dlg.m_pRefFasSysData = m_pFasSysData;
// 	if (dlg.DoModal() != IDOK)
// 		return;
}


void CSysLinkerApp::OnFacpEmmergency()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_EMERGENCY);
}


void CSysLinkerApp::OnFacpLinkPs()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_PSWITCH);
}


void CSysLinkerApp::OnFacpLinkPump()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_PUMP);
}


void CSysLinkerApp::OnFacpLocation()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_LOCATION);
}


void CSysLinkerApp::OnFacpPattern()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_MAKEPATTERN);
}


void CSysLinkerApp::OnFacpPreviewLink()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_LINKVIEW);
}


void CSysLinkerApp::OnFacpReverseLink()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	if (m_pFasSysData != nullptr)
	{
#ifndef ENGLISH_MODE
		if (AfxMessageBox(L"������Ʈ�� �����ֽ��ϴ�. ���� ������Ʈ�� �ݰ� ����ȯ�� �����Ͻðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
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
		GF_AddLog(L"ROM ���Ϸκ��� ������Ʈ�� �����ϴµ� �����߽��ϴ�.");
		AfxMessageBox(L"ROM ���Ϸκ��� ������Ʈ�� �����ϴµ� �����߽��ϴ�.");
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
	AfxMessageBox(L"ROM ���Ϸκ��� ������Ʈ�� �����ϴµ� �����߽��ϴ�.");
#else
	AfxMessageBox(L"Successfully created a project from the ROM file.");
#endif
}


void CSysLinkerApp::OnFacpSetFacppwd()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CDlgMakePwd dlg;
	dlg.DoModal();
}

void CSysLinkerApp::OnFacpRelayedit()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_RELAYEDIT);

}



void CSysLinkerApp::OnUpdateHomeLogin(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
	if (pCmdUI)
		pCmdUI->Enable(!m_bProgramLogin);
}


void CSysLinkerApp::OnUpdateHomeProjectClose(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
	if (pCmdUI)
		pCmdUI->Enable(m_bProgramLogin);
}


void CSysLinkerApp::OnUpdateHomeProjectOpen(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
	if (pCmdUI)
		pCmdUI->Enable(m_bProgramLogin);
}


void CSysLinkerApp::OnUpdateHomeProjectSave(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
	if (pCmdUI)
		pCmdUI->Enable(FALSE);
}


void CSysLinkerApp::OnUpdateBasicSetEquip(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.

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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
#ifndef _DEBUG
	if (pCmdUI)
		pCmdUI->Enable(FALSE);
#endif
}


void CSysLinkerApp::OnUpdateBasicSetUsergroup(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
#ifndef _DEBUG
	if (pCmdUI)
		pCmdUI->Enable(FALSE);
#endif
}


void CSysLinkerApp::OnUpdateBasicSetEditLogic(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
// 	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
	if (pCmdUI)
		//20240312 GBM start - �޴� ��Ȱ��ȭ -> 20240618 GBM start - �ٽ� Ȱ��ȭ
		//pCmdUI->Enable(FALSE);
		pCmdUI->Enable(m_bProgramLogin);
		//20240312 GBM end
}


void CSysLinkerApp::OnUpdateFacpRelayedit(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OpenFormView(FV_ERRORCHECK);
}

void CSysLinkerApp::OnBasicSetEnv()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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
		GF_AddLog(L"�����ͺ��̽� ���̺��� ���µ� �����߽��ϴ�. : TB_EQUIP_MST open failed.");
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
 	// ���� ���� : Project ���� ���� ���� , Symbol �����ؿ��� , �߰�� ���̺�
 	//
 	nRet = CreateProjectFolder();
	if (nRet <= 0)
		return 0;
 	//////////////////////////////////////////////////////////////////////////
 	// Data �Է� - mssql���� DB ��°�� �����ؼ� �ִ´�. - ���̺� , �������� , 
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
	// ���� ���� : Project ���� ���� ���� , Symbol �����ؿ��� , �߰�� ���̺�
	//
	nRet = CreateProjectFolder();
	
	//////////////////////////////////////////////////////////////////////////
	// Data �Է� - mssql���� DB ��°�� �����ؼ� �ִ´�. - ���̺� , �������� , 
	nRet = CreateProjectDatabase();

	return nRet;
}

/*!
* @fn         CreateProjectFolder
* @version    1.0
* @date       2019-12-20 10:15:56
* @param
* @brief      ���ο� ������Ʈ�� �����.
* @return    ���� �� 1 , ���н� 0
* @note
1. ������Ʈ ������ ���� ����
2. ������Ʈ ��ȿ� VERSION_TEMP ���� (���� ����ÿ� VERSION 1.0 ���� ����)
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

	// project verstion_temp ���� ����
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
	// 1. C:\\FICON3\\SYMBOLE ���� Version folder�ȿ� ����.
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
	// 2. �߰�� ���̺�
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
	// MS-SQL DB ���� : program root\db\base.dat , base.log --> Project\version_temp\db
	CString strTarget;
	strFile.Format(L"%s%s\\%s", g_strAppPath, F3_PRJ_DIR_DATABASE, FAS_BASE_BACKUP_FILE);
	if (strTargetDBPath.Right(1) != '\\')
		strTargetDBPath += L"\\";

	strTarget.Format(L"%s%s.BAK", strTargetDBPath,strPrjName);
	if (::CopyFile(strFile, strTarget, FALSE) == FALSE)
		//if (::CopyFile(strFile, strTarget, FALSE) == FALSE)
	{
#ifndef ENGLISH_MODE
		strError.Format(L"DBMS�� ������ ������ �����ϴµ� �����߽��ϴ�.ErrorCode:%ld" , GetLastError());
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
	// Database ���� 
	// 1. Main Database ����
	// 2. Project Database ��� , ����
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
// 			AfxMessageBox(L"���� �����ͺ��̽� ���ӿ� �����߽��ϴ�.\nȯ�漳���� �ٽ� �ϰ� �õ��� �ֽʿ�");
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
// 			strError.Format(L"������Ʈ �����ͺ��̽�(%s)�� ����ϴµ� ���� �߽��ϴ�.\n%s"
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
// 		strError.Format(L"������Ʈ �����ͺ��̽�(%s)�� �����ϴµ� ���� �߽��ϴ�.\n%s"
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

	//20240808 GBM start - ���̺� �������� �ʰ� ���� TB_FACP, TB_UNIT�� Ȱ��
#if 0
	//20240222 GBM start - �߰�� �϶�ǥ �Ľ��� ������ ���� DB�� �غ�� ������ GT1 �߰� ���̺� Data Insert
	CNewDBManager::Instance()->SetDBAccessor(m_pFasSysData->m_pDB);
	//GT1 �߰� �Է� Ÿ���� gfs_base�� �̸� �־� ������Ʈ DB�� �����ϸ� �����ϰ� GT1 �߰� ���̺��� �߰�� �϶�ǥ �󿡼� GT1 �߰� ������ ������ ���� GT1 �߰� ���̺��� ����
	BOOL bRet = FALSE;

	//����ȯ�� ��� ���ű� Ÿ�� ������ ���� GT1 ������Ʈ���� �Ǵ�
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
	// �Ϲ����� �� ������Ʈ ������ ���� �߰�� �϶�ǥ�� �����Ƿ� GT1 ���� ��Ʈ�� �����ϴ� �� ���θ� ���� �Ǵ�
	else
	{
		// GT1�߰� ���̺��� ���� ����� �߰�� �϶�ǥ�� GT1 �߰� ������ �ִ� ��쿡�� ����, �翬�� �ش� Sheet �� �ϳ��� ������ �� �ְ����� Ȥ�� �Ϻ����� ������ DB�� ���� �ʵ��� ��
		// -> ���ű� Ÿ�԰� ���� Ÿ�Ը� ���
		bRet = CNewExcelManager::Instance()->bExistFT && CNewExcelManager::Instance()->bExistUT;
	}
	
	if (bRet)
	{
		BOOL bRet = FALSE;
		bRet = CNewDBManager::Instance()->CheckAndCreateGT1DBTables();
		if (bRet)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"GT1 ���� ���̺� (���ű� TYPE, UNIT TYPE) ������ �����߽��ϴ�.");
#else
			GF_AddLog(L"Successfully created the GT1 information table (FACP TYPE, UNIT TYPE).");
#endif
			Log::Trace("Inserting new DB table succeeded!");
		}
		else
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"GT1 ���� ���̺� (���ű� TYPE, UNIT TYPE) ������ �����߽��ϴ�, DB�� Ȯ���ϼ���.");
#else
			GF_AddLog(L"Failed to create the GT1 information table (FACP TYPE, UNIT TYPE). Please check the DB.");
#endif
			Log::Trace("Inserting new DB table failed!");
		}

		//20240222 GBM start - �߰�� �϶�ǥ �Ľ��� ���� ������ GT1 �߰� ���̺� Data Insert
		bRet = CNewDBManager::Instance()->InsertDatasIntoGT1DBTables();
		if (bRet)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"GT1 ���� ���̺� (���ű� TYPE, UNIT TYPE) ������ �߰��� �����߽��ϴ�.");
#else
			GF_AddLog(L"Successfully added the GT1 information table (FACP TYPE, UNIT TYPE) data.");
#endif
			Log::Trace("GT1 DB table insertion succeeded!");
		}
		else
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"GT1 ���� ���̺� (���ű� TYPE, UNIT TYPE) ������ �߰��� �����߽��ϴ�, DB�� Ȯ���ϼ���.");
#else
			GF_AddLog(L"Failed to add the GT1 information table (FACP TYPE, UNIT TYPE) data. Please check the DB.");
#endif
			Log::Trace("GT1 DB table insertion failed!");
		}
		//20240222 GBM end

		//20240703 GBM start - �߰�� �϶�ǥ ���� ��� Disable
#ifdef MODULE_TABLE_UPDATE_MODE
		//20240422 GBM start
		//GT1�� F3�� ȥ��Ǿ� ���� ��� ����ġ�Ǵ� ���� ���ǰ� �޸𸮿� �߰� �ݿ��Ǿ� ��(m_pFasSysData->InsertPrjBaseData())���� DB�� ��������
		//�߰�� �϶�ǥ Sheet���� ������� �����Ƿ� �̸� ����, �߰��� ���� ������ CRelayTableData::AddNewEquip(CString strEquipName, int nType)���� 
		//�߰�� �϶�ǥ ���� ���� �޸𸮿� �ݿ����� ����(�� �߰�� �϶�ǥ ���� �� �񱳵����Ͱ� �ƴϹǷ�), 
		//�� �������� �ѹ��̶� �� ���� ���ǰ� �߰��Ǿ��ٸ� ������Ʈ ������ �߰�� �϶�ǥ ������ �̹� �߰�� �϶�ǥ ���簡 �Ϸ�Ǿ����Ƿ� ���� ����
		//�� �۾��� �߰�� �϶�ǥ�� �ٽ� �� ����� �ϹǷ� ������ ũ�� ���� �ɸ��� �۾��̹Ƿ� �� ���ǰ� �߰��Ǿ��� ���� ����
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
					nID--;	//��ȣ�� 1���̽� �ε����� 0���̽�
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
				GF_AddLog(L"���� ���� Sheet�� ��� �߰��� ���� ���Ǹ� �߰�� �϶�ǥ�� �����ϴ� ���� �����߽��ϴ�.");
#else
				GF_AddLog(L"Successfully saved the added equipment definition to the module table because it was not found in the equipment definition sheet.");
#endif
				Log::Trace("We succeeded in saving the added equipment definition to the equipment definition sheet in module table file because it was not in the equipment definition sheet.");
			}
			else
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"���� ���� Sheet�� ��� �߰��� ���� ���Ǹ� �߰�� �϶�ǥ�� �����ϴ� ���� �����߽��ϴ�.");
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
		// Project File ���� ����
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ ������ �����ϴµ� �����߽��ϴ�.\n");
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
// 	file.WriteString(m_pFasSysData->GetPrjModifyDate().Format(L"%Y-%m-%d %H:%M:%S") + L"\n");	//20240731 GBM - ������Ʈ ���� �ð� ���� (�ּ� ����)
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
		// Project File ���� ����
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ ������ �����ϴµ� �����߽��ϴ�.\n");
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
		// Project File ���� ����
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ ������ �����ϴµ� �����߽��ϴ�.\n");
#else
		GF_AddLog(L"Failed to create the project file.\n");
#endif
		return 0;
	}
	// Project Version Temp Folder ����
	// DB ���� : Main DB class���� Version DB�����ϰ� Project Name_temp
	// ����� ���� ���� File�� ����
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
		// Project File ���� ����
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ ������ �����ϴµ� �����߽��ϴ�.\n");
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
		strError.Format(L"���α׷� ���� �����ͺ��̽�(%s)�� ����ϴµ� �����߽��ϴ�.\n%s"
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
			// �������� ���
			// MainDB ������
			if (RegisterBasicDB() <= 0)
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"�������� �����ͺ��̽��� ������ ����ϴµ� �����߽��ϴ�.");
				AfxMessageBox(L"�������� �����ͺ��̽��� ������ ����ϴµ� �����߽��ϴ�.\r\nMSSQL Server�� ��ġ�ϰų� ����Ʋ �������ֽñ� �ٶ��ϴ�.");
#else
				GF_AddLog(L"Failed to register the default information database with the server.");
				AfxMessageBox(L"Failed to register the default information database with the server.\nPlease install MSSQL Server or start the service.");
#endif
				return 0;
			}
			if (m_pMainDb->DBOpen() == FALSE)
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"�������� �����ͺ��̽� ���ӿ� �����߽��ϴ�.");
				AfxMessageBox(L"�����ͺ��̽� ���ӿ� �����߽��ϴ�.");
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
	// Database ���� 
	// 1. Main Database ����
	// 2. Project Database ��� , ����
	CString strSql;
	CString strDBPath, strPath;
	CString strBkName , strDB , strLog;
	int nRet = 0;
	if (m_pMainDb == nullptr)
	{
		if (OpenBaseDatabase() == 0)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"�������� �����ͺ��̽��� ���µ� �����߽��ϴ�.");
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

	// �̹� ��ϵǾ� ������ DB�и�
	if (m_pMainDb->IsAttachedDatabase(pFasSysData->GetDBName()) == TRUE)
	{
		if (m_pMainDb->DetachMSDB(pFasSysData->GetDBName()) == FALSE)
		{
			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"������Ʈ �����ͺ��̽�(%s)�� �и��ϴµ� �����߽��ϴ�.\n%s"
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
	if (GF_FindFile(strDBPath, strDB) == TRUE) // DB ��� �ʿ�
	{
		strDB.Format(L"%s%s_DATA.MDF", strDBPath, pFasSysData->GetDBName());
		strLog.Format(L"%s%s_LOG.LDF", strDBPath, pFasSysData->GetDBName());
		//ATTACH
		if (m_pMainDb->AttachMSDB(pFasSysData->GetDBName(), strDB, strLog) == FALSE)
		{

			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"������Ʈ �����ͺ��̽�(%s)�� ����ϴµ� �����߽��ϴ�.\n%s"
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
	else // Restore �ʿ�
	{
		strBkName.Format(L"%s.BAK", pFasSysData->GetDBName());
		if (m_pMainDb->RestoreDatabase(pFasSysData->GetDBName(), strDBPath, strBkName) == FALSE)
		{

			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"������Ʈ �����ͺ��̽�(%s)�� �����ϴµ� �����߽��ϴ�.\n%s"
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
		strError.Format(L"������Ʈ �����ͺ��̽�(%s)�� �����ϴµ� �����߽��ϴ�.\n%s"
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
	// Project ���� 
	// �α��� ������Ʈ --> Version_Temp
	if (strPrjFullPath.Right(1) != '\\')
		strPrjFullPath += L"\\";

	if(bVerTemp == FALSE)
	{
		if (CopyProjectVersionTemp(strPrjName, strPrjFullPath, dwVer) <= 0)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"������Ʈ ������ �������µ� �����߽��ϴ�.");
			AfxMessageBox(L"������Ʈ ������ �������µ� �����߽��ϴ�.");
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

	//20240523 GBM start - ������Ʈ �ε� �� ���� �ð��� ���� �ɸ��� �Ʒ� ������ ������� ��ü
#if 1
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bThreadSucceeded = FALSE;
#ifndef ENGLISH_MODE
	CString strMsg = _T("        ������Ʈ DB�� ���� ���Դϴ�. ��� ��ٷ� �ּ���.");
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
		Log::Trace("������ ��� ����! dw : %d", dw);
	}

	if (m_bThreadSucceeded)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ DB ���⿡ �����߽��ϴ�.");
#else
		GF_AddLog(L"Successfully opened the project DB.");
#endif
		Log::Trace("Successfully opened the project DB.");
	}
	else
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"������Ʈ DB ���⿡ �����߽��ϴ�.");
#else
		GF_AddLog(L"Failed to open the project DB.");
#endif
		Log::Trace("Failed to open project DB.");
	}

#else
	if (OpenProjectDatabase(m_pFasSysData) < 0)
	{
		GF_AddLog(L"������Ʈ �����ͺ��̽��� ���µ� ���� �߽��ϴ�.");
		return 0;
	}

	//20240327 GBM start - DB Open �� DB���� �����͸� �������� ���� GT1 �߰� �Է� Ÿ���� �߰� -> �߰�� �϶�ǥ(WEB)���� ȸ�� ���� ���� ���Ǹ� ������ ���� ������ �� ������ ���� ����

	// 	CNewDBManager::Instance()->SetDBAccessor(theApp.m_pFasSysData->m_pDB);
	// 
	// 	BOOL bRet = FALSE;
	// 
	// 	bRet = CNewDBManager::Instance()->CheckAndInsertEquipmentNewInputType();
	// 	if (bRet)
	// 	{
	// 		GF_AddLog(L"GT1 �Է� Ÿ�� �ڵ� �߰��� �����߽��ϴ�.");
	// 		Log::Trace("Inserting a new input type of equipment succeeded!");
	// 	}
	// 	else
	// 	{
	// 		GF_AddLog(L"GT1 �Է� Ÿ�� �ڵ� �߰��� �����߽��ϴ�. ��� ���� �Է� Ÿ���� Ȯ���ϼ���.");
	// 		Log::Trace("Inserting a new input type of equipment failed!");
	// 	}

	//20240327 GBM end

	if (m_pFasSysData->LoadProjectDatabase() == 0)
	{
		GF_AddLog(L"������Ʈ �����ͺ��̽����� �����͸� �������� ���� ���� �߽��ϴ�.");
		return 0;
	}
	m_pFasSysData->SetProjectOpened(TRUE);
	//GF_AddLog(L"������Ʈ�� �ҷ����µ� ���� �߽��ϴ�.");	//20240424 GBM - �ߺ� �޼��� ����
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
		if (AfxMessageBox(L"��������� �ֽ��ϴ�.\n������Ʈ�� �����Ͻðڽ��ϱ�?"
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

	//20240307 GBM start - GT1 �߰� ���� �޸� �ʱ�ȭ
	memset(&CNewInfo::Instance()->m_gi, NULL, sizeof(GT1APPENDIX_INFO));
	memset(&CNewInfo::Instance()->m_ei, NULL, sizeof(EQUIPMENT_INFO));
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
	// 1. C:\\FICON3\\SYMBOLE ���� Version folder�ȿ� ����.
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
	// 1. Version_Temp\\ver.pvi �̵�
	// 2. ETC ���� �̵�
	// 3. MAP ���� �̵�
	// 4. RELAYTABLE 
	// 5. RESOURCE
	// 6. SYMBOL
	// 7. DB���� ����

	if (strPrjFullPath.Right(1) != '\\')
		strPrjFullPath += L"\\";
	
	strFrom.Format(L"%s%s\\%s"
		, strPrjFullPath, F3_VERSIONTEMPFOLDER_NAME
		, F3_VERSIONFINFO_FILE
	);
	//////////////////////////////////////////////////////////////////////////
	// 1. Version folder ���� Version_Temp\\ver.pvi �̵�
	strTo.Format(L"%s%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
	);
	if (CreateDirectory(strTo,nullptr) == FALSE)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"���� ������ �����ϴµ� �����߽��ϴ�.");
			AfxMessageBox(L"���� ������ �����ϴµ� �����߽��ϴ�.");
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
		GF_AddLog(L"�������� ������ �̵��ϴµ� �����߽��ϴ�.");
		AfxMessageBox(L"�������� ������ �̵��ϴµ� �����߽��ϴ�.");
#else
		GF_AddLog(L"Failed to move the version information file.");
		AfxMessageBox(L"Failed to move the version information file.");
#endif
		return 0; 
	}
	//////////////////////////////////////////////////////////////////////////
	// 2. ETC ���� �̵�
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
	// 3. MAP ���� �̵�
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
	// 7. DB���� ����
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_DATABASE
	);
	if (CreateDirectory(strTo, nullptr) == FALSE)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"�����ͺ��̽� ������ �����ϴµ� �����߽��ϴ�.");
			AfxMessageBox(L"�����ͺ��̽� ������ �����ϴµ� �����߽��ϴ�.");
#else
			GF_AddLog(L"Failed to create a database folder.");
			AfxMessageBox(L"Failed to create a database folder.");
#endif
			return 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 8. RELEASE ���� ����
	strTo.Format(L"%s%s\\%s"
		, strPrjFullPath, GF_GetVersionFolderName(wMajor, wMinor)
		, F3_PRJ_DIR_RELEASE
	);
	if (CreateDirectory(strTo, nullptr) == FALSE)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
#ifndef ENGLISH_MODE
			GF_AddLog(L"RELEASE ������ �����ϴµ� �����߽��ϴ�.");
			AfxMessageBox(L"RELEASE ������ �����ϴµ� �����߽��ϴ�.");
#else
			GF_AddLog(L"Failed to create a RELEASE folder.");
			AfxMessageBox(L"Failed to create a RELEASE folder.");
#endif
			return 0;
		}
	}
	return 1;
}


//void CAboutDlg::OnChkViewdkoutput()
//{
//	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
//}


void CSysLinkerApp::OnFileClose()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CSysLinkerApp::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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