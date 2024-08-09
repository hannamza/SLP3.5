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

// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "SysLinker.h"

#include "MainFrm.h"


#include "SysLinkerDoc.h"
#include "SysLinkerView.h"
#include "RelayTableData.h"

#include "DlgErrorCheck.h"

#include "DlgAdminMode.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)

	ON_REGISTERED_MESSAGE(AFX_WM_CHANGING_ACTIVE_TAB, &CMainFrame::OnAfxWmChangingActiveTab)

	ON_MESSAGE(UWM_REG_OUTVIEW_DROPWND, &CMainFrame::OnRegisterOutViewTree)
	ON_MESSAGE(UWM_REG_PTNVIEW_DROPWND, &CMainFrame::OnRegisterPatternViewTree)
	ON_MESSAGE(UWM_REG_PUMPVIEW_DROPWND, &CMainFrame::OnRegisterPumpViewTree)
	ON_MESSAGE(UWM_REG_CONTACTVIEW_DROPWND, &CMainFrame::OnRegisterContactViewTree)
	ON_MESSAGE(UWM_REG_PSWITCHVIEW_DROPWND, &CMainFrame::OnRegisterPSwitchViewTree)
	ON_MESSAGE(UWM_REG_EMVIEW_DROPWND, &CMainFrame::OnRegisterEmergencyViewTree)
	ON_MESSAGE(UWM_DKP_INPUTVIEW_ITEMCHANGE, &CMainFrame::OnDkpInputViewItemChange)
	ON_MESSAGE(UWM_DKP_INPUTVIEW_REFRESH, &CMainFrame::OnDkpInputRefresh)
	ON_MESSAGE(UWM_DKP_OUTPUTVIEW_REFRESH, &CMainFrame::OnDkpOutputRefresh)
	ON_MESSAGE(UWM_DKP_PATTERN_REFRESH, &CMainFrame::OnDkpPatternRefresh)
	ON_MESSAGE(UWM_DKP_EMERGENCY_REFRESH, &CMainFrame::OnDkpEmergencyRefresh)
	ON_MESSAGE(UWM_DKP_PSWITCH_REFRESH, &CMainFrame::OnDkpPSwitchRefresh)
	ON_MESSAGE(UWM_DKP_PUMP_REFRESH, &CMainFrame::OnDkpPumpRefresh)
	ON_MESSAGE(UWM_DKP_CONTACT_REFRESH, &CMainFrame::OnDkpContactRefresh)

	ON_MESSAGE(UWM_VIEWFORM_RELAY, &CMainFrame::OnViewFormRelayItem)

	ON_MESSAGE(UWM_VIEWFORM_ADDITEM, &CMainFrame::OnViewFormAddItem)
	ON_MESSAGE(UWM_VIEWFORM_DELITEM, &CMainFrame::OnViewFormDeleteItem)
	ON_MESSAGE(UWM_VIEWFORM_CHANGEITEM, &CMainFrame::OnViewFormChangeItem)

	ON_MESSAGE(UWM_ERRORCHECK_NOTIFY,&CMainFrame::OnErrorCheckEnd)
	ON_MESSAGE(UWM_ERRORCHECK_CREATELINK,&CMainFrame::OnErrorCheckAfterCreateLink)
	//ON_MESSAGE(UWM_ERRORCHECK_CLOSE,&CMainFrame::OnErrorCheckClose)

	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)

	ON_COMMAND(ID_CHK_VIEWDKOUTPUT, &CMainFrame::OnChkViewdkoutput)
	ON_UPDATE_COMMAND_UI(ID_CHK_VIEWDKOUTPUT, &CMainFrame::OnUpdateChkViewdkoutput)
	ON_COMMAND(ID_CHK_VIEWDKPATTERN, &CMainFrame::OnChkViewdkpattern)
	ON_UPDATE_COMMAND_UI(ID_CHK_VIEWDKPATTERN, &CMainFrame::OnUpdateChkViewdkpattern)
	ON_COMMAND(ID_CHK_VIEWDKINPUT, &CMainFrame::OnChkViewdkinput)
	ON_UPDATE_COMMAND_UI(ID_CHK_VIEWDKINPUT, &CMainFrame::OnUpdateChkViewdkinput)
	ON_COMMAND(ID_CHK_VIEWDKEMERGENCY, &CMainFrame::OnChkViewdkemergency)
	ON_UPDATE_COMMAND_UI(ID_CHK_VIEWDKEMERGENCY, &CMainFrame::OnUpdateChkViewdkemergency)
	ON_COMMAND(ID_CHK_VIEWDKPSWITCH, &CMainFrame::OnChkViewdkpswitch)
	ON_UPDATE_COMMAND_UI(ID_CHK_VIEWDKPSWITCH, &CMainFrame::OnUpdateChkViewdkpswitch)
	ON_COMMAND(ID_CHK_VIEWDKPUMP, &CMainFrame::OnChkViewdkpump)
	ON_UPDATE_COMMAND_UI(ID_CHK_VIEWDKPUMP, &CMainFrame::OnUpdateChkViewdkpump)
	ON_COMMAND(ID_CHK_VIEWDKCONTACT, &CMainFrame::OnChkViewdkContact)
	ON_UPDATE_COMMAND_UI(ID_CHK_VIEWDKCONTACT, &CMainFrame::OnUpdateChkViewdkContact)
	ON_COMMAND(ID_FACP_CREATE_LINK,&CMainFrame::OnFacpCreateLink)
	ON_UPDATE_COMMAND_UI(ID_FACP_CREATE_LINK,&CMainFrame::OnUpdateFacpCreateLink)

	ON_COMMAND(ID_HOME_ERROR_CHECK,&CMainFrame::OnSimpleErrorCheck)
	ON_UPDATE_COMMAND_UI(ID_HOME_ERROR_CHECK,&CMainFrame::OnUpdateSimpleErrorCheck)
