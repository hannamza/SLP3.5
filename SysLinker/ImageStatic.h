#pragma once


// CImageStatic

class CImageStatic : public CStatic
{
	DECLARE_DYNAMIC(CImageStatic)

public:
	CImageStatic();
	virtual ~CImageStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	bool m_bcheck;
	UINT m_nid;
	COLORREF m_forecolor;
	COLORREF m_selcolor;
	bool m_bpicture;
	CBrush m_brback;
	COLORREF m_bkColor;
	CString  m_dispNum;
	void LoadBitmap(UINT nid);
	void SetSelColor(COLORREF selcolor);
	void SetCheck(bool bcheck);

	void SetForecolor(COLORREF cr);
	void Update();
	void Draw(CDC *pdc);
	void SetNum(CString num);
	void SetBackColor(COLORREF bkcolor);



	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};


