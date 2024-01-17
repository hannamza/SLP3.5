// ImageStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ImageStatic.h"


// CImageStatic

IMPLEMENT_DYNAMIC(CImageStatic, CStatic)

CImageStatic::CImageStatic()
{
	m_dispNum = L"";
	m_bkColor = RGB(255, 255, 255);
	m_forecolor = RGB(0, 0, 255);
	m_brback.CreateSolidBrush(m_bkColor);
	m_bcheck = false;
	m_selcolor = RGB(255, 0, 0);
	m_bpicture = false;
}

CImageStatic::~CImageStatic()
{
}


BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CImageStatic 메시지 처리기입니다.


void CImageStatic::SetBackColor(COLORREF bkcolor)
{
	if (bkcolor != 0xffffffff)
		m_bkColor = bkcolor;
	else // Set default background color
		m_bkColor = ::GetSysColor(COLOR_BTNFACE);

	m_brback.DeleteObject();
	m_brback.CreateSolidBrush(m_bkColor);

	Update();
}

void CImageStatic::SetNum(CString  num)
{
	m_dispNum = num;
	Update();
}

void CImageStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	if (m_bpicture)
	{
		CBitmap bmp;
		CDC memdc;
		memdc.CreateCompatibleDC(&dc);
		BITMAP bm;
		
		bmp.LoadBitmap(m_nid);
		bmp.GetBitmap(&bm);

		CBitmap *poldbmp = memdc.SelectObject(&bmp);
		dc.StretchBlt(0, 0, rc.Width(), rc.Height(), &memdc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		memdc.SelectObject(poldbmp);
		//	Draw(&dc) ; 

	}

	else
	{
		if (m_bcheck)
		{
			CPen pn;

			//br.CreateSolidBrush (RGB(255,0, 0) ) ;
			pn.CreatePen(PS_SOLID, 4, m_selcolor);
			CPen *poldpen = dc.SelectObject(&pn);
			// TODO: Add your message handler code here
			dc.Rectangle(rc);
			dc.SelectObject(poldpen);
		}

		dc.FillSolidRect(&rc, m_bkColor);

		//		dc.FrameRect(&rc ,&br) ; 	
		Draw(&dc);
	}
	//CBrush br ; 

	// Do not call CStatic::OnPaint() for painting messages
}

void CImageStatic::Draw(CDC *pdc)
{
	CFont font;
	//CPaintDC dc(this) ; 
	CRect rc;
	//CBrush brback ; 


	GetClientRect(&rc);

	//int nHeight = -(pdc->GetDeviceCaps(LOGPIXELSY) * (rc.Height()) /72 );
	int nHeight = rc.Height() - 5;
	font.CreateFont(nHeight, 0, 0, 0, 0, FALSE, 0, 0
		, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"굴림");

	pdc->SetBkMode(TRANSPARENT);
	//pdc->SetBkColor(TRANSPARENT) ; 
	pdc->SetTextColor(RGB(0, 0, 0));

	CFont *poldfont = pdc->SelectObject(&font);
	//	CString str ; 
	//	str.Format("%d" , m_dispNum) ; 

	pdc->DrawText(m_dispNum, -1, &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	pdc->SelectObject(poldfont);

}

HBRUSH CImageStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO: Change any attributes of the DC here
	pDC->SetTextColor(m_forecolor);
	pDC->SetBkColor(m_bkColor);
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return (HBRUSH)m_brback;
}

void CImageStatic::Update()
{
	if (::IsWindow(m_hWnd))
		Invalidate(FALSE);
}

void CImageStatic::SetForecolor(COLORREF cr)
{
	if (cr != 0xffffffff)
		m_bkColor = cr;
	else // Set default background color
		m_forecolor = ::GetSysColor(COLOR_BTNFACE);

	// m_brback.DeleteObject();
	// m_brback.CreateSolidBrush(m_bkColor);

	Update();

}

void CImageStatic::SetCheck(bool bcheck)
{
	m_bcheck = bcheck;
	Update();
}

void CImageStatic::SetSelColor(COLORREF selcolor)
{
	m_selcolor = selcolor;
	Update();
}

void CImageStatic::LoadBitmap(UINT nid)
{
	m_nid = nid;
	m_bpicture = true;
	Update();
}

