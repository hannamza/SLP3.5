#pragma once



// CFormContactFacp �� ���Դϴ�.

class CFormContactFacp : public CFormView
{
	DECLARE_DYNCREATE(CFormContactFacp)

protected:
	CFormContactFacp();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormContactFacp();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMCONTACTFACP };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};


