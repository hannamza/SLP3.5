#pragma once


// CImgViewCtrl

class CImgViewCtrl : public CStatic
{
	DECLARE_DYNAMIC(CImgViewCtrl)

public:
	CImgViewCtrl();
	virtual ~CImgViewCtrl();
	CString m_stFullName;
	void SetImageFile(LPCTSTR stSourceName);
	Image* m_pImage;
	UINT m_uCurrentFrame;
	GUID* m_pDimensionID;
	UINT m_GUIDCount;
protected:
	DECLARE_MESSAGE_MAP()
public:
// 	afx_msg void OnPaint();
// 	afx_msg void OnTimer(UINT nIDEvent);
// 	afx_msg void OnDestroy();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


