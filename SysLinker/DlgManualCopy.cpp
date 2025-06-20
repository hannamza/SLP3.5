// DlgManualCopy.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgManualCopy.h"
#include "afxdialogex.h"

#include "DataLinked.h"
#include "DataDevice.h"

// CDlgManualCopy ��ȭ �����Դϴ�.

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


// CDlgManualCopy �޽��� ó�����Դϴ�.


BOOL CDlgManualCopy::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	InitControls();
	SetNewInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDlgManualCopy::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	delete this;

	CDialogEx::PostNcDestroy();
}


void CDlgManualCopy::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	ShowWindow(SW_HIDE);
	return;

	CDialogEx::OnOK();
}

void CDlgManualCopy::InitControls()
{
	// ����Ʈ��Ʈ�� ��� �Է�
	m_ctrlListPattern.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlListPattern.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

#ifndef ENGLISH_MODE
	m_ctrlListPattern.InsertColumn(0, L"���� ����", LVCFMT_LEFT, 150);
	m_ctrlListPattern.InsertColumn(1, L"���� ����", LVCFMT_LEFT, 150);
#else
	m_ctrlListPattern.InsertColumn(0, L"CONTROL RELAY CONTACT", LVCFMT_LEFT, 150);
	m_ctrlListPattern.InsertColumn(1, L"RELAY CONTACT TYPE", LVCFMT_LEFT, 150);
#endif

	m_ctrlListRelay.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlListRelay.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

#ifndef ENGLISH_MODE
	m_ctrlListRelay.InsertColumn(0, L"����", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(1, L"����̸�", LVCFMT_LEFT, 270);
	m_ctrlListRelay.InsertColumn(2, L"�Է�Ÿ��", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(3, L"���Ÿ��", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(4, L"�����", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(5, L"�����ȣ", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(6, L"��¼���", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(7, L"��ġ", LVCFMT_LEFT, 150);
	m_ctrlListRelay.InsertColumn(8, L"����Ÿ��", LVCFMT_LEFT, 80);
	m_ctrlListRelay.InsertColumn(9, L"�������", LVCFMT_LEFT, 80);
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
	//���� ����Ʈ �׸� �ʱ�ȭ
	m_ctrlListRelay.DeleteAllItems();
	m_ctrlListPattern.DeleteAllItems();

	// ������ �����ͷ� ����Ʈ�� ä��
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
