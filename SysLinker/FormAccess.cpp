// FormAccess.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormAccess.h"


// CFormAccess

IMPLEMENT_DYNCREATE(CFormAccess, CFormView)

#ifndef ENGLISH_MODE
CFormAccess::CFormAccess()
	: CFormView(IDD_FORMACCESS)
{

}
#else
CFormAccess::CFormAccess()
	: CFormView(IDD_FORMACCESS_EN)
{

}
#endif

CFormAccess::~CFormAccess()
{
}

void CFormAccess::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormAccess, CFormView)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CFormAccess �����Դϴ�.

#ifdef _DEBUG
void CFormAccess::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormAccess::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormAccess �޽��� ó�����Դϴ�.


void CFormAccess::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}


BOOL CFormAccess::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CFormView::PreCreateWindow(cs);
}


int CFormAccess::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}


void CFormAccess::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CFormView::OnClose();
}


void CFormAccess::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

LRESULT CFormAccess::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	return 0;
}

void CFormAccess::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
