#pragma once

#include "DlgChildBasePage.h"

// CDlgChildEditEmpty 대화 상자입니다.
class CDlgChildEditMain;
class CRelayTableData;
class CDlgChildEditEmpty : public CDlgChildBasePage
{
	DECLARE_DYNAMIC(CDlgChildEditEmpty)

public:
	CDlgChildEditEmpty(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgChildEditEmpty();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CHILD_EDIT_EMPTY };
#endif
public:
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_EMPTY, pParent); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
