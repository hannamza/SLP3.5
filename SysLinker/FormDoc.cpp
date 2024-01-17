// FormDoc.cpp : 구현 파일입니다.
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


// CFormDoc 진단입니다.

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
// CFormDoc serialization입니다.

void CFormDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}
#endif


// CFormDoc 명령입니다.
