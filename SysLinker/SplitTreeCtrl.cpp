// SplitTreeCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "SplitTreeCtrl.h"


// CSplitTreeCtrl

IMPLEMENT_DYNAMIC(CSplitTreeCtrl, CTreeCtrl)

CSplitTreeCtrl::CSplitTreeCtrl()
{

}

CSplitTreeCtrl::~CSplitTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CSplitTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CSplitTreeCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CSplitTreeCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CSplitTreeCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, &CSplitTreeCtrl::OnNMKillfocus)
	ON_NOTIFY_REFLECT(NM_OUTOFMEMORY, &CSplitTreeCtrl::OnNMOutofmemory)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CSplitTreeCtrl::OnNMRClick)
	ON_NOTIFY_REFLECT(NM_RDBLCLK, &CSplitTreeCtrl::OnNMRDblclk)
	ON_NOTIFY_REFLECT(NM_RETURN, &CSplitTreeCtrl::OnNMReturn)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, &CSplitTreeCtrl::OnNMSetfocus)
	ON_NOTIFY_REFLECT(NM_THEMECHANGED, &CSplitTreeCtrl::OnNMThemeChanged)
	ON_NOTIFY_REFLECT(NM_TVSTATEIMAGECHANGING, &CSplitTreeCtrl::OnNMTVStateImageChanging)
	ON_NOTIFY_REFLECT(TVN_ASYNCDRAW, &CSplitTreeCtrl::OnTvnAsyncDraw)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CSplitTreeCtrl::OnTvnBegindrag)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CSplitTreeCtrl::OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, &CSplitTreeCtrl::OnTvnBeginrdrag)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &CSplitTreeCtrl::OnTvnDeleteitem)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CSplitTreeCtrl::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, &CSplitTreeCtrl::OnTvnGetdispinfo)
	ON_NOTIFY_REFLECT(TVN_GETINFOTIP, &CSplitTreeCtrl::OnTvnGetInfoTip)
	ON_NOTIFY_REFLECT(TVN_ITEMCHANGED, &CSplitTreeCtrl::OnTvnItemChanged)
	ON_NOTIFY_REFLECT(TVN_ITEMCHANGING, &CSplitTreeCtrl::OnTvnItemChanging)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CSplitTreeCtrl::OnTvnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &CSplitTreeCtrl::OnTvnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, &CSplitTreeCtrl::OnTvnKeydown)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CSplitTreeCtrl::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, &CSplitTreeCtrl::OnTvnSelchanging)
	ON_NOTIFY_REFLECT(TVN_SETDISPINFO, &CSplitTreeCtrl::OnTvnSetdispinfo)
	ON_NOTIFY_REFLECT(TVN_SINGLEEXPAND, &CSplitTreeCtrl::OnTvnSingleExpand)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PARENTNOTIFY_REFLECT()
END_MESSAGE_MAP()



// CSplitTreeCtrl 메시지 처리기입니다.




void CSplitTreeCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMOutofmemory(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMRDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMReturn(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMThemeChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 이 기능을 사용하려면 Windows XP 이상이 필요합니다.
	// _WIN32_WINNT 기호는 0x0501보다 크거나 같아야 합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnNMTVStateImageChanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnAsyncDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVASYNCDRAW *pNMTVAsyncDraw = reinterpret_cast<NMTVASYNCDRAW*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnItemChanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE *>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnSetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSplitTreeCtrl::OnTvnSingleExpand(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


HBRUSH CSplitTreeCtrl::CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/)
{
	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  부모 처리기가 호출되지 않을 경우 Null이 아닌 브러시를 반환합니다.
	return NULL;
}


void CSplitTreeCtrl::ParentNotify(UINT /*message*/, LPARAM /*lParam*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
