#pragma once
#include "stdafx.h"
#include "Def_CustomCtrl.h"
#include "CustomItemManager.h"

// CCustomListCtrl 뷰입니다.
class CCustomBaseItem;
class CCustomListCtrl : public CScrollView
{
	DECLARE_DYNCREATE(CCustomListCtrl)                                                                         

public:
	CCustomListCtrl();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CCustomListCtrl();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	CSize						m_szTotalView;		/// Logical 화면의 크기
	int							m_nCurHScrollPos;	/// Vertical Scroll Position
	int							m_nCurWScrollPos;	/// Horizon Scroll Position


	int							m_nSpaceWidth;		/// 각 아이템간의 좌우 간격
	int							m_nSpaceHeight;		/// 각 아이템간의 상하 간격

	CSize						m_szOneItem;		/// 한개 아이템의 크기 - 간격은 포함되지 않는다.
													/// 아이템의 OnInitDialog 시 배경이미지의 크기로 설정

	int							m_nColumnCnt;		/// 컬럼 개수 - 0일때 컬럼개수는 폭에 맞춘다.
													/// 아이템 * 컬럼 개수가 폭보다 크면 Horizon Scroll 생성
	int							m_nMaxItemCount;	/// 최대 아이템의 개수를 제한한다.
	CCustomItemManager			m_Manager;			/// Item을 관리하기 위한 컨테이너(vector사용)

	// [2025/9/16 11:12:15 KHS] 
	// Item의 LButton Down후 일정 움직임이 있을 때 해당 리스트로 BeginDrag 이벤트를 전송한다.
	// x,y 위치중 10 Pixel이상 차이나면 이벤트 메세지를 전송한다.
	// Item은 Begin Drag 이벤트를 Parent(CCustomListCtlrl)로 보내고 자신의 Drag flag를 초기화 한다
	BOOL						m_bDragChild;		/// Item을 Drag : BeginDrag시 설정 --> Parent(상위 Dialog or View)로 Drag 시작 이벤트를 보내지 않는다.
													/// MouseMove 시 최초 좌표(BeginDrag 시 좌표)에서 어느정도 벗어나면 Parent로 Drag 이벤트 보냄
													/// Mouse Up 시 초기화
	CImageList			*		m_pDragImage;		/// BeginDrag 시 생성
	CCustomBaseItem		*		m_pDragItem;		/// Drag Item

	// [2025/9/16 10:09:26 KHS] 
	// Scroll Area Setting
	// 마우스가 Control의 일정 영역에 있을 때 스크롤 하기 위해 영역을 설정한다.
	// 우선 순위는 왼쪽위 모서리 일때 Up이 , 오른쪽아래 모서리 일때 Down
	int					m_nAreaUpScroll; // 상단 : 스크롤을 위로 올린다.
	int					m_nAreaDownScroll; // 하단 : 스크롤을 위로 올린다.
	int					m_nAreaLeftScroll; // 상단 : 스크롤을 위로 올린다.
	int					m_nAreaRightScroll; // 상단 : 스크롤을 위로 올린다.


	// [2025/9/25 14:00:44 KHS] 
	// Current Edit Item; - 한번 설정되면 nullptr이 될 상황이 없을거 같다
	// list내부에서 변경되거나 , List내의 lbutton 클릭시 아이템이 선택 아이템이 변경 될 수 있을 지 알수없다
	//int							m_nCurrentItem;
	CCustomBaseItem		*		m_pCurrentItem;
protected:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags,short zDelta,CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags,CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags,CPoint point);

	//Item Event
	afx_msg LRESULT OnItemBeginDrag(WPARAM wp,LPARAM lParam);
	afx_msg LRESULT OnItemLButtonDown(WPARAM wp,LPARAM lParam);
	afx_msg LRESULT OnItemLButtonUp(WPARAM wp,LPARAM lParam);
	afx_msg LRESULT OnItemLButtonDblClk(WPARAM wp,LPARAM lParam);
	afx_msg LRESULT OnItemRButtonDown(WPARAM wp,LPARAM lParam);
	afx_msg LRESULT OnItemRButtonUp(WPARAM wp,LPARAM lParam);	
	afx_msg LRESULT OnItemRButtonDblClk(WPARAM wp,LPARAM lParam);
	afx_msg LRESULT OnItemDelete(WPARAM wp,LPARAM lParam);
	afx_msg LRESULT OnItemSelected(WPARAM wp,LPARAM lParam);
	afx_msg LRESULT OnItemEditEnd(WPARAM wp,LPARAM lParam);
