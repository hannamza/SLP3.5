#pragma once



// CFormPumpEdit �� ���Դϴ�.

class CFormPumpEdit : public CFormView
{
	DECLARE_DYNCREATE(CFormPumpEdit)

protected:
	CFormPumpEdit();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormPumpEdit();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMPUMPEDIT };
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


