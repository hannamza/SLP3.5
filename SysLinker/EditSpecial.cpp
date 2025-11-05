// EditSpecial.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EditSpecial.h"


// CEditSpecial

IMPLEMENT_DYNAMIC(CEditSpecial, CEdit)

CEditSpecial::CEditSpecial()
{
}

CEditSpecial::~CEditSpecial()
{
}


BEGIN_MESSAGE_MAP(CEditSpecial, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CEditSpecial 메시지 처리기입니다.




void CEditSpecial::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 허용 문자가 아닌 경우, 입력을 막습니다.
	if(',' == nChar)
	{
		return; // 입력 무시
	}
	CEdit::OnChar(nChar,nRepCnt,nFlags);
}


BOOL CEditSpecial::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CEdit::PreTranslateMessage(pMsg);
}
