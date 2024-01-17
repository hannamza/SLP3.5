// FormContactFacp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormContactFacp.h"


// CFormContactFacp

IMPLEMENT_DYNCREATE(CFormContactFacp, CFormView)

CFormContactFacp::CFormContactFacp()
	: CFormView(IDD_FORMCONTACTFACP)
{

}

CFormContactFacp::~CFormContactFacp()
{
}

void CFormContactFacp::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormContactFacp, CFormView)
END_MESSAGE_MAP()


// CFormContactFacp 진단입니다.

#ifdef _DEBUG
void CFormContactFacp::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormContactFacp::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormContactFacp 메시지 처리기입니다.
