#pragma once



// CFormTable �� ���Դϴ�.

class CFormTable : public CFormView
{
	DECLARE_DYNCREATE(CFormTable)

protected:
	CFormTable();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormTable();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMTABLE };
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


