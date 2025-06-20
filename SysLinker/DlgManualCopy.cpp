// DlgManualCopy.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgManualCopy.h"
#include "afxdialogex.h"

#include "DataLinked.h"
#include "DataDevice.h"

// CDlgManualCopy 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgManualCopy, CDialogEx)

#ifndef ENGLISH_MODE
CDlgManualCopy::CDlgManualCopy(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MANUAL_COPY, pParent)
{

}
#else
CDlgManualCopy::CDlgManualCopy(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MANUAL_COPY_EN, pParent)
{

}
#endif

CDlgManualCopy::~CDlgManualCopy()
{
}

void CDlgManualCopy::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_ctrlEditInput);
	DDX_Control(pDX, IDC_LIST_PATTERN, m_ctrlListPattern);
	DDX_Control(pDX, IDC_LIST_RELAY, m_ctrlListRelay);
}


BEGIN_MESSAGE_MAP(CDlgManualCopy, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgManualCopy::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgManualCopy 메시지 처리기입니다.


BOOL CDlgManualCopy::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	InitControls();
	SetNewInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgManualCopy::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;

	CDialogEx::PostNcDestroy();
}


void CDlgManualCopy::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ShowWindow(SW_HIDE);
	return;

	CDialogEx::OnOK();
}

void CDlgManualCopy::InitControls()
{
	// 리스트컨트롤 헤더 입력
	m_ctrlListPattern.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlListPattern.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

#ifndef ENGLISH_MODE
	m_ctrlListPattern.InsertColumn(0, L"제어 접점", LVCFMT_LEFT, 150);
	m_ctrlListPattern.InsertColumn(1, L"접점 종류", LVCFMT_LEFT, 150);
#else
	m_ctrlListPattern.InsertColumn(0, L"CONTROL RELAY CONTACT", LVCFMT_LEFT, 150);
	m_ctrlListPattern.InsertColumn(1, L"RELAY CONTACT TYPE", LVCFMT_LEFT, 150);
#endif

	m_ctrlListRelay.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlListRelay.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

#ifndef ENGLISH_MODE
	m_ctrlListRelay.InsertColumn(0, L"순서", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(1, L"출력이름", LVCFMT_LEFT, 270);
	m_ctrlListRelay.InsertColumn(2, L"입력타입", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(3, L"출력타입", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(4, L"설비명", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(5, L"설비번호", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(6, L"출력설명", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(7, L"위치", LVCFMT_LEFT, 150);
	m_ctrlListRelay.InsertColumn(8, L"연동타입", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(9, L"출력종류", LVCFMT_LEFT, 80);
#else
	m_ctrlListRelay.InsertColumn(0, L"ORDER", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(1, L"OUTPUT NAME", LVCFMT_LEFT, 300);
	m_ctrlListRelay.InsertColumn(2, L"INPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(3, L"OUTPUT TYPE", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(4, L"EQUIPMENT NAME", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(5, L"EQUIPMENT NO.", LVCFMT_LEFT, 50);
	m_ctrlListRelay.InsertColumn(6, L"OUTPUT DESCRIPTION", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(7, L"LOCATION", LVCFMT_LEFT, 150);
	m_ctrlListRelay.InsertColumn(8, L"INTERLOCK TYPE", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(9, L"OUTPUT CATEGORY", LVCFMT_LEFT, 80);
#endif

}

void CDlgManualCopy::SetNewInfo()
{
	SetInputCircuitInfo();
	SetManualOutputInfo();
}

void CDlgManualCopy::SetInputCircuitInfo()
{
	CDataDevice* pDataDevice = nullptr;
	pDataDevice = CManualLinkManager::Instance()->GetCurrentInputCircuit();
	if (pDataDevice == nullptr)
	{
		return;
	}

	CString strInputName = _T("");
	strInputName = pDataDevice->GetInputFullName();
	m_ctrlEditInput.SetWindowText(strInputName);
}

void CDlgManualCopy::SetManualOutputInfo()
{
	//기존 리스트 항목 초기화
	m_ctrlListRelay.DeleteAllItems();
	m_ctrlListPattern.DeleteAllItems();

	// 가공된 데이터로 리스트를 채움
	std::vector<MANUAL_COPY_INFO> mciVec;
	mciVec = CManualLinkManager::Instance()->GetManualLinkVector();

	if (mciVec.size() == 0)
	{
		return;
	}
	std::vector<MANUAL_COPY_INFO>::iterator iter;
	iter = mciVec.begin();

	int nRelayListIndex = 0;
	int nPatternListIndex = 0;
	for (; iter != mciVec.end(); iter++)
	{
		if (iter->nLinkType == LK_TYPE_RELEAY)
		{
			InsertManualOutputRelayList(*iter, nRelayListIndex);
			nRelayListIndex++;
		}
		else
		{
			InsertManualOutputPatternList(*iter, nPatternListIndex);
			nPatternListIndex++;
		}
	}

}

void CDlgManualCopy::InsertManualOutputRelayList(MANUAL_COPY_INFO mci, int nIndex)
{
	CString strIndex = _T("");
	strIndex.Format(_T("%03d"), nIndex + 1);
	m_ctrlListRelay.InsertItem(nIndex, strIndex);
	m_ctrlListRelay.SetItemText(nIndex, 1, mci.strFullName);
	m_ctrlListRelay.SetItemText(nIndex, 2, mci.strInputType);
	m_ctrlListRelay.SetItemText(nIndex, 3, mci.strOutputType);
	m_ctrlListRelay.SetItemText(nIndex, 4, mci.strEquipmentName);
	m_ctrlListRelay.SetItemText(nIndex, 5, mci.strEquipmentNumber);
	m_ctrlListRelay.SetItemText(nIndex, 6, mci.strOutputDecription);
	m_ctrlListRelay.SetItemText(nIndex, 7, mci.strPosition);
	m_ctrlListRelay.SetItemText(nIndex, 8, mci.strLinkType);
	m_ctrlListRelay.SetItemText(nIndex, 9, mci.strOutputKind);
}

void CDlgManualCopy::InsertManualOutputPatternList(MANUAL_COPY_INFO mci, int nIndex)
{
	m_ctrlListPattern.InsertItem(nIndex, mci.strFullName);
	m_ctrlListPattern.SetItemText(nIndex, 1, mci.strContactType);
}

BOOL CDlgManualCopy::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
