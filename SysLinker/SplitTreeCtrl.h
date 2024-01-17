#pragma once


// CSplitTreeCtrl

class CSplitTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CSplitTreeCtrl)

public:
	CSplitTreeCtrl();
	virtual ~CSplitTreeCtrl();
	void SetMessageParent(CWnd * pMsgParent) {m_pMsgParent;};
protected:
	CWnd		*		m_pMsgParent;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMOutofmemory(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReturn(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMTVStateImageChanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnAsyncDraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemChanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSingleExpand(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void ParentNotify(UINT /*message*/, LPARAM /*lParam*/);
};


