#pragma once



// CFormErrorCheck �� ���Դϴ�.

class CFormErrorCheck : public CFormView
{
	DECLARE_DYNCREATE(CFormErrorCheck)

protected:
	CFormErrorCheck();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormErrorCheck();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMERRORCHECK };
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


