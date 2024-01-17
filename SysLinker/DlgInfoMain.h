#pragma once

#include "ScrollDialog.h"
// CDlgInfoMain 대화 상자입니다.

class CDlgInfoMain : public CScrollDialog
{
	DECLARE_DYNAMIC(CDlgInfoMain)

public:
	CDlgInfoMain(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInfoMain();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INFO_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
