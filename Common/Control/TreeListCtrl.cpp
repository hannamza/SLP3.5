//*****************************************************************************
//*
//*
//*		TreeListCtrl.cpp
//*
//*
//*****************************************************************************
//
//	(C) Copyright Anton Zechner 2007
//
//	This code creates a Tree-List-Control for MFC
//
#if		   _MSC_VER > 1299
#ifndef		WINVER
#define		WINVER 	0x0501
#endif
#endif

#include	<Afx.h>
#include	<tchar.h>
#include	"TreeListCtrl.h"



IMPLEMENT_DYNAMIC(CTreeListCtrl, CWnd)

//*****************************************************************************
//*
//*		CreateEx
//*
//*****************************************************************************
//	Creates a new Tree-List-Control
//	dwExStyle	: Is the exdended style of the window (WS_EX_????)
//	dwStyle     : Style of the Window
//                	TVS_CHECKBOXES
//                	TVS_DISABLEDRAGDROP
//                	TVS_EDITLABELS
//                	TVS_FULLROWSELECT
//                	TVS_HASBUTTONS
//                	TVS_HASLINES
//                	TVS_INFOTIP
//                	TVS_LINESATROOT
//                	TVS_NONEVENHEIGHT
//                	TVS_NOSCROLL
//                	TVS_NOTOOLTIPS
//                	TVS_RTLREADING
//                	TVS_SHOWSELALWAYS
//                	TVS_SINGLEEXPAND
//                	TVS_TRACKSELECT
//	sRect		: Rectangle where the window should be shown
//	pParentWnd  : Pointer to parent window
//	nId         : Is the ID of the window
//	Returns TRUE if the window was created or FALSE if an error occurs
BOOL CTreeListCtrl::CreateEx(DWORD dwExStyle,DWORD dwStyle,const RECT &sRect,CWnd *pParentWnd,UINT nId)
{

    TreeListRegister(AfxGetInstanceHandle());

return CWnd::CreateEx(dwExStyle,_T("TreeList"),NULL,dwStyle|WS_HSCROLL|WS_VSCROLL,sRect,pParentWnd,nId);
}


//*****************************************************************************
//*
//*		CreateEx
//*
//*****************************************************************************
//	Creates a new Tree-List-Control over a dialog item
//	dwExStyle	: Is the exdended style of the window (WS_EX_????)
//	dwStyle     : Style of the Window
//                	TVS_CHECKBOXES
//                	TVS_DISABLEDRAGDROP
//                	TVS_EDITLABELS
//                	TVS_FULLROWSELECT
//                	TVS_HASBUTTONS
//                	TVS_HASLINES
//                	TVS_INFOTIP
//                	TVS_LINESATROOT
//                	TVS_NONEVENHEIGHT
//                	TVS_NOSCROLL
//                	TVS_NOTOOLTIPS
//                	TVS_RTLREADING
//                	TVS_SHOWSELALWAYS
//                	TVS_SINGLEEXPAND
//                	TVS_TRACKSELECT
//	uItem       : Is the ID of the dialog item
//	pDlg		: Pointer to parent dialog
//	nId			: Is the ID of the window
//	Returns TRUE if the window was created or FALSE if an error occurs
BOOL CTreeListCtrl::CreateEx(DWORD dwExStyle,DWORD dwStyle,UINT uItem,CDialog *pDlg,UINT nId)
{
CPoint	sPoint(0,0);
CRect	sRect;
CWnd   *pWnd;


	pWnd=pDlg->GetDlgItem(uItem);
	ASSERT(pWnd!=0);


	pWnd->GetWindowRect (&sRect);
	pDlg->ClientToScreen(&sPoint);

	sRect.left	  -= sPoint.x;
	sRect.right   -= sPoint.x;
	sRect.top	  -= sPoint.y;
	sRect.bottom  -= sPoint.y;


	if(!CTreeListCtrl::CreateEx(dwExStyle,dwStyle|WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL,sRect,pDlg,nId))
		{
		return FALSE;
		}
	
	SetWindowPos(pWnd,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW);


return TRUE;
}


