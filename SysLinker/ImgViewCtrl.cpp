// ImgViewCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "ImgViewCtrl.h"

const UINT IDD_ANIMATION_TIMER = 1;

// CImgViewCtrl

IMPLEMENT_DYNAMIC(CImgViewCtrl, CStatic)

CImgViewCtrl::CImgViewCtrl()
{
	m_pImage = NULL;
	m_uCurrentFrame = 0;
	m_pDimensionID = NULL;
	m_GUIDCount = 0;
}

CImgViewCtrl::~CImgViewCtrl()
{
	if (m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
	if (m_pDimensionID)
	{
		delete[] m_pDimensionID;
		m_pDimensionID = NULL;
	}
}


BEGIN_MESSAGE_MAP(CImgViewCtrl, CStatic)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CImgViewCtrl 메시지 처리기입니다.


// CImgViewCtrl message handlers
void CImgViewCtrl::SetImageFile(LPCTSTR stSourceName)
{
	if (m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	//20240920 GBM start - 메모리 누수 오류 수정
	if (m_pDimensionID)
	{
		delete[] m_pDimensionID;
		m_pDimensionID = NULL;
	}
	//20240920 GBM end

	m_stFullName = stSourceName;

#ifdef _UNICODE
	m_pImage = Image::FromFile(m_stFullName);
#else if
	CA2W pFileName(m_stFullName);
	m_pImage = Image::FromFile(pFileName);
#endif

	m_GUIDCount = m_pImage->GetFrameDimensionsCount();
	m_pDimensionID = new GUID[m_GUIDCount];
	m_pImage->GetFrameDimensionsList(m_pDimensionID, m_GUIDCount);
	if (m_pImage->GetFrameCount(&m_pDimensionID[0]) > 1)
	{
		m_uCurrentFrame = 0;
		SetTimer(IDD_ANIMATION_TIMER, 200, NULL);
	}
	else
		KillTimer(IDD_ANIMATION_TIMER);

	Invalidate();
}

void CImgViewCtrl::OnDestroy()
{
	CStatic::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CImgViewCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CWnd::OnPaint() for painting messages
	CRect rect;
	GetClientRect(&rect);
	if (rect.IsRectEmpty())
		return;
	RectF RectDrw;
	Bitmap bitmap(rect.Width(), rect.Height());
	Graphics gr(&bitmap);
	gr.Clear(Color::White);
	if (m_pImage != NULL)
	{
		if (m_pImage->GetWidth() != 0 && m_pImage->GetHeight() != 0)
		{
			RectDrw = RectF(1.0f*rect.left, 1.0f*rect.top, 1.0f*rect.Width(), 1.0f*rect.Height());
			INT X = m_pImage->GetWidth(), Y = m_pImage->GetHeight(), x = rect.Width(), y = rect.Height();
			float a = (float)X / x, b = (float)Y / y;
			if (((float)m_pImage->GetHeight() / rect.Height()) > ((float)m_pImage->GetWidth() / rect.Width()))
			{
				// rectDraw.Y=0;
				RectDrw.Width = (float)rect.Height()*m_pImage->GetWidth() / m_pImage->GetHeight();
				RectDrw.X = (float)(rect.Width() - RectDrw.Width) / 2;
			}
			else
			{
				// rectDraw.X=0;
				RectDrw.Height = (float)rect.Width()*m_pImage->GetHeight() / m_pImage->GetWidth();
				RectDrw.Y = (float)(rect.Height() - RectDrw.Height) / 2;
			}
			gr.DrawImage(m_pImage, RectDrw);
		}
	}
	Graphics graphics(dc.m_hDC);
#ifdef __TEST_CODE__
	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

#endif
	graphics.DrawImage(&bitmap, 0, 0);
}


void CImgViewCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == IDD_ANIMATION_TIMER)
	{
		m_pImage->SelectActiveFrame(&m_pDimensionID[0], m_uCurrentFrame++);
		if (m_uCurrentFrame >= m_pImage->GetFrameCount(&m_pDimensionID[0]))
			m_uCurrentFrame = 0;
		Invalidate();
	}
	CStatic::OnTimer(nIDEvent);
}
