// FormAutoLogic.cpp : ���� �����Դϴ�.
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


// CFormAutoLogic �����Դϴ�.

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


// CFormAutoLogic �޽��� ó�����Դϴ�.