//*****************************************************************************
//*
//*		InsertColumn
//*
//*****************************************************************************
//	Inserts a column in the header of the Tree-List-Control
//	nCol		: Position where the column should be inserted (use -1 for end)
//	pColumnText	: Text for the column
//	nFormat		: Ist the format for the text
//					TVCFMT_USERDRAW
//					TVCFMT_CENTER
//					TVCFMT_IMAGE
//					TVCFMT_RIGHT
//					TVCFMT_LEFT
//					TVCFMT_MARK
//	nWidth		: Is the width for the column in pixels.
//				  A negative value means is a autoexpanting column.
//				  The value of abs(nWidth) is the weight of the collumn.
//	nMin		: Ist the minimal size for the column.
//				  A negative value means abs(nMin)  the minimal size
//				  is only for dynamic size changing.
//				  Use 0x8000 for no minimal size
//	Returns the position of the column or -1 if an error occurs
int CTreeListCtrl::InsertColumn(int nCol,LPCTSTR pColumnText,int nFormat,int nWidth,int nMin)
{
TV_COLUMN	sItem;


	ASSERT(::IsWindow(m_hWnd));

	sItem.mask = 0;

	if(pColumnText)
		{
		sItem.mask		|=  TVCF_TEXT;
		sItem.pszText	 =  (LPTSTR)pColumnText;
		sItem.cchTextMax =  256;
		}

	if(nFormat>=0)
		{
		sItem.mask		|=  TVCF_FMT|TVCF_MARK|TVCFMT_OWNERDRAW;
		sItem.fmt		 =  nFormat;
		}

	if(nMin!=0x8000)
		{
		sItem.mask		|=  TVCF_MIN;
		sItem.iOrder	 =  nMin;
		}

	if(nWidth>=0)
		{
		sItem.mask		|=  TVCF_WIDTH;
		sItem.cx		 =  nWidth;
		}
	else{
		sItem.mask		|=  TVCF_VWIDTH;
		sItem.cx		 = -nWidth;
		}



return InsertColumn(nCol,&sItem);
}


//*****************************************************************************
//*
//*		SetMultiItems
//*
//*****************************************************************************
//	Changes some properties of several items item
//	hItem			: is the handle of the item
//	nCol			: is the column for the text (0=tree column)
//	nMask			: defies which properties should be set and which items:
//						TVIF_PARENTS      = change all parents items
//						TVIF_OWNITEM      = change own item
//						TVIF_SIBLINGS     = change all sibling items
//						TVIF_CHILDS	      = change all child items
//						TVIF_SAVESTATE	  = save states from nStateMask>>16
//						TVIF_RESTORESTATE = restore states from state high word
//						TVIF_...	      = see at TreeList documentation
//	pText			: is the new text of the item (add TVIF_TEXT to nMask)
//	nImage			: is the new image for the item (add TVIF_IMAGE to nMask)
//	nSelectedImage	: is the new image for the item (add TVIF_SELECTEDIMAGE to nMask)
//	nState			: is the new state for the item (add TVIF_STATE to nMask)
//	nStateMask		: is a mask for the states
//	lParam			: is the new LPARAM parameter for the item (add TVIF_PARAM to nMask)
//	Returns the count of changed items, or -1 if an error occurs.
BOOL CTreeListCtrl::SetMultiItems(HTREEITEM hItem,int nCol,UINT nMask,LPCTSTR pText,int nImage,int nSelectedImage,UINT nState, UINT nStateMask,LPARAM lParam)
{
TV_ITEM		sItem;


	ASSERT(::IsWindow(m_hWnd));

	sItem.mask				= nMask|TVIF_SUBITEM;
    sItem.hItem				= hItem;
    sItem.state				= nState;
    sItem.stateMask			= nStateMask;
    sItem.pszText			=(LPTSTR)pText;
    sItem.cchTextMax		=(pText)? 256:0;
    sItem.iImage			= nImage;
    sItem.iSelectedImage	= nSelectedImage;
    sItem.cChildren			= nCol;
	sItem.lParam			= lParam;


return SetMultiItems(&sItem);
}


