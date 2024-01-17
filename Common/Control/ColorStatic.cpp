// ColorStatic.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ColorStatic.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

CColorStatic::CColorStatic()
{
	m_nHeight = 20;
	m_strFont = L"����ü";
	//m_Font.CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"����ü");
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



// CColorStatic �޽��� ó�����Դϴ�.




void CColorStatic::SetText(CString strText)
{
	SetWindowText(strText);
	m_strText = strText;
	Invalidate();
}


void CColorStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CStatic::OnPaint()��(��) ȣ������ ���ʽÿ�.
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
