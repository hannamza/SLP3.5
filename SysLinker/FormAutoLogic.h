#pragma once



// CFormAutoLogic �� ���Դϴ�.

class CFormAutoLogic : public CFormView
{
	DECLARE_DYNCREATE(CFormAutoLogic)

protected:
	CFormAutoLogic();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormAutoLogic();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMAUTOLOGIC };
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