//*****************************************************************************
//*
//*		ListInsertItem
//*
//*****************************************************************************
//	Inserts an item in a Tree-List-Control which is used as List-Control
//	iRow	: Is the row of the item
//	pText	: Is the text for the item
//	nImage	: Is the number for the icon (use TV_NOIMAGE for no icon)
//	nState	: Is the state of the image
//				TVIS_BOLD		 	= text is bolded
//				TVIS_UNTERLINE	 	= text is underlined
//				TVIS_SELECTED		= sItem is selected
//				TVIS_OVERLAYMASK	= overlay bits for image
//				TVIS_STATEIMAGEMASK = image for state icons
//	iMask	: Is the mask of bits which are used in the nState parameter
//				TVIS_BOLD		 	= text is bolded
//				TVIS_UNTERLINE	 	= text is underlined
//				TVIS_SELECTED	 	= sItem is selected
//				TVIS_OVERLAYMASK	= overlay bits for image
//				TVIS_STATEIMAGEMASK = image for state icons
//	Returns the insert position of the item or -1 if an error occurs
int CTreeListCtrl::ListInsertItem(int iRow,LPCTSTR pText,int nImage,int nState,int iMask)
{
HTREEITEM		hItem;
TVINSERTSTRUCT	sData;


	ASSERT(::IsWindow(m_hWnd));

	sData.item.mask	= 0;

	if(pText)
		{
		sData.item.mask		 |= TVIF_TEXT;
		sData.item.pszText	  = (LPTSTR)pText;
		sData.item.cchTextMax = 256;
		}

	if(nImage>=0)
		{
		sData.item.mask		     |= TVIF_SELECTEDIMAGE|TVIF_IMAGE ;
		sData.item.iImage		  = nImage;
		sData.item.iSelectedImage = nImage;
		}

	if(nState>=0)
		{
		sData.item.mask	    |= TVIF_STATE;
		sData.item.state	 = nState;
		sData.item.stateMask = iMask&(TVIS_BOLD|TVIS_UNTERLINE|TVIS_SELECTED|TVIS_OVERLAYMASK|TVIS_STATEIMAGEMASK);
		}


		hItem = GetItemOfRow(iRow);
	if(!hItem)
		{
		sData.hInsertAfter	= TVI_LAST;
		sData.hParent       = TVI_ROOT;
		}
	else{
		sData.hParent		= hItem;
		sData.hInsertAfter	= TVI_BEFORE;
		}

	hItem = InsertItem(&sData);


return GetRowOfItem(hItem);
}

//*****************************************************************************
//*
//*		ListSetItem
//*
//*****************************************************************************
//	Changes an item in a Tree-List-Control which is used as List-Control
//	iRow	: Is the row of the item
//	nCol	: Is the column of the item
//	pText	: Is the text for the item
//	nImage	: Is the number for the icon (use TV_NOIMAGE for no icon)
//	nState	: Is the state of the image
//				TVIS_BOLD			= text is bolded
//				TVIS_UNTERLINE		= text is underlined
//				TVIS_SELECTED		= sItem is selected
//				TVIS_OVERLAYMASK	= overlay bits for image
//				TVIS_STATEIMAGEMASK = image for state icons	(only for column 0)
//	iMask	: Is the mask of bits which are used in the nState parameter
//				TVIS_BOLD			= text is bolded
//				TVIS_UNTERLINE		= text is underlined
//				TVIS_SELECTED		= sItem is selected
//				TVIS_OVERLAYMASK	= overlay bits for image
//				TVIS_STATEIMAGEMASK = image for state icons (only for column 0)
//	Returns TRUE if ok or FALSE if an error occurs
BOOL CTreeListCtrl::ListSetItem(int iRow,int nCol,LPCTSTR pText,int nImage,int nState,int iMask)
{
TV_ITEM		sData;


	ASSERT(::IsWindow(m_hWnd));

	sData.mask		= TVIF_HANDLE|TVIF_SUBITEM;
	sData.hItem		= GetItemOfRow(iRow);
	sData.cChildren	= nCol;

	if(!sData.hItem)return FALSE;

	if(pText)
		{
		sData.mask		 |= TVIF_TEXT;
		sData.pszText	  = (LPTSTR)pText;
		sData.cchTextMax  = 256;
		}

	if(nImage>=0)
		{
		sData.mask		     |= TVIF_SELECTEDIMAGE|TVIF_IMAGE ;
		sData.iImage		  = nImage;
		sData.iSelectedImage  = nImage;
		}

	if(nState>=0)
		{
		sData.mask	    |= TVIF_STATE;
		sData.state	     = nState;
		sData.stateMask  = iMask&(TVIS_BOLD|TVIS_UNTERLINE|TVIS_SELECTED);
		}



return SetItem(&sData);
}


