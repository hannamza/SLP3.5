// FormDoc.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormDoc.h"


// CFormDoc

IMPLEMENT_DYNCREATE(CFormDoc, CDocument)

CFormDoc::CFormDoc()
{
}

BOOL CFormDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CFormDoc::~CFormDoc()
{
}


BEGIN_MESSAGE_MAP(CFormDoc, CDocument)
END_MESSAGE_MAP()


// CFormDoc �����Դϴ�.

#ifdef _DEBUG
void CFormDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CFormDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CFormDoc serialization�Դϴ�.

void CFormDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}
#endif


// CFormDoc ����Դϴ�.
