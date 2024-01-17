// FormPSwitchEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormPSwitchEdit.h"

#include "DlgChildPSwitch.h"
#include "DataPS.h"
#include "DataFacp.h"
#include "ManagerPS.h"
#include "RelayTableData.h"
#include "DlgTreePane.h"
#include "DataLinked.h"
#include "../Common/Control/DTreeCtrl.h"
#include "DataPattern.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "DataDevice.h"
#include "DataLinked.h"
#include "DataEmBc.h"
#include "DataPS.h"
#include "DataPump.h"
// CFormPSwitchEdit

IMPLEMENT_DYNCREATE(CFormPSwitchEdit, CView)
void CFormPSwitchEdit::OnDraw(CDC* /*pDC*/)
{
	//CMDIDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
}
CFormPSwitchEdit::CFormPSwitchEdit()
	//: CView(IDD_FORMPSWITCHEDIT)
{
	m_pDlgLeftBottomPSwitchInfo = nullptr;
	m_pDlgLeftTopTreePane = nullptr;
	m_pCurrentData = nullptr;
	m_pChangeData = nullptr;
	m_crBack = ::GetSysColor(COLOR_3DFACE);
	m_brBackground.CreateSolidBrush(m_crBack);
}

CFormPSwitchEdit::~CFormPSwitchEdit()
{
	if (m_pChangeData)
	{
		delete m_pChangeData;
		m_pChangeData = nullptr;
	}
}

// void CFormPSwitchEdit::DoDataExchange(CDataExchange* pDX)
// {
// 	CView::DoDataExchange(pDX);
// 	DDX_Control(pDX, IDC_LIST_PATTERN, m_ctrlPatternList);
// 	DDX_Control(pDX, IDC_LIST_RELAY, m_ctrlRelayList);
// 	DDX_Control(pDX, IDC_TREE, m_ctrlTree);
// }

BEGIN_MESSAGE_MAP(CFormPSwitchEdit, CView)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CFormPSwitchEdit 진단입니다.

#ifdef _DEBUG
void CFormPSwitchEdit::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormPSwitchEdit::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormPSwitchEdit 메시지 처리기입니다.


void CFormPSwitchEdit::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pChangeData = new CDataPS;
// 	m_ctrlPatternList.InsertColumn(0, L"제어 접점", LVCFMT_LEFT, 150);
// 	m_ctrlPatternList.InsertColumn(1, L"접점 종류", LVCFMT_LEFT, 150);
// 	m_ctrlPatternList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
// 	
// 	m_ctrlRelayList.InsertColumn(0, L"출력이름", LVCFMT_LEFT, 300);
// 	m_ctrlRelayList.InsertColumn(1, L"입력타입", LVCFMT_LEFT, 80);
// 	m_ctrlRelayList.InsertColumn(2, L"출력타입", LVCFMT_LEFT, 80);
// 	m_ctrlRelayList.InsertColumn(3, L"설비명", LVCFMT_LEFT, 80);
// 	m_ctrlRelayList.InsertColumn(4, L"설비번호", LVCFMT_LEFT, 50);
// 	m_ctrlRelayList.InsertColumn(5, L"출력설명", LVCFMT_LEFT, 80);
// 	m_ctrlRelayList.InsertColumn(6, L"위치", LVCFMT_LEFT, 150);
// 	m_ctrlRelayList.InsertColumn(7, L"연동타입", LVCFMT_LEFT, 80);
// 	m_ctrlRelayList.InsertColumn(8, L"출력종류", LVCFMT_LEFT, 80);
// 
// 	// 	m_ctrlRightBottomRelayList.InsertColumn(0, L"감지기/중계기", LVCFMT_LEFT, 150);
// 	// 	m_ctrlRightBottomRelayList.InsertColumn(1, L"주소", LVCFMT_LEFT, 150);
// 	// 	m_ctrlRightBottomRelayList.InsertColumn(2, L"위치", LVCFMT_LEFT, 150);
// 	// 	m_ctrlRightBottomRelayList.InsertColumn(3, L"로직타입", LVCFMT_LEFT, 150);
// 	m_ctrlRelayList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
// 
// 	if (AfxGetMainWnd())
// 	{
// 		// WPARAM : ADD/DEL , LPARAM : WINDOW
// 		AfxGetMainWnd()->SendMessage(UWM_REG_PTNVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
// 		AfxGetMainWnd()->SendMessage(UWM_REG_EMVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
// 		AfxGetMainWnd()->SendMessage(UWM_REG_PUMPVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
// 		AfxGetMainWnd()->SendMessage(UWM_REG_PSWITCHVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlPatternList);
// 
// 		AfxGetMainWnd()->SendMessage(UWM_REG_OUTVIEW_DROPWND, DROP_WND_ADD, (LPARAM)&m_ctrlRelayList);
// 
// 		m_pRefFasSysData = theApp.GetRelayTableData();
// 	}

	
	if (m_pDlgLeftBottomPSwitchInfo)
	{
		//m_pDlgLeftBottomPSwitchInfo->SetMainWindow(this);
		m_pDlgLeftBottomPSwitchInfo->SetChangeData(m_pChangeData);
	}
	theApp.SetFormViewInitComplete(FV_PSWITCH);


	if (m_pDlgLeftTopTreePane == nullptr)
		m_pDlgLeftTopTreePane = new CDlgTreePane;

	if (!m_pDlgLeftTopTreePane->CreateDlg(1, this)) //1 : child pane - presure switch
		return;
	m_pDlgLeftTopTreePane->ShowWindow(SW_SHOW);

	if (m_pDlgLeftBottomPSwitchInfo == nullptr)
		m_pDlgLeftBottomPSwitchInfo = new CDlgChildPSwitch;

	if (!m_pDlgLeftBottomPSwitchInfo->CreateDlg(this))
		return;
	m_pDlgLeftBottomPSwitchInfo->ShowWindow(SW_SHOW);

