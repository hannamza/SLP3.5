// FormUnit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormUnit.h"


// CFormUnit

IMPLEMENT_DYNCREATE(CFormUnit, CFormView)

CFormUnit::CFormUnit()
	: CFormView(IDD_FORMUNIT)
{

}

CFormUnit::~CFormUnit()
{
}

void CFormUnit::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormUnit, CFormView)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CFormUnit �����Դϴ�.

#ifdef _DEBUG
void CFormUnit::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormUnit::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormUnit �޽��� ó�����Դϴ�.


LRESULT CFormUnit::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
// 	if (m_ctrlRelayList.GetSafeHwnd())
// 		m_ctrlRelayList.DeleteAllItems();
// 	if (m_ctrlPatternList)
// 		m_ctrlPatternList.DeleteAllItems();
	return 0;
}


void CFormUnit::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