//*****************************************************************************
//*
//*		ListGetItemText
//*
//*****************************************************************************
//	Gets the text of an item and stores it in an buffer
//	iRow		: Row index of the item whose text is to be retrieved.
//	pBuffer		: Is the text buffer where the text will be saved
//	iMax		: Is the size of the text buffer in chars
//	nCol		: Is the column of the item
//	Returns a pointer to the text or NULL if an error occurs
BOOL CTreeListCtrl::ListGetItemText(int iRow,LPTSTR pBuffer,int iMax,int nCol)
{
TV_ITEM		sItem;
HTREEITEM	hItem;


	ASSERT(::IsWindow(m_hWnd));

	hItem = GetItemOfRow(iRow);

	if(!hItem || iMax<=0)return FALSE;

	pBuffer[0]		 = 0;
	sItem.mask		 = TVIF_TEXT|TVIF_SUBITEM;
	sItem.hItem		 = hItem;
	sItem.pszText	 = pBuffer;
	sItem.cchTextMax = iMax;
	sItem.cChildren	 = nCol;


return GetItem(&sItem);
}


//*****************************************************************************
//*
//*		FindItem
//*
//*****************************************************************************
//	Seaches for an Item in the control with several properties
//	hItem		: Is the item handle where the seach begins
//	iFlags		: This flag define the seach options
//					TVIF_CHILDS	= begin at the first child of hItem to seach
//					TVIF_NEXT	= begin at the next item after hItem to seach
//					TVIF_PARAM	= compare the the lParam  entry at the seach
//					TVIF_TEXT	= compare the the pText   entry at the seach
//					TVIF_STATE	= compare the the uState  entry at the seach
//					TVIF_CASE	= don't differ between lower and upper case
//	nCol		: Selects the column for the text comparison
//	pText		: Is the text option for the seach
//	lParam		: Is the param option for the seach
//	uState		: Are the state bits for the seach
//	uStateMask	: Only bits which are set in this mask will be comared
//	Retuns the handle of the item which was found, or NULL if no item was found
HTREEITEM CTreeListCtrl::FindItem(HTREEITEM hItem,int iFlags,int nCol,LPCTSTR pText,LPARAM lParam,UINT uState,UINT uStateMask)
{
TV_FIND		sFind;


	sFind.uFlags		= iFlags;
	sFind.uColumn		= nCol;
	sFind.pText			= pText;
	sFind.lParam		= lParam;
	sFind.uState		= uState;
	sFind.uStateMask	= uStateMask;


return FindItem(hItem,&sFind);
}


//*****************************************************************************
//*
//*		InsertItem
//*
//*****************************************************************************
//	Call this function to insert a new item in a tree list control.
//	nMask			: Integer specifying which attributes to set. See theTVITEM structure in the Platform SDK.
//	pText			: Address of a string containing the item’s text.
//	nImage			: Index of the item’s image in the tree view control’s image list.
//	nSelImage		: Index of the item’s selected image in the tree view control’s image list.
//	nState			: Specifies values for the item’s states. SeeTree View Control Item States in the Platform SDK for a list of appropriate states.
//	nStateMask		: Specifies which states are to be set. See theTVITEM structure in the Platform SDK.
//	lParam			: A 32-bit application-specific value associated with the item.
//	hParent			: Handle of the inserted item’s parent.
//	hInsertAfter	: Handle of the item after which the new item is to be inserted.
//	Returns the handle of the new item if successful, otherwise NULL.
HTREEITEM CTreeListCtrl::InsertItem(UINT nMask, LPCTSTR pText,int nImage,int nSelImage,UINT nState,UINT nStateMask,LPARAM lParam,HTREEITEM hParent,HTREEITEM hInsertAfter)
{
TVINSERTSTRUCT	sInsert;


	ASSERT(::IsWindow(m_hWnd));

	sInsert.hParent				= hParent;
	sInsert.hInsertAfter		= hInsertAfter;
	sInsert.item.mask			= nMask;
	sInsert.item.pszText		= (LPTSTR)pText;
	sInsert.item.iImage			= nImage;
	sInsert.item.iSelectedImage = nSelImage;
	sInsert.item.state			= nState;
	sInsert.item.stateMask		= nStateMask;
	sInsert.item.lParam			= lParam;


return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEM,0,(LPARAM)&sInsert);
}