// 	if (m_pDlgLeftTopTreePane)
// 		m_pDlgLeftTopTreePane->SetMainWindow(this);

// 	HTREEITEM hItem = m_ctrlTree.InsertItem(L"테스트 데이터 입력");
// 	m_ctrlTree.InsertItem(L"데이터입력" , hItem);
	m_SpLeft.Create(
		WS_CHILD | WS_VISIBLE |/*WS_BORDER|*/WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		m_pDlgLeftTopTreePane,		// top pane
		m_pDlgLeftBottomPSwitchInfo,		// bottom pane
		IDC_SP_LEFT,	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		CRect(0,0,100,100),				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

// 	m_SpRight.Create(
// 		WS_CHILD | WS_VISIBLE |/*WS_BORDER|*/WS_CLIPCHILDREN | SS_HORIZ,
// 		this,				// the parent of the splitter pane
// 		&m_ctrlPatternList,		// top pane
// 		&m_ctrlRelayList,		// bottom pane
// 		IDC_SP_RIGHT,	// this ID is used for saving/restoring splitter
// 							// position and therefore it must be unique 
// 							// within your application
// 		CRect(0, 0, 100, 100),				// dimensions of the splitter pane
// 		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
// 		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
// 	);
// 
// 	m_SpMain.Create(
// 		WS_CHILD | WS_VISIBLE |/*WS_BORDER|*/WS_CLIPCHILDREN | SS_VERT,
// 		this,				// the parent of the splitter pane
// 		&m_SpLeft,		// top pane
// 		&m_SpRight,		// bottom pane
// 		IDC_SP_MAIN,	// this ID is used for saving/restoring splitter
// 							// position and therefore it must be unique 
// 							// within your application
// 		CRect(0, 0, 100, 100),				// dimensions of the splitter pane
// 		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
// 		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
// 	);

	SetSplitterPos();


}


void CFormPSwitchEdit::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetSplitterPos();
}


void CFormPSwitchEdit::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnClose();
}

int CFormPSwitchEdit::SetSplitterPos()
{
	if (m_SpLeft.GetSafeHwnd() == nullptr)
		return 0;

	CRect rect;
	GetClientRect(&rect);
	rect.DeflateRect(20, 20);
	m_SpLeft.MoveWindow(&rect);

	return 1;
}

BOOL CFormPSwitchEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//return CView::OnEraseBkgnd(pDC);
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, &m_brBackground);
	pDC->SetBkColor(m_crBack);
	pDC->SetBkMode(TRANSPARENT);
	return TRUE; 
}