#if _USE_OUTPUTWND_
	ON_COMMAND(ID_CHK_VIEWDKLOG, &CMainFrame::OnChkViewdkLog)
	ON_UPDATE_COMMAND_UI(ID_CHK_VIEWDKLOG, &CMainFrame::OnUpdateChkViewdkLog)
#endif
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
	m_pRefFasSysData = nullptr;
	m_pDlgErrorCheck = nullptr;
}

CMainFrame::~CMainFrame()
{
	if(m_pDlgErrorCheck)
	{
		delete m_pDlgErrorCheck;
		m_pDlgErrorCheck = nullptr;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 사용할 수 있는 다른 스타일...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bTabIcons = FALSE;    // TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor = TRUE;    // FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu = TRUE; // 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
#ifndef ENGLISH_MODE
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
#else
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON_EN);
#endif

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strTitlePane1;
	CString strTitlePane2;
#ifndef ENGLISH_MODE
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
#else
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1_EN);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2_EN);
	ASSERT(bNameValid);
#endif
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	m_wndDkInputRelay.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndDkInputRelay);
	CDockablePane* pTabbedBar = NULL;

//	m_wndDkInputRelay.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
#if _USE_OUTPUTWND_
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	g_pLogWnd = &m_wndOutput;
	CString strCat, strName;
	BOOL bFind = FALSE;
//	DockPane(&m_wndOutput);

	// 	CMFCRibbonButton* pBtn = NULL;
	// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, m_wndRibbonBar.FindByID(ID_SECURITY));

// 	int nCategoryCount = m_wndRibbonBar.GetCategoryCount();
// 	for (i = 0; i < nCategoryCount; i++)
// 	{
// 		prc = m_wndRibbonBar.GetCategory(i);
// 		if (prc == nullptr)
// 			continue;
// 		strCat = prc->GetName();
// 		if (strCat != L"홈")
// 			continue;
// 		nPCnt = prc->GetPanelCount();
// 		for (x = 0; x < nPCnt; x++)
// 		{
// 			pPnl = prc->GetPanel(x);
// 			if (pPnl == nullptr)
// 				continue;
// 			strName = pPnl->GetName();
// 			if (strName != L"창")
// 				continue;
// 			pBtn = new CMFCRibbonCheckBox(ID_CHK_VIEWDKLOG, L"로그 뷰 보기");
// 			pPnl->Add(pBtn);
// 
// 		}
// 	}
	m_wndOutput.DockToWindow(&m_wndDkInputRelay, CBRS_BOTTOM);
#endif

	m_wndDkOutputRelay.EnableDocking(CBRS_ALIGN_ANY);
	m_wndDkEmergency.EnableDocking(CBRS_ALIGN_ANY);
	m_wndDkPump.EnableDocking(CBRS_ALIGN_ANY);
	m_wndDkPattern.EnableDocking(CBRS_ALIGN_ANY);
	m_wndDkContact.EnableDocking(CBRS_ALIGN_ANY);

	DockPane(&m_wndDkOutputRelay);
	DockPane(&m_wndDkPattern);
	DockPane(&m_wndDkEmergency);
	DockPane(&m_wndDkPump);
	DockPane(&m_wndDkContact);
#if _USE_PSWITCH_
	m_wndDkPSwitch.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndDkPSwitch);
#endif
	m_wndDkEmergency.AttachToTabWnd(&m_wndDkPSwitch, DM_SHOW, TRUE, &pTabbedBar);
	m_wndDkPump.AttachToTabWnd(&m_wndDkEmergency, DM_SHOW, TRUE, &pTabbedBar);
	m_wndDkPattern.AttachToTabWnd(&m_wndDkPump, DM_SHOW, TRUE, &pTabbedBar);
	m_wndDkContact.DockToWindow(&m_wndDkPattern, CBRS_TOP);
	m_wndDkOutputRelay.DockToWindow(&m_wndDkContact, CBRS_TOP);

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	// 향상된 창 관리 대화 상자를 활성화합니다.
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 창 제목 표시줄에서 문서 이름 및 응용 프로그램 이름의 순서를 전환합니다.
	// 문서 이름이 축소판 그림과 함께 표시되므로 작업 표시줄의 기능성이 개선됩니다.
	ModifyStyle(0, FWS_PREFIXTITLE);
#ifndef ENGLISH_MODE
	GF_AddLog(L"SLP3 프로그램이 정상 기동됐습니다.");
#else
	GF_AddLog(L"The SLP3 program has been started successfully.");
