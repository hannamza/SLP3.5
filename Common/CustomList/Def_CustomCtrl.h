#pragma once
// Item과 Parent(CCustomListCtrl)과 통신 메세지 정의
// CCustomListCtrl의 Internal Message
#define	CIM_ITEM_BASE						WM_USER + 2000
#define CIM_ITEM_BEGINDRAG					CIM_ITEM_BASE + 1
#define CIM_ITEM_LBUTTON_DCLICK				CIM_ITEM_BASE + 2
#define CIM_ITEM_LBUTTON_DOWN				CIM_ITEM_BASE + 3
#define CIM_ITEM_LBUTTON_UP					CIM_ITEM_BASE + 4
#define CIM_ITEM_RBUTTON_DCLICK				CIM_ITEM_BASE + 5
#define CIM_ITEM_RBUTTON_DOWN				CIM_ITEM_BASE + 6
#define CIM_ITEM_RBUTTON_UP					CIM_ITEM_BASE + 7
#define CIM_ITEM_SELECTED					CIM_ITEM_BASE + 8	// 현재 선택된 아이템 : Item에서 List로 전송, Item 내부의 컨트롤의 setfocus발생 시 마다 보낸다.
#define CIM_ITEM_DELETE						CIM_ITEM_BASE + 9
#define CIM_ITEM_EDITEND					CIM_ITEM_BASE + 10	// 현재 선택된 아이템 : Item에서 List로 전송, Item 내부의 컨트롤의 setfocus발생 시 마다 보낸다.


// Custom List Notify Code  
#define	NMCL_BASE							5000
#define NMCL_LBUTTON_DBLCLK					NMCL_BASE + 1
#define NMCL_LBUTTON_DOWN					NMCL_BASE + 2
#define NMCL_LBUTTON_UP						NMCL_BASE + 3
#define NMCL_RBUTTON_DBLCLK					NMCL_BASE + 4
#define NMCL_RBUTTON_DOWN					NMCL_BASE + 5
#define NMCL_RBUTTON_UP						NMCL_BASE + 6
#define NMCL_ITEM_BEGINDRAG					NMCL_BASE + 7
#define NMCL_ITEM_ENDDRAG					NMCL_BASE + 8
#define NMCL_ITEM_SELCHANGED				NMCL_BASE + 9 

#define	NMCL_EDIT_BASE						NMCL_BASE + 100
#define	NMCL_END_MOVEITEM					NMCL_EDIT_BASE + 1
#define	NMCL_END_COPYITEM					NMCL_EDIT_BASE + 2
#define	NMCL_END_DELITEM					NMCL_EDIT_BASE + 3
#define	NMCL_END_CHANGEITEM					NMCL_EDIT_BASE + 4


#define DO_UPSCROLL_AREA	20
#define DO_DOWNSCROLL_AREA	20
#define DO_LEFTSCROLL_AREA	20
#define DO_RIGHTSCROLL_AREA	20

enum
{
	HT_OUTOF_CTRL = 0,
	HT_LEFT_SCROLL = 1,
	HT_RIGHT_SCROLL = 2,
	HT_TOP_SCROLL = 3,
	HT_DOWN_SCROLL = 4,
	HT_IN_ITEM = 5,
};

#pragma pack(push,1)

struct CLNOTIFY_INFO
{
	NMHDR				hdr;
	DWORD_PTR			dwItemPtr;
	int					nIndex; // Vector에서의 인덱스 
};

struct CLNOTIFY_DRAGINFO
{
	NMHDR				hdr;
	CPoint				ptDropScreen;
	DWORD_PTR			dwDragItemPtr;
	int					nDragIndex; // Vector에서의 인덱스 0부터시작
	DWORD_PTR			dwTargetItemPtr;
	int					nTargetIndex; // Vector에서의 인덱스 
};

struct CLNOTIFY_SELCHANGEINFO
{
	NMHDR				hdr;
	DWORD_PTR			dwOldItemPtr;
	int					nOldIndex; // Vector에서의 인덱스  0부터시작
	DWORD_PTR			dwSelItemPtr;
	int					nSelIndex; // Vector에서의 인덱스 0부터시작
};

struct ST_HITTEST_RET
{
	BYTE btHitArea;
	BYTE btResv01;
	BYTE btResv02;
	BYTE btResv03;
	int	nIdx;
};

union SU_HITTEST_RET
{
	ST_HITTEST_RET	stHit;
	DWORD_PTR		dwHit;
};



#pragma pack(pop)