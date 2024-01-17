#pragma once


// CColorStatic

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:
	CColorStatic();
	virtual ~CColorStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetText(CString strText);
	afx_msg void OnPaint();
	CString m_strText;
	//CFont		m_Font;

	//CBrush			m_brBack;
	COLORREF		m_crBack;
	COLORREF		m_crText;
	int				m_nHeight;
	CString			m_strFont;
	int				m_nAlignment;
	BOOL			m_bBoarder;
	DWORD			m_dwData;
	void SetAlignment(int nAlign) { m_nAlignment = nAlign; }
	void SetFont(CString strFont) { m_strFont = strFont; }
	void SetHeight(int nHeight) { m_nHeight = nHeight; }
	void 	SetBackColor(COLORREF crBack) { m_crBack = crBack; }
	void SetTextColor(COLORREF crText) { m_crText = crText; }
	void SetBoarder(bool bShow);
	DWORD GetItemData() { return m_dwData; }
	void SetItemData(DWORD dwData) { m_dwData = dwData; }
};