public:
	// [2025/10/16 13:20:11 KHS] 
	// CustomBaseItem의 변경이 있었는지 확인
	BOOL IsItemModified();
	// [2025/10/16 9:02:10 KHS] 
	// 생성된 모든 CCustomBaseItem을 초기화 한다.
	void InitAllItemData();
	// [2025/9/16 14:52:56 KHS] 
	// 컬럼 개수를 고정으로 할때 설정 - 0일때 폭 사이즈에 들어갈 수 있는 최대 컬럼
	void SetColumnCount(int nCnt) { m_nColumnCnt = nCnt; }

	// [2025/9/18 17:19:53 KHS] 
	// 한개 아이템의 크기를 설정
	void SetOneItemSize(CSize szOneItem) { m_szOneItem = szOneItem; }
	
	DWORD_PTR GetItemData(INT nIndex) { return m_Manager.GetItemData(nIndex); }
	BOOL SetItemData(INT nIndex,DWORD_PTR dwData) { return m_Manager.SetItemData(nIndex,dwData); }
	BOOL AddItem(CCustomBaseItem * pItem);
	BOOL InsertAfterItem(int nIndex,CCustomBaseItem * pItem);
	int InsertBeforeItem(int nIndex,CCustomBaseItem * pItem);
	// [2025/9/18 17:21:02 KHS] 
	// 마우스의 Screen Point를 사용해서 아이템의 Index를 가져온다.
	int PointToIndex(CPoint ptScreen);

	// [2025/9/18 17:22:24 KHS] 
	// 마우스의 Screen Point를 사용해서 아이템을 가져온다.
	CCustomBaseItem * PointToItem(CPoint ptScreen);
	
	// [2025/9/18 17:22:43 KHS] 
	// 마우스의 Screen Point가 Scroll영역에 있는지 확인한다.(상,하,좌,우,전체영역안)
	int IsScrollArea(CPoint ptScreen);

	// [2025/9/18 17:23:58 KHS] 
	// 리스트의 Logical 화면을 상하로 스크롤한다. bAccel은 속도 증가시킴
	int ScrollUpDownList(BOOL bUp , BOOL bAccel);

	// [2025/9/18 17:24:54 KHS] 
	// Item의 Drag Image를 생성한다.
	CImageList* CreateDragControlImage(int nItem);
	// [2025/9/18 17:25:33 KHS] 
	// 아이템의 개수를 리턴
	int	GetItemCount() { return (int)m_Manager.size(); }

	// [2025/9/18 17:25:48 KHS] 
	// nIndex는 vector에서 Index
	CCustomBaseItem * GetItem(int nIndex); // 0부터
	BOOL DeleteListItem(INT nIndex);
	// 모두 삭제
	int RemoveAllData();

	int GetCurSelIndex();
	CCustomBaseItem * GetCurSelData() { return m_pCurrentItem; }

	BOOL SetCurSelIndex(int nSel);
	BOOL SetCurSelData(CCustomBaseItem * pItem);
protected:
	// [2025/9/18 17:26:18 KHS] 
	// 화면 영역에 맞게 Item을 재 배치한다.
	int ReArrangeItem();

	// [2025/9/18 17:26:52 KHS] 
	// 한개의 아이템의 크기를 설정한다.
	BOOL InitOneItemSize();
};