//*****************************************************************************
//*
//*		InsertItem
//*
//*****************************************************************************
//	Call this function to insert a new item in a tree list control, with a sort fonction.
//	nMask			: Integer specifying which attributes to set. See theTVITEM structure in the Platform SDK.
//	pText			: Address of a string containing the item’s text.
//	nImage			: Index of the item’s image in the tree view control’s image list.
//	nSelImage		: Index of the item’s selected image in the tree view control’s image list.
//	nState			: Specifies values for the item’s states. SeeTree View Control Item States in the Platform SDK for a list of appropriate states.
//	nStateMask		: Specifies which states are to be set. See theTVITEM structure in the Platform SDK.
//	lParam			: A 32-bit application-specific value associated with the item.
//	pCmpProc		: Is the sort function which is used to find the insert position
//	hParent			: Handle of the inserted item’s parent.
//	Returns the handle of the new item if successful, otherwise NULL.
HTREEITEM CTreeListCtrl::InsertItem(UINT nMask, LPCTSTR pText,int nImage,int nSelImage,UINT nState,UINT nStateMask,LPARAM lParam,PFNTVSORTEX pCmpProc,HTREEITEM hParent)
{
TVINSERTSTRUCT	sInsert;


	ASSERT(::IsWindow(m_hWnd));

	sInsert.hParent				= hParent;
	sInsert.hInsertAfter		= TVI_SORTEX;
	sInsert.item.hItem			= (HTREEITEM)pCmpProc;
	sInsert.item.pszText		= (LPTSTR)pText;
	sInsert.item.mask			= nMask;
	sInsert.item.iImage			= nImage;
	sInsert.item.iSelectedImage = nSelImage;
	sInsert.item.state			= nState;
	sInsert.item.stateMask		= nStateMask;
	sInsert.item.lParam			= lParam;


return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEM,0,(LPARAM)&sInsert);
}

//*****************************************************************************
//*
//*		SetColumnText
//*
//*****************************************************************************
//	Sets the text of an item in the column header
//	nCol	: is the number of the column
//	pText	: is the new text
//	Retuns TRUE if successful
BOOL CTreeListCtrl::SetColumnText(int nCol,LPCTSTR pText)
{
TV_COLUMN	sData;


	sData.pszText		= (LPTSTR)pText;
	sData.cchTextMax	= (int)_tcslen(pText);
	sData.mask			= TVCF_TEXT;


return	SetColumn(nCol,&sData);
}

//*****************************************************************************
//*
//*		SetColumnImage
//*
//*****************************************************************************
//	Sets the image in the column header.
//	nCol	: is the number of the column
//	iImage	: is the new image number
//	iPlace	: is the pace for the image (0=left of text 1=right of text)
//	Retuns TRUE if successful
BOOL CTreeListCtrl::SetColumnImage(int nCol,int iImage,int iPlace)
{
CHeaderCtrl	   *pHeader;
HDITEM			sItem;


		pHeader	= GetHeaderCtrl();
	if(!pHeader)return FALSE;

	sItem.mask	= HDI_FORMAT;

	pHeader->GetItem(nCol,&sItem);

	if(iImage<-1)
			sItem.fmt&=~HDF_IMAGE;
	else	sItem.fmt|= HDF_IMAGE;

	if(iPlace&1)
			sItem.fmt|= HDF_BITMAP_ON_RIGHT;
	else	sItem.fmt&=~HDF_BITMAP_ON_RIGHT;

	sItem.mask	|= HDI_IMAGE;
	sItem.iImage = iImage;


return pHeader->SetItem(nCol,&sItem);
}

