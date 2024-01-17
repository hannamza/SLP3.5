// DlgPatternProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgPatternProperty.h"
#include "afxdialogex.h"

#include "RelayTableData.h"
#include "ManagerPattern.h"
#include "DataPattern.h"
#include	"../Common/Utils/YAdoDatabase.h"

// CDlgPatternProperty 대화 상자입니다.

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


// CDlgPatternProperty 메시지 처리기입니다.


BOOL CDlgPatternProperty::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	VERIFY(m_ctrlPatternList.m_cImageList.Create(IDB_CHECKBOXES,16,3,RGB(255,0,255)));
	m_ctrlPatternList.m_HeaderCtrl.SetImageList(&m_ctrlPatternList.m_cImageList);
	m_ctrlPatternList.InsertColumn(0,_T("패턴"),LVCFMT_LEFT,350);
	m_ctrlPatternList.InsertColumn(1,_T("자동"),LVCFMT_LEFT,80);
	m_ctrlPatternList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	int i;
	HDITEM hditem;
	for(i = 0; i < m_ctrlPatternList.m_HeaderCtrl.GetItemCount(); i++)
	{
		hditem.mask = HDI_IMAGE | HDI_FORMAT;
		m_ctrlPatternList.m_HeaderCtrl.GetItem(i,&hditem);
		hditem.fmt |= HDF_IMAGE;
		if(i == 1) // 2번째 column을 check box로 setting
			hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
		else
			hditem.iImage = XHEADERCTRL_NO_IMAGE;

		m_ctrlPatternList.m_HeaderCtrl.SetItem(i,&hditem);
	}

	InitPattern();
	RedrawControl();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgPatternProperty::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"프로젝트가 연결되지 않았습니다.");
		return;
	}

	CString strSql;
	int i = 0,nCnt,nChk=0; 
	CDataPattern * pPtn;
	YAdoDatabase * pDB = nullptr;
	pDB = m_pRefFasSysData->GetPrjDB();
	if(pDB == nullptr)
	{
		AfxMessageBox(L"프로젝트 데이터베이스 정보를 가져오는데 실패 했습니다.");
		return;
	}

	if(pDB->IsOpen() == FALSE)
	{
		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		//m_ctrlPatternList.SetItemText(nIdx,1,L"자동");
		m_ctrlPatternList.SetCheckbox(nIdx,1,0);
		m_ctrlPatternList.SetItemData(nIdx,(DWORD_PTR)pPtn);
		nIdx ++;
	}
	return 1;
}


void CDlgPatternProperty::OnSize(UINT nType,int cx,int cy)
{
	CDialogEx::OnSize(nType,cx,cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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
