#pragma once

#include "ScrollDialog.h"
// CDlgInfoMain ��ȭ �����Դϴ�.

class CDlgInfoMain : public CScrollDialog
{
	DECLARE_DYNAMIC(CDlgInfoMain)

public:
	CDlgInfoMain(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgInfoMain();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INFO_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
