// DkContactFacp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DkContactFacp.h"
#include "DataFacpRelay.h"
#include "DataFacp.h"
#include "ManagerFacpRelay.h"
#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "../Common/Utils/ExcelWrapper.h"
// CDkContactFacp

IMPLEMENT_DYNAMIC(CDkContactFacp, CDockablePane)

CDkContactFacp::CDkContactFacp()
{
	m_pRefFasSysData = nullptr;
}

CDkContactFacp::~CDkContactFacp()
{
	RemoveAllTreeData();
}


BEGIN_MESSAGE_MAP(CDkContactFacp, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE, OnTvnContactDropedItem)
	//ON_COMMAND(ID_LOAD_CONTACT, &CDkContactFacp::OnLoadContact)
	ON_COMMAND(ID_LOAD_CONTACTFACP, &CDkContactFacp::OnLoadContact)
	//ON_COMMAND(ID_LOAD_CONTACTFACP, OnLoadContact)
	//ON_COMMAND(ID_PROPERTIES, OnLoadContact)
	ON_UPDATE_COMMAND_UI(ID_LOAD_CONTACTFACP, OnUpdateLoadContact)
END_MESSAGE_MAP()

// CDkContactFacp 메시지 처리기입니다.

int CDkContactFacp::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_CHECKBOXES | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_SHOWSELALWAYS;

	if (!m_ctrlContact.Create(dwViewStyle, rectDummy, this, IDC_CONTACTFACP_TREE))
	{
		TRACE0("클래스 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_ctrlContact.SetCursorResourceID(IDC_DROPADD_CURSOR);
	m_ctrlContact.SetAllowDragFlag(TRUE);
	// 이미지를 로드합니다.
 	m_TbContact.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_CONT_FACP);
 	m_TbContact.LoadToolBar(IDR_CONT_FACP, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

	m_TbContact.SetPaneStyle(m_TbContact.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_TbContact.SetPaneStyle(m_TbContact.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_TbContact.SetOwner(this);
	m_TbContact.SetRouteCommandsViaFrame(FALSE);
// 
// 	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
// 	m_TbContact.SetRouteCommandsViaFrame(FALSE);
	m_ctrlContact.SetAllowDragFlag(TRUE);

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	//FillClassView();
	AdjustLayout();

	m_ctrlContact.SetSendEventContainer(TRUE);
	return 0;
}

void CDkContactFacp::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_TbContact.CalcFixedLayout(FALSE, TRUE).cy;

	m_TbContact.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_ctrlContact.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDkContactFacp::OnChangeVisualStyle()
{
// 	m_ImgList.DeleteImageList();
// 
// 	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_BMP_DEVICE_ICON : IDB_BMP_DEVICE_ICON;
// 
// 	CBitmap bmp;
// 	if (!bmp.LoadBitmap(uiBmpId))
// 	{
// 		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
// 		ASSERT(FALSE);
// 		return;
// 	}
// 
// 	BITMAP bmpObj;
// 	bmp.GetBitmap(&bmpObj);
// 
// 	UINT nFlags = ILC_MASK;
// 
// 	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;
// 
// 	m_ImgList.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
// 	m_ImgList.Add(&bmp, RGB(0, 255, 255));
// 
// 	m_ctrlContact.SetImageList(&m_ImgList, TVSIL_NORMAL);
// 
// 	m_TbContact.CleanUpLockedImages();
// 	m_TbContact.LoadBitmap(theApp.m_bHiColorIcons ? IDB_BMP_TB_DKP_RELAY : IDB_BMP_TB_DKP_RELAY, 0, 0, TRUE /* 잠금 */);
}

void CDkContactFacp::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AdjustLayout();
}

void CDkContactFacp::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDkContactFacp::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CRect rectTree;
	m_ctrlContact.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}


void CDkContactFacp::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ctrlContact.SetFocus();

}

int CDkContactFacp::RemoveAllTreeData()
{
//	m_bPreventSelect = TRUE;
	ST_TREEITEM * pItem;
	if (m_ctrlContact.GetSafeHwnd())
		m_ctrlContact.DeleteAllItems();
	while (!m_ptrItemList.IsEmpty())
	{
		pItem = (ST_TREEITEM *)m_ptrItemList.RemoveHead();
		if (pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
	return 0;
}

int CDkContactFacp::InitTree()
{
	if (m_pRefFasSysData == nullptr)
		m_pRefFasSysData = theApp.GetRelayTableData();

	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"펌프정보가 정보가 설정되지 않았습니다.");
		return 0;
	}
	RemoveAllTreeData();
	std::shared_ptr<CManagerFacpRelay> spContact;
	POSITION pos;
	CDataFacp * pFacp;
	ST_TREEITEM * pItem;
	HTREEITEM hItem;
	HTREEITEM hFacp[D_MAX_FACP_COUNT + 1] = { nullptr };
	CDataFacpRelay * pData;
	spContact = m_pRefFasSysData->GetFacpContactManager();
	if (spContact == nullptr)
		return 0;
	pos = spContact->GetHeadPosition();
	while (pos)
	{
		pData = spContact->GetNext(pos);
		if (pData == nullptr)
			continue;
		if (hFacp[pData->GetFacpID()] == nullptr)
		{
			pFacp = m_pRefFasSysData->GetFacpByID(pData->GetFacpID());
			if (pFacp == nullptr)
				continue;
			pItem = new ST_TREEITEM;
			memset((void*)pItem, 0, sizeof(ST_TREEITEM));
			pItem->nDataType = TTYPE_DEV_FACP;
			pItem->pData = pFacp;
			pItem->hParent = TVI_ROOT;
			hItem = m_ctrlContact.InsertItem(pFacp->GetFacpName(), 2, 2, TVI_ROOT);
			pItem->hItem = hItem;
			hFacp[pData->GetFacpID()] = hItem;
			m_ctrlContact.SetItemData(hItem, (DWORD_PTR)pItem);
			m_ptrItemList.AddTail(pItem);
		}
		hItem = m_ctrlContact.InsertItem(pData->GetFRelayName(), 2, 2, hFacp[pData->GetFacpID()]);
		pItem = new ST_TREEITEM;
		memset((void*)pItem, 0, sizeof(ST_TREEITEM));
		pItem->nDataType = TTYPE_DEV_CONTACTFACP;
		pItem->pData = pData;
		pItem->hParent = hFacp[pData->GetFacpID()];
		pItem->hItem = hItem;
		m_ctrlContact.SetItemData(hItem, (DWORD_PTR)pItem);
		m_ptrItemList.AddTail(pItem);
	}
	return 1;
}