#endif
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.style &= ~FWS_ADDTOTITLE;
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 클래스 뷰를 만듭니다.
#ifndef ENGLISH_MODE

	CString strClassView;
	bNameValid = strClassView.LoadString(ID_DKPANE_INPUTRELAY);
	ASSERT(bNameValid);
	if (!m_wndDkInputRelay.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_INPUTRELAY, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	CString strOut;
	bNameValid = strOut.LoadString(ID_DKPANE_OUTPUTRELAY);
	ASSERT(bNameValid);
	if (!m_wndDkOutputRelay.Create(strOut, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_OUTPUTRELAY, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	CString strPattern;
	bNameValid = strPattern.LoadString(ID_DKPANE_PATTERN);
	ASSERT(bNameValid);
	if (!m_wndDkPattern.Create(strPattern, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_PATTERN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	// 속성 창을 만듭니다.

	CString strEm;
	bNameValid = strEm.LoadString(ID_DKPANE_EMERGENCY);
	ASSERT(bNameValid);
	if (!m_wndDkEmergency.Create(strEm, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_EMERGENCY, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	CString strPump;
	bNameValid = strPump.LoadString(ID_DKPANE_PUMP);
	ASSERT(bNameValid);
	if (!m_wndDkPump.Create(strPump, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_PUMP, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
#if _USE_PSWITCH_
	CString strPSwitch;
	bNameValid = strPSwitch.LoadString(ID_DKPANE_PSWITCH);
	ASSERT(bNameValid);
	if (!m_wndDkPSwitch.Create(strPSwitch, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_PSWITCH, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
#endif

#if _USE_OUTPUTWND_
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
#endif

	CString strContact;
	bNameValid = strContact.LoadString(IDS_DKPANE_CONTACT);
	ASSERT(bNameValid);
	if (!m_wndDkContact.Create(strContact, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_CONTACT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

#else

	CString strClassView;
	bNameValid = strClassView.LoadString(ID_DKPANE_INPUTRELAY_EN);
	ASSERT(bNameValid);
	if (!m_wndDkInputRelay.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_INPUTRELAY_EN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	CString strOut;
	bNameValid = strOut.LoadString(ID_DKPANE_OUTPUTRELAY_EN);
	ASSERT(bNameValid);
	if (!m_wndDkOutputRelay.Create(strOut, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_OUTPUTRELAY_EN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	CString strPattern;
	bNameValid = strPattern.LoadString(ID_DKPANE_PATTERN_EN);
	ASSERT(bNameValid);
	if (!m_wndDkPattern.Create(strPattern, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_PATTERN_EN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	// 속성 창을 만듭니다.

	CString strEm;
	bNameValid = strEm.LoadString(ID_DKPANE_EMERGENCY_EN);
	ASSERT(bNameValid);
	if (!m_wndDkEmergency.Create(strEm, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_EMERGENCY_EN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	CString strPump;
	bNameValid = strPump.LoadString(ID_DKPANE_PUMP_EN);
	ASSERT(bNameValid);
	if (!m_wndDkPump.Create(strPump, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_PUMP_EN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
#if _USE_PSWITCH_
	CString strPSwitch;
	bNameValid = strPSwitch.LoadString(ID_DKPANE_PSWITCH_EN);
	ASSERT(bNameValid);
	if (!m_wndDkPSwitch.Create(strPSwitch, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_PSWITCH_EN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
#endif

#if _USE_OUTPUTWND_
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND_EN);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, IDS_OUTPUT_WND_EN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
#endif

	CString strContact;
	bNameValid = strContact.LoadString(ID_DKPANE_CONTACT_EN);
	ASSERT(bNameValid);
	if (!m_wndDkContact.Create(strContact, this, CRect(0, 0, 200, 200), TRUE, ID_DKPANE_CONTACT_EN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
#endif

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDkInputRelay.SetIcon(hClassViewIcon, FALSE);

	HICON hOutViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUT_VIEW_HC : IDI_OUT_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDkOutputRelay.SetIcon(hOutViewIcon, FALSE);

	HICON hPtrnViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PTRN_VIEW_HC : IDI_PTRN_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDkPattern.SetIcon(hPtrnViewIcon, FALSE);

	HICON hEmViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PTRN_VIEW_HC : IDI_PTRN_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDkEmergency.SetIcon(hPtrnViewIcon, FALSE);

	HICON hPumpViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PTRN_VIEW_HC : IDI_PTRN_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDkPump.SetIcon(hPtrnViewIcon, FALSE);

#if _USE_PSWITCH_
	HICON hPSwitchViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PTRN_VIEW_HC : IDI_PTRN_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDkPSwitch.SetIcon(hPtrnViewIcon, FALSE);
#endif
#if _USE_OUTPUTWND_
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);
#endif

	HICON hContViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PTRN_VIEW_HC : IDI_PTRN_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDkContact.SetIcon(hContViewIcon, FALSE);
	UpdateMDITabbedBarsIcons();
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}
#if _USE_OUTPUTWND_
	m_wndOutput.UpdateFonts();
#endif

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}




int CMainFrame::MakeProjectFolder()
{
	return 0;
}



LRESULT CMainFrame::OnRegisterPatternViewTree(WPARAM wp, LPARAM lp)
{
	m_wndDkPattern.AddDropWnd((CWnd*)lp);
	return 0;
}


LRESULT CMainFrame::OnRegisterOutViewTree(WPARAM wp, LPARAM lp)
{
	m_wndDkOutputRelay.AddDropWnd((CWnd*)lp);
	return 0;
}


LRESULT CMainFrame::OnRegisterEmergencyViewTree(WPARAM wp, LPARAM lp)
{
	m_wndDkEmergency.AddDropWnd((CWnd*)lp);
	return 0;
}


LRESULT CMainFrame::OnRegisterPumpViewTree(WPARAM wp, LPARAM lp)
{
	m_wndDkPump.AddDropWnd((CWnd*)lp);
	return 0;
}


LRESULT CMainFrame::OnRegisterContactViewTree(WPARAM wp, LPARAM lp)
{
	m_wndDkContact.AddDropWnd((CWnd*)lp);
	return 0;
}

LRESULT CMainFrame::OnRegisterPSwitchViewTree(WPARAM wp, LPARAM lp)
{
#if _USE_PSWITCH_
	m_wndDkPSwitch.AddDropWnd((CWnd*)lp);
#endif
	return 0;
}

LRESULT CMainFrame::OnDkpInputViewItemChange(WPARAM wp, LPARAM lp)
{
	CSysLinkerView *pView;
	pView = (CSysLinkerView*)theApp.GetSysLinkerView(theApp.m_pTempleMakeLink);
	if (pView == nullptr)
		return 0;

	pView->SetSelectTreeItem((CPtrList*)wp , (ST_TREEITEM*)lp);
	return 0;
}

LRESULT CMainFrame::OnDkpInputRefresh(WPARAM wp, LPARAM lp)
{
	if(wp == DATA_ALL)
		m_wndDkInputRelay.InitTree();
	return 0;
}

LRESULT CMainFrame::OnDkpOutputRefresh(WPARAM wp, LPARAM lp)
{
	if (wp == DATA_ALL)
		m_wndDkOutputRelay.InitTree();
	return 0;
}

LRESULT CMainFrame::OnDkpPatternRefresh(WPARAM wp, LPARAM lp)
{
	if (wp == DATA_ALL)
		m_wndDkPattern.InitTree();
	return 0;
}

LRESULT CMainFrame::OnDkpEmergencyRefresh(WPARAM wp, LPARAM lp)
{
	if (wp == DATA_ALL)
		m_wndDkEmergency.InitTree();
	return 0;
}

LRESULT CMainFrame::OnDkpPSwitchRefresh(WPARAM wp, LPARAM lp)
{
	if (wp == DATA_ALL)
		m_wndDkPSwitch.InitTree();
	return 0;
}

LRESULT CMainFrame::OnDkpPumpRefresh(WPARAM wp, LPARAM lp)
{
	if (wp == DATA_ALL)
		m_wndDkPump.InitTree();
	return 0;
}


LRESULT CMainFrame::OnDkpContactRefresh(WPARAM wp, LPARAM lp)
{
	if (wp == DATA_ALL)
		m_wndDkContact.InitTree();
	return 0;
}

////

LRESULT CMainFrame::OnViewFormRelayItem(WPARAM wp, LPARAM lp)
{
	m_wndDkInputRelay.SelectItem((int)wp, (LPVOID)lp);
	return 0;
}


LRESULT CMainFrame::OnViewFormAddItem(WPARAM wp, LPARAM lp)
{
	int nRet = m_wndDkInputRelay.AddTreeItem(wp, (CDataSystem*)lp);
	if (nRet <= 0)
	{
		if (AfxGetApp())
			((CSysLinkerApp*)AfxGetApp())->PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
	}
	else
	{
		theApp.PostMessageFormView(FV_MAKEPATTERN, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_PSWITCH, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_SWITCH, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LINKVIEW, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_PUMP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_EQUIP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_USER, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_ACCESS, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_EMERGENCY, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOCATION, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_FACP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_UNIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_RELAYEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_AUTOMAKE, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOGICEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOGICEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOADRELAYTABLE, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);

		m_wndDkOutputRelay.InitTree();
		m_wndDkPattern.InitTree();
		m_wndDkEmergency.InitTree();
		m_wndDkPSwitch.InitTree();
		m_wndDkPump.InitTree();
		m_wndDkContact.InitTree();
	}
	return 0;
}


LRESULT CMainFrame::OnViewFormDeleteItem(WPARAM wp, LPARAM lp)
{
	//m_wndDkInputRelay.SelectItem(wp, (LPVOID)lp);
	int nRet = m_wndDkInputRelay.DeleteTreeItem(wp, (ST_TREEITEM*)lp);
	if (nRet < 0)
	{
		if (AfxGetApp())
			((CSysLinkerApp*)AfxGetApp())->PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
	}
	else
	{
		theApp.PostMessageFormView(FV_MAKEPATTERN, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_PSWITCH, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_SWITCH, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LINKVIEW, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_PUMP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_EQUIP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_USER, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_ACCESS, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_EMERGENCY, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOCATION, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_FACP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_UNIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_RELAYEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_AUTOMAKE, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOGICEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOGICEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOADRELAYTABLE, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);

		m_wndDkOutputRelay.InitTree();
		m_wndDkPattern.InitTree();
		m_wndDkEmergency.InitTree();
		m_wndDkPSwitch.InitTree();
		m_wndDkPump.InitTree();
		m_wndDkContact.InitTree();
	}
	return 0;
}


LRESULT CMainFrame::OnViewFormChangeItem(WPARAM wp, LPARAM lp)
{
	int nRet = m_wndDkInputRelay.ChangeTreeItem(wp, (ST_TREEITEM*)lp);
	if (nRet < 0)
	{
		if (AfxGetApp())
			((CSysLinkerApp*)AfxGetApp())->PostMessageAllView(UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
	}
	else
	{
		theApp.PostMessageFormView(FV_MAKEPATTERN, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_PSWITCH, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_SWITCH, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LINKVIEW, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_PUMP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_EQUIP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_USER, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_ACCESS, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_EMERGENCY, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOCATION, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_FACP, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_UNIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_RELAYEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_AUTOMAKE, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOGICEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOGICEDIT, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);
		theApp.PostMessageFormView(FV_LOADRELAYTABLE, UDBC_ALLDATA_INIT, FORM_PRJ_REFESH, 0);

		m_wndDkOutputRelay.InitTree();
		m_wndDkPattern.InitTree();
		m_wndDkEmergency.InitTree();
		m_wndDkPSwitch.InitTree();
		m_wndDkPump.InitTree();
		m_wndDkContact.InitTree();
	}
	return 0;
}
// 
// LRESULT CMainFrame::OnChangeFormEmergency(WPARAM wp, LPARAM lp)
// {
// // 	if (wp == DATA_ALL)
// // 		m_wndDkOutputRelay.InitTree();
// 	return 0;
// }
// 
// LRESULT CMainFrame::OnChangeFormPattern(WPARAM wp, LPARAM lp)
// {
// 	theApp.OpenFormView()
// 	return 0;
// }
// 
// LRESULT CMainFrame::OnChangeFormPump(WPARAM wp, LPARAM lp)
// {
// // 	if (wp == DATA_ALL)
// // 		m_wndDkEmergency.InitTree();
// 	return 0;
// }
// 
// LRESULT CMainFrame::OnChangeFormPSwitch(WPARAM wp, LPARAM lp)
// {
// // 	if (wp == DATA_ALL)
// // 		m_wndDkPSwitch.InitTree();
// 	return 0;
// }
// 
// LRESULT CMainFrame::OnChangeFormEquip(WPARAM wp, LPARAM lp)
// {
// // 	if (wp == DATA_ALL)
// // 		m_wndDkPump.InitTree();
// 	return 0;
// }

int CMainFrame::SetRelayTable(CRelayTableData* pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
	m_wndDkInputRelay.SetRelayTable(m_pRefFasSysData);
	m_wndDkInputRelay.InitTree();

	m_wndDkOutputRelay.SetRelayTable(m_pRefFasSysData);
	m_wndDkOutputRelay.InitTree();

	m_wndDkPattern.SetRelayTable(m_pRefFasSysData);
	m_wndDkPattern.InitTree();

	m_wndDkEmergency.SetRelayTable(m_pRefFasSysData);
	m_wndDkEmergency.InitTree();

	m_wndDkPump.SetRelayTable(m_pRefFasSysData);
	m_wndDkPump.InitTree();

	m_wndDkEmergency.SetRelayTable(m_pRefFasSysData);
	m_wndDkEmergency.InitTree();

	m_wndDkContact.SetRelayTable(m_pRefFasSysData);
	m_wndDkContact.InitTree();

#if _USE_PSWITCH_
	m_wndDkPSwitch.SetRelayTable(m_pRefFasSysData);
	m_wndDkPSwitch.InitTree();
#endif
	return 1;
}

void CMainFrame::OnChkViewdkoutput()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_wndDkOutputRelay.IsVisible())
		m_wndDkOutputRelay.ShowPane(FALSE, FALSE, FALSE);
	else
		m_wndDkOutputRelay.ShowPane(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateChkViewdkoutput(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->SetCheck(m_wndDkOutputRelay.IsVisible());

}


void CMainFrame::OnChkViewdkpattern()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_wndDkPattern.IsVisible())
		m_wndDkPattern.ShowPane(FALSE, FALSE, FALSE);
	else
		m_wndDkPattern.ShowPane(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateChkViewdkpattern(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->SetCheck(m_wndDkPattern.IsVisible());
}


void CMainFrame::OnChkViewdkinput()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_wndDkInputRelay.IsVisible())
		m_wndDkInputRelay.ShowPane(FALSE, FALSE, FALSE);
	else
		m_wndDkInputRelay.ShowPane(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateChkViewdkinput(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->SetCheck(m_wndDkInputRelay.IsVisible());
}


void CMainFrame::OnChkViewdkemergency()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_wndDkEmergency.IsVisible())
		m_wndDkEmergency.ShowPane(FALSE, FALSE, FALSE);
	else
		m_wndDkEmergency.ShowPane(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateChkViewdkemergency(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->SetCheck(m_wndDkEmergency.IsVisible());
}


void CMainFrame::OnChkViewdkpswitch()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#if _USE_PSWITCH_
	if (m_wndDkPSwitch.IsVisible())
		m_wndDkPSwitch.ShowPane(FALSE, FALSE, FALSE);
	else
		m_wndDkPSwitch.ShowPane(TRUE, FALSE, TRUE);
#endif
}


void CMainFrame::OnUpdateChkViewdkpswitch(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
#if _USE_PSWITCH_
	if (pCmdUI)
		pCmdUI->SetCheck(m_wndDkPSwitch.IsVisible());
#endif
}


void CMainFrame::OnChkViewdkpump()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_wndDkPump.IsVisible())
		m_wndDkPump.ShowPane(FALSE, FALSE, FALSE);
	else
		m_wndDkPump.ShowPane(TRUE, FALSE, TRUE);
}

void CMainFrame::OnUpdateChkViewdkpump(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->SetCheck(m_wndDkPump.IsVisible());
}

void CMainFrame::OnChkViewdkContact()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_wndDkContact.IsVisible())
		m_wndDkContact.ShowPane(FALSE, FALSE, FALSE);
	else
		m_wndDkContact.ShowPane(TRUE, FALSE, TRUE);
}

void CMainFrame::OnUpdateChkViewdkContact(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->SetCheck(m_wndDkContact.IsVisible());
}

#if _USE_OUTPUTWND_
void CMainFrame::OnChkViewdkLog()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_wndOutput.IsVisible())
		m_wndOutput.ShowPane(FALSE, FALSE, FALSE);
	else
		m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

void CMainFrame::OnUpdateChkViewdkLog(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (pCmdUI)
		pCmdUI->SetCheck(m_wndOutput.IsVisible());
}
#endif

LRESULT CMainFrame::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	if (wp == FORM_PRJ_CLOSE)
	{
		m_wndDkInputRelay.RemoveAllTreeData();
		m_wndDkOutputRelay.RemoveAllTreeData();
		m_wndDkPattern.RemoveAllTreeData();
		m_wndDkEmergency.RemoveAllTreeData();
		m_wndDkPSwitch.RemoveAllTreeData();
		m_wndDkPump.RemoveAllTreeData();
		m_wndDkContact.RemoveAllTreeData();
	}
	else if (wp == FORM_PRJ_REFESH)
	{
		m_wndDkInputRelay.InitTree();
		m_wndDkOutputRelay.InitTree();
		m_wndDkPattern.InitTree();
		m_wndDkEmergency.InitTree();
		m_wndDkPSwitch.InitTree();
		m_wndDkPump.InitTree();
		m_wndDkContact.InitTree();
	}
	else
	{

	}
	return 0;
}

LRESULT CMainFrame::OnAfxWmChangingActiveTab(WPARAM wParam, LPARAM lParam)
{
	CMFCTabCtrl* pTab = (CMFCTabCtrl*)lParam;

	int iTabsCount = pTab->GetTabsNum();
// 
// 	// removed?
// 	if (m_iCurrentTabCount <= iTabsCount)
// 	{
// 		m_iCurrentTabCount = iTabsCount;
// 		return 0;
// 	}
// 
// 	m_iCurrentTabCount = iTabsCount;

 	CWnd *pWnd = pTab->GetActiveWnd();
 	if (NULL != pWnd)
 	{
		CString str;
// 		pWnd->GetWindowTextW(str);
// 		SetWindowText(L"프로젝트명 - " + str);
// 		//AfxMessageBox(str);
 		// determine what tab you want to activate
 		// I used tab #1 just for demonstration purpose
 		//pTab->SetActiveTab(1);
 
 		// do not allow any further tab activation
 		//return 1;
 	}

	return 0;
}


void CMainFrame::OnFacpCreateLink()
{
	//20240808 GBM start - 현재 수신기/유닛 타입 정보를 가져옴
	memset(CNewInfo::Instance()->m_gi.facpType, NULL, MAX_FACP_COUNT);
	memset(CNewInfo::Instance()->m_gi.unitType, NULL, MAX_FACP_COUNT * MAX_UNIT_COUNT);
	m_pRefFasSysData->GetFacpAndUnitType();
	//20240808 GBM end

	//20240618 GBM start - GT1 프로젝트가 아닌 경우 관리자 모드 여부를 묻지 않음
	BOOL bGT1TypeExist = FALSE;
	for (int i = 0; i < MAX_FACP_COUNT; i++)
	{
		if (CNewInfo::Instance()->m_gi.facpType[i] == GT1)
		{
			bGT1TypeExist = TRUE;
			break;
		}
	}

	if (bGT1TypeExist)
	{
		//20240415 GBM start - 연동데이터 생성 시작 시 [관리자 모드]로 실행할 지 여부를 판단

		//인증 여부 초기화
		CNewInfo::Instance()->m_gi.projectInfo.authorized = false;
#ifndef ENGLISH_MODE
		if (AfxMessageBox(_T("관리자 모드(ROM 인증 모드)로 진행하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			CDlgAdminMode dlg;
			if (dlg.DoModal() == IDOK)
			{
				CString strPassword = ADMIN_MODE_PASSWORD;
				CString strUserInputPassword = _T("");
				strUserInputPassword = dlg.m_strEditPassword;

				if (strUserInputPassword.Compare(strPassword) == 0)
				{
					AfxMessageBox(_T("관리자 모드가 인증되었습니다. 인증 ROM 파일 생성을 진행합니다."));
					GF_AddLog(L"관리자 모드가 인증되었습니다. 인증 ROM 파일 생성을 진행합니다.");
					Log::Trace("Administrator mode is authorized. Proceed to create a authorized ROM file.");
					CNewInfo::Instance()->m_gi.projectInfo.authorized = true;
				}
				else
				{
					AfxMessageBox(_T("잘못된 암호입니다."));
					return;
				}
			}
			else
			{
				GF_AddLog(L"일반 ROM 파일 생성을 진행합니다.");
				Log::Trace("Proceed with creating a unauthorized ROM file.");
			}
		}
		else
		{
			GF_AddLog(L"일반 ROM 파일 생성을 진행합니다.");
			Log::Trace("Proceed with creating a unauthorized ROM file.");
		}
#else
		if (AfxMessageBox(_T("Do you want to proceed to the administrator mode (ROM authentication mode)?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			CDlgAdminMode dlg;
			if (dlg.DoModal() == IDOK)
			{
				CString strPassword = ADMIN_MODE_PASSWORD;
				CString strUserInputPassword = _T("");
				strUserInputPassword = dlg.m_strEditPassword;

				if (strUserInputPassword.Compare(strPassword) == 0)
				{
					AfxMessageBox(_T("Admin mode verified. Proceeds to generate the ROM file for authentication."));
					GF_AddLog(L"Admin mode verified. Proceeds to generate the ROM file for authentication.");
					Log::Trace("Administrator mode is authorized. Proceed to create a authorized ROM file.");
					CNewInfo::Instance()->m_gi.projectInfo.authorized = true;
				}
				else
				{
					AfxMessageBox(_T("Invalid password."));
					return;
				}
			}
			else
			{
				GF_AddLog(L"Proceeds to create a generic ROM file.");
				Log::Trace("Proceed with creating a unauthorized ROM file.");
			}
		}
		else
		{
			GF_AddLog(L"Proceeds to create a generic ROM file.");
			Log::Trace("Proceed with creating a unauthorized ROM file.");
		}
#endif

		//20240415 GBM end
	}
	//20240618 GBM end

	// TODO: 여기에 명령 처리기 코드를 추가합니다.
#ifndef ENGLISH_MODE
	if(AfxMessageBox(L"연동데이터를 현재 상태로 컴파일합니다.\nYes : 오류검사 후 컴파일\nNo:오류검사 없이 컴파일",MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		CreateFacpLink();
	}
	else
		StartErrorCheck(ERR_CHECK_CREATELINK,this);
#else
	if (AfxMessageBox(L"Compiles the site logic data to the current state.\nYes: Compile with error check\nNo : Compile without error check", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		CreateFacpLink();
	}
	else
		StartErrorCheck(ERR_CHECK_CREATELINK, this);
#endif
}

void CMainFrame::OnUpdateFacpCreateLink(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(pCmdUI)
	{
		if(m_pRefFasSysData != nullptr && m_pRefFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

int CMainFrame::CreateFacpLink()
{
	int nRet = 0;
	CString strPath;
	//CRelayTableData * pRefTable = m_pRefFasSysData;
	if(m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트가 닫힌 상태 입니다. 프로젝트를 여시고 다시 시도하여주십시오.");
		AfxMessageBox(L"프로젝트가 닫힌 상태 입니다. 프로젝트를 여시고 다시 시도하여주십시오.");
#else
		GF_AddLog(L"The project is closed. Please open the project and try again.");
		AfxMessageBox(L"The project is closed. Please open the project and try again.");
#endif
		return 0;
	}
	CString strPrjPath,strFullPath,strDBPath;

	strPrjPath = g_stConfig.szPrjPath;
	if(strPrjPath.Right(1) != '\\')
		strPrjPath += "\\";

	strFullPath = strPrjPath + m_pRefFasSysData->GetPrjName();
	strPath.Format(L"%s\\%s\\Release"
		,strFullPath,F3_VERSIONTEMPFOLDER_NAME
	);
	GF_CreateFolder(strPath);
	nRet = m_pRefFasSysData->MakeLinkData(strPath);
	if(nRet > 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트를 컴파일하는데 성공했습니다.(연동데이터 생성 성공)");
#else
		GF_AddLog(L"Successfully compiled the project (site logic data generation succeeded)");
#endif
		//	AfxMessageBox(L"프로젝트를 컴파일 하는데 성공했습니다.");

		//20240703 GBM start - 중계기 일람표 편집 기능 Disable
#ifdef MODULE_TABLE_UPDATE_MODE

		//20240305 GBM start - 연동데이터 생성이 성공한 시점에 프로젝트 번호 업데이트 및 중계기 일람표 적용

		//수신기 타입 정보에서 GT1 타입이 하나도 없으면 아래 행정을 하지 않음 (F3만 있으면 의미가 없기 때문)
		BOOL bGT1TypeExist = FALSE;
		for (int i = 0; i < MAX_FACP_COUNT; i++)
		{
			if (CNewInfo::Instance()->m_gi.facpType[i] == GT1)
			{
				bGT1TypeExist = TRUE;
				break;
			}
		}

		if (bGT1TypeExist)
		{
			int nModuleTableVerNum = -1;
			int nLinkedDataVerNum = -1;
			bool bAuthorized = CNewInfo::Instance()->m_gi.projectInfo.authorized;
			CString strAuthorized = _T("");
			if (bAuthorized)
			{
				strAuthorized = _T("A");
			}
			nModuleTableVerNum = CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum;
			nLinkedDataVerNum = CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum;
			BOOL bRet = FALSE;
			CString strMsg = _T("");

			//프로젝트 폴더명
			CString strWin32AppProjectName = _T("");
			strWin32AppProjectName = theApp.m_pFasSysData->m_strPrjName;

			bRet = CNewExcelManager::Instance()->UpdateProjectInfo(strWin32AppProjectName);
			if (bRet)
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"중계기 일람표 파일에 프로젝트 번호를 업데이트하는 데에 성공했습니다.[프로젝트 버전: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
#else
				GF_AddLog(L"Successfully updated the project number in the module table file [project version: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
#endif
				strMsg.Format(_T("Successfully updated project number in Module Table file.[Project Ver: %02d.%02d%s]"), nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
			}
			else
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"중계기 일람표 파일에 프로젝트 번호를 업데이트하는 데에 실패했습니다.[프로젝트 버전: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
#else
				GF_AddLog(L"Failed to update the project number in the module table file. [Project Version: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
#endif
				strMsg.Format(_T("Failed to update project number in Module Table file.[Project Ver: %02d.%02d%s]"), nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
			}
			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));

			//프로젝트 버전 DB 적용
			bRet = CNewDBManager::Instance()->InsertDataIntoProjectInfoTable();
			if (bRet)
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"데이터베이스에 프로젝트 번호를 업데이트하는 데에 성공했습니다.[프로젝트 버전: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
#else
				GF_AddLog(L"Successfully updated the project number in the database [Project version: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
#endif
				strMsg.Format(_T("Successfully updated project number in the database.[Project Ver: %02d.%02d%s]"), nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
			}
			else
			{
#ifndef ENGLISH_MODE
				GF_AddLog(L"데이터베이스에 프로젝트 번호를 업데이트하는 데에 실패했습니다.[프로젝트 버전: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
#else
				GF_AddLog(L"Failed to update the project number in the database. [Project Version: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
#endif
				strMsg.Format(_T("Failed to update project number in the database.[Project Ver: %02d.%02d%s]"), nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
			}
			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
		}
		else
		{
			Log::Trace("There is no GT1 type among the FACPs, so project version number will not be updated!");
		}
		//20240305 GBM end

#else	//중계기 일람표를 갱신하지 않더라도(MODULE_TABLE_UPDATE_MODE undefined) 프로젝트 정보 DB는 반드시 갱신해야 함

		//20240730 GBM start - 프로젝트 DB 대신 기존의 프로젝트 파일의 버전 정보를 사용 아래 주석처리
		//20240305 GBM start - 연동데이터 생성이 성공한 시점에 프로젝트 번호 업데이트 및 중계기 일람표 적용
		//수신기 타입 정보에서 GT1 타입이 하나도 없으면 아래 행정을 하지 않음 (F3만 있으면 의미가 없기 때문)
// 		BOOL bGT1TypeExist = FALSE;
// 		for (int i = 0; i < MAX_FACP_COUNT; i++)
// 		{
// 			if (CNewInfo::Instance()->m_gi.facpType[i] == GT1)
// 			{
// 				bGT1TypeExist = TRUE;
// 				break;
// 			}
// 		}
// 
// 		if (bGT1TypeExist)
// 		{
// 			int nModuleTableVerNum = -1;
// 			int nLinkedDataVerNum = -1;
// 			bool bAuthorized = CNewInfo::Instance()->m_gi.projectInfo.authorized;
// 			CString strAuthorized = _T("");
// 			if (bAuthorized)
// 			{
// 				strAuthorized = _T("A");
// 			}
// 			nModuleTableVerNum = CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum;
// 			nLinkedDataVerNum = CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum;
// 			BOOL bRet = FALSE;
// 			CString strMsg = _T("");
// 
// 			//프로젝트 버전 DB 적용
// 			bRet = CNewDBManager::Instance()->InsertDataIntoProjectInfoTable();
// 			if (bRet)
// 			{
// #ifndef ENGLISH_MODE
// 				GF_AddLog(L"데이터베이스에 프로젝트 번호를 업데이트하는 데에 성공했습니다.[프로젝트 버전: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
// #else
// 				GF_AddLog(L"Successfully updated the project number in the database [Project version: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
// #endif
// 				strMsg.Format(_T("Successfully updated project number in the database.[Project Ver: %02d.%02d%s]"), nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
// 			}
// 			else
// 			{
// #ifndef ENGLISH_MODE
// 				GF_AddLog(L"데이터베이스에 프로젝트 번호를 업데이트하는 데에 실패했습니다.[프로젝트 버전: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
// #else
// 				GF_AddLog(L"Failed to update the project number in the database. [Project Version: %02d.%02d%s]", nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
// #endif
// 				strMsg.Format(_T("Failed to update project number in the database.[Project Ver: %02d.%02d%s]"), nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);
// 			}
// 			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
// 		}
		//20240730 GBM end
#endif
		//20240703 GBM end
	}
	else
	{
		if(strPath.Right(1) != '\\')
			strPath += L"\\";
		strPath += L"*.*";
		//GF_DeleteDir(strPath);
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트를 컴파일하는데 실패했습니다.(연동데이터 생성 실패)");
		AfxMessageBox(L"프로젝트를 컴파일하는데 실패했습니다.",MB_OK | MB_ICONSTOP);
#else
		GF_AddLog(L"Failed to compile the project. (Failed to generate Data)");
		AfxMessageBox(L"Failed to compile the project.", MB_OK | MB_ICONSTOP);
#endif
		return 0;
		//AfxMessageBox(L"프로젝트를 컴파일 하는데 실패했습니다.");
	}
	nRet = m_pRefFasSysData->MakeConstructorTable(strPath);
	if(nRet > 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트를 컴파일하는데 성공했습니다.(연동출력표 생성 성공)");
		AfxMessageBox(L"프로젝트를 컴파일하는데 성공했습니다.",MB_OK | MB_ICONINFORMATION);
#else
		GF_AddLog(L"Successfully compiled the project (interlock output table generation succeeded)");
		AfxMessageBox(L"Successfully compiled the project.", MB_OK | MB_ICONINFORMATION);
#endif
		return 1;
	}
	else
	{
		if(strPath.Right(1) != '\\')
			strPath += L"\\";
		strPath += L"*.*";
		//GF_DeleteDir(strPath);
#ifndef ENGLISH_MODE
		GF_AddLog(L"프로젝트를 컴파일하는데 실패했습니다.(연동데이터 생성 실패)");
		AfxMessageBox(L"프로젝트를 컴파일하는데 실패했습니다.", MB_OK | MB_ICONSTOP);
#else
		GF_AddLog(L"Failed to compile the project. (Failed to generate Data)");
		AfxMessageBox(L"Failed to compile the project.", MB_OK | MB_ICONSTOP);
#endif
		return 0; 
	}
}

void CMainFrame::StartErrorCheck(int nCheckType , CWnd * pTargetWnd)
{
	if(m_pDlgErrorCheck == nullptr)
	{
		m_pDlgErrorCheck = new CDlgErrorCheck(nCheckType,m_pRefFasSysData);
		m_pDlgErrorCheck->CreateChkDlg(this);
	}
	m_pDlgErrorCheck->ShowWindow(SW_SHOW);
	m_pDlgErrorCheck->StartErrorCheck(nCheckType,pTargetWnd);

}

LRESULT CMainFrame::OnErrorCheckAfterCreateLink(WPARAM wp,LPARAM lp)
{
	CreateFacpLink();
	return 0;
}
LRESULT CMainFrame::OnErrorCheckEnd(WPARAM wp,LPARAM lp)
{
	switch(wp)
	{
	case ERR_CHECK_SIMPLE:
		break;
	case ERR_CHECK_CREATELINK:
#ifndef ENGLISH_MODE
 		if(lp == 0)
			PostMessage(UWM_ERRORCHECK_CREATELINK,wp,lp);
 		else if(lp == -1)
 			AfxMessageBox(L"사용자가 취소했습니다.");
 // 		else if(lp == -2)
 // 			AfxMessageBox(L"오류가 발생하여 컴파일 할 수 없습니다.");
 		else
 			AfxMessageBox(L"오류가 발생하여 컴파일 할 수 없습니다.");
#else
		if (lp == 0)
			PostMessage(UWM_ERRORCHECK_CREATELINK, wp, lp);
		else if (lp == -1)
			AfxMessageBox(L"User canceled.");
		// 		else if(lp == -2)
		// 			AfxMessageBox(L"오류가 발생하여 컴파일 할 수 없습니다.");
		else
			AfxMessageBox(L"Could not compile due to error.");
#endif

		break;
	case ERR_CHECK_MAKEAUTOLINK:
		break;
	}
	return 0;
}
// 
// LRESULT CMainFrame::OnErrorCheckClose(WPARAM wp,LPARAM lp)
// {
// 	if(m_pDlgErrorCheck)
// 	{
// 		delete m_pDlgErrorCheck;
// 		m_pDlgErrorCheck = nullptr;
// 	}
// 	return 0;
// }


void CMainFrame::OnSimpleErrorCheck()
{
	StartErrorCheck(ERR_CHECK_SIMPLE,this);
}

void CMainFrame::OnUpdateSimpleErrorCheck(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if(pCmdUI)
	{
		if(m_pRefFasSysData != nullptr && m_pRefFasSysData->GetProjectOpened())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}
