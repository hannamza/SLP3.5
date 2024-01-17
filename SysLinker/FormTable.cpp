// FormTable.cpp : 구현 파일입니다.
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


// CFormTable 진단입니다.

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


// CFormTable 메시지 처리기입니다.
