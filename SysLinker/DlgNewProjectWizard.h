#pragma once


// CDlgNewProjectWizard 대화 상자입니다.

class CDlgNewProjectWizard : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNewProjectWizard)

public:
	CDlgNewProjectWizard(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNewProjectWizard();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_NEW_PROJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	//CPropSheetNewProject	m_SheetNewProject;
};
