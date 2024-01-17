// FormUser.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormUser.h"


// CFormUser

IMPLEMENT_DYNCREATE(CFormUser, CFormView)

CFormUser::CFormUser()
	: CFormView(IDD_FORMUSER)
{

}

CFormUser::~CFormUser()
{
}

void CFormUser::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormUser, CFormView)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CFormUser 진단입니다.

#ifdef _DEBUG
void CFormUser::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormUser::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormUser 메시지 처리기입니다.


void CFormUser::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


BOOL CFormUser::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::PreCreateWindow(cs);
}


void CFormUser::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnClose();
}


int CFormUser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CFormUser::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

LRESULT CFormUser::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
// 	if (m_ctrlRelayList.GetSafeHwnd())
// 		m_ctrlRelayList.DeleteAllItems();
// 	if (m_ctrlPatternList)
// 		m_ctrlPatternList.DeleteAllItems();
	return 0;
}


void CFormUser::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
