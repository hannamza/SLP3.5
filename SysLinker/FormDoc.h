#pragma once

// CFormDoc �����Դϴ�.

class CFormDoc : public CDocument
{
	DECLARE_DYNCREATE(CFormDoc)

public:
	CFormDoc();
	virtual ~CFormDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // ���� ��/����� ���� �����ǵǾ����ϴ�.
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