//*****************************************************************************
//*
//*		SetColumnBitmap
//*
//*****************************************************************************
//	Sets the bitmap in the column header.
//	nCol	: is the number of the column
//	hBmp	: is the bitmap handle
//	iPlace	: is the pace for the bitmap (0=left of text 1=right of text)
//	Retuns TRUE if successful
BOOL CTreeListCtrl::SetColumnBitmap(int nCol,HBITMAP hBmp,int iPlace)
{
CHeaderCtrl	   *pHeader;
HDITEM			sItem;


		pHeader	= GetHeaderCtrl();
	if(!pHeader)return FALSE;

	sItem.mask  = HDI_FORMAT;

	pHeader->GetItem(nCol,&sItem);

	if(hBmp==0)
			sItem.fmt&=~HDF_BITMAP;
	else	sItem.fmt|= HDF_BITMAP;

	if(iPlace&1)
			sItem.fmt|= HDF_BITMAP_ON_RIGHT;
	else	sItem.fmt&=~HDF_BITMAP_ON_RIGHT;

	sItem.mask |= HDI_BITMAP;
	sItem.hbm	= hBmp;


return pHeader->SetItem(nCol,&sItem);
}

//*****************************************************************************
//*
//*		SetColumnMark
//*
//*****************************************************************************
//	Sets the mark state of a column. A marked column will be drawn darker.
//	nCol	: is the number of the column
//	bOn		: is the mark state
//	Returns TRUE if successful
BOOL CTreeListCtrl::SetColumnMark(int nCol,BOOL bOn)
{
TV_COLUMN	sData;


	sData.fmt	= (bOn)? TVCFMT_MARK:0;
	sData.mask	= TVCF_MARK;


return	SetColumn(nCol,&sData);
}

//*****************************************************************************
//*
//*		SetColumnOwnerDraw
//*
//*****************************************************************************
//	Sets the user draw option of a column.
//	nCol	: is the number of the column
//	bOn		: is the draw state
//	Returns TRUE if successful
BOOL CTreeListCtrl::SetColumnOwnerDraw(int nCol,BOOL bOn)
{
TV_COLUMN	sData;


	sData.fmt	= (bOn)? TVCFMT_OWNERDRAW:0;
	sData.mask	= TVCF_OWNERDRAW;


return	SetColumn(nCol,&sData);
}

//*****************************************************************************
//*
//*		FixColumnSize
//*
//*****************************************************************************
//	Fixes the size of a a column. The size of a fixed column can't be changed.
//	nCol	: is the number of the column
//	bOn		: is the fixed state
//	iWidth	: is the new width for the column (-1 means don't change width)
//			  TVCF_LASTSIZE restore size befor fixing
//	iRecalc	: Sollen die variablen Spaltenbreiten neu berechnet
//				-1 = nein
//				 0 = nur die Änderung
//				 1 = alle neu setzen
//	Returns TRUE if successful
BOOL CTreeListCtrl::FixColumnSize(int nCol,BOOL bOn,int iWidth,int iRecalc)
{
LV_COLUMN	sData;


	sData.fmt	= (bOn)? TVCFMT_FIXED:0;
	sData.mask	= TVCF_FIXED;

	if(iWidth>=0)										// set also the column width
		{
		sData.mask |= TVCF_WIDTH;
		sData.cx    = iWidth;
		}

	if(iRecalc>=0)sData.mask|=TVCF_RECALC;
	if(iRecalc> 0)sData.fmt |=TVCFMT_RECALC;


return	SetColumn(nCol,&sData);
}

//*****************************************************************************
//*
//*		CreateDragImage
//*
//*****************************************************************************
CImageList *CTreeListCtrl::CreateDragImage(HTREEITEM hItem,int nCol)
{
CImageList *pImageList;
HIMAGELIST	hImageList;


	ASSERT(::IsWindow(m_hWnd));

		hImageList = TreeList_CreateDragImageEx(m_hWnd,hItem,nCol);
	if(!hImageList)
		{
		return NULL;
		}

	pImageList = new CImageList;
	VERIFY(pImageList->Attach(hImageList));

return pImageList;
}

//*****************************************************************************
//*
//*		InsertCmp
//*
//*****************************************************************************
int CALLBACK CTreeListCtrl::InsertCmp(HWND hWnd,HTREEITEM hItem,LPCTSTR pTextItem,LPCTSTR pTextInsert,LPARAM lParamItem,LPARAM lParamInsert)
{
PFNTVTEXTCMP	pCmp=(PFNTVTEXTCMP)lParamInsert;

	hWnd;
	hItem;
	lParamItem;

return pCmp(pTextInsert,pTextItem);
}
