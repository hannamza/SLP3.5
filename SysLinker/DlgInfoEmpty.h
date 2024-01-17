#pragma once

#include "DlgInfoBasePage.h"

// CDlgInfoEmpty 대화 상자입니다.

class CDlgInfoEmpty : public CDlgInfoBasePage
{
	DECLARE_DYNAMIC(CDlgInfoEmpty)

public:
	CDlgInfoEmpty(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInfoEmpty();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INFO_EDIT_EMPTY };
#endif
public:
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_INFO_EDIT_EMPTY, pParent); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void ChangeDataControlUpdate() {};

	DECLARE_MESSAGE_MAP()
};
