#pragma once


// CDlgNewProjectWizard ��ȭ �����Դϴ�.

class CDlgNewProjectWizard : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNewProjectWizard)

public:
	CDlgNewProjectWizard(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgNewProjectWizard();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_NEW_PROJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	//CPropSheetNewProject	m_SheetNewProject;
};
