// FormErrorCheck.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "resource.h"
#include "FormErrorCheck.h"


// CFormErrorCheck

IMPLEMENT_DYNCREATE(CFormErrorCheck, CFormView)

CFormErrorCheck::CFormErrorCheck()
	: CFormView(IDD_FORMERRORCHECK)
{

}

CFormErrorCheck::~CFormErrorCheck()
{
}

void CFormErrorCheck::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormErrorCheck, CFormView)
END_MESSAGE_MAP()


// CFormErrorCheck �����Դϴ�.

#ifdef _DEBUG
void CFormErrorCheck::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormErrorCheck::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormErrorCheck �޽��� ó�����Դϴ�.
