// FormAutoLogic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormAutoLogic.h"


// CFormAutoLogic

IMPLEMENT_DYNCREATE(CFormAutoLogic, CFormView)

CFormAutoLogic::CFormAutoLogic()
	: CFormView(IDD_FORMAUTOLOGIC)
{

}

CFormAutoLogic::~CFormAutoLogic()
{
}

void CFormAutoLogic::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormAutoLogic, CFormView)
END_MESSAGE_MAP()


// CFormAutoLogic 진단입니다.

#ifdef _DEBUG
void CFormAutoLogic::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormAutoLogic::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormAutoLogic 메시지 처리기입니다.
