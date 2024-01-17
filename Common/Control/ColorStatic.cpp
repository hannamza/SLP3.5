// ColorStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ColorStatic.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

CColorStatic::CColorStatic()
{
	m_nHeight = 20;
	m_strFont = L"돋움체";
	//m_Font.CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"돋움체");
	m_crBack = RGB(0, 0, 0);
	//m_brBack.CreateSolidBrush(m_crBack);
	m_crText = RGB(255, 128, 9);
	m_nAlignment = DT_RIGHT;
	m_bBoarder = FALSE;
	m_dwData = 0;
}

CColorStatic::~CColorStatic()
{

}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CColorStatic 메시지 처리기입니다.




void CColorStatic::SetText(CString strText)
{
	SetWindowText(strText);
	m_strText = strText;
	Invalidate();
}


void CColorStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CStatic::OnPaint()을(를) 호출하지 마십시오.
	CRect rc;
	CFont fn;
	CFont *pOldFont = nullptr;
	
	fn.CreateFont(m_nHeight, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, m_strFont);
	pOldFont = dc.SelectObject(&fn);
	GetClientRect(&rc);
	dc.SetTextColor(m_crText);
	//dc.FillRect(&rc, &br);
	if (m_bBoarder)
	{
		CBrush br;
		br.CreateSolidBrush(m_crBack);
		CPen pen(PS_SOLID, 2, m_crText);	// same as combobox
		CPen *pOldPen = dc.SelectObject(&pen);
		CBrush * pOldBrush = (CBrush *)dc.SelectObject(&br);
		dc.Rectangle(&rc);
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}
	else
	{
		dc.FillSolidRect(&rc, m_crBack);
	}
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(m_strText, &rc, m_nAlignment);
	dc.SelectObject(pOldFont);
//	dc.TextOut(0, 0, m_strText);
}



void CColorStatic::SetBoarder(bool bShow)
{
	m_bBoarder = bShow;
}
