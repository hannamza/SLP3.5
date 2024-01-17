// DlgPatternProperty.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgPatternProperty.h"
#include "afxdialogex.h"

#include "RelayTableData.h"
#include "ManagerPattern.h"
#include "DataPattern.h"
#include	"../Common/Utils/YAdoDatabase.h"

// CDlgPatternProperty ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgPatternProperty, CDialogEx)

CDlgPatternProperty::CDlgPatternProperty(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PATTERN_PROPERTY, pParent)
{
	m_pRefFasSysData = nullptr;
}

CDlgPatternProperty::~CDlgPatternProperty()
{
}

void CDlgPatternProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_PATTERN_LIST,m_ctrlPatternList);
}


BEGIN_MESSAGE_MAP(CDlgPatternProperty, CDialogEx)
	ON_BN_CLICKED(IDOK,&CDlgPatternProperty::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,&CDlgPatternProperty::OnBnClickedCancel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgPatternProperty �޽��� ó�����Դϴ�.


BOOL CDlgPatternProperty::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	VERIFY(m_ctrlPatternList.m_cImageList.Create(IDB_CHECKBOXES,16,3,RGB(255,0,255)));
	m_ctrlPatternList.m_HeaderCtrl.SetImageList(&m_ctrlPatternList.m_cImageList);
	m_ctrlPatternList.InsertColumn(0,_T("����"),LVCFMT_LEFT,350);
	m_ctrlPatternList.InsertColumn(1,_T("�ڵ�"),LVCFMT_LEFT,80);
	m_ctrlPatternList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	int i;
	HDITEM hditem;
	for(i = 0; i < m_ctrlPatternList.m_HeaderCtrl.GetItemCount(); i++)
	{
		hditem.mask = HDI_IMAGE | HDI_FORMAT;
		m_ctrlPatternList.m_HeaderCtrl.GetItem(i,&hditem);
		hditem.fmt |= HDF_IMAGE;
		if(i == 1) // 2��° column�� check box�� setting
			hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
		else
			hditem.iImage = XHEADERCTRL_NO_IMAGE;

		m_ctrlPatternList.m_HeaderCtrl.SetItem(i,&hditem);
	}

	InitPattern();
	RedrawControl();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDlgPatternProperty::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"������Ʈ�� ������� �ʾҽ��ϴ�.");
		return;
	}

	CString strSql;
	int i = 0,nCnt,nChk=0; 
	CDataPattern * pPtn;
	YAdoDatabase * pDB = nullptr;
	pDB = m_pRefFasSysData->GetPrjDB();
	if(pDB == nullptr)
	{
		AfxMessageBox(L"������Ʈ �����ͺ��̽� ������ �������µ� ���� �߽��ϴ�.");
		return;
	}

	if(pDB->IsOpen() == FALSE)
	{
		AfxMessageBox(L"������Ʈ �����ͺ��̽��� ������� �ʾҽ��ϴ�.");
		return;
	}


	nCnt = m_ctrlPatternList.GetItemCount();
	for(i = 0; i < nCnt; i++)
	{
		pPtn = (CDataPattern *)m_ctrlPatternList.GetItemData(i);
		if(pPtn == nullptr)
			continue; 

		nChk = m_ctrlPatternList.GetCheckbox(i,1);
		strSql.Format(L"UPDATE TB_PATTERN_MST SET MANUAL_MAKE=%d "
			L" WHERE PT_ID=%d "
			,nChk,pPtn->GetPatternID()
		);

		if(pDB->ExecuteSql(strSql) == FALSE)
		{
			theApp.InsertLog(pDB->GetLastErrorString());
		}
		pPtn->SetManualMake(nChk);
	}

	g_bRequirePtnManualCheck = FALSE;
	CDialogEx::OnOK();

}


void CDlgPatternProperty::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnCancel();
}


int CDlgPatternProperty::InitPattern()
{
	if(m_pRefFasSysData == nullptr)
		return 0; 

	std::shared_ptr<CManagerPattern> sp = m_pRefFasSysData->GetPatternManager();
	if(sp == nullptr)
		return 0; 
	
	int nIdx = 0; 
	POSITION pos;
	CDataPattern * pPtn;
	pos = sp->GetHeadPosition();
	while(pos)
	{
		pPtn = sp->GetNext(pos);
		if(pPtn == nullptr)
			continue; 

		m_ctrlPatternList.InsertItem(nIdx,pPtn->GetPatternName());
		//m_ctrlPatternList.SetItemText(nIdx,1,L"�ڵ�");
		m_ctrlPatternList.SetCheckbox(nIdx,1,0);
		m_ctrlPatternList.SetItemData(nIdx,(DWORD_PTR)pPtn);
		nIdx ++;
	}
	return 1;
}


void CDlgPatternProperty::OnSize(UINT nType,int cx,int cy)
{
	CDialogEx::OnSize(nType,cx,cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	RedrawControl();

}


int CDlgPatternProperty::RedrawControl()
{
	CRect rc;
	if(m_ctrlPatternList.GetSafeHwnd())
	{
		GetClientRect(&rc);
		m_ctrlPatternList.MoveWindow(4,4,rc.Width() - 8,rc.Height() - 35);
		GetDlgItem(IDOK)->MoveWindow(rc.Width() - 204,rc.Height() - 30,100,20);
		GetDlgItem(IDCANCEL)->MoveWindow(rc.Width() - 104,rc.Height() - 30,100,20);
	}
	return 0;
}
