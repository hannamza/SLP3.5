// FormTable.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormTable.h"


// CFormTable

IMPLEMENT_DYNCREATE(CFormTable, CFormView)

CFormTable::CFormTable()
	: CFormView(IDD_FORMTABLE)
{

}

CFormTable::~CFormTable()
{
}

void CFormTable::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFormTable, CFormView)
END_MESSAGE_MAP()


// CFormTable �����Դϴ�.

#ifdef _DEBUG
void CFormTable::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormTable::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormTable �޽��� ó�����Դϴ�.
