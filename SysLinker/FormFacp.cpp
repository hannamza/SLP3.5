// FormFacp.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormFacp.h"


// CFormFacp

IMPLEMENT_DYNCREATE(CFormFacp, CFormView)

#ifndef ENGLISH_MODE
CFormFacp::CFormFacp()
	: CFormView(IDD_FORMFACP)
{

}
#else
CFormFacp::CFormFacp()
	: CFormView(IDD_FORMFACP_EN)
{

}
#endif

CFormFacp::~CFormFacp()
{
}

void CFormFacp::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormFacp, CFormView)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// CFormFacp �����Դϴ�.

#ifdef _DEBUG
void CFormFacp::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormFacp::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormFacp �޽��� ó�����Դϴ�.

LRESULT CFormFacp::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
// 	if (m_ctrlRelayList.GetSafeHwnd())
// 		m_ctrlRelayList.DeleteAllItems();
// 	if (m_ctrlPatternList)
// 		m_ctrlPatternList.DeleteAllItems();
	return 0;
}


void CFormFacp::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