void CDkContactFacp::SetRelayTable(CRelayTableData * pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
}

int CDkContactFacp::AddDropWnd(CWnd* pWnd)
{
	m_ctrlContact.AddDropWnd(pWnd);
	return 0;
}
// 
// void CDkContactFacp::OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	*pResult = 0;
// 	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
// 	if (ptc == nullptr)
// 		return;
// 	// 	if (AfxGetMainWnd())
// 	// 		AfxGetMainWnd()->SendMessage(UWM_BYPASS_DROPEVENT, , ptc);
// 	// 	POSITION pos = theApp.GetFirstDocTemplatePosition();
// 	// 	CDocTemplate* pDocTemplate =nullptr;
// 	// 	POSITION doc_pos = nullptr;
// 	// 	CDocument* pDoc = nullptr;
// 	// 	POSITION view_pos = nullptr;
// 	// 	CView* pView = nullptr;
// 	// 
// 	// 	for (pos = theApp.GetFirstDocTemplatePosition(); pos != NULL; )
// 	// 	{
// 	// 		pDocTemplate = theApp.GetNextDocTemplate(pos);
// 	// 		for (doc_pos = pDocTemplate->GetFirstDocPosition(); doc_pos != NULL; ) 
// 	// 		{
// 	// 			pDoc = pDocTemplate->GetNextDoc(doc_pos);
// 	// 			for (view_pos = pDoc->GetFirstViewPosition(); view_pos != NULL; ) 
// 	// 			{
// 	// 				pView = pDoc->GetNextView(view_pos);
// 	// 				if (pView->IsKindOf(RUNTIME_CLASS(CSysLinkerView))) 
// 	// 				{
// 	// 					pView->SendMessage(WM_NOTIFY, (WPARAM)IDC_OUTVIEW_TREE, (LPARAM)ptc);
// 	// 				}
// 	// 			}
// 	// 		}
// 	// 	}
// }


void CDkContactFacp::OnLoadContact()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (AfxMessageBox(L"기존 접점 모두 삭제 됩니다\n그래도 진행하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
	if (pRelayTable == nullptr)
	{
		AfxMessageBox(L"프로젝트에 로그인하거나 새로 생성한 후에 진행해 주십시요");
		return;
	}
	CExcelWrapper xls;
	CString strDefault = L"*.xls", strFilter = L"접점 데이터(*.xls)|*.xls|All Files (*.*)|*.*||";
	CFileDialog FileDialog(TRUE, NULL, strDefault, OFN_HIDEREADONLY, strFilter, this);
	int i, nSheetCnt;
	BOOL bRead = FALSE;
	CString strSheetName, strSql;
	YAdoDatabase * pDB;
	std::shared_ptr<CManagerFacpRelay> spRefFacpContact;

	spRefFacpContact = pRelayTable->GetFacpContactManager();
	if (spRefFacpContact == nullptr)
	{
		// 		AfxMessageBox(L"자동생성 로직이 없습니다.");
		// 		return;
	}

	FileDialog.m_ofn.lpstrInitialDir = pRelayTable->GetProjectVersionPath();

	if (FileDialog.DoModal() != IDOK)
		return;
	pDB = pRelayTable->GetPrjDB();
	if (pDB == nullptr)
		return;

	CString strPath = FileDialog.GetPathName();
	if (xls.Open(strPath) == false)
	{
		AfxMessageBox(L"Excel파일을 여는데 실패 했습니다.");
		return;
	}

	nSheetCnt = xls.GetSheetCount();
	for (i = 0; i < nSheetCnt; i++)
	{
		// sheet 번호는 1부터
		if (xls.SetWorkSheetChange(i + 1) == FALSE)
			continue;
		strSheetName = xls.GetSheetName(i + 1);
		if (strSheetName.CompareNoCase(L"CONTACT") != 0)
			continue;
		bRead = TRUE;
		break;
	}

	if (bRead == FALSE)
	{
		AfxMessageBox(L"Excel파일 내에 'CONTACT' Sheet가 없습니다.");
		xls.Close();
		return;
	}


	strSql.Format(L"DELETE FROM TB_FACP_CONTACT ");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"데이터베이스에서 자동생성 로직을 삭제하는데 실패 했습니다.");
		xls.Close();
		return;
	}

	spRefFacpContact->RemoveAllData();

	pRelayTable->ParsingFacpContactData(&xls, spRefFacpContact);
	pRelayTable->InsertPrjBaseFacpContactDB();
	xls.Close();
	InitTree();
}
void CDkContactFacp::OnUpdateLoadContact(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	//pCmdUI->Enable(FALSE);
	pCmdUI->Enable(TRUE);
// 	if (pCmdUI)
// 		pCmdUI->SetCheck(m_wndOutput.IsVisible());
}
