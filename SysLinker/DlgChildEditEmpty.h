#pragma once

#include "DlgChildBasePage.h"

// CDlgChildEditEmpty ��ȭ �����Դϴ�.
class CDlgChildEditMain;
class CRelayTableData;
class CDlgChildEditEmpty : public CDlgChildBasePage
{
	DECLARE_DYNAMIC(CDlgChildEditEmpty)

public:
	CDlgChildEditEmpty(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgChildEditEmpty();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CHILD_EDIT_EMPTY };
#endif
public:
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_EMPTY, pParent); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
