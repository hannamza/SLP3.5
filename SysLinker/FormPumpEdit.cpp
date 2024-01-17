// FormPumpEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormPumpEdit.h"


// CFormPumpEdit

IMPLEMENT_DYNCREATE(CFormPumpEdit, CFormView)

CFormPumpEdit::CFormPumpEdit()
	: CFormView(IDD_FORMPUMPEDIT)
{

}

CFormPumpEdit::~CFormPumpEdit()
{
}

void CFormPumpEdit::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormPumpEdit, CFormView)
END_MESSAGE_MAP()


// CFormPumpEdit 진단입니다.

#ifdef _DEBUG
void CFormPumpEdit::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormPumpEdit::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormPumpEdit 메시지 처리기입니다.
