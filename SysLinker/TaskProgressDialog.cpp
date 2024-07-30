// TaskProgressDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "TaskProgressDialog.h"
#include "afxdialogex.h"


// CTaskProgressDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTaskProgressDialog, CDialogEx)

CTaskProgressDialog::CTaskProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TASKPROGRESS_DLG, pParent)
{
	m_bCancel = FALSE;
	m_nLower = 0;
	m_nUpper = 100;
	m_nStep = 10;
	m_nCurTaskID = 0;
	m_HilightColor = RGB(0,0,0);
	m_TaskDoneColor = RGB(0,0,0);
	m_TaskToDoColor = RGB(0,0,0);
	m_sTaskList.RemoveAll();
	m_bSelMode = TRUE;
	m_bParentDisabled = FALSE;
}

CTaskProgressDialog::~CTaskProgressDialog()
{
	m_sTaskList.RemoveAll();

}

void CTaskProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TASK_LIST,m_TaskListCtrl);
	DDX_Control(pDX,IDC_PROG_JOB,m_Progress);
}


BEGIN_MESSAGE_MAP(CTaskProgressDialog, CDialogEx)
	ON_BN_CLICKED(IDOK,&CTaskProgressDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,&CTaskProgressDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// CTaskProgressDialog 메시지 처리기입니다.
BOOL CTaskProgressDialog::Create(CWnd *pParent)
{
	// Get the true parent of the dialog
	//     m_pParentWnd = CWnd::GetSafeOwner(pParent);
	// 
	//     // m_bParentDisabled is used to re-enable the parent window
	//     // when the dialog is destroyed. So we don't want to set
	//     // it to TRUE unless the parent was already enabled.
	// 
	//     if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
	//     {
	//       m_pParentWnd->EnableWindow(FALSE);
	//       m_bParentDisabled = TRUE;
	//     }

	if(!CDialogEx::Create(IDD_TASKPROGRESS_DLG,pParent))
	{
		//ReEnableParent();
		return FALSE;
	}

	return TRUE;
}


void CTaskProgressDialog::SetStatus(LPCTSTR lpszMessage)
{
	ASSERT(m_hWnd); // Don't call this _before_ the dialog has
					// been created. Can be called from OnInitDialog
	CWnd *pWndStatus = GetDlgItem(IDC_ST_PROG_STATUS);

	// Verify that the static text control exists
	ASSERT(pWndStatus != NULL);
	pWndStatus->SetWindowText(lpszMessage);
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::OnCancel()
{
	m_bCancel = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::SetRange(int nLower,int nUpper)
{
	m_nLower = nLower;
	m_nUpper = nUpper;
	m_Progress.SetRange(nLower,nUpper);
}

/////////////////////////////////////////////////////////////////////////////
//
int CTaskProgressDialog::SetPos(int nPos)
{
	//PumpMessages();
	int iResult = m_Progress.SetPos(nPos);
	UpdatePercent(nPos);
	return iResult;
}
/////////////////////////////////////////////////////////////////////////////
//
// void CTaskProgressDialog::PumpMessages()
// {
//     // Must call Create() before using the dialog
//     ASSERT(m_hWnd!=NULL);
// 
//     MSG msg;
//     // Handle dialog messages
//     while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//     {
//       if(!IsDialogMessage(&msg))
//       {
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);  
//       }
//     }
// }
/////////////////////////////////////////////////////////////////////////////
//
int CTaskProgressDialog::SetStep(int nStep)
{
	m_nStep = nStep; // Store for later use in calculating percentage
	return m_Progress.SetStep(nStep);
}

int CTaskProgressDialog::OffsetPos(int nPos)
{
	//PumpMessages();
	int iResult = m_Progress.OffsetPos(nPos);
	UpdatePercent(iResult + nPos);
	return iResult;
}

/////////////////////////////////////////////////////////////////////////////
//
int CTaskProgressDialog::StepIt()
{
	// PumpMessages();
	int iResult = m_Progress.StepIt();
	UpdatePercent(iResult + m_nStep);
	return iResult;
}

BOOL CTaskProgressDialog::CheckCancelButton()
{
	// Process all pending messages
	// PumpMessages();

	// Reset m_bCancel to FALSE so that
	// CheckCancelButton returns FALSE until the user
	// clicks Cancel again. This will allow you to call
	// CheckCancelButton and still continue the operation.
	// If m_bCancel stayed TRUE, then the next call to
	// CheckCancelButton would always return TRUE

	BOOL bResult = m_bCancel;
	m_bCancel = FALSE;

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::UpdatePercent(int nNewPos)
{
	CWnd *pWndPercent = GetDlgItem(CG_IDC_PROGDLG_PERCENT);
	int nPercent;

	int nDivisor = m_nUpper - m_nLower;
	ASSERT(nDivisor > 0);  // m_nLower should be smaller than m_nUpper

	int nDividend = (nNewPos - m_nLower);
	ASSERT(nDividend >= 0);   // Current position should be greater than m_nLower

	nPercent = nDividend * 100 / nDivisor;

	// Since the Progress Control wraps, we will wrap the percentage
	// along with it. However, don't reset 100% back to 0%
	if(nPercent != 100)
		nPercent %= 100;

	// Display the percentage
	CString strBuf;
	strBuf.Format(_T("%d%c"),nPercent,_T('%'));

	CString strCur; // get current percentage
	pWndPercent->GetWindowText(strCur);

	if(strCur != strBuf)
		pWndPercent->SetWindowText(strBuf);
}

/////////////////////////////////////////////////////////////////////////////
// CTaskProgressDialog message handlers

BOOL CTaskProgressDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int i;
	m_Progress.SetRange(m_nLower,m_nUpper);
	m_Progress.SetStep(m_nStep);
	m_Progress.SetPos(m_nLower);

#ifndef ENGLISH_MODE
	SetWindowText(L"작업 진행 현황.....");
#else
	SetWindowText(L"WORK IN PROGRESS.....");
#endif

	HICON hIcon[3];
	m_ImageList.Create(16,16,0,2,2);				// 32, 32 for large icons
	hIcon[0] = AfxGetApp()->LoadIcon(IDI_OK1);
	hIcon[1] = AfxGetApp()->LoadIcon(IDI_RIGHTARROW1);
	hIcon[2] = AfxGetApp()->LoadIcon(IDI_CANCEL1);

	for(i = 0; i < 3; i++)	m_ImageList.Add(hIcon[i]);

	m_TaskListCtrl.SetImageList(&m_ImageList,LVSIL_SMALL);

	//	m_TaskListCtrl.SetTextBkColor(::GetSysColor( COLOR_3DFACE));
	//	m_TaskListCtrl.SetBkColor(::GetSysColor( COLOR_3DFACE));
	m_TaskListCtrl.SetTextColor(RGB(0,0,0));

	m_TaskListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_TaskListCtrl.InsertColumn(0,_T("Status"),LVCFMT_LEFT,45,-1);
	m_TaskListCtrl.InsertColumn(1,_T("Tasks"),LVCFMT_LEFT,400,-1);

	InitTasks();

	UpdateData(FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::SetTasks(const CStringArray &sTaskList)
{
	if(sTaskList.GetSize() < 1) return;			// Empty list, do nothing
	m_sTaskList.RemoveAll();						// Clear the list
	for(int i = 0; i < sTaskList.GetSize(); i++)
		m_sTaskList.Add(sTaskList.GetAt(i));		// Copy list items to task list of dialog
}

int CTaskProgressDialog::AddTask(CString strTask,BOOL bInit)
{
	if(bInit == TRUE)
		m_sTaskList.RemoveAll();						// Clear the list
	m_sTaskList.Add(strTask);
	return m_sTaskList.GetSize();
}


/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::SetCurTask(int nID)
{
	if((nID < 0) || (nID >= m_sTaskList.GetSize())) return; // Illegal task ID
	m_nCurTaskID = nID;		// Set current task to be the desired one

	UpdateTasks();
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::UpdateTasks()
{
	if(m_sTaskList.GetSize() < 1) return;
	int nPrevItemID = m_nCurTaskID - 1;
	if(nPrevItemID >= 0)				// Set previous task done
	{
		m_TaskListCtrl.SetItem(nPrevItemID,0,LVIF_IMAGE,NULL,0,0,0,0);
		if(IsSelectMode())
			m_TaskListCtrl.SetItemState(nPrevItemID,0,LVIS_SELECTED | LVIS_FOCUSED);
	}

	m_TaskListCtrl.SetItem(m_nCurTaskID,0,LVIF_IMAGE,L"",1,0,0,0);	// Set current task as processing	
	if(IsSelectMode())
		m_TaskListCtrl.SetItemState(m_nCurTaskID,LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
	else
		m_TaskListCtrl.SetHotItem(m_nCurTaskID);

	// Make sure that the current processing task is visible at the middle of the list view
	int nBottom = m_nCurTaskID + m_TaskListCtrl.GetCountPerPage() / 2;
	if(nBottom > m_sTaskList.GetSize() - 1) nBottom = m_sTaskList.GetSize() - 1;
	m_TaskListCtrl.EnsureVisible(nBottom,FALSE);
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::InitTasks()
{
	if(m_sTaskList.GetSize() < 1) return;
	m_TaskListCtrl.DeleteAllItems();
	int i;

	for(i = 0; i < m_nCurTaskID; i++)		// Processed tasks
	{
		m_TaskListCtrl.InsertItem(i,L"",0);
		m_TaskListCtrl.SetItem(i,1,LVIF_TEXT,m_sTaskList.GetAt(i),0,0,0,0);
	}

	// Current task
	m_TaskListCtrl.InsertItem(m_nCurTaskID,L"",1);
	m_TaskListCtrl.SetItem(m_nCurTaskID,1,LVIF_TEXT,m_sTaskList.GetAt(m_nCurTaskID),0,0,0,0);
	if(IsSelectMode())								// Selection mode
		m_TaskListCtrl.SetItemState(m_nCurTaskID,LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
	else
		m_TaskListCtrl.SetHotItem(m_nCurTaskID);	// Hot mode

	for(i = m_nCurTaskID + 1; i < m_sTaskList.GetSize(); i++)	// Un-processed tasks
	{
		m_TaskListCtrl.InsertItem(i,L"",2);
		m_TaskListCtrl.SetItem(i,1,LVIF_TEXT,m_sTaskList.GetAt(i),0,0,0,0);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::SetSelectMode(BOOL bSel /* = TRUE */)
{
	m_bSelMode = bSel;
}

/////////////////////////////////////////////////////////////////////////////
//
BOOL CTaskProgressDialog::IsSelectMode() const
{
	return (m_bSelMode);
}


void CTaskProgressDialog::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialog::OnOK();
}

void CTaskProgressDialog::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
