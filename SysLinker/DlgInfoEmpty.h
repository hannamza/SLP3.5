#pragma once

#include "DlgInfoBasePage.h"

// CDlgInfoEmpty ��ȭ �����Դϴ�.

class CDlgInfoEmpty : public CDlgInfoBasePage
{
	DECLARE_DYNAMIC(CDlgInfoEmpty)

public:
	CDlgInfoEmpty(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgInfoEmpty();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INFO_EDIT_EMPTY };
#endif
public:
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_INFO_EDIT_EMPTY, pParent); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void ChangeDataControlUpdate() {};

	DECLARE_MESSAGE_MAP()
};
