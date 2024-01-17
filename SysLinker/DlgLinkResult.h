#pragma once

#include	"../Common/Control/TreeListCtrl.h"

// CDlgLinkResult ��ȭ �����Դϴ�.
class CRelayTableData;
class CDlgLinkResult : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLinkResult)

public:
	CDlgLinkResult(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgLinkResult();

	CTreeListCtrl	m_ctrlTreeList;
	CImageList		m_ImgDeviceIcons;
	CImageList		m_ImgChecks;
	CRelayTableData		*		m_pRefFasSysData; /// �߰�� �϶�ǥ�� ��絥����
	CPtrList			*		m_pRefPtrSelList;

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LINKRESULT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int CreateDlg(CRelayTableData* pFasSysData, CPtrList* pPtrSelList);
	int FillSelectCondTreeList();
	static int CALLBACK DrawItem(HDC hDc, LPVOID	pParam, TV_DRAWITEM *pData);
};
