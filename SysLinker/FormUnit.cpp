// FormUnit.cpp : 구현 파일입니다.
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


// CFormUnit 진단입니다.

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


// CFormUnit 메시지 처리기입니다.


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
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
