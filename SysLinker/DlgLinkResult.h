#pragma once

#include	"../Common/Control/TreeListCtrl.h"

// CDlgLinkResult 대화 상자입니다.
class CRelayTableData;
class CDlgLinkResult : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLinkResult)

public:
	CDlgLinkResult(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgLinkResult();

	CTreeListCtrl	m_ctrlTreeList;
	CImageList		m_ImgDeviceIcons;
	CImageList		m_ImgChecks;
	CRelayTableData		*		m_pRefFasSysData; /// 중계기 일람표의 모든데이터
	CPtrList			*		m_pRefPtrSelList;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LINKRESULT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int CreateDlg(CRelayTableData* pFasSysData, CPtrList* pPtrSelList);
	int FillSelectCondTreeList();
	static int CALLBACK DrawItem(HDC hDc, LPVOID	pParam, TV_DRAWITEM *pData);
};
