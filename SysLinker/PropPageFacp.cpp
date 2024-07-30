// PropPageFacp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPageFacp.h"
#include "afxdialogex.h"
#include "RelayTableData.h"
#define USER_DATA_SIZE	256
// CPropPageFacp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropPageFacp, CPropertyPage)

#ifndef ENGLISH_MODE
CPropPageFacp::CPropPageFacp()
	: CPropertyPage(IDD_PROP_PAGE_FACP)
{
	m_pRefFasSysData = nullptr;

}
#else
CPropPageFacp::CPropPageFacp()
	: CPropertyPage(IDD_PROP_PAGE_FACP_EN)
{
	m_pRefFasSysData = nullptr;

}
#endif

CPropPageFacp::~CPropPageFacp()
{
}

void CPropPageFacp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropPageFacp, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CPropPageFacp::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropPageFacp::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPropPageFacp 메시지 처리기입니다.


void CPropPageFacp::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPropPageFacp::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CPropPageFacp::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CRect		rcFrame;
	CPoint		pt(0, 0);
	GetDlgItem(IDC_ST_FRAME)->GetWindowRect(&rcFrame);
	ClientToScreen(&pt);

	rcFrame.left -= pt.x;
	rcFrame.right -= pt.x;
	rcFrame.top -= pt.y;
	rcFrame.bottom -= pt.y;

	//m_hSelect = NULL;
	//m_iSelCol = 0;

	m_ImgDeviceIcons.Create(IDB_BMP_DEVICE_ICON, 16, 8, RGB(0, 255, 255));
	m_ImgChecks.Create(IDB_BMP_CHECKS, 16, 8, RGB(0, 255, 255));
	m_ctrlTreeList.CreateEx(
		WS_EX_CLIENTEDGE
		, TVS_HASBUTTONS | TVS_HASLINES | TVS_FULLROWSELECT | TVS_TRACKSELECT | TVS_EDITLABELS | WS_CHILD | WS_VISIBLE | WS_TABSTOP
		, rcFrame
		, this
		, IDC_TREELIST
	);

	m_ctrlTreeList.SetImageList(&m_ImgDeviceIcons, TVSIL_NORMAL);
#ifndef ENGLISH_MODE
	m_ctrlTreeList.InsertColumn(0, _T("수신기") , LVCFMT_LEFT , 300);
	m_ctrlTreeList.InsertColumn(1, _T("입력타입"), LVCFMT_LEFT, 80);
	m_ctrlTreeList.InsertColumn(2, _T("출력타입"), LVCFMT_LEFT, 80);
	m_ctrlTreeList.InsertColumn(3, _T("설비명"), LVCFMT_LEFT, 80);
	m_ctrlTreeList.InsertColumn(4, _T("번호"), LVCFMT_LEFT, 50);
	m_ctrlTreeList.InsertColumn(5, _T("출력내용"), LVCFMT_LEFT, 80);
	m_ctrlTreeList.InsertColumn(6, _T("패턴"), LVCFMT_LEFT, 80);
#else
	m_ctrlTreeList.InsertColumn(0, _T("FACP"), LVCFMT_LEFT, 300);
	m_ctrlTreeList.InsertColumn(1, _T("INPUT TYPE"), LVCFMT_LEFT, 80);
	m_ctrlTreeList.InsertColumn(2, _T("OUTPUT TYPE"), LVCFMT_LEFT, 80);
	m_ctrlTreeList.InsertColumn(3, _T("EQUIPMENT NAME"), LVCFMT_LEFT, 80);
	m_ctrlTreeList.InsertColumn(4, _T("NUMBER"), LVCFMT_LEFT, 50);
	m_ctrlTreeList.InsertColumn(5, _T("OUTPUT DESCRIPTION"), LVCFMT_LEFT, 80);
	m_ctrlTreeList.InsertColumn(6, _T("PATTERN"), LVCFMT_LEFT, 80);
#endif
	m_ctrlTreeList.SetExtendedStyle(TVS_EX_ITEMLINES | TVS_EX_ITEMLINES | TVS_EX_ALTERNATECOLOR
		| TVS_EX_SUBSELECT | TVS_EX_SUBSELECT) ;
	m_ctrlTreeList.SetUserDataSize(USER_DATA_SIZE);
	m_ctrlTreeList.SetDrawItemCB(DrawItem, this);
	m_ctrlTreeList.ExpandAll(TVI_ROOT);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


//*****************************************************************************
//*
//*		DrawItem
//*
//*****************************************************************************
//	Callback for item drawing.
//	hDc		: device context for drawing
//	pParam	: user parameter from the window (see at SetDrawItemCB(...))
//	pData	: is the item data
//	Retuns a value which decides how the item wil be drawn 
//				0 = Item will be drawed by window.  
//				1 = Item was drawed by function.  
//				2 = Background was drawed by function. Text will be drawn by window.  
int CALLBACK CPropPageFacp::DrawItem(HDC hDc, LPVOID pParam, TV_DRAWITEM *pData)
{
	HGDIOBJ	hLast;
	CRect	sRect;
	int		iEndX;
	int		iPosX;
	int		iPosY;

	if (pData->uTextSize == 0)return 0;


	ExtTextOut(hDc, 0, 0, ETO_OPAQUE, pData->pRect, NULL, 0, NULL);	// Draw Backgrount


	sRect = *pData->pRect;
	// Calc text width															
	DrawText(hDc, pData->pText, pData->uTextSize, &sRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT);

	iEndX = pData->pRect->right;
	iPosY = pData->pRect->top + 3;
	iPosX = sRect.right + 4;


	MoveToEx(hDc, iPosX, iPosY, NULL);
	SetDCPenColor(hDc, (pData->uLine & 1) ? RGB(100, 255, 100) : RGB(255, 100, 100));
	hLast = SelectObject(hDc, GetStockObject(DC_PEN));

	for (iPosX += 10; iPosX<iEndX; iPosX += 10)						// Draw line		
	{
		iPosY += 10;

		LineTo(hDc, iPosX, iPosY);

		iPosX += 10;
		iPosY -= 10;

		LineTo(hDc, iPosX, iPosY);
	}


	SelectObject(hDc, hLast);


	return 2;
}

BOOL CPropPageFacp::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	FillFacpList();
	return CPropertyPage::OnSetActive();
}

int CPropPageFacp::InitFacpList()
{
	if (m_pRefFasSysData == nullptr)
		return 0;

	return 0;
}

int CPropPageFacp::FillFacpList()
{
	if (m_pRefFasSysData == nullptr)
		return 0;

	m_pRefFasSysData->FillDeviceTreeList(&m_ctrlTreeList);

	m_ctrlTreeList.Expand(TVI_ROOT , TVE_EXPAND | TVE_ALLCHILDS);
	return 0;
}


LRESULT CPropPageFacp::OnWizardNext()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CPropertyPage::OnWizardNext();
}
