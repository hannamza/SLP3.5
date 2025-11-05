#pragma once


// CEditSpecial

class CEditSpecial : public CEdit
{
	DECLARE_DYNAMIC(CEditSpecial)

public:
	CEditSpecial();
	virtual ~CEditSpecial();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


