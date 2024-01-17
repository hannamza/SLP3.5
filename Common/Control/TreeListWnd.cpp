//*****************************************************************************
//*
//*
//*		TreeListWnd.cpp
//*
//*
//*****************************************************************************
//
//	Dieser Code erstellt ein Tree-Fenster mit einer Liste
//
//
//	Copyright (C) Anton Zechner 2007
//
//	TreeListWnd is distributed under the GNU GENERAL PUBLIC LICENSE (GPL)
//	Sourcecode which use TreeListWnd must be published. Commercial users
//	must published their code too, or make an licence agreement with me.
//
//
//	TreeListWnd wird unter GNU GENERAL PUBLIC LICENSE (GPL) vertreiben.
//	Sourcecode welcher TreeListWnd verwendet muss ver?fendlicht werden.
//	Komerzielle Nutzer m?sen ihren Code ebenfalls ver?fentlichen, oder
//	eine Nutzungsvereinbarung mit mir treffen.
//
//	az_software@inode.at
//
//	Version:	2.33
//
#if		   _MSC_VER > 1299
#ifndef		WINVER
#define		WINVER 	0x0501
#endif
#endif

#ifdef		UNICODE
#ifndef	   _UNICODE
#define    _UNICODE
#endif
#endif

#include	<stdio.h>

#include	<windows.h>
#include	<commctrl.h>
#include	<string.h>
#include	<malloc.h>
#include	<tchar.h>

#include	"TreeListWnd.h"

#ifdef	   _MSC_VER
#pragma		warning( disable :4996 )
#endif

#ifndef		GWLP_USERDATA
#define 	GWLP_USERDATA		GWL_USERDATA
#endif
#ifndef 	GWLP_WNDPROC
#define 	GWLP_WNDPROC		GWL_WNDPROC
#endif
#ifndef	   _WIN64
#ifndef		SetWindowLongPtr
#define		SetWindowLongPtr	SetWindowLong
#endif
#ifndef		GetWindowLongPtr
#define		GetWindowLongPtr	GetWindowLong
#endif
#ifndef		DWORD_PTR
#define		DWORD_PTR			DWORD
#endif
#ifndef		LONG_PTR
#define		LONG_PTR			LONG
#endif
#endif
#ifdef		UNICODE
#define 	str_len				(unsigned)wcslen
#define 	str_cmp				wcscmp
#define 	str_ncpy			wcsncpy
#define 	str_ncmp			wcsncmp
#define 	str_icmp		   _wcsicmp
#else
#define 	str_len				(unsigned)strlen
#define 	str_cmp				strcmp
#define 	str_ncpy			strncpy
#define 	str_ncmp			strncmp
#define 	str_icmp		   _stricmp
#endif
#ifndef		WM_MOUSEWHEEL
#define		WM_MOUSEWHEEL		0x020A
#endif
#ifndef		WHEEL_DELTA
#define		WHEEL_DELTA			120
#endif
#ifndef		MAX_COLUMNS
#define 	MAX_COLUMNS			32
#endif
#define 	MOVE_MARK			3
#define		MAX_COLORS			18
#define 	EN_SETTEXT			0x1000
#define 	EN_RETURN			0x1578
#define 	EN_ESCAPE			0x1579
#define 	ID_AUTOSCROLL		0x3913
#define 	ID_TOOLTIPCHECK		0x3912
#define 	SORT_NOUPDATE		1234567
#define 	VK_ISACHAR			0x01000000
#define 	FIRST_LINE			0xFFFFFFFE
#define 	FROM_HEADER			0x88776655
#define 	I_CCB				I_CHILDRENCALLBACK
#define 	U(h)				((unsigned)(h))
#define 	THEMEIMGLIST		((HIMAGELIST)1)
#define 	GetHandle(h)		((TreeListData*)GetWindowLongPtr(h,0))
#define 	TVIF_ALL			(TVIF_CHILDREN|TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT)
#define 	SYNCH_TYPE			CRITICAL_SECTION
#define 	SYNCH_CREATE(d)		InitializeCriticalSection(&d->sSynch);
#define 	SYNCH_DELETE(d)		DeleteCriticalSection(&d->sSynch);
#define 	UNLOCK(d)			{													\
								if(d->sSynch.LockCount==0xFFFFFFFF)					\
									{												\
									char aLine[16];									\
									sprintf(aLine,"%i",__LINE__);					\
									MessageBoxA(0,aLine,"Unlock Error",MB_OK);		\
									}												\
								LeaveCriticalSection(&d->sSynch);					\
								}
#define 	LOCK(d)				EnterCriticalSection(&d->sSynch)
#define 	TVIS_EDIT(m)		((1<<m)&((1<<TVAX_EDIT)|(1<<TVAX_COMBO)|(1<<TVAX_STEPED)|(1<<TVAX_CHECKED)))
#define 	TVIS_BASEFLAGS		(TVIS_EXPANDED|TVIS_EXPANDEDONCE|TVIS_EXPANDPARTIAL|TVIS_SELECTED)
#define 	TVC_ONLYFOCUS		TVIF_ONLYFOCUS
#define 	TVC_UNSELECT		0x4000
#define 	TVC_DESELECT		0x8000
#define 	DEFAULT_IDENT		19
#define 	DEFAULT_SHIFT		7
#ifndef 	IDC_HAND
#define		IDC_HAND			MAKEINTRESOURCE(32649)
#endif
#ifndef		BPBF_COMPATIBLEBITMAP
#define 	BPBF_COMPATIBLEBITMAP	0
#endif
#ifndef		TVP_GLYPH
#define		TVP_GLYPH			2
#endif
#ifndef		GLPS_CLOSED
#define		GLPS_CLOSED			1
#endif
#ifndef		GLPS_OPENED
#define		GLPS_OPENED			2
#endif
#ifndef		BP_CHECKBOX
#define		BP_CHECKBOX			3
#endif
#ifndef		CBS_UNCHECKEDNORMAL
#define		CBS_UNCHECKEDNORMAL 1
#endif
#ifndef		CBS_UNCHECKEDHOT
#define		CBS_UNCHECKEDHOT	2
#endif
#ifndef		CBS_UNCHECKEDPRESSED
#define		CBS_UNCHECKEDPRESSED 3
#endif
#ifndef		CBS_UNCHECKEDDISABLED
#define		CBS_UNCHECKEDDISABLED 4
#endif
#ifndef		CBS_CHECKEDNORMAL
#define		CBS_CHECKEDNORMAL	5
#endif
#ifndef		CBS_CHECKEDHOT
#define		CBS_CHECKEDHOT		6
#endif
#ifndef		CBS_CHECKEDPRESSED
#define		CBS_CHECKEDPRESSED	7
#endif
#ifndef		CBS_CHECKEDDISABLED
#define		CBS_CHECKEDDISABLED	8
#endif
#ifndef		GET_X_LPARAM
#define		GET_X_LPARAM(x)		((short)LOWORD(x))
#endif
#ifndef		GET_Y_LPARAM
#define		GET_Y_LPARAM(x)		((short)HIWORD(x))
#endif
#ifndef		DC_BRUSH
#define 	DC_BRUSH			18
#endif
#ifndef		DC_PEN
#define		DC_PEN				19
#endif
#ifdef		DrawText
#undef 		DrawText
#define 	DrawText(h,p,n,r,f)	DrawTextEx(h,(LPTSTR)(p),n,r,f,0)
#endif


#define 	TVAX_NONE			(TVAE_NONE   >>TVAE_MODEPOS)// Kein atomatisches editieren
#define 	TVAX_EDIT			(TVAE_EDIT	 >>TVAE_MODEPOS)// Atomatisches editieren mit Edit-Fenster
#define 	TVAX_COMBO			(TVAE_COMBO	 >>TVAE_MODEPOS)// Atomatisches editieren mit ComboBox
#define 	TVAX_CBLIST			(TVAE_CBLIST >>TVAE_MODEPOS)// Atomatisches editieren mit ComboListBox
#define 	TVAX_STEP			(TVAE_STEP	 >>TVAE_MODEPOS)// Einzelnes Weiterschalten mit Enter
#define 	TVAX_STEPED			(TVAE_STEPED >>TVAE_MODEPOS)// Einzelnes Weiterschalten mit Enter und Edit
#define 	TVAX_CHECK			(TVAE_CHECK	 >>TVAE_MODEPOS)// Atomatisches editieren mit CheckBox
#define 	TVAX_CHECKED		(TVAE_CHECKED>>TVAE_MODEPOS)// Atomatisches editieren mit CheckBox und Edit

#define 	TVIX_VARBUTTON		0x01						// Schaltfl?hen sind nicht fix eingestellt
#define 	TVIX_HASBUTTON		0x02						// Der Eintrag hat eine Schaltfl?he
#define 	TVIX_HASIMAGE		0x04						// Der Eintrag hat ein Icon
#define 	TVIX_TRACKED		0x08						// Der Eintrag unter dem Cursor
#define 	TVIX_TEXTCOLOR		0x10						// Der Eintrag hat eine eigene Textfarbe
#define 	TVIX_BKCOLOR		0x20						// Der Eintrag hat eine eigene Hintergrundfarbe
#define 	TVIX_FOCUSED		0x40						// Der Eintrag hat einen Eingabe-Focus
#define 	TVIX_LINE			0x80						// Der Eintrag ist nicht der letzte seiner Ebene

#define 	UPDATE_STATE		0x01						// Status eines Eintrages setzen
#define 	UPDATE_INSERT		0x02						// Status eines Eintrages einf?en
#define 	UPDATE_EXPAND		0x03						// Einen Eintrag expantieren
#define 	UPDATE_UNSEL		0x04						// Notify die einen Eintrag abw?lt
#define 	UPDATE_SCROLL		0x05						// SetScrollPos () aufrufen
#define 	UPDATE_SCRLX		0x06						// UpdateScrollX() aufrufen
#define 	UPDATE_SCRLY		0x07						// UpdateScrollY() aufrufen
#define 	UPDATE_ITEMS		0x08						// UpdateItems  () aufrufen
#define 	UPDATE_RECT			0x09						// UpdateRect   () aufrufen
#define 	UPDATE_VIEW			0x0A						// UpdateView   () aufrufen

#if			TVIS_TRACKED	!=	(TVIX_TRACKED<<16)
#error		TVIS_TRACKED has wrong value
#endif
#if			TVIS_BKCOLOR	!=	(TVIX_BKCOLOR<<16)
#error		TVIS_BKCOLOR has wrong value
#endif
#if			TVIS_FOCUSED	!=	(TVIX_FOCUSED<<16)
#error		TVIS_FOCUSED has wrong value
#endif
#if			TVIS_TEXTCOLOR	!=	(TVIX_TEXTCOLOR<<16)
#error		TVIS_TEXTCOLOR has wrong value
#endif

typedef struct
	{
	void	   *pNext;										// Zeiger auf den n?hsten Block
	WPARAM		wParam;										// WPARAM f? den Aufruf
	LPARAM		lParam;										// LPARAM f? den Aufruf
	unsigned	uType;										// Art des Aufrufes
	}UpdateData;

typedef struct
	{
	LPARAM		lParam;										// Ist der LPARAM Wert f? den Eintrag
	LPTSTR		pText;										// Zeiger auf Tree-Text
	UINT		uState;										// Zustand des Eintrages
	int			iImage;										// Ist die Nummer des an zu zeigenden Icons
	int			iSelectedImage; 							// Ist die Nummer des an zu zeigenden Icons (wenn ausgew?lt)
	int 		iTextPixels;								// Ist die Breites des Textes in Pixel
	unsigned	uShowPos;									// Ist die Position in der Sichtbarliste (0=unsichtbar)
	unsigned	uFirstChild;								// Ist die Nummer des ersten Kind-Eintrages (0=keines)
	unsigned	uLastChild;									// Ist die Nummer des letzten Kind-Eintrages (0=keines)
	unsigned	uPrevItem;									// Ist die Nummer des vorherigen Eintrages (0=keines)
	unsigned	uNextItem; 									// Ist die Nummer des n?hsten Eintrages (0=keines)
	unsigned	uParent; 									// Ist die Nummer des Elterneintrages (0=Root)
	WORD		uLevel;										// Ist die Ebene des Eintrages (0=Root)
	WORD		uTextSize;									// L?ge des Textes in Zeichen
	BYTE		bCallback;									// Sind Bits f? Callbacks
	BYTE		bFlags;										// Diverse Flags
	BYTE		bDone;										// Markierflags f?s Einf?en
	BYTE		dDummy;										// Unbenutzt
	COLORREF	uColorText;									// Spezielle Textfarbe
	COLORREF	uColorBk;									// Spezielle Hintergrundfarbe
	}BaseItem;

typedef struct
	{
	LPTSTR		pText;										// Zeiger auf Tree-Text
	UINT		uState;										// Zustand des Eintrages
	int			iImage;										// Ist die Nummer des an zu zeigenden Icons
	int 		iTextPixels;								// Ist die Breites des Textes in Pixel
	WORD		uTextSize;									// L?ge des Textes in Zeichen
	BYTE		bCallback;									// Sind Bits f? Callbacks
	BYTE		bFlags;										// Diverse Flags
	COLORREF	uColorText;									// Spezielle Textfarbe
	COLORREF	uColorBk;									// Spezielle Hintergrundfarbe
	}ExtraItem;

typedef struct
	{
	void 	   *pCbData;									// Daten f? das Auto-Editieren
	INT  	    iCbIcon;									// Startoffset f? in Iconliste f? Autoedit
	short 		sSize;										// Ist die soll Breite einer Spalte
	short 		sReal;										// Ist die wirkliche Breite einer Spalte
	short 		sMin;										// Ist die Ausrichtung einer Spalte
	short  	    sFixed;										// Ist die Spalte fixiert (ist die Breite vor dem Fixieren)
	BYTE		bMinEx;										// Darf die Spaltenbreite auch nicht vom User unterschritten werden
	BYTE		bWeight;									// Gewicht der variable Spalten
	BYTE		bNext;										// Ist die Spalte die nach der eigenen sichtbar ist	(gespeicherte Reihenfolge)
	BYTE		bIndex;										// Ist die Spalte in der diese Reihe sichtbar ist (sichtbarer Index)
	BYTE  	    bAlign;										// Text ausrichtung f? die Spalten
	BYTE  	    bEdit;										// Automaisches Editiern einer Spalte (siehe TVAE_???>>7)
	BYTE  	    bFlags;										// Automaisches Editiern einer Spalte (siehe TVAE_???)
	BYTE  	    bEnable;									// Automaisches einer mit Statebits aktivieren
	BYTE  	    bCbSize;									// Maximale Anzahl der Eintr?e in der Datenliste
	BYTE  	    bCbChar;									// Trennzeichnen f? die Datenliste
	BYTE  	    bMark;										// Ist die Spalte markiert
	BYTE  	    bDraw;										// Soll die Spalte via Callback gezeichnet werden
	//BYTE  	    bDummy[32 - 24 - sizeof(void*)];				// F?lbytes auf 32 Grenze
	BYTE  	    bDummy[32 - 24 - 4];				// F?lbytes auf 32 Grenze
	}ColumnData;

typedef struct
	{
	HWND		hWnd;										// Fenster Handle
	SYNCH_TYPE	sSynch;										// Zugriffsbeschgr?kung
	LPVOID		hTheme;										// Handle f? benutztes Thema (TREELIST)
	LPVOID		hThemeBt;									// Handle f? benutztes Thema (BUTTON)
	LPVOID		pDrawParam;									// Parameter f? das Zeichen von Eintr?en
	PFNTVDRAW	pDrawItem;									// Callback f?s Zeichnen von eintr?en
	WNDPROC		pProcId3;									// Fenster Funktion f? ID3 Fenster
	HIMAGELIST	hStates;									// Handle der Icon-Liste f? States und Overlay
	HIMAGELIST	hImages;									// Handle der Icon-Liste
	HIMAGELIST	hChecks;									// Handle der Icon-Liste f? die Checkboxen in den Spalten
	HIMAGELIST	hSubImg;									// Handle der Icon-Liste f? die Spalten
	HIMAGELIST	hHdrImg;									// Handle der Icon-Liste f? den Header
	HFONT		hFontN;										// Ist der normale Zeichensatz
	HFONT		hFontB;										// Ist der fette Zeichensatz
	HFONT		hFontL;										// Der letzte zugewiesene Font
	HFONT		hFontX;										// Der letzte erzeugte fette Font
	HFONT		hFontT;										// Ist der Font f? das Tooltip
	HWND		hEdit;										// Handle des Edit-Fensters
	HWND		hHeader;									// Handle des Header Fensters
	HWND		hToolTip;									// Handle des Tooltip-Fensters
	WNDPROC		pToolProc;									// Alte Fensterfunktion des Tooltips
	COLORREF	aColors[MAX_COLORS];						// 0=Hintergrundfarbe  1=Abwechselnte Farbe  2=Farbe f? Trennlinien  3=Textfarbe
	int			iFontHeight;								// Ist die H?e des Fonts
	int			iFontLine;									// Ist die Position der Linie beim unterstreichen
	int			iFontOff;									// Ist die Position um der ein Text horizontal verschoben wird
	int			iStatesMode;								// Die hStates Image-Liste wurde f? die Checkboxen erzeugt
	int			iStatesSizeX;								// Breite der States und Overlay Icons
	int			iStatesSizeY;								// H?e   der States und Overlay Icons
	int			iChecksMode;								// Die hChecks Image-Liste wurde f? die Checkboxen erzeugt
	int			iChecksSizeX;								// Breite der States und Overlay Icons
	int			iChecksSizeY;								// H?e   der States und Overlay Icons
	int			iImagesSizeX;								// Breite der Icons
	int			iImagesSizeY;								// H?e   der Icons
	int			iSubImgMode;								// Die SubImg Image-Liste ist nicht die hImages Liste
	int			iSubImgXsize;								// Breite der Icons
	int			iSubImgYsize;								// H?e   der Icons
	int			iRowHeight;									// Ist die H?e einer Zeile
	int			iVarSize;									// Ist die Breite aller variablen Spalten
	int			iFixSize;									// Ist die Breite aller fixen Spalten
	int			iIndent;									// Einr?kung der Kindereint?e
	int			iShift;										// Einr?kung der vertikalen Linien
	int			iAutoAdd;									// Offset zum Open-Icon  f? TVS_EX_AUTOEXPANDICON
	int			iMaxSizeX;									// Die Größe des breitesten sichtbaren Eintrages
	int			iUnderline;									// L?genver?derung f? Unterline
	LPINT		pTempPos;									// Tempor?ere Positionen
	LPTSTR		pTempPtr;									// Tempor?er Text
	unsigned	uTempSize;									// Größe vom Tempor? Speicher
	unsigned	uItemPosCount;								// Anzahl der sichtbaren Eintr?e
	unsigned   *pItemPos;									// Liste mit den Offsets der sichtbaren Eintr?e
	UpdateData *pUpdateFirst;								// Anfang der Liste f? ein Update
	UpdateData *pUpdateLast;								// Ende der Liste f? ein Update
	BaseItem  **pTreeItems;									// Zeiger auf Item Zeiger
	ExtraItem **pExtraItems[MAX_COLUMNS-1];					// Zeiger auf die Spalteneintr?e
	unsigned	uTreeItemsMax;								// Größe der Liste mit den vorhanden Eintr?e (alociert um 1 größer)
	unsigned	uTreeItemsCount;							// Anzahl der vorhanden Eintr?e
	unsigned	uNextSearchPos;								// N?hste Position zum suchen von freien Eintr?en
	unsigned	uUserDataSize;								// Ist die Größe der Userdaten in einem Eintrag
	unsigned	uFirstChild;								// Ist die Nummer des ersten Kind-Eintrages (0=keines)
	unsigned	uLastChild;									// Ist die Nummer des letzten Kind-Eintrages (0=keines)
	unsigned	uSingleSel;									// Ist die Nummer des gew?lten Eintrages (bei Checkboxen)
	unsigned	uScrollX;									// Aktuelle X-Scroll-Position
	unsigned	uScrollY;									// Aktuelle Y-Scroll-Position
	unsigned	uSizeX;										// Aktuelle X-Fenster-Größe
	unsigned	uSizeY;										// Aktuelle Y-Fenster-Größe
	unsigned	uSizeYsub;									// Aktuelle Y-Fenster-Größe ohne Header
	unsigned	uStyle;										// Ist der aktuele Style des Fensters
	unsigned	uStyleEx;									// Erweiterte Sytle-Flags (siehe TVS_EX_???)
	unsigned	uStartPixel;								// Ist die Y-Koordinate bei der der erste Eintrag beginnt
	unsigned	uMaxEnties;									// Anzahl der sichtbaren Eintr?e (inkl. halbsichtbare)
	unsigned	uPageEnties;								// Anzahl der sichtbaren Eintr?e (ohne  halbsichtbare)
	unsigned	uFixedLines;								// Anzahl der fixierten Zeilen am oberen Ende
	unsigned	uFixedAlways;								// Wird auf 0x80000000 gesetzt um fixe Trennug immer anzuzeigen
	unsigned	uColumnCount;								// Anzahl der Spalten
	unsigned	uColumnCountVar;							// Anzahl der variabeln Spalten
	unsigned	uSelectedCount;								// Anzahl der ausgew?lten Eintr?e
	unsigned	uSelectedBase;								// Ist der Eintrag ab dem gew?lt wurde
	unsigned	uSelectedItem;								// Ist der Eintrag der gerade gew?lt ist
	unsigned	uSelectedSub;								// Ist die Spalte die gerade gew?lt ist
	unsigned	uCheckedDown;								// Ist die Checkbox gerade gedr?kt
	unsigned	uCheckedItem;								// Ist die Checkbox ?er der der Corsor steht
	unsigned	uCheckedSub;								// Ist die Spalte der Checkbox ?er der der Corsor steht
	unsigned	uFocusItem;									// Ist der Eintrag der einen leeren Focus hat
	unsigned	uFocusSub;									// Ist die Spalte die einen leeren Focus hat
	unsigned	uMoveItem;									// Ist der Starteintrag der Auto-Drag-Move Operation
	unsigned	uMoveFlags;									// Diverse Flags f? Drag-Move-Operationen (TVDM_????)
	unsigned	uMoveLastW;									// Letzter WPARAM vom Drag-Move-Operationen
	unsigned	uMoveLastL;									// Letzter LPARAM vom Drag-Move-Operationen
	unsigned	uToolTipItem;								// Ist der ToolTip-Eintrag der gerade gew?lt ist
	unsigned	uToolTipShow;								// Ist die Zeitverz?erung in 500 ms Schritten f? das Tooltip
	unsigned	uToolTipSub;								// Ist die ToolTip-Spalte die gerade gew?lt ist
	POINT		sToolTipPos;								// Ist die globale Koordinate des ToolTips
	unsigned	uEditMode;									// Ist der Modus des Editfensters (0=Edit 1=ComboBox 2=ComboBox fix)
	unsigned	uEditItem;									// Ist der Eintrag der gerade editiert wird
	unsigned	uEditSub;									// Ist die Spalte die gerade editiert wird
	unsigned	uOldPageX;									// Alte Werte f? X-Scroll-Bar
	unsigned	uOldCountX;									//			*
	unsigned	uOldPageY;									// Alte Werte f? Y-Scroll-Bar
	unsigned	uOldCountY;									//			*
	unsigned	uTrippleB;									// Bereite des "..." Strings f? den fetten Fonts
	unsigned	uTrippleN;									// Bereite des "..." Strings f? den normalen Fonts
	unsigned	uTrackedItem;								// Ist der Eintrag der unterstrichen werden soll
	unsigned	uTrackedSub;								// Ist die Spalte des Eintrages der unterstrichen werden soll
	unsigned	uInsertMark;								// Ist der Eintrag mit der Einf?emarke
	unsigned	uMarkedCols;								// Anzahl der markierten Spalten
	unsigned	uDragFlags;									// Welche Maustasten sind an
	unsigned	uDragItem;									// Eintrag f? Dragoperation
	unsigned	uDragSub;									// Untereintrag f? Dragoperation
	unsigned	uNoDrag;									// Maske um Drag&Drop Operation zu sperren
	unsigned	uLastSel;									// Letzte Textauswahl beim Editieren
	unsigned	uLastMove;									// Letzte Cursorposition bei WM_MOUSEMOVE
	unsigned	uLineItem;									// Eintrag auf dem die Markierungslinie gezeichnet wird
	unsigned	uLineHeight;								// H?e der Markierungslinie (0=Zeilenh?e)
	unsigned 	uButtonPos;									// Wo wurde eine Maustaste wurde zuletzt gedr?kt
	unsigned 	uButtonLast;								// Wann wurde eine Maustaste wurde zuletzt gedr?kt
	unsigned	uToolTipSize;								// Textspeichergröße f? Tooltip
	LPTSTR		pToolTipText;								// Textspeicher f? Tooltip
	TCHAR		cTempText1   [260];							// Erster Textpuffer f? Callbacks
	TCHAR		cTempText2   [260];							// Zeiter Textpuffer f? Callbacks
	ColumnData	aColumn      [MAX_COLUMNS];					// Daten der Spalten
	int			aColumnXpos  [MAX_COLUMNS+2];				// Array mit den Positionen der Spalten
	BYTE		aColumnPos   [MAX_COLUMNS+2];				// Array mit Anzeigepositionen der Spalten
	char		aColorChanged[MAX_COLORS ];					// Welche Farbe wurden ver?dert
	char		cEditClickMode;								// Editklickoptionen (Bit0=nur auf Text, Bit1=direkt ohne Auswahl)
	char   	    cColumnStart;								// Wurde das Autoeditiren mit einer WM_CHAR Eingabe gestartet
	char 		cFixedHeight;								// Ist eine fixe H?e eingestellt
	char		cLockChanges;								// Sperren von Fenster?derungen
	char		cHasRootRow;								// Wird gesetzt wenn eine Root-Spalte eingef?t wird
	char		cKeyIgnore;									// Die n?hste Taste nicht f? Sucher verwenden
	char		cClickFlag;									// Merker f? LBUTTON-DOWN bei Multiselect
	char		cClickEdit;									// Merker f? LBUTTON-DOWN bei Edit-Click
	char		cIsEnabled;									// Ist das Fenster freigegeben
	char		cHasFocus;									// Hat das Fenster den Focus
	char		cReSelect;									// Soll die Auswahl neu selektiert werden
	char		cGlyphOk;									// Die Schaltf?he ?er Themen zeichnen
	char		cEditCb;									// Muss das Edit-Fenster einen Callback aufrufen
	char		cScrollMode;								// Modus f? das Autoscrollen (Bit0=nicht au?r Fenster Bit1=innerhalb Fenster)
	char		cNoExpandNf;								// Keine Expand-Notify Nachrichten
	char		cButtonFlag;								// Welche Maustaste wurde zuletzt gedr?kt
	char		cUpdateFlag;								// Ist ein Update anstehend
	char		cMinHeight;									// Minimale H?e eines Eintrages
	char		cScrollX;									// Ist die horizontale Scrollbar sichtbar
	BYTE		bScrollTicker;								// Ticker f? das Autoscrollen
	}TreeListData;

typedef	HRESULT (WINAPI *SetWindowThemeT )(HWND,LPCWSTR,LPCWSTR);
typedef	HRESULT (WINAPI *EndBufferedPtT  )(HANDLE,BOOL);
typedef	HANDLE  (WINAPI *BeginBufferedPnT)(HDC,RECT*,DWORD,LPVOID,HDC*);
typedef	HRESULT (WINAPI *BufferedPtInitT )(VOID);
typedef	LPVOID  (WINAPI *OpenThemeDataT  )(HWND hwnd,LPCWSTR pszClassList);
typedef	HRESULT (WINAPI *CloseThemeDataT )(LPVOID);
typedef	HRESULT (WINAPI *DrawThemeBackgT )(LPVOID,HDC,int,int,const RECT*,const RECT*);
typedef	HRESULT (WINAPI *GetThemeBackgRcT)(LPVOID,HDC,int,int,LPCRECT,LPRECT);
typedef	HRESULT (WINAPI *GetThemeMetricT )(LPVOID,HDC,int,int,int,int*);
typedef	HRESULT (WINAPI *GetThemePartSzT )(LPVOID,HDC,int,int,LPCRECT,int,LPSIZE);

static HMODULE			hUxThemeDll		= NULL;
static SetWindowThemeT	pSetWindowTheme	= NULL;
static EndBufferedPtT	pEndBufferedPt  = NULL;
static BeginBufferedPnT	pBeginBufferedPt= NULL;
static BufferedPtInitT	pBufferedPtInit = NULL;
static BufferedPtInitT	pBufferedPtExit = NULL;
static OpenThemeDataT	pOpenThemeData	= NULL;
static CloseThemeDataT	pCloseThemeData	= NULL;
static DrawThemeBackgT	pDrawThemeBackg	= NULL;
static GetThemeBackgRcT	pGetThemeBackgRc= NULL;
static GetThemePartSzT	pGetThemePartSz = NULL;

static HPEN				hPatternPen		= NULL;
static HBRUSH			hPatternBrush	= NULL;
static HFONT			hDefaultFontN	= NULL;
static HFONT			hDefaultFontB	= NULL;
static HCURSOR			hCursorNo		= NULL;
static HCURSOR			hCursorMove		= NULL;
static HCURSOR			hCursorArrow	= NULL;
static LONG				lWindowCount	= -1;
static RECT				sToolRect={-2,0,2,64};
static TCHAR			cKeyData[16];
static unsigned 		uKeyLast;
static unsigned 		uKeyPos;
static void				TreeListDraw(HWND hWnd,HDC hDc,RECT *pRect);
static LRESULT CALLBACK TreeListProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static void				TreeListHandleDragMove (TreeListData *pData,unsigned wParam,unsigned lParam);
static int				TreeListSelectItem     (TreeListData *pData,unsigned uItem ,unsigned uSubItem,int iMode);
static int				TreeListGetItemRect    (TreeListData *pData,unsigned uItem ,unsigned uFlags,RECT *pRect);
static int				TreeListStartNotifyEdit(TreeListData *pData,unsigned uItem ,unsigned uSub,unsigned wParam,unsigned lParam);
static int				TreeListStartAutoEdit  (TreeListData *pData,unsigned uCol  ,unsigned wParam,unsigned lParam);
static int				TreeListEndLabelEdit   (TreeListData *pData,int iMode);
extern "C" COLORREF		WINAPI SetDCPenColor   (HDC,COLORREF);
extern "C" COLORREF		WINAPI SetDCBrushColor (HDC,COLORREF);



//*****************************************************************************
//*
//*		TreeListRegister
//*
//*****************************************************************************
//	Registiert das TreeList Fenster.
//	Ergibt 1 wenn das Fenster erfolgreich registiert wurde.
int TreeListRegister(HINSTANCE hInstance)
{
static int		iIsRegistered=FALSE;
WNDCLASSEX		sClass;


    if(iIsRegistered)return TRUE;


    memset(&sClass,0,sizeof(sClass));

    sClass.cbSize           = sizeof(sClass);
    sClass.style            = CS_DBLCLKS;
    sClass.lpfnWndProc      = TreeListProc;
    sClass.cbClsExtra       = 0;
    sClass.cbWndExtra       = sizeof(TreeListData*);
    sClass.hInstance        = hInstance;
    sClass.hIcon            = NULL;
    sClass.hCursor          = LoadCursor(NULL,IDC_ARROW);
    sClass.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
    sClass.lpszMenuName     = NULL;
    sClass.hIconSm          = NULL;
    sClass.lpszClassName	= _T("TreeList");

    if(!RegisterClassEx(&sClass))return 0;

    iIsRegistered = TRUE;


return TRUE;
}

//*****************************************************************************
//*
//*		GlobalInit
//*
//*****************************************************************************
static void GlobalInit()
{
HBITMAP		hBmp;
LOGBRUSH	sLog;
long		lCount;



	   lCount = InterlockedIncrement(&lWindowCount);
	if(lCount>0)return;

	sLog.lbHatch = 0;
	sLog.lbStyle = PS_SOLID;
	sLog.lbColor = GetSysColor(COLOR_BTNSHADOW);
	hPatternPen  = ExtCreatePen(PS_COSMETIC|PS_ALTERNATE,1,&sLog,0,NULL);
	hCursorArrow = LoadCursor(NULL,IDC_ARROW);
	hCursorMove	 = LoadCursor(NULL,IDC_HAND);
	hCursorNo	 = LoadCursor(NULL,IDC_NO);


    if(!hPatternPen)
		{
		hPatternPen = CreatePen(PS_DOT,1,RGB(0,0,0));
		}

	if(!hPatternBrush)
		{
		hBmp		   = CreateBitmap(8,8,1,1,"\x55\x55\xAA\xAA\x55\x55\xAA\xAA\x55\x55\xAA\xAA\x55\x55\xAA\xAA");
		hPatternBrush  = CreatePatternBrush(hBmp);
		DeleteObject(hBmp);
		}

	if(!hUxThemeDll)
		{
		   hUxThemeDll = LoadLibrary(_T("UxTheme.dll"));
		if(hUxThemeDll)
			{
			pSetWindowTheme  = (SetWindowThemeT )GetProcAddress(hUxThemeDll,"SetWindowTheme"     );
			pEndBufferedPt   = (EndBufferedPtT  )GetProcAddress(hUxThemeDll,"EndBufferedPaint"   );
			pBeginBufferedPt = (BeginBufferedPnT)GetProcAddress(hUxThemeDll,"BeginBufferedPaint" );
			pBufferedPtInit	 = (BufferedPtInitT	)GetProcAddress(hUxThemeDll,"BufferedPaintInit"  );
			pBufferedPtExit  = (BufferedPtInitT	)GetProcAddress(hUxThemeDll,"BufferedPaintUnInit");
			pOpenThemeData	 = (OpenThemeDataT	)GetProcAddress(hUxThemeDll,"OpenThemeData"		 );
			pCloseThemeData	 = (CloseThemeDataT	)GetProcAddress(hUxThemeDll,"CloseThemeData"	 );
			pDrawThemeBackg	 = (DrawThemeBackgT )GetProcAddress(hUxThemeDll,"DrawThemeBackground");
			pGetThemePartSz  = (GetThemePartSzT	)GetProcAddress(hUxThemeDll,"GetThemePartSize"   );
			pGetThemeBackgRc = (GetThemeBackgRcT)GetProcAddress(hUxThemeDll,"GetThemeBackgroundContentRect");
			}
		}

	if(pBufferedPtInit)
		{
		pBufferedPtInit();
		}

}


//*****************************************************************************
//*
//*		GlobalDeinit
//*
//*****************************************************************************
static void GlobalDeinit()
{
int	lCount;


	   lCount=InterlockedDecrement(&lWindowCount);
	if(lCount>=0)return;

    if(hDefaultFontN)
        {
        DeleteObject(hDefaultFontN);
        hDefaultFontN = NULL;
        }

    if(hDefaultFontB)
        {
        DeleteObject(hDefaultFontB);
        hDefaultFontB = NULL;
        }

    if(hPatternPen)
        {
        DeleteObject(hPatternPen);
        hPatternPen = NULL;
        }

	if(hPatternBrush)
		{
		DeleteObject(hPatternBrush);
		}

	if(pBufferedPtExit)
		{
		pBufferedPtExit();
		}

}

//*****************************************************************************
//*
//*		DrawItem
//*
//*****************************************************************************
//	Eine Zelle via Callback zeichen
//	hDc			: DC zum Zeichnen
//	uColumn		: Nummer der Spalte in der die Zelle ist
//	uLine		: Nummer der sichtbaren Zeile (0=erste sichtbare Zeile)
//	uItem		: Ist die Nummer des Eintrages
//	uMode		: Modus f? den Text (ETO_OPAQUE oder ETO_OPAQUE|ETO_CLIPPED)
//				  Er wird ver?dert wenn der Hintergrund gezeichnet wurde.
//	uState		: Staust (TVIS_SELECTED,TVIS_DROPHILITED,TVIS_UNTERLINE,TVIS_TRACKED,TVIS_TEXTCOLOR,TVIS_FOCUSED)
//	pText		: Zeiger auf den Text
//	uTextSize	: L?ge des Textes in Zeichen
//	uTextColor	: Textfarbe (nur wenn TVIS_TEXTCOLOR in uState gesetzt ist)
//	lParam		: Datenfeld des Eintrages
//	pRect		: Abmessungen der Zelle	ohne Rand
//	Ergibt welche ein StatusCode der bestimmt was noch zuzeichnen ist:
//					0 = Der Text muss noch ausgeben werden
//					1 = Die ganze Zelle wurde bereits gezeichnet
static inline int DrawItem(TreeListData *pData,HDC hDc,unsigned uColumn,unsigned uLine,unsigned uItem,unsigned &uMode,unsigned uState,LPCTSTR pText,unsigned uTextSize,COLORREF uTextColor,LPARAM lParam,LPRECT pRect)
{
TV_DRAWITEM		sData;
int				iRet;


	if(pData->pDrawItem == NULL )return 0;
	if(pRect->left>=pRect->right)return 0;

	sData.hWnd			= pData->hWnd;
	sData.hItem			= (HTREEITEM)uItem;
	sData.pRect			= pRect;
    sData.lParam		= lParam;
	sData.pText			= pText;
    sData.uTextSize		= uTextSize;
	sData.uState		= uState;
	sData.uLine			= uLine;
	sData.ucColumn		= uColumn;
	sData.uTextColor	= uTextColor;

	   iRet = pData->pDrawItem(hDc,pData->pDrawParam,&sData);
	if(iRet<=1)return iRet;

	uMode = 0;													// Keinen Hintergrund zeichnen


return 0;
}

//*****************************************************************************
//*
//*		AllocTemp
//*
//*****************************************************************************
//	Tempor?en Speicher allokieren
static inline void AllocTemp(TreeListData *pData,LPINT *pPos,LPTSTR *pPtr,unsigned uTextSize)
{

	uTextSize += 4;

	if(pData->uTempSize<uTextSize)
		{
		if(pData->uTempSize)
			{
			delete pData->pTempPos;
			delete pData->pTempPtr;
			}

		pData->uTempSize = (uTextSize+0xFF)&~0xFF;
		pData->pTempPos	 = new INT  [pData->uTempSize+4];
		pData->pTempPtr	 = new TCHAR[pData->uTempSize+4];
		}

	if(pPos)*pPos=pData->pTempPos;
	if(pPtr)*pPtr=pData->pTempPtr;

}

//*****************************************************************************
//*
//*		SendNotify
//*
//*****************************************************************************
//	Sendet eine WM_NOTIFY Nachricht and das Elternfenster
//	pData		: Zeiger auf die Fensterdaten
//	pNotify		: Zeiger auf die Notify-Daten
//	Ergibt den R?kgabewert der WM_NOTIFY Nachrich
static LRESULT SendNotify(TreeListData *pData,NMHDR	*pNotify)
{


	pNotify->hwndFrom	= pData->hWnd;
    pNotify->idFrom		= GetWindowLong(pNotify->hwndFrom,GWL_ID);


return SendMessage(GetParent(pNotify->hwndFrom),WM_NOTIFY,pNotify->idFrom,(LPARAM)pNotify);
}


//*****************************************************************************
//*
//*		CallbackEntry
//*
//*****************************************************************************
//	Sendet eine WM_NOTIFY Nachricht and das Elternfenster um Daten zuholen
//	pData		: Zeiger auf die Fensterdaten
//	pEntry		: Zeiger auf den Eintrag
//	uItem		: Nummer des Eintrages
//	uFlags		: Welche Daten sollen abgefragt werden
//	Ergibt den R?kgabewert der WM_NOTIFY Nachrich
static void CallbackEntry(TreeListData *pData,BaseItem *pEntry,unsigned uItem,unsigned uFlags,int &iImage,unsigned &uTextSize,LPCTSTR &pText)
{
NMTVDISPINFO	sInfo;


	sInfo.item.mask				= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|uFlags;
	sInfo.item.lParam			= pEntry->lParam;
	sInfo.item.hItem			= (HTREEITEM)uItem;
	sInfo.item.state			= pEntry->uState;
	sInfo.item.stateMask		= 0xFFFFFFFF;
	sInfo.item.iImage			= I_IMAGECALLBACK;
	sInfo.item.iSelectedImage	= I_IMAGECALLBACK;
	sInfo.item.cChildren		= I_CHILDRENCALLBACK;

	if(uFlags&TVIF_TEXT)
		{
		if(uTextSize)
			{
			pData->cTempText2[sizeof(pData->cTempText2)/sizeof(TCHAR)-1]	= 0;
			pData->cTempText2[0]	= 0;
			sInfo.item.pszText		= pData->cTempText2;
			sInfo.item.cchTextMax	= sizeof(pData->cTempText2)/sizeof(TCHAR)-1;
			}
		else{
			pData->cTempText1[sizeof(pData->cTempText1)/sizeof(TCHAR)-1]	= 0;
			pData->cTempText1[0]	= 0;
			sInfo.item.pszText		= pData->cTempText1;
			sInfo.item.cchTextMax	= sizeof(pData->cTempText1)/sizeof(TCHAR)-1;
			}
		}
	else{
		sInfo.item.pszText		= 0;
		sInfo.item.cchTextMax	= 0;
		}

	sInfo.hdr.hwndFrom	= pData->hWnd;
    sInfo.hdr.idFrom	= GetWindowLong(pData->hWnd,GWL_ID);
	sInfo.hdr.code		= TVN_GETDISPINFO;

	UNLOCK(pData);
	SendMessage(GetParent(sInfo.hdr.hwndFrom),WM_NOTIFY,sInfo.hdr.idFrom,(LPARAM)&sInfo);
	LOCK(pData);


	if(uFlags&TVIF_IMAGE)
		{
		if(!(pEntry->uState&TVIS_SELECTED))iImage = sInfo.item.iImage;
		}
	else if(uFlags&TVIF_SELECTEDIMAGE)
		{
		if(pEntry->uState&TVIS_SELECTED)iImage = sInfo.item.iSelectedImage;
		}

	if(uFlags&TVIF_CHILDREN)
		{
		switch(sInfo.item.cChildren)
			{
		case  0:		pEntry->bFlags    &= ~TVIX_HASBUTTON;
						pEntry->bFlags    |=  TVIX_VARBUTTON;
						break;

		case  1:		pEntry->bFlags    &= ~TVIX_VARBUTTON;
						pEntry->bFlags    |=  TVIX_HASBUTTON;
						break;

		default:		pEntry->bFlags    |=  TVIX_VARBUTTON;

						if(pEntry->uFirstChild)
								pEntry->bFlags |=  TVIX_HASBUTTON;
						else	pEntry->bFlags &= ~TVIX_HASBUTTON;
			}
		}

	if(uFlags&TVIF_TEXT)
		{
		pText		= sInfo.item.pszText;
		uTextSize	= str_len(sInfo.item.pszText);
		pEntry->iTextPixels = 0;
 		}

}

//*****************************************************************************
//*
//*		CallbackExtra
//*
//*****************************************************************************
//	Sendet eine WM_NOTIFY Nachricht and das Elternfenster um Daten zuholen
//	pData		: Zeiger auf die Fensterdaten
//	pEntry		: Zeiger auf den Eintrag
//	uItem		: Nummer des Eintrages
//	uFlags		: Welche Daten sollen abgefragt werden
//	Ergibt den R?kgabewert der WM_NOTIFY Nachrich
static void CallbackExtra(TreeListData *pData,BaseItem *pEntry,ExtraItem *pExtra,unsigned uItem,unsigned uSub,unsigned uFlags,int &iImage,unsigned &uTextSize,LPCTSTR &pText)
{
NMTVDISPINFO	sInfo;


	sInfo.item.mask				= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|uFlags;
	sInfo.item.lParam			= pEntry->lParam;
	sInfo.item.hItem			= (HTREEITEM)uItem;
	sInfo.item.state			= pExtra->uState;
	sInfo.item.state		   |=(pEntry->uState&TVIS_BASEFLAGS);
	sInfo.item.stateMask		= 0xFFFFFFFF;
	sInfo.item.iImage			= I_IMAGECALLBACK;
	sInfo.item.iSelectedImage	= I_IMAGECALLBACK;
	sInfo.item.cChildren		= uSub;

	if(uFlags&TVIF_TEXT)
		{
		pData->cTempText1[sizeof(pData->cTempText1)/sizeof(TCHAR)-1]	= 0;
		pData->cTempText1[0]	= 0;
		sInfo.item.pszText		= pData->cTempText1;
		sInfo.item.cchTextMax	= sizeof(pData->cTempText1)/sizeof(TCHAR)-1;
		}
	else{
		sInfo.item.pszText		= 0;
		sInfo.item.cchTextMax	= 0;
		}

	sInfo.hdr.hwndFrom	= pData->hWnd;
    sInfo.hdr.idFrom	= GetWindowLong(pData->hWnd,GWL_ID);
	sInfo.hdr.code		= TVN_GETDISPINFO;

	UNLOCK(pData);
	SendMessage(GetParent(sInfo.hdr.hwndFrom),WM_NOTIFY,sInfo.hdr.idFrom,(LPARAM)&sInfo);
	LOCK(pData);


	if(uFlags&TVIF_IMAGE)iImage = sInfo.item.iImage;
	if(uFlags&TVIF_TEXT)
		{
		pText		= sInfo.item.pszText;
		uTextSize	= str_len(sInfo.item.pszText);
		}

}

//*****************************************************************************
//*
//*		EditProc
//*
//*****************************************************************************
//	Ist die Fensterfunktion f? das Edit Fenster
static LRESULT CALLBACK EditProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
TreeListData   *pData;
WNDPROC			pProc;
DWORD			dwStop;
DWORD			dwStart;
DWORD			dwCount;
LRESULT			lResult;
HWND			hParent;
HWND			hCombo;
int				iDelta;
int				iState;
int				iPos;
int				iId;


	hParent = GetParent(hWnd);
	iId		= GetWindowLong(hWnd,GWL_ID);

	if(iId==3)
		{
		hCombo  = hWnd;
		pData   = GetHandle(hParent);
		pProc   = pData->pProcId3;
		}
	else{
		hCombo  = hParent;
		hParent = GetParent(hParent);
		pData   = GetHandle(hParent);
		pProc   = (WNDPROC)GetWindowLongPtr(hWnd,GWLP_USERDATA);
		}

	if(uMsg==WM_KEYDOWN)
		{
		if(wParam==VK_RETURN)
			{
			SendMessage(hParent,WM_COMMAND,MAKELONG(3,EN_RETURN),(LPARAM)hWnd);
			return 0;
			}

		if(wParam==VK_ESCAPE)
			{
			SendMessage(hParent,WM_COMMAND,MAKELONG(3,EN_ESCAPE),(LPARAM)hWnd);
			return 0;
			}

		if((pData->uStyleEx&TVS_EX_STEPOUT) && !(GetAsyncKeyState(VK_SHIFT)&0x8000))
			{
			switch(wParam)										// Aus Fenster springen
				{
			case VK_UP:		if(pData->uEditMode)break;
							PostMessage(hParent,WM_COMMAND,MAKELONG(3,EN_RETURN),(LPARAM)hWnd);
							PostMessage(hParent,WM_KEYDOWN,VK_UP  ,0x00500001);
							PostMessage(hParent,WM_KEYUP  ,VK_UP  ,0x00500001);
							return 0;

			case VK_DOWN:	if(pData->uEditMode)break;
							PostMessage(hParent,WM_COMMAND,MAKELONG(3,EN_RETURN),(LPARAM)hWnd);
							PostMessage(hParent,WM_KEYDOWN,VK_DOWN,0x00500001);
							PostMessage(hParent,WM_KEYUP  ,VK_DOWN,0x00500001);
							return 0;

			case VK_LEFT:	if(pData->uEditMode && iId==3)break;
							SendMessage(hWnd,EM_GETSEL,(WPARAM)&dwStart,(LPARAM)&dwStop);
							if(dwStart || dwStop)break;
							PostMessage(hParent,WM_COMMAND,MAKELONG(3,EN_RETURN),(LPARAM)hWnd);
							PostMessage(hParent,WM_KEYDOWN,VK_LEFT,0x00500001);
							PostMessage(hParent,WM_KEYUP  ,VK_LEFT,0x00500001);
							return 0;

			case VK_RIGHT:	if(pData->uEditMode && iId==3)break;
							SendMessage(hWnd,EM_GETSEL,(WPARAM)&dwStart,(LPARAM)&dwStop);
							   dwCount=(DWORD)SendMessage(hWnd,EM_LINELENGTH,0,0);
							if(dwCount>dwStart)break;
							if(dwCount>dwStop )break;

							PostMessage(hParent,WM_COMMAND,MAKELONG(3,EN_RETURN),(LPARAM)hWnd);
							PostMessage(hParent,WM_KEYDOWN,VK_RIGHT,0x00500001);
							PostMessage(hParent,WM_KEYUP  ,VK_RIGHT,0x00500001);
							return 0;
				}
			}


		if(wParam==VK_DOWN && pData->uEditMode)
			{
			if(!SendMessage(hCombo,CB_GETDROPPEDSTATE,0,0))
				{
				SendMessage(hCombo,CB_SHOWDROPDOWN,1,0);
				return 0;
				}
			}
		}
	else if(uMsg==WM_CHAR)
		{
		if(wParam==VK_RETURN)
			{
			return 0;
			}

		if(wParam==VK_ESCAPE)
			{
			return 0;
			}
		}
	else if(uMsg==WM_COMMAND)
		{
		if(wParam==MAKELONG(3,EN_ESCAPE) || wParam==MAKELONG(3,EN_RETURN))
			{
			SendMessage(hParent,WM_COMMAND,wParam,(LPARAM)hWnd);
			return 0;
			}
		}
	else if(uMsg==WM_MOUSEWHEEL)
		{
		   iState=(int)CallWindowProc(pProc,hWnd,CB_GETDROPPEDSTATE,0,0);
		if(iState)
			{
			iDelta  = (short)HIWORD(wParam);
			iDelta /= WHEEL_DELTA;
			iPos	= (int)CallWindowProc(pProc,hWnd,CB_GETTOPINDEX,0,0);
			iPos   -= iDelta;
			CallWindowProc(pProc,hWnd,CB_SETTOPINDEX,iPos,0);
			return 0;
			}
		}
	else if(uMsg==WM_GETDLGCODE)								// Welche Tasten werden im Dialog benutzt
		{
		return DLGC_WANTALLKEYS;
		}
	else if(uMsg==WM_SETTEXT)
		{
		lResult = CallWindowProc(pProc,hWnd,uMsg,wParam,lParam);;
		SendMessage(hParent,WM_COMMAND,MAKELONG(3,EN_SETTEXT),(LPARAM)hWnd);
		return lResult;
		}


return CallWindowProc(pProc,hWnd,uMsg,wParam,lParam);
}

//*****************************************************************************
//*
//*		ChangeColSize
//*
//*****************************************************************************
//	Ändert die Größe der variablen Spalten
//	pData		: Zeiger auf die Fensterdaten
//	iDelta		: Ist die Größen?derung in Pixel
static void ChangeColSize(TreeListData *pData,int iDelta)
{
unsigned 	uPos;
HDITEM	 	sItem;
RECT		sRect;
TV_COLSIZE	sNotify;
int			iWeight;
int			iValue;
int			iPoint;
int			iStart;
int			iFirst;
int			iSize;
int			iRest;
int			iOff;
int			iNum;
int			iCnt;
int			iAll;
int			iVar;
int			iFix;



	sItem.mask	= HDI_WIDTH;
	iFirst		= -1;
	iCnt		= 0;
	iAll		= 0;

	for(uPos=0;;uPos++)									// Gesamt Gewicht berechen
		{
		if(uPos>=pData->uColumnCount  )break;
		if(pData->aColumn[uPos].sFixed)continue;

		iWeight = pData->aColumn[uPos].bWeight;

		if(!iWeight)continue;
		if(iFirst<0)iFirst=uPos;

		iAll += iWeight;
		iCnt += 1;
		}


	if(iCnt<=1)											// Nur eine variable Spalte
		{
		if(iCnt==0)return;

		uPos	  = iFirst;
		iValue    =	pData->aColumn[uPos].sSize;
		iValue   += iDelta;
		sItem.cxy = iValue;

		pData->aColumn[uPos].sSize = (short)iValue;
		pData->iVarSize            =        iValue;

		if(sItem.cxy<pData->aColumn[uPos].sMin)
			{
			sItem.cxy = pData->aColumn[uPos].sMin;
			}

		if(pData->aColumn[uPos].sReal!=sItem.cxy)		// Ändert sich die Breite
			{
			pData->aColumn[uPos].sReal	= (short)sItem.cxy;
			Header_SetItem(pData->hHeader,uPos,&sItem);

			if(pData->uStyleEx&TVS_EX_HEADERCHGNOTIFY)
				{
				sNotify.hdr.code			= TVN_COLUMNCHANGED;
				sNotify.uColumn				= uPos;
				sNotify.uIndex				= pData->aColumn[uPos].bIndex;
				sNotify.uPosX				= pData->aColumnXpos[uPos];
				sNotify.iSize				= sItem.cxy;

				UNLOCK(pData);
				SendNotify(pData,&sNotify.hdr);
				LOCK(pData);
				}
			}

		return;
		}

	if(iDelta>0)
			iStart = (pData->uSizeX       )%iAll;
	else	iStart = (pData->uSizeX-iDelta)%iAll;

	for(uPos=0,iOff=0;;uPos++)							// Suchen die Anfangsspalte
		{
		if(pData->aColumn[uPos].sFixed)continue;

			iWeight = pData->aColumn[uPos].bWeight;
		if(!iWeight)continue;

		   iOff+=iWeight;
		if(iOff> iStart)break;
		}


	iPoint		= 0;
	iSize		= iDelta/iAll;
	iRest		= iDelta%iAll;
	iNum		= iRest;
	iOff	   -= iStart;

	iWeight		= iOff;
	iValue		= pData->aColumn[uPos].sSize;
	iValue	   += iSize *iWeight;
	iPoint	   += iRest *iWeight;
	iValue	   += iPoint/iAll;
	iNum	   -= iPoint/iAll;
	iPoint     %= iAll;

	pData->aColumn[uPos].sSize = (short)iValue;


	if(iWeight>=pData->aColumn[uPos].bWeight)			// Wurde die ganze Spalte berechnet
		{
		iCnt--;
		iOff=0;
		}

	while(iCnt>0)
		{
		uPos++;

		if(uPos>=pData->uColumnCount)uPos=0;

		if(pData->aColumn[uPos].sFixed)continue;

			iWeight=pData->aColumn[uPos].bWeight;
		if(!iWeight)continue;

		iValue		= pData->aColumn[uPos].sSize;

 		   iCnt--;
		if(iCnt)
			{
			iValue += iSize*iWeight;
			iPoint += iRest *iWeight;
			iValue += iPoint/iAll;
			iNum   -= iPoint/iAll;
			iPoint %= iAll;
			}
		else{
			iWeight-= iOff;
			iValue += iSize*iWeight;
			iValue += iNum;
			}

		pData->aColumn[uPos].sSize = (short)iValue;
		}


	iVar   =  0;
	iFix   =  0;
	iCnt   =  pData->uColumnCountVar;

	for(uPos=0;iCnt>0;uPos++)							// Ausgeben der neuen Breiten
		{
			iWeight=pData->aColumn[uPos].bWeight;
		if(!iWeight)
			{
			iFix     +=	pData->aColumn[uPos].sSize;
			continue;
			}

		    iVar     +=	pData->aColumn[uPos].sSize;
		    sItem.cxy = pData->aColumn[uPos].sSize;
		if( sItem.cxy < pData->aColumn[uPos].sMin)
			{
			sItem.cxy = pData->aColumn[uPos].sMin;
			}

		if(pData->aColumn[uPos].sReal!=sItem.cxy)		// Ändert sich die Breite
			{
			pData->aColumn[uPos].sReal  = (short)sItem.cxy;
			Header_SetItem(pData->hHeader,uPos,&sItem);

			if(pData->uStyleEx&TVS_EX_HEADERCHGNOTIFY)
				{
				sNotify.hdr.code			= TVN_COLUMNCHANGED;
				sNotify.uColumn				= uPos;
				sNotify.uIndex				= pData->aColumn[uPos].bIndex;
				sNotify.uPosX				= pData->aColumnXpos[uPos];
				sNotify.iSize				= sItem.cxy;

				UNLOCK(pData);
				SendNotify(pData,&sNotify.hdr);
				LOCK(pData);
				}
			}

		iCnt--;
		}

	pData->iFixSize = iFix;
	pData->iVarSize = iVar;

	if(iDelta>0)
		{
		GetClientRect (pData->hHeader,&sRect);
		InvalidateRect(pData->hHeader,NULL,FALSE);
		}

}

//*****************************************************************************
//*
//*		CreateToolTip
//*
//*****************************************************************************
//	Erzeugt ein ToolTip Fenster
//	pData		: Zeiger auf die Fensterdaten
static void CreateToolTip(TreeListData *pData)
{
TOOLINFO	sInfo;


	if(pData->hToolTip)return;

	pData->hToolTip	   = CreateWindowEx(WS_EX_TRANSPARENT,TOOLTIPS_CLASS,NULL,WS_POPUP,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,pData->hWnd,NULL,NULL,NULL);
	pData->pToolProc   = (WNDPROC)GetWindowLongPtr(pData->hToolTip,GWLP_WNDPROC);

    sInfo.cbSize       = sizeof(TOOLINFO);
    sInfo.uFlags       = TTF_ABSOLUTE|TTF_TRACK|TTF_IDISHWND;
    sInfo.hwnd         = pData->hWnd;
    sInfo.hinst		   = NULL;
    sInfo.uId          = (LPARAM)(pData->hWnd);
    sInfo.lpszText     = LPSTR_TEXTCALLBACK;

	GetClientRect   (pData->hWnd,&sInfo.rect);
	SendMessage     (pData->hToolTip,TTM_ADDTOOL,0,(LPARAM)&sInfo);
	SendMessage     (pData->hToolTip,TTM_SETMAXTIPWIDTH,0,10000);
	SetWindowLong   (pData->hToolTip,GWL_ID,2);


}

//*****************************************************************************
//*
//*		CreateStateImageList
//*
//*****************************************************************************
//	Erzeugt eine Image-Liste mit zwei Checkboxen
//	pData		: Zeiger auf die Fensterdaten
//	iMode		: Welche Imageliste soll erzeugt werden (0=hStates 1=hChecks)
static void CreateStateImageList(TreeListData *pData,int iMode)
{
BITMAPINFO	sInfo;
BYTE		aMem[0x1000];
RECT		sRect;
SIZE		sSize;
HDC			hDcSrc;
HDC			hDc;
HBITMAP		hBmp;
HBITMAP		hBmpNew;
int			iBits;



	if(pOpenThemeData)									// ?er Thema zeichnen
		{
		if(!pData->hThemeBt)
			{
			pData->hThemeBt = pOpenThemeData(pData->hWnd,L"BUTTON");
			}

		if(pData->hThemeBt)
			{
			sRect.left			= 0;
			sRect.right			= 3;
			sRect.top			= 0;
			sRect.bottom		= 3;

			pGetThemePartSz(pData->hThemeBt,NULL,BP_CHECKBOX,CBS_UNCHECKEDNORMAL,&sRect,1,&sSize);

			sSize.cx           += 3;

			if(iMode)									// f? hChecks
				{
				if(pData->hChecks && pData->hChecks!=THEMEIMGLIST)
					{
					ImageList_Destroy(pData->hChecks);
					}

				pData->hChecks		= THEMEIMGLIST;
				pData->iChecksSizeX	= 16;
				pData->iChecksSizeY	= 16;
				pData->iChecksMode	= 1;

				if(pData->iChecksSizeX<sSize.cx)pData->iChecksSizeX=sSize.cx;
				if(pData->iChecksSizeY<sSize.cy)pData->iChecksSizeY=sSize.cy;
				}
			else{										// f? hStates
				if(pData->hStates && pData->hStates!=THEMEIMGLIST)
					{
					ImageList_Destroy(pData->hStates);
					}

				pData->hStates		= THEMEIMGLIST;
				pData->iStatesSizeX	= 16;
				pData->iStatesSizeY	= 16;
				pData->iStatesMode	= 1;

				if(pData->iStatesSizeX<sSize.cx)pData->iStatesSizeX=sSize.cx;
				if(pData->iStatesSizeY<sSize.cy)pData->iStatesSizeY=sSize.cy;
				}

			return;
			}
		}


	if(iMode)
		{
		if(pData->hChecks && pData->hChecks!=THEMEIMGLIST)return;
		}
	else{
		if(pData->hStates && pData->hStates!=THEMEIMGLIST)return;
		}


	hDcSrc	= GetDC(NULL);
	hDc		= CreateCompatibleDC(NULL);
	hBmp    = CreateCompatibleBitmap(hDcSrc,16*3,16);

	SelectObject(hDc,hBmp);
	SelectObject(hDc,GetStockObject(NULL_PEN));
	SetBkMode   (hDc,OPAQUE);
	SetBkColor	(hDc,GetSysColor(COLOR_WINDOW));
	SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
	Rectangle   (hDc,-1,-1,16*3+2,16+2);

	sRect.top	  = 8-6;
	sRect.bottom  = 8+7;
	sRect.left    = 16*1+8-7;
	sRect.right   = 16*1+8+6;

	DrawFrameControl(hDc,&sRect,DFC_BUTTON,DFCS_BUTTONCHECK|DFCS_FLAT);

	sRect.left    = 16*2+8-7;
	sRect.right   = 16*2+8+6;

	DrawFrameControl(hDc,&sRect,DFC_BUTTON,DFCS_BUTTONCHECK|DFCS_CHECKED|DFCS_FLAT);


	iBits = GetDeviceCaps(hDc,BITSPIXEL);

	sInfo.bmiHeader.biSize		  	= sizeof(BITMAPINFOHEADER);
	sInfo.bmiHeader.biWidth			= 16*3;
	sInfo.bmiHeader.biHeight		= 16;
	sInfo.bmiHeader.biPlanes	  	= 1;
	sInfo.bmiHeader.biBitCount	  	= (WORD)iBits;
	sInfo.bmiHeader.biCompression 	= BI_RGB;
	sInfo.bmiHeader.biSizeImage		= 0;
	sInfo.bmiHeader.biXPelsPerMeter	= 0;
	sInfo.bmiHeader.biYPelsPerMeter	= 0;
	sInfo.bmiHeader.biClrUsed		= (iBits>8)? 0:1<<iBits;;
	sInfo.bmiHeader.biClrImportant	= (iBits>8)? 0:1<<iBits;;

	GetDIBits(hDc,hBmp,0,0 ,NULL,&sInfo,DIB_RGB_COLORS);
	GetDIBits(hDc,hBmp,0,16,aMem,&sInfo,DIB_RGB_COLORS);

	hBmpNew = CreateCompatibleBitmap(hDc,16*3,16);

	SetDIBits(hDc,hBmpNew,0,16,aMem,&sInfo,DIB_RGB_COLORS);

	if(iMode==0)
		{
		pData->hStates		= ImageList_Create(16,16,ILC_COLORDDB|ILC_MASK,3,14);
		pData->iStatesSizeX	= 16;
		pData->iStatesSizeY	= 16;
		pData->iStatesMode	= 1;

		ImageList_AddMasked(pData->hStates,hBmpNew,GetSysColor(COLOR_HIGHLIGHT));
		}
	else{
		pData->hChecks		= ImageList_Create(16,16,ILC_COLORDDB|ILC_MASK,3,14);
		pData->iChecksSizeX	= 16;
		pData->iChecksSizeY	= 16;
		pData->iChecksMode	= 1;

		ImageList_AddMasked(pData->hChecks,hBmpNew,GetSysColor(COLOR_HIGHLIGHT));
		}


	DeleteObject(hBmpNew);
	DeleteObject(hBmp);
	DeleteDC(hDc);
	ReleaseDC(NULL,hDcSrc);


}

//*****************************************************************************
//*
//*		CreateDragImage
//*
//*****************************************************************************
//	Erzeugt eine Image-Liste mit zwei Checkboxen
//	pData		: Zeiger auf die Fensterdaten
//	uSub		: Ist die Spalte f? die das Drag-Image erzeugt werden soll
//	Ergibt ein Handle mit der Imageliste oder NULL bei einem Fehler
static HIMAGELIST CreateDragImage(TreeListData *pData,unsigned uItem,unsigned uSub)
{
ExtraItem  *pExtra;
BaseItem   *pEntry;
HIMAGELIST	hList;
BITMAPINFO	sInfo;
BYTE	   *pMem;
HDC			hDcSrc;
HDC			hDc;
HBITMAP		hBmp;
HBITMAP		hBmpNew;
RECT		sRect;
unsigned	uTSize;
int			iAdd;
int			iBits;
int			iWidth;
int			iHeigh;
int			iImage;
int			iYpos;
LPCTSTR		pText;



	if(uItem>pData->uTreeItemsMax)return NULL;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;

	iHeigh = pData->iFontHeight;

	if(uSub)												// Image f? Extraeintrag erzeugen
		{
		if(uSub>=pData->uColumnCount)return 0;

			pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)
			{
			pText  = _T("????");
			uTSize = 4;
			iImage = -1;
			iWidth = pData->iFontHeight*4;
			}
		else{
			pText  = pExtra->pText;
			uTSize = pExtra->uTextSize;
			iImage = pExtra->iImage;
			iWidth = pExtra->iTextPixels;

			if(pExtra->bCallback&(TVIF_IMAGE|TVIF_TEXT))
				{
				CallbackExtra(pData,pEntry,pExtra,uItem,uSub,pExtra->bCallback,iImage,uTSize,pText);
				}
			}
		}
	else{													// Image f? Haupteintrag erzeugen
		pText  = pEntry->pText;
		uTSize = pEntry->uTextSize;
		iImage = pEntry->iImage;
		iWidth = pEntry->iTextPixels;

		if(pEntry->bCallback&(TVIF_IMAGE|TVIF_TEXT))
			{
			CallbackEntry(pData,pEntry,uItem,pEntry->bCallback,iImage,uTSize,pText);
			}
		}

	if(pData->hImages && iImage>=0)							// Größen f? Images anpassen
		{
		if(iHeigh<pData->iImagesSizeY)iHeigh=pData->iImagesSizeY;
		iAdd    = pData->iImagesSizeX+2;
		iWidth += iAdd;
		}
	else{
		iAdd   = 0;
		iImage = 1;
		}


	if(iWidth>240)iWidth=240;
	if(iHeigh>32 )iHeigh=32;

		pMem = new BYTE[iHeigh*(iWidth+4)*4+1024];
	if(!pMem)return NULL;

	hDcSrc	= GetDC(NULL);
	hDc		= CreateCompatibleDC(NULL);
	hBmp    = CreateCompatibleBitmap(hDcSrc,iWidth,iHeigh);

	SelectObject(hDc,hBmp);
	SelectObject(hDc,GetStockObject(NULL_PEN));
	SelectObject(hDc,(pEntry->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN);
	SetTextColor(hDc,pData->aColors[TVC_TEXT]);
	SetBkColor	(hDc,RGB(123,77,91));

	sRect.top	  = 0;
	sRect.bottom  = iHeigh;
	sRect.left    = 0;
	sRect.right   = iWidth;
	iYpos		  =(iHeigh-pData->iFontHeight)/2;

	ExtTextOut(hDc,iAdd,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sRect,pText,uTSize,NULL);

	if(iImage>=0)
		{
		SetBkColor(hDc,GetSysColor(COLOR_WINDOW));
		ImageList_Draw(pData->hImages,iImage,hDc,0,0,ILD_TRANSPARENT);
		}



	iBits = GetDeviceCaps(hDc,BITSPIXEL);

	sInfo.bmiHeader.biSize		  	= sizeof(BITMAPINFOHEADER);
	sInfo.bmiHeader.biWidth			= iWidth;
	sInfo.bmiHeader.biHeight		= iHeigh;
	sInfo.bmiHeader.biPlanes	  	= 1;
	sInfo.bmiHeader.biBitCount	  	= (WORD)iBits;
	sInfo.bmiHeader.biCompression 	= BI_RGB;
	sInfo.bmiHeader.biSizeImage		= 0;
	sInfo.bmiHeader.biXPelsPerMeter	= 0;
	sInfo.bmiHeader.biYPelsPerMeter	= 0;
	sInfo.bmiHeader.biClrUsed		= (iBits>8)? 0:1<<iBits;;
	sInfo.bmiHeader.biClrImportant	= (iBits>8)? 0:1<<iBits;;

	GetDIBits(hDc,hBmp,0,0     ,NULL,&sInfo,DIB_RGB_COLORS);
	GetDIBits(hDc,hBmp,0,iHeigh,pMem,&sInfo,DIB_RGB_COLORS);

	hBmpNew = CreateCompatibleBitmap(hDc,iWidth,iHeigh);

	SetDIBits(hDc,hBmpNew,0,iHeigh,pMem,&sInfo,DIB_RGB_COLORS);

	hList = ImageList_Create(iWidth,iHeigh,ILC_COLORDDB|ILC_MASK,1,0);

	ImageList_AddMasked(hList,hBmpNew,RGB(123,77,91));

	DeleteObject(hBmpNew);
	DeleteObject(hBmp);
	DeleteDC(hDc);
	ReleaseDC(NULL,hDcSrc);

	delete pMem;


return hList;
}

//*****************************************************************************
//*
//*		UpdateStart
//*
//*****************************************************************************
//	H?gt eine Update-Event an oder startet ihn.
//	pData		: Zeiger auf die Fensterdaten
//	uType		: Bestimmt die Update-Funktion
//	wParam		: Erster Parameter
//	lParam		: Zweiter Parameter
static void UpdateStart(TreeListData *pData,unsigned uType,WPARAM wParam=0,WPARAM lParam=0)
{
UpdateData	*pUpdate;


		pUpdate	= new UpdateData;
	if(!pUpdate)return;

	pUpdate->pNext	= 0;
	pUpdate->uType	= uType;
	pUpdate->wParam	= wParam;
	pUpdate->lParam	= lParam;

	if(pData->pUpdateFirst)										// Am Ende der Liste anh?gen
		{
		pData->pUpdateLast->pNext =	pUpdate;
		}
	else{														// Eine neue  Liste erstellen
		pData->pUpdateFirst		  =	pUpdate;
		}

	pData->pUpdateLast = pUpdate;

	if(pData->cUpdateFlag==0)
		{
		pData->cUpdateFlag = 1;
		PostMessage(pData->hWnd,TVM_STARTUPDATE,0,0);
		}

}

//*****************************************************************************
//*
//*		UpdateColors
//*
//*****************************************************************************
//	Aktualisiert alle Farben
//	pData		: Zeiger auf die Fensterdaten
static void UpdateColors(TreeListData *pData)
{
unsigned	uColOdd;
unsigned 	uColor;
int 		iDiff;
int 		iSum;


	if(!pData->aColorChanged[TVC_BK     ])pData->aColors[TVC_BK     ]  = GetSysColor(COLOR_WINDOW);
	if(!pData->aColorChanged[TVC_BOX    ])pData->aColors[TVC_BOX    ]  = GetSysColor(COLOR_BTNSHADOW);
	if(!pData->aColorChanged[TVC_EVEN   ])pData->aColors[TVC_EVEN   ]  = GetSysColor(COLOR_WINDOW);
	if(!pData->aColorChanged[TVC_TEXT   ])pData->aColors[TVC_TEXT   ]  = GetSysColor(COLOR_WINDOWTEXT);
	if(!pData->aColorChanged[TVC_LINE   ])pData->aColors[TVC_LINE   ]  = GetSysColor(COLOR_WINDOWTEXT);
	if(!pData->aColorChanged[TVC_FRAME  ])pData->aColors[TVC_FRAME  ]  = GetSysColor(COLOR_3DFACE);
	if(!pData->aColorChanged[TVC_TRACK  ])pData->aColors[TVC_TRACK  ]  = GetSysColor(COLOR_WINDOWTEXT)^RGB(0,0,255);
	if(!pData->aColorChanged[TVC_INSERT ])pData->aColors[TVC_INSERT ]  = GetSysColor(COLOR_INFOBK);
	if(!pData->aColorChanged[TVC_ODD    ])pData->aColors[TVC_ODD    ]  = GetSysColor(COLOR_INFOBK);
	if(!pData->aColorChanged[TVC_BOXBG  ])pData->aColors[TVC_BOXBG  ]  = GetSysColor(COLOR_WINDOW);
	if(!pData->aColorChanged[TVC_COLBK  ])pData->aColors[TVC_COLBK  ]  = GetSysColor(COLOR_WINDOW);
	if(!pData->aColorChanged[TVC_COLODD ])pData->aColors[TVC_COLODD ]  = GetSysColor(COLOR_BTNSHADOW);
	if(!pData->aColorChanged[TVC_COLEVEN])pData->aColors[TVC_COLEVEN]  = GetSysColor(COLOR_WINDOW);
	if(!pData->aColorChanged[TVC_GRAYED ])pData->aColors[TVC_GRAYED ]  = GetSysColor(COLOR_SCROLLBAR);

	if(pData->hTheme && !pData->aColorChanged[TVC_BOXBG] && !pData->aColorChanged[TVC_BOX] && !pData->aColorChanged[TVC_LINE])
		{
		pData->cGlyphOk = 1;
		}
	else{
		pData->cGlyphOk = 0;
		}


	if(!pData->aColorChanged[TVC_NOFOCUS])
		{
		pData->aColors[TVC_NOFOCUS]  = ((GetSysColor(COLOR_3DFACE   )&0x00FCFCFC)>>2)*3;
		pData->aColors[TVC_NOFOCUS] += ((GetSysColor(COLOR_HIGHLIGHT)&0x00FCFCFC)>>2);
		}

	if(!pData->aColorChanged[TVC_GRAYED])
		{
		pData->aColors[TVC_GRAYED]   = ((GetSysColor(COLOR_SCROLLBAR)&0x00FEFEFE)>>1);
		pData->aColors[TVC_GRAYED]  += ((GetSysColor(COLOR_WINDOW   )&0x00FEFEFE)>>1);
		}

	if(!pData->aColorChanged[TVC_ODD])
		{
		uColOdd = pData->aColors[TVC_ODD];
		iDiff   = ((uColOdd    )&0xFF)-((pData->aColors[TVC_EVEN]    )&0xFF); iSum  = iDiff*iDiff;
		iDiff   = ((uColOdd>> 8)&0xFF)-((pData->aColors[TVC_EVEN]>> 8)&0xFF); iSum += iDiff*iDiff;
		iDiff   = ((uColOdd>>16)&0xFF)-((pData->aColors[TVC_EVEN]>>16)&0xFF); iSum += iDiff*iDiff;

		if(iSum<64)												// Ist die alternierente Farbe fast gleich ?
			{
			uColOdd  =   pData->aColors[TVC_EVEN]&0x0000FFFF;
			uColOdd |= ((pData->aColors[TVC_EVEN]&0x00FF0000)-0x00080000)&0x00FF0000;
			}

		pData->aColors[TVC_ODD] = uColOdd;
		}

	if(!pData->aColorChanged[TVC_COLBK])
		{
		   uColor = GetSysColor(COLOR_WINDOW);
		if(uColor&0x00F00000)uColor-=0x000F0000;
		if(uColor&0x0000F000)uColor-=0x00000B00;
		if(uColor&0x000000F0)uColor-=0x0000000B;

		pData->aColors[TVC_COLBK] = uColor;
		}

	if(!pData->aColorChanged[TVC_COLODD])
		{
		   uColor = pData->aColors[TVC_ODD];
		if(uColor&0x00F00000)uColor-=0x000F0000;
		if(uColor&0x0000F000)uColor-=0x00000B00;
		if(uColor&0x000000F0)uColor-=0x0000000B;

		pData->aColors[TVC_COLODD] = uColor;
		}

	if(!pData->aColorChanged[TVC_COLEVEN])
		{
		   uColor = GetSysColor(COLOR_WINDOW);
		if(uColor&0x00F00000)uColor-=0x000F0000;
		if(uColor&0x0000F000)uColor-=0x00000B00;
		if(uColor&0x000000F0)uColor-=0x0000000B;

		pData->aColors[TVC_COLEVEN] = uColor;
		}

	if(!pData->aColorChanged[TVC_MARKODD ])pData->aColors[TVC_MARKODD ]  =((pData->aColors[TVC_ODD     ]>>3)&0x1F1F1F)*7;
	if(!pData->aColorChanged[TVC_MARKODD ])pData->aColors[TVC_MARKODD ] +=((GetSysColor(COLOR_HIGHLIGHT)>>3)&0x1F1F1F)*1;
	if(!pData->aColorChanged[TVC_MARKEVEN])pData->aColors[TVC_MARKEVEN]  =((pData->aColors[TVC_EVEN    ]>>3)&0x1F1F1F)*7;
	if(!pData->aColorChanged[TVC_MARKEVEN])pData->aColors[TVC_MARKEVEN] +=((GetSysColor(COLOR_HIGHLIGHT)>>3)&0x1F1F1F)*1;


}

//*****************************************************************************
//*
//*		UpdateHeight
//*
//*****************************************************************************
//	Pr?t ob die Zeilenh?e sich ge?tert hat
//	pData		: Zeiger auf die Fensterdaten
//	Ergibt 1 bei Änderungen sonst 0
static int UpdateHeight(TreeListData *pData)
{
int		iHeight;
RECT	sRect;


	if(pData->cFixedHeight)return 0;

	iHeight = 8;

	if(pData->hChecks)
	if(iHeight<pData->iChecksSizeY)iHeight=pData->iChecksSizeY;
	if(pData->hStates)
	if(iHeight<pData->iStatesSizeY)iHeight=pData->iStatesSizeY;
	if(iHeight<pData->iSubImgYsize)iHeight=pData->iSubImgYsize;
	if(iHeight<pData->iImagesSizeY)iHeight=pData->iImagesSizeY;

	if(pData->uStyleEx&(TVS_EX_ALTERNATECOLOR|TVS_EX_ITEMLINES))
		{
		iHeight += 2;
		}
	else{
		iHeight += 1;
		}

	if(iHeight<pData->iFontHeight+3)iHeight=pData->iFontHeight+3;
	if(iHeight<pData->cMinHeight   )iHeight=pData->cMinHeight;
	if(pData->uStyleEx&TVS_EX_ITEMLINES)iHeight++;
	if(pData->uStyle  &TVS_NONEVENHEIGHT && (iHeight&1))iHeight++;
	if(pData->iRowHeight==iHeight)return 0;

	pData->iRowHeight	 = iHeight;

	if(pData->uSizeY>pData->uStartPixel)
		{
		pData->uMaxEnties  = pData->uSizeY;
		pData->uMaxEnties -= pData->uStartPixel;
		}
	else{
		pData->uMaxEnties  = 0;
		}

	pData->uPageEnties	 = pData->uMaxEnties;
	pData->uMaxEnties	+= pData->iRowHeight-1;
	pData->uMaxEnties   /= pData->iRowHeight;
	pData->uPageEnties  /= pData->iRowHeight;

	GetClientRect (pData->hWnd,&sRect);
	InvalidateRect(pData->hWnd,&sRect,FALSE);


return 1;
}

//*****************************************************************************
//*
//*		UpdateRect
//*
//*****************************************************************************
//	Zeichnet einen Eintrag neu
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uSub		: Ist die Spaltennummer
//	Ergibt 1 wenn der Eintrag sichtbar war
static int UpdateRect(TreeListData *pData,unsigned uItem,unsigned uSub)
{
BaseItem   *pEntry;
RECT		sRect;
UINT		uNext;
UINT		uPos;



	    pEntry = pData->pTreeItems[uItem];
	if(!pEntry || !pEntry->uShowPos)return 0;				// Ist der Eintrag aufgeklappt

	   uPos = pEntry->uShowPos-pData->uScrollY-1;
	if(uPos>=pData->uMaxEnties)								// Eintrag im Fenster sichtbar ?
		{
		if(pEntry->uShowPos>pData->uFixedLines)
			{
			return 0;
			}

		uPos = pEntry->uShowPos-1;
		}

	uNext		  = pData->aColumn[uSub].bNext;
	sRect.left    = pData->aColumnXpos[uSub ];
	sRect.left   -= pData->uScrollX;
	sRect.right	  = pData->aColumnXpos[uNext];
	sRect.right  -= pData->uScrollX;
	sRect.top     = pData->uStartPixel;
	sRect.top    += pData->iRowHeight*uPos;
	sRect.bottom  = pData->iRowHeight+sRect.top;

	InvalidateRect(pData->hWnd,&sRect,FALSE);



return 1;
}

//*****************************************************************************
//*
//*		UpdateColRect
//*
//*****************************************************************************
//	Zeichnet einen ganze Spalte neu
//	pData		: Zeiger auf die Fensterdaten
//	uColumn		: Die nummer der Spalte
//	Ergibt 1 wenn der Eintrag sichtbar war
static int UpdateColRect(TreeListData *pData,unsigned uColumn)
{
RECT	sRect;
UINT	uNext;


	if(uColumn>=pData->uColumnCount)return 0;

	   sRect.left  = pData->aColumnXpos[uColumn];
	   sRect.left -= pData->uScrollX;
	if(sRect.left>(int)pData->uSizeX)return 0;

	uNext = pData->aColumn[uColumn].bNext;

	   sRect.right = pData->aColumnXpos[uNext];
	   sRect.right-= pData->uScrollX;
	if(sRect.right<0)return 0;

	sRect.top      = 0;
	sRect.bottom   = pData->uSizeY;

	InvalidateRect(pData->hWnd,&sRect,FALSE);


return 1;
}

//*****************************************************************************
//*
//*		UpdateRow
//*
//*****************************************************************************
//	Zeichnet einen Eintrag ?er die ganze Zeile neu
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	Ergibt 1 wenn der Eintrag sichtbar war
static int UpdateRow(TreeListData *pData,unsigned uItem)
{
BaseItem   *pEntry;
RECT		sRect;
unsigned	uPos;



	    pEntry = pData->pTreeItems[uItem];
	if(!pEntry || !pEntry->uShowPos)return 0;				// Ist der Eintrag aufgeklappt

	   uPos = pEntry->uShowPos-pData->uScrollY-1;
	if(uPos>=pData->uMaxEnties)								// Eintrag im Fenster sichtbar ?
		{
		if(pEntry->uShowPos>pData->uFixedLines)
			{
			return 0;
			}

		uPos = pEntry->uShowPos-1;
		}

	sRect.left    = 0;
	sRect.right   = pData->uSizeX;
	sRect.top     = pData->uStartPixel;
	sRect.top    += pData->iRowHeight*uPos;
	sRect.bottom  = pData->iRowHeight+sRect.top;
	InvalidateRect(pData->hWnd,&sRect,FALSE);



return 1;
}

//*****************************************************************************
//*
//*		UpdateView
//*
//*****************************************************************************
//	Zeichnet das ganze Fenster neu
//	pData		: Zeiger auf die Fensterdaten
//	Ergibt 1 wenn der Eintrag sichtbar war
static void UpdateView(TreeListData *pData)
{
RECT	sRect;


	GetClientRect (pData->hWnd,&sRect);
	sRect.top =    pData->uStartPixel;
	InvalidateRect(pData->hWnd,&sRect,FALSE);

}

//*****************************************************************************
//*
//*		UpdateScrollX
//*
//*****************************************************************************
//	Aktualisiert die X-Scroolbar
//	pData		: Zeiger auf die Fensterdaten
//	bUnlock		: Die Sperre aufheben
//	Ergibt 1 wenn der sich Einstellungen ver?dert haben
static void UpdateScrollX(TreeListData *pData,BOOL bUnlock=TRUE)
{
SCROLLINFO  sInfo;
unsigned	uSize;
unsigned	uCols;



	   uCols = pData->uColumnCount;
	if(uCols)
			uSize = pData->aColumnXpos[uCols]-1;
	else	uSize = pData->iMaxSizeX-1;


	if(pData->uOldCountX==uSize)
	if(pData->uOldPageX ==pData->uSizeX)
		{
		return;
		}

	pData->uOldPageX	= pData->uSizeX;
	pData->uOldCountX	= uSize;

	if(bUnlock)UNLOCK(pData);

	sInfo.cbSize	= sizeof(SCROLLINFO);
	sInfo.fMask		= SIF_ALL;
	sInfo.nMin		= 0;
	sInfo.nMax  	= uSize;
	sInfo.nPage 	= pData->uSizeX;
	sInfo.nPos  	= pData->uScrollX;
	sInfo.nTrackPos	= 0;


	if(pData->uStyle&TVS_NOSCROLL)
		{
		sInfo.nMax = 0;
		}
	else if(pData->uStyleEx&TVS_EX_AUTOHSCROLL)
		{
		sInfo.nMax = 0;
		}
	else if(sInfo.nMax<0)
		{
		sInfo.nMax = 0;
		}
	else if(sInfo.nMax>0)
		{
		sInfo.nMax--;
		}


	if((int)sInfo.nPage>=sInfo.nMax && pData->uScrollX>0)
		{
		sInfo.nPos      = 0;
		pData->uScrollX = 0;

		UpdateView(pData);

		if(pData->hHeader)
			{
			MoveWindow(pData->hHeader,0,0,pData->uSizeX,pData->uStartPixel,TRUE);
			}
		}

	pData->cScrollX = (sInfo.nMax>0);
	SetScrollInfo(pData->hWnd,SB_HORZ,&sInfo,TRUE);

	if(bUnlock)LOCK(pData);

}

//*****************************************************************************
//*
//*		UpdateScrollY
//*
//*****************************************************************************
//	Aktualisiert die Y-Scroolbar
//	pData		: Zeiger auf die Fensterdaten
//	bUnlock		: Die Sperre aufheben
//	Ergibt 1 wenn der sich Einstellungen ver?dert haben
static void UpdateScrollY(TreeListData *pData,BOOL bUnlock=TRUE)
{
SCROLLINFO  sInfo;



	if(pData->uOldCountY==pData->uItemPosCount)
	if(pData->uOldPageY ==pData->uPageEnties  )
		{
		return;
		}

	pData->uOldPageY	= pData->uPageEnties;
	pData->uOldCountY	= pData->uItemPosCount;

	if(bUnlock)UNLOCK(pData);

	sInfo.cbSize	= sizeof(SCROLLINFO);
	sInfo.fMask		= SIF_ALL;
	sInfo.nMin		= 0;
	sInfo.nMax  	= pData->uItemPosCount;
	sInfo.nPage 	= pData->uPageEnties;
	sInfo.nPos  	= pData->uScrollY;
	sInfo.nTrackPos	= 0;

	if(pData->uStyle&TVS_NOSCROLL)
		{
		sInfo.nMax = 0;
		}
	else if(sInfo.nMax<0)
		{
		sInfo.nMax = 0;
		}

	if((int)sInfo.nPage>=sInfo.nMax && pData->uScrollY>0)
		{
		sInfo.nPos      = 0;
		pData->uScrollY = 0;

		UpdateView(pData);
		}

	if(pData->uStyleEx&TVS_EX_ALWAYSSCROLLY)				// Scrollbar immer einblenden
		{
		sInfo.fMask    |= SIF_DISABLENOSCROLL;
		}

	SetScrollInfo(pData->hWnd,SB_VERT,&sInfo,TRUE);

	if(bUnlock)LOCK(pData);


}

//*****************************************************************************
//*
//*		UpdateToolTip
//*
//*****************************************************************************
//	Aktualisiert den Text f? das Tootip
//	pData	: Zeiger auf Fensterdaten
//	uItem	: Item auf den der Mauszeiger zeigt
//	uFlags	: Flags vom HitTest
static void UpdateToolTip(TreeListData *pData,unsigned uItem,unsigned uFlags)
{
TCHAR			cTemp[INFOTIPSIZE];
HWND			hToolTip;
NMTVGETINFOTIP	sToolNv;
NMTREEVIEW		sNotify;
TOOLINFO		sInfo;
ExtraItem	   *pExtra;
BaseItem	   *pEntry;
LPCTSTR	    	pText;
RECT			sRect;
unsigned		uSize;
unsigned		uCol;
unsigned		uLen;
LRESULT			lRet;
int				iTemp;



															// Tooltip ausbelnden
	if(!uItem || (uItem==pData->uEditItem && TVHT_SUBTOCOL(uFlags)==pData->uEditSub))
		{
		if(pData->uToolTipItem)goto ExitTip;
		return;
		}


	pEntry = pData->pTreeItems[uItem];

	if(uFlags&TVHT_ONITEM)
		{
		if(pData->uToolTipItem!=uItem || pData->uToolTipSub!=0)
			{
			if(!pData->pTreeItems[uItem])					// Existiert der Eintag noch ?
				{
				goto ExitTip;
				}

			TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT,&sRect);

			if(sRect.right>(int)pData->uSizeX)
				{
				sRect.right = pData->uSizeX;
				}

			lRet = 0;

			if(pData->uStyleEx&TVS_EX_TOOLTIPNOTIFY)		// Tooltip-Daten via speziellem Notify holen
				{
				sNotify.hdr.code			= TVN_ITEMTOOLTIP;
				sNotify.action				= 0;
				sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
				sNotify.itemNew.hItem		= (HTREEITEM)uItem;
				sNotify.itemNew.stateMask	= 0xFFFFFFFF;
				sNotify.itemNew.state		= pEntry->uState;
				sNotify.itemNew.lParam		= pEntry->lParam;
				sNotify.itemNew.pszText		= pEntry->pText;
				sNotify.itemNew.cchTextMax	= pEntry->uTextSize;
				sNotify.itemNew.cChildren	= 0;
				sNotify.itemOld.mask		= 0;
				sNotify.ptDrag.x			= sRect.left;
				sNotify.ptDrag.y			= sRect.top;

				UNLOCK(pData);
				lRet=SendNotify(pData,&sNotify.hdr);
				LOCK(pData);

				if(lRet)goto UserTip;
				}
			else if(pData->uStyle&TVS_INFOTIP)				// Tooltip-Daten via normalem Notify holen
				{
				sToolNv.hdr.code			= TVN_GETINFOTIP;
				sToolNv.cchTextMax			= INFOTIPSIZE;
				sToolNv.hItem				= (HTREEITEM)uItem;
				sToolNv.lParam				= pEntry->lParam;
				sToolNv.pszText				= cTemp;

				str_ncpy(cTemp,pEntry->pText,INFOTIPSIZE);
				cTemp[INFOTIPSIZE-1]=0;

				UNLOCK(pData);
				lRet=SendNotify(pData,&sNotify.hdr);
				LOCK(pData);

				sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
				sNotify.itemNew.hItem		= (HTREEITEM)uItem;
				sNotify.itemNew.pszText		= sToolNv.pszText;
				sNotify.itemNew.cchTextMax	= str_len(sToolNv.pszText);
				sNotify.itemNew.cChildren	= 0;
				sNotify.itemNew.stateMask	= 0xFFFFFFFF;
				sNotify.itemNew.state		= pEntry->uState;
				sNotify.itemNew.lParam		= pEntry->lParam;
				sNotify.ptDrag.x			= sRect.left;
				sNotify.ptDrag.y			= sRect.top;

				goto UserTip;
				}

															// Passt der Text in die Spalte
			if(sRect.right-sRect.left<=pEntry->iTextPixels+4)
				{
				pText	= pEntry->pText;
				uSize	= pEntry->uTextSize;

				if(pEntry->bCallback&TVIF_TEXT)
					{
					CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iTemp,uSize,pText);
					}

				if(!pText || *pText==0)goto ExitTip;

				   uLen = str_len(pText)+1;
				if(uLen>=pData->uToolTipSize)				// Tooltipspeicher vergrößern
					{
					delete pData->pToolTipText;
					pData->uToolTipSize = (uLen+255)&~0xFF;
					pData->pToolTipText = new TCHAR[pData->uToolTipSize+4];
					}

				memcpy(pData->pToolTipText,pText,uLen*sizeof(TCHAR));
				pData->hFontT =(pEntry->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;


				hToolTip				= pData->hToolTip;
				pData->sToolTipPos.x	= sRect.left;
				pData->sToolTipPos.y	= sRect.top;

				ClientToScreen(pData->hWnd,&pData->sToolTipPos);

				UNLOCK(pData);

				sInfo.cbSize			= sizeof(sInfo);
				sInfo.hwnd				= pData->hWnd;
				sInfo.uId				= (UINT_PTR)pData->hWnd;

				if(pData->uToolTipItem)
					{
					SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
					pData->uToolTipItem = 0;
					}

				SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(pData->sToolTipPos.x,pData->sToolTipPos.y));
				SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

				LOCK(pData);

				pData->uToolTipItem		= uItem;
				pData->uToolTipShow 	= 0;
				pData->uToolTipSub		= 0;

				SetTimer(pData->hWnd,ID_TOOLTIPCHECK,1500,NULL);
				}
			else{
				if(pData->uToolTipItem)goto ExitTip;
				}
			}

		return;
		}

	if(uFlags&(TVHT_ONSUBICON|TVHT_ONSUBLABEL))
		{
		if(pData->uToolTipItem!=uItem || TVHT_SUBTOCOL(uFlags)!=pData->uToolTipSub)
			{
			lRet   = 0;
			uCol   = TVHT_SUBTOCOL(uFlags);
			pExtra = pData->pExtraItems[uCol-1][uItem];

			if(pData->uStyleEx&TVS_EX_TOOLTIPNOTIFY)		// Tooltip-Daten via Notify holen
				{
				TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT|TVIR_COLTOSUB(uCol),&sRect);

				if(sRect.right>(int)pData->uSizeX)
					{
					sRect.right = pData->uSizeX;
					}

				if(pExtra)
					{
					sNotify.itemNew.state		= pExtra->uState;
					sNotify.itemNew.pszText		= pExtra->pText;
					sNotify.itemNew.cchTextMax	= pExtra->uTextSize;
					}
				else{
					sNotify.itemNew.state		= 0;
					sNotify.itemNew.cchTextMax	= 0;
					sNotify.itemNew.pszText		= _T("");
					}

				sNotify.hdr.code			= TVN_ITEMTOOLTIP;
				sNotify.action				= 0;
				sNotify.itemNew.lParam		= pEntry->lParam;
				sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
				sNotify.itemNew.hItem		= (HTREEITEM)uItem;
				sNotify.itemNew.stateMask	= 0xFFFFFFFF;
				sNotify.itemNew.cChildren	= uCol;
				sNotify.itemOld.mask		= 0;
				sNotify.ptDrag.x			= sRect.left;
				sNotify.ptDrag.y			= sRect.top;

				UNLOCK(pData);
				lRet=SendNotify(pData,&sNotify.hdr);
				LOCK(pData);

				if(lRet)goto UserTip;
				}

			if(pExtra)										// Tooltip auf Unterspalte ?
				{
				TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT|TVIR_COLTOSUB(uCol),&sRect);

				if(sRect.right>(int)pData->uSizeX)
					{
					sRect.right = pData->uSizeX;
					}

				if(sRect.right-sRect.left<=pExtra->iTextPixels+4)
					{
					pText	= pExtra->pText;
					uSize	= pExtra->uTextSize;

					if(pExtra->bCallback&TVIF_TEXT)
						{
						CallbackExtra(pData,pEntry,pExtra,uItem,uCol,TVIF_TEXT,iTemp,uSize,pText);
						}

					if(!pText || *pText==0)goto ExitTip;

					   uLen = str_len(pText)+1;
					if(uLen>=pData->uToolTipSize)			// Tooltipspeicher vergrößern
						{
						delete pData->pToolTipText;
						pData->uToolTipSize = (uLen+255)&~0xFF;
						pData->pToolTipText = new TCHAR[pData->uToolTipSize+4];
						}

					memcpy(pData->pToolTipText,pText,uLen*sizeof(TCHAR));
					pData->hFontT =(pExtra->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;

					pData->sToolTipPos.x	= sRect.left;
					pData->sToolTipPos.y	= sRect.top;
					hToolTip				= pData->hToolTip;

					ClientToScreen(pData->hWnd,&pData->sToolTipPos);

					UNLOCK(pData);

					sInfo.cbSize			= sizeof(sInfo);
					sInfo.hwnd				= pData->hWnd;
					sInfo.uId				= (UINT_PTR)pData->hWnd;

					if(pData->uToolTipItem)
						{
						SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
						pData->uToolTipItem = 0;
						}

					SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(pData->sToolTipPos.x,pData->sToolTipPos.y));
					SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

					LOCK(pData);

					pData->uToolTipItem		= uItem;
					pData->uToolTipSub		= uCol;
					pData->uToolTipShow 	= 0;

					SetTimer(pData->hWnd,ID_TOOLTIPCHECK,1500,NULL);
					}
				else{
					if(pData->uToolTipItem)goto ExitTip;
					}
				}
			else{
				if(pData->uToolTipItem)goto ExitTip;
				}
			}

		return;
 		}

ExitTip:

	if(pData->uToolTipItem)									// Tooltip ausblenden
		{
		UNLOCK(pData);

		sInfo.cbSize			= sizeof(sInfo);
		sInfo.hwnd				= pData->hWnd;
		sInfo.uId				= (UINT_PTR)pData->hWnd;

		SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);

		LOCK(pData);

		pData->uToolTipItem = 0;
		pData->uToolTipSub	= 0;
		pData->uToolTipShow = 0;

		KillTimer(pData->hWnd,ID_TOOLTIPCHECK);
		}


	return;


UserTip:

	pText	   = sNotify.itemNew.pszText;					// Soll ein User-Tooltip angezeigt werden
	uSize	   = sNotify.itemNew.cchTextMax;
	sRect.left = sNotify.ptDrag.x;
	sRect.top  = sNotify.ptDrag.y;

	if(!pText ||*pText==0)goto ExitTip;

	   uLen = str_len(pText)+1;
	if(uLen>=pData->uToolTipSize)							// Tooltipspeicher vergrößern
		{
		delete pData->pToolTipText;
		pData->uToolTipSize = (uLen+255)&~0xFF;
		pData->pToolTipText = new TCHAR[pData->uToolTipSize+4];
		}

	memcpy(pData->pToolTipText,pText,uLen*sizeof(TCHAR));
	pData->hFontT		= (pEntry->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;

	ClientToScreen(pData->hWnd,&sNotify.ptDrag);
	pData->sToolTipPos	= sNotify.ptDrag;
	hToolTip			= pData->hToolTip;
															// Tooltip verz?ert anzeigen
	if((sNotify.itemNew.mask&TVIF_TOOLTIPTIME) && sNotify.itemNew.lParam>0)
		{
		pData->uToolTipShow = (unsigned)(sNotify.itemNew.lParam+499)/500;
		pData->uToolTipSub	= sNotify.itemNew.cChildren;
		pData->uToolTipItem = uItem;

		SetTimer(pData->hWnd,ID_TOOLTIPCHECK,500,NULL);

		return;
		}

	UNLOCK(pData);

	sInfo.cbSize		= sizeof(sInfo);
	sInfo.hwnd			= pData->hWnd;
	sInfo.uId			= (UINT_PTR)pData->hWnd;

	if(pData->uToolTipItem)									// Tooltip Fenster aktivieren
		{
		SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
		pData->uToolTipItem = 0;
		}

	SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(sNotify.ptDrag.x,sNotify.ptDrag.y));
	SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

	LOCK(pData);

	pData->uToolTipShow = 0;
	pData->uToolTipItem = uItem;
	pData->uToolTipSub	= sNotify.itemNew.cChildren;

	SetTimer(pData->hWnd,ID_TOOLTIPCHECK,1500,NULL);


}

//*****************************************************************************
//*
//*		UpdateFont
//*
//*****************************************************************************
//	Erzeugt einen den fetten Font f? das Fenster
//	pData		: Zeiger auf die Fensterdaten
//	iRedraw		: Soll das Fenster neugezeichnet werden
//	Ergibt 1 wenn der Font ver?dert wurde
static int UpdateFont(TreeListData *pData)
{
int			iPos;
int			iRet;
HDC			hDc;
LOGFONT		sLog;
SIZE		sSize;
TEXTMETRIC	sMetrics;
BaseItem   *pEntry;
BaseItem  **pList;
ExtraItem  *pExtra;
ExtraItem **pItems;
unsigned	uSub;



	if(!hDefaultFontN)										// Den Standard-Font erzeugen
		{
		SystemParametersInfo(SPI_GETICONTITLELOGFONT,sizeof(sLog),&sLog,0);
		sLog.lfWeight=FW_NORMAL;hDefaultFontN=CreateFontIndirect(&sLog);
		sLog.lfWeight=FW_BOLD  ;hDefaultFontB=CreateFontIndirect(&sLog);
		}


	if(pData->hFontN==NULL)									// Standard-Font benutzen
		{
		pData->hFontN=hDefaultFontN;
		}


	if(pData->hFontN==hDefaultFontN)						// Ist der Standard-Font eingestellt
		{
		pData->hFontB=hDefaultFontB;
		}
	else{
		pData->hFontB = (pData->hFontX)? pData->hFontX:pData->hFontN;
		}


	if(pData->hFontN!=pData->hFontL)						// Wurde ein neuer Font definiert
		{
		pData->hFontL=pData->hFontN;

		hDc = GetDC(NULL);
		SelectObject(hDc,pData->hFontN);
		GetTextMetrics(hDc,&sMetrics);
		pData->iFontHeight =  sMetrics.tmHeight;
		pData->iFontLine   =  sMetrics.tmAscent+1;
		pData->iFontOff    = (sMetrics.tmPitchAndFamily&TMPF_FIXED_PITCH)? 0:-1;
		if(pData->hFontN!=hDefaultFontN)GetTextFace(hDc,LF_FACESIZE,sLog.lfFaceName);
		ReleaseDC(NULL,hDc);

		if(pData->hFontN!=hDefaultFontN)					// Erstelle fetten Font
			{
			if(pData->hFontX)
				{
				DeleteObject(pData->hFontX);
				pData->hFontX = 0;
				}

			sLog.lfHeight			= sMetrics.tmHeight;
			sLog.lfWidth 			= sMetrics.tmAveCharWidth;
			sLog.lfEscapement 		= GM_COMPATIBLE;
			sLog.lfOrientation 		= 0;
			sLog.lfItalic 			= sMetrics.tmItalic;
			sLog.lfUnderline 		= sMetrics.tmUnderlined;
			sLog.lfStrikeOut 		= sMetrics.tmStruckOut;
			sLog.lfCharSet 			= sMetrics.tmCharSet;
			sLog.lfOutPrecision 	= OUT_DEVICE_PRECIS;
			sLog.lfClipPrecision 	= CLIP_DEFAULT_PRECIS;
			sLog.lfQuality 			= DEFAULT_QUALITY;
			sLog.lfPitchAndFamily 	= sMetrics.tmPitchAndFamily;
			sLog.lfWeight			= FW_BOLD;

			pData->hFontX = CreateFontIndirect(&sLog);
			if(pData->hFontX)pData->hFontB=pData->hFontX;
			}

		pList = pData->pTreeItems;
		iPos  = pData->uTreeItemsMax;

		for(;iPos>=0;iPos--)							// Alle Textbreiten zur?ksetzen
			{
				pEntry = pList[iPos];
			if(!pEntry)continue;

			pEntry->iTextPixels=0;
			}


		for(uSub=1;uSub<pData->uColumnCount;uSub++)
			{
			iPos    = pData->uTreeItemsMax;
			pItems 	= pData->pExtraItems[uSub-1];

			for(;iPos>=0;iPos--)
				{
					pExtra = pItems[iPos];
				if(!pExtra)continue;

				pExtra->iTextPixels=0;
				}
			}

		iRet = 1;
		}
	else{
		iRet = 0;
		}


	hDc = GetDC(NULL);										// Breite der "..." Texte
	SelectObject(hDc,pData->hFontN);
	GetTextExtentExPoint(hDc,_T("..."),3,256,NULL,NULL,&sSize);
	pData->uTrippleN =  sSize.cx;
	SelectObject(hDc,pData->hFontB);
	GetTextExtentExPoint(hDc,_T("..."),3,256,NULL,NULL,&sSize);
	pData->uTrippleB =  sSize.cx;
	ReleaseDC(NULL,hDc);




return iRet;
}


//*****************************************************************************
//*
//*		UpdateItems
//*
//*****************************************************************************
//	Berechnet die Positionen der Zeilen f? die sichtbaren Eintr?e
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist der Eintrag ab dem begonnen wird
static void UpdateItems(TreeListData *pData,unsigned uItem)
{
unsigned	uPos;
unsigned	uOld;
unsigned	uNum;
unsigned	uTemp;
unsigned	uStart;
unsigned   *pLines;
BaseItem  **pItems;
BaseItem   *pEntry;
BaseItem   *pTemp;
RECT		sRect;


	uOld	= pData->uItemPosCount;
	pLines	= pData->pItemPos;
	pItems	= pData->pTreeItems;


	if(!uItem)												// Am Anfang beginnen
		{
			uItem = pData->uFirstChild;
		if(!uItem)											// Leere Liste
			{
			if(!uOld)return;

			for(uNum=0;uNum<uOld;uNum++)					// Die alten Eintr?e zur?ksetzen
				{
					uTemp = pLines[uNum];
				if(!uTemp)continue;
					pLines[uNum]=0;
					pTemp = pItems[uTemp];
				if(!pTemp)continue;
					pTemp->uShowPos = 0;
				}

			pData->uItemPosCount = 0;

			GetClientRect (pData->hWnd,&sRect);
			InvalidateRect(pData->hWnd,&sRect,TRUE);

			memset(pLines,0,sizeof(unsigned)*uOld);
			return;
			}


		for(uNum=0;uNum<uOld;uNum++)						// Die alten Eintr?e zur?ksetzen
			{
				uTemp = pLines[uNum];
			if(!uTemp)continue;
				pLines[uNum]=0;
				pTemp = pItems[uTemp];
			if(!pTemp)continue;
				pTemp->uShowPos = 0;
			}

		pEntry			 = pItems[uItem];
		pEntry->bFlags	|= TVIX_LINE;
		pEntry->uShowPos = 1;
		pLines[0]		 = uItem;
		uPos			 = 1;
		uStart			 = 0;
		}
	else{													// Bei einem Eintrag beginnen
		pEntry			 = pItems[uItem];
		pEntry->bFlags	|= TVIX_LINE;
		uPos			 = pEntry->uShowPos;
		if(uPos)uStart	 = uPos-1;
		else	uStart	 = 0;

		for(uNum=uPos;uNum<uOld;uNum++)						// Die alten Eintr?e zur?ksetzen
			{
				uTemp = pLines[uNum];
			if(!uTemp)continue;
				pLines[uNum]=0;
				pTemp = pItems[uTemp];
			if(!pTemp)continue;
				pTemp->uShowPos = 0;
			}
		}


	if(pData->uStyleEx&TVS_EX_ALLOWITEMHIDE)				// K?nen Eintr?e ausgeblendet werden
		{
		if(uPos==1 && (pEntry->uState&TVIS_HIDEITEM))
			{
			pEntry->uShowPos = 0;
			uPos             = 0;
			}

		for(;;)												// Die Zeilen neu zuordnen
			{
			if(pEntry->uFirstChild && (pEntry->uState&TVIS_EXPANDED) && !(pEntry->uState&TVIS_HIDEITEM))
				{
				uItem=pEntry->uFirstChild;
				}
			else if(pEntry->uNextItem)
				{
				uItem=pEntry->uNextItem;
				}
			else{
				if(pEntry->uShowPos)
					{
					pEntry->bFlags &= ~TVIX_LINE;
					}
				else{
					for(pTemp=pEntry;;)						// Suche letzen sichtbaren in der Ebene
						{
							uTemp = pTemp->uPrevItem;
						if(!uTemp)break;

							pTemp = pItems[uTemp];
						if(!pTemp->uShowPos)continue;
						if( pTemp->bFlags&TVIX_LINE)
							{
							if(pTemp->uShowPos<=uStart)
								{
								uStart = pTemp->uShowPos-1;
								}

							pTemp->bFlags &= ~TVIX_LINE;
							}

						break;
						}
					}

				for(;;)
					{
						uItem = pEntry->uParent;
					if(!uItem)break;

					   pEntry = pItems[uItem];
					if(pEntry->uNextItem)					// Gibt es etwas in der gleichen Ebene
						{
						uItem = pEntry->uNextItem;
						break;
						}

					if(pEntry->uShowPos)
						{
						pEntry->bFlags &= ~TVIX_LINE;
						}
					else{
						for(pTemp=pEntry;;)					// Suche letzen sichtbaren in der Ebene
							{
								uTemp = pTemp->uPrevItem;
							if(!uTemp)break;

								pTemp = pItems[uTemp];
							if(!pTemp->uShowPos)continue;
							if( pTemp->bFlags&TVIX_LINE)
								{
								if(pTemp->uShowPos<=uStart)
									{
									uStart = pTemp->uShowPos-1;
									}

								pTemp->bFlags &= ~TVIX_LINE;
								}

							break;
							}
						}
					}

				if(!uItem)break;
				}

			pEntry = pItems[uItem];

			if(pEntry->uState&TVIS_HIDEITEM)				// Ist der Eintrag ausgeblendet
				{
				continue;
				}

			if(pLines[uPos]!=uItem)
				{
				pLines[uPos] = uItem;
				}
			else{
				if(uStart==uPos)uStart++;
				}

			uPos++;
			pEntry->bFlags	|= TVIX_LINE;
			pEntry->uShowPos = uPos;
			}
		}
	else{
		for(;;)												// Die Zeilen neu zuordnen
			{
			if(pEntry->uFirstChild && (pEntry->uState&TVIS_EXPANDED))
				{
				uItem=pEntry->uFirstChild;
				}
			else if(pEntry->uNextItem)
				{
				uItem=pEntry->uNextItem;
				}
			else{
				pEntry->bFlags &= ~TVIX_LINE;

				for(;;)										// Ebenen nach oben
					{
						uItem = pEntry->uParent;
					if(!uItem)
						{
						break;
						}

					   pEntry = pItems[uItem];
					if(pEntry->uNextItem)					// Gibt es etwas in der gleichen Ebene
						{
						uItem = pEntry->uNextItem;
						break;
						}

					pEntry->bFlags &= ~TVIX_LINE;
					}

				if(!uItem)break;
				}

			pEntry = pItems[uItem];

			if(pLines[uPos]!=uItem)
				{
				pLines[uPos] = uItem;
				}
			else{
				if(uStart==uPos)uStart++;
				}

			uPos++;
			pEntry->bFlags	|= TVIX_LINE;
			pEntry->uShowPos = uPos;
			}
		}


	pData->uItemPosCount = uPos;


	if(uStart>pData->uScrollY)								// Neu zu zeichnenten Bereich bestimmen
			uStart-= pData->uScrollY;
	else	uStart = 0;

	GetClientRect(pData->hWnd,&sRect);

	sRect.top = pData->uStartPixel+pData->iRowHeight*uStart;

	if(sRect.top<=sRect.bottom)
		{
		InvalidateRect(pData->hWnd,&sRect,FALSE);
		}

	if(uOld!=uPos)UpdateScrollY(pData);



}


//*****************************************************************************
//*
//*		UpdateColumns
//*
//*****************************************************************************
//	Pr?t ob es Ver?derungen in Spaltenbreiten gab
//	pData		: Zeiger auf die Fensterdaten
//	Ergibt die Breite ab der die Spalten ver?dert wurden oder 0x10000
static int UpdateColumns(TreeListData *pData)
{
HWND	hHeader;
UINT	uNext;
UINT	uCol;
UINT	uSub;
int		iSize;
int		iNum;
int		iNow;
int		iOld;
int		iRet;


	hHeader = pData->hHeader;
	pData->aColumnXpos[0] = 0;

	iRet = 0x10000;
	iOld = 0;
	iNow = 0;

	for(uCol=0;uCol<pData->uColumnCount;)					// Suche die erste ge?derte Spalte
		{
		uSub  = pData->aColumnPos[uCol];
		iSize = pData->aColumn[uSub].sReal;
		uSub  = pData->aColumn[uSub].bNext;
		iOld  = iNow;
		iNow += iSize;
		uCol += 1;

		if(uCol==1  )iNow -= 1;
		if(iNow<iOld)iNow  = iOld;
		if(uSub==pData->uColumnCount)
		if(iNow>=(int)pData->uSizeX-1)
			{
			iNow++;
			}

		iNum = pData->aColumnXpos[uSub];

		if(iNum==iNow)continue;
		if(iNum==0)iNum=iOld;
		if(iNum>=iNow)
			{
			iRet = iOld;
			}
		else{
			iRet = iOld;

			if(pData->uSelectedItem)						// Problem bei ausgew?lten leeren Eintr?en
				{
				   uNext = pData->aColumn[pData->uSelectedSub].bNext;
				if(uNext==uSub)
					{
					UpdateRect(pData,pData->uSelectedItem,pData->uSelectedSub);
					}
				}

			if(pData->uTrackedItem)
				{
				   uNext = pData->aColumn[pData->uTrackedSub].bNext;
				if(uNext==uSub)
					{
					UpdateRect(pData,pData->uTrackedItem,pData->uTrackedSub);
					}
				}
			}


		pData->aColumnXpos[uSub] = iNow;
		break;
		}


	while(uCol<pData->uColumnCount)							// Restliche Spalten berechen
		{
		iOld  = iNow;
		uSub  = pData->aColumnPos[uCol];
		iNow += pData->aColumn[uSub].sReal;
		uSub  = pData->aColumn[uSub].bNext;
		uCol += 1;

		if(uCol==pData->uColumnCount)
		if(iNow>=(int)pData->uSizeX-1)
			{
			iNow++;
			}

		pData->aColumnXpos[uSub] = iNow;
		}

	pData->aColumnXpos[pData->uColumnCount+1] = pData->uSizeX+1;



return iRet;
}

//*****************************************************************************
//*
//*		TreeListSetOrderArray
//*
//*****************************************************************************
//	Stellt die anzeige Reihenfolge der Spalten ein
//	pData		: Zeiger auf die Fensterdaten
//	uItems		: Ist die Nummer des Eintrages
//	pArray		: Zeiger auf die Eintr?e. Null steht f? die Standartreihenfolge.
//				  z.B. {0,2,1} meint die sichtbare Reihenfolge Col0,Col2,Col1
//				  Der erste Eintrag muss 0 immer sein.
//	Ergibt   1 = Ok
//			 0 = Fehler
static int TreeListSetOrderArray(TreeListData *pData,unsigned uItems,unsigned *pArray)
{
BYTE		aFlags[MAX_COLUMNS+1];
UINT		aArray[MAX_COLUMNS+1];
TV_COLSIZE	sNotify;
UINT		uDiff;
UINT		uCol;
UINT 		uSub;


	if(!pArray)												// Spezialreihenfolge setzen
		{
		if(uItems==FROM_HEADER)								// Array aus Header holen
			{
			if(!Header_GetOrderArray(pData->hHeader,pData->uColumnCount,aArray))
				{
				return 0;
				}

			if(aArray[0]!=0)
				{
				return 0;
				}
			}
		else{
			for(uCol=pData->uColumnCount;uCol>0;uCol++)		// Standartreihenfolge
				{
				uCol--;
				aArray[uCol] = uCol;
				}
			}

		uItems = pData->uColumnCount;
		pArray = aArray;
		}
	else{													// Pr?e Array
		if(pData->uColumnCount!=uItems || uItems==0 || *pArray)
			{
			return 0;
			}
		}


	memset(aFlags,0,sizeof(aFlags)-1);

	for(uCol=0,uDiff=0;uCol<uItems;uCol++)					// Die Eintr?e pr?en
		{
		   uSub	= pArray[uCol];
		if(uSub>=uItems)return 0;
		if(aFlags[uSub])return 0;

		aFlags[uSub] = (BYTE)uCol;

		uDiff |= uCol^pData->aColumnPos[uSub];
		}

	if(uDiff==0)											// Alles blieb gleich
		{
		return 1;
		}

	aFlags[0     ] = 0;
	aFlags[uItems] = (BYTE)uItems;

	for(uCol=1;uCol<uItems;uCol++)							// Die Eintr?e anpassen
		{
		pData->aColumnPos[uCol]		= (BYTE)pArray[uCol];
		}

	for(uCol=0;uCol<uItems;uCol++)
		{
		uSub						= aFlags[uCol];
		pData->aColumn[uCol].bIndex	= (BYTE)uSub;
		pData->aColumn[uCol].bNext	= pData->aColumnPos[uSub+1];
		}

	Header_SetOrderArray(pData->hHeader,uItems,pArray);
	UpdateColumns(pData);
	UpdateView(pData);


	if(pData->uStyleEx&TVS_EX_HEADERCHGNOTIFY)				// Alle Spalten haben sich ver?dert
		{
		UNLOCK(pData);

		for(uCol=0;uCol<uItems;uCol++)
			{
 			sNotify.hdr.code	= TVN_COLUMNCHANGED;
			sNotify.uColumn		= uCol;
			sNotify.uIndex		= pData->aColumn[uCol].bIndex;
			sNotify.uPosX		= pData->aColumnXpos[uCol];
			sNotify.iSize		= pData->aColumn[uCol].sReal;

			SendNotify(pData,&sNotify.hdr);
			}

		LOCK(pData);
		}



return 1;
}

//*****************************************************************************
//*
//*		TreeListToggleItem
//*
//*****************************************************************************
//	Klappt bei einem Eintrag die Kinder um, und schickt alle Notify-Nachrichten.
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uAddFlags	: Sind die State-Flags die hinzugef?t werden sollen
//				  Bits 0..3 enthalten das Kommando, bei automatisch ermitteln
//	Ergibt  -1 = Fehler
//			 0 = Ausgef?rt
//			 1 = Abbruch
static int TreeListToggleItem(TreeListData *pData,unsigned uItem,unsigned uAddFlags)
{
NMTREEVIEW	sNotify;
BaseItem  **pList;
BaseItem   *pEntry;
BaseItem   *pTemp;
unsigned	uAction;
unsigned	uLevel;
unsigned	uNext;
unsigned	uSet;
LRESULT		lRet;
BOOL		bDo;



	if(uItem>pData->uTreeItemsMax)return 0;

		pList  = pData->pTreeItems;
		pEntry = pList[uItem];
	if(!pEntry)return -1;


		uAction= uAddFlags&0x0F;
	if(!uAction)
		{
		uAction	= ((pEntry->uState^TVIS_EXPANDED)&(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))? TVE_EXPAND:TVE_COLLAPSE;
		}


	if(pData->cNoExpandNf==0)								// Notify-Nachricht senden
		{
		sNotify.action				=  uAction;
		sNotify.hdr.code			=  TVN_ITEMEXPANDING;
		sNotify.itemNew.mask		=  TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
		sNotify.itemNew.hItem		=  (HTREEITEM)uItem;
		sNotify.itemNew.stateMask	=  0xFFFFFFFF;
		sNotify.itemNew.state		=  pEntry->uState;
		sNotify.itemNew.lParam		=  pEntry->lParam;
		sNotify.itemNew.pszText		=  (LPTSTR)-1;
		sNotify.itemNew.cchTextMax	=  -1;
		sNotify.itemOld.mask		=  0;
		sNotify.ptDrag.x			=  0;
		sNotify.ptDrag.y			=  0;


		UNLOCK(pData);

		lRet=SendNotify(pData,&sNotify.hdr);

		LOCK(pData);

		pList  = pData->pTreeItems;
		pEntry = pList[uItem];

		if(pEntry==0)return -1;								// Eintrag inzischen gel?cht ?
		if(lRet  !=0)return  1;								// User-Abbruch ?
		}


	if(uAction==TVE_EXPAND)									// Aufklappen
		{
		if(pEntry->uState&TVIS_EXPANDED)
			{
			bDo = FALSE;									// Nur von + auf -
			}
		else{
			pEntry->uState |= TVIS_EXPANDED;				// Kinder aufklappen
			bDo = TRUE;
			}
		}
	else{													// Zuklappen
		pEntry->uState &= ~TVIS_EXPANDED;
		bDo	   = TRUE;
		}

	pEntry->uState &= ~TVIS_EXPANDPARTIAL;
	pEntry->uState |=  uAddFlags&~0x0F;


	if(pEntry->uShowPos && bDo)
		{
		if(pEntry->uState&TVIS_EXPANDED)					// Kinderfenster aktuallisieren
			{
			uLevel			=  0;
			uNext			=  pEntry->uFirstChild;

			while(uNext)
				{
				pTemp = pList[uNext];
				pTemp->uShowPos = 0;

				if(pTemp->uFirstChild)
					{
					uNext=pTemp->uFirstChild;
					uLevel++;
					continue;
					}

				if(pTemp->uNextItem)
					{
					uNext=pTemp->uNextItem;
					continue;
					}

				if(uLevel==0)break;

				uNext = pList[pTemp->uParent]->uNextItem;
				uLevel--;
				}
			}

		UpdateItems(pData,uItem);
		}


	if(pData->cNoExpandNf==0)								// Notify-Nachricht senden
		{
		sNotify.action				=  uAction;
		sNotify.hdr.code			=  TVN_ITEMEXPANDED;
		sNotify.itemNew.mask		=  TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
		sNotify.itemNew.hItem		=  (HTREEITEM)uItem;
		sNotify.itemNew.stateMask	=  0xFFFFFFFF;
		sNotify.itemNew.state		=  pEntry->uState;
		sNotify.itemNew.lParam		=  pEntry->lParam;
		sNotify.itemNew.pszText		=  (LPTSTR)-1;
		sNotify.itemNew.cchTextMax	=  -1;
		sNotify.itemOld.mask		=  0;
		sNotify.ptDrag.x			=  0;
		sNotify.ptDrag.y			=  0;

		UNLOCK(pData);

		SendNotify(pData,&sNotify.hdr);

		LOCK(pData);

		pList  = pData->pTreeItems;
		pEntry = pData->pTreeItems[uItem];

		if(!pEntry)return -1;								// Eintrag inzischen gel?cht ?
		}

	if(uAction==TVE_EXPAND)									// ONCE setzen nach Expandieren
		{
		pEntry->uState |= TVIS_EXPANDEDONCE;
		}
	else if(pData->uSelectedItem && bDo)					// War der ausgew?lten Eintrag vorher sichtbar ?
		{
			pEntry = pList[pData->uSelectedItem];
		if(!pEntry)
			{
			pData->uSelectedItem = 0;
			pData->uSelectedSub	 = 0;
			}
		else if(!pEntry->uShowPos)
			{
			uSet = 0;

			while(pEntry && !pEntry->uShowPos)
				{
				if(uItem==pData->uSelectedItem)
					{
					uSet = 1;
					}

				uItem  = pEntry->uParent;
				pEntry = pList[uItem];
				}

			if(uItem && uSet)
				{
				TreeListSelectItem(pData,uItem,pData->uSelectedSub,TVC_UNKNOWN);
				}
			}
		}

	if(bDo==FALSE)											// Nur von + auf -
		{
		UpdateRect(pData,uItem,0);
		}



return 0;
}

//*****************************************************************************
//*
//*		TreeListGetItemRect
//*
//*****************************************************************************
//	Holt das Rechteck eines Eintrages
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uFlags		: Bit 0   : 0=volle Zeile  1=nur Text
//				  Bit 7   : 1=nur Spalte
//				  Bit 24.. : Spaltennummer
//	Ergibt 1 wenn der Eintrag sichtbar war
static int TreeListGetItemRect(TreeListData *pData,unsigned uItem,unsigned uFlags,RECT *pRect)
{
ExtraItem  *pExtra;
BaseItem   *pEntry;
unsigned	uNext;
unsigned	uPos;
unsigned	uSub;



	if(uItem>pData->uTreeItemsMax)
		{
		memset(pRect,0,sizeof(RECT));
		return 0;
		}

	    pEntry = pData->pTreeItems[uItem];
	if(!pEntry->uShowPos)									// Ist der Eintrag aufgeklappt
		{
		memset(pRect,0,sizeof(RECT));
		return 0;
		}


	uPos = pEntry->uShowPos-pData->uScrollY-1;

	if(pData->uFixedLines)									// Gibt es fixierte Linien
		{
		if(pEntry->uShowPos<=pData->uFixedLines)
			{
			uPos = pEntry->uShowPos-1;
			}
		else if(uPos<pData->uFixedLines)
			{
			memset(pRect,0,sizeof(RECT));
			return 0;
			}
		}

	if(uPos>=pData->uMaxEnties)								// Eintrag im Fenster sichtbar
		{
		memset(pRect,0,sizeof(RECT));
		return 0;
		}


	pRect->top     = pData->uStartPixel;
	pRect->top    += pData->iRowHeight*uPos;
	pRect->bottom  = pData->iRowHeight+pRect->top;

	if((uFlags&0xFC)==TVIR_GETCOLUMN)						// Nur Spalten
		{
		   uSub = uFlags>>24;
		if(uSub>=pData->uColumnCount)uSub=0;

		uNext		   = pData->aColumn[uSub].bNext;
		pRect->left    = pData->aColumnXpos[uSub];
		pRect->left   -= pData->uScrollX;
		pRect->right   = pData->aColumnXpos[uNext];
		pRect->right  -= pData->uScrollX;
		}
	else{
		uSub		   = 0;
		pRect->left    = 0;
		pRect->left   -= pData->uScrollX;
		pRect->right   = pData->uSizeX;
		}


	if(uFlags&TVIR_TEXT)									// Nur Text ausgeben
		{
		if(uSub>0)
			{
			pExtra =  pData ->pExtraItems[uSub-1][uItem];

			if(pData->aColumn[uSub].bEdit==TVAX_CHECK)
				{
				   pRect->left+=pData->iChecksSizeX;
				if(pRect->left>pRect->right)pRect->left=pRect->right;
				}
			else if(pExtra && pExtra->bFlags&TVIX_HASIMAGE)
				{
				   pRect->left+=pData->iImagesSizeX;
				if(pRect->left>pRect->right)pRect->left=pRect->right;
				}
			}
		else{
			if(pData->cHasRootRow)							// Root-Linien ausgleichen
				{
				pRect->left += pData->iIndent;
				}

			pRect->left += pData->iIndent*pEntry->uLevel;

			if(pData->hStates)
				{
				pRect->left += pData->iStatesSizeX;
				}

			if(!(pData->uStyle&TVS_HASLINES))
				{
				pRect->left -= 1;
				}

			if(pData->uStyleEx&TVS_EX_ITEMLINES)
				{
				pRect->left += 1;
				if(pEntry->bFlags&TVIX_HASIMAGE)pRect->left++;
				}

			if(pEntry->bFlags&TVIX_HASIMAGE)
				{
				pRect->left += pData->iImagesSizeX;
				}

			if(pRect->left>pRect->right)
				{
				pRect->left=pRect->right;
				}
			}
		}



return 1;
}

//*****************************************************************************
//*
//*		TreeListEnsureVisible
//*
//*****************************************************************************
//	Macht einen Eintrag sichtbar
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uSub		: Untereintrag der sichtbar sein soll
//						0xFFFFFFFF nur Zeile
//						FIRST_LINE als oberster Eintrag
//	Ergibt 1 wenn nur zum Eintrag gescrollt wurde bzw. 0 wenn aufgeklapt wurde
static int TreeListEnsureVisible(TreeListData *pData,unsigned uItem,unsigned uSub)
{
BaseItem   *pEntry;
BaseItem   *pTemp;
unsigned	uTemp;
unsigned	uNext;
unsigned	uPos;
unsigned	uCmp;
int			iNum;
int			iAnf;
int			iOff;
int			iEnd;
int			iMax;
int			iRet;



	if(uItem>pData->uTreeItemsMax)return -1;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return -1;

		uPos = pEntry->uShowPos;
	if(!uPos)												// Zweige aufklappen wenn Eintrag zugeklappt
		{
		iRet = 0;

		for(pTemp=pEntry;;)
			{
				uTemp = pTemp->uParent;
				pTemp = pData->pTreeItems[uTemp];
			if(!pTemp)break;
			if((pTemp->uState&TVIS_EXPANDED)==0)
				{
				if(TreeListToggleItem(pData,uTemp,0))return 0;
				}
			}

			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;

			uPos = pEntry->uShowPos;
		if(!uPos)return 0;
		}
	else{													// Nur Scrollen
		iRet = 1;
		}


	uPos -= 1;
	uCmp  = pData->uScrollY;
	uCmp += pData->uFixedLines;

	if(uPos<pData->uFixedLines)								// Ist es eine fixierte Linie
		{


		}
	else if(uPos<uCmp)										// Vor erster Zeile
		{
		uPos -= pData->uFixedLines;
		pData->uScrollY	= uPos;
		SetScrollPos(pData->hWnd,SB_VERT,uPos,TRUE);
		UpdateView(pData);
		}
	else if(uSub==FIRST_LINE)								// Als ersten Eintrag
		{
		if(uPos!=uCmp)
			{
			uPos -= pData->uFixedLines;
			pData->uScrollY	= uPos;
			SetScrollPos(pData->hWnd,SB_VERT,uPos,TRUE);
			UpdateView(pData);
			}

		return iRet;
		}
	else if(uPos>=pData->uScrollY+pData->uPageEnties)		// Nach letzter Zeile
		{
		iOff  = uPos-(pData->uPageEnties-1);
		iMax  = pData->uItemPosCount;
		iMax -=	pData->uPageEnties-1;

		if(iOff>=iMax)iOff=iMax;
		if(iOff<0)iOff=0;
		if(iOff!=(int)pData->uScrollY)
			{
			pData->uScrollY	= iOff;
			SetScrollPos(pData->hWnd,SB_VERT,iOff,TRUE);
			UpdateView(pData);
			}
		}


	if(uSub<pData->uColumnCount)							// Horizontal einrichten
		{
		uNext = pData->aColumn[uSub].bNext;
		iNum  = pData->uSizeX;
		iOff  = pData->uScrollX;
		iAnf  = pData->aColumnXpos[uSub ];
		iEnd  = pData->aColumnXpos[uNext];

		if(iOff+iNum< iAnf)iOff=iAnf;
		if(iOff     >=iEnd)iOff=iAnf;
		if(iOff+iNum< iEnd)iOff=iEnd-iNum;
		if(iOff     > iAnf)iOff=iAnf;

		iMax  = pData->aColumnXpos[pData->uColumnCount];
		iMax -= pData->uSizeX/2;

		if(iOff>iMax)iOff=iMax;
		if(iOff<   0)iOff=0;
		if(iOff!=(int)pData->uScrollX)
			{
			pData->uScrollX	= iOff;
			SetScrollPos(pData->hWnd,SB_HORZ,iOff,TRUE);
			UpdateView(pData);
			MoveWindow(pData->hHeader,-iOff,0,iNum+iOff,pData->uStartPixel,TRUE);
			}
		}


return iRet;
}

//*****************************************************************************
//*
//*		TreeListIsVisible
//*
//*****************************************************************************
//	Pr?t ob ein Eintrag sichtbar ist
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uSub		: Untereintrag der gepr?t werden soll
//						0xFFFFFFFF nur Zeile pr?en
//	Ergibt den Zustand des Eintrages:
//				   -1 = Unbekannter Eintrag
//					0 = Eintrag ist zugeklappt
//					1 = Eintrag ist aufgeklappt aber nicht sichtbar
//					2 = Eintrag ist aufgeklappt und teilweise sichtbar
//					3 = Eintrag ist aufgeklappt und Spalte ist nur teilweise sichtbar
//					4 = Eintrag ist aufgeklappt und ganz sichtbar
static int TreeListIsVisible(TreeListData *pData,unsigned uItem,unsigned uSub)
{
BaseItem   *pEntry;
unsigned	uNext;
unsigned	uPos;
int			iNum;
int			iAnf;
int			iOff;
int			iEnd;



	if(uItem>pData->uTreeItemsMax)return -1;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return -1;

		uPos = pEntry->uShowPos;
	if(!uPos)												// Ist der Eintrag zugeklappt
		{
		return 0;
		}

	   uPos--;
	if(uPos<pData->uScrollY)								// Vor erster Zeile
		{
		return 1;
		}

	if(uPos>=pData->uScrollY+pData->uMaxEnties)				// Nach letzter Zeile
		{
		return 1;
		}

	if(uPos==pData->uScrollY+pData->uPageEnties)			// Auf halbsichtbarer Zeile
		{
		if(uSub<pData->uColumnCount)
			{
			uNext = pData->aColumn[uSub].bNext;
			iNum  = pData->uSizeX;
			iOff  = pData->uScrollX;
			iAnf  = pData->aColumnXpos[uSub ];
			iEnd  = pData->aColumnXpos[uNext];

			if(iOff+iNum< iAnf)return 1;
			if(iOff     >=iEnd)return 1;
			}

		return 2;
		}

	if(uSub<pData->uColumnCount)							// Spalte pr?en
		{
		uNext = pData->aColumn[uSub].bNext;
		iNum  = pData->uSizeX;
		iOff  = pData->uScrollX;
		iAnf  = pData->aColumnXpos[uSub ];
		iEnd  = pData->aColumnXpos[uNext];

		if(iOff+iNum< iAnf)return 1;
		if(iOff     >=iEnd)return 1;
		if(iOff+iNum< iEnd)return 3;
		if(iOff     > iAnf)return 3;
		}



return 4;
}

//*****************************************************************************
//*
//*		TreeListDeleteItem
//*
//*****************************************************************************
//	L?cht einen Eintrag aus dem Fenster
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der gel?cht werden soll
//	iMode		: Wie soll der Eintrag gel?cht werden
//					0 = Eintrag l?chen und nicht neu zeichnen
//					1 = Eintrag l?chen und neu zeichnen
//					2 = Nur Kindereintr?e l?chen und neu zeichnen
//					4 = Eintrag l?chen und nicht neu zeichnen (Remote)
//					5 = Eintrag l?chen und neu zeichnen (Remote)
//					6 = Nur Kindereintr?e l?chen und neu zeichnen (Remote)
//	Ergibt 1 wenn der Eintrag gel?cht wurde.
static int TreeListDeleteItem(TreeListData *pData,unsigned uItem,int iMode)
{
NMTREEVIEW	sNotify;
ExtraItem **pList;
ExtraItem  *pExtra;
BaseItem   *pEntry;
BaseItem   *pTemp;
unsigned	uPos;
int			iOff;
int			iMax;


	if(pData->cLockChanges)return 0;



	if(uItem>pData->uTreeItemsMax)							// Pr?e den Eintrag
		{
		if(uItem != U(TVI_ROOT))return 0;					// Alles l?chen
		if(pData->uLastChild==0)return 0;

		while(pData->uLastChild)
			{
			TreeListDeleteItem(pData,pData->uLastChild,iMode&4);
			}

		pData->uItemPosCount = 0;

		if(iMode&4)											// Remote Start
			{
			UpdateStart(pData,UPDATE_SCRLY);
			UpdateStart(pData,UPDATE_VIEW);
			}
		else{
			UpdateScrollY(pData);
			UpdateView   (pData);
			}

		return 1;
		}

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)												// Pr?e den Eintrag
		{
		return 0;
		}


	if(iMode&2)												// Nur Kindereintr?e l?chen
		{
		if(!pEntry->uFirstChild)
			{
			return 0;
			}

		while(pEntry->uLastChild)							// Alle Kinder l?chen
			{
			TreeListDeleteItem(pData,pEntry->uLastChild,iMode&4);
			}

		   uPos = pEntry->uShowPos;
		if(uPos)
			{
			if(iMode&4)										// Remote Start
					UpdateStart(pData,UPDATE_ITEMS,uItem);
			else	UpdateItems(pData,uItem);
			}

		return 1;
		}



	while(pEntry->uLastChild)								// Alle Kinder l?chen
		{
		TreeListDeleteItem(pData,pEntry->uLastChild,iMode&4);
		}


	if((pEntry->uState&TVIS_SELECTED) && (iMode&4))			// Einen ausgew?lten Eintrag l?chen
		{
		UpdateStart(pData,UPDATE_UNSEL,uItem,pEntry->lParam);
		}
	else if(pEntry->uState&TVIS_SELECTED)
		{
		sNotify.hdr.code			= TVN_SELCHANGED;
		sNotify.action				= TVC_UNKNOWN;
		sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
		sNotify.itemOld.hItem		= (HTREEITEM)uItem;
		sNotify.itemOld.stateMask	= 0xFFFFFFFF;
		sNotify.itemOld.state		= pEntry->uState&~TVIS_SELECTED;
		sNotify.itemOld.lParam		= pEntry->lParam;
		sNotify.itemOld.cChildren	= 0;
		sNotify.itemOld.pszText		= (LPTSTR)-1;
		sNotify.itemOld.cchTextMax	= -1;
		sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
		sNotify.itemNew.hItem		= NULL;
		sNotify.itemNew.stateMask	= 0xFFFFFFFF;
		sNotify.itemNew.state		= 0;
		sNotify.itemNew.lParam		= 0;
		sNotify.itemNew.cChildren	= 0;
		sNotify.itemNew.pszText		= (LPTSTR)-1;
		sNotify.itemNew.cchTextMax	= -1;
		sNotify.ptDrag.x			= 0;
		sNotify.ptDrag.y			= 0;

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);						// Bekant geben das der Eintrag nicht mehr ausgew?lt ist
		LOCK(pData);

		if(pData->uSelectedItem==uItem)
			{
			pData->uSelectedItem = 0;
			pData->uSelectedSub  = 0;
			}
		}


	if(!(iMode&4))
		{
		sNotify.hdr.code			= TVN_DELETEITEM;
		sNotify.itemNew.mask		= 0;
		sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
		sNotify.itemOld.hItem		= (HTREEITEM)uItem;
		sNotify.itemOld.lParam		= pEntry->lParam;
		sNotify.itemOld.pszText		= (LPTSTR)-1;
		sNotify.itemNew.stateMask	= 0xFFFFFFFF;
		sNotify.itemNew.state		= pEntry->uState;
		sNotify.itemOld.cchTextMax	= -1;
		sNotify.ptDrag.x			= 0;
		sNotify.ptDrag.y			= 0;

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);
		LOCK(pData);
		}

		pEntry = pData->pTreeItems[uItem];					// Pr?en ob der Eintrag noch existiert
	if(!pEntry)return 0;



	if(uItem==pData->uTrackedItem)							// Einen unterstrichenen Eintrag l?chen
		{
		pData->uTrackedItem  = 0;
		pData->uTrackedSub   = 0;
		}

	if(pData->uInsertMark==uItem)
		{
		pData->uInsertMark = 0;
		}

	if(pData->uSingleSel==uItem)
		{
		pData->uSingleSel = 0;
		}

	if(pEntry->uPrevItem)									// Gibt es einen vorherigen Eintrag
		{
		pTemp			 = pData->pTreeItems[pEntry->uPrevItem];
		pTemp->uNextItem = pEntry->uNextItem;
		}
	else{
		if(pEntry->uParent)									// Neues erstes Kind in Elterneintrag
			{
			pTemp			   = pData->pTreeItems[pEntry->uParent];
			pTemp->uFirstChild = pEntry->uNextItem;
			}
		else{
			pData->uFirstChild = pEntry->uNextItem;
			}
		}

	if(pEntry->uNextItem)									// Gibt es einen vorherigen Eintrag
		{
		pTemp			 = pData->pTreeItems[pEntry->uNextItem];
		pTemp->uPrevItem = pEntry->uPrevItem;
		}
	else{
		if(pEntry->uParent)									// Neues letztes Kind in Elterneintrag
			{
			pTemp			  = pData->pTreeItems[pEntry->uParent];
			pTemp->uLastChild = pEntry->uPrevItem;

			if(pTemp->uFirstChild==0 && pTemp->uLastChild==0)
				{
				pTemp->bFlags &= ~TVIX_HASBUTTON;
				}
			}
		else{
			pData->uLastChild = pEntry->uPrevItem;
			}
		}


	for(uPos=1;uPos<pData->uColumnCount;uPos++)				// Alle Extraeintr?e l?chen
		{
		pList = pData->pExtraItems[uPos-1];

			pExtra = pList[uItem];
		if(!pExtra)continue;

		pList[uItem] = NULL;

		if(pExtra->pText)
			{
			pExtra->uTextSize=0;
			delete pExtra->pText;
			}

		delete pExtra;
		}


	pData->pTreeItems[uItem] = NULL;						// Den Eintrag l?chen

	if(pEntry->pText)
		{
		pEntry->uTextSize=0;
		delete pEntry->pText;
		}


	if(iMode&1)												// Den Eintrag neuzeichnen
		{
			uItem =   pEntry->uPrevItem;
		if(!uItem && !pEntry->uNextItem)
			{
				uItem = pEntry->uParent;
			if(!uItem)uPos=1;
			else uPos = pData->pTreeItems[uItem]->uShowPos;
			}
		else{
			uPos = pEntry->uShowPos;
			}

		if(uPos)
			{
			if(iMode&4)										// Remote Start
					UpdateStart(pData,UPDATE_ITEMS,uItem);
			else	UpdateItems(pData,uItem);
			}
		}



	if(pEntry->uState&TVIS_SELECTED)						// Ausgew?lte Eintr?e runterz?len
	if(pData->uSelectedCount>0)
		{
		pData->uSelectedCount--;
		}

	delete pEntry;

	pData->uTreeItemsCount--;


	iOff  = pData->uScrollY;								// Pr?e die Scrollposition
	iMax  = pData->uItemPosCount;
	iMax -=	pData->uPageEnties-1;

	if(iOff>=iMax)iOff=iMax;
	if(iOff<0)iOff=0;
	if(iOff!=(int)pData->uScrollY)
		{
		pData->uScrollY	= iOff;

		if(iMode&4)											// Remote Start
			{
			UpdateStart(pData,UPDATE_SCROLL,SB_VERT,iOff);
			UpdateStart(pData,UPDATE_VIEW,uItem);
			}
		else{
			SetScrollPos(pData->hWnd,SB_VERT,iOff,TRUE);
			UpdateView(pData);
			}
		}



return 1;
}


//*****************************************************************************
//*
//*		TreeListXorSelectItem
//*
//*****************************************************************************
//	W?lt einen Eintrag ab bzw. an
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der ausgew?lt werden soll
//	iMode		: Ist der Grund f? die Änderung
//						TVC_BYKEYBOARD
//						TVC_BYMOUSE
//						TVC_UNKNOWN
//	Ergibt		1 wenn der Eintrag ab/angew?lt wurde
//				0 wenn der Eintrag nicht ver?dert wurde
static int TreeListXorSelectItem(TreeListData *pData,unsigned uItem,int iMode)
{
NMTREEVIEW	sNotify;
BaseItem   *pEntry;
unsigned	uOld;
unsigned	uRet;



		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;
	if(uItem==pData->uSelectedItem)return 0;

	uOld = pEntry->uState;

	sNotify.hdr.code			= TVN_SELCHANGING;
	sNotify.action				= iMode;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		= pEntry->uState;
	sNotify.itemNew.lParam		= pEntry->lParam;
	sNotify.itemNew.cChildren	= 0;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
	sNotify.itemOld.hItem		= NULL;
	sNotify.itemOld.stateMask	= 0xFFFFFFFF;
	sNotify.itemOld.state		= 0;
	sNotify.itemOld.lParam		= 0;
	sNotify.itemOld.cChildren	= 0;
	sNotify.itemOld.pszText		= (LPTSTR)-1;
	sNotify.itemOld.cchTextMax	= -1;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	UNLOCK(pData);
	uRet=U(SendNotify(pData,&sNotify.hdr));
	LOCK(pData);

	if(uRet)return 0;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;
		pEntry->uState ^= TVIS_SELECTED;

	sNotify.hdr.code			= TVN_SELCHANGED;
	sNotify.action				= iMode;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		= pEntry->uState;
	sNotify.itemNew.lParam		= pEntry->lParam;
	sNotify.itemNew.cChildren	= 0;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
	sNotify.itemOld.hItem		= NULL;
	sNotify.itemOld.stateMask	= 0xFFFFFFFF;
	sNotify.itemOld.state		= 0;
	sNotify.itemOld.lParam		= 0;
	sNotify.itemOld.cChildren	= 0;
	sNotify.itemOld.pszText		= (LPTSTR)-1;
	sNotify.itemOld.cchTextMax	= -1;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	UNLOCK(pData);
	SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;
	if( pEntry->uShowPos)
		{
		if(pData->uStyleEx&TVS_EX_FULLROWMARK)
				UpdateRow (pData,uItem);
		else	UpdateRect(pData,uItem,0);
		}

	if((uOld^pEntry->uState)&TVIS_SELECTED)
		{
		if(pEntry->uState&TVIS_SELECTED)
				pData->uSelectedCount++;
		else	pData->uSelectedCount--;
		}


return 1;
}


//*****************************************************************************
//*
//*		TreeListRemoveFocus
//*
//*****************************************************************************
//	W?lt den Focus ab
//	pData		: Zeiger auf die Fensterdaten
static void TreeListRemoveFocus(TreeListData *pData)
{
ExtraItem  *pExtra;
BaseItem   *pEntry;
unsigned	uItem;
unsigned	uSub;


	if(!pData->uFocusItem)return;

	uItem  = pData->uFocusItem;
	pEntry = pData->pTreeItems[uItem];

	if(pEntry)
		{
		pEntry->bFlags &= ~TVIX_FOCUSED;

		uSub = pData->uFocusSub;

		if(uSub)
			{
			   pExtra= pData->pExtraItems[uSub-1][uItem];
			if(pExtra)pExtra->bFlags &= ~TVIX_FOCUSED;
			}

		UpdateRect(pData,uItem,uSub);
		}

	pData->uFocusItem = 0;
	pData->uFocusSub  = 0;

}

//*****************************************************************************
//*
//*		TreeListSetFocus
//*
//*****************************************************************************
//	W?lt den Focus-Eintrag
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Eintrag f? den Focus (0xFFFFFFFF=keine Änderung)
//	uSub		: Spalte f? den Focus	(0xFFFFFFFF=keine Änderung)
//	Ergibt 1 wenn der Focus gesetzt wurde, bzw 0 bei einem Fehler
static int  TreeListSetFocus(TreeListData *pData,unsigned uItem,unsigned uSub)
{
ExtraItem  *pExtra;
BaseItem   *pEntry;
BaseItem   *pTemp;
unsigned	uTemp;
unsigned	uCol;



	if(pData->uFocusItem)
		{
		if(uSub ==0xFFFFFFFF)uSub =pData->uFocusSub;
		if(uItem==0xFFFFFFFF)uItem=pData->uFocusItem;
		}
	else{
		if(uSub ==0xFFFFFFFF)uSub =pData->uSelectedSub;
		if(uItem==0xFFFFFFFF)uItem=pData->uSelectedItem;
		}


	if(pData->uFocusItem==uItem)
	if(pData->uFocusSub ==uSub )return 1;


	if(!uItem)												// Focus abw?len
		{
		TreeListRemoveFocus(pData);
		return 1;
		}


	if(uItem>pData->uTreeItemsMax)							// Den Eintrag pr?en
		{
		return 0;
		}

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)
		{
		return 0;
		}

	if(!(pData->uStyleEx&TVS_EX_SUBSELECT))uSub=0;

	if(!(pData->uStyleEx&TVS_EX_MULTISELECT))				// Einzel auswahl
		{
		return TreeListSelectItem(pData,uItem,uSub,TVC_UNKNOWN);
		}


	uTemp  = pData->uFocusItem;
	pTemp  = pData->pTreeItems[uTemp];

	if(pTemp)												// Den alten Eintrag abw?len
		{
		pTemp->bFlags &= ~TVIX_FOCUSED;
		uCol = pData->uFocusSub;

		if(uCol)
			{
			   pExtra= pData->pExtraItems[uCol-1][uTemp];
			if(pExtra)pExtra->bFlags &= ~TVIX_FOCUSED;
			}

		UpdateRect(pData,uItem,uSub);
		}


	if(uSub)												// Neuen Eintrag w?len
		{
		   pExtra =pData->pExtraItems[uSub-1][uItem];
		if(pExtra)pExtra->bFlags |= TVIX_FOCUSED;
		}
	else{
		pEntry->bFlags |= TVIX_FOCUSED;
		}


	pData->uFocusItem = uItem;
	pData->uFocusSub  = uSub;


	if(pEntry->uState&TVIS_SELECTED)						// Auch die Auswahl nachziehen
		{
		if(pData->uSelectedItem!=uItem)
			{
			uTemp = pData->uSelectedItem;
			uCol  = pData->uSelectedSub;
			}
		else{
			uTemp = 0;
			uCol  = pData->uSelectedSub;
			}

		pData->uSelectedItem = uItem;
		pData->uSelectedSub  = uSub;

		if(pData->uStyleEx&TVS_EX_FULLROWMARK)
			{
			uCol = uSub+1;
			}

		if(uTemp)
			{
			if(uCol!=uSub)
					UpdateRow (pData,uTemp);
			else	UpdateRect(pData,uTemp,uCol);
			}


		if(uCol!=uSub)
				UpdateRow (pData,uItem);
		else	UpdateRect(pData,uItem,uCol);
		}
	else{
		UpdateRect(pData,uItem,uSub);
		}



return 1;
}

//*****************************************************************************
//*
//*		TreeListSelectItem
//*
//*****************************************************************************
//	W?lt einen Eintrag aus
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der ausgew?lt werden soll
//	uSubItem	: Ist die Spalte die gew?lt werden soll
//	iMode		: Ist der Grund f? die Änderung
//						TVC_BYKEYBOARD
//						TVC_BYMOUSE
//						TVC_UNKNOWN
//						TVC_ONLYFOCUS	(nur der Focus hat sich ver?dert)
//						TVC_DESELECT	(dieses Flag l?cht die alte Auswahl)
//						TVC_UNSELECT	(dieses Flag l?cht Auswahl bei MultiSel)
//	Ergibt		2 wenn der Eintrag gew?lt und umgeklapt wurde
//				1 wenn der Eintrag gew?lt wurde
//				0 wenn der Eintrag nicht gew?lt wurde
static int TreeListSelectItem(TreeListData *pData,unsigned uItem,unsigned uSubItem,int iMode)
{
NMTREEVIEW	sNotify;
ExtraItem  *pExtra;
BaseItem   *pEntry;
BaseItem   *pTemp;
LPARAM		lParam;
LPARAM		lPaOld;
unsigned	uState;
unsigned	uStOld;
unsigned	uNext;
unsigned	uPos;
unsigned	uOld;
unsigned	uSub;
unsigned	uRet;
int			iDel;
int			iSel;



	uOld = pData->uSelectedItem;
	uSub = pData->uSelectedSub;


	if(uSubItem>=pData->uColumnCount && uSubItem>0)return 0;
	if(uItem   > pData->uTreeItemsMax)return 0;
	if(uItem   ==uOld)
	if(uSubItem==uSub)
	if(pData->uSelectedCount<=1 || !(pData->uStyleEx&TVS_EX_MULTISELECT))
		{
		return 1;
		}


	if(pData->uStyleEx&TVS_EX_MULTISELECT)					// Ist die Mehrfachauswahl m?lich
		{
			iSel = iMode&TVC_UNSELECT;
			iDel = iMode&TVC_DESELECT;
		if(!iDel)
			{
			if(pData->uStyleEx&(TVS_EX_FULLROWMARK|TVS_EX_SUBSELECT))
					UpdateRow (pData,uOld);
			else	UpdateRect(pData,uOld,uSub);

			uOld = 0;
			uSub = 0;
			}
		else{												// Alle gew?lten Eintr?e abw?len
			if(pData->uSelectedCount>1 && pData->uTreeItemsMax)
				{
				for(uPos=pData->uTreeItemsMax;uPos;uPos--)
					{
					pEntry = pData->pTreeItems[uPos];
					if(!pEntry || !(pEntry->uState&TVIS_SELECTED))continue;
					if(TreeListXorSelectItem(pData,uPos,iMode))
					if(!pData->uSelectedCount)break;		// Wurden alle Eintr?e abgew?lt
					}
				}
			}
		}
	else{													// Altes Select l?chen
		iMode &= ~TVC_ONLYFOCUS;
		iDel   =  1;
		iSel   =  0;
		}

	iMode &= ~(TVC_DESELECT|TVC_UNSELECT);


		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)												// Neuen Statatus holen
		{
		if(uItem)return 0;
		uState = 0;
		lParam = 0;
		}
	else{
		uState = pEntry->uState;
		lParam = pEntry->lParam;

		if(uSubItem)
			{
			uState &= TVIS_BASEFLAGS;
			pExtra  = pData->pExtraItems[uSubItem-1][uItem];
			if(pExtra)uState|=pExtra->uState;
			}
		}

		pTemp = pData->pTreeItems[uOld];
	if(!pTemp)												// Alten Status holen
		{
		uStOld = 0;
		lPaOld = 0;
		}
	else{
		uStOld = pTemp->uState;
		lPaOld = pTemp->lParam;

		if(uSub)
			{
			uStOld &= TVIS_BASEFLAGS;
			pExtra  = pData->pExtraItems[uSub-1][uOld];
			if(pExtra)uStOld|=pExtra->uState;
			}
		}


	sNotify.hdr.code			= TVN_SELCHANGING;
	sNotify.action				= iMode;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		= uState;
	sNotify.itemNew.lParam		= lParam;
	sNotify.itemNew.cChildren	= uSubItem;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
	sNotify.itemOld.hItem		= (HTREEITEM)uOld;
	sNotify.itemOld.stateMask	= 0xFFFFFFFF;
	sNotify.itemOld.state		= uStOld;
	sNotify.itemNew.lParam		= lPaOld;
	sNotify.itemOld.cChildren	= uSub;
	sNotify.itemOld.pszText		= (LPTSTR)-1;
	sNotify.itemOld.cchTextMax	= -1;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;


	UNLOCK(pData);

	if(SendNotify(pData,&sNotify.hdr))						// Abfragen ob der Eintrag gew?lt werden darf
		{
		LOCK(pData);
		return 0;
		}

	LOCK(pData);

	if(uItem)												// Pr?en ob der Eintrag noch existiert
		{
			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		}

	if(iDel)
		{
		uOld    = pData->uSelectedItem;
		pTemp	= pData->pTreeItems[uOld];
		}


	if(pTemp)												// Den alten Eintrag abw?len
		{
		if(pTemp->uShowPos)									// Den Eintrag neu zeichnen
			{
			if((pData->uStyleEx&TVS_EX_FULLROWMARK) || pData->uSelectedSub)
					UpdateRow (pData,uOld);
			else	UpdateRect(pData,uOld,uSub);
			}

		if(pTemp->uState&TVIS_SELECTED)
			{
			uStOld		  &= ~TVIS_SELECTED;
			pTemp->uState &= ~TVIS_SELECTED;

			if(pData->uSelectedCount>0)
				{
				pData->uSelectedCount -= 1;
				}
			}

		pData->uSelectedSub    = 0;
		pData->uSelectedItem   = 0;
		}
	else{
		uOld  = 0;
		}


	if(uItem)												// Den neuen Eintrag w?len
		{
		if(iSel)
			{
			if(pEntry->uState&TVIS_SELECTED)
				{
				uState				  &= ~TVIS_SELECTED;
				pEntry->uState        &= ~TVIS_SELECTED;
				if(pData->uSelectedCount)pData->uSelectedCount--;
				}
			}
		else{
			if(!(pEntry->uState&TVIS_SELECTED))
				{
				uState				  |= TVIS_SELECTED;
				pEntry->uState        |= TVIS_SELECTED;
				pData->uSelectedCount += 1;
				}
			}

		if(uSubItem && uSubItem<pData->uColumnCount)
			{
				pExtra = pData->pExtraItems[uSubItem-1][uItem];
			if(!pExtra)
				{
				pExtra  = new ExtraItem;
				memset(pExtra,0,sizeof(ExtraItem));
				pExtra->iImage = TV_NOIMAGE;
				pExtra->uState = pEntry->uState&(TVIS_BOLD|TVIS_UNTERLINE);
				pData->pExtraItems[uSubItem-1][uItem] = pExtra;
				}

			uState  = pExtra->uState;
			uState |= pEntry->uState&TVIS_BASEFLAGS;
			}
		else{
			uState  = pEntry->uState;
			}

		if(pEntry->uShowPos)								// Den Eintrag neu zeichnen
			{
			if(pData->uStyleEx&(TVS_EX_FULLROWMARK|TVS_EX_SUBSELECT))
					UpdateRow (pData,uItem);
			else	UpdateRect(pData,uItem,uSubItem);
			}

		pData->uSelectedSub  = uSubItem;
		pData->uSelectedItem = uItem;
		}
	else{
		pData->uSelectedItem = 0;
		pData->uSelectedSub  = 0;
		uState				 = 0;
		}

	sNotify.hdr.code			= TVN_SELCHANGED;
	sNotify.action				= iMode;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		= uState;
	sNotify.itemNew.lParam		= lParam;
	sNotify.itemNew.cChildren	= uSubItem;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
	sNotify.itemOld.hItem		= (HTREEITEM)uOld;
	sNotify.itemOld.stateMask	= 0xFFFFFFFF;
	sNotify.itemOld.state		= uStOld;
	sNotify.itemOld.lParam		= lPaOld;
	sNotify.itemOld.cChildren	= uSub;
	sNotify.itemOld.pszText		= (LPTSTR)-1;
	sNotify.itemOld.cchTextMax	= -1;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	UNLOCK(pData);
	SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

	if(!(pData->uStyle&TVS_SINGLEEXPAND))					// Einzelmodus aktiv
		{
		if(pData->uStyle&TVS_SHOWSELALWAYS)
		if(pData->uSelectedItem)
			{
			TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			}

		return 1;
		}


//*****************************************************************************


	sNotify.hdr.code			= TVN_SINGLEEXPAND;
	sNotify.action				= iMode;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		= (pEntry)? pEntry->uState:0;
	sNotify.itemNew.cChildren	= 0;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
	sNotify.itemOld.hItem		= (HTREEITEM)uOld;
	sNotify.itemOld.stateMask	= 0xFFFFFFFF;
	sNotify.itemOld.state		= (pTemp)? pTemp->uState:0;
	sNotify.itemOld.cChildren	= 0;
	sNotify.itemOld.pszText		= (LPTSTR)-1;
	sNotify.itemOld.cchTextMax	= -1;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	UNLOCK(pData);
	uRet=U(SendNotify(pData,&sNotify.hdr));					// Anfragen ob die Zweige umgeklappt werden d?fen
	LOCK(pData);


	pTemp	= pData->pTreeItems[uOld ];						// Zeiger neu holen falls es Änderungen gab
	pEntry	= pData->pTreeItems[uItem];



	while(pTemp && pEntry)									// Beide Zweige sysnchronisieren
		{
		if(pEntry->uLevel>pTemp->uLevel)
			{
			uNext  = pEntry->uParent;

			if(!(uRet&TVNRET_SKIPNEW))
			if(!(pEntry->uState&TVIS_EXPANDED))
				{
				TreeListToggleItem(pData,uItem,0);
				}

			pEntry = pData->pTreeItems[uNext];
			uItem  = uNext;

			if(!uItem)break;

			continue;
			}

		if(uItem==uOld)goto EndSel;							// Bis zum gleichen Knoten

		uNext  = pTemp->uParent;

		if(!(uRet&TVNRET_SKIPOLD))
		if(pTemp->uState&TVIS_EXPANDED)
			{
			TreeListToggleItem(pData,uOld,0);
			}

		pTemp  = pData->pTreeItems[uNext];
		uOld   = uNext;
		}

	if(!uItem)
		{
		if(!(uRet&TVNRET_SKIPOLD))
		while(pTemp)										// Alten Zweig zuklappen
			{
			uNext = pTemp->uParent;

			if(pTemp->uState&TVIS_EXPANDED)
				{
				TreeListToggleItem(pData,uOld,0);
				}

			pTemp = pData->pTreeItems[uNext];
			uOld  = uNext;
			}

		goto EndSel;
		}

	if(!uOld)
		{
		if(!(uRet&TVNRET_SKIPNEW))
		while(pEntry)										// Neuen Zweig aufklappen
			{
			uNext = pEntry->uParent;

			if(!(pEntry->uState&TVIS_EXPANDED))
				{
				TreeListToggleItem(pData,uItem,0);
				}

			pEntry = pData->pTreeItems[uNext];
			uItem  = uNext;
			}
		}

EndSel:

	if(pData->uStyle&TVS_SHOWSELALWAYS)
	if(pData->uSelectedItem)
		{
		TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
		}


return 2;
}

//*****************************************************************************
//*
//*		TreeListSelectChilds
//*
//*****************************************************************************
//	W?lt einen Eintrag aus
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der ausgew?lt werden soll
//	iMode		: Bit 0 = Untereint?e auch ?dern
//				  Bit 1 = Eintr?e abw?len
//	Ergibt 1 wenn die Auswahl funktioniert hat, bzw. 0 bei einem Fehler
static int TreeListSelectChilds(TreeListData *pData,unsigned uItem,int iMode)
{
BaseItem   *pEntry;
unsigned	uLevel;
unsigned	uXor;


	if(!(pData->uStyleEx&TVS_EX_MULTISELECT))return 0;


	uLevel = 0;

	if(uItem==U(TVI_ROOT))
		{
		uItem  = pData->uFirstChild;
		}
	else{
		if(uItem>pData->uTreeItemsMax)return 0;
		}


	if(!pData->pTreeItems[uItem])
		{
		return 0;
		}


	uXor   = (iMode&TVIS_DESELECT)? 0:TVIS_SELECTED;
	iMode &= TVIS_WITHCHILDS;


	for(;;)
		{
		pEntry = pData->pTreeItems[uItem];

		if((pEntry->uState^uXor)&TVIS_SELECTED)
			{
			TreeListXorSelectItem(pData,uItem,TVC_UNKNOWN);
			}

		if(iMode && pEntry->uFirstChild)					// Auch Kinder ?dern
			{
			uItem = pEntry->uFirstChild;
			uLevel++;
			continue;
			}


		for(;;)												// Eine Ebene h?er
			{
			   uItem = pEntry->uNextItem;
			if(uItem !=0)break;
			if(uLevel==0)return 1;

			uLevel--;

			uItem  = pEntry->uParent;
			pEntry = pData->pTreeItems[uItem];
			}
		}

}

//*****************************************************************************
//*
//*		TreeListInsertItem
//*
//*****************************************************************************
//	F?t einen Eintrag ins Fenster ein
//	pData		: Zeiger auf die Fensterdaten
//	pInsert		: Zeiger auf die ein zu f?enden Daten
//	Ergibt die Einf?eposition des neuen Eintrages oder 0 bei einem Fehler
static unsigned TreeListInsertItem(TreeListData *pData,TV_INSERTSTRUCT *pInsert)
{
char	   *pTemp;
BYTE		bFlag;
LPCTSTR	    pText;
LPCTSTR	    pTextTemp;
PFNTVSORTEX pCompare;
ExtraItem **pExOld[MAX_COLUMNS];
ExtraItem **pExNew[MAX_COLUMNS];
BaseItem   *pNew;
BaseItem  **pOld;
BaseItem  **pItems;
BaseItem   *pEntry;
BaseItem   *pParent;
unsigned   *pPosNew;
unsigned   *pPosOld;
unsigned   *pFirst;
unsigned   *pLast;
unsigned	uRemote;
unsigned	uBefore;
unsigned	uParent;
unsigned	uAfter;
unsigned	uFirst;
unsigned	uSize;
unsigned	uBits;
unsigned	uItem;
unsigned	uNext;
unsigned	uMax;
unsigned	uPos;
unsigned	uNum;
int			iCmp;
int			iNone;
int			iCount;
int			iShift;


	if(pData->cLockChanges)return 0;


	   uParent=U(pInsert->hParent);
	if(uParent>pData->uTreeItemsMax)						// Pr?e das Elternelement
		{
		if(pInsert->hParent!=TVI_ROOT)
			{
			return 0;
			}

		pParent = NULL;
		}
	else{
			pParent = pData->pTreeItems[uParent];
		if(!pParent)
			{
			if(uParent)return 0;
			pParent = NULL;
			}
		}



	if(pData->uTreeItemsCount>=pData->uTreeItemsMax)		// Größe der Liste erh?en
		{
		pPosOld = pData->pItemPos;
		pOld	= pData->pTreeItems;
		uMax	= pData->uTreeItemsMax;
		uMax   += pData->uTreeItemsMax/2;
		uMax   += 64;
		pItems  = new BaseItem*[uMax+1];

		if(!pItems)
			{
			return 0;
			}

			pPosNew = new unsigned[uMax];
		if(!pPosNew)
			{
			delete pItems;
			return 0;
			}

		for(uPos=1;uPos<pData->uColumnCount;uPos++)
			{
			pExOld[uPos] = pData->pExtraItems[uPos-1];
			pExNew[uPos] = new ExtraItem*[uMax+1];

			if(!pExNew[uPos])
				{
				for(uPos--;uPos>0;uPos--)delete pExNew[uPos];
				delete pPosNew;
				delete pItems;
				return 0;
				}
			}

		memcpy(pItems ,pData->pTreeItems       ,sizeof(BaseItem*)*(     pData->uTreeItemsMax+1));
		memset(pItems +pData->uTreeItemsMax+1,0,sizeof(BaseItem*)*(uMax-pData->uTreeItemsMax  ));
		memcpy(pPosNew,pData->pItemPos         ,sizeof(unsigned )*(     pData->uTreeItemsCount));
		memset(pPosNew+pData->uTreeItemsCount,0,sizeof(unsigned )*(uMax-pData->uTreeItemsCount));

		for(uPos=1;uPos<pData->uColumnCount;uPos++)
			{
			memcpy(pExNew[uPos],pExOld[uPos]            ,sizeof(ExtraItem*)*(     pData->uTreeItemsMax+1));
			memset(pExNew[uPos]+pData->uTreeItemsMax+1,0,sizeof(ExtraItem*)*(uMax-pData->uTreeItemsMax  ));
			pData->pExtraItems[uPos-1]=pExNew[uPos];
			delete pExOld[uPos];
			}

		pData->uTreeItemsMax = uMax;
		pData->pTreeItems	 = pItems;
		pData->pItemPos		 = pPosNew;
		delete pPosOld;
		delete pOld;
		}


//******************** Den neuen Eintrag erzeugen *****************************


	pItems	= pData->pTreeItems;
	uPos	= pData->uNextSearchPos+1;
	pTemp	= new char[sizeof(BaseItem)+pData->uUserDataSize];
	pNew	= (BaseItem*)pTemp;

	if(!pNew)												// Konnte der Speicher reserviert werden
		{
		return 0;
		}

	if(pData->uUserDataSize)								// Die Userdaten auf 0 setzen
		{
		memset(pTemp+sizeof(BaseItem),0,pData->uUserDataSize);
		}

	for(;;uPos++)											// Suche freie Position
		{
		if(uPos>pData->uTreeItemsMax)uPos=1;
		if(pItems[uPos]==NULL)break;
		}

	pData->uNextSearchPos = uPos;

	memset(pNew,0,sizeof(BaseItem));						// Erstelle den neuen Eintrag

	uBits	= pInsert->item.mask;

															// Remote-Aufruf aus anderem Thread
	if((uBits&(TVIF_REMOTE|TVIF_STATE))==(TVIF_REMOTE|TVIF_STATE))
		{
		uRemote = pInsert->item.state&(TVIS_SELECTED|TVIS_EXPANDED|TVIS_EXPANDPARTIAL)&pInsert->item.stateMask;
		pInsert->item.stateMask   &= ~(TVIS_SELECTED|TVIS_EXPANDED|TVIS_EXPANDPARTIAL);
		}
	else{
		uRemote	= 0;
		}

	if(uBits&TVIF_STATE)
		{
		pNew->uState = pInsert->item.state&pInsert->item.stateMask;
		}
	else{
		if(  pData->uStyle  &TVS_CHECKBOXES)
		if(!(pData->uStyleEx&TVS_EX_BITCHECKBOX))
			{
			pNew->uState=0x1000;
			}
		}

	if(uBits&TVIF_PARAM)
		{
		pNew->lParam = pInsert->item.lParam;
		}

	if(uBits&TVIF_IMAGE)
		{
		   pNew->iImage = pInsert->item.iImage;
		if(pNew->iImage==I_IMAGECALLBACK)pNew->bCallback|=TVIF_IMAGE;
		}

	if(uBits&TVIF_SELECTEDIMAGE)
		{
		   pNew->iSelectedImage = pInsert->item.iSelectedImage;
		if(pNew->iSelectedImage==I_IMAGECALLBACK)pNew->bCallback|=TVIF_SELECTEDIMAGE;
		}

	if(uBits&TVIF_CHILDREN)									// Art der Schaltfl?hen
		{
		switch(pInsert->item.cChildren)
			{
		case  0:											break;
		case  1:		pNew->bFlags   |=TVIX_HASBUTTON;	break;
		case  I_CCB:	pNew->bCallback|=TVIF_CHILDREN;		break;
		default:		pNew->bFlags   |=TVIX_VARBUTTON;	break;
			}
		}
	else{
		pNew->bFlags |=TVIX_VARBUTTON;
		}

	if(pData->uStyle&TVS_SINGLEEXPAND)						// Nicht aufklappen bei Einzelmodus
		{
		pNew->uState &= ~TVIS_EXPANDED;
		}

	if(uBits&TVIF_TEXT)										// Text einf?en
		{
		if(pInsert->item.pszText==LPSTR_TEXTCALLBACK)
			{
			pNew->bCallback|= TVIF_TEXT;
			pNew->uTextSize = 0;
			pNew->pText		= 0;
			}
		else{
			pNew->uTextSize	= (WORD)str_len(pInsert->item.pszText);
			pNew->pText		= new TCHAR[pNew->uTextSize+1];
			memcpy(pNew->pText,pInsert->item.pszText,sizeof(TCHAR)*(pNew->uTextSize+1));
			}
		}
	else{
		pNew->pText		= new TCHAR[1];
		pNew->pText[0]	= 0;
		pNew->uTextSize	= 0;
		}


	if(!pParent)											// Einen Root-Eintrag einf?en
		{
		pNew->uParent	= 0;
		uParent			= 0;
		bFlag			= 0;
		uFirst			= 0xFFFFFFFF;
		pFirst			= &pData->uFirstChild;
		pLast			= &pData->uLastChild;
		}
	else{													// Einen Tree-Eintrag einf?en
		pNew->uParent	=  uParent;
		pNew->uLevel	=  pParent->uLevel+1;
		uFirst			=  pParent->uFirstChild;
		pFirst			= &pParent->uFirstChild;
		pLast			= &pParent->uLastChild;
		bFlag			=  pParent->bFlags;

		if(pParent->bFlags&TVIX_VARBUTTON)
			{
			pParent->bFlags|=TVIX_HASBUTTON;
			}
		}


//******************** Eintrage einf?en **************************************


	uAfter = U(pInsert->hInsertAfter);

	switch(uAfter)
		{
	case U(TVI_BEFORE):										// Nach einem Eintrag einf?en

			if(pParent)										// Einen Root-Eintrag einf?en
				{
				pEntry  = pParent;
				pParent ->bFlags  = bFlag;
				uParent = pParent->uParent;
				pParent = pItems  [uParent];

				if(!pParent)
					{
					pNew->uParent	= 0;
					pNew->uLevel	= 0;
					uParent			= 0;
					uFirst			= 0xFFFFFFFF;
					pFirst			= &pData->uFirstChild;
					pLast			= &pData->uLastChild;
					}
				else{										// Einen Tree-Eintrag einf?en
					pNew->uParent	=  uParent;
					pNew->uLevel	=  pParent->uLevel+1;
					uFirst			=  pParent->uFirstChild;
					pFirst			= &pParent->uFirstChild;
					pLast			= &pParent->uLastChild;

					if(pParent->bFlags&TVIX_VARBUTTON)
						{
						pParent->bFlags|=TVIX_HASBUTTON;
						}
					}

				if(pEntry->uPrevItem)
					{
					uAfter = pEntry->uPrevItem;
					goto DoInsert;
					}
				}

	case U(TVI_FIRST):										// Am Anfang einf?en

			if(pFirst[0])									// Gibt es schon Eintr?e
				{
				pEntry = pItems[pFirst[0]];
				pEntry->uPrevItem = uPos;
				}
			else{
				pFirst[0] = uPos;
				pLast [0] = uPos;
				break;
				}

			pNew ->uNextItem = pFirst[0];					// Eintrag einf?en
			pFirst[0]		 = uPos;

			break;


	case U(TVI_ROOT):										// Als Root-Eintrag einf?en

			pNew->uParent	= 0;
			uParent			= 0;
			pFirst			= &pData->uFirstChild;
			pLast			= &pData->uLastChild;


	case U(TVI_LAST):										// Am Ende einf?en

			if(pLast[0])									// Gibt es schon Eintr?e
				{
				pEntry = pItems[pLast[0]];
				pEntry->uNextItem = uPos;
				}
			else{
				pFirst[0] = uPos;
				pLast [0] = uPos;
				break;
				}

			pNew ->uPrevItem = pLast[0];					// Eintrag einf?en
			pLast[0]		 = uPos;


			break;


	case U(TVI_SORTEX):										// Einf?en mittels Funktion

				uItem = pFirst[0];
			if(!uItem)										// Gibt es keine Kindeintr?e
				{
				pFirst[0] = uPos;
				pLast [0] = uPos;
				break;
				}

			if(pNew->bCallback&TVIF_TEXT)					// Text ?er Callback holen
				{
				uSize=1;
				LOCK(pData);
				CallbackEntry(pData,pNew,uPos,TVIF_TEXT,iNone,uSize,pText);
				UNLOCK(pData);
				}
			else{
				pText = pNew->pText;
				}


			pData->cLockChanges=1;

			pCompare = (PFNTVSORTEX)(pInsert->item.hItem);
			uNext	 = uItem;
			iCount	 = 0;
			uBefore  = 0;

			while(uNext)									// Z?le die Eintr?e
				{
				iCount++;
				uNext = pItems[uNext]->uNextItem;
				}

			while(iCount>0)
				{											// Binary-Seach Algorithnus
				iShift = iCount/2;
				uNext  = uItem;


				while(iShift>0)
					{
					uNext  = pItems[uNext]->uNextItem;
					iShift--;
					}


				   pEntry  = pItems[uNext];
				if(pEntry->bCallback&TVIF_TEXT)				// Text ?er Callback holen
					{
					uSize=0;
					LOCK(pData);
					CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pTextTemp);
					UNLOCK(pData);
					}
				else{
					pTextTemp = pEntry->pText;
					}

				   iCmp = pCompare(pData->hWnd,(HTREEITEM)uNext,pTextTemp,pText,pEntry->lParam,pInsert->item.lParam);
				if(iCmp<0)
					{
					iCount -= (iCount+1)/2;
					continue;
					}

				if(iCmp>0)
					{
					iCount -= iCount/2+1;
					uBefore = uNext;
					uItem   = pItems[uNext]->uNextItem;
					continue;
					}

				uBefore	= pEntry->uPrevItem;
				uItem	= uNext;
				break;
				}


			pData->cLockChanges=0;

			pNew->uNextItem = uItem;
			pNew->uPrevItem = uBefore;

			if(uBefore)										// Vorherigen Eintrag anpassen
				{
				pEntry = pItems[uBefore];
				pEntry->uNextItem = uPos;
				}
			else{											// Am Anfang einf?en
				pFirst[0] = uPos;
				}

			if(uItem)										// N?hsten Eintrag anpassen
				{
				pEntry = pItems[uItem];
				pEntry->uPrevItem = uPos;
				}
			else{											// Am Ende anh?gen
				pLast[0] = uPos;
				}

			break;


	case U(TVI_SORT):										// Alphapetisch einf?en

				uItem = pFirst[0];
			if(!uItem)										// Gibt es keine Kindeintr?e
				{
				pFirst[0] = uPos;
				pLast [0] = uPos;
				break;
				}

			if(pNew->bCallback&TVIF_TEXT)					// Text ?er Callback holen
				{
				uSize=1;
				LOCK(pData);
				CallbackEntry(pData,pNew,uPos,TVIF_TEXT,iNone,uSize,pText);
				UNLOCK(pData);
				}
			else{
				pText = pNew->pText;
				}


			pData->cLockChanges=1;

			uNext	= uItem;
			iCount	= 0;
			uBefore = 0;

			while(uNext)									// Z?le die Eintr?e
				{
				iCount++;
				uNext = pItems[uNext]->uNextItem;
				}

			while(iCount>0)
				{											// Binary-Seach Algorithnus
				iShift = iCount/2;
				uNext  = uItem;


				while(iShift>0)
					{
					uNext  = pItems[uNext]->uNextItem;
					iShift--;
					}


				   pEntry  = pItems[uNext];
				if(pEntry->bCallback&TVIF_TEXT)				// Text ?er Callback holen
					{
					uSize=0;
					LOCK(pData);
					CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pTextTemp);
					UNLOCK(pData);
					}
				else{
					pTextTemp = pEntry->pText;
					}

				iCmp = str_icmp(pText,pTextTemp);

				if(iCmp<0)
					{
					iCount -= (iCount+1)/2;
					continue;
					}

				if(iCmp>0)
					{
					iCount -= iCount/2+1;
					uBefore = uNext;
					uItem   = pItems[uNext]->uNextItem;
					continue;
					}

				uBefore	= pEntry->uPrevItem;
				uItem	= uNext;
				break;
				}


			pData->cLockChanges=0;

			pNew->uNextItem = uItem;
			pNew->uPrevItem = uBefore;

			if(uBefore)										// Vorherigen Eintrag anpassen
				{
				pEntry = pItems[uBefore];
				pEntry->uNextItem = uPos;
				}
			else{											// Am Anfang einf?en
				pFirst[0] = uPos;
				}

			if(uItem)										// N?hsten Eintrag anpassen
				{
				pEntry = pItems[uItem];
				pEntry->uPrevItem = uPos;
				}
			else{											// Am Ende anh?gen
				pLast[0] = uPos;
				}

			break;


	case U(TVI_AFTER):										// Nach einem Eintrag einf?en

			uAfter  = uParent;

			if(pParent)										// Einen Root-Eintrag einf?en
				{
				pParent ->bFlags  = bFlag;
				uParent = pParent->uParent;
				pParent = pItems  [uParent];

				if(!pParent)
					{
					pNew->uParent	= 0;
					pNew->uLevel	= 0;
					uParent			= 0;
					uFirst			= 0xFFFFFFFF;
					pFirst			= &pData->uFirstChild;
					pLast			= &pData->uLastChild;
					}
				else{										// Einen Tree-Eintrag einf?en
					pNew->uParent	=  uParent;
					pNew->uLevel	=  pParent->uLevel+1;
					uFirst			=  pParent->uFirstChild;
					pFirst			= &pParent->uFirstChild;
					pLast			= &pParent->uLastChild;

					if(pParent->bFlags&TVIX_VARBUTTON)
						{
						pParent->bFlags|=TVIX_HASBUTTON;
						}
					}
				}

 	default:												// Hinter einen Eintrag einf?en
	DoInsert:
				uItem = pFirst[0];
			if(!uItem)										// Gibt es keine Kindeintr?e
				{
				pFirst[0] = uPos;
				pLast [0] = uPos;
				break;
				}

			if(uAfter>pData->uTreeItemsMax)
				{
				if((uAfter&0xFFF00000)==0xFFE00000)			// In einer genauen Reihe nach Patent einf?en
					{
					uAfter &= 0xFFFFF;

						uItem = pFirst[0];
					if(!uItem)								// Gibt es keine Kindeintr?e
						{
						pFirst[0] = uPos;
						pLast [0] = uPos;
						break;
						}

					if(uAfter==0)							// In die erste Reihe einf?en
						{
						pEntry = pItems[uItem];
						pEntry->uPrevItem = uPos;
						pNew  ->uNextItem = uItem;
						pFirst[0]		  = uPos;
						break;
						}

					uNum    = 1;
					uBefore	= 0;
															// Suche Einf?ereihe
					for(;uItem;uItem=pItems[uItem]->uNextItem)
						{
						uBefore = uItem;

						if(uNum==uAfter)
							{
							uItem = pItems[uItem]->uNextItem;
							break;
							}

						uNum++;
						}

					pNew->uNextItem = uItem;
					pNew->uPrevItem = uBefore;

					if(uBefore)								// Vorherigen Eintrag anpassen
						{
						pEntry = pItems[uBefore];
						pEntry->uNextItem = uPos;
						}
					else{									// Am Anfang einf?en
						pFirst[0] = uPos;
						}

					if(uItem)								// N?hsten Eintrag anpassen
						{
						pEntry = pItems[uItem];
						pEntry->uPrevItem = uPos;
						}
					else{									// Am Ende anh?gen
						pLast[0] = uPos;
						}

					break;
					}

				pEntry  = NULL;
				}
			else{
				pEntry  = pItems[uAfter];
				}


			if(pEntry && uParent==pEntry->uParent)			// Stimmt der Elterneintrag ?
				{
				uItem   = pEntry->uNextItem;
				uBefore = uAfter;
				}
			else{
				uItem   = 0;
				uBefore = pLast[0];
				pEntry  = pItems[uBefore];
				}

			pNew->uNextItem = uItem;
			pNew->uPrevItem = uBefore;

			if(uBefore)										// Vorherigen Eintrag anpassen
				{
				pEntry->uNextItem = uPos;
				}
			else{											// Am Anfang einf?en
				pFirst[0] = uPos;
				}

			if(uItem)										// N?hsten Eintrag anpassen
				{
				pEntry = pItems[uItem];
				pEntry->uPrevItem = uPos;
				}
			else{											// Am Ende anh?gen
				pLast[0] = uPos;
				}

			break;
		}


	pItems[uPos] = pNew;
	pData->uTreeItemsCount++;


	if(uRemote)												// Select+Expand via PostMessage setzen
		{
		uRemote |= (TVIS_SELECTED|TVIS_EXPANDED|TVIS_EXPANDPARTIAL)<<16;
		UpdateStart(pData,UPDATE_INSERT,uPos,uRemote);
		}

															// Die Anzeigezeilen akualisieren
	if(!pParent || !uFirst || (pParent->uState&TVIS_EXPANDED))
		{
			uItem = pNew->uPrevItem;
		if(!uItem)uItem = uParent;

		if(!uItem)
			{
			if(uBits&uBits&TVIF_REMOTE)
					UpdateStart(pData,UPDATE_ITEMS,0);
			else	UpdateItems(pData,0);
			}
		else{
			   pEntry = pItems[uItem];
			if(pEntry && pEntry->uShowPos)
				{
				if(uBits&uBits&TVIF_REMOTE)
						UpdateStart(pData,UPDATE_ITEMS,uItem);
				else	UpdateItems(pData,uItem);
				}
			}
		}

	if(pNew->uState&TVIS_SELECTED)							// Den ausgew?lten Eintrag ausw?len
		{
		TreeListSelectItem(pData,uPos,0,TVC_UNKNOWN);
		}




return uPos;
}

//*****************************************************************************
//*
//*		TreeListSetItem
//*
//*****************************************************************************
//	Ändert einen Eintrag im Fenster
//	pData		: Zeiger auf die Fensterdaten
//	pItem		: Zeiger auf die ein zu ?dernden Daten
//	Ergibt 1 wenn ok oder 0 bei einem Fehler
static int TreeListSetItem(TreeListData *pData,const TV_ITEM *pItem)
{
BYTE			bCall;
BYTE			bFlags;
ExtraItem	  **pList;
ExtraItem	   *pExtra;
BaseItem	   *pEntry;
BaseItem	   *pTemp;
unsigned		uChange;
unsigned		uRemote;
unsigned		uStMask;
unsigned		uMask;
unsigned		uBits;
unsigned		uItem;
unsigned		uFrom;
unsigned		uSub;
unsigned		uLen;
int				iVal;
int				iRet;


	uChange = 0;


	   uItem = U(pItem->hItem);
	if(uItem>pData->uTreeItemsMax)return 0;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;


	uBits   = pItem->mask;
	uStMask	= pItem->stateMask;
	uRemote	= 0;

															// Remote-Call aus anderem Thread
	if((uBits&(TVIF_REMOTE|TVIF_STATE))==(TVIF_REMOTE|TVIF_STATE))
		{
		uMask	= (TVIS_SELECTED|TVIS_EXPANDED|TVIS_EXPANDPARTIAL)&uStMask;

		if((pEntry->uState^pItem->state)&uMask)
			{
			uRemote  =   pItem->state&uMask;
			uStMask &= ~(TVIS_SELECTED|TVIS_EXPANDED);
			}
		}


	if(uBits&TVIF_SUBITEM)									// Einen Extraeintrag ?dern
		{
		   uSub = pItem->cChildren;
		if(uSub>0)
			{
			if(uSub>= pData->uColumnCount)return 0;
			pList	= pData->pExtraItems[uSub-1];
			pExtra	= pList[uItem];

			if(!pExtra)										// Einen neuen Eintrag erzeugen
				{
				pExtra  = new ExtraItem;
				memset(pExtra,0,sizeof(ExtraItem));
				pExtra->iImage = TV_NOIMAGE;
				pExtra->uState = pEntry->uState&(TVIS_BOLD|TVIS_UNTERLINE);
				pList[uItem]   = pExtra;
				}

			if(uBits&TVIF_PARAM)
				{
				pEntry->lParam	= pItem->lParam;
				}

			if((uBits&TVIF_IMAGE) && pExtra->iImage!=pItem->iImage)
				{
				if(pData->hImages)uChange=1;
					pExtra->iImage =pItem->iImage;
				if( pExtra->iImage==I_IMAGECALLBACK)
						pExtra->bCallback |= TVIF_IMAGE;
				else	pExtra->bCallback &= TVIF_IMAGE;
				}

			if(uBits&TVIF_TEXT)								// Einen neuen Text einstellen
				{
				if(pItem->pszText==LPSTR_TEXTCALLBACK)
					{
					if(pExtra->pText)delete pExtra->pText;
					pExtra->bCallback|= TVIF_TEXT;
					pExtra->uTextSize = 0;
					pExtra->pText	  = 0;
					uChange			  = 1;
					}
				else{
					uLen = str_len(pItem->pszText);

					if(uLen!=pExtra->uTextSize ||			// Hat sich der Text ge?dert
					   pItem->pszText==LPSTR_TEXTCALLBACK ||
					   memcmp(pExtra->pText,pItem->pszText,uLen*sizeof(TCHAR)))
						{
						uChange = 1;
						}

					if(uLen>pExtra->uTextSize || !pExtra->pText)
						{
						if(pExtra->pText)delete pExtra->pText;
						pExtra->pText=new TCHAR[uLen+1];
						}

					memcpy(pExtra->pText,pItem->pszText,(uLen+1)*sizeof(TCHAR));
					pExtra->bCallback  &=~TVIF_TEXT;
					pExtra->uTextSize   = (WORD)uLen;
					pExtra->iTextPixels = 0;
					}
				}

			if(uBits&TVIF_STATE)							// Den Status ?dern
				{
				uMask			=  uStMask&~TVIS_BASEFLAGS;
				uBits			=  uMask&(pExtra->uState^pItem->state);
				uBits		   |= (uStMask& TVIS_BASEFLAGS)&(pEntry->uState^pItem->state);
				pExtra->uState &= ~uMask;
				pExtra->uState |=  uMask&pItem->state;

				if(uBits&(TVIS_STATEIMAGEMASK|TVIS_OVERLAYMASK|TVIS_CUT))
				if(pData->hImages || pData->aColumn[uSub].bEdit>=TVAX_CHECK)
					{
					uChange=1;								// Ein Icon hats sich ver?dert
					}

				if(uBits&(TVIS_BOLD|TVIS_UNTERLINE|TVIS_DROPHILITED))
					{
					pExtra->iTextPixels=0;
					uChange=1;
					}

				if((uBits&TVIS_EXPANDED) && pEntry->uFirstChild)
					{
					   iVal=TreeListToggleItem(pData,uItem,0);
					if(iVal<0)return 0;

						pEntry = pData->pTreeItems[uItem];
					if(!pEntry)return 0;
					}

				if(uBits&TVIS_SELECTED)						// Hat sich die Auswahl ge?dert
					{
					iVal = (pData->uStyleEx&TVS_EX_SUBSELECT)? uSub:0;

					if(pItem->state&TVIS_SELECTED)
						{
						iRet = TreeListSelectItem(pData,uItem,iVal,TVC_UNKNOWN);
						}
					else if(pData->uStyleEx&TVS_EX_MULTISELECT)
						{
							   TreeListSelectItem   (pData,0  ,0,TVC_UNKNOWN);
						iRet = TreeListXorSelectItem(pData,uItem,TVC_UNKNOWN);
						}
					else{
						iRet = TreeListSelectItem(pData,0,0,TVC_UNKNOWN);
						}


						pEntry = pData->pTreeItems[uItem];
					if(!pEntry)return 0;

					if(iRet>=2)
						{
							pList	= pData->pExtraItems[uSub-1];
							pExtra	= pList[uItem];
						if(!pExtra)return 0;
						}
					else if(iRet==1)
						{
						uChange=1;
						}
					}
				}

			if(uRemote)										// Select+Expand via PostMessage setzen
				{
				uRemote |= (uBits&(TVIS_SELECTED|TVIS_EXPANDED|TVIS_EXPANDPARTIAL))<<16;
				UpdateStart(pData,UPDATE_STATE,uItem,uRemote);
				}

			if(!uChange || !pEntry->uShowPos)return 1;		// Neuzeichnen des Eintrages


			if(uBits&TVIF_REMOTE)
					UpdateStart(pData,UPDATE_RECT,uItem,uSub);
			else	UpdateRect (pData,uItem,uSub);

			return 1;
			}

		uBits &= ~TVIF_CHILDREN;
		}


//******************** Einen Basis Eintrag ?dern *****************************


	if(uBits&TVIF_PARAM)
		{
		pEntry->lParam	= pItem->lParam;
		}

	if((uBits&TVIF_IMAGE) && pEntry->iImage!=pItem->iImage)
		{
		     pEntry->iImage = pItem->iImage;
		if(!(pEntry->uState&TVIS_SELECTED) && pData->hImages)uChange=1;
		if(  pEntry->iImage==I_IMAGECALLBACK)
				pEntry->bCallback |= TVIF_IMAGE;
		else	pEntry->bCallback &= TVIF_IMAGE;
		}

	if((uBits&TVIF_SELECTEDIMAGE) && pEntry->iSelectedImage!=pItem->iSelectedImage)
		{
			pEntry->iSelectedImage = pItem->iSelectedImage;
		if((pEntry->uState&TVIS_SELECTED) && pData->hImages)uChange=1;
		if( pEntry->iSelectedImage==I_IMAGECALLBACK)
				pEntry->bCallback |= TVIF_SELECTEDIMAGE;
		else	pEntry->bCallback &= TVIF_SELECTEDIMAGE;
		}

	if(uBits&TVIF_CHILDREN)
		{
		bCall  = pEntry->bCallback;
		bFlags = pEntry->bFlags;

		switch(pItem->cChildren)
			{
		case  0:		pEntry->bCallback &= ~TVIF_CHILDREN;
						pEntry->bFlags    &= ~TVIX_HASBUTTON;
						pEntry->bFlags    |=  TVIX_VARBUTTON;
						break;

		case  1:		pEntry->bCallback &= ~TVIF_CHILDREN;
						pEntry->bFlags    &=  TVIX_VARBUTTON;
						pEntry->bFlags    |=  TVIX_HASBUTTON;
						break;

		case  I_CCB:	pEntry->bCallback |=  TVIF_CHILDREN;
						pEntry->bFlags    &= ~TVIX_VARBUTTON;
						break;

		default:		pEntry->bCallback &= ~TVIF_CHILDREN;
						pEntry->bFlags    |=  TVIX_VARBUTTON;

						if(pEntry->uFirstChild)
								pEntry->bFlags |= TVIX_HASBUTTON;
						else	pEntry->bFlags &=~TVIX_HASBUTTON;
			}

		if(bCall!=pEntry->bCallback || bFlags!=pEntry->bFlags)
			{
			uChange=1;
			}
		}

	if(uBits&TVIF_TEXT)										// Einen neuen Text einstellen
		{
		if(pItem->pszText==LPSTR_TEXTCALLBACK)
			{
			if(pEntry->pText)delete pEntry->pText;
			pEntry->bCallback|= TVIF_TEXT;
			pEntry->uTextSize = 0;
			pEntry->pText	  = 0;
			uChange			  = 1;
			}
		else{
			uLen = str_len(pItem->pszText);

			if(uLen!=pEntry->uTextSize ||					// Hat sich der Text ge?dert
			   pItem->pszText==LPSTR_TEXTCALLBACK ||
			   memcmp(pEntry->pText,pItem->pszText,(uLen+1)*sizeof(TCHAR)))
				{
				uChange = 1;
				}

			if(uLen>pEntry->uTextSize)
				{
				if(pEntry->pText)delete pEntry->pText;
				pEntry->pText=new TCHAR[uLen+1];
				}

			memcpy(pEntry->pText,pItem->pszText,(uLen+1)*sizeof(TCHAR));
			pEntry->bCallback  &=~TVIF_TEXT;
			pEntry->uTextSize   = (WORD)uLen;
			pEntry->iTextPixels = 0;
			}
		}

	if(uBits&TVIF_STATE)
		{
		if(pData->uStyle&TVS_SINGLEEXPAND)					// Nicht aufklappen bei Einzelmodus
			{
			uStMask &= ~TVIS_EXPANDED;
			}

		uBits			=  uStMask&(pEntry->uState^pItem->state);
		pEntry->uState &= ~uStMask;
		pEntry->uState |=  uStMask&pItem->state;


		if((uBits&(TVIS_OVERLAYMASK|TVIS_CUT)) && pData->hImages)
			{
			uChange=1;
			}

		if(uBits&TVIS_STATEIMAGEMASK)						// Haben sich die State-Bits ver?dert
			{
			if(pData->hStates)
				{
				uChange = 1;
				}

			if(pData->uStyleEx&TVS_EX_ALLOWITEMHIDE)		// K?nen Eintr?e ausgeblendet werden
				{
				uStMask = TVIS_STATEIMAGEMASK^TVIS_HIDEITEM;
															// Einen Eintag ein bzw. ausblenden
				if(uBits&TVIS_HIDEITEM)
					{
					if(pEntry->uShowPos)					// Eine Zeile vorher
						{
						uFrom = pEntry->uShowPos-1;
						if(uFrom)uFrom=pData->pItemPos[uFrom-1];
						}
					else{
						pTemp = pEntry;

						for(;;)								// Suche sichtbares Elternelement
							{
								uFrom =  pTemp->uParent;
							if(!uFrom)break;
								pTemp = pData->pTreeItems[uFrom];
							if( pTemp->uShowPos)break;
							}
						}

					UpdateItems(pData,uFrom);
					}
				}
			else{
				uStMask = TVIS_STATEIMAGEMASK;
				}

			if(pData->uStyleEx&TVS_EX_BITCHECKBOX)
				{
				if(pEntry->uState&0x1000)
					{
					pData->uSingleSel = uItem;
					}
				else if(pData->uSingleSel==uItem)
					{
					pData->uSingleSel = 0;
					}
				}
			else{
				if((pEntry->uState&uStMask)==0x2000)
					{
					pData->uSingleSel = uItem;
					}
				else if(pData->uSingleSel==uItem)
					{
					pData->uSingleSel = 0;
					}
				}
			}

		if(uBits&(TVIS_BOLD|TVIS_UNTERLINE|TVIS_DROPHILITED))
			{
			pEntry->iTextPixels = 0;
			uChange = 1;
			}

		if(uBits&TVIS_SELECTED)								// Hat sich die Auswahl ge?dert
			{
			pEntry->uState ^= TVIS_SELECTED;

			if(pItem->state&TVIS_SELECTED)
				{
				iRet = TreeListSelectItem(pData,uItem,0,TVC_UNKNOWN);
				}
			else if(pData->uStyleEx&TVS_EX_MULTISELECT)
				{
					   TreeListSelectItem   (pData,0  ,0,TVC_UNKNOWN);
				iRet = TreeListXorSelectItem(pData,uItem,TVC_UNKNOWN);
				}
			else{
				iRet = TreeListSelectItem(pData,0,0,TVC_UNKNOWN);
				}

				pEntry = pData->pTreeItems[uItem];
			if(!pEntry)return 0;

			if(iRet==1)
				{
				uChange=1;
				}
			}

		if((uBits&TVIS_EXPANDED) && pEntry->uFirstChild)	// Sollen Teile auf/zugeklappt werden
			{
			uStMask		   &= TVIS_EXPANDPARTIAL|TVIS_EXPANDPARTIAL;
			pEntry->uState ^= TVIS_EXPANDED;
			pEntry->uState ^= uBits&uStMask;
			iVal			= uStMask&pItem->state;

			   iRet=TreeListToggleItem(pData,uItem,iVal);
			if(iRet)										// Abbruch oder Fehler beim Auf/Zuklappen
				{
				if(uChange && pEntry->uShowPos)				// Neuzeichnen des Eintrages
					{
					UpdateRect(pData,uItem,0);
					}

				return 0;
				}

			pEntry->uState &= ~uStMask;
			pEntry->uState |=  iVal;
			}
		}


	if(uRemote)												// Select+Expand via PostMessage setzen
		{
		uRemote |= (uBits&(TVIS_SELECTED|TVIS_EXPANDED|TVIS_EXPANDPARTIAL))<<16;
		UpdateStart(pData,UPDATE_STATE,uItem,uRemote);
		}


	if(uChange && pEntry->uShowPos)							// Neuzeichnen des Eintrages
		{
		if(uBits&TVIF_REMOTE)
				UpdateStart(pData,UPDATE_RECT,uItem,0);
		else	UpdateRect (pData,uItem,0);
		}



return 1;
}


//*****************************************************************************
//*
//*		TreeListGetItem
//*
//*****************************************************************************
//	Daten vone einem Eintrag abfragen
//	pData		: Zeiger auf die Fensterdaten
//	pItem		: Zeiger auf die den Datenspeicher
//	Ergibt 1 wenn ok oder 0 bei einem Fehler
static unsigned TreeListGetItem(TreeListData *pData,TV_ITEM *pItem)
{
ExtraItem	  **pList;
ExtraItem	   *pExtra;
BaseItem	   *pEntry;
unsigned		uBits;
unsigned		uItem;
unsigned		uSub;
unsigned		uLen;




	   uItem = U(pItem->hItem);
	if(uItem>pData->uTreeItemsMax)return 0;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;


	uBits = pItem->mask;

	if(uBits&TVIF_SUBITEM)									// Einen Extraeintrag abfragen
		{
		   uSub = pItem->cChildren;
		if(uSub>0)
			{
			if(uSub>= pData->uColumnCount)return 0;
			pList	= pData->pExtraItems[uSub-1];
			pExtra	= pList[uItem];

			if(!pExtra)										// Einen neuen Eintrag erzeugen
				{
				pExtra  = new ExtraItem;
				memset(pExtra,0,sizeof(ExtraItem));
				pExtra->iImage = TV_NOIMAGE;
				pExtra->uState = pEntry->uState&(TVIS_BOLD|TVIS_UNTERLINE);
				pList[uItem]   = pExtra;
				}

			if(uBits&TVIF_PARAM)
				{
				pItem->lParam = pEntry->lParam;
				}

			if(uBits&TVIF_IMAGE)
				{
				pItem->iImage = pExtra->iImage;
				}

			if(uBits&TVIF_TEXT)								// Einen neuen Text einstellen
				{
				if(pExtra->pText==LPSTR_TEXTCALLBACK)
					{
					pItem->pszText = LPSTR_TEXTCALLBACK;
					}
				else if(uBits&TVIF_TEXTPTR)
					{
					if(!pExtra->pText)
						{
						pItem->pszText    = _T("");
						pItem->cchTextMax = 0;
						}
					else{
						pItem->pszText    = pExtra->pText;
						pItem->cchTextMax = pExtra->uTextSize+1;
						}
					}
				else{
					if(pExtra->pText)
						{
						   uLen = pExtra->uTextSize+1;
						if(pItem->cchTextMax<(int)uLen)
							{
							if(pItem->cchTextMax<=0)
								{
								uLen=0;
								}
							else{
								uLen=pItem->cchTextMax-1;
								pItem->pszText[uLen]=0;
								}
							}

						memcpy(pItem->pszText,pExtra->pText,uLen*sizeof(TCHAR));
						}
					else{
						if(pItem->cchTextMax>0)
							{
							pItem->pszText[0]=0;
							}
						}
					}
				}

			if(uBits&TVIF_STATE)
				{
				pItem->state  = pExtra->uState&~TVIS_BASEFLAGS;
				pItem->state &= pItem->stateMask;
				}

			return 1;
			}

		if(pEntry->bCallback&TVIF_CHILDREN)
				pItem->cChildren = I_CHILDRENCALLBACK;
		else	pItem->cChildren = (pEntry->uFirstChild)? 1:0;


		uBits &= ~TVIF_CHILDREN;
		}


//******************** Einen Basis Eintrag ?dern *****************************


	if(uBits&TVIF_PARAM)
		{
		pItem->lParam = pEntry->lParam;
		}

	if(uBits&TVIF_IMAGE)
		{
		pItem->iImage = pEntry->iImage;
		}

	if(uBits&TVIF_SELECTEDIMAGE)
		{
		pItem->iSelectedImage = pEntry->iSelectedImage;
		}

	if(uBits&TVIF_CHILDREN)
		{
		if(pEntry->bCallback&TVIF_CHILDREN)
				pItem->cChildren = I_CHILDRENCALLBACK;
		else	pItem->cChildren = (pEntry->uFirstChild)? 1:0;
		}

	if(uBits&TVIF_TEXT)										// Einen neuen Text einstellen
		{
		if(pEntry->pText==LPSTR_TEXTCALLBACK)
			{
			pItem->pszText = LPSTR_TEXTCALLBACK;
			}
		else if(uBits&TVIF_TEXTPTR)
			{
			pItem->pszText    = pEntry->pText;
			pItem->cchTextMax = pEntry->uTextSize+1;
			}
		else{
			   uLen = pEntry->uTextSize+1;
			if(pItem->cchTextMax<(int)uLen)
				{
				if(pItem->cchTextMax<=0)
					{
					uLen=0;
					}
				else{
					uLen=pItem->cchTextMax-1;
					pItem->pszText[uLen]=0;
					}
				}

			memcpy(pItem->pszText,pEntry->pText,uLen*sizeof(TCHAR));
			}
		}

	if(uBits&TVIF_STATE)
		{
		pItem->state  = pEntry->uState;
		pItem->state &= pItem->stateMask;
		}


return 1;
}

//*****************************************************************************
//*
//*		TreeListSetItem
//*
//*****************************************************************************
//	Ändert die Eintr?e meherer im Fenster. In pItem->mask wird angegebn wo
//	welche Eintr ge?dert werden sollen:
//				TVIF_PARENTS	: In allen Elterneintr?en
//				TVIF_OWNITEM	: Im eignenen eintrgen
//				TVIF_SIBLINGS	: In allen Bruder eintr?en
//				TVIF_CHILDS		: In allen Kindeintr?en
//	pData		: Zeiger auf die Fensterdaten
//	pItem		: Zeiger auf die ein zu ?dernden Daten
//	Ergibt die Anzahl der ge?derten Eintr?en
static int TreeListSetMultiItems(TreeListData *pData,TV_ITEM *pItem)
{
BaseItem   *pEntry;
BaseItem   *pTemp;
unsigned	uItem;
unsigned	uTemp;
unsigned	uBits;
unsigned	uMode;
unsigned	uFlag;
unsigned	uLast;
unsigned	uMask;
int			iNum;


	uItem	= U(pItem->hItem);
	uBits	= pItem->stateMask;
	uMode	= pItem->mask;
	uFlag	= 0;
	iNum	= 0;

	if(uMode&TVIF_SAVESTATE)								// State-Bits in 16..31 speichern
		{
		uFlag             = uBits&0xFFFF0000;
		pItem->stateMask |=	uBits&0xFFFF0000;
		}

	if(uMode&TVIF_RESTORESTATE)								// State-Bits von 16..31 holen
		{
		uFlag            |= uBits>>16;
		pItem->stateMask |=	uBits>>16;
		}

	uMask = ~uFlag;

	if(uItem>pData->uTreeItemsMax)							// Root-Eintrag
		{
		if(uItem!=U(TVI_ROOT))return -1;

			uTemp = pData->uFirstChild;
		if(!uTemp)return 0;
		}
	else{
			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return -1;


		if(uMode&TVIF_OWNITEM)								// Eigenen Eintrag ?dern
			{
			if(uFlag)
				{
				uLast			= pEntry->uState<<16;
				uLast		   |= pEntry->uState>>16;
				uLast		   &= uFlag;
				pItem->state   &= uMask;
				pItem->state   |= uLast;
				}

			TreeListSetItem(pData,pItem);
			iNum++;
			}

		if(uMode&TVIF_PARENTS)								// Elterneintr?e ?dern
			{
			for(uTemp=pEntry->uParent;uTemp;uTemp=pTemp->uParent)
				{
					pTemp = pData->pTreeItems[uTemp];
				if(!pTemp)break;

				if(uFlag)
					{
					uLast			= pTemp->uState<<16;
					uLast		   |= pTemp->uState>>16;
					uLast		   &= uFlag;
					pItem->state   &= uMask;
					pItem->state   |= uLast;
					}

				*((unsigned*)&pItem->hItem) = uTemp;
				TreeListSetItem(pData,pItem);
				iNum++;
				}
			}

		if(uMode&TVIF_SIBLINGS)								// Brudereintr?e ?dern
			{
			for(uTemp=pEntry->uPrevItem;uTemp;uTemp=pTemp->uPrevItem)
				{
					pTemp = pData->pTreeItems[uTemp];
				if(!pTemp)break;

				if(uFlag)
					{
					uLast			= pTemp->uState<<16;
					uLast		   |= pTemp->uState>>16;
					uLast		   &= uFlag;
					pItem->state   &= uMask;
					pItem->state   |= uLast;
					}

				*((unsigned*)&pItem->hItem) = uTemp;
				TreeListSetItem(pData,pItem);
				iNum++;
				}

			for(uTemp=pEntry->uNextItem;uTemp;uTemp=pTemp->uNextItem)
				{
					pTemp = pData->pTreeItems[uTemp];
				if(!pTemp)break;

				if(uFlag)
					{
					uLast			= pTemp->uState<<16;
					uLast		   |= pTemp->uState>>16;
					uLast		   &= uFlag;
					pItem->state   &= uMask;
					pItem->state   |= uLast;
					}

				*((unsigned*)&pItem->hItem) = uTemp;
				TreeListSetItem(pData,pItem);
				iNum++;
				}
			}

		uTemp = pEntry->uFirstChild;
		}


	if(uMode&TVIF_CHILDS)									// Kindereintr?e ?dern
		{
		for(;;)
			{
				pTemp = pData->pTreeItems[uTemp];
			if(!pTemp)break;

			if(uFlag)
				{
				uLast			= pTemp->uState<<16;
				uLast		   |= pTemp->uState>>16;
				uLast		   &= uFlag;
				pItem->state   &= uMask;
				pItem->state   |= uLast;
				}

			*((unsigned*)&pItem->hItem) = uTemp;
			TreeListSetItem(pData,pItem);
			iNum++;

			if(pTemp->uFirstChild)							// Gibt es Kinder
				{
				uTemp = pTemp->uFirstChild;
				continue;
				}

			   uTemp = pTemp->uNextItem;
			if(uTemp)continue;

			for(;;)											// Aufsteigen
				{
				   uTemp =pTemp->uParent;
				if(uTemp==uItem)goto Exit;

				   pTemp =pData->pTreeItems[uTemp];
				if(pTemp==0)break;

				if(pTemp->uNextItem)
					{
					uTemp = pTemp->uNextItem;
					break;
					}
				}
			}
		}

Exit:

	*((unsigned*)&pItem->hItem) = uItem;
	pItem->stateMask			= uBits;


return iNum;
}


//*****************************************************************************
//*
//*		TreeListExpand
//*
//*****************************************************************************
//	Einen Eintrag auf oder zuklappen
//	uItem		: Ist der Eintrag
//	nCode		: Flags f?'s Auf/Zuklappen
//					TVE_COLLAPSE		Zuklappen
//					TVE_COLLAPSERESET   Zuklappen und Kinder l?chen
//					TVE_EXPAND			Aufklappen
//					TVE_TOGGLE   		Umklappen
//				  flags which can use with TVE_EXPAND;
//					TVIS_EXPANDPARTIAL	Das TVIS_EXPANDPARTIAL Flag setzen
//					TVE_EXPANDRECURSIVE Alle Eltern aun/zuklappen
//					TVE_EXPANDFORCE 	Das Partial-Flag nicht l?chen
//					TVE_EXPANDNEXT		Beim ersten Eltereintrag beginnen
//					TVE_ALLCHILDS		Bei allen Kindern auch machen
//					TVE_ALLSIBLINGS		Bei allen Br?ern auch machen
//					TVE_ONLYCHILDS		Beim ersten Kind starten
//	Ergibt 1 wenn ok bzw. 0 bei einem Fehler
static int TreeListExpand(TreeListData *pData,unsigned uItem,unsigned uCode)
{
BaseItem   *pEntry;
unsigned	uMode;
unsigned	uVal;
unsigned 	uRet;
unsigned	uEnd;
char		cOld;


	if(uItem>pData->uTreeItemsMax)								// Eintrag pr?en
		{
		if(uItem!=U(TVI_ROOT) || !(uCode&TVE_ALLCHILDS))
			{
			return 0;
			}

		uItem  = pData->uFirstChild;							// Alle der ersten Ebene abarbeiten
		uCode |= TVE_ALLSIBLINGS;
		}

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)
		{
		return 0;
		}

	if(uCode&TVE_EXPANDNEXT)									// Bei erstem Eltereneintrag beginnen
		{
		if(!pEntry->uParent)
			{
			if(!(uCode&TVE_ALLCHILDS))
				{
				return 0;
				}

			uCode &= ~TVE_EXPANDNEXT;
			uCode |=  TVE_ALLSIBLINGS;
			}

		uItem  = pEntry->uParent;
		pEntry = pData->pTreeItems[uItem];
		uCode &= ~TVE_EXPANDNEXT;
		}

	if(uCode&TVE_REMOTE)										// Die Aktion Remote ausf?ren
		{
			pEntry	 = pData->pTreeItems[uItem];
		if(!pEntry)return 0;

		uCode &= ~TVE_REMOTE;
		UpdateStart(pData,UPDATE_EXPAND,uCode,uItem);
		return 1;
		}


	uRet = 0;


	if(uCode&(TVE_ALLCHILDS|TVE_ALLSIBLINGS))					// Auf Kinder und Br?er anwenden
		{
		uMode    = uCode;
		uCode   &= ~(TVE_EXPANDRECURSIVE|TVE_EXPANDNEXT|TVE_ONLYCHILDS|TVE_ALLCHILDS|TVE_ALLSIBLINGS);
		pEntry	 = pData->pTreeItems[uItem];

		if(pEntry->uNextItem && !(uMode&TVE_ALLSIBLINGS))		// Wo aufh?en
			{
			uEnd = pEntry->uNextItem;
			}
		else{
			uEnd = pEntry->uParent;
			}

		uMode   &= TVE_ALLCHILDS;

		while(uItem!=uEnd)
			{
			uRet |= TreeListExpand(pData,uItem,uCode);

				pEntry = pData->pTreeItems[uItem];
			if(!pEntry)break;

			if(pEntry->uFirstChild && uMode)					// Auch Kinder
				{
				uItem  = pEntry->uFirstChild;
				}
			else if(pEntry->uNextItem)							// Gibt es Br?er
				{
				uItem  = pEntry->uNextItem;
				}
			else for(;;)										// Ebenen aufsteigen
				{
					uItem  = pEntry->uParent;
				if(!uItem || uItem==uEnd)break;
					pEntry = pData->pTreeItems[uItem];
				if(!pEntry->uNextItem)continue;

				uItem = pEntry->uNextItem;
				break;
				}
			}

		return uRet;
		}


//*****************************************************************************


	cOld = pData->cNoExpandNf;

	if(uCode&TVE_NONOTIFY)
		{
		pData->cNoExpandNf = 1;
		}


	for(;;)
		{
		if(uItem>pData->uTreeItemsMax)
			{
			pData->cNoExpandNf = cOld;
			return uRet;
			}

			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)
			{
			pData->cNoExpandNf = cOld;
			return uRet;
			}


		switch(uCode&0x0F)
			{
		case TVE_COLLAPSE:										// Zuklappen

					if(pEntry->uState&TVIS_EXPANDED)
						{
						uRet = (TreeListToggleItem(pData,uItem,0))? 0:1;
						}
					else{
						pEntry->uState &= ~TVIS_EXPANDPARTIAL;	// Nur Flag l?chen
						uRet = 1;
						}

					if(uCode&TVE_COLLAPSERESET)					// Kinder l?chen
						{
						pEntry->uState &= ~TVIS_EXPANDEDONCE;

						while(pEntry->uLastChild)
							{
							if(!TreeListDeleteItem(pData,pEntry->uLastChild,1))break;
							pEntry = pData->pTreeItems[uItem];
							}
						}

					if(!(uCode&TVE_EXPANDRECURSIVE))			// Auch Eltern zuklappen
						{
						break;
						}

					if(!pEntry->uParent)
						{
						break;
						}

					uItem = pEntry->uParent;

					continue;

		case TVE_EXPAND:										// Aufklappen

				if(!(pEntry->uState&TVIS_EXPANDED))
					{
					uVal = (uCode&TVE_EXPANDPARTIAL)? TVIS_EXPANDPARTIAL:0;
					uRet = (TreeListToggleItem(pData,uItem,uVal))? 0:1;
					}
				else{											// Schon aufgeklappt
					uRet = 1;

					if(uCode&TVE_EXPANDPARTIAL)
						{
						if(!(uCode&TVE_EXPANDFORCE))
						if((pEntry->uState&(TVIS_EXPANDPARTIAL|TVIS_EXPANDEDONCE))!=(TVIS_EXPANDPARTIAL|TVIS_EXPANDEDONCE))
							{
							uRet = (TreeListToggleItem(pData,uItem,TVE_EXPAND|TVIS_EXPANDPARTIAL))? 0:1;
							}
						}
					else{
						if(pEntry->uState&TVIS_EXPANDPARTIAL)
							{
							uRet = (TreeListToggleItem(pData,uItem,TVE_EXPAND))? 0:1;
							}
						}
					}

				if(!(uCode&TVE_EXPANDRECURSIVE))				// Auch Eltern aufklappen
					{
					break;
					}

				if(!pEntry->uParent)
					{
					break;
					}

				uItem = pEntry->uParent;

				continue;

		case TVE_TOGGLE:										// Umklappen

				uRet = (TreeListToggleItem(pData,uItem,0))? 0:1;
				break;
			}

		break;
		}

	pData->cNoExpandNf = cOld;


return uRet;
}

//*****************************************************************************
//*
//*		TreeListSetMarkLine
//*
//*****************************************************************************
//	Sellt die Markierungslinie f? das Control ein.
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist der Eintrag auf bei dem die Markierungslinie
//				  angezeigt werden soll
//					TVI_LAST = nach dem Ende anzeigen
//					TVI_ROOT = Position der Markierungslinie auslesen
//					NULL	 = Markierungslinie ausblenden
//	uHeight		: Ist die H?e der Markierungslinie (0=Zeilenh?e)
//	Ergibt 1 wenn die Spalte gel?cht wurde
static int TreeListSetMarkLine(TreeListData *pData,unsigned uItem,unsigned uHeight)
{
RECT		sRect;
unsigned	uPos;



	if(uHeight==0)uHeight=pData->iRowHeight;

	if(uItem<=pData->uTreeItemsMax || uItem==U(TVI_LAST))
	if(uItem!=pData->uLineItem     || uHeight!=pData->uLineHeight)
		{
		if(pData->uLineItem<pData->uTreeItemsMax)		// Alte Position zeichnen
			{
			UpdateRow(pData,pData->uLineItem);
			}
		else if(pData->uLineItem==U(TVI_LAST))
			{
			uPos	 	  = pData->uItemPosCount;
			uPos	 	 -= pData->uScrollY;
			uPos		 *= pData->iRowHeight;
			uPos		 += pData->uStartPixel;

			if(uPos>pData->uSizeY)
				{
				uPos      = pData->uSizeY;
				}

			sRect.left    = 0;
			sRect.right   = pData->uSizeX;
			sRect.top     = uPos;
			sRect.bottom  = uPos+3;

			InvalidateRect(pData->hWnd,&sRect,FALSE);
			}

		if(uHeight>=U(pData->iRowHeight))				// Zeilenh?e pr?en
			{
			uHeight			= pData->iRowHeight;
			}

		pData->uLineItem	= uItem;
		pData->uLineHeight  = uHeight;

		if(pData->uLineItem<pData->uTreeItemsMax)		// Neue Position zeichnen
			{
			UpdateRow(pData,pData->uLineItem);
			}
		else if(pData->uLineItem==U(TVI_LAST))
			{
			uPos	 	  = pData->uItemPosCount;
			uPos	 	 -= pData->uScrollY;
			uPos		 *= pData->iRowHeight;
			uPos		 += pData->uStartPixel;

			if(uPos>pData->uSizeY)
				{
				uPos      = pData->uSizeY;
				}

			sRect.left    = 0;
			sRect.right   = pData->uSizeX;
			sRect.top     = uPos;
			sRect.bottom  = uPos+3;

			InvalidateRect(pData->hWnd,&sRect,FALSE);
			}
		}


return pData->uLineItem;
}

//*****************************************************************************
//*
//*		TreeListMoveCheck
//*
//*****************************************************************************
//	Pr?t ob das Verschieben eines Eintrags m?lich ist.
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der gel?cht werden soll
//	uTarget		: Ist der Eintrag auf desen Platz der verschobene landen soll
//				  TVI_LAST oder 0 sthet f?: am Ende einf?en.
//	uMode		: Bestimmt wor der Eintrag eingef?t wird
//					0 = vor uTarget auf der gleichen Ebene
//					1 = vor uTarget auf der gleichen Ebene
//					2 = in uTarget als erstes  Kind
//					3 = in uTarget als letztes Kind
//	Ergibt einen Wert ungleich Null wenn der Eintrag verschoben werden kann:
//					0 = Eintrag kann nicht verschoben werden
//					1 = Eintrag wird auf die eigene Position verschoben
//					2 = Eintrag kann verschoben werden
static int TreeListMoveCheck(TreeListData *pData,unsigned uItem,unsigned uTarget,unsigned uMode=0)
{
BaseItem   *pTarget;
BaseItem   *pEntry;
BaseItem   *pTemp;




	if(pData->cLockChanges)return 0;

	if(uItem==0 || uItem>pData->uTreeItemsMax)				// Ist der Eintrag g?tig
		{
		return 0;
		}

	if(uItem==uTarget)										// Bleibt auf gleicher Position
		{
		return 1;
		}

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)												// Pr?e den Eintrag
		{
		return 0;
		}

	if(uTarget==U(TVI_LAST))
		{
		uTarget = 0;
		}

	if(uTarget)												// Pr?e das Ziel
		{
		if(uTarget>pData->uTreeItemsMax)return 0;
		   pTarget=pData->pTreeItems[uTarget];
		if(pTarget==0)return 0;

		switch(uMode&3)										// Ist der Eintrag bereits am Ziel
			{
		case 0:	if(pTarget->uPrevItem==uItem)return 1;	break;
		case 1:	if(pTarget->uNextItem==uItem)return 1;	break;
		default:if(uTarget           ==uItem)return 1;	break;
			}

		for(pTemp=pTarget;pTemp && pTemp->uParent;pTemp)	// Pr?e auf Rekursion
			{
			if(uItem==pTemp->uParent)return 0;
			pTemp = pData->pTreeItems[pTemp->uParent];
			}
		}
	else{
		pTarget = 0;
		pTemp   = pEntry;

		if(!pTemp->uFirstChild)								// Ist Eintrag ist bereits am Ende
			{
			while(!pTemp->uNextItem)
				{
				if(pTemp->uParent==0)return 0;
				pTemp = pData->pTreeItems[pTemp->uParent];
				}
			}
		}


return 2;
}

//*****************************************************************************
//*
//*		TreeListMoveItem
//*
//*****************************************************************************
//	Verschiebt einen Eintrag.
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der gel?cht werden soll
//	uTarget		: Ist der Eintrag auf desen Platz der verschobene landen soll
//				  TVI_LAST oder 0 sthet f?: am Ende einf?en.
//	uMode		: Bestimmt wor der Eintrag eingef?t wird
//					0 = vor uTarget auf der gleichen Ebene
//					1 = vor uTarget auf der gleichen Ebene
//					2 = in uTarget als erstes  Kind
//					3 = in uTarget als letztes Kind
//	Ergibt einen Wert ungleich Null wenn der Eintrag verschoben wurde
//					0 = Eintrag wurde nicht verschoben
//					1 = Eintrag wird auf seine eigene Position verschoben
//					2 = Eintrag wurde auf eine neue Position verschoben
static int TreeListMoveItem(TreeListData *pData,unsigned uItem,unsigned uTarget,unsigned uMode=0)
{
BaseItem   *pTarget;
BaseItem   *pParent;
BaseItem   *pEntry;
BaseItem   *pPrev;
BaseItem   *pNext;
BaseItem   *pTemp;
unsigned	uTemp;
unsigned	uNext;
unsigned	uPrev;
unsigned	uLevel;
unsigned	uParent;
int			iDiff;



	if(pData->cLockChanges)return 0;

	if(uItem==0 || uItem>pData->uTreeItemsMax)				// Ist der Eintrag g?tig
		{
		return 0;
		}

	if(uItem==uTarget)										// Bleibt auf gleicher Position
		{
		return 1;
		}

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)												// Pr?e den Eintrag
		{
		return 0;
		}

	if(uTarget==U(TVI_LAST))
		{
		uTarget = 0;
		}

	if(uTarget)												// Pr?e das Ziel
		{
		if(uTarget>pData->uTreeItemsMax)
			{
			if(uTarget==U(TVI_ROOT) && (uMode&2))
				{
				uTarget = (uMode&1)? pData->uLastChild:pData->uFirstChild;
				uMode  &= ~2;
				}
			else{
				return 0;
				}
			}

		   pTarget=pData->pTreeItems[uTarget];
		if(pTarget==0)return 0;

		switch(uMode&3)										// Ist der Eintrag bereits am Ziel
			{
		case 0:	if(pTarget->uPrevItem==uItem)return 1;	break;
		case 1:	if(pTarget->uNextItem==uItem)return 1;	break;
		default:if(uTarget           ==uItem)return 1;	break;
			}

		for(pTemp=pTarget;pTemp && pTemp->uParent;pTemp)	// Pr?e auf Rekursion
			{
			if(uItem==pTemp->uParent)return 0;
			pTemp = pData->pTreeItems[pTemp->uParent];
			}
		}
	else{
		pTarget = 0;
		pTemp   = pEntry;

		if(!pTemp->uFirstChild)								// Ist Eintrag ist bereits am Ende
			{
			while(!pTemp->uNextItem)
				{
				if(pTemp->uParent==0)return 0;
				pTemp = pData->pTreeItems[pTemp->uParent];
				}
			}
		}


	uNext	= pEntry->uNextItem;
	uPrev	= pEntry->uPrevItem;
	uParent	= pEntry->uParent;
	uLevel	= pEntry->uLevel;


	if(uNext)												// Eintrag von n?hsten l?en
		{
		pNext				 = pData ->pTreeItems[uNext];
		pNext->uPrevItem	 = uPrev;
		}
	else if(uParent)										// Eintrag war das letzte Kind
		{
		pParent				 = pData->pTreeItems[uParent];
		pParent->uLastChild	 = uPrev;

		if(!uPrev)											// Elterneintrag hat keine Kinder mehr
			{
			pParent->bFlags &=~TVIX_HASBUTTON;
			}
		}
	else{													// Eintrag war das letzte Root-Kind
		pData->uLastChild	 = uPrev;
		pNext				 = 0;
		}


	if(uPrev)												// Eintrag von vorherigen l?en
		{
		pPrev				 = pData ->pTreeItems[uPrev];
		pPrev->uNextItem	 = uNext;
		}
	else if(uParent)										// Ist der Eintrag das erste Kind geworden
		{
		pParent				 = pData->pTreeItems[uParent];
		pParent->uFirstChild = uNext;
		pPrev				 = 0;
		}
	else{													// Eintrag war das erste Root-Kind
		pData->uFirstChild   = uNext;
		pPrev				 = 0;
		}


	if(uTarget==0)											// Eintrag ans Ende verschieben
		{
		if(!uPrev && uParent)
			{
			uPrev = uParent;
			pPrev = pData->pTreeItems[uPrev];
			}

		uTemp = pData->uLastChild;
		pTemp = pData->pTreeItems[uTemp];

		if(!pPrev || pPrev->uShowPos>pTemp->uShowPos)		// Bei der Quelle oder am Ziel neuzeichnen
			{
			uPrev = uTemp;
			}

		pEntry->uLevel		= 0;
		pEntry->uParent		= 0;
		pEntry->uNextItem	= 0;
		pEntry->uPrevItem	= uTemp;
		pTemp				= pData->pTreeItems[uTemp];
		pTemp ->uNextItem	= uItem;
		pData ->uLastChild	= uItem;
		}
	else if(uMode&2)										// Als Kind einf?en
		{
		pTarget->bFlags	   |= TVIX_HASBUTTON;
		pEntry->uLevel		= pTarget->uLevel+1;
		pEntry->uParent     = uTarget;

		if(uMode&1)											// Als letztes Kind einf?en
			{
			pEntry->uPrevItem	 = pTarget->uLastChild;
			pEntry->uNextItem	 = 0;

			if(pTarget->uLastChild)
				{
				pTemp			 = pData->pTreeItems[pTarget->uLastChild];
				pTemp->uNextItem = uItem;
				}
			else{
				pTarget->uFirstChild = uItem;
				}

			pTarget->uLastChild  = uItem;
			}
		else{												// Als erstes Kind einf?en
			pEntry->uNextItem    = pTarget->uFirstChild;
			pEntry->uPrevItem	 = 0;

			if(pTarget->uFirstChild)
				{
				pTemp			 = pData->pTreeItems[pTarget->uFirstChild];
				pTemp->uPrevItem = uItem;
				}
			else{
				pTarget->uLastChild = uItem;
				}

			pTarget->uFirstChild = uItem;
			}

		if(uPrev || uParent)								// Der Eintrag war nicht am Anfang
			{
			if(!uPrev)										// Beim Elternteil mit Neuberechung beginnen
				{
				uPrev = uParent;
				pPrev = pData->pTreeItems[uPrev];
				}

			if(pEntry->uPrevItem)
					uTemp = pEntry->uPrevItem;
			else	uTemp = pEntry->uParent;

			pTemp = pData->pTreeItems[uTemp];

															// Bei der Quelle oder am Ziel neuzeichnen
			if(pTemp->uShowPos && pPrev->uShowPos>pTemp->uShowPos)
				{
				uPrev = uTemp;
				}
			}
		}
	else{													// Zu bestehenten Ziel verschieben
		pEntry ->uLevel		= pTarget->uLevel;
		pEntry ->uParent    = pTarget->uParent;

		if(uMode&1)											// Am Ende einf?en
			{
			pEntry ->uNextItem	 = pTarget->uNextItem;
			pEntry ->uPrevItem   = uTarget;
			pTarget->uNextItem	 = uItem;

			if(pEntry->uNextItem)							// N?hsten Eintrag anpassen
				{
				pTemp			 = pData->pTreeItems[pEntry->uNextItem];
				pTemp->uPrevItem = uItem;
				}
			else if(pEntry->uParent)						// Eintrag ist das letzte Kind geworden
				{
				pParent = pData->pTreeItems[pEntry->uParent];
				pParent->uLastChild = uItem;
				}
			else{											// Ist letzte Kind in Root geworden
				pData->uLastChild   = uItem;
				}
			}
		else{
			pEntry ->uPrevItem   = pTarget->uPrevItem;
			pEntry ->uNextItem	 = uTarget;
			pTarget->uPrevItem	 = uItem;

			if(pEntry->uPrevItem)							// Vorherigen Eintrag bearbeiten
				{
				pTemp			 = pData->pTreeItems[pEntry->uPrevItem];
				pTemp->uNextItem = uItem;
				}
			else if(pEntry->uParent)						// Eintrag ist das erste Kind geworden
				{
				pParent = pData->pTreeItems[pEntry->uParent];
				pParent->uFirstChild = uItem;
				}
			else{											// Ist erstes Kind in Root geworden
				pData->uFirstChild   = uItem;
				}
			}

		if(!pEntry->uParent && !pEntry->uPrevItem)			// Der Eintrag ist jetzt am Anfang
			{
			uPrev = 0;
			}
		else if(uPrev || uParent)							// Der Eintrag war nicht am Anfang
			{
			if(!uPrev)										// Beim Elternteil mit Neuberechung beginnen
				{
				uPrev = uParent;
				pPrev = pData->pTreeItems[uPrev];
				}

			if(pEntry->uPrevItem)
					uTemp = pEntry->uPrevItem;
			else	uTemp = pEntry->uParent;

			pTemp = pData->pTreeItems[uTemp];

															// Bei der Quelle oder am Ziel neuzeichnen
			if(pTemp->uShowPos && pPrev->uShowPos>pTemp->uShowPos)
				{
				uPrev = uTemp;
				}
			}
		}


	if(uLevel!=pEntry->uLevel && pEntry->uFirstChild)		// Ebenen?derung
		{
		iDiff = pEntry->uLevel-uLevel;
		uTemp = pEntry->uFirstChild;

		for(;;)
			{
			pTemp		   = pData->pTreeItems[uTemp];
			pTemp->uLevel += iDiff;

			if(pTemp->uFirstChild)
				{
				uTemp = pTemp->uFirstChild;
				continue;
				}

			if(pTemp->uNextItem)
				{
				uTemp = pTemp->uNextItem;
				continue;
				}

			for(;;)
				{
				uTemp = pTemp->uParent;
				if(uTemp==uItem)break;
				pTemp = pData->pTreeItems[uTemp];

				if(pTemp->uNextItem)
					{
					uTemp = pTemp->uNextItem;
					break;
					}
				}

			if(uTemp==uItem)break;
			}
		}


	UpdateItems(pData,uPrev);								// Sohw-Positionen neu berechen



return 2;
}

//*****************************************************************************
//*
//*		TreeListMoveLevelCheck
//*
//*****************************************************************************
//	Pr?t ob die Einf?eposition zum Level und Elterneintrag passt
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist der Eintag der verschoben werden soll, wenn kein
//	uTarget		: Der Zieleintrag wohin verschoben werden soll.
//				  TVI_END heist ans Ende verschieben.
//	uMode		: Ist der Einf?emodus
//	Ergibt 1 wenn der Eintrag nicht eingef?t werden kann
static int TreeListMoveLevelCheck(TreeListData *pData,unsigned uItem,unsigned &uTarget,unsigned &uMode)
{
BaseItem   *pTarget;
BaseItem   *pParent;
BaseItem   *pEntry;
unsigned	uParent;
unsigned	uLevel;
unsigned	uPrev;


	if(uItem==0 || uItem>pData->uTreeItemsMax)		// Den Eintrag pr?en
		{
		return 1;
		}

		pEntry	= pData->pTreeItems[uItem];
	if(!pEntry)return 1;


	if(uMode&2)										// Beim Kind einf?en
		{
		uParent	= uTarget;
		}
	else{											// Vorher oder nacher einf?en
		pTarget = pData->pTreeItems[uTarget];
		uParent	= (pTarget)? pTarget->uParent:0;
		}


	pParent =  pData->pTreeItems[uParent];
	uLevel  = (pParent)? pParent->uLevel+1:0;

	if(pEntry->uLevel==uLevel)						// Auf gleichem Level
		{
		if(pData->uMoveFlags&TVDM_SAMEPARENT)		// Elterneintrag pr?en
		if(pEntry->uParent!=uParent)
			{
			return 1;
			}

		return 0;
		}

	if(uMode&2)										// Als Kind einf?en
		{
		return 1;
		}

	if(!uTarget)									// Auch vorherigen Eintrag pr?en
		{
		uPrev = pData->pItemPos[pData->uItemPosCount-1];
		}
	else{
			pTarget = pData->pTreeItems[uTarget];
		if(!pTarget || pTarget->uShowPos<=1)
			{
			return 1;
			}

		uPrev = pData->pItemPos[pTarget->uShowPos-2];
		}

	pTarget =  pData->pTreeItems[uPrev];

Retry:

	uParent	= (pTarget)? pTarget->uParent:0;
	pParent =  pData->pTreeItems[uParent];
	uLevel  = (pParent)? pParent->uLevel+1:0;

	if(pEntry==0 || pEntry->uLevel!=uLevel)			// Auf anderem Level
		{
		if(pEntry->uLevel<=uLevel)
		if(pParent && !pParent->uNextItem)
			{
			uPrev   = uParent;
			pTarget	= pParent;
			goto Retry;
			}

		return 1;
		}

	if(pData->uMoveFlags&TVDM_SAMEPARENT)			// Elterneintrag pr?en
	if(pEntry->uParent!=uParent)
		{
		return 1;
		}

	uTarget	= uPrev;
	uMode	= 1;


return 0;
}

//*****************************************************************************
//*
//*		TreeListMoveSelection
//*
//*****************************************************************************
//	Verschiebt alle gew?lten Eintr?e
//	pData		: Zeiger auf die Fensterdaten
//	uTarget		: Der Zieleintrag wohin verschoben werden soll.
//				  TVI_END oder TVI_LAST heist ans Ende verschieben.
//	uItem		: Ist der Eintag der verschoben werden soll, wenn kein
//				  Eintrag ausgew?lt ist.
//	wParam		: WPARAM von der WM_LBUTTONUP Message
//	Ergibt die Anzahl der verschobenen Eintr?e.
static int TreeListMoveSelection(TreeListData *pData,unsigned uTarget,unsigned uItem=0,unsigned wParam=0)
{
NMTREEVIEW	sNotify;
BaseItem   *pTarget;
BaseItem   *pEntry;
unsigned	uParent;
unsigned	uChild;
unsigned	uState;
unsigned 	uMode;
unsigned	uLast;
unsigned	uStop;
unsigned	uNext;
unsigned	uRet;
unsigned	uNum;




	if(uTarget>pData->uTreeItemsMax)						// Zieleintrag pr?en
		{
		if(uTarget!=U(TVI_LAST))
			{
			return 0;
			}

		uTarget = U(TVI_END);
		}
	else if(!pData->pTreeItems[uTarget])
		{
		return 0;
		}

															// Modus aus STRG und SHIFT holen
	if((pData->uMoveFlags&TVDM_NOTOCHILDS) || !(wParam&MK_CONTROL))
		{
		uMode = 0;
		}
	else if(wParam&MK_SHIFT)
		{
		uMode=2;
		}
	else{
		uMode=3;
		}
															// Nicht auf fixierten Bereich
	if((pData->uMoveFlags&TVDM_NOFIXEDITEMS) && pData->uFixedLines && uTarget<=pData->uTreeItemsMax)
		{
		   pEntry = pData->pTreeItems[uTarget];
		if(pEntry && pEntry->uShowPos<=pData->uFixedLines)
			{
			return 0;
			}
		}


//******************** Einfachauswahl *****************************************


	if(pData->uSelectedCount<=1)							// Einzeleintrag verschieben
		{
		if(pData->uSelectedCount)uItem=pData->uSelectedItem;

		if(!uItem)return 0;

		if(pData->uMoveFlags&TVDM_ONLYENDPOINTS)			// Nur Eintr?e ohne Kinder
			{
			if(uItem>pData->uTreeItemsMax)return 0;
			pEntry = pData->pTreeItems[uItem];
			if(!pEntry || pEntry->uFirstChild)return 0;
			}


															// Level und Einzeleintrag verschieben
		if(pData->uMoveFlags&(TVDM_SAMELEVEL|TVDM_SAMEPARENT))
		if(TreeListMoveLevelCheck(pData,uItem,uTarget,uMode))
			{
			return 0;
			}

		uRet = TreeListMoveItem(pData,uItem,uTarget,uMode);	// Eintrag verschieben

		if(uRet==0)
			{
			return 0;
			}

		if(uMode&2)											// Ziel mit Eintrag aufklappen
			{
			   pTarget = pData->pTreeItems[uTarget];
			if(pTarget && !(pTarget->uState&TVIS_EXPANDED))
				{
				TreeListToggleItem(pData,uTarget,0);
				}
			}

		uNum						= 1|(uRet<<30);
		sNotify.action				= uMode<<28;
		sNotify.itemNew.hItem		= (HTREEITEM)uItem;
		sNotify.itemOld.hItem		= (HTREEITEM)uTarget;

		goto Exit;
		}


//******************** Mehrfachauswahl ****************************************


	uItem	= pData->uFirstChild;
	uLast	= 0;
	uNum	= 0;

	if(!uTarget)											// Done-Flags bis zum Ende l?chen
		{
		uStop = 0;
		}
	else if(uMode&2)
		{
		uStop = uTarget;
		}
	else{ 													// Done-Flags l?chen ab Eleterneintrag l?chen
		   pEntry=pData->pTreeItems[uTarget];
		if(pEntry)
				uStop=pEntry->uParent;
		else	uStop=0;
		}

	sNotify.action = 0xFFFFFFFF;

	for(;;)													// Alle ausgew?lten Eintr?e suchen
		{
		   pEntry = pData->pTreeItems[uItem];
		if(pEntry==0)break;

		uNext	= pEntry->uNextItem;
		uChild	= pEntry->uFirstChild;
		uParent	= pEntry->uParent;
		uState	= pEntry->uState;

		if((uState&TVIS_SELECTED) && pEntry->bDone==0)		// Ist der Eintrag gew?t und wurde nicht verschoben
			{
			if(pData->uMoveFlags&TVDM_ONLYENDPOINTS)		// Nur Eintr?e ohne Kinder
			if(pEntry->uFirstChild)
				{
				uState = 0;
				}
															// Level und Einzeleintrag verschieben
			if(uState && (pData->uMoveFlags&(TVDM_SAMELEVEL|TVDM_SAMEPARENT)))
			if(TreeListMoveLevelCheck(pData,uItem,uTarget,uMode))
				{
				uState = 0;
				}

			if(sNotify.action==0xFFFFFFFF)					// Notify-Daten speichern
				{
				sNotify.action			= uMode<<28;
				sNotify.itemNew.hItem	= (HTREEITEM)uItem;
				sNotify.itemOld.hItem	= (HTREEITEM)uTarget;
				}
															// Ist der Eintrag ausgew?lt und nicht verschoben
			if(uState)
				{
				   uRet = TreeListMoveItem(pData,uItem,uTarget,uMode);
				if(uRet)
					{
					if(uMode&2)								// Ziel mit Eintrag aufklappen
						{
						   pTarget = pData->pTreeItems[uTarget];
						if(pTarget && !(pTarget->uState&TVIS_EXPANDED))
							{
							TreeListToggleItem(pData,uTarget,0);
							}
						}

					if(!uLast)uLast = uItem;
					uTarget			= uItem;
					pEntry->bDone	= 1;
					uMode			= 1;
					uNum		   += 1;
					uNum		   |= uRet<<30;
					}
				}
			}

		if(uChild && pEntry->bDone==0)						// Zum n?hsten Eintrag
			{
			uItem = uChild;
			continue;
			}

		if(uNext)
			{
			uItem = uNext;
			continue;
			}

		uItem = uParent;

		while(uItem)										// Ebenen aufsteigen
			{
				pEntry = pData->pTreeItems[uItem];
			if(!pEntry->uNextItem)
				{
				uItem = pEntry->uParent;
				continue;
				}

			uItem = pEntry->uNextItem;
			break;
			}
		}


	uItem = uLast;

	for(;;)													// Alle Done-Flags l?chen
		{
		   pEntry = pData->pTreeItems[uItem];
		if(pEntry==0)break;

		pEntry->bDone = 0;

		if(pEntry->uFirstChild)								// Zum n?hsten Eintrag
			{
			uItem = pEntry->uFirstChild;
			continue;
			}

		if(pEntry->uNextItem)
			{
			uItem = pEntry->uNextItem;
			continue;
			}

		for(;;)												// Ebenen aufsteigen
			{
				uItem = pEntry->uParent;
			if(!uItem || uItem==uStop)goto Exit;

				pEntry = pData->pTreeItems[uItem];
			if(!pEntry->uNextItem)
				{
				uItem = pEntry->uParent;
				continue;
				}

			uItem = pEntry->uNextItem;
			break;
			}
		}

Exit:

	if(uNum==0)return 0;


//******************** Notify-Message senden **********************************


	sNotify.hdr.code			= TVN_ITEMSMOVED;
	sNotify.action			   |= uNum|(uMode<<28);
	sNotify.itemNew.mask		= TVIF_HANDLE;
	sNotify.itemOld.mask		= TVIF_HANDLE;
	sNotify.ptDrag.x			= GET_X_LPARAM(pData->uMoveLastW);
	sNotify.ptDrag.y			= GET_Y_LPARAM(pData->uMoveLastW);


	UNLOCK(pData);
	SendNotify(pData,&sNotify.hdr);
	LOCK(pData);


return uNum&0x3FFFFFFF;
}

//*****************************************************************************
//*
//*		TreeListDeleteColumn
//*
//*****************************************************************************
//	L?cht eine Spalte aus dem Header
//	pData		: Zeiger auf die Fensterdaten
//	uCol		: Ist die Nummer der Spalte die gel?cht werden soll
//	Ergibt 1 wenn die Spalte gel?cht wurde
static int TreeListDeleteColumn(TreeListData *pData,unsigned uCol)
{
ExtraItem **pList;
ExtraItem  *pExtra;
RECT		sRect;
BYTE		bItem;
BYTE		bByte;
unsigned	uPos;
unsigned	uSub;
unsigned	uItem;
unsigned	uIndex;
int			iDelta;
int			iXoff;
int			iNum;
int			iCnt;
int			iVar;
int			iSub;
int			iAll;
int			iFix;


	if(uCol>=pData->uColumnCount)return 0;

	if(uCol && uCol==pData->uSelectedSub)					// Ist die Auswahl in der Spalte
		{
		TreeListSelectItem(pData,pData->uSelectedItem,0,TVC_UNKNOWN);
		}

	if(uCol && uCol==pData->uEditSub)
		{
		pData->uEditSub  = 0;
		pData->uEditItem = 0;
		TreeListEndLabelEdit(pData,0);
		}

	if(uCol && uCol==pData->uFocusSub)
		{
		pData->uFocusSub  = 0;
		pData->uFocusItem = 0;
		}

	if(uCol==pData->uTrackedSub)
		{
		pData->uTrackedSub  = 0;
		pData->uTrackedItem = 0;
		}



	GetClientRect(pData->hWnd,&sRect);


	iDelta	= pData->aColumn[uCol].sSize;
	iSub	= pData->aColumn[uCol].bWeight;
	iCnt	= 0;
	iVar	= 0;
	iFix	= 0;
	iAll	= 0;

	for(uPos=0;uPos<pData->uColumnCount;uPos++)				// Z?lern der variablen Spalten
		{
		if(uPos==uCol)continue;
		if(pData->aColumn[uPos].bWeight==0)
			{
			iFix +=	pData->aColumn[uPos].sSize;
			continue;
			}

		iVar +=	pData->aColumn[uPos].sSize;
		iAll += pData->aColumn[uPos].bWeight;
		iCnt += 1;
		}


	Header_DeleteItem(pData->hHeader,uCol);
	pData->uColumnCount--;

	if(pData->uColumnCount>0)								// Liste mit Extraeintr?en l?chen
		{
		   iNum=uCol-1;
		if(iNum<0)iNum=0;

		   pList = pData->pExtraItems[iNum];
		if(pList)
            {
			for(uItem=0;uItem<=pData->uTreeItemsMax;uItem++)// Alle Eintr?e aus der Liste l?chen
				{
					pExtra = pList[uItem];
				if(!pExtra)continue;

				if(pExtra->pText)
					{
					pExtra->uTextSize=0;
					delete pExtra->pText;
					}

				delete pExtra;
				}

			memmove(pData->pExtraItems+iNum,pData->pExtraItems+iNum+1,sizeof(pList)*(MAX_COLUMNS-1-iNum));
			pData->pExtraItems[pData->uColumnCount]=NULL;
			delete pList;
			}
		}
	else{
		iNum=MAX_COLUMNS;
		}

	if(pData->aColumn[uCol].bWeight)
		{
		pData->uColumnCountVar--;
		}

	if(pData->aColumn[uCol].bMark)
		{
		pData->uMarkedCols--;
		}

	uSub = pData->aColumnPos[uCol];

	memmove(pData->aColumn+uCol,pData->aColumn+uCol+1,(MAX_COLUMNS-1-uCol)*sizeof(ColumnData));

	for(uIndex=0;uIndex<uSub;uIndex++)						// Zuordnungs-Array anpassen
		{
		   bItem = pData->aColumnPos[uIndex-1];
		if(bItem<uCol)continue;

		bItem++;
		pData->aColumnPos[uIndex] = bItem;
		}

	for(;uIndex<=pData->uColumnCount;uIndex++)				// Spaltenpositionen verschieben
		{
		bItem = pData->aColumnPos[uIndex+1];

		if(bItem>=uCol)
			{
			uCol--;
			}

		pData->aColumnPos[uIndex] = bItem;
		}

	for(uIndex=pData->uColumnCount;uIndex>0;)
		{
		uIndex--;
		bByte = pData->aColumn[uIndex].bIndex;

		if(bByte>=uSub)
			{
			bByte--;
			pData->aColumn[uIndex].bIndex = bByte;
			}

		pData->aColumn[uIndex].bNext = pData->aColumnPos[bByte+1];
		}


	pData->iFixSize   = iFix;
	pData->aColumn[pData->uColumnCount].bWeight=0;


	if(iCnt && iDelta)										// Variable Breiten anpassen
		{
		ChangeColSize(pData,iDelta);
		}
	else{
		if(iSub && !iCnt)
			{
			pData->iVarSize = 0;
			}
		}

	if(pData->uSelectedSub>uCol)							// Ist die Auswahl vor der Spalte
		{
		pData->uSelectedSub--;
		}

	if(pData->uEditSub>uCol)
		{
		pData->uEditSub--;
		}

	if(pData->uFocusSub>uCol)
		{
		pData->uFocusSub--;
		}

	if(pData->uTrackedSub>uCol)
		{
		pData->uTrackedSub--;
		}


	pData->aColumnPos[pData->uColumnCount] = pData->uColumnCount;


	if(!pData->uColumnCount)								// Den Header l?chen
		{
		DestroyWindow(pData->hHeader);
		pData->hHeader	   = NULL;
		pData->uStartPixel = 0;
		pData->iRowHeight  = 1;
		UpdateHeight  (pData);
		InvalidateRect(pData->hWnd,&sRect,FALSE);
		}


	   iXoff=UpdateColumns(pData);							// Hat sich die Spaltenbreiten ver?dert
	if(iXoff<0x10000)
		{
		sRect.left  = iXoff;
		sRect.left -= pData->uScrollX;
		sRect.top   = pData->uStartPixel;
		InvalidateRect(pData->hWnd,&sRect,FALSE);
		}


	UpdateScrollX(pData);




return 1;
}


//*****************************************************************************
//*
//*		TreeListInsertColumn
//*
//*****************************************************************************
//	F?t eine neue Spalte in den Header ein
//	pData		: Zeiger auf die Fensterdaten
//	uCol		: Ist die Nummer der Spalte die eingef?t wird
//	pInsert		: Zeiger auf die ein zu f?enden Daten
//	Ergibt die Einf?eposition der neuen Spalte bzw. -1 bei eimem Fehler
static int TreeListInsertColumn(TreeListData *pData,unsigned uCol,TV_COLUMN *pColumn)
{
ExtraItem **pList;
TV_COLSIZE	sNotify;
HDITEM	 	sItem;
RECT		sRect;
short 		sFixed;
UINT		uIndex;
BYTE		bByte;
BYTE		bItem;
BYTE		bMark;
BYTE		bDraw;
BYTE		bMinEx;
BYTE		bAlign;
int			iWeight;
int			iDelta;
int			iStart;
int			iSize;
int			iXoff;
int			iYoff;
int			iNum;
int			iVar;
int			iAll;
int			iAdd;
int			iMin;
int			iFix;



	GetClientRect(pData->hWnd,&sRect);


	if(!pData->hHeader)										// Einen neuen Header erzeugen
		{
		iStart = GetSystemMetrics(SM_CYHSCROLL);
		iYoff  = sRect.top+iStart;

		if(pData->uStyleEx&TVS_EX_HIDEHEADERS)
			{
			iYoff=0;
			}

			pData->hHeader=CreateWindow(WC_HEADER,NULL,WS_VISIBLE|WS_CHILD|HDS_HORZ|HDS_BUTTONS|HDS_DRAGDROP,sRect.left,sRect.top,sRect.right,iYoff,pData->hWnd,(HMENU)1,NULL,NULL);
		if(!pData->hHeader)return -1;

		pData->uStartPixel = (pData->uStyleEx&TVS_EX_HIDEHEADERS)? 0:iStart;
		pData->iRowHeight  = 1;
		UpdateHeight(pData);


		InvalidateRect(pData->hWnd,&sRect,FALSE);
		SendMessage   (pData->hHeader,HDM_SETIMAGELIST,0,(LPARAM)((pData->hHdrImg)? pData->hHdrImg:pData->hImages));
		SendMessage   (pData->hHeader,WM_SETFONT,(WPARAM)hDefaultFontN,0);

		if(pData->uSizeX<=pData->uStartPixel)
				pData->uSizeYsub = 0;
		else	pData->uSizeYsub = pData->uSizeX-pData->uStartPixel;
		}


	if(pData->uColumnCount>=MAX_COLUMNS)					// Pr?e die Anzahl der Spalten
		{
		return -1;
		}


	memset(&sItem,0,sizeof(sItem));							// Die Spaltendaten zusammenstellen

	if(uCol>=pData->uColumnCount)
		{
		uCol = pData->uColumnCount;
		}

	if(pColumn->mask&TVCF_FMT)								// Welche Textausrichtung
		{
		sItem.mask    |= HDI_FORMAT;
		sItem.fmt      = pColumn->fmt;

		switch(sItem.fmt&HDF_JUSTIFYMASK)
			{
		case HDF_CENTER: bAlign = DT_CENTER;	break;
		case HDF_RIGHT:	 bAlign = DT_RIGHT;		break;
		default: 		 bAlign = DT_LEFT;		break;
			}
		}
	else{
		bAlign = DT_LEFT;
		}

	if(pColumn->mask&TVCF_IMAGE)							// Hat die Spalte auch ein Icon
		{
		sItem.mask    |=  HDI_IMAGE;
		sItem.iImage   =  pColumn->iImage;
		}

	if(pColumn->mask&TVCF_TEXT)								// Auch einen Text ?ergeben
		{
		sItem.mask    |=  HDI_TEXT;
		sItem.pszText  =  pColumn->pszText;
		}

	if(pColumn->mask&TVCF_MIN)								// Auch einen Min-Wert ?ergeben
		{
		iMin		   =  pColumn->iOrder;
		bMinEx		   =  1;

		if(iMin<0)
			{
			iMin	   = -iMin;
			bMinEx	   =  0;
			}
		}
	else{
		iMin		   =  16;
		bMinEx		   =  0;
		}

	if(pColumn->mask&TVCF_WIDTH)							// Fixe Breite f? die Spalte
		{
		iWeight		   =  0;
		sItem.mask    |=  HDI_WIDTH;
		sItem.cxy	   =  pColumn->cx;
		iSize		   =  pColumn->cx;
		iDelta		   = -pColumn->cx;
		iAdd		   =  0;
		}
	else{													// Variable vordefinierte Breite
		if(pColumn->mask&TVCF_VWIDTH)
			   iWeight =  pColumn->cx;
		else   iWeight =  1;

		iVar  = pData->iVarSize;
		iFix  = pData->iFixSize;
		iSize = pData->uSizeX-iVar-iFix;

		if(iWeight<= 0)iWeight=1;
		if(iWeight>255)iWeight=255;


		if(pData->uColumnCountVar)							// Gibt es schon variable Spalten
			{
			for(uIndex=0,iAll=0;uIndex<pData->uColumnCount;uIndex++)
				{
				if(pData->aColumn[uIndex].sFixed)continue;
				iAll += pData->aColumn[uIndex].bWeight;
				}

			iSize  = (iVar*iWeight)/(iAll+iWeight);
			iDelta = -iSize;
			}
		else{
			iDelta = 0;
			}

		sItem.mask    |= HDI_WIDTH;
		sItem.cxy      = iSize;
		iAdd		   = 1;

		if(sItem.cxy<iMin)sItem.cxy=iMin;
		}



	   uCol = Header_InsertItem(pData->hHeader,uCol,&sItem);
	if(uCol&0x80000000)return -1;


	if(pData->uColumnCount>0)								// Liste mit Extraeintr?en erzeugen
		{
			pList = new ExtraItem*[pData->uTreeItemsMax+1];
		if(!pList)
			{
			Header_DeleteItem(pData->hHeader,uCol);
			return -1;
			}

		memset(pList,0,sizeof(ExtraItem*)*(pData->uTreeItemsMax+1));

		   iNum = uCol-1;
		if(iNum<0)iNum=0;

		memmove(pData->pExtraItems+iNum+1,pData->pExtraItems+iNum,sizeof(pList)*(MAX_COLUMNS-2-iNum));
		pData->pExtraItems[iNum]=pList;
		}


	memmove(pData->aColumn+uCol+1,pData->aColumn+uCol,(MAX_COLUMNS-1-uCol)*sizeof(ColumnData));

	for(uIndex=pData->uColumnCount+2;uIndex>uCol;uIndex--)	// Zuordnungs-Array anpassen
		{
		   bItem = pData->aColumnPos[uIndex-1];
		if(bItem>=uCol)bItem++;

		pData->aColumnPos[uIndex] = bItem;
		}

	pData->aColumnPos[uCol] = (BYTE)uCol;

	while(uIndex>0)
		{
		uIndex--;

		   bItem=pData->aColumnPos[uIndex];
		if(bItem<uCol)continue;

		bItem++;
		pData->aColumnPos[uIndex] = bItem;
		}

	for(uIndex=pData->uColumnCount;;)						// Folgende Spalten verschieben
		{
		bByte = pData->aColumn[uIndex].bIndex;

		if(bByte>=uCol)
			{
			bByte++;
			pData->aColumn[uIndex].bIndex = bByte;
			}

		if(uIndex==0)break;
		uIndex--;
		}


	bMark  = 0;
	bDraw  = 0;
	sFixed = 0;

	if(pColumn->mask&TVCF_MARK  )							// Ist die Spalte markiert
	if(pColumn->fmt &TVCFMT_MARK)
		{
		bMark = 1;
		}

	if(pColumn->mask&TVCF_FIXED  )							// Ist die Spalte fixiert
	if(pColumn->fmt &TVCFMT_FIXED)
		{
		sFixed = (short)((sItem.cxy>0)? sItem.cxy:100);
		}

	if(pColumn->mask&TVCF_OWNERDRAW  )						// Spalte via Callback zeichnen
	if(pColumn->fmt &TVCFMT_OWNERDRAW)
		{
		bDraw = 1;
		}

	pData->aColumn[uCol].bWeight = (BYTE)iWeight;
	pData->aColumn[uCol].sReal   = (short)sItem.cxy;
	pData->aColumn[uCol].sSize   = (short)iSize;
	pData->aColumn[uCol].sMin    = (short)iMin;
	pData->aColumn[uCol].bIndex  = (BYTE)uCol;
	pData->aColumn[uCol].bMinEx  =  bMinEx;
	pData->aColumn[uCol].bAlign  =  bAlign;
	pData->aColumn[uCol].bMark   =  bMark;
	pData->aColumn[uCol].bDraw   =  bDraw;
	pData->aColumn[uCol].sFixed  =  sFixed;
	pData->uMarkedCols			+= 	bMark;


	for(uIndex=pData->uColumnCount;;)						// N?hste sichtbare Spalten aktualisieren
		{
		bByte						 = pData->aColumn[uIndex].bIndex;
		pData->aColumn[uIndex].bNext = pData->aColumnPos[bByte+1];
		if(uIndex==0)break;
		uIndex--;
		}


	if(pData->uColumnCountVar)								// Variable Breiten anpassen
		{
		if(iDelta)
			{
			ChangeColSize(pData,iDelta);
			pData->iVarSize  -= iDelta;
			}
		else if(!iAdd)
			{
			pData->iFixSize  += iSize;
			}
		}
	else{
		if(iAdd)pData->iVarSize  = iSize;
		else    pData->iFixSize += iSize;
		}


	pData->uColumnCountVar      +=  iAdd;
	pData->uColumnCount	        +=  1;


	if(pData->uSelectedSub>0 && pData->uSelectedSub>=uCol)
		{
		pData->uSelectedSub++;
		}

	if(pData->uTrackedSub>0 && pData->uTrackedSub>=uCol)
		{
		pData->uTrackedSub++;
		}

	if(pData->uFocusSub>0 && pData->uFocusSub>=uCol)
		{
		pData->uFocusSub++;
		}

	if(pData->uEditSub>0 && pData->uEditSub>=uCol)
		{
		pData->uEditSub++;
		}



	   iXoff=UpdateColumns(pData);							// Hat sich die Spaltenbreiten ver?dert
	if(iXoff<0x10000)
		{
		sRect.left  = iXoff;
		sRect.left -= pData->uScrollX;
		sRect.top   = pData->uStartPixel;
		InvalidateRect(pData->hWnd,&sRect,FALSE);
		}


	UpdateScrollX(pData);


	if(pData->uInsertMark)									// Fehlende Infomarken einf?en
		{
		TV_ITEM		sSet;
		ExtraItem  *pExtra;

		sSet.mask		= TVIF_SUBITEM;
		sSet.hItem		= (HTREEITEM)pData->uInsertMark;
		sSet.cChildren	= uCol;

		TreeListSetItem(pData,&sSet);

		   pExtra = pData->pExtraItems[uCol-1][pData->uInsertMark];
		if(pExtra)
			{
			pExtra->uColorBk  = pData->aColors[TVC_INSERT];
			pExtra->bFlags	 |= TVIX_BKCOLOR;
			}
		}


	if(pData->uStyleEx&TVS_EX_HEADERCHGNOTIFY)				// Ge?derte Spalten melden
		{
 		sNotify.hdr.code			= TVN_COLUMNCHANGED;
		sNotify.uColumn				= uCol;
		sNotify.uIndex				= pData->aColumn[uCol].bIndex;
		sNotify.uPosX				= pData->aColumnXpos[uCol];
		sNotify.iSize				= pData->aColumn[uCol].sReal;

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);
		LOCK(pData);
		}


return uCol;
}

//*****************************************************************************
//*
//*		TreeListScanColumn
//*
//*****************************************************************************
//	Berechnet die Breite der sichtbaren Eintr?e einer Spalte
//	pData		: Zeiger auf die Fensterdaten
//	uSub		: Ist die Spalte
//	Ergibt sie gescannte Breite
static int TreeListScanColumn(TreeListData *pData,unsigned uSub)
{
BaseItem  **pList;
BaseItem   *pEntry;
ExtraItem  *pExtra;
ExtraItem **pItems;
unsigned   *pPList;
unsigned	uPos;
int			iMax;
int			iPos;



	if(uSub>=pData->uColumnCount)return 0;

	if(uSub>0)												// Extraspalte
		{
		pItems = pData->pExtraItems[uSub-1];
		pPList = pData->pItemPos;
		iMax   = 0;

		for(uPos=0;uPos<pData->uItemPosCount;uPos++)
			{
				pExtra = pItems[pPList[uPos]];
			if(!pExtra)
				{
				if(iMax<8)iMax=8;
				continue;
				}

			if(pData->hSubImg && (pExtra->bFlags&TVIX_HASIMAGE))
					iPos = pData->iSubImgXsize;
			else	iPos = 0;

			   iPos += pExtra->iTextPixels+8;
			if(iPos>iMax)iMax=iPos;
			}

		return iMax;
		}


	pList  = pData->pTreeItems;
	pPList = pData->pItemPos;
	iMax   = 0;


	for(uPos=0;uPos<pData->uItemPosCount;uPos++)			// Erste Spalte
		{
		pEntry = pList[pPList[uPos]];

		if(pEntry->bFlags&TVIX_HASIMAGE)
				iPos = pData->iImagesSizeX;
		else	iPos = 0;

		   iPos += pEntry->uLevel*pData->iIndent;
		   iPos += pEntry->iTextPixels+8;
		if(iPos>iMax)iMax=iPos;
		}

	if(pData->uStyleEx&TVS_EX_ITEMLINES)
		{
		iMax += 1;
		}

	if(pData->cHasRootRow)
		{
		iMax += pData->iIndent;
		}

	if(pData->hStates)
		{
		iMax += pData->iStatesSizeX;
		}



return iMax;
}


//*****************************************************************************
//*
//*		TreeListHitTest
//*
//*****************************************************************************
//	Pr?t wo eine Koordinate im Fenster ist
//	pData		: Zeiger auf die Fensterdaten
//	pInfo		: Zeiger auf die ein zu f?enden Daten
//	Ergibt das Item auf dem die Koordinate zeigt
static unsigned TreeListHitTest(TreeListData *pData,TV_HITTESTINFO *pInfo)
{
BaseItem   *pEntry;
ExtraItem  *pExtra;
unsigned	uItem;
unsigned	uNext;
unsigned	uSub;
unsigned	uCol;
int			iPosX;
int			iPosY;
int			iPosZ;
int			iWidth;
int			iIcon;



	iPosX = pInfo->pt.x;
	iPosY = pInfo->pt.y;

	if((unsigned)iPosX>=pData->uSizeX)						// Au?rhalb der X-Größe
		{
		pInfo->hItem = NULL;
		pInfo->flags = (iPosX<0)? TVHT_TOLEFT:TVHT_TORIGHT;
		return 0;
		}

	iPosY -= pData->uStartPixel;

	if((unsigned)iPosY>=pData->uSizeY)						// Au?rhalb der Y-Größe
		{
		pInfo->hItem = NULL;
		pInfo->flags = (iPosY<0)? TVHT_ABOVE:TVHT_BELOW;
		return 0;
		}


	iPosZ = iPosY/pData->iRowHeight;

	if((unsigned)iPosZ>=pData->uFixedLines)					// Gibt es fixierte Linien
		{
		iPosZ += pData->uScrollY;
		}

	if((unsigned)iPosZ>=pData->uItemPosCount)				// Gibt es keinen Eintrag auf der Zeile
		{
		pInfo->hItem = NULL;
		pInfo->flags = TVHT_NOWHERE;
		return 0;
		}

	iPosX		+= pData->uScrollX;
	uItem		 = pData->pItemPos  [iPosZ];
	pEntry		 = pData->pTreeItems[uItem];
	pInfo->hItem = (HTREEITEM)uItem;


	if(!pEntry)return 0;

	uSub = pData->aColumnPos[1];

	if(iPosX>=pData->aColumnXpos[uSub])						// Auf Extraeintrag
		{
		for(uCol=1;uCol<pData->uColumnCount;uCol++)
			{
			uSub  = pData->aColumnPos[uCol  ];
			uNext = pData->aColumnPos[uCol+1];

			if(iPosX>=pData->aColumnXpos[uNext])continue;
			   iPosX-=pData->aColumnXpos[uSub ];

			pExtra = pData->pExtraItems[uSub-1][uItem];


			if(pData->aColumn[uSub].bEdit>=TVAX_CHECK)		// Hat der Extraeintrag ein Icon
				{
				iIcon = pData->iChecksSizeX;
				}
			else if(pExtra && (pExtra->bFlags&TVIX_HASIMAGE))
				{
				iIcon = pData->iImagesSizeX;
				}
			else{
				iIcon = 0;
				}

			pInfo->flags = uSub<<24;

			if(iPosX<iIcon)									// Auf Icon
				{
				pInfo->flags |= TVHT_ONSUBICON;
				return uItem;
				}

			if(!pExtra || !pExtra->uTextSize)				// Auf Text wenn leerer Eintrag
				{
				pInfo->flags |= TVHT_ONSUBLABEL;

				if(pData->aColumn[uSub].bAlign==DT_RIGHT)	// AUf den ersten 20-Pixel des Leerteils
					{
					iWidth  = pData->aColumnXpos[uNext];
					iWidth -= pData->aColumnXpos[uSub ];
					if(iPosX>=iWidth-20)pInfo->flags|=TVHT_ONSUBBASE;
					}
				else{
					if(iPosX-iIcon  <20)pInfo->flags|=TVHT_ONSUBBASE;
					}

				return uItem;
				}

			switch(pData->aColumn[uSub].bAlign)				// Textausrichtung
				{
			default:		if(iPosX-iIcon<pExtra->iTextPixels+5)
								{
								pInfo->flags |= TVHT_ONSUBLABEL|TVHT_ONSUBBASE;
								return uItem;
								}
							break;

			case DT_RIGHT:	iWidth  = pData->aColumnXpos[uNext];
							iWidth -= pData->aColumnXpos[uSub ];

							if(iPosX>=iWidth-pExtra->iTextPixels-5)
								{
								pInfo->flags |= TVHT_ONSUBLABEL|TVHT_ONSUBBASE;
								return uItem;
								}
							break;

			case DT_CENTER:	iWidth  = pData->aColumnXpos[uNext];
							iWidth -= pData->aColumnXpos[uSub ];
							iWidth += iIcon;
							iWidth /= 2;

							if(iPosX>=iWidth-pExtra->iTextPixels/2-3)
							if(iPosX<=iWidth+pExtra->iTextPixels/2+3)
								{
								pInfo->flags |= TVHT_ONSUBLABEL|TVHT_ONSUBBASE;
								return uItem;
								}
							break;
				}

			pInfo->flags |= TVHT_ONSUBRIGHT;

			return uItem;
			}


		pInfo->flags = TVHT_ONRIGHTSPACE;

		return uItem;
		}


	if(!pData->cHasRootRow)									// Root-Linien ausgleichen
		{
		iPosX += pData->iIndent;
		}


	iPosX -= pData->iIndent*pEntry->uLevel;

	if(iPosX<pData->iIndent)								// Auf einger?ktem Bereich
		{
		if(pData->uStyle &TVS_HASBUTTONS)
		if(pEntry->bFlags&TVIX_HASBUTTON)
			{
			if(iPosX>=pData->iShift-6)
			if(iPosX<=pData->iShift+7)
				{
				iPosY %= pData->iRowHeight;
				iPosY -= pData->iRowHeight/2;

				if(iPosY>=-6 && iPosY<=7)
					{
					pInfo->flags = TVHT_ONITEMBUTTON;
					return uItem;
					}
				}
			}

		pInfo->flags = TVHT_ONITEMINDENT;
		return uItem;
		}


	iPosX -= pData->iIndent;

	if(pData->uStyleEx&TVS_EX_ITEMLINES)
		{
		iPosX--;
		}

	if(pData->hStates)										// Auf der Auswahl-Box
		{
		iPosX -= pData->iStatesSizeX;

		if(iPosX<0)
			{
			pInfo->flags = TVHT_ONITEMSTATEICON;
			return uItem;
			}
		}


	if(pEntry->bFlags&TVIX_HASIMAGE)						// Auf dem Icon
		{
		iPosX -= pData->iImagesSizeX;

		if(pData->uStyleEx&TVS_EX_ITEMLINES)iPosX--;

		if(iPosX<0)
			{
			pInfo->flags = TVHT_ONITEMICON;
			return uItem;
			}
		}
															// Auf Text
	if(iPosX<pEntry->iTextPixels+5)
		{
		pInfo->flags = TVHT_ONITEMLABEL|TVHT_ONITEMBASE;
		}
	else if(!pEntry->pText || !pEntry->pText[0])			// Leertext
		{
		pInfo->flags = (iPosX<20)? TVHT_ONITEMLABEL|TVHT_ONITEMBASE:TVHT_ONITEMLABEL;
		}
	else{
		pInfo->flags = TVHT_ONITEMRIGHT;
		}



return uItem;
}

//*****************************************************************************
//*
//*		TreeListSetInsertMark
//*
//*****************************************************************************
//	Setzt das den Einf?eeintrag
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages bei dem eingef?t werden soll
//	iMode	: 0=davor einf?en  1=nachher einf?en
//	Ergibt 1 wenn der Eintrag eingef?t wurde
static unsigned TreeListSetInsertMark(TreeListData *pData,unsigned uItem,int iMode)
{
TV_INSERTSTRUCT	sInsert;
ExtraItem	   *pExtra;
BaseItem	   *pEntry;
unsigned		uSub;
int				iRet;



	if(pData->uInsertMark)
		{
		iRet=TreeListDeleteItem(pData,pData->uInsertMark,1);
		pData->uInsertMark=0;
		}
	else{
		iRet=0;
		}

	if(uItem==0)return iRet;
	if(uItem>pData->uTreeItemsMax)return 0;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;


	if(iMode)
		{
			uItem=pEntry->uPrevItem;
		if(!uItem)uItem=U(TVI_FIRST);
		}

	sInsert.hParent		 		= (HTREEITEM)pEntry->uParent;
	sInsert.hInsertAfter 		= (HTREEITEM)uItem;
	sInsert.item.mask	 		= TVIF_SELECTEDIMAGE|TVIF_IMAGE;
	sInsert.item.iImage			= TV_NOIMAGE;
	sInsert.item.iSelectedImage	= TV_NOIMAGE;

		uItem = TreeListInsertItem(pData,&sInsert);
	if(!uItem)return 0;


	pEntry				= pData->pTreeItems[uItem];
	pEntry->uColorBk	= pData->aColors[TVC_INSERT];
	pEntry->bFlags	   |= TVIX_BKCOLOR;
	sInsert.item.mask  |= TVIF_SUBITEM;
	sInsert.item.hItem	= (HTREEITEM)uItem;

	for(uSub=1;uSub<pData->uColumnCount;uSub++)
		{
		sInsert.item.cChildren	= uSub;

		TreeListSetItem(pData,&sInsert.item);

		pExtra = pData->pExtraItems[uSub-1][uItem];
		pExtra->uColorBk	= pData->aColors[TVC_INSERT];
		pExtra->bFlags	   |= TVIX_BKCOLOR;
		}


	pData->uInsertMark = uItem;



return uItem;
}

//*****************************************************************************
//*
//*		TreeListGetItemColor
//*
//*****************************************************************************
//	Holt die Farbe eins Feldes
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages
//	uSub	: Ist die Spalte
//	iMode	: 0=Hintergrundfarbe 1=Textfarbe abfragen
//	Ergibt die alte Farbe oder TV_NOCOLOR wenn keine Farbe eingestellt war
static LRESULT TreeListGetItemColor(TreeListData *pData,unsigned uItem,unsigned uSub,int iMode)
{
COLORREF	uColor;
ExtraItem  *pExtra;
BaseItem   *pEntry;



	if(uItem>pData->uTreeItemsMax)return TV_NOCOLOR;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return TV_NOCOLOR;


	if(uSub)												// Extra-Eintrag abfragen
		{
		if(uSub>=pData->uColumnCount)return TV_NOCOLOR;

			pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)return TV_NOCOLOR;

		if(iMode)
				uColor = (pExtra->bFlags&TVIX_TEXTCOLOR)? pExtra->uColorText:TV_NOCOLOR;
		else	uColor = (pExtra->bFlags&TVIX_BKCOLOR  )? pExtra->uColorBk  :TV_NOCOLOR;
		}
	else{
		if(iMode)
				uColor = (pEntry->bFlags&TVIX_TEXTCOLOR)? pEntry->uColorText:TV_NOCOLOR;
		else	uColor = (pEntry->bFlags&TVIX_BKCOLOR  )? pEntry->uColorBk  :TV_NOCOLOR;
		}



return (LRESULT)uColor;
}

//*****************************************************************************
//*
//*		TreeListSetItemColor
//*
//*****************************************************************************
//	Setzt die Farbe eines Feldes
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages
//	uSub	: Ist die Spalte
//	uColor	: Ist die neue Farbe
//	iMode	: 0=Hintergrundfarbe 1=Textfarbe einstellen
//	Ergibt die alte Farbe oder TV_NOCOLOR wenn keine Farbe eingestellt war
static COLORREF TreeListSetItemColor(TreeListData *pData,unsigned uItem,unsigned uSub,COLORREF uColor,int iMode)
{
TV_ITEM		sSet;
COLORREF	uOld;
ExtraItem  *pExtra;
BaseItem   *pEntry;



	if(uItem>pData->uTreeItemsMax)return TV_NOCOLOR;

		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return TV_NOCOLOR;


	if(uSub>=255)											// Die Ganze Zeile ?dern
		{
		if(pData->uColumnCount)
			{
			for(uSub=pData->uColumnCount;uSub>0;uSub--)
				{
				TreeListSetItemColor(pData,uItem,uSub,uColor,iMode);
				}
			}

		uSub = 0;
		}


	if(uSub)												// Extra-Eintrag ver?dern
		{
		if(uSub>=pData->uColumnCount)return TV_NOCOLOR;

			pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)											// Extra-Eintrag erzeugen
			{
			sSet.mask		= TVIF_SUBITEM;
			sSet.hItem		= (HTREEITEM)uItem;
			sSet.cChildren	= uSub;

			if(!TreeListSetItem(pData,&sSet))return TV_NOCOLOR;

			pExtra = pData->pExtraItems[uSub-1][uItem];
			}

		if(iMode)											// Textfarbe
			{
			uOld = (pExtra->bFlags&TVIX_TEXTCOLOR)? pExtra->uColorText:TV_NOCOLOR;

			if(uColor==TV_NOCOLOR)
				{
				pExtra->bFlags	   &= ~TVIX_TEXTCOLOR;
				}
			else{
				pExtra->bFlags	   |=  TVIX_TEXTCOLOR;
				pExtra->uColorText  =  uColor;
				}
			}
		else{												// Hintergrund
			uOld = (pExtra->bFlags&TVIX_BKCOLOR)? pExtra->uColorBk:TV_NOCOLOR;

			if(uColor==TV_NOCOLOR)
				{
				pExtra->bFlags	   &= ~TVIX_BKCOLOR;
				}
			else{
				pExtra->bFlags	   |=  TVIX_BKCOLOR;
				pExtra->uColorBk    =  uColor;
				}
			}
		}
	else{
		if(iMode)											// Textfarbe
			{
			uOld = (pEntry->bFlags&TVIX_TEXTCOLOR)? pEntry->uColorText:TV_NOCOLOR;

			if(uColor==TV_NOCOLOR)
				{
				pEntry->bFlags	   &= ~TVIX_TEXTCOLOR;
				}
			else{
				pEntry->bFlags	   |=  TVIX_TEXTCOLOR;
				pEntry->uColorText  =  uColor;
				}
			}
		else{												// Hintergrund
			uOld = (pEntry->bFlags&TVIX_BKCOLOR)? pEntry->uColorBk:TV_NOCOLOR;

			if(uColor==TV_NOCOLOR)
				{
				pEntry->bFlags	   &= ~TVIX_BKCOLOR;
				}
			else{
				pEntry->bFlags	   |=  TVIX_BKCOLOR;
				pEntry->uColorBk    =  uColor;
				}
			}
		}


	if(uColor!=uOld)										// Neu zeichnen
		{
		UpdateRect(pData,uItem,uSub);
		}



return uOld;
}

//*****************************************************************************
//*
//*		TreeListSetTrackItem
//*
//*****************************************************************************
//	Setzt das unterstrichene Item
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages
//	uSub	: Ist die Spalte
//	Ergibt 1 wenn der Eintrag unterstrichen wurde
static int TreeListSetTrackItem(TreeListData *pData,unsigned uItem,unsigned uSub)
{
ExtraItem	   *pExtra;
BaseItem	   *pEntry;
TRACKMOUSEEVENT sEvent;
int				iRet=1;


	if(!(pData->uStyleEx&TVS_EX_SUBSELECT))
		{
		uSub=0;
		}
	else{
		if(uSub>=pData->uColumnCount)
			{
			uItem = 0;
			uSub  = 0;
			iRet  = 0;
			}
		}

	if(uItem>pData->uTreeItemsMax)
		{
		uItem = 0;
		uSub  = 0;
		iRet  = 0;
		}
	else{
		if(uItem==pData->uTrackedItem)
		if(uSub ==pData->uTrackedSub )
			{
			return iRet;
			}
		}




	if(pData->uTrackedItem)									// Den alten Eintrag zur?ksetzen
		{
		if(pData->uTrackedSub)
			{
			   pExtra = pData->pExtraItems[pData->uTrackedSub-1][pData->uTrackedItem];
			if(pExtra)
				{
				pExtra->bFlags &= ~TVIX_TRACKED;
				UpdateRect(pData,pData->uTrackedItem,pData->uTrackedSub);
				}
			}
		else{
			   pEntry = pData->pTreeItems[pData->uTrackedItem];
			if(pEntry)
				{
				pEntry->bFlags &= ~TVIX_TRACKED;
				UpdateRect(pData,pData->uTrackedItem,0);
				}
			}
		}


	if(uItem)												// Den neuen Eintrag setzen
		{
		sEvent.cbSize		= sizeof(TRACKMOUSEEVENT);
		sEvent.dwFlags		= TME_LEAVE;
		sEvent.hwndTrack	= pData->hWnd;

		_TrackMouseEvent(&sEvent);

		if(uSub)
			{
			   pExtra = pData->pExtraItems[uSub-1][uItem];
			if(pExtra)
				{
				pData->uTrackedSub  = uSub;
				pData->uTrackedItem = uItem;
				pExtra->bFlags     |= TVIX_TRACKED;
				UpdateRect(pData,uItem,uSub);
				}
			else{
				iRet=0;
				}
			}
		else{
			   pEntry = pData->pTreeItems[uItem];
			if(pEntry)
				{
				pData->uTrackedSub  = 0;
				pData->uTrackedItem = uItem;
				pEntry->bFlags     |= TVIX_TRACKED;
				UpdateRect(pData,uItem,0);
				}
			else{
				iRet=0;
				}
			}
		}
	else{													// Keine Untersteichung
		pData->uTrackedSub  = 0;
		pData->uTrackedItem = 0;
		}


return iRet;
}

//*****************************************************************************
//*
//*		TreeListGetNextItem
//*
//*****************************************************************************
//	Sucht den n?hsten Eintrag
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages
//	uFlags	: Bestimmt nach welchem Eintrag gesucht werden soll
//	Ergibt 1 wenn der Eintrag unterstrichen wurde
static unsigned TreeListGetNextItem(TreeListData *pData,unsigned uItem,unsigned uFlags)
{
BaseItem	   *pEntry;
unsigned		uStop;
unsigned		uPos;


	switch(uFlags)
		{
	case TVGN_ROOT:				return pData->uFirstChild;

	case TVGN_NEXT:				if(uItem>pData->uTreeItemsMax)
									{
									return 0;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								return pEntry->uNextItem;

	case TVGN_PREVIOUS:			if(uItem>pData->uTreeItemsMax)
									{
									return 0;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								return pEntry->uPrevItem;


	case TVGN_PARENT:			if(uItem>pData->uTreeItemsMax)
									{
									return 0;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								return pEntry->uParent;


	case TVGN_CHILD:			if(uItem>pData->uTreeItemsMax)
									{
									if(uItem==U(TVI_ROOT))return pData->uFirstChild;
									return 0;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								return pEntry->uFirstChild;


	case TVGN_LASTCHILD:		if(uItem>pData->uTreeItemsMax)
									{
									if(uItem==U(TVI_ROOT))return pData->uLastChild;
									return 0;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								return pEntry->uLastChild;


	case TVGN_NONEFIXED:		if(pData->uItemPosCount<=pData->uFixedLines)return 0;

								uItem = pData->pItemPos[pData->uFixedLines];

								if(uItem>pData->uTreeItemsMax || !pData->pTreeItems[uItem])
									{
									return 0;
									}

								return uItem;

	case TVGN_FIRSTNONEFIXED:	if(pData->uFixedLines)
									{
									if(pData->uItemPosCount<=pData->uFixedLines)return 0;

									uPos = pData->uScrollY+pData->uFixedLines;

									if(pData->uItemPosCount<=uPos)return 0;

									uItem = pData->pItemPos[uPos];

									if(uItem>pData->uTreeItemsMax || !pData->pTreeItems[uItem])
										{
										return 0;
										}

									return uItem;
									}

	case TVGN_FIRSTVISIBLE:		if(pData->uItemPosCount<=0)return 0;
								if(pData->uItemPosCount<=pData->uScrollY)return 0;

								uItem = pData->pItemPos[pData->uScrollY];

								if(uItem>pData->uTreeItemsMax || !pData->pTreeItems[uItem])
									{
									return 0;
									}

								return uItem;

	case TVGN_NEXTVISIBLE:		if(uItem>pData->uTreeItemsMax)
									{
									if(uItem!=U(TVI_ROOT))return 0;
									if(pData->uFirstChild==0)return 0;

									pEntry = pData->pTreeItems[pData->uFirstChild];

									   uPos=pEntry->uShowPos;
									if(uPos<=pData->uScrollY)return 0;

									return pData->uFirstChild;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								   uPos=pEntry->uShowPos;
								if(uPos<=pData->uScrollY)return 0;
								if(uPos> pData->uScrollY+pData->uPageEnties)return 0;

								uItem = pData->pItemPos[uPos];

								if(uItem>pData->uTreeItemsMax || !pData->pTreeItems[uItem])
									{
									return 0;
									}

								return uItem;


	case TVGN_NEXTSELECTED:		if(uItem>pData->uTreeItemsMax)
									{
									if(uItem!=U(TVI_ROOT))return 0;

									if(pData->uSelectedCount==1 && pData->uSelectedItem)
										{
										return pData->uSelectedItem;
										}

									uItem = pData->uFirstChild;

										pEntry = pData->pTreeItems[uItem];
									if(!pEntry)return 0;
									if( pEntry->uState&TVIS_SELECTED)return uItem;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								if(pData->uSelectedCount==1 && uItem==pData->uSelectedItem)
									{
									return 0;
									}

								for(;;)
									{
									if(pEntry->uFirstChild)
										{
										uItem = pEntry->uFirstChild;
										}
									else if(pEntry->uNextItem)
										{
										uItem = pEntry->uNextItem;
										}
									else{
										for(;;)
											{
												uItem  = pEntry->uParent;
												pEntry = pData ->pTreeItems[uItem];
											if(!pEntry)return 0;
											if( pEntry->uNextItem)
												{
												uItem=pEntry->uNextItem;
												break;
												}
											}
										}

										pEntry = pData->pTreeItems[uItem];
									if(!pEntry)break;
									if( pEntry->uState&TVIS_SELECTED)return uItem;
									}

								return 0;

	case TVGN_NEXTSELCHILD:		if(uItem>pData->uTreeItemsMax)
									{
									if(uItem!=U(TVI_ROOT))return 0;

									uItem = pData->uFirstChild;

										pEntry = pData->pTreeItems[uItem];
									if(!pEntry)return 0;
									if( pEntry->uState&TVIS_SELECTED)return uItem;

									uStop = 0;
									}
								else{
										pEntry = pData->pTreeItems[uItem];
									if(!pEntry || !pEntry->uFirstChild)return 0;

									uStop = uItem;
									}

								for(;;)
									{
									if(pEntry->uFirstChild)
										{
										uItem = pEntry->uFirstChild;
										}
									else if(pEntry->uNextItem)
										{
										uItem = pEntry->uNextItem;
										}
									else{
										for(;;)
											{
												uItem= pEntry->uParent;
											if( uItem==uStop)return 0;
												pEntry = pData ->pTreeItems[uItem];
											if(!pEntry)return 0;
											if( pEntry->uNextItem)
												{
												uItem=pEntry->uNextItem;
												break;
												}
											}
										}

										pEntry = pData ->pTreeItems[uItem];
									if(!pEntry)break;
									if( pEntry->uState&TVIS_SELECTED)return uItem;
									}

								return 0;


	case TVGN_ENUM:				if(uItem>pData->uTreeItemsMax)
									{
									if(uItem!=U(TVI_ROOT))return 0;

									uItem = pData->uFirstChild;

										pEntry = pData->pTreeItems[uItem];
									if(!pEntry)return 0;

									return uItem;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								for(;;)
									{
									if(pEntry->uFirstChild)
										{
										uItem = pEntry->uFirstChild;
										}
									else if(pEntry->uNextItem)
										{
										uItem = pEntry->uNextItem;
										}
									else{
										for(;;)
											{
												uItem  = pEntry->uParent;
												pEntry = pData ->pTreeItems[uItem];
											if(!pEntry)return 0;
											if( pEntry->uNextItem)
												{
												uItem=pEntry->uNextItem;
												break;
												}
											}
										}

										pEntry = pData->pTreeItems[uItem];
									if(!pEntry)break;

									return uItem;
									}

								return 0;


	case TVGN_PREVIOUSVISIBLE:	if(uItem>pData->uTreeItemsMax)
									{
									return 0;
									}

									pEntry = pData->pTreeItems[uItem];
								if(!pEntry)return 0;

								   uPos=pEntry->uShowPos-1;
								if(uPos<=pData->uScrollY)return 0;
								if(uPos> pData->uScrollY+pData->uPageEnties)return 0;

								return pData->pItemPos[uPos-1];

	case TVGN_LASTVISIBLE:		uPos=pData->uItemPosCount;
								if(uPos<=0)return 0;
								return pData->pItemPos[uPos-1];

	case TVGN_DROPHILITE:		return pData->uTrackedItem;

	case TVGN_DROPHILITESUB:	return pData->uTrackedSub;

	case TVGN_CARET:			return pData->uSelectedItem;

	case TVGN_CARETSUB:			return pData->uSelectedSub;

	case TVGN_FOCUS:			return (pData->uFocusItem)? pData->uFocusItem:pData->uSelectedItem;

	case TVGN_FOCUSSUB:			return (pData->uFocusItem)? pData->uFocusSub:pData->uSelectedSub;

	default:					uPos = uFlags&0xFFC00000;

								if(uPos==TVGN_CHILDEX(0))
									{
									if(uItem>pData->uTreeItemsMax)
										{
										if(uItem!=U(TVI_ROOT))break;
										uItem = pData->uFirstChild;
										}
									else{
											pEntry = pData->pTreeItems[uItem];
										if(!pEntry)break;

										uItem = pEntry->uFirstChild;
										}
									}
								else if(uPos==TVGN_NEXTEX(0))
									{
									if(uItem>pData->uTreeItemsMax)break;
										pEntry = pData->pTreeItems[uItem];
									if(!pEntry)break;
									}
								else{
									break;
									}

								for(uFlags&=0x003FFFFF;uFlags;uFlags--)
									{
									if(uItem>pData->uTreeItemsMax)
										{
										return 0;
										}

										pEntry = pData->pTreeItems[uItem];
									if(!pEntry)return 0;
									}

								return uItem;

		}


return 0;
}


//*****************************************************************************
//*
//*		TreeListFindItem
//*
//*****************************************************************************
//	Sucht einen Kindeintrag mit einem bestimmten Text
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Elterneintrages
//	pFind	: Die Struktur mit den Suchparametern
//	Ergibt die Nummer des Eintrages bzw. 0 wenn nicht gefunden wurde
static unsigned TreeListFindItem(TreeListData *pData,unsigned uItem,TVFIND *pFind)
{
int				iImage;
BaseItem	   *pEntry;
ExtraItem	   *pExtra;
LPCTSTR			pCmpText;
unsigned		uTextSize;
unsigned		uChkParam;
unsigned		uChkState;
unsigned		uChkShow;
unsigned		uChkText;
unsigned		uChkCase;
unsigned		uTextLen;
unsigned		uLevel;
unsigned		uMax;
unsigned		uSub;




	if(pFind->uFlags&TVIF_CHILDS)							// In den Kindern suchen
		{
		if(uItem>pData->uTreeItemsMax)
			{
			if(uItem!=U(TVI_ROOT))return 0;
			uItem = pData->uFirstChild;
			}
		else{
				pEntry = pData->pTreeItems[uItem];
			if(!pEntry)return 0;

			uItem = pEntry->uFirstChild;
			}
		}
	else{
		if(uItem>pData->uTreeItemsMax)						// Ist der Eintrag g?tig
			{
			if(uItem!=U(TVI_ROOT))return 0;
			if(!(pFind->uFlags&TVIF_RECURSIVE))return 0;
			if(  pFind->uFlags&TVIF_NEXT      )return 0;
			uItem = 0;
			}
		else if(pFind->uFlags&TVIF_NEXT)					// Beim n?hsten Eintrag weitersuchen
			{
				pEntry = pData->pTreeItems[uItem];
			if(!pEntry)return 0;

			uItem = pEntry->uNextItem;
			}
		}


	uChkParam = pFind->uFlags&TVIF_PARAM;
	uChkState = pFind->uFlags&TVIF_STATE;
	uChkText  = pFind->uFlags&TVIF_TEXT;
	uChkCase  = pFind->uFlags&TVIF_CASE;
	uSub	  = pFind->uColumn;
	uTextLen  = 0;


	if(uChkText)											// Textl?ge ermitteln
		{
		uTextLen = str_len(pFind->pText);
		}


	if(pFind->uFlags&TVIF_RECURSIVE)						// Auch in allen Kindern suchen
		{
		uChkShow = ((pFind->uFlags&TVIF_VISIBLE));
		uLevel	 = ((pFind->uFlags>>16)&0xFF);
		uMax	 = ((pFind->uFlags>>8 )&0xFF)-1;

		if(!uItem)											// Neustart bei 0
			{
			uItem		   = pData->uFirstChild;
			pFind->uFlags &= ~(TVIF_NEXT|TVIF_CHILDS|TVIF_RESTART|0x00FF0000);
			}
		else if(pFind->uFlags&TVIF_RESTART)					// Neustart der Suche
			{
				pEntry = pData->pTreeItems[uItem];
			if(!pEntry || (uChkShow && pEntry->uShowPos==0))
				{
				return 0;
				}

			   uLevel = pEntry->uLevel;
			if(uLevel>=uMax)return 0;

			pFind->uFlags &= ~(TVIF_NEXT|TVIF_CHILDS|TVIF_RESTART|0x00FF0000);
			pFind->uFlags |=   uLevel<<16;
			}
		else{												// Weitersuchen
				pEntry = pData->pTreeItems[uItem];
			if(!pEntry)return 0;

			goto Next;
			}
		}


	while(uItem)											// Durchlaufe alle Kinder
		{
			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;

		if(uChkParam && pFind->lParam!=pEntry->lParam)		// Vergleiche lParam
			{
			goto Next;
			}
															// Vergleiche die State-Bits
		if(uChkState && ((pEntry->uState^pFind->uState)&pFind->uStateMask))
			{
			goto Next;
			}

		if(uChkText==0)										// Text nicht vergleichen
			{
			break;
			}


		if(uSub)											// Text von Spalten
			{
			if(uSub>=pData->uColumnCount)
				{
				goto Next;
				}

				pExtra = pData->pExtraItems[uSub-1][uItem];
			if(!pExtra)
				{
				pCmpText=_T("");
				uTextSize = 0;
				}
			else{
					pCmpText=pExtra->pText;
				if(!pCmpText)pCmpText=_T("");
				uTextSize = pExtra->uTextSize;
				}
			}
		else{												// Text vom Haupteintrag
			if(pEntry->bCallback&TVIF_TEXT)
				{
				CallbackEntry(pData,pEntry,uItem,pEntry->bCallback,iImage,uTextSize,pCmpText);
				}
			else{
					pCmpText=pEntry->pText;
				if(!pCmpText)pCmpText=_T("");
				uTextSize = pEntry->uTextSize;
				}
			}


		if(uTextLen!=uTextSize)
			{
			goto Next;
			}

		if(uChkCase)										// Zwischen Gro?Kleinbuchstaben unterscheiden
			{
			if(!str_icmp(pCmpText,pFind->pText))break;
			}
		else{
			if(!memcmp(pCmpText,pFind->pText,uTextSize*sizeof(TCHAR)))break;
			}

Next:
		if(!(pFind->uFlags&TVIF_RECURSIVE))					// Nur in einer Ebene suchen
			{
			uItem = pEntry->uNextItem;
			continue;
			}
															// Rekursiv suchen
		if(pEntry->uFirstChild && pEntry->uLevel<uMax && (!uChkShow || (pEntry->uShowPos && (pEntry->uState&TVIS_EXPANDED))))
			{
			uItem = pEntry->uFirstChild;
			}
		else if(pEntry->uNextItem)
			{
			uItem = pEntry->uNextItem;
			}
		else for(;;)
			{
			if(uLevel>=pEntry->uLevel)return 0;

				uItem = pEntry->uParent;
			if(!uItem)return 0;

				pEntry = pData->pTreeItems[uItem];
			if(!pEntry)return 0;

				uItem = pEntry->uNextItem;
			if(!uItem)continue;

			break;
			}
		}



return uItem;
}


//*****************************************************************************
//*
//*		TreeListNextSelUntil
//*
//*****************************************************************************
//	Sucht rekursiv ausgew?lte Eintr?e
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages nach dem mit der Suche begonnen werden soll
//	uStop	: Bestimmt bei welchem Eintrag abgebrochen werden soll werden soll
//	Ergibt die Nummer des Eintrages oder 0 wenn keiner gefunden wurde
static unsigned TreeListNextSelUntil(TreeListData *pData,unsigned uItem,unsigned uStop)
{
BaseItem	   *pEntry;



	if(uItem>pData->uTreeItemsMax)
		{
		if(uItem!=U(TVI_ROOT))return 0;

		uItem = pData->uFirstChild;

			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		if( pEntry->uState&TVIS_SELECTED)return uItem;

		uStop = 0;
		}
	else{
			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		if(!pEntry->uFirstChild && uItem==uStop)return 0;
		}

	for(;;)
		{
		if(pEntry->uFirstChild)
			{
			   uItem= pEntry->uFirstChild;
			if(uItem==uStop)return 0;
			}
		else if(pEntry->uNextItem)
			{
			   uItem= pEntry->uNextItem;
			if(uItem==uStop)return 0;
			}
		else{
			for(;;)
				{
					uItem= pEntry->uParent;
				if( uItem==uStop)return 0;
					pEntry = pData ->pTreeItems[uItem];
				if(!pEntry)return 0;
				if( pEntry->uNextItem)
					{
					   uItem=pEntry->uNextItem;
					if(uItem==uStop)return 0;
					break;
					}
				}
			}

			pEntry = pData ->pTreeItems[uItem];
		if(!pEntry)break;
		if( pEntry->uState&TVIS_SELECTED)return uItem;
		}


return 0;
}

//*****************************************************************************
//*
//*		TreeListNextUnselUntil
//*
//*****************************************************************************
//	Sucht rekursiv nicht ausgew?lte Eintr?e
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages nach dem mit der Suche begonnen werden soll
//	uStop	: Bestimmt bei welchem Eintrag abgebrochen werden soll werden soll
//	Ergibt die Nummer des Eintrages oder 0 wenn keiner gefunden wurde
static unsigned TreeListNextUnselUntil(TreeListData *pData,unsigned uItem,unsigned uStop)
{
BaseItem	   *pEntry;



	if(uItem>pData->uTreeItemsMax)
		{
		if(uItem!=U(TVI_ROOT))return 0;

		uItem = pData->uFirstChild;

			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		if((pEntry->uState&TVIS_SELECTED)==0)return uItem;

		uStop = 0;
		}
	else{
			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		if(!pEntry->uFirstChild && uItem==uStop)return 0;
		}

	for(;;)
		{
		if(pEntry->uFirstChild)
			{
			   uItem= pEntry->uFirstChild;
			if(uItem==uStop)return 0;
			}
		else if(pEntry->uNextItem)
			{
			   uItem= pEntry->uNextItem;
			if(uItem==uStop)return 0;
			}
		else{
			for(;;)
				{
					uItem= pEntry->uParent;
				if( uItem==uStop)return 0;
					pEntry=pData->pTreeItems[uItem];
				if(!pEntry)return 0;
				if( pEntry->uNextItem)
					{
					   uItem=pEntry->uNextItem;
					if(uItem==uStop)return 0;
					break;
					}
				}
			}

			pEntry = pData->pTreeItems[uItem];
		if(!pEntry)break;
		if((pEntry->uState&TVIS_SELECTED)==0)return uItem;
		}


return 0;
}

//*****************************************************************************
//*
//*		TreeListChangeCheckbox
//*
//*****************************************************************************
//	Schaltet eine Checkboc um
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist der Eintrag der ge?dert werden soll
//	iPosX/Y	: Cursor Position f? Notify-Message
static void TreeListChangeCheckbox(TreeListData *pData,UINT uItem,int iPosX=0,int iPosY=0)
{
BaseItem   *pTemp;
BaseItem   *pEntry;
NMTREEVIEW	sNotify;
TV_ITEM		sItem;
UINT		uBits;



	sNotify.itemOld.mask  = 0;
	sNotify.itemOld.hItem = 0;

	pEntry = pData->pTreeItems[uItem];
	uBits  = pEntry->uState&TVIS_STATEIMAGEMASK;


	if(pData->uStyleEx&TVS_EX_SINGLECHECKBOX)				// Einzelauswahl
		{
		pTemp = pData->pTreeItems[pData->uSingleSel];

		if(pData->uSingleSel==uItem)
			{
			if(pData->uStyleEx&TVS_EX_BITCHECKBOX)
				{
				if(uBits&0x1000)return;
				}
			else{
				if(uBits==0x2000)return;
				}
			}
		else if(pData->uSingleSel && pTemp)					// Anderer Eintrag gew?lt
			{
			sItem.hItem		= (HTREEITEM)pData->uSingleSel;
			sItem.mask		= TVIF_STATE;
			sItem.stateMask	= TVIS_STATEIMAGEMASK;
			sItem.state		= (pData->uStyleEx&TVS_EX_BITCHECKBOX)?	0x0000:0x1000;

			TreeListSetItem(pData,&sItem);

			sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT;
			sNotify.itemOld.hItem		= (HTREEITEM)pData->uSingleSel;
			sNotify.itemOld.stateMask	= 0xFFFFFFFF;
			sNotify.itemOld.state		= pTemp->uState;
			sNotify.itemOld.lParam		= pTemp->lParam;
			sNotify.itemOld.pszText		= pTemp->pText;
			sNotify.itemOld.cchTextMax	= pTemp->uTextSize;
			sNotify.itemOld.cChildren	= 0;
			}
		}


	sItem.hItem		= (HTREEITEM)uItem;
	sItem.mask		= TVIF_STATE;
	sItem.stateMask	= TVIS_STATEIMAGEMASK;


	if(pData->uStyleEx&TVS_EX_BITCHECKBOX)
			sItem.state	= (uBits^0x1000);
	else	sItem.state	= (uBits&0x1000)? 0x2000:0x1000;

	TreeListSetItem(pData,&sItem);
	pData->uSingleSel = uItem;

	sNotify.hdr.code			= TVN_CBSTATECHANGED;
	sNotify.action				= VK_DBLCLK;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		= pEntry->uState;
	sNotify.itemNew.lParam		= pEntry->lParam;
	sNotify.itemNew.pszText		= pEntry->pText;
	sNotify.itemNew.cchTextMax	= pEntry->uTextSize;
	sNotify.itemNew.cChildren	= 0;
	sNotify.ptDrag.x			= iPosX;
	sNotify.ptDrag.y			= iPosY;

	UNLOCK(pData);
	SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

}

//*****************************************************************************
//*
//*		TreeListAutoScroll
//*
//*****************************************************************************
//	Startet das automatische Scrollen
//	pData	: Zeiger auf Fensterdaten
//	uMode	: Bestimmt den Scrollmodus
//				Bit0 = Y-Cursor darf nicht nich au?rhalb des Fensters sein
//				Bit1 = X-Cursor muss innerhalb des Fensres sein und der
//					   Y-Cursor kann auf der obersten oder untersten Reihe liegen
//	uTime	: Ist die Zeit f? den Timer (0=scrollen abschalten)
//	Ergibt TRUE wenn erfolgreich
static BOOL TreeListAutoScroll(TreeListData *pData,unsigned uMode,unsigned uTime)
{
BOOL	bRet;


	if(!uTime)												// Scrollen abschalten
		{
		return KillTimer(pData->hWnd,ID_AUTOSCROLL);
		}

	if(uTime< 30)uTime=30;
	if(uTime>900)uTime=900;

	pData->cScrollMode	= uMode;
	bRet = SetTimer(pData->hWnd,ID_AUTOSCROLL,uTime,NULL);
	if(bRet)pData->bScrollTicker=0;


return bRet;
}


//*****************************************************************************
//*
//*		TreeListMouseNotify
//*
//*****************************************************************************
//	Soll f? einen Mausklick eine
//	pData	: Zeiger auf die Fensterdaten
//	uMsg	: Message des Mausklicks
//	wParam	: WPARAM des Mausklicks
//	lParam	: LPARAM des Mausklicks
static void TreeListMouseNotify(TreeListData *pData,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
TV_HITTESTINFO	sInfo;
NMTREEVIEW		sNotify;
BaseItem	   *pEntry;
unsigned		uItem;


	sInfo.flags = (UINT) wParam;
	sInfo.pt.x	= GET_X_LPARAM(lParam);
	sInfo.pt.y	= GET_Y_LPARAM(lParam);
	uItem		= TreeListHitTest(pData,&sInfo);

	if(uItem)
		{
		pEntry						= pData->pTreeItems[uItem];
		sNotify.itemNew.stateMask	= 0xFFFFFFFF;
		sNotify.itemNew.state		= pEntry->uState;
		sNotify.itemNew.lParam		= pEntry->lParam;
		sNotify.itemNew.cChildren	= TVHT_SUBTOCOL(sInfo.flags);
		}
	else{
		sNotify.itemNew.stateMask	= 0;
		sNotify.itemNew.state		= 0;
		sNotify.itemNew.lParam		= 0;
		}

	sNotify.action					= 0;
	sNotify.hdr.code				= uMsg;
	sNotify.itemNew.mask			= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBNUMBER;
	sNotify.itemNew.hItem			= (HTREEITEM)uItem;
	sNotify.itemNew.pszText			= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax		= -1;
	sNotify.itemOld.mask			= 0;
	sNotify.ptDrag.x				= GET_X_LPARAM(lParam);
	sNotify.ptDrag.y				= GET_Y_LPARAM(lParam);

	SendNotify(pData,&sNotify.hdr);

}

//*****************************************************************************
//*
//*		TreeListMouseClick
//*
//*****************************************************************************
//	Soll ein Mausklick ausgef?rt werden. ACHTUNG UNLOCK wird ausgef?rt
//	pData	: Zeiger auf die Fensterdaten
//	uMsg	: Message des Mausklicks
//	wParam	: WPARAM des Mausklicks
//	lParam	: LPARAM des Mausklicks
static void TreeListMouseClick(TreeListData *pData,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
ExtraItem      *pExtra;
BaseItem	   *pEntry;
BaseItem	   *pTemp;
TV_HITTESTINFO	sInfo;
NMTREEVIEW		sNotify;
unsigned		uOldSub;
unsigned		uOldItem;
unsigned		uMaskItem;
unsigned		uMaskSub;
unsigned		uMsgOld;
unsigned		uToggle;
unsigned		uItem;
unsigned		uLine;
unsigned		uTemp;
unsigned		uStop;
unsigned		uSub;
unsigned		uSel;
unsigned		uPos;
unsigned		uNum;
int				iMode;
int				iAdd;



	if(!pData->cIsEnabled)									// Ist das Fenster freigegeben
		{
		return;
		}

	if(!pData->cHasFocus)									// Hat das Fenster den Focus
		{
		if(GetFocus()!=pData->hWnd)
			{
			UNLOCK(pData);
			SetFocus(pData->hWnd);
			LOCK(pData);

			if(GetFocus()!=pData->hWnd)
				{
				UNLOCK(pData);
				return;
				}
			}
		else{
			pData->cHasFocus = 1;
			}
		}

	sInfo.flags = (UINT) wParam;
	sInfo.pt.x	= GET_X_LPARAM(lParam);
	sInfo.pt.y	= GET_Y_LPARAM(lParam);
	uItem		= TreeListHitTest(pData,&sInfo);
	uMsgOld		= uMsg;
	uToggle		= 0;


	if(uItem)												// Wurde auf einen Eintrag getr?kt
		{
		pEntry = pData->pTreeItems[uItem];

		if(pData->uStyle&TVS_FULLROWSELECT)
			{
			uMaskItem = TVHT_ONITEMICON|TVHT_ONITEMLABEL|TVHT_ONITEMBASE|TVHT_ONITEMSTATEICON|TVHT_ONITEMRIGHT;
			uMaskSub  =	TVHT_ONSUBICON |TVHT_ONSUBLABEL |TVHT_ONSUBBASE |TVHT_ONSUBRIGHT;
			}
		else{
			uMaskItem = TVHT_ONITEMICON|TVHT_ONITEMLABEL|TVHT_ONITEMBASE|TVHT_ONITEMSTATEICON;
			uMaskSub  =	TVHT_ONSUBICON |TVHT_ONSUBLABEL |TVHT_ONSUBBASE;
			}


		if(sInfo.flags&TVHT_ONITEMBUTTON)					// Eintrag aufklappen
			{
			if(uMsg==WM_LBUTTONDOWN || uMsg==WM_LBUTTONDBLCLK)
				{
				   pEntry = pData->pTreeItems[uItem];
				if(pEntry && (pEntry->uState&(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))==(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))
					{
					TreeListToggleItem(pData,uItem,0);		// Von + auf - umschalten
					}
				else{
					uToggle = 1;							// Eintrag auflappen
					}
				}
			}
		else if(sInfo.flags&TVHT_ONITEMSTATEICON)			// Checkbox umschalten
			{
			if(pData->uStyle&TVS_CHECKBOXES)
			if(uMsg==WM_LBUTTONDOWN || uMsg==WM_LBUTTONDBLCLK)
				{
				pData->uCheckedDown = 1;
				TreeListChangeCheckbox(pData,uItem,sInfo.pt.x,sInfo.pt.y);
				}
			}
		else if(sInfo.flags&uMaskItem)						// Eintrag ausw?len
			{
			if(!(pData->uStyle&TVS_DISABLEDRAGDROP))
				{
				if(uMsg==WM_LBUTTONDOWN)
					{
					pData->uDragFlags = MK_LBUTTON;
					pData->uDragItem  = uItem;
					pData->uDragSub   = 0;
					}

				if(uMsg==WM_RBUTTONDOWN)
					{
					pData->uDragFlags = MK_RBUTTON;
					pData->uDragItem  = uItem;
					pData->uDragSub   = 0;
					}
				}

			if(pData->uStyleEx&TVS_EX_MULTISELECT)			// Mehrfachauswahl erlaubt
				{
				if(uMsg==WM_RBUTTONDOWN)					// Keine Abwahl wenn mehrer Eintr?e aus gew?lt
				if(pData->uSelectedCount>1)
					{
					   pEntry=pData->pTreeItems[uItem];
					if(pEntry->uState&TVIS_SELECTED)
						{
						goto End;
						}
					}

				if(uMsg==WM_LBUTTONDOWN)					// Spezialsteuerung f? Multiselect bei Darg
				if(!(wParam&(MK_CONTROL|MK_SHIFT)))
				if(pData->uSelectedCount>0)
					{
					   pEntry =   pData->pTreeItems[uItem];
					if(pEntry && (pEntry->uState&TVIS_SELECTED))
						{
						pData->cClickFlag = 1;
						pData->cClickEdit = 0;
						goto End;
						}
					}

				if(uMsg==WM_LBUTTONUP && pData->cClickFlag)
					{
					wParam &= ~(MK_CONTROL|MK_SHIFT);
					uMsg    =   WM_LBUTTONDOWN;
					}


				if(wParam&MK_SHIFT)							// Bis zum angeklicken Ausw?len
					{
					if(uMsg!=WM_LBUTTONDOWN)
					if(uMsg!=WM_LBUTTONDBLCLK)goto End;

					pData->cClickEdit = 0;

						uTemp = pData->uSelectedItem;
					if(!uTemp)
						{
						    uTemp = pData->uFocusItem;
						if(!uTemp || !pData->cReSelect)goto End;
						}

						pEntry = pData->pTreeItems[uTemp];
						uLine  = pEntry->uShowPos;
					if(!uLine)goto End;

						pEntry = pData->pTreeItems[uItem];
						uStop  = pEntry->uShowPos;
					if(!uStop)goto End;

					if(pData->uSelectedCount<=1)
						{
						pData->uSelectedBase = uTemp;
						}

														// Shift-Select neu ausw?len
					if(pData->cReSelect && pData->uSelectedBase)
						{
						TreeListSelectItem   (pData,uItem,0,TVC_BYMOUSE|TVC_DESELECT);
						TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);

						uTemp = pData->uSelectedBase;
						pTemp = pData->pTreeItems[uTemp];
						uStop = uItem;


						while(pTemp && pTemp->uShowPos==0)
							{
								pTemp = pData->pTreeItems[pTemp->uParent];
							if(!pTemp)break;
							}

						if(pTemp)
							{
							if(!(pTemp->uState&TVIS_SELECTED))
								{
								TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
								}

							if(pTemp->uShowPos<pEntry->uShowPos)
								{
								uStop = uItem;
								}
							else{
								uStop = uTemp;
								uTemp = uItem;
								}

							for(;;)
								{
									uTemp=TreeListNextUnselUntil(pData,uTemp,uStop);
								if(!uTemp)break;
								TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
								}
							}


						pData->cReSelect = 0;
						goto End;
						}


					TreeListSelectItem(pData,uItem,0,TVC_BYMOUSE);

					iAdd = (uLine>uStop)? -1:1;
					uPos =  uLine;

					if(uPos!=uStop)
						{
						pEntry = pData->pTreeItems[pData->uSelectedBase];

						while(pEntry && pEntry->uShowPos==0)
							{
							pEntry = pData->pTreeItems[pEntry->uParent];
							}

						if(pEntry)
							{
							if(iAdd<0)
								{if(pEntry->uShowPos>uPos && uPos>uStop)uPos--;}
							else{if(pEntry->uShowPos<uPos && uPos<uStop)uPos++;}
							}
						}



					for(;;uPos+=iAdd)						// Eintr?e w?len
						{
						uTemp  = pData->pItemPos  [uPos-1];
						pEntry = pData->pTreeItems[uTemp ];
						uSel   = pEntry->uState&TVIS_SELECTED;

						if(uTemp!=pData->uSelectedBase || !uSel)
							{
							TreeListXorSelectItem(pData,uTemp,TVC_BYMOUSE);
							uSel = pEntry->uState&TVIS_SELECTED;
							}

															// Auch unsichtbare Kinder w?len
						   pTemp =   pData->pTreeItems[pEntry->uFirstChild];
						if(pTemp && !pTemp->uShowPos)
							{
							if(uSel)
								{
								for(uNum=uTemp;;)			// Kinder ausw?len
									{
										uNum=TreeListNextUnselUntil(pData,uNum,uTemp);
									if(!uNum)break;
									TreeListXorSelectItem(pData,uNum,TVC_BYMOUSE);
									}
								}
							else{
								for(uNum=uTemp;;)			// Kinder abw?len
									{
										uNum=TreeListNextSelUntil(pData,uNum,uTemp);
									if(!uNum)break;
									TreeListXorSelectItem(pData,uNum,TVC_BYMOUSE);
									}
								}
							}

						if(uPos==uStop)break;
						}


					TreeListRemoveFocus(pData);

					pEntry = pData->pTreeItems[uItem];
					uSub   = TVHT_SUBTOCOL(sInfo.flags);
					if(!(pData->uStyleEx&TVS_EX_SUBSELECT))uSub=0;

					pData->uFocusSub     = uSub;
					pData->uFocusItem    = uItem;

					if(uSub)
						{
						   pExtra =pData->pExtraItems[uSub-1][uItem];
						if(pExtra)pExtra->bFlags |= TVIX_FOCUSED;
						}
					else{
						pEntry->bFlags |= TVIX_FOCUSED;
						}

					UpdateRect(pData,uItem,uSub);

					TreeListSelectItem   (pData,uItem,0,TVC_BYMOUSE);
					TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);

					goto End;
					}


				if(wParam&MK_CONTROL)						// Einzelen Eintrag umschalten
					{
					pData->uSelectedBase = uItem;
					pData->cReSelect	 = 1;
					pData->cClickEdit	 = 0;


					   pEntry = pData->pTreeItems[uItem];
					if(pEntry && (pEntry->uState&TVIS_SELECTED))
						{
						if(uMsg!=WM_LBUTTONDOWN)
						if(uMsg!=WM_LBUTTONDBLCLK)goto End;

						TreeListSelectItem(pData,0,0,TVC_BYMOUSE);

						if(TreeListXorSelectItem(pData,uItem,TVC_BYMOUSE))
							{								// Auch unsichtbare Kinder abw?len
								pTemp =   pData->pTreeItems[pEntry->uFirstChild];
							if( pTemp && !pTemp->uShowPos)
							for(uTemp=uItem;;)
								{
									uTemp=TreeListNextSelUntil(pData,uTemp,uItem);
								if(!uTemp)break;
								TreeListXorSelectItem(pData,uTemp,TVC_BYMOUSE);
								}
							}

						   uTemp = pData->uFocusItem;
						if(uTemp)							// Ist ein Focus definiert
							{
							TreeListRemoveFocus(pData);
							}

						uSub = TVHT_SUBTOCOL(sInfo.flags);
						if(!(pData->uStyleEx&TVS_EX_SUBSELECT))uSub=0;

						pData->uFocusItem = uItem;
						pData->uFocusSub  = uSub;

						if(uSub)
							{
							   pExtra =pData->pExtraItems[uSub-1][uItem];
							if(pExtra)pExtra->bFlags |= TVIX_FOCUSED;
							}
						else{
							pEntry->bFlags |= TVIX_FOCUSED;
							}

						UpdateRect(pData,uItem,uSub);

						goto End;
						}
					}
				}

			if(wParam&MK_CONTROL)							// Ist die Ctrl-Taste gedr?kt
				{
				iMode = TVC_BYMOUSE;
				}
			else{
				iMode = TVC_BYMOUSE|TVC_DESELECT;
				}

			uOldSub	  = pData->uSelectedSub;
			uOldItem  = pData->uSelectedItem;

			TreeListRemoveFocus  (pData);
			TreeListSelectItem   (pData,uItem,0,iMode);
			TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			pEntry = pData->pTreeItems[uItem];

			if(uMsg==WM_LBUTTONDBLCLK)
				{											// Auto-Edit mit Doppelklick
				if(pData->aColumn[0].bFlags&TVAE_DBLCLICK && pData->aColumn[0].bEdit)
					{
					TreeListStartAutoEdit(pData,0,VK_DBLCLK,U(lParam));
					}
				else if(TreeListStartNotifyEdit(pData,pData->uSelectedItem,0,VK_DBLCLK,U(lParam)))
					{

					}
				else if(pEntry && (pEntry->uState&(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))==(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))
					{
					TreeListToggleItem(pData,uItem,0);		// Von + auf - umschalten
					}
				else{
					uToggle = 1;							// Eintrag auflappen
					}
				}
			else if(uMsgOld==WM_LBUTTONDOWN)
				{
				if(pData->uStyleEx&TVS_EX_EDITCLICK)		// Editieren direkt mit einem Mausklick
				if(!(wParam&(MK_CONTROL|MK_SHIFT)))
				if((pData->cEditClickMode&2) || (uOldSub==pData->uSelectedSub && uOldItem==pData->uSelectedItem))
					{
					if(pData->cEditClickMode&1)
						{
						if(sInfo.flags&TVHT_ONITEMBASE)pData->cClickEdit=1;
						}
					else{
						if(sInfo.flags&uMaskItem&~TVHT_ONITEMICON)pData->cClickEdit=1;
						}
					}
				}
			else if(uMsgOld==WM_LBUTTONUP)
				{
				if(pData->uStyleEx&TVS_EX_EDITCLICK)
				if(pData->cClickFlag && !pData->cClickEdit)
				if(!(wParam&(MK_CONTROL|MK_SHIFT)))
				if((pData->cEditClickMode&2) || (uOldSub==pData->uSelectedSub && uOldItem==pData->uSelectedItem))
					{
					if(pData->cEditClickMode&1)
						{
						if(sInfo.flags&TVHT_ONITEMBASE)pData->cClickEdit=1;
						}
					else{
						if(sInfo.flags&uMaskItem&~TVHT_ONITEMICON)pData->cClickEdit=1;
						}
					}

				if(pData->cClickEdit)
					{
					pData->cClickEdit = 0;

					if(pData->aColumn[0].bEdit)
						{
						TreeListStartAutoEdit(pData,0,VK_EDITCLK,U(lParam));
						}
					else{
						TreeListStartNotifyEdit(pData,pData->uSelectedItem,0,VK_EDITCLK,U(lParam));
						}
					}
				}
			}
		else if(sInfo.flags&uMaskSub)						// Extra-Eintrag ausw?len
			{
			if(pData->uStyleEx&TVS_EX_SUBSELECT)
					uSub = TVHT_SUBTOCOL(sInfo.flags);
			else	uSub = 0;

			if(!(pData->uStyle&TVS_DISABLEDRAGDROP))
				{
				if(uMsg==WM_LBUTTONDOWN)
					{
					pData->uDragFlags = MK_LBUTTON;
					pData->uDragItem  = uItem;
					pData->uDragSub   = uSub;
					}

				if(uMsg==WM_RBUTTONDOWN)
					{
					pData->uDragFlags = MK_RBUTTON;
					pData->uDragItem  = uItem;
					pData->uDragSub   = uSub;
					}
				}

			if(wParam&MK_CONTROL)
				{
				if(pData->uStyleEx&TVS_EX_MULTISELECT)		// Auswahl umschalten
					{
					   pEntry = pData->pTreeItems[uItem];
					if(pEntry && (pEntry->uState&TVIS_SELECTED))
						{
						if(uMsg!=WM_LBUTTONDOWN)
						if(uMsg!=WM_LBUTTONDBLCLK)goto End;

						TreeListSelectItem(pData,0,0,TVC_BYMOUSE);

						if(TreeListXorSelectItem(pData,uItem,TVC_BYMOUSE))
							{								// Auch unsichtbare Kinder abw?len
								pTemp =   pData->pTreeItems[pEntry->uFirstChild];
							if( pTemp && !pTemp->uShowPos)
							for(uTemp=uItem;;)
								{
									uTemp=TreeListNextSelUntil(pData,uTemp,uItem);
								if(!uTemp)break;
								TreeListXorSelectItem(pData,uTemp,TVC_BYMOUSE);
								}
							}

						TreeListRemoveFocus(pData);

						pData->cClickEdit = 0;
						pData->uFocusItem = uItem;
						pData->uFocusSub  = uSub;

						if(uSub)
							{
							   pExtra =pData->pExtraItems[uSub-1][uItem];
							if(pExtra)pExtra->bFlags |= TVIX_FOCUSED;
							}
						else{
							pEntry->bFlags |= TVIX_FOCUSED;
							}

						UpdateRect(pData,uItem,uSub);

						goto End;
						}
					}

				iMode = TVC_BYMOUSE;
				}
			else{
				iMode = TVC_BYMOUSE|TVC_DESELECT;
				}

			uOldSub	  = pData->uSelectedSub;
			uOldItem  = pData->uSelectedItem;

			TreeListRemoveFocus  (pData);
			TreeListSelectItem   (pData,uItem,uSub,(wParam&MK_CONTROL)? TVC_BYMOUSE:TVC_BYMOUSE|TVC_DESELECT);
			TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			pEntry = pData->pTreeItems[uItem];

			if(uMsg==WM_LBUTTONDBLCLK)
				{
				if( pData->aColumn[uSub].bEdit &&			// Auto-Edit mit Doppelklick
				  ((pData->aColumn[uSub].bFlags&TVAE_DBLCLICK ) ||
				  ((pData->aColumn[uSub].bFlags&TVAE_ICONCLICK) && (sInfo.flags&TVHT_ONSUBICON))))
					{
					TreeListStartAutoEdit(pData,uSub,(sInfo.flags&TVHT_ONSUBICON)? VK_ICONCLK:VK_DBLCLK,U(lParam));
					}
				else if(TreeListStartNotifyEdit(pData,uItem,uSub,VK_DBLCLK,U(lParam)))
					{

					}
				else{
					uToggle = 1;							// Eintrag auflappen
					}
				}
			else if(uMsg==WM_LBUTTONDOWN)
				{
				if(pData->aColumn[uSub].bEdit)				// Auto-Edit mit Klick auf Icon
				if(pData->aColumn[uSub].bFlags&TVAE_ICONCLICK)
				if(sInfo.flags&TVHT_ONSUBICON)
					{
					TreeListStartAutoEdit(pData,uSub,VK_ICONCLK,U(lParam));
					}

				if(pData->uStyleEx&TVS_EX_EDITCLICK)		// Editieren direkt mit einem Mausklick
				if(uSub && !(wParam&(MK_CONTROL|MK_SHIFT)))
				if((pData->cEditClickMode&2) || (uOldSub==pData->uSelectedSub && uOldItem==pData->uSelectedItem))
					{
					if(pData->cEditClickMode&1)
						{
						if(sInfo.flags&TVHT_ONSUBBASE)pData->cClickEdit=1;
						}
					else{
						if(sInfo.flags&uMaskSub&~TVHT_ONSUBICON)pData->cClickEdit=1;
						}
					}
				}
			else if(uMsg==WM_LBUTTONUP)
				{
				if(pData->cClickEdit)
					{
					pData->cClickEdit = 0;

					if(pData->aColumn[uSub].bEdit)
						{
						TreeListStartAutoEdit(pData,uSub,VK_EDITCLK,U(lParam));
						}
					else{
						TreeListStartNotifyEdit(pData,uItem,uSub,VK_EDITCLK,U(lParam));
						}
					}
				}
			}
		}
	else{
		pEntry = NULL;
		uSub   = 0;
		}

End:


	switch(uMsgOld)
		{
	case WM_LBUTTONUP:		sNotify.hdr.code = TVN_LBUTTONUP;	break;
	case WM_LBUTTONDOWN:	sNotify.hdr.code = NM_CLICK;		break;
	case WM_LBUTTONDBLCLK:	sNotify.hdr.code = NM_DBLCLK;		break;
	case WM_RBUTTONUP:		sNotify.hdr.code = TVN_RBUTTONUP;	break;
	case WM_RBUTTONDOWN:	sNotify.hdr.code = NM_RCLICK;		break;
	case WM_RBUTTONDBLCLK:	sNotify.hdr.code = NM_RDBLCLK;		break;
	default:				UNLOCK(pData);						return;
		}


	if(pEntry)
		{
		sNotify.itemNew.stateMask	= 0xFFFFFFFF;
		sNotify.itemNew.state		= pEntry->uState;
		sNotify.itemNew.lParam		= pEntry->lParam;
		sNotify.itemNew.cChildren	= TVHT_SUBTOCOL(sInfo.flags);
		}
	else{
		sNotify.itemNew.stateMask	= 0;
		sNotify.itemNew.state		= 0;
		sNotify.itemNew.lParam		= 0;
		}

	sNotify.action					= 0;
	sNotify.itemNew.mask			= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBNUMBER;
	sNotify.itemNew.hItem			= (HTREEITEM)uItem;
	sNotify.itemNew.pszText			= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax		= -1;
	sNotify.itemOld.mask			= 0;
	sNotify.ptDrag.x				= GET_X_LPARAM(lParam);
	sNotify.ptDrag.y				= GET_Y_LPARAM(lParam);


	UNLOCK(pData);

	if(!SendNotify(pData,&sNotify.hdr))
		{
		if(uToggle)											// Aufklappen nur wenn Notify nicht abgefangen
			{
			LOCK(pData);
			TreeListToggleItem(pData,uItem,0);
			UNLOCK(pData);
			}
		}

}

//*****************************************************************************
//*
//*		TreeListChar
//*
//*****************************************************************************
//	Eingabe von Buchstaben ?er die WM_CHAR Nachricht
//	pData	: Zeiger auf die Fensterdaten
//	nChar	: Ist das Zeichen das eingegeben wurde
static void TreeListChar(TreeListData *pData,UINT nChar,LPARAM lParam)
{
LPTSTR			pName;
ExtraItem	   *pExtra;
BaseItem	   *pEntry;
unsigned		uDelta;
unsigned		uItem;
unsigned 		uTick;
unsigned		uSub;
unsigned		uPos;
unsigned		uVal;
int				iNum;
int				iMax;
int				i;



	if(nChar>=' ')
		{
		if(pData->cKeyIgnore)return;						// Taste soll ignoriert werden

		iMax	= pData->uItemPosCount;
		uSub	= pData->uSelectedSub;
		i		= 0;

		if(TVIS_EDIT(pData->aColumn[uSub].bEdit))			// Bei AutoEdit keine Auswahl
		if(!(pData->aColumn[uSub].bFlags&TVAE_ONLYRETURN))
			{
			TreeListStartAutoEdit(pData,uSub,nChar,0);
			return;
			}

		if(TreeListStartNotifyEdit(pData,pData->uSelectedItem,uSub,nChar|VK_ISACHAR,U(lParam)))
			{
			return;
			}

		if(iMax<=0)return;


		   iNum = pData->uSelectedItem;						// Hole die Startzeile
		if(iNum>0)
			{
			   iNum = pData->pTreeItems[iNum]->uShowPos-1;
			if(iNum<0)iNum=0;
			}


		if((nChar>='a' && nChar<='z') ||					// In Grosbuchstaben umwandeln
		   (nChar>=224 && nChar<=254))
			{
			nChar-='a'-'A';
			}

		if(!(pData->uStyleEx&TVS_EX_NOCHARSELCET))
		for(;;) 											// Suche Anfangsbuchstaben
			{
			uTick  = GetTickCount();
			uDelta = (uKeyPos>0)? 750:500;

			if(uKeyPos>=3           )uDelta=1000;
			if(uTick-uKeyLast>uDelta)uKeyPos=0;
			if(uKeyPos>=16          )uKeyPos=0;
			if(uKeyPos==1 && cKeyData[0]==(TCHAR)nChar)
				{
				uKeyPos=0;
				}

			cKeyData[uKeyPos] = (TCHAR)nChar;
			uKeyLast          = uTick;
			uKeyPos          += 1;


			if(uKeyPos>1)
				{
				iNum--;
				if(iNum<0)iNum=iMax-1;
				}

			if(iMax<=0)break;

			for(i=iNum+1;i!=iNum;i++)					// Suche ?ereinstimmung
				{
				if(i>=iMax){i=-1;continue;}
				uItem=pData->pItemPos[i];

				if(uSub)
					{
					pExtra =  pExtra = pData->pExtraItems[uSub-1][uItem];
					pName  = (pExtra && pExtra->pText)? pExtra->pText:_T("");
					}
				else{
					pEntry =  pData->pTreeItems[uItem];
					pName  = (pEntry && pEntry->pText)? pEntry->pText:_T("");
					}


				for(uPos=0;uPos<uKeyPos;uPos++)			// Vergleiche die Texte
					{
					#if UNICODE
					uVal = pName[uPos];
					#else
					uVal = ((unsigned char*)pName)[uPos];
					#endif

					if((uVal>='a' && uVal<='z') ||		// In Grosbuchstaben umwandeln
					   (uVal>=224 && uVal<=254))
						{
						uVal-='a'-'A';
						}

					if(cKeyData[uPos]!=(TCHAR)uVal)
						{
						break;
						}
					}

				if(uPos<uKeyPos)continue;

				if(TreeListSelectItem(pData,uItem,uSub,TVC_UNKNOWN|TVC_DESELECT))
					{
					TreeListEnsureVisible(pData,uItem,uSub);
					}

				return;
				}

			if(i!=iNum   )break;
			if(uKeyPos<=1)break;

			iNum++;
			if(iNum>=iMax)iNum=iMax-1;

			uKeyPos=0;
			}
		}


}

//*****************************************************************************
//*
//*		TreeListKeyDown
//*
//*****************************************************************************
//	Soll ein Tastentruck ausgef?rt werden. ACHTUNG UNLOCK wird ausgef?rt
//	pData	: Zeiger auf die Fensterdaten
//	wParam	: WPARAM des Mausklicks
//	lParam	: LPARAM des Mausklicks
static void TreeListKeyDown(TreeListData *pData,WPARAM wParam,LPARAM lParam)
{
BaseItem	   *pTemp;
BaseItem	   *pEntry;
TV_KEYDOWN_EX	sNotify;
unsigned		uTemp;
unsigned		uItem;
unsigned		uStop;
unsigned		uVal;
int				iLineCmp;
int				iOldLine;
int				iOldCol;
int				iFocus;
int				iShift;
int				iBase;
int				iLine;
int				iAdd;
int				iPos;
int				iSub;
int				iCol;
int				iMax;
int				iDel;
int				iScr;
int				iSel;


	if(!pData->cIsEnabled)									// Ist das Fenster freigegeben
		{
		return;
		}

	if(!pData->cHasFocus)									// Hat das Fenster den Focus
		{
		if(GetFocus()!=pData->hWnd)
			{
			UNLOCK(pData);
			SetFocus(pData->hWnd);
			LOCK(pData);

			if(GetFocus()!=pData->hWnd)
				{
				UNLOCK(pData);
				return;
				}
			}
		else{
			pData->cHasFocus = 1;
			}
		}

	if(wParam==VK_RETURN)									// Wurde Enter gedr?kt
		{
		sNotify.hdr.code	=  NM_RETURN;
		sNotify.wVKey		= (WORD)(wParam);
		sNotify.wScan		= (WORD)(wParam>>16);
		sNotify.flags		= (UINT)(lParam);

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);
		LOCK(pData);

		iSub = pData->uSelectedSub;

		if(pData->aColumn[iSub].bEdit)						// Auto-Edit starten
			{
			if(GetAsyncKeyState(VK_CONTROL)&0x8000)
				{
				wParam = VK_ICONCLK;
				lParam = 0;
				}

			TreeListStartAutoEdit(pData,iSub,U(wParam),U(lParam));
			}
		else{
			TreeListStartNotifyEdit(pData,pData->uSelectedItem,iSub,U(wParam),U(lParam));
			}

		UNLOCK(pData);
		return;
		}


	if(pData->uMoveItem)									// Ist Auto-Move aktif
		{
		if(wParam==VK_ESCAPE)
			{
			TreeListSetMarkLine(pData,0,0);
			TreeListAutoScroll (pData,0,0);
			SetCursor(hCursorArrow);
			pData->uMoveItem = 0;
			ReleaseCapture();
			UNLOCK(pData);
			return;
			}

		if(wParam==VK_CONTROL && pData->uLineHeight<5)
			{
			TreeListHandleDragMove(pData,pData->uMoveLastW|MK_CONTROL,pData->uMoveLastL);
			}
		}

	pEntry = pData->pTreeItems[pData->uSelectedItem];
	iDel   = (GetAsyncKeyState(VK_SHIFT  )&0x8000)? 0:TVC_DESELECT;
	iScr   =  GetAsyncKeyState(VK_CONTROL)&0x8000;


	if(iDel && iScr)										// Ende oder Anfang w?len
	if(wParam==VK_HOME || wParam==VK_END)
	if(pData->uStyleEx&TVS_EX_HOMEENDSELECT)
		{
		iDel = TVC_DESELECT;
		iScr = 0;
		}


	if(iDel && iScr)										// Das Fenster scrollen
		{
		iLine = pData->uScrollY;
		iSub  = pData->uScrollX;

		switch(wParam)
			{
		case VK_END:	iLine  = pData->uTreeItemsCount;	// Ans Ende scrollen
						iLine -= pData->uPageEnties;
						break;

		case VK_HOME:	iLine=0;							// Zum Anfang scrollen
						break;

		case VK_LEFT:	iSub-=16;							// Links scrollen
						break;

		case VK_RIGHT:	iSub+=16;							// Rechts scrollen
						break;

		case VK_UP:		iLine--;							// Nach oben scrollen
						break;

		case VK_DOWN:	iLine++;							// Nach unten scrollen
						if(pData->uFixedLines^iLine)break;
						iLine=pData->uScrollY+pData->uFixedLines;
						break;

		case VK_PRIOR:	iLine-=pData->uSizeX;				// Eine Seite nach oben
						break;

		case VK_NEXT:	iLine+=pData->uSizeX;				// Eine Seite nach unten
						break;

		case VK_SPACE:	if(pEntry==NULL)break;				// Expantieren und schlie?n
						if(pEntry->bFlags&TVIX_HASBUTTON)
							{
							TreeListToggleItem(pData,pData->uSelectedItem,0);
							}

						break;

		default:		goto Exit;
			}

		if(iLine!=(int)pData->uScrollY)
			{
			iMax  = pData->uItemPosCount;
			iMax -=	pData->uPageEnties-1;

			if(iLine>=iMax)iLine=iMax;
			if(iLine<0)iLine=0;
			if(iLine!=(int)pData->uScrollY)
			if(!(pData->uStyle&TVS_NOSCROLL))
				{
				pData->uScrollY = iLine;
				SetScrollPos(pData->hWnd,SB_VERT,iLine,TRUE);
				UpdateView(pData);
				}
			}

		if(iSub!=(int)pData->uScrollX)
			{
			   uVal = pData->uColumnCount;
			if(uVal)iMax  = pData->aColumnXpos[uVal]-pData->uSizeX/2;
			else	iMax  = pData->iMaxSizeX;
					iMax -=	pData->uSizeX-pData->uSizeX/2;

			if(iSub>=iMax)iSub=iMax;
			if(iSub<0)iSub=0;
			if(iSub!=(int)pData->uScrollX)
			if(!(pData->uStyle&TVS_NOSCROLL))
				{
				pData->uScrollX = iSub;
				SetScrollPos(pData->hWnd,SB_HORZ,iSub,TRUE);
				UpdateView(pData);

				if(pData->hHeader)
					{
					MoveWindow(pData->hHeader,-iSub,0,pData->uSizeX+iSub,pData->uStartPixel,TRUE);
					}
				}
			}
		}
	else{													// Einen anderen Eintrag ausw?len
		iSub  = pData->uSelectedSub;
		iCol  = pData->aColumn[iSub].bIndex;

		if(!pEntry)
			{
			iLine    =  0;
			iCol     =  0;
			iOldCol	 =  0;
			iOldLine = -1;

			   uTemp = pData->uFocusItem;
			if(uTemp)										// Ist ein Focus definiert
				{
				pTemp = pData->pTreeItems[uTemp];

				while(pTemp && !pTemp->uShowPos)
					{
					pTemp = pData->pTreeItems[pTemp->uParent];
					}

				if(pTemp)
					{
					iLine = pTemp->uShowPos-1;
					iSub  = pData->uFocusSub;
					iCol  = pData->aColumn[iSub].bIndex;
					}


				if(pData->uSelectedCount>1)
					{
					iOldCol	 = iCol;
					iOldLine = iLine;
					}
				else{
					TreeListRemoveFocus(pData);
					}
				}
			}
		else{
			   iLine =  pEntry->uShowPos-1;
			if(iLine<0)iLine=0;

			uTemp	 =  pData->uSelectedItem;
			iOldLine =  iLine;
			iOldCol	 =  iCol;
			}


		switch(wParam)
			{
		case VK_END:	iLine=pData->uItemPosCount-1;	break;
		case VK_HOME:	iLine=0;						break;
		case VK_LEFT:	if(pData->uColumnCount>1 && (pData->uStyleEx&TVS_EX_SUBSELECT))
							{
							while(iCol>0)
								{
								iCol--;
								iSub = pData->aColumnPos[iCol];
								if(pData->aColumn[iSub].sReal>0)break;
								}
							}
						else{
							if(! pEntry)break;
							if(!(pEntry->uState&TVIS_EXPANDED) || !pEntry->uFirstChild)
								{
								   pTemp = pData->pTreeItems[pEntry->uParent];
								if(pTemp)iLine=pTemp->uShowPos-1;
								break;
								}

							if(pEntry->bFlags&TVIX_HASBUTTON)
								{
								TreeListToggleItem(pData,pData->uSelectedItem,0);
								}
							}

						break;

		case VK_RIGHT:	if(pData->uColumnCount>1 && (pData->uStyleEx&TVS_EX_SUBSELECT))
							{
							while(iCol+1<(int)pData->uColumnCount)
								{
								iCol++;
								iSub = pData->aColumnPos[iCol];
								if(pData->aColumn[iSub].sReal>0)break;
								}
							}
						else{
							if(!pEntry)break;
							if( pEntry->uState&TVIS_EXPANDED)
								{
								iLine++;
								break;
								}

							if(pEntry->bFlags&TVIX_HASBUTTON)
								{
								TreeListToggleItem(pData,pData->uSelectedItem,0);
								}
							}

						break;

		case VK_UP:		iLine--;						break;
 		case VK_DOWN:	iLine++;						break;

		case VK_PRIOR:	iAdd=pData->uPageEnties-1;			// Eine Seite nach oben
						if(iAdd<=0)iAdd=1;
						iLine-=iAdd;
						break;

		case VK_NEXT:	iAdd=pData->uPageEnties-1;			// Eine Seite nach unten
						if(iAdd<=0)iAdd=1;
						iLine+=iAdd;
						break;

		case VK_BACK:		  								// Eine Ebene h?er
						if(pEntry)
							{
								uItem = pEntry->uParent;
							if(!uItem)iLine=0;
							else{
								   iLine = pData->pTreeItems[uItem]->uShowPos-1;
								if(iLine<0)iLine=0;
								}
							}

						break;

		case VK_SPACE:
						if(pEntry && iCol==0 && (pData->uStyle&TVS_CHECKBOXES))
							{
							TreeListChangeCheckbox(pData,pData->uSelectedItem);
							}

						break;

		default:		goto Exit;
			}

		if(iCol >=(int)pData->uColumnCount)iCol=pData->uColumnCount-1;
		if(iCol <0)iCol=0;
		if(iLine>=(int)pData->uItemPosCount)iLine=pData->uItemPosCount-1;
		if(iLine<0)
			{
			if(iOldLine==0 && pData->uFixedLines && pData->uScrollY && pData->uFixedLines<pData->uItemPosCount)
				{
				   iLine +=   pData->uFixedLines+pData->uScrollY;
				if(iLine<(int)pData->uFixedLines)iLine = pData->uFixedLines;

				TreeListEnsureVisible(pData,pData->pItemPos[iLine],0xFFFFFFFF);
				}

			iLine = 0;
			}

		if(!(pData->uStyleEx&TVS_EX_SUBSELECT))iCol=0;


		if(pData->uItemPosCount>0)
		if(iLine!=iOldLine || iCol!=iOldCol)				// Wurde ein anderer Eintrag ausgew?lt
			{
			if(pData->uSelectedCount>1)
				{
				TreeListRemoveFocus(pData);
				}
			else{
				pData->uSelectedBase = uTemp;
				}


			iShift = iDel;

			if(pData->cReSelect)							// Die Shift-Auswahl neu erstellen
				{
				iDel = TVC_DESELECT;
				}

			uItem  = pData->pItemPos  [iLine];
			pTemp  = pData->pTreeItems[uItem];
			iSub   = pData->aColumnPos[iCol ];
			iFocus =(pTemp && (pTemp->uState&TVIS_SELECTED))? TVC_ONLYFOCUS:0;

			TreeListSelectItem(pData,uItem,iSub,TVC_BYKEYBOARD|iDel|iFocus);

			if(!(pData->uStyle&TVS_NOSCROLL))
				{
				TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
				}

			if(pData->cReSelect && pData->uSelectedBase)	// Shift-Select neu ausw?len
				{
				uTemp = pData->uSelectedBase;
				pTemp = pData->pTreeItems[uTemp];
				uStop = uItem;


				while(pTemp && pTemp->uShowPos==0)
					{
						pTemp = pData->pTreeItems[pTemp->uParent];
					if(!pTemp)break;
					}

				if(pTemp && !iShift)
					{
					if(!(pTemp->uState&TVIS_SELECTED))
						{
						TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
						}

					   iLineCmp = pTemp->uShowPos-1;
					if(iLineCmp<iLine)
						{
						uStop = uItem;
						}
					else{
						uStop = uTemp;
						uTemp = uItem;
						}

					for(;;)
						{
							uTemp=TreeListNextUnselUntil(pData,uTemp,uStop);
						if(!uTemp)break;
						TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
						}
					}

				pData->cReSelect = 0;
				}


			if((pData->uStyleEx&TVS_EX_MULTISELECT) && !iDel)
				{
				uVal  = pData->uSelectedBase;
				pTemp = pData->pTreeItems[uVal];
				iBase =(pTemp)? pTemp->uShowPos-1:-1;

				if(iLine>iOldLine)							// Nach oben oder nach unten
					{
					   iMax = (iOldLine>iBase)? iOldLine+1:iOldLine;
					if(iMax<0)iMax=0;
					   iPos = iLine;
					}
				else{
					   iMax = iLine;
					   iPos = (iOldLine<iBase)? iOldLine-1:iOldLine;
					if(iPos<0)iPos=0;
					}

				for(;iPos>=iMax;iPos--)						// ?ersprungene Eintr?e mit aus/abw?len
					{
					uItem = pData->pItemPos[iPos];

					if(iPos!=iLine && iPos!=iBase)
						{
						if(!TreeListXorSelectItem(pData,uItem,TVC_BYKEYBOARD))continue;
						}

					   pTemp = pData->pTreeItems[uItem];	// Auch unsichtbare Kinder abw?len
					if(pTemp)
						{
							iSel  =  pTemp->uState&TVIS_SELECTED;
							pTemp =  pData->pTreeItems[pTemp->uFirstChild];
						if(!pTemp || pTemp->uShowPos)continue;
						if(!iSel)
							{
							for(uTemp=uItem;;)
								{
									uTemp=TreeListNextSelUntil(pData,uTemp,uItem);
								if(!uTemp)break;
								TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
								}
							}
						else{
							for(uTemp=uItem;;)
								{
									uTemp=TreeListNextUnselUntil(pData,uTemp,uItem);
								if(!uTemp)break;
								TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
								}
							}
						}
					}
				}
			}
		}


Exit:

	sNotify.hdr.code	=  TVN_KEYDOWN;
	sNotify.wVKey		= (WORD)(wParam);
	sNotify.wScan		= (WORD)(wParam>>16);
	sNotify.flags		= (UINT)(lParam);

	UNLOCK(pData);

	pData->cKeyIgnore	= (char)SendNotify(pData,&sNotify.hdr);

}

//*****************************************************************************
//*
//*		TreeListSortItemsEx
//*
//*****************************************************************************
//	Sortiert die Kindereintr?e eines Eintrages mitteles einer Funktion
//	pData		: Zeiger auf die Fensterdaten
//	pSortData	: Ist ein Zeiger auf die Sortiertaten
//	iMode		: 1=Rekursiv sortieren
//	Ergibt 1 wenn ok sonst 0
static int TreeListSortItemsEx(TreeListData *pData,TV_SORTEX *pSortData,int iMode)
{
TV_SORTEX	sSort;
unsigned	uNum;
unsigned	uItem;
unsigned	uLast;
unsigned	uFirst;
unsigned	uParent;
BaseItem   *pParent;
BaseItem   *pEntry;
BaseItem   *pNext;
BaseItem  **pList;



	pList = pData->pTreeItems;


	   uParent=U(pSortData->hParent);
	if(uParent>pData->uTreeItemsMax)						// Root-Eintrag sortieren
		{
		if(uParent!=U(TVI_ROOT))return 0;

		   uLast  = pData->uLastChild;
		   uFirst = pData->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==uLast)									// Einzelner Eintrag
			{
				pNext = pList[uFirst];
			if(!pNext->uFirstChild)return 1;

			sSort.hParent		= (HTREEITEM)uFirst;
			sSort.lParam		= pSortData->lParam;
			sSort.lpfnCompare	= pSortData->lpfnCompare;
			TreeListSortItemsEx(pData,&sSort,iMode);
			return 1;
			}

		pParent = 0;
		uParent = 0;
		}
	else{													// Untereintrag sortieren
		   pParent = pList[uParent];
		if(pParent==NULL)return 0;

		   uLast  = pParent->uLastChild;
		   uFirst = pParent->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==uLast)									// Einzelner Eintrag
			{
				pNext = pList[uFirst];
			if(!pNext->uFirstChild)return 1;

			sSort.hParent		= (HTREEITEM)uFirst;
			sSort.lParam		= pSortData->lParam;
			sSort.lpfnCompare	= pSortData->lpfnCompare;
			TreeListSortItemsEx(pData,&sSort,iMode);
			return 1;
			}
		}


	if(iMode)												// Sortiere die Untereintr?e
		{
		pNext				= pList[uFirst];
		sSort.hParent		= (HTREEITEM)uFirst;
		sSort.lParam		= pSortData->lParam;
		sSort.lpfnCompare	= pSortData->lpfnCompare;

		while(pNext)
			{
			if(pNext->uFirstChild)
				{
				TreeListSortItemsEx(pData,&sSort,SORT_NOUPDATE);
				}

			sSort.hParent	= (HTREEITEM)pNext->uNextItem;
			pNext			= pList[pNext->uNextItem];
			}
		}



	if(uFirst==uLast)return 1;



//******************** Erzeuge Eintragsliste **********************************
unsigned   *pItemList;
unsigned   *pItemNew;
unsigned	uEnties[128];
unsigned	uPos;
unsigned	uMax;


	uItem		= uFirst;
	pItemList	= uEnties;
	uMax		= 128;
	uPos		= 0;

	do	{													// Alle Kindeintr?e suchen
		if(uPos>=uMax)
			{
			uMax *= 2;
			pItemNew	= new unsigned[uMax];
			memcpy(pItemNew,pItemList,uPos*sizeof(pItemList[0]));
			if(uPos>128)delete pItemList;
			pItemList = pItemNew;
			}

		pItemList[uPos] = uItem;
		pNext			= pList[uItem];
		uItem			= pNext->uNextItem;
		uPos++;
		}while(uItem);




//************************* Qsort-Algorithmus *********************************
#define XCHANGE_MEM(a,b)	uTemp=pItemList[a];pItemList[a]=pItemList[b];pItemList[b]=uTemp;
PFNTVCOMPAREEX	pCompare;
HTREEITEM		hItemTemp;
LPARAM			lParamTemp;
LPARAM			lParamSort;
HWND			hTreeWnd;
int				iLower,iUpper,iMiddle,iCmp;
int				uMemL[30],uMemU[30];
int				iStart,iLast;
int				iLevel;
unsigned		uTemp;


	pData->cLockChanges=1;
	UNLOCK(pData);

	hTreeWnd	= pData->hWnd;
	pCompare	= pSortData->lpfnCompare;
	lParamSort	= pSortData->lParam;
	iLast		= uPos-1;
	iStart		= 0;
	iLevel		= 0;

	for(;;)
		{
		iLower	=  iStart;
		iMiddle = (iStart+iLast)>>1;						// Mitte bereichnen
		iUpper	=  iLast +1;


		XCHANGE_MEM(iMiddle,iLower);

		uItem	   = pItemList[iStart];
		hItemTemp  = (HTREEITEM)uItem;
		lParamTemp = pList[uItem]->lParam;


		for(;;)
			{
			do  {
				iLower++;
				if(iLower>iLast)break;
				uItem = pItemList[iLower];
				iCmp=pCompare(hTreeWnd,(HTREEITEM)uItem,hItemTemp,pList[uItem]->lParam,lParamTemp,lParamSort);
				}while(iCmp<=0);


			do  {
				iUpper--;
				if(iUpper<=iStart)break;
				uItem = pItemList[iUpper];
				iCmp=pCompare(hTreeWnd,(HTREEITEM)uItem,hItemTemp,pList[uItem]->lParam,lParamTemp,lParamSort);
				}while(iCmp>=0);

			if(iUpper<iLower)break;

			XCHANGE_MEM(iUpper,iLower);
			}


		XCHANGE_MEM(iStart,iUpper);


		if(iUpper-1-iStart >= iLast-iLower )
			{
			if(iStart+1 < iUpper)
				{
				uMemL[iLevel] = iStart;
				uMemU[iLevel] = iUpper-1;
				iLevel++;
				}
			if(iLower < iLast)
				{
				iStart = iLower;
				continue;
				}
			}
		else{
			if(iLower<iLast)
				{
				uMemL[iLevel] = iLower;
				uMemU[iLevel] = iLast;
				iLevel++;
				}

			if(iStart+1 < iUpper)
				{
				iLast = iUpper-1;
				continue;
				}
			}
															// Eine Ebene absteigen
		iLevel--;

		if(iLevel>=0)										// Noch Ebenen vorhanden
			{
			iStart = uMemL[iLevel];
			iLast  = uMemU[iLevel];
			continue;
			}

		break;
		}


	LOCK(pData);
	pData->cLockChanges=0;


//******************** Eintr?e neu einsortirenen *****************************


	uPos--;


		pEntry = pList[uParent];
	if(!pEntry)
		{
		pData->uFirstChild  = pItemList[ 0  ];
		pData->uLastChild   = pItemList[uPos];
		}
	else{
		pEntry->uFirstChild = pItemList[ 0  ];
		pEntry->uLastChild  = pItemList[uPos];
		}


	uLast  = 0;
	uItem  = pItemList[0];

	for(uNum=0;uNum<uPos;)									// Kinder neu einh?gen
		{
		pEntry = pList[uItem];
		pEntry->uPrevItem = uLast;

		uNum++;
		uLast  = uItem;
		uItem  = pItemList[uNum];

		pEntry->uNextItem = uItem;
		}

	pEntry = pList[uItem];
	pEntry->uPrevItem = uLast;
	pEntry->uNextItem = 0;


	if(iMode!=SORT_NOUPDATE)								// Ausgabeliste neuerstellen
	if(uParent==0 || (pParent->uShowPos && (pParent->uState&TVIS_EXPANDED)))
		{
		UpdateItems(pData,uParent);

		if(pData->uStyle&TVS_SHOWSELALWAYS)
		if(pData->uSelectedItem)
			{
			TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			}
		}


	if(uMax>128)delete pItemList;



return 1;
}

//*****************************************************************************
//*
//*		TreeListSortItemsCb
//*
//*****************************************************************************
//	Sortiert die Kindereintr?e eines Eintrages mitteles einer Funktion
//	pData		: Zeiger auf die Fensterdaten
//	pSortData	: Ist ein Zeiger auf die Sortiertaten
//	iMode		: 1=Rekursiv sortieren
//	Ergibt 1 wenn ok sonst 0
static int TreeListSortItemsCb(TreeListData *pData,TV_SORTCB *pSortData,int iMode)
{
TV_SORTCB	sSort;
unsigned	uNum;
unsigned	uItem;
unsigned	uLast;
unsigned	uFirst;
unsigned	uParent;
BaseItem   *pParent;
BaseItem   *pEntry;
BaseItem   *pNext;
BaseItem  **pList;



	pList = pData->pTreeItems;

	   uParent=U(pSortData->hParent);
	if(uParent>pData->uTreeItemsMax)						// Root-Eintrag sortieren
		{
		if(uParent!=U(TVI_ROOT))return 0;

		   uLast  = pData->uLastChild;
		   uFirst = pData->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==uLast)									// Einzelner Eintrag
			{
				pNext = pList[uFirst];
			if(!pNext->uFirstChild)return 1;

			sSort.hParent		= (HTREEITEM)uFirst;
			sSort.lParam		= pSortData->lParam;
			sSort.lpfnCompare	= pSortData->lpfnCompare;
			TreeListSortItemsCb(pData,&sSort,iMode);
			return 1;
			}

		pParent = 0;
		uParent = 0;
		}
	else{													// Untereintrag sortieren
		   pParent = pList[uParent];
		if(pParent==NULL)return 0;

		   uLast  = pData->uLastChild;
		   uFirst = pParent->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==uLast)									// Einzelner Eintrag
			{
				pNext = pList[uFirst];
			if(!pNext->uFirstChild)return 1;

			sSort.hParent		= (HTREEITEM)uFirst;
			sSort.lParam		= pSortData->lParam;
			sSort.lpfnCompare	= pSortData->lpfnCompare;
			TreeListSortItemsCb(pData,&sSort,iMode);
			return 1;
			}
		}


	if(iMode)												// Sortiere die Untereintr?e
		{
		pNext				= pList[uFirst];
		sSort.hParent		= (HTREEITEM)uFirst;
		sSort.lParam		= pSortData->lParam;
		sSort.lpfnCompare	= pSortData->lpfnCompare;

		while(pNext)
			{
			if(pNext->uFirstChild)
				{
				TreeListSortItemsCb(pData,&sSort,SORT_NOUPDATE);
				}

			sSort.hParent	= (HTREEITEM)pNext->uNextItem;
			pNext			= pList[pNext->uNextItem];
			}
		}


	if(uFirst==uLast)return 1;



//******************** Erzeuge Eintragsliste **********************************
unsigned   *pItemList;
unsigned   *pItemNew;
unsigned	uEnties[128];
unsigned	uPos;
unsigned	uMax;


	uItem		= uFirst;
	pItemList	= uEnties;
	uMax		= 128;
	uPos		= 0;

	do	{													// Alle Kindeintr?e suchen
		if(uPos>=uMax)
			{
			uMax *= 2;
			pItemNew	= new unsigned[uMax];
			memcpy(pItemNew,pItemList,uPos*sizeof(pItemList[0]));
			if(uPos>128)delete pItemList;
			pItemList = pItemNew;
			}

		pItemList[uPos] = uItem;
		pNext			= pList[uItem];
		uItem			= pNext->uNextItem;
		uPos++;
		}while(uItem);




//************************* Qsort-Algorithmus *********************************
#define XCHANGE_MEM(a,b)	uTemp=pItemList[a];pItemList[a]=pItemList[b];pItemList[b]=uTemp;
PFNTVCOMPARE	pCompare;
LPARAM			lParamTemp;
LPARAM			lParamSort;
int				iLower,iUpper,iMiddle,iCmp;
int				uMemL[30],uMemU[30];
int				iStart,iLast;
int				iLevel;
unsigned		uTemp;


	pData->cLockChanges=1;
	UNLOCK(pData);

	pCompare	= pSortData->lpfnCompare;
	lParamSort	= pSortData->lParam;
	iLast		= uPos-1;
	iStart		= 0;
	iLevel		= 0;

	for(;;)
		{
		iLower	=  iStart;
		iMiddle = (iStart+iLast)>>1;						// Mitte bereichnen
		iUpper	=  iLast +1;


		XCHANGE_MEM(iMiddle,iLower);

		uItem	   = pItemList[iStart];
		lParamTemp = pList[uItem]->lParam;

		for(;;)
			{
			do  {
				iLower++;
				if(iLower>iLast)break;
				uItem = pItemList[iLower];
				iCmp=pCompare(pList[uItem]->lParam,lParamTemp,lParamSort);
				}while(iCmp<=0);

			do  {
				iUpper--;
				if(iUpper<=iStart)break;
				uItem = pItemList[iUpper];
				iCmp=pCompare(pList[uItem]->lParam,lParamTemp,lParamSort);
				}while(iCmp>=0);

			if(iUpper<iLower)break;

			XCHANGE_MEM(iUpper,iLower);
			}


		XCHANGE_MEM(iStart,iUpper);


		if(iUpper-1-iStart >= iLast-iLower )
			{
			if(iStart+1 < iUpper)
				{
				uMemL[iLevel] = iStart;
				uMemU[iLevel] = iUpper-1;
				iLevel++;
				}
			if(iLower < iLast)
				{
				iStart = iLower;
				continue;
				}
			}
		else{
			if(iLower<iLast)
				{
				uMemL[iLevel] = iLower;
				uMemU[iLevel] = iLast;
				iLevel++;
				}

			if(iStart+1 < iUpper)
				{
				iLast = iUpper-1;
				continue;
				}
			}
															// Eine Ebene absteigen
		iLevel--;

		if(iLevel>=0)										// Noch Ebenen vorhanden
			{
			iStart = uMemL[iLevel];
			iLast  = uMemU[iLevel];
			continue;
			}

		break;
		}


	LOCK(pData);
	pData->cLockChanges=0;


//******************** Eintr?e neu einsortirenen *****************************



	uPos--;


		pEntry = pList[uParent];
	if(!pEntry)
		{
		pData->uFirstChild  = pItemList[ 0  ];
		pData->uLastChild   = pItemList[uPos];
		}
	else{
		pEntry->uFirstChild = pItemList[ 0  ];
		pEntry->uLastChild  = pItemList[uPos];
		}


	uLast  = 0;
	uItem  = pItemList[0];

	for(uNum=0;uNum<uPos;)									// Kinder neu einh?gen
		{
		pEntry = pList[uItem];
		pEntry->uPrevItem = uLast;

		uNum++;
		uLast  = uItem;
		uItem  = pItemList[uNum];

		pEntry->uNextItem = uItem;
		}

	pEntry = pList[uItem];
	pEntry->uPrevItem = uLast;
	pEntry->uNextItem = 0;


	if(iMode!=SORT_NOUPDATE)								// Ausgabeliste neuerstellen
	if(uParent==0 || (pParent->uShowPos && (pParent->uState&TVIS_EXPANDED)))
		{
		UpdateItems(pData,uParent);

		if(pData->uStyle&TVS_SHOWSELALWAYS)
		if(pData->uSelectedItem)
			{
			TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			}
		}


	if(uMax>128)delete pItemList;



return 1;
}

//*****************************************************************************
//*
//*		TreeListSortItems
//*
//*****************************************************************************
//	Sortiert die Kindereintr?e eines Eintrages via Text
//	pData		: Zeiger auf die Fensterdaten
//	pSortData	: Ist ein Zeiger auf die Sortiertaten
//	iMode		: 1=Rekursiv sortieren
//	Ergibt 1 wenn ok sonst 0
static int TreeListSortItems(TreeListData *pData,unsigned uParent,int iMode)
{
unsigned	uNum;
unsigned	uItem;
unsigned	uLast;
unsigned	uFirst;
BaseItem   *pParent;
BaseItem   *pEntry;
BaseItem   *pNext;
BaseItem  **pList;



	pList = pData->pTreeItems;


	if(uParent>pData->uTreeItemsMax)						// Root-Eintrag sortieren
		{
		if(uParent!=U(TVI_ROOT))return 0;

		   uLast  = pData->uLastChild;
		   uFirst = pData->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==pData->uLastChild)						// Einzelner Eintrag
			{
				pNext = pList[uFirst];
			if(!pNext->uFirstChild)return 1;

			TreeListSortItems(pData,uFirst,iMode);
			return 1;
			}

		pList   = pData->pTreeItems;
		pParent = 0;
		uParent = 0;
		}
	else{													// Untereintrag sortieren
		   pParent = pList[uParent];
		if(pParent==NULL)return 0;

		   uLast  = pParent->uLastChild;
		   uFirst = pParent->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==pParent->uLastChild)						// Einzelner Eintrag
			{
				pNext = pList[uFirst];
			if(!pNext->uFirstChild)return 1;

			TreeListSortItems(pData,uFirst,iMode);
			return 1;
			}
		}


	if(iMode)												// Sortiere die Untereintr?e
		{
		uItem = uFirst;

		while(uItem)
			{
			   pNext = pList[uItem];
			if(pNext->uFirstChild)
				{
				TreeListSortItems(pData,uItem,SORT_NOUPDATE);
				}

			uItem = pNext->uNextItem;
			}
		}


	if(uLast==uFirst)return 1;


//******************** Erzeuge Eintragsliste **********************************
unsigned   *pItemList;
unsigned   *pItemNew;
unsigned	uEnties[128];
unsigned	uPos;
unsigned	uMax;


	uItem		= uFirst;
	pItemList	= uEnties;
	uMax		= 128;
	uPos		= 0;

	do	{													// Alle Kindeintr?e suchen
		if(uPos>=uMax)
			{
			uMax *= 2;
			pItemNew	= new unsigned[uMax];
			memcpy(pItemNew,pItemList,uPos*sizeof(pItemList[0]));
			if(uPos>128)delete pItemList;
			pItemList = pItemNew;
			}

		pItemList[uPos] = uItem;
		pNext			= pList[uItem];
		uItem			= pNext->uNextItem;
		uPos++;
		}while(uItem);




//************************* Qsort-Algorithmus *********************************
#define XCHANGE_MEM(a,b)	uTemp=pItemList[a];pItemList[a]=pItemList[b];pItemList[b]=uTemp;
LPCTSTR			pTextTemp;
LPCTSTR			pText;
int				iLower,iUpper,iMiddle,iCmp;
int				uMemL[30],uMemU[30];
int				iStart,iLast;
int				iLevel;
int				iNone;
unsigned		uSize;
unsigned		uTemp;


	pData->cLockChanges=1;
	UNLOCK(pData);

	iLast		= uPos-1;
	iStart		= 0;
	iLevel		= 0;

	for(;;)
		{
		iLower	=  iStart;
		iMiddle = (iStart+iLast)>>1;						// Mitte bereichnen
		iUpper	=  iLast +1;


		XCHANGE_MEM(iMiddle,iLower);

		   uItem  = pItemList[iStart];
		   pEntry = pList[uItem];
		if(pEntry->bCallback&TVIF_TEXT)
			{
			uSize=0;
			LOCK(pData);
			CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pTextTemp);
			UNLOCK(pData);
			}
		else{
			pTextTemp = pEntry->pText;
			}


		for(;;)
			{
			do  {
				iLower++;
				if(iLower>iLast)break;

				   uItem  = pItemList[iLower];
				   pEntry = pList[uItem];
				if(pEntry->bCallback&TVIF_TEXT)
					{
					uSize=1;
					LOCK(pData);
					CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pText);
					UNLOCK(pData);
					}
				else{
					pText = pEntry->pText;
					}

				iCmp=str_icmp(pText,pTextTemp);
				}while(iCmp<=0);

			do  {
				iUpper--;

				if(iUpper<=iStart)break;
				   uItem  = pItemList[iUpper];
				   pEntry = pList[uItem];
				if(pEntry->bCallback&TVIF_TEXT)
					{
					LOCK(pData);
					CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pText);
					UNLOCK(pData);
					}
				else{
					pText = pEntry->pText;
					}

				iCmp=str_icmp(pText,pTextTemp);
				}while(iCmp>=0);

			if(iUpper<iLower)break;

			XCHANGE_MEM(iUpper,iLower);
			}


		XCHANGE_MEM(iStart,iUpper);


		if(iUpper-1-iStart >= iLast-iLower )
			{
			if(iStart+1 < iUpper)
				{
				uMemL[iLevel] = iStart;
				uMemU[iLevel] = iUpper-1;
				iLevel++;
				}
			if(iLower < iLast)
				{
				iStart = iLower;
				continue;
				}
			}
		else{
			if(iLower<iLast)
				{
				uMemL[iLevel] = iLower;
				uMemU[iLevel] = iLast;
				iLevel++;
				}

			if(iStart+1 < iUpper)
				{
				iLast = iUpper-1;
				continue;
				}
			}
															// Eine Ebene absteigen
		iLevel--;

		if(iLevel>=0)										// Noch Ebenen vorhanden
			{
			iStart = uMemL[iLevel];
			iLast  = uMemU[iLevel];
			continue;
			}

		break;
		}


	LOCK(pData);
	pData->cLockChanges=0;


//******************** Eintr?e neu einsortirenen *****************************


	uPos--;


		pEntry = pList[uParent];
	if(!pEntry)
		{
		pData->uFirstChild  = pItemList[ 0  ];
		pData->uLastChild   = pItemList[uPos];
		}
	else{
		pEntry->uFirstChild = pItemList[ 0  ];
		pEntry->uLastChild  = pItemList[uPos];
		}


	uLast  = 0;
	uItem  = pItemList[0];

	for(uNum=0;uNum<uPos;)									// Kinder neu einh?gen
		{
		pEntry = pList[uItem];
		pEntry->uPrevItem = uLast;

		uNum++;
		uLast  = uItem;
		uItem  = pItemList[uNum];

		pEntry->uNextItem = uItem;
		}

	pEntry = pList[uItem];
	pEntry->uPrevItem = uLast;
	pEntry->uNextItem = 0;


	if(iMode!=SORT_NOUPDATE)								// Ausgabeliste neuerstellen
	if(uParent==0 || (pParent->uShowPos && (pParent->uState&TVIS_EXPANDED)))
		{
		UpdateItems(pData,uParent);

		if(pData->uStyle&TVS_SHOWSELALWAYS)
		if(pData->uSelectedItem)
			{
			TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			}
		}


	if(uMax>128)delete pItemList;



return 1;
}

//*****************************************************************************
//*
//*		TreeListEndLabelEdit
//*
//*****************************************************************************
//	Beendet das editiert eines Eintrages
//	pData	: Zeiger auf Fensterdaten
//	iMode	: Gibt an wie das Editieren beendet wurde
//				0 = Es gab es einen Abbruch mit ESC
//				1 = Eingabe ohne Enter
//				2 = Eingabe mit  Enter
//	Ergibt das Handle des Edit-Controlls oder NULL bei einem Fehler
static int TreeListEndLabelEdit(TreeListData *pData,int iMode)
{
TCHAR			cText[2052];
NMTVDISPINFO	sNotify;
TV_ITEM			sSet;
LRESULT			lRet;
unsigned		uSub;
unsigned		uItem;
ExtraItem	   *pExtra;
BaseItem	   *pEntry;
LPCTSTR		   *pList;
LPCTSTR			pText;
LPTSTR			pGetT;
TCHAR			cChar;
int 			iAuto;
int				iIcon;
int				iLine;
int				iPos;
int				iLen;
int				iMax;
int				iSel;
char			cCb;




	uItem = pData->uEditItem;
	uSub  = pData->uEditSub;
	cCb	  =	pData->cEditCb;

	pData->uEditItem = 0;
	pData->uEditSub  = 0;
	pData->cEditCb	 = 0;


	if(uItem>pData->uTreeItemsMax || uSub>=pData->uColumnCount)
		{
		return 0;
		}

   		pEntry = pData->pTreeItems[uItem];
	if(!pEntry)
		{
		return 0;
		}


	if(iMode)												// Eingabe
		{
		GetWindowText(pData->hEdit,cText,sizeof(cText)/sizeof(cText[0]));
		cText[sizeof(cText)/sizeof(cText[0])-1]=0;
		sNotify.item.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT|TVIF_SUBITEM;
		sNotify.item.cchTextMax = sizeof(cText)/sizeof(cText[0])-1;
		sNotify.item.pszText	= cText;

		if(pData->cColumnStart)								// Wurde ein Text eingegeben
			{
			sNotify.item.mask  |= TVIF_TEXTCHANGED;
			}

		if(iMode==2)										// Wurde der Text mit RETURN eingegeben
			{
			sNotify.item.mask  |= TVIF_RETURNEXIT;
			}
		}
	else{													// Abbruch
		sNotify.item.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_CANCELED|TVIF_SUBITEM;
		sNotify.item.pszText	= NULL;
		sNotify.item.cchTextMax = 0;
		}


	sNotify.hdr.code			= TVN_ENDLABELEDIT;
	sNotify.item.hItem			= (HTREEITEM)uItem;
	sNotify.item.stateMask		= 0xFFFFFFFF;
	sNotify.item.cChildren		= uSub;


	if(uSub)												// Wurde der Text in einer Sub-Spalte ge?dert
		{
			pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)
			{
			sNotify.item.state	= 0;
			sNotify.item.lParam	= 0;
			}
		else{
			sNotify.item.state  = pEntry->uState&TVIS_BASEFLAGS;
			sNotify.item.state |= pExtra->uState;
			sNotify.item.lParam	= pEntry->lParam;
			}
		}
	else{
		sNotify.item.state	= pEntry->uState;
		sNotify.item.lParam	= pEntry->lParam;
		}


	UNLOCK(pData);
	ShowWindow(pData->hEdit,SW_HIDE);
	lRet=SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

	if(lRet || !iMode)return 0;


	if(cCb)													// Callback aufrufen
		{
		sNotify.hdr.code		= TVN_SETDISPINFO;
		sNotify.item.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT|TVIF_SUBITEM;
		sNotify.item.stateMask	= (UINT)~TVIS_BASEFLAGS;
		sNotify.item.hItem		= (HTREEITEM)uItem;
		sNotify.item.cChildren	= uSub;

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);
		LOCK(pData);
		}
	else{													// Neuen Text eingeben
		pGetT			= sNotify.item.pszText;
		sSet.mask		= TVIF_SUBITEM|TVIF_TEXT;
		sSet.cchTextMax	= sNotify.item.cchTextMax;
		sSet.hItem		= (HTREEITEM)uItem;
		sSet.pszText	= pGetT;
		sSet.cChildren	= uSub;

		iIcon			= pData->aColumn[uSub].iCbIcon;
		iAuto			= pData->aColumn[uSub].bEdit;


		if(iIcon>=0 && iAuto!=TVAX_NONE)					// Auch ein Icon zuweisen
			{
			iPos = -1;

			if((1<<iAuto)&((1<<TVAX_CBLIST)|(1<<TVAX_COMBO)))
				{
				iPos = (int)SendMessage(pData->hEdit,CB_GETCURSEL,0,0);
				sSet.mask		   |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				sSet.iImage			= iIcon+iPos;
				sSet.iSelectedImage	= sSet.iImage;
				}

			if(iPos<=-1)
				{
				   iMax =pData->aColumn[uSub].bCbSize;
				if(iMax<=0)iMax=1024;

				iSel = -1;


				if(pData->aColumn[uSub].bFlags&TVAE_PTRLIST)// Zeigerliste  char *pTexte[]={"1","2",NULL};
					{
					pList = (LPCTSTR*)pData->aColumn[uSub].pCbData;

					for(iPos=0;iPos<iMax;iPos++)
						{
						if(!pList[iPos])break;
						if(!str_cmp(pGetT,pList[iPos]))iSel=iPos;
						}
					}
				else{										// Textliste  char *pText="1|2|3";
					pText = (LPTSTR)pData->aColumn[uSub].pCbData;
					cChar = (TCHAR )pData->aColumn[uSub].bCbChar;

					for(iPos=0;iPos<iMax;iPos++)
						{
						for(iLen=0;pText[iLen];iLen++)
							{
							if(pText[iLen]==cChar)break;
							}

						if(str_ncmp(pGetT,pText,iLen)==0 && !pGetT[iLen])
							{
							iSel = iPos;
							break;
							}

						   pText+=iLen;
						if(pText[0]==cChar)pText++;
						if(pText[0]==0)break;
						}
					}

				sSet.mask		   |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				sSet.iImage			= iIcon+iSel;
				sSet.iSelectedImage	= sSet.iImage;
				}
			}

		if(pData->aColumn[uSub].bFlags&TVAE_NEXTLINE &&			// In die n?hste Zeile springen
		   iMode==2 && iAuto!=TVAX_NONE && iAuto!=TVAX_STEP)
			{
			if(!pData->cColumnStart)
				{
				if(uSub)										// Hat sich der Text ver?dert ?
					{
					   pExtra = pData->pExtraItems[uSub-1][uItem];
					if(pExtra && pExtra->pText)
						{
						if(!_tcscmp(pGetT,pExtra->pText))return 1;
						}
					else{
						if(!*pGetT)return 1;
						}
					}
				else{
					   pEntry = pData->pTreeItems[uItem];
					if(pEntry && pEntry->pText)
						{
						if(!_tcscmp(pGetT,pEntry->pText))return 1;
						}
					else{
						if(!*pGetT)return 1;
						}
					}
				}

			TreeListSetItem(pData,&sSet);


			   iLine = pEntry->uShowPos;
			if(iLine<(int)pData->uItemPosCount)
				{
				uItem = pData->pItemPos[iLine];
				TreeListSelectItem(pData,uItem,pData->uSelectedSub,TVC_BYKEYBOARD|TVC_DESELECT);
				}
			}
		else{
			TreeListSetItem(pData,&sSet);
			}
		}



return 1;
}

//*****************************************************************************
//*
//*		TreeListEditLabel
//*
//*****************************************************************************
//	Startet das editiert eines Eintrages
//	pData	: Zeiger auf Fensterdaten
//	uItem	: Item das editiert werden soll
//	uSub	: Spalte die editiert werden soll und andere Flags
//				TVIR_EDITCOL(n)		= Spalte angeben
//				TVIR_SELAREA(a,b)	= Einen Textbereich ausw?len
//				TVIR_SETCURSOR(n)	= Den Cursor auf eine bestimmte Textstelle
//				TVIR_SETAT(n)		= Den Cursor auf eine bestimmte Pixelstelle
//				TVIR_SELALL			= Den gesammten Text w?len
//				TVIR_EDITCOMBOLIST	= Statt dem Edit-Fenster eine ComboBox nur mit Listenauswahl einblenden
//				TVIR_EDITFULL		= Das Edit-Fenster ?er die volle Breite einblenden
//				TVIR_EDITCOMBOBOX	= Statt dem Edit-Fenster eine ComboBox einblenden
//	Ergibt das Handle des Edit-Controlls oder NULL bei einem Fehler
static HWND TreeListEditLabel(TreeListData *pData,unsigned uItem,unsigned uSub)
{
HDC				hDc;
HWND			hWnd;
LRESULT			lRet;
WNDPROC			pProc;
char			cTemp;
LPARAM			lParam;
ExtraItem	   *pExtra;
BaseItem	   *pEntry;
NMTVDISPINFO	sNotify;
LPCTSTR			pText;
RECT			sRect;
SIZE			sSize;
HFONT			hFont;
unsigned		uCol;
unsigned		uSel;
unsigned		uNext;
unsigned		uBits;
unsigned		uSize;
unsigned		uStart;
unsigned		uState;
unsigned		uFlags;
unsigned		uHeight;
int				iPixels;
int				iWidth;
int				iTemp;



	uBits	=  uSub>>29;
	uSel    = (uSub>>8)&0x0FFFFF;
	uSub   &=  0xFF;


	if( uSub >=pData->uColumnCount )return NULL;
	if( uItem> pData->uTreeItemsMax)return NULL;
		pEntry=pData->pTreeItems[uItem];
	if(!pEntry)return NULL;


	if(uItem!=pData->uSelectedItem || uSub!=pData->uSelectedSub)
		{
		uCol  = (pData->uStyleEx&TVS_EX_SUBSELECT)? uSub:0;
		iTemp = TreeListSelectItem(pData,uItem,uCol,TVC_UNKNOWN);
		}

	TreeListEnsureVisible(pData,uItem,uSub);

	if(pData->hEdit)										// Editfenster l?chen
		{
		DestroyWindow(pData->hEdit);
		pData->hEdit=0;
		}


	switch(uBits&3)											// Editfenster neu erzeugen
		{
	case 1:	pData->hEdit=CreateWindow(_T("COMBOBOX"),NULL,WS_BORDER|WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWN|WS_VSCROLL,0,0,0,0,pData->hWnd,(HMENU)3,NULL,NULL);
			pData->uEditMode=1;
			SendMessage(pData->hEdit,CB_LIMITTEXT,2048,0);
			break;

	case 2:	pData->hEdit=CreateWindow(_T("COMBOBOX"),NULL,WS_BORDER|WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWNLIST|WS_VSCROLL,0,0,0,0,pData->hWnd,(HMENU)3,NULL,NULL);
			pData->uEditMode=2;
			SendMessage(pData->hEdit,CB_LIMITTEXT,2048,0);
			break;

	default:uFlags=ES_LEFT;

			if(uBits&4)
				{
				if(pData->aColumn[uSub].bAlign==DT_RIGHT )uFlags=ES_RIGHT;
				if(pData->aColumn[uSub].bAlign==DT_CENTER)uFlags=ES_CENTER;
				}

			pData->hEdit=CreateWindow(_T("EDIT"),NULL,WS_BORDER|WS_CHILD|ES_AUTOHSCROLL|uFlags,0,0,0,0,pData->hWnd,(HMENU)3,NULL,NULL);
			pData->uEditMode=0;
			SendMessage(pData->hEdit,EM_SETLIMITTEXT,2048,0);
			break;
		}



	if(!pData->hEdit)return NULL;

	if(pSetWindowTheme)										// Entferne die Visual-Styles (ab XP)
		{
		pSetWindowTheme(pData->hEdit,L"", L"");
		}


	pData->pProcId3 = (WNDPROC)GetWindowLongPtr(pData->hEdit,GWLP_WNDPROC);
	SetWindowLongPtr(pData->hEdit,GWLP_USERDATA,(LPARAM)pData);
	SetWindowLongPtr(pData->hEdit,GWLP_WNDPROC ,(LPARAM)EditProc);

	hWnd = GetWindow(pData->hEdit,GW_CHILD);

	while(hWnd)
		{
		pProc = (WNDPROC)GetWindowLongPtr(hWnd,GWLP_WNDPROC);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LPARAM)pProc);
		SetWindowLongPtr(hWnd,GWLP_WNDPROC ,(LPARAM)EditProc);
		hWnd  = GetNextWindow(hWnd,GW_HWNDNEXT);
		}


	pData->cEditCb = 0;

	if(pData->uEditMode>=1)									// ComboBox leeren
		{
		SendMessage(pData->hEdit,CB_RESETCONTENT,0,0);
		}



	if(uSub==0)												// Haupteintrag bearbeiten
		{
		TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT,&sRect);

		pText	= pEntry->pText;
		uSize	= pEntry->uTextSize;
		iPixels	= pEntry->iTextPixels+10;
		lParam	= pEntry->lParam;
		uState	= pEntry->uState;
		hFont	=(uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;

		if(pEntry->bCallback&TVIF_TEXT)
			{
			CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iTemp,uSize,pText);
			hDc=GetDC    (pData->hWnd);
			SelectObject (hDc,hFont);
			DrawText     (hDc,pText,uSize,&sRect,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
			ReleaseDC    (pData->hWnd,hDc);
			iPixels  = sRect.right-sRect.left+10;
			pData->cEditCb=1;
			}

		if(uBits&4)											// Volle Spaltenbreite
			{
			if(pEntry->iImage!=TV_NOIMAGE)
				{
				uNext		 = pData->aColumnPos[1];
				sRect.right  = pData->aColumnXpos[uNext];
				sRect.right -= pData->uScrollX;
				}

			iPixels	= sRect.right-sRect.left-2;
			}
		else{
			if(pData->uEditMode)
				{
				if(iPixels<60)iPixels=60;
				}
			else{
				if(iPixels<48)iPixels=48;
				}

			if(pText && *pText)
				{
				iPixels += str_len(pText);
				}

			if(pData->uEditMode)
				{
				iPixels+=GetSystemMetrics(SM_CXHSCROLL);
				}
			}
		}
	else{													// Extraeintrag bearbeiten
		if(uBits&4)
			{
			TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_COLTOSUB(uSub),&sRect);

			if(pData->aColumn[uSub].bEdit>=TVAX_CHECK)
				{
				   sRect.left += pData->iChecksSizeX;
				if(sRect.right<sRect.left)sRect.right=sRect.left+1;
				}
			}
		else{
			TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT|TVIR_COLTOSUB(uSub),&sRect);
			}

			pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)
			{
			pData->cTempText1[0] = 0;
			pText	= pData->cTempText1;
			uSize	= sizeof(pData->cTempText1)/sizeof(pData->cTempText1[0]);
			iPixels	= sRect.right-sRect.left+10;
			hFont	= pData->hFontN;
			uState  = pEntry->uState&TVIS_BASEFLAGS;
			lParam	= 0;
			}
		else{
			pText	= pExtra->pText;
			uSize	= pExtra->uTextSize;
			iPixels	= pExtra->iTextPixels+10;
			lParam	= pEntry->lParam;
			uState	= pExtra->uState;
			uState |= pEntry->uState&TVIS_BASEFLAGS;
			hFont	=(uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;


			if(pExtra->bCallback&TVIF_TEXT)
				{
				CallbackExtra(pData,pEntry,pExtra,uItem,uSub,TVIF_TEXT,iTemp,uSize,pText);
				hDc=GetDC    (pData->hWnd);
				SelectObject (hDc,hFont);
				DrawText     (hDc,pText,uSize,&sRect,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
				ReleaseDC    (pData->hWnd,hDc);
				iPixels=sRect.right-sRect.left;
				pData->cEditCb=1;
				}
			}

		if(uBits&4)											// Volle Spaltenbreite
			{
			if(pExtra && pExtra->iImage!=TV_NOIMAGE)
				{
				sRect.left += pData->iImagesSizeX+1;
				}

			iPixels	= sRect.right-sRect.left-2;
			}
		else{
			if(pData->uEditMode)
				{
				if(iPixels<60)iPixels=60;
				}
			else{
				if(iPixels<48)iPixels=48;
				}

			if(pText && *pText)
				{
				iPixels += str_len(pText);
				}

			if(pData->uEditMode)
				{
				iPixels+=GetSystemMetrics(SM_CXHSCROLL);
				}

			switch(pData->aColumn[uSub].bAlign)
				{
			case DT_RIGHT:	iWidth		= sRect.right-sRect.left;
							sRect.left += iWidth-iPixels;
							break;

			case DT_CENTER:	iWidth		= sRect.right-sRect.left;
							sRect.left += (iWidth-iPixels)/2;
							break;
				}
			}
		}


	UNLOCK(pData);

	sNotify.hdr.code			= TVN_BEGINLABELEDIT;
	sNotify.item.mask			= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT|TVIF_SUBITEM;
	sNotify.item.hItem			= (HTREEITEM)uItem;
	sNotify.item.pszText		= (LPTSTR)pText;
	sNotify.item.lParam			= lParam;
	sNotify.item.state			= uState;
	sNotify.item.cchTextMax		= uSize;
	sNotify.item.stateMask		= 0xFFFFFFFF;
	sNotify.item.cChildren		= uSub;

	lRet=SendNotify(pData,&sNotify.hdr);

	LOCK(pData);

	if(lRet)												// Das Editieren abbrechen
		{
		TreeListEndLabelEdit(pData,0);
		return NULL;
		}


		pEntry=pData->pTreeItems[uItem];
	if(!pEntry)return NULL;


	if(pData->uToolTipItem)									// Ein offenes Tooltip verstecken
		{
		UpdateToolTip(pData,0,0);
		}


	UNLOCK(pData);
	SetFocus(pData->hEdit);
	LOCK(pData);

	if(pData->uEditMode)
		{
		sRect.top--;
		uHeight	= 260;
		}
	else{
		uHeight=pData->iFontHeight+4;
		}

	if(pData->uEditMode)
		{
		if(iPixels<60)iPixels=60;
		}
	else{
		if(iPixels<48)iPixels=48;
		}

	cTemp = pData->cColumnStart;
	SetWindowPos (pData->hEdit,HWND_TOP,sRect.left+2,sRect.top+1,iPixels,uHeight,SWP_SHOWWINDOW);
	SendMessage  (pData->hEdit,WM_SETFONT,(WPARAM)hFont,0);
	SetWindowText(pData->hEdit,pText);
	pData->cColumnStart = cTemp;


	switch(uSel>>18)										// Welche Textauswahl
		{
	case 3:	uStart = (uSel   )&0x01FF;						// Einen Textbereich markieren
			uSize  = (uSel>>9)&0x01FF;
			break;

	case 2:	uStart = uSel&0x3FFFF;							// Cursor auf eine bestimmte Stelle
			uSize  = uStart;
			break;

	case 1:	uSel  &= 0x3FFFF;
			if(uSel>4)uSel-=4;
			hDc=GetDC(pData->hEdit);
			SelectObject(hDc,hFont);
			GetTextExtentExPoint(hDc,pText,uSize,uSel,&iWidth,NULL,&sSize);

			if(uSize>0 && iWidth<(int)uSize-1)				// Halben Buchstaben addieren
				{
				GetTextExtentExPoint(hDc,pText+iWidth,1,uSel,NULL,NULL,&sSize);
				uSel += sSize.cx/2;
				GetTextExtentExPoint(hDc,pText,uSize,uSel,&iWidth,NULL,&sSize);
				}

			ReleaseDC(pData->hEdit,hDc);

			if(sSize.cx<=(int)uSel)
				{
				uStart = 0;
				break;
				}

			uStart	= iWidth;
			uSize	= iWidth;
			break;

	default:uStart = 0;										// Alles markieren
			break;
		}


	switch(pData->uEditMode)
		{
	case 0:	SendMessage(pData->hEdit,EM_SETSEL,uStart,uSize);					break;
	case 1:	SendMessage(pData->hEdit,CB_SETEDITSEL,0,MAKELPARAM(uStart,uSize));	break;
	default:uStart = 0; uSize=0xFFFF;											break;
		}

	RedrawWindow(pData->hEdit,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);

	pData->uLastSel  = MAKELPARAM(uStart,uSize);
	pData->uEditItem = uItem;
	pData->uEditSub	 = uSub;



return pData->hEdit;
}


//*****************************************************************************
//*
//*		TreeListStartAutoEdit
//*
//*****************************************************************************
//	Startet die Editierung f? einen Wert via Notyfy-R?kfrage.
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist der Eintrag der Editiert werden soll
//	uSub		: Ist die Nummer der Spalte
//	wParam		: Ist der W-Parameter des Tastendrucks
//					VK_EDITCLK	bei einem Clickauf das ausgew?lte Feld
//					VK_DBLCLK	bei einem Doppelclick
//					VK_RETURN	bei einen Enter-Druck
//					VK_ISACHAR	bei WM_CHAR Nachrichten
//					<char>		bei einer Zeicheneingabe
//	lParam		: Ist der L-Parameter des Tastendrucks (bzw. die Koordinaten)
//	Ergibt 1 das Editieren gestartet wurde, ansonsten 0
//				0 wenn der Eintrag nicht gew?lt wurde
static int TreeListStartNotifyEdit(TreeListData *pData,unsigned uItem,unsigned uSub,unsigned wParam,unsigned lParam)
{
TCHAR			cText[1024];
TV_STARTEDIT	sNotify;
ExtraItem      *pExtra;
BaseItem       *pEntry;
unsigned		uBits;
unsigned		uCnt;
unsigned		uMax;
unsigned		uLen;
unsigned		uNum;
LPCTSTR			pText;
RECT			sRect;
TCHAR			cChar;
LRESULT			lRet;
MSG				sMsg;
HWND			hWnd;
INT				iSel;



	if(!(pData->uStyle&TVS_EDITLABELS)        )return 0;
	if(uItem==0 || uItem> pData->uTreeItemsMax)return 0;
	if(uSub  >0 && uSub >=pData->uColumnCount )return 0;


		pEntry  =  pData->pTreeItems[uItem];
	if(!pEntry)return 0;

	if(wParam<' ' && wParam!=VK_RETURN)						// Falsche Taste
		{
		return 0;
		}

	if(wParam>=' ' && wParam<=0xFFFF)						// Shift und Cltr pr?en
		{
		if(GetKeyState(VK_MENU)&0x8000)
			{
			return 0;
			}

		if(GetKeyState(VK_CONTROL)&0x8000)
			{
			return 0;
			}
		}


	sNotify.hdr.code			= TVN_STARTEDIT;
	sNotify.uAction				= (UINT)wParam;
	sNotify.item.mask			= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
	sNotify.item.hItem			= (HTREEITEM)uItem;
	sNotify.item.stateMask		= 0xFFFFFFFF;
	sNotify.item.lParam			= pEntry->lParam;
	sNotify.item.cChildren		= uSub;
	sNotify.uHeight				= 0;
	sNotify.pTextEntries		= 0;
	sNotify.pTextList			= 0;
	sNotify.uMaxEntries			= 255;
	sNotify.ptAction.x			= GET_X_LPARAM(lParam);
	sNotify.ptAction.y			= GET_Y_LPARAM(lParam);


	if(uSub)												// Spalte verwenden
		{
	    pExtra  =  pData->pExtraItems[uSub-1][pData->uSelectedItem];

		if(pExtra)
			{
			sNotify.item.state		= pExtra->uState&~TVIS_BASEFLAGS;
			sNotify.item.pszText	= pExtra->pText;
			sNotify.item.cchTextMax	= pExtra->uTextSize;
			}
		else{
			sNotify.item.state		= 0;
			sNotify.item.cchTextMax	= 0;
			sNotify.item.pszText	= _T("");
			}

		sNotify.item.state		   |= pEntry->uState&TVIS_BASEFLAGS;
		}
	else{													// Haupteintrag
		sNotify.item.state			= pEntry->uState;
		sNotify.item.pszText		= pEntry->pText;
		sNotify.item.cchTextMax		= pEntry->uTextSize;
		}

	UNLOCK(pData);
	lRet=SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

	if(!lRet)return 0;


	if(wParam==VK_RETURN)
		{
		wParam = VK_DBLCLK;
		uBits  = 0;
		}
	else if(wParam==VK_EDITCLK)
		{
		if(pData->uStyleEx&TVS_EX_NOCURSORSET)
			{
			uBits = 0;
			}
		else{
			TreeListGetItemRect(pData,uItem,TVIR_COLTOSUB(uSub)|TVIR_GETCOLUMN|TVIR_TEXT,&sRect);
			uBits = TVIR_SETAT(LOWORD(lParam)-sRect.left);
			}
		}
	else{
		uBits = 0;
		}

	cChar  = (TCHAR)((lRet>>8)&0xFF);
	uBits |= U(lRet)&TVIR_EDITFULL;

	if(U(lRet)&TVIR_EDITCOMBOBOX)							// Eine Combobox anzeigen
		{
		uBits |= (U(lRet)&TVIR_EDITCOMBOLIST)? 0x40000000:0x20000000;
		}


		hWnd = TreeListEditLabel(pData,uItem,uSub|uBits);
	if(!hWnd)return 0;

	if(lRet&TVIR_EDITCOMBOBOX)								// Die Combobox f?len
		{
		uMax = sNotify.uMaxEntries;
		iSel = -1;

		if(sNotify.pTextList)								// Texte ?er Listenfeld
			{
			for(uCnt=0;uCnt<uMax;uCnt++)
				{
					pText = sNotify.pTextList[uCnt];
				if(!pText)break;

				SendMessage(hWnd,CB_ADDSTRING,1,(LPARAM)pText);

				if(sNotify.item.pszText && !_tcscmp(pText,sNotify.item.pszText))
					{
					iSel = uCnt;
					}
				}
			}
		else{												// Text mit
			pText = sNotify.pTextEntries;

			if(!pText)pText=_T("\0");

			for(uCnt=0;uCnt<uMax;uCnt++)
				{
				for(uLen=0;pText[uLen];uLen++)				// Subtextl?ge holen
					{
					if(pText[uLen]==cChar)break;
					}

				if(!uLen && !pText[uLen])break;

				   uNum = uLen;
				if(uNum>=sizeof(cText)/sizeof(TCHAR))
					{
					uNum=sizeof(cText)/sizeof(TCHAR)-1;
					}

				memcpy(cText,pText,uNum*sizeof(TCHAR));
				cText[uNum]=0;
				SendMessage(hWnd,CB_ADDSTRING,1,(LPARAM)cText);

				if(sNotify.item.pszText && wParam>=0x10000 && !_tcscmp(cText,sNotify.item.pszText))
					{
					iSel = uCnt;
					}

				pText += uLen;
				if(!cChar || *pText)pText++;
				}

			if(lRet&TVIR_EDITCOMBODEL)						// Den Puffer l?chen
				{
				delete (TCHAR*)sNotify.pTextEntries;
				}
			}

		if(iSel>=0)											// Listeneintrag ausw?len
			{
			SendMessage(hWnd,CB_SETCURSEL,iSel,0);
			}

		if(sNotify.uHeight)									// H?e der Dropdown-Liste einstellen
			{
			SendMessage(hWnd,CB_SETDROPPEDWIDTH,sNotify.uHeight,0);
			}

		if(lRet&TVIR_EDITCOMBODOWN)							// Dropdown-Liste einblenden
			{
			SendMessage(hWnd,CB_SHOWDROPDOWN,1,0);
			}

		if(!(lRet&TVIR_EDITCOMBOLIST))						// Textauswahl wiederherstellen
			{
			SetWindowText(hWnd,sNotify.item.pszText);
			SendMessage(hWnd,CB_SETEDITSEL,0,pData->uLastSel);
			}
		}


	if(wParam<0x10000)										// Taste an Fenster senden
		{
		pData->cColumnStart = 1;
		sMsg.hwnd			= hWnd;
		sMsg.lParam			= lParam;
		sMsg.wParam			= wParam;
		sMsg.message		= WM_KEYDOWN;

		TranslateMessage(&sMsg);
		}
	else if(wParam&VK_ISACHAR)
		{
		SendMessage(hWnd,WM_CHAR,wParam&0xFFFF,lParam);
		}
	else{
		pData->cColumnStart = 0;
		}



return 1;
}

//*****************************************************************************
//*
//*		TreeListHandleDragMove
//*
//*****************************************************************************
//	Verarbeitet die Drag-Move-Operation	wenn der Cursor verschoben wird.
//	pData		: Zeiger auf die Fensterdaten
//	wParam		: W-Parameter von WM_MOUSEMOVE
//	lParam		: L-Parameter von WM_MOUSEMOVE
static void TreeListHandleDragMove(TreeListData *pData,unsigned wParam,unsigned lParam)
{
TV_HITTESTINFO	sInfo;
BaseItem	   *pEntry;
HCURSOR			hCursor;
unsigned		uTarget;
unsigned		uHeight;
unsigned		uFlag;
unsigned		uItem;
unsigned		uStop;
unsigned		uMode;
unsigned		uAdd;



	pData->uMoveLastW	= wParam;
	pData->uMoveLastL	= lParam;

	if(!(wParam&MK_LBUTTON))								// Linke Taste nicht gedr?kt
		{
		pData->uMoveItem = 0;
		TreeListSetMarkLine(pData,0,MOVE_MARK);
		TreeListAutoScroll (pData,0,0);
		ReleaseCapture();
		return;
		}


	if((wParam&MK_CONTROL) && !(pData->uMoveFlags&TVDM_NOTOCHILDS))
		{
 		uHeight = pData->iRowHeight;
		uAdd	= 0;
		}
	else{
		uHeight = MOVE_MARK;
		uAdd	= pData->iRowHeight/2;
		}



	sInfo.pt.x	= GET_X_LPARAM(lParam);						// Linie verschieben
	sInfo.pt.y	= GET_Y_LPARAM(lParam)+uAdd;
	TreeListHitTest(pData,&sInfo);


	if(U(sInfo.hItem)==0)sInfo.hItem=TVI_LAST;
	if(U(sInfo.hItem)==pData->uLineItem && uHeight==pData->uLineHeight)
		{
		return;
		}

															// Nie g?tiges Ziel
	if(pData->uMoveItem>pData->uTreeItemsMax || !pData->pTreeItems[pData->uMoveItem])
		{
		TreeListSetMarkLine(pData,U(sInfo.hItem),uHeight);
		SetCursor(hCursorNo);
		return;
		}
															// Nicht im fixierten Breich
	if((pData->uMoveFlags&TVDM_NOFIXEDITEMS) && pData->uFixedLines)
		{
		uItem  = U(sInfo.hItem);

		if(uItem<=pData->uTreeItemsMax)
			{
			   pEntry = pData->pTreeItems[uItem];
			if(pEntry && pEntry->uShowPos<=pData->uFixedLines)
				{
				TreeListSetMarkLine(pData,U(sInfo.hItem),uHeight);
				SetCursor(hCursorNo);
				return;
				}
			}
		}


//******************** Einfachauswahl *****************************************


	if(pData->uSelectedCount<=1)
		{
		uStop = (pData->uSelectedCount)? pData->uMoveItem:pData->uSelectedItem;
		uItem = U(sInfo.hItem);

		if(uItem==U(TVI_LAST))uItem=0;


		while(uItem<=pData->uTreeItemsMax)					// Ziel pr?en
			{
			   pEntry = pData->pTreeItems[uItem];
			if(pEntry==0)break;
			if(pEntry->uParent==uStop)break;
			uItem = pEntry->uParent;
			}

		 if(uItem)											// Ist der Eintrag g?tig
			{
			hCursor = hCursorNo;
			}
		else{
			hCursor = (uItem)? hCursorNo:hCursorMove;
			}

		if(hCursor!=hCursorNo)								// Level und Elterneintrag pr?en
		if(pData->uMoveFlags&(TVDM_SAMELEVEL|TVDM_SAMEPARENT))
			{
			uMode = (uAdd)? 0:2;
			uItem = (pData->uSelectedCount)? pData->uSelectedItem:pData->uMoveItem;

			   uTarget= U(sInfo.hItem);
			if(uTarget==U(TVI_LAST))uTarget=0;

			if(TreeListMoveLevelCheck(pData,uItem,uTarget,uMode))
				{
				hCursor = hCursorNo;
				}
			}

		if(pData->uMoveFlags&TVDM_ONLYENDPOINTS)			// Nur Eintr?e ohne Kinder
		if(hCursor!=hCursorNo)
			{
			if(uItem>pData->uTreeItemsMax)
				{
				hCursor = hCursorNo;
				}
			else{
					pEntry = pData->pTreeItems[uItem];
				if(!pEntry || pEntry->uFirstChild)
					{
					hCursor = hCursorNo;
					}
				}
			}

		TreeListSetMarkLine(pData,U(sInfo.hItem),uHeight);
		SetCursor(hCursor);
		return;
		}


//******************** Mehrfachauswahl ****************************************


	uItem	= pData->uFirstChild;
	uMode	= (uAdd)? 0:2;
	hCursor	= hCursorNo;

	   uTarget= U(sInfo.hItem);
	if(uTarget==U(TVI_LAST))uTarget=0;


	for(;;)													// Alle ausgew?lten Eintr?e suchen
		{
		   pEntry = pData->pTreeItems[uItem];
		if(pEntry==0)break;
		if(pEntry->uState&TVIS_SELECTED)					// Ist der Eintrag ausgew?lt und nicht verschoben
			{
			uFlag = 1;

			if(pData->uMoveFlags&TVDM_ONLYENDPOINTS)		// Nur Eintr?e ohne Kinder
			if(pEntry->uFirstChild)
				{
				uFlag = 0;
				}
															// Level und Elterneintrag pr?en
			if(uFlag && (pData->uMoveFlags&(TVDM_SAMELEVEL|TVDM_SAMEPARENT)))
				{
				   uTarget= U(sInfo.hItem);
				if(uTarget==U(TVI_LAST))uTarget=0;

				if(TreeListMoveLevelCheck(pData,uItem,uTarget,uMode))
					{
					uFlag = 0;
					}
				}

			if(uFlag && TreeListMoveCheck(pData,uItem,uTarget,uMode))
				{
				hCursor = hCursorMove;
				break;
				}
			}
															// Zum n?hsten Kindeintrag
		if(pEntry->uFirstChild && pEntry->bDone==0)
			{
			uItem = pEntry->uFirstChild;
			continue;
			}

		if(pEntry->uNextItem)								// Zum Eintrag
			{
			uItem = pEntry->uNextItem;
			continue;
			}

		uItem = pEntry->uParent;

		while(uItem)										// Ebenen aufsteigen
			{
				pEntry = pData->pTreeItems[uItem];
			if(!pEntry->uNextItem)
				{
				uItem = pEntry->uParent;
				continue;
				}

			uItem = pEntry->uNextItem;
			break;
			}
		}


	TreeListSetMarkLine(pData,U(sInfo.hItem),uHeight);
	SetCursor(hCursor);




}

//*****************************************************************************
//*
//*		TreeListStartAutoEdit
//*
//*****************************************************************************
//	Startet die Autoeditierung f? eine Spalte
//	pData		: Zeiger auf die Fensterdaten
//	uColumn		: Ist die Nummer der Spalte
//	wParam		: Ist der W-Parameter des Tastendrucks
//					VK_EDITCLK	bei einem Clickauf das ausgew?lte Feld
//					VK_ICONCLK	bei einem Clickauf das Icon
//					VK_DBLCLK	bei einem Doppelclick
//	lParam		: Ist der L-Parameter des Tastendrucks (bzw. die Koordinaten)
//	Ergibt 1 das Editieren gestartet wurde, ansonsten 0
//				0 wenn der Eintrag nicht gew?lt wurde
static int TreeListStartAutoEdit(TreeListData *pData,unsigned uColumn,unsigned wParam,unsigned lParam)
{
TCHAR		cBuffer[256];
NMTREEVIEW	sNotify;
ExtraItem  *pExtra;
BaseItem   *pEntry;
LPTSTR	   *pList;
LPTSTR	    pText;
TV_ITEM		sItem;
RECT		sRect;
TCHAR		cChar;
unsigned	uMode;
unsigned	uMask;
unsigned	uBits;
unsigned	uFlag;
unsigned	uMax;
unsigned	uPos;
unsigned	uLen;
unsigned	uSub;
HWND		hWnd;
int			iSel;
int			iRet;
int			iIcon;


	if(  pData->uEditItem			  )return 0;
	if(  pData->uColumnCount<=uColumn )return 0;
	if(!(pData->uStyle&TVS_EDITLABELS))return 0;


	uBits = pData->aColumn[uColumn].bFlags;
	uMode = pData->aColumn[uColumn].bEdit;

	if(uMode==0)return 0;


	if(uBits&TVAE_STATEENABLE)									// Kann das Editieren gesperrt werden
		{
		if(uColumn)
			{
			    pExtra  =  pData->pExtraItems[uColumn-1][pData->uSelectedItem];
			if( pExtra && (pExtra->uState&TVIS_DISABLEBIT))return 0;
			}
		else{
				pEntry  =  pData->pTreeItems[pData->uSelectedItem];
			if(!pEntry || (pEntry->uState&TVIS_DISABLEBIT))return 0;
			}
		}


	if(wParam!=VK_RETURN && wParam<0x10000)						// Zeicheneingabe
		{
		if(!TVIS_EDIT(uMode))return 0;
		if(uBits&TVAE_ONLYRETURN)return 0;
		if(wParam<=' ')return 0;
		pData->cColumnStart = 1;
		}
	else{
		pData->cColumnStart = 0;
		}



		uMax=pData->aColumn[uColumn].bCbSize;
	if(!uMax)uMax=1024;


//******************** Weiterschalten mit Return ******************************


	if(uMode>=TVAX_STEP)
		{
		sItem.mask		 = TVIF_TEXT|TVIF_SUBITEM|TVIF_TEXTPTR|((uMode<TVAX_CHECK)? 0:TVIF_STATE|TVIF_PARAM);
		sItem.stateMask	 = 0xFFFFFFFF;
		sItem.hItem		 = (HTREEITEM)pData->uSelectedItem;
		sItem.cChildren	 = uColumn;

		if(!TreeListGetItem(pData,&sItem))return 0;

		if((uMode&1) && wParam!=VK_ICONCLK)						// Ein Edit-Feld ?fnen
			{
			uMode = TVAX_EDIT;
			goto EditField;
			}

		if(uMode>=TVAX_CHECK)									// State-Bits bei Checkboxen setzen
			{
			uMask	= U(~TVIS_STATEIMAGEMASK);

			if(uBits&TVAE_STATEENABLE)
				{
				uMask |= TVIS_DISABLEBIT;
				}

			if(pData->uStyleEx&TVS_EX_ALLOWITEMHIDE)
				{
				uMask |= TVIS_HIDEITEM;
				}

			if(pData->hChecks==THEMEIMGLIST && wParam==VK_ICONCLK)
				{
				pData->uCheckedDown = 1;
				}

			uFlag = sItem.state&uMask;

			if(pData->uStyleEx&TVS_EX_BITCHECKBOX)
					sItem.state	= (sItem.state^0x1000);
			else	sItem.state	= (sItem.state&0x1000)?	0x2000:0x1000;

			sItem.state |= uFlag;
			}
		else{
			sItem.mask  &= ~TVIF_STATE;
			}

		if(uBits&TVAE_PTRLIST)									// Zeigerliste  char *pTexte[]={"1","2",NULL};
			{
			pList = (LPTSTR*)pData->aColumn[uColumn].pCbData;

			if(!pList)
				{
				if(uMode<TVAX_CHECK)return 0;
				sItem.mask &= ~TVIF_TEXT;
				pText       =  sItem.pszText;
				uPos		=  0;
				goto NoTextChange;
				}

			for(uPos=0;;uPos++)
				{
				if(uPos>=uMax || !pList[uPos]){uPos=0;break;}
				if(str_cmp(sItem.pszText,pList[uPos]))continue;
				   uPos++;
				if(uPos>=uMax || !pList[uPos])uPos=0;
				break;
				}

				pText = pList[uPos];
			if(!pText)
				{
				if(uMode<TVAX_CHECK)return 0;
				sItem.mask &= ~TVIF_TEXT;
				pText       =  sItem.pszText;
				}
			}
		else{													// Textliste  char *pText="1|2|3";
			pText = (LPTSTR)pData->aColumn[uColumn].pCbData;
			cChar = (TCHAR )pData->aColumn[uColumn].bCbChar;
			sItem.cchTextMax--;

			if(!pText)
				{
				if(uMode<TVAX_CHECK)return 0;
				sItem.mask &= ~TVIF_TEXT;
				pText       =  sItem.pszText;
				uPos		=  0;
				goto NoTextChange;
				}

			for(uPos=0;;uPos++)
				{
				if(uPos>=uMax)
					{
					pText = (LPTSTR)pData->aColumn[uColumn].pCbData;
					uPos  = 0;
					break;
					}

				for(uLen=0;pText[uLen];uLen++)					// Subtextl?ge bestimmen
					{
					if(pText[uLen]==cChar)break;
					}

				if(sItem.cchTextMax==(int)uLen)					// Ist der Subtext der aktuelle Text
				if(str_ncmp(sItem.pszText,pText,uLen)==0)
					{
					uPos++;

					if(uPos>=uMax)
						{
						pText = (LPTSTR)pData->aColumn[uColumn].pCbData;
						uPos  = 0;
						break;
						}

					   pText+=uLen;
					if(pText[0]==cChar)pText++;
					if(pText[0]==0)
						{
						pText = (LPTSTR)pData->aColumn[uColumn].pCbData;
						uPos  = 0;
						}

					break;
					}

				   pText+=uLen;
				if(pText[0]==cChar)pText++;
				if(pText[0]==0)
					{
					pText = (LPTSTR)pData->aColumn[uColumn].pCbData;
					uPos  = 0;
					break;
					}

				}

			for(uLen=0;pText[uLen] && uLen<256;uLen++)			// Ausgew?lten Text kopiren
				{
				if(pText[uLen]==cChar)break;
				cBuffer[uLen] = pText[uLen];
				}

			cBuffer[uLen] = 0;
			pText = cBuffer;
			}


NoTextChange:

		sItem.mask		&= ~TVIF_TEXTPTR;
		sItem.stateMask	&=  TVIS_STATEIMAGEMASK;
		sItem.hItem		 =  (HTREEITEM)pData->uSelectedItem;
		sItem.pszText	 =  pText;
		sItem.cchTextMax =  256;
		sItem.cChildren	 =  uColumn;
		iIcon			 =  pData->aColumn[uColumn].iCbIcon;

		if(iIcon>=0)											// Auch ein Icon zuweisen
			{
			sItem.mask	|= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			sItem.iImage			= iIcon+uPos;
			sItem.iSelectedImage	= sItem.iImage;
			}

		iRet = TreeListSetItem(pData,&sItem);

		sNotify.hdr.code			= (uMode<TVAX_CHECK)? TVN_STEPSTATECHANGED:TVN_CBSTATECHANGED;
		sNotify.action				= (UINT)wParam;
		sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT|TVIF_SUBITEM;
		sNotify.itemNew.stateMask	= 0xFFFFFFFF;
		sNotify.itemNew.hItem		= sItem.hItem;
		sNotify.itemNew.state		= sItem.state;
		sNotify.itemNew.lParam		= sItem.lParam;
		sNotify.itemNew.pszText		= sItem.pszText;
		sNotify.itemNew.cchTextMax	= sItem.cchTextMax;
		sNotify.itemNew.cChildren	= uColumn;
		sNotify.itemOld.mask		= 0;
		sNotify.ptDrag.x			= GET_X_LPARAM(lParam);
		sNotify.ptDrag.y			= GET_Y_LPARAM(lParam);

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);
		LOCK(pData);

		return iRet;
		}


//******************** Edit oder Combobox *************************************

EditField:

	uSub	 = (uMode-1)<<29;
	uSub    |=  uColumn;

	if(uBits&TVAE_FULLWIDTH)
		{
		uSub |= TVIR_EDITFULL;
		}


	if(wParam==VK_EDITCLK)										// Cursor auf Klickposition
	if(!(pData->uStyleEx&TVS_EX_NOCURSORSET))
		{
		TreeListGetItemRect(pData,pData->uSelectedItem,TVIR_COLTOSUB(uColumn)|TVIR_GETCOLUMN|TVIR_TEXT,&sRect);
		uSub |= TVIR_SETAT(LOWORD(lParam)-sRect.left);
		}

		hWnd = TreeListEditLabel(pData,pData->uSelectedItem,uSub);
	if(!hWnd)return 0;


	if(uMode!=TVAX_EDIT)
		{
		iSel			 = (uMode!=TVAX_CBLIST)? -1:0;
		sItem.mask		 = TVIF_TEXT|TVIF_SUBITEM|TVIF_TEXTPTR;
		sItem.hItem		 = (HTREEITEM)pData->uSelectedItem;
		sItem.cChildren	 = uColumn;

		if(!TreeListGetItem(pData,&sItem))return 0;


		if(uBits&TVAE_PTRLIST)									// Zeigerliste  char *pTexte[]={"1","2",NULL};
			{
			pList = (LPTSTR*)pData->aColumn[uColumn].pCbData;

			for(uPos=0;uPos<uMax;uPos++)
				{
				if(!pList[uPos])break;
				if(!str_cmp(sItem.pszText,pList[uPos]))iSel=uPos;
				SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)pList[uPos]);
				}
			}
		else{													// Textliste  char *pText="1|2|3";
			pText = (LPTSTR)pData->aColumn[uColumn].pCbData;
			cChar = (TCHAR )pData->aColumn[uColumn].bCbChar;
			sItem.cchTextMax--;

			for(uPos=0;uPos<uMax;uPos++)
				{
				for(uLen=0;pText[uLen];uLen++)
					{
					if(pText[uLen]==cChar)break;
					}

				if(sItem.cchTextMax==(int)uLen)
				if(str_ncmp(sItem.pszText,pText,uLen)==0)
					{
					iSel = uPos;
					}

				if(cChar)
					{
					if(uLen<256){memcpy(cBuffer,pText,sizeof(TCHAR)*uLen);cBuffer[uLen]=0;}
					else		{memcpy(cBuffer,pText,sizeof(TCHAR)*255 );cBuffer[255 ]=0;}
					SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)cBuffer);
					}
				else{
					SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)pText);
					}

				   pText+=uLen;
				if(pText[0]==cChar)pText++;
				if(pText[0]==0)break;
				}
			}

		if(iSel>=0)
			{
			SendMessage(hWnd,CB_SETCURSEL,iSel,0);
			}
		}



	if((uBits&TVAE_DROPDOWN) && (uMode&~1)==2)					// Dropdownliste aufklappen
		{
		SendMessage(hWnd,CB_SHOWDROPDOWN,1,0);
		}

																// Ersten Buchstaben ans Fenster ?ergeben
	if(TVIS_EDIT(uMode) && pData->cColumnStart && wParam!=VK_EDITCLK)
		{
		SendMessage(hWnd,WM_CHAR,wParam,0);
		}




return 1;
}

//*****************************************************************************
//*
//*		TreeListProc
//*
//*****************************************************************************
//	Ist die Fensterfunktion f? das TreeList Fenster
static LRESULT CALLBACK TreeListProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
BaseItem	   *pEntry;
TreeListData   *pData;
MSG			   *pMsg;
TV_HITTESTINFO	sInfo;
POINT			sPoint;
SCROLLINFO		sScroll;
unsigned		uChange;
unsigned		uDelta;
unsigned		uTime;
unsigned		uFlag;
unsigned		uMode;
unsigned		uVal;
unsigned		uOld;
unsigned		uSub;
LPARAM			lRet;
int 			iDif;
int				iPos;
int				iMax;
HDC				hDc;




	switch(uMsg)
		{
	case WM_CREATE:												// Das Fenster erzeugen

				pData= new TreeListData;
			if(!pData)return -1;

			memset(pData,0,sizeof(TreeListData));
			pData->pToolTipText = new TCHAR[256];
			pData->uToolTipSize = 256;

			if(!pData->pToolTipText)
				{
				delete pData;
				return -1;
				}

				pData->pTreeItems = new BaseItem*[1];
			if(!pData->pTreeItems)
				{
				delete pData->pToolTipText;
				delete pData;
				return -1;
				}

				pData->pItemPos  = new unsigned[1];
			if(!pData->pItemPos)
				{
				delete pData->pToolTipText;
				delete pData->pTreeItems;
				delete pData;
				return -1;
				}

			GlobalInit();

			pData->pItemPos  [0] = 0;
			pData->pTreeItems[0] = NULL;

			SYNCH_CREATE(pData);
			SetWindowLongPtr(hWnd,0,(LONG_PTR)pData);
			pData->iIndent		 = DEFAULT_IDENT;
			pData->iShift		 = DEFAULT_SHIFT;
			pData->uStyle		 = GetWindowLong(hWnd,GWL_STYLE);
			pData->hWnd			 = hWnd;
			pData->cIsEnabled	 = (char)IsWindowEnabled(hWnd);
			pData->iAutoAdd		 = 1;
			pData->aColumnPos[0] = 0;
			pData->aColumnPos[1] = 1;
			pData->aColumn[0].bNext = 1;

			if(!(pData->uStyle&(TVS_HASBUTTONS|TVS_HASLINES)))
				{
				pData->cHasRootRow = 0;
				}
			else{
				pData->cHasRootRow = (char)((pData->uStyle&TVS_LINESATROOT)? 1:0);
				}

			if(!(pData->uStyle&TVS_NOTOOLTIPS))
				{
				CreateToolTip(pData);
				}

			UpdateFont   (pData);
			UpdateHeight (pData);
			UpdateColors (pData);
			UpdateScrollY(pData,FALSE);

			if(pOpenThemeData)									// Soll ein Thema angezeigt werden
					pData->hTheme = pOpenThemeData(hWnd,L"TREEVIEW");
			else	pData->hTheme = NULL;

			pData->cGlyphOk = (char)((pData->hTheme)? 1:0);

			if(pData->uStyle&TVS_CHECKBOXES)
				{
				CreateStateImageList(pData,0);
				}

			return 0;

	case WM_DESTROY:											// Das Fenster zerst?en

			pData = GetHandle(hWnd);

			LOCK(pData);

			TreeListDeleteItem(pData,U(TVI_ROOT),0);
			if(pData->hStates==THEMEIMGLIST)pData->hStates=0;
			if(pData->hChecks==THEMEIMGLIST)pData->hChecks=0;
			if(pData->hFontX  ){DeleteObject     (pData->hFontX  );pData->hFontX  =0;}
			if(pData->hEdit   ){DestroyWindow    (pData->hEdit   );pData->hEdit   =0;}
			if(pData->hHeader ){DestroyWindow    (pData->hHeader );pData->hHeader =0;}
			if(pData->hToolTip){DestroyWindow    (pData->hToolTip);pData->hToolTip=0;}
			if(pData->hStates ){ImageList_Destroy(pData->hStates );pData->hStates =0;}

			if(pData->uStyleEx&TVS_EX_SHAREIMAGELISTS)
				{
				if(pData->hStates && pData->iStatesMode){ImageList_Destroy(pData->hStates);pData->hStates=0;}
				if(pData->hChecks && pData->iChecksMode){ImageList_Destroy(pData->hChecks);pData->hChecks=0;}
				pData->hImages = 0;
				pData->hSubImg = 0;
				}
			else{
				if(pData->hStates){ImageList_Destroy(pData->hStates);pData->hStates=0;}
				if(pData->hChecks){ImageList_Destroy(pData->hChecks);pData->hChecks=0;}
				if(pData->hImages){ImageList_Destroy(pData->hImages);pData->hImages=0;}
				if(pData->hSubImg){ImageList_Destroy(pData->hSubImg);pData->hSubImg=0;}
				}

			if(pData->hThemeBt){pCloseThemeData(pData->hThemeBt);pData->hThemeBt=0;}
			if(pData->hTheme  ){pCloseThemeData(pData->hTheme  );pData->hTheme  =0;}

			for(uVal=1;uVal<pData->uColumnCount;uVal++)
				{
				delete pData->pExtraItems[uVal-1];
				pData->pExtraItems[uVal-1] = 0;
				}

			if(pData->uTempSize)
				{
				delete pData->pTempPos;
				delete pData->pTempPtr;
				}

			pData->uColumnCount = 0;

			UNLOCK(pData);

			return 0;

	case WM_NCDESTROY:											// Das Fenster zerst?en

			pData = GetHandle(hWnd);

			LOCK(pData);

			delete pData->pToolTipText;
			delete pData->pTreeItems;
			delete pData->pItemPos;

			UNLOCK(pData);

			SetWindowLongPtr(hWnd,0,0);
			SYNCH_DELETE(pData);
			memset(pData,0,sizeof(TreeListData));
			delete pData;

			GlobalDeinit();


			return 0;

	case WM_SHOWWINDOW:											// Fenster ein/ausblenden

			if(wParam)
				{
				pData = GetHandle(hWnd);

				sScroll.cbSize		= sizeof(SCROLLINFO);
				sScroll.fMask		= SIF_ALL;
				sScroll.nMin		= 0;
				sScroll.nMax  		= 0;
				sScroll.nPage 		= 0;
				sScroll.nPos  		= 0;
				sScroll.nTrackPos	= 0;

				SetScrollInfo(pData->hWnd,SB_HORZ,&sScroll,TRUE);

				if(pData->uStyleEx&TVS_EX_ALWAYSSCROLLY)		// Y-Scrollbar immer einblenden
					{
					sScroll.fMask  |= SIF_DISABLENOSCROLL;
					}

				SetScrollInfo(pData->hWnd,SB_VERT,&sScroll,TRUE);

				pData->uOldCountX	= 0xFFFF;
				pData->uOldCountY	= 0xFFFF;

				UpdateScrollY(pData,FALSE);
				UpdateScrollX(pData,FALSE);
				}

			return 0;

	case WM_SIZE:												// Die Fenstergröße wurde ver?dert

			pData = GetHandle(hWnd);
			uFlag = 0;

			LOCK(pData);

			   uVal = LOWORD(lParam);
			if(uVal && uVal!=pData->uSizeX)
				{
				uOld			= pData->uSizeX;
				pData->uSizeX	= uVal;


				if(pData->uColumnCountVar)						// Spalten mit variabler Breite nach f?ren
					{
					RECT	sRect;
					int		iDelta;
					int		iNum;

					iDelta = uVal-uOld;

					ChangeColSize(pData,iDelta);

					iNum = UpdateColumns(pData);
					GetClientRect(hWnd,&sRect);
					sRect.left  = iNum;
					sRect.left -= pData->uScrollX;
					sRect.top   = pData->uStartPixel;
					InvalidateRect(hWnd,&sRect,FALSE);
					}

				if(uVal>uOld)									// Hat sich die Breite vergrößert
					{
					RECT	sRect;

					GetClientRect (hWnd,&sRect);
					sRect.right  = uVal;
					sRect.left   = uOld;
					InvalidateRect(hWnd,&sRect,FALSE);
					}

				pData->aColumnXpos[pData->uColumnCount+1] = uVal+1;

				MoveWindow(pData->hHeader,-(int)pData->uScrollX,0,uVal+pData->uScrollX,pData->uStartPixel,uVal>uOld);
				UpdateScrollX(pData);
				}

			   uVal = HIWORD(lParam);
			if(uVal && uVal!=pData->uSizeY)
				{
				if(!pData->cScrollX && uVal<pData->uSizeY)		// BUG Windows wenn X-Schrollbar initialisiert wird
					{
					RECT	sRect;

					GetClientRect (hWnd,&sRect);
					uVal = sRect.bottom;
					}

				if(uVal>pData->uSizeY)							// Hat sich die H?e vergrößert
					{
					RECT	sRect;

					GetClientRect (hWnd,&sRect);
					uFlag			 = 1;
					sRect.bottom	 = uVal;
					sRect.top		 = pData->uSizeY;
					InvalidateRect(hWnd,&sRect,FALSE);
					}
				else{
					uFlag			 = 0;
					}

				pData->uSizeY		 =  uVal;
				pData->uSizeYsub	 = (uVal<=pData->uStartPixel)? 0:uVal-pData->uStartPixel;

				if(pData->uSizeY>pData->uStartPixel)
					{
					pData->uMaxEnties  = pData->uSizeY;
					pData->uMaxEnties -= pData->uStartPixel;
					}
				else{
					pData->uMaxEnties  = 0;
					}

				pData->uPageEnties	 = pData->uMaxEnties;
				pData->uMaxEnties	+= pData->iRowHeight-1;
				pData->uMaxEnties   /= pData->iRowHeight;
				pData->uPageEnties  /= pData->iRowHeight;
																// Wenn H?e vergrößert dann Scroll-Position pr?en
				if(uFlag && pData->uPageEnties>2 && pData->uScrollY>0)
					{
					if(pData->uScrollY+pData->uPageEnties+1>pData->uItemPosCount)
						{
						   iPos = pData->uItemPosCount-pData->uPageEnties+1;
						if(iPos<0)iPos=0;
						if(U(iPos)!=pData->uScrollY)
							{
							pData->uScrollY = iPos;
							UpdateView(pData);
							}
						}
					}
				UpdateScrollY(pData);
				}


			UNLOCK(pData);

			if(uFlag)
				{
				RedrawWindow(pData->hHeader,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
				}

			return 0;

	case WM_ENABLE:												// Das Fenster feischalten oder sperren

			   pData = GetHandle(hWnd);
			if(pData->cIsEnabled!=((U(wParam))? 1:0))
				{
				uVal  = GetWindowLong(hWnd,GWL_STYLE);
				uVal &= (0xFFFF0000&~WS_DISABLED);
				uVal |= (0x0000FFFF| WS_DISABLED)&(pData->uStyle^WS_DISABLED);
				SetWindowLong(hWnd,GWL_STYLE,uVal);
				}

			return 0;

	case WM_SETFOCUS:											// Das Fenster bekommt den Focus

				pData = GetHandle(hWnd);
			if(!pData->cHasFocus)
				{
				NMHDR	sNotify;

				LOCK(pData);
				pData->cHasFocus=1;

				if(pData->uSelectedCount<=1)
					{
					if(pData->uSelectedItem)
						{
						if(pData->uStyleEx&TVS_EX_FULLROWMARK)
								UpdateRow (pData,pData->uSelectedItem);
						else	UpdateRect(pData,pData->uSelectedItem,pData->uSelectedSub);
						}
					}
				else{
					UpdateView(pData);
					}

				UNLOCK(pData);

				sNotify.code = NM_SETFOCUS;
				SendNotify(pData,&sNotify);
				}

			return 0;

	case WM_KILLFOCUS:											// Das Fenster bekommt den Focus

			   pData = GetHandle(hWnd);
			if(pData->cHasFocus)
				{
				NMHDR	sNotify;

				LOCK(pData);
				pData->cHasFocus=0;

				if(pData->uSelectedCount<=1)
					{
					if(pData->uSelectedItem)
						{
						if(pData->uStyleEx&TVS_EX_FULLROWMARK)
								UpdateRow (pData,pData->uSelectedItem);
						else	UpdateRect(pData,pData->uSelectedItem,pData->uSelectedSub);
						}
					}
				else{
					UpdateView(pData);
					}

				UNLOCK(pData);

				sNotify.code = NM_KILLFOCUS;
				SendNotify(pData,&sNotify);
				}

			return 0;

	case WM_MOUSELEAVE:											// Die Maus hat das Fenster veralssen

			GetCursorPos(&sPoint);
			ScreenToClient(hWnd,&sPoint);
			lParam = MAKELONG(sPoint.x,sPoint.y);
			wParam = 0x00000000;

	case WM_MOUSEMOVE:											// Gab es eine Mausbewegung

			pData = GetHandle(hWnd);

			LOCK(pData);

			sInfo.hItem = 0;
			sInfo.flags = 0x00800000;
			sInfo.pt.x	= GET_X_LPARAM(lParam);

			if(!(pData->uStyle&TVS_NOTOOLTIPS))
				{
				if(pData->cIsEnabled)							// Ist das Fenster freigegeben
					{
					sInfo.pt.y	= GET_Y_LPARAM(lParam);
					TreeListHitTest(pData,&sInfo);
					UpdateToolTip(pData,U(sInfo.hItem),sInfo.flags);
					}
				else if(pData->uToolTipItem)
					{
					UpdateToolTip(pData,0,0);
					}
				}

			if(pData->uMoveItem)								// Auto-Drag-Move aktiv
				{
				TreeListHandleDragMove(pData,U(wParam),U(lParam));
				}
			else if(pData->uStyle&TVS_TRACKSELECT)				// Ist Track-Select aktiviert
				{
				if(pData->cIsEnabled)							// Ist das Fenster freigegeben
					{
					if(sInfo.flags==0x00800000)
						{
						sInfo.pt.y	= GET_Y_LPARAM(lParam);
						TreeListHitTest(pData,&sInfo);
						}

					if(sInfo.hItem && (sInfo.flags&(TVHT_ONSUBITEM|TVHT_ONITEM)))
						{
						TreeListSetTrackItem(pData,U(sInfo.hItem),TVHT_SUBTOCOL(sInfo.flags));
						}
					else if(pData->uTrackedItem)
						{
						TreeListSetTrackItem(pData,0,0);
						}
					}
				else if(pData->uTrackedItem)
					{
					TreeListSetTrackItem(pData,0,0);
					}
				}

			if(pData->hStates==THEMEIMGLIST)					// Mit Thema zeichnen
			if(pData->uStyle&TVS_CHECKBOXES)					// Ist Cursor ?er Checkbox
				{
				if(sInfo.flags==0x00800000)
					{
					sInfo.pt.y	= GET_Y_LPARAM(lParam);
					TreeListHitTest(pData,&sInfo);
					}

				if(sInfo.hItem && (sInfo.flags&TVHT_ONITEMSTATEICON))
					{
					if(U(sInfo.hItem)!=pData->uCheckedItem || pData->uCheckedSub)
						{
						if(!pData->uCheckedItem)
							{
							TRACKMOUSEEVENT sEvent;

							sEvent.cbSize		= sizeof(TRACKMOUSEEVENT);
							sEvent.dwFlags		= TME_LEAVE;
							sEvent.hwndTrack	= pData->hWnd;

							_TrackMouseEvent(&sEvent);
							}

						UpdateRect(pData,pData->uCheckedItem,pData->uCheckedSub);
						pData->uCheckedDown = U(wParam)&MK_LBUTTON;
						pData->uCheckedItem = U(sInfo.hItem);
						pData->uCheckedSub	= 0;
						UpdateRect(pData,U(sInfo.hItem),0);
						}
					}
				else{
					if(pData->uCheckedItem)
						{
						UpdateRect(pData,pData->uCheckedItem,pData->uCheckedSub);
						pData->uCheckedDown = 0;
						pData->uCheckedItem = 0;
						pData->uCheckedSub	= 0;
						}
					}
				}

			if(pData->hChecks==THEMEIMGLIST)					// Checkboxen in Unterspalten
				{
				uSub  = pData->aColumnPos[1];
				iPos  = pData->aColumnXpos[uSub];
				uFlag = 0;

				if(sInfo.pt.x>=iPos)							// Cursor auf Extraeintrag
					{
					if(sInfo.flags==0x00800000)
						{
						sInfo.pt.y	= GET_Y_LPARAM(lParam);
						TreeListHitTest(pData,&sInfo);
						}
																// Checkboxen statt Icons
					if(sInfo.hItem && (sInfo.flags&TVHT_ONSUBICON))
						{
						uSub = TVHT_SUBTOCOL(sInfo.flags);

						if(pData->aColumn[uSub].bEdit>=TVAX_CHECK)
							{
							uFlag = 1;

							if(U(sInfo.hItem)!=pData->uCheckedItem || pData->uCheckedSub!=uSub)
								{
								if(!pData->uCheckedItem)
									{
									TRACKMOUSEEVENT sEvent;

									sEvent.cbSize		= sizeof(TRACKMOUSEEVENT);
									sEvent.dwFlags		= TME_LEAVE;
									sEvent.hwndTrack	= pData->hWnd;

									_TrackMouseEvent(&sEvent);
									}

								UpdateRect(pData,pData->uCheckedItem,pData->uCheckedSub);
								pData->uCheckedDown = U(wParam)&MK_LBUTTON;
								pData->uCheckedItem = U(sInfo.hItem);
								pData->uCheckedSub	= uSub;
								UpdateRect(pData,U(sInfo.hItem),uSub);
								}
							}
						}
					}

				if(!uFlag && pData->uCheckedItem)
					{
					UpdateRect(pData,pData->uCheckedItem,pData->uCheckedSub);
					pData->uCheckedDown = 0;
					pData->uCheckedItem = 0;
					pData->uCheckedSub	= 0;
					}
				}

			if(  wParam & pData->uDragFlags)					// Drag beginnen
			if(U(lParam)!=pData->uLastMove )
				{
				NMTREEVIEW	sNotify;
				int			iDiffX;
				int			iDiffY;

				iDiffX = (short)(lParam    )-(short)(pData->uLastMove    );
				iDiffY = (short)(lParam>>16)-(short)(pData->uLastMove>>16);

				if(iDiffX<0)iDiffX=-iDiffX;
				if(iDiffY<0)iDiffY=-iDiffY;

				if(iDiffX<=2 && iDiffY<=2)						// Mehr als zwei Pixel-Verschub
					{
					UNLOCK(pData);
					return 0;
					}

				if(pData->uDragItem>pData->uTreeItemsMax)
					{
					UNLOCK(pData);
					return 0;
					}

					pEntry = pData->pTreeItems[pData->uDragItem];
				if(!pEntry)
					{
					UNLOCK(pData);
					return 0;
					}

				sNotify.hdr.code			= TVN_BEGINDRAG;
				sNotify.action				= 0;
				sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBNUMBER;
				sNotify.itemNew.hItem		= (HTREEITEM)pData->uDragItem;
				sNotify.itemNew.stateMask	= 0xFFFFFFFF;
				sNotify.itemNew.state		= pEntry->uState;
				sNotify.itemNew.lParam		= pEntry->lParam;
				sNotify.itemNew.cChildren	= pData->uDragSub;
				sNotify.itemNew.pszText		= (LPTSTR)-1;
				sNotify.itemNew.cchTextMax	= -1;
				sNotify.itemOld.mask		= 0;
				sNotify.ptDrag.x			= GET_X_LPARAM(lParam);
				sNotify.ptDrag.y			= GET_Y_LPARAM(lParam);
				pData->uDragFlags			= 0;
				pData->cClickFlag			= 0;
				pData->cClickEdit			= 0;

				   uMode = pData->uStyle&TVS_AUTODRAGMOVE;
				if(uMode)										// Keine Auto-Drag-Move Operation
					{
					if(pEntry->uState&pData->uNoDrag)
						{
						uMode = 0;
						}

					if((pData->uMoveFlags&TVDM_NOFIXEDITEMS) && pData->uFixedLines>=pEntry->uShowPos)
						{
						uMode = 0;
						}

					if((pData->uMoveFlags&TVDM_ONLYFIRSTLEVEL) && pEntry->uLevel!=0)
						{
						uMode = 0;
						}
					}

				UNLOCK(pData);
				lRet=SendNotify(pData,&sNotify.hdr);
				LOCK(pData);

				if(uMode && lRet!=1)							// Starte Auto-Drag-Move Operation
					{
					pData->uMoveItem = (lRet)? U(TVI_ROOT):pData->uDragItem;
					SetCapture(hWnd);

					if(pData->uTrackedItem)
						{
						TreeListSetTrackItem(pData,0,0);
						}

					TreeListHandleDragMove(pData,U(wParam),U(lParam));
					TreeListAutoScroll(pData,0,100);
					}
				}

			pData->uLastMove = U(lParam);

			UNLOCK(pData);

			return 0;

	case WM_LBUTTONUP:											// Linken Mausklick aufheben

			pData = GetHandle(hWnd);
			pData->uDragFlags &= ~MK_LBUTTON;

			if(pData->uMoveItem)
				{
				LOCK(pData);
				ReleaseCapture();
				TreeListMoveSelection(pData,pData->uLineItem,pData->uMoveItem,U(wParam));
				TreeListSetMarkLine(pData,0,0);
				TreeListAutoScroll (pData,0,0);
				SetCursor(hCursorArrow);
				pData->uMoveItem = 0;
				UNLOCK(pData);
				}
			else if(pData->cClickFlag || pData->cClickEdit)
				{
				LOCK(pData);
				TreeListMouseClick(pData,uMsg,wParam,lParam);
				pData->cClickFlag = 0;
				pData->cClickEdit = 0;
				}
			else{
				TreeListMouseNotify(pData,TVN_LBUTTONUP,wParam,lParam);
				}

			if(pData->uCheckedDown)								// Checkbox neu zeichnen
				{
				pData->uCheckedDown = 0;

				if(pData->uCheckedItem)
					{
					UpdateRect(pData,pData->uCheckedItem,pData->uCheckedSub);
					}
				}

			return 0;


	case WM_RBUTTONUP:											// Linken Mausklick aufheben

			pData = GetHandle(hWnd);
			pData->uDragFlags  &= ~MK_RBUTTON;

			TreeListMouseNotify(pData,TVN_RBUTTONUP,wParam,lParam);

			break;

	case WM_LBUTTONDOWN:										// Mausklick

			pData = GetHandle(hWnd);

			LOCK(pData);

			uTime = GetTickCount();

			if(pData->cButtonFlag && pData->uToolTipItem)		// Doppelklick simulieren ?er ToolTip
				{
				pData->cButtonFlag = 0;
				uDelta = uTime-pData->uButtonLast;

				if(uDelta<700)
					{
					iPos  = abs(GET_X_LPARAM(lParam)-GET_X_LPARAM(pData->uButtonPos));
					iPos += abs(GET_Y_LPARAM(lParam)-GET_Y_LPARAM(pData->uButtonPos));

					if(iPos<=6)
						{
						TreeListMouseClick(pData,WM_LBUTTONDBLCLK,wParam,lParam);
						return 0;
						}
					}
				}

			pData->cButtonFlag = 1;
			pData->uButtonLast = uTime;
			pData->uButtonPos  = U(lParam);

			TreeListMouseClick(pData,uMsg,wParam,lParam);

			return 0;

	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:

			pData	= GetHandle(hWnd);

			LOCK(pData);

			pData->cButtonFlag = 0;
			TreeListMouseClick(pData,uMsg,wParam,lParam);

			return 0;

	case WM_KEYDOWN:											// Tastendruck

			pData	= GetHandle(hWnd);
			LOCK(pData);
			TreeListKeyDown(pData,wParam,lParam);

			return 0;

	case WM_KEYUP:												// Tastendruck

			if(wParam==VK_CONTROL)
				{
				pData	= GetHandle(hWnd);

				LOCK(pData);

				if(pData->uMoveItem && pData->uLineHeight>=5)	// Ist Auto-Move aktif
					{
					TreeListHandleDragMove(pData,pData->uMoveLastW&~MK_CONTROL,pData->uMoveLastL);
					}

				UNLOCK(pData);
				}

			DefWindowProc(hWnd,uMsg,wParam,lParam);
			return 1;

	case WM_CHAR:												// Zeicheneingabe

			pData	= GetHandle(hWnd);
			iMax	= lParam&0xFFFF;

			LOCK(pData);

			for(iPos=0;iPos<iMax;iPos++)
				{
				TreeListChar(pData,(UINT)wParam,lParam);
				}

			UNLOCK(pData);

			return 0;


	case WM_DRAWITEM:											// Weietleiten von OwnerDraw des Headers

			if(wParam==1)
				{
				pData = GetHandle(hWnd);
				return SendMessage(GetParent(hWnd),uMsg,GetWindowLong(hWnd,GWL_ID),lParam);
				}

			return 0;

	case WM_MOUSEWHEEL:											// Scrollen mit dem Mausrad

			if(!(LOWORD(wParam)&(MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
				{
				pData = GetHandle(hWnd);

				if(pData->uEditItem)return 0;

				iDif  =	(short)HIWORD(wParam)/(WHEEL_DELTA/2);

				if(wParam&MK_CONTROL)
					{
					iDif *= 9;
					}
				else if(wParam&MK_SHIFT)
					{
					iDif *= 3;
					}

				iPos  = pData->uScrollY-iDif;
				iMax  = pData->uItemPosCount;
				iMax -=	pData->uPageEnties-1;

				if(iPos>=iMax)iPos=iMax;
				if(iPos<0)iPos=0;
				if(iPos!=(int)pData->uScrollY)
				if(!(pData->uStyle&TVS_NOSCROLL))
					{
					pData->uScrollY = iPos;
					SetScrollPos(hWnd,SB_VERT,iPos,TRUE);
					UpdateView(pData);
					}

				return 1;
				}

			return 0;


	case WM_HSCROLL:											// Vertikalles scrollen

			pData	= GetHandle(hWnd);
			iPos	= pData->uScrollX;

			if(pData->uEditItem)return 0;

			switch(LOWORD(wParam))
				{
			case SB_LINEDOWN:		iPos+=16;				break;
			case SB_LINEUP:			iPos-=16;				break;
			case SB_PAGEDOWN:		iPos+=pData->uSizeX;	break;
			case SB_PAGEUP:			iPos-=pData->uSizeX;	break;
			case SB_THUMBPOSITION:	iPos =HIWORD(wParam);  	break;
			case SB_THUMBTRACK:		iPos =HIWORD(wParam);  	break;
				}

			   uVal = pData->uColumnCount;
			if(uVal)iMax  = pData->aColumnXpos[uVal]-pData->uSizeX/2;
			else	iMax  = pData->iMaxSizeX;
					iMax -=	pData->uSizeX/2;

			if(iPos>=iMax)iPos=iMax;
			if(iPos<0)iPos=0;
			if(iPos!=(int)pData->uScrollX)
				{
				pData->uScrollX = iPos;
				SetScrollPos(hWnd,SB_HORZ,iPos,TRUE);
				UpdateView(pData);

				if(pData->hHeader)
					{
					MoveWindow(pData->hHeader,-iPos,0,pData->uSizeX+iPos,pData->uStartPixel,TRUE);
					}
				}

			return 0;

	case WM_VSCROLL:											// Vertikalles scrollen

			pData	= GetHandle(hWnd);
			iPos	= pData->uScrollY;

			if(pData->uEditItem)return 0;

			switch(LOWORD(wParam))
				{
			case SB_LINEDOWN:		iPos++;													break;
			case SB_LINEUP:			iPos--;													break;
			case SB_PAGEDOWN:		iPos+=(pData->uPageEnties>1)? pData->uPageEnties-1:1;	break;
			case SB_PAGEUP:			iPos-=(pData->uPageEnties>1)? pData->uPageEnties-1:1;	break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:		if(pData->uItemPosCount<0x7F00)
										{
										iPos = HIWORD(wParam);
										}
									else{
										sScroll.cbSize = sizeof(SCROLLINFO);
										sScroll.fMask  = SIF_TRACKPOS;
										GetScrollInfo(hWnd,SB_VERT,&sScroll);
										iPos = sScroll.nTrackPos;
										}
				}

			iMax  = pData->uItemPosCount;
			iMax -=	pData->uPageEnties-1;

			if(iPos>=iMax)iPos=iMax;
			if(iPos<0)iPos=0;
			if(iPos!=(int)pData->uScrollY)
				{
				pData->uScrollY = iPos;
				SetScrollPos(hWnd,SB_VERT,iPos,TRUE);
				UpdateView(pData);
				}

			return 0;

	case WM_SYSCOLORCHANGE:										// Wurden die Systemfarben ver?dert

			pData	= GetHandle(hWnd);

			LOCK(pData);

			if(pData->hTheme)
				{
				pCloseThemeData(pData->hTheme);
				pData->hTheme   = NULL;
				}

			if(pData->hThemeBt)
				{
				pCloseThemeData(pData->hThemeBt);
				pData->hThemeBt = NULL;
				}

			if(pOpenThemeData)
				{
				pData->hTheme   = pOpenThemeData(hWnd,L"TREEVIEW");
				pData->cGlyphOk = (char)((pData->hTheme)? 1:0);
				}

			if(pData->iStatesMode)CreateStateImageList(pData,0);
			if(pData->iChecksMode)CreateStateImageList(pData,1);

			if(pData->hHeader)SendMessage(pData->hHeader,WM_SYSCOLORCHANGE,0,0);
			UpdateColors(pData);
			UNLOCK(pData);

			return 0;

	case WM_GETFONT:											// Aktuellen Font abfragen

			pData	= GetHandle(hWnd);
			return (LRESULT)pData->hFontN;

	case WM_SETFONT:											// Einen neuen Font zuweisen

			pData	= GetHandle(hWnd);

			LOCK(pData);

			pData->hFontN=(HFONT)wParam;
			if(UpdateFont(pData))UpdateView(pData);

			UNLOCK(pData);

			return 0;

	case WM_STYLECHANGED:										// Hat sich der Fenstersytle ge?dert

			if(wParam==GWL_STYLE)
				{
				pData	= GetHandle(hWnd);
				lParam	= ((STYLESTRUCT*)lParam)->styleNew;

				LOCK(pData);
				uChange		   = U(lParam)^pData->uStyle;
				pData->uStyle  = U(lParam);

				if(uChange&(TVS_CHECKBOXES|TVS_NONEVENHEIGHT))
					{
					if(lParam&TVS_CHECKBOXES)
						{
						if(pData->hStates==NULL)
							{
							CreateStateImageList(pData,0);
							}
						}
					else{
						if(pData->iStatesMode && pData->hStates)
							{
							if(pData->hStates!=THEMEIMGLIST)
								{
								ImageList_Destroy(pData->hStates);
								}

							pData->hStates     = NULL;
							pData->iStatesMode = 0;
							}
						}

					UpdateHeight (pData);
					UpdateScrollY(pData);
					}

				if(uChange&(TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_RTLREADING|TVS_CHECKBOXES|TVS_TRACKSELECT))
					{
					if(!(pData->uStyle&(TVS_HASBUTTONS|TVS_HASLINES)))
						{
						pData->cHasRootRow = 0;
						}
					else{
						pData->cHasRootRow = (char)((pData->uStyle&TVS_LINESATROOT)? 1:0);
						}

					UpdateView(pData);
					}

				if(uChange&TVS_NOSCROLL)
					{
					pData->uOldCountY = 0xFFFF;
					pData->uOldCountX = 0xFFFF;

					UpdateScrollX(pData);
					UpdateScrollY(pData);
					}

				if(uChange&TVS_NOTOOLTIPS)
					{
					if(pData->uStyle&TVS_NOTOOLTIPS)
						{
						UpdateToolTip(pData,0,0);
						}
					else{
						CreateToolTip(pData);
						}
					}

				if(uChange&WS_DISABLED)
					{
					pData->cIsEnabled=(char)((pData->uStyle&WS_DISABLED)? 0:1);
					UpdateView(pData);
					if(!pData->cIsEnabled)UpdateToolTip(pData,0,0);
					}

				UNLOCK(pData);
				}

			return 0;

	case WM_PAINT:												// Das Fenster zeichnen

			if(wParam)
				{
				TreeListDraw(hWnd,(HDC)wParam,NULL);
				}
			else{
				HANDLE		hBufferedPaint;
				HDC			hDcBuffer;
				PAINTSTRUCT sPaint;

				hDc = BeginPaint(hWnd,&sPaint);

				if(pBeginBufferedPt && pEndBufferedPt && 0)		// Gepuffert zeichnen
					{
					hBufferedPaint = pBeginBufferedPt(hDc,&sPaint.rcPaint,BPBF_COMPATIBLEBITMAP,NULL,&hDcBuffer);

					if(hBufferedPaint)
						{
						SetViewportOrgEx(hDcBuffer,sPaint.rcPaint.left,sPaint.rcPaint.top,NULL);

						TreeListDraw(hWnd,hDcBuffer,&sPaint.rcPaint);


						pEndBufferedPt(hBufferedPaint,TRUE);
						}
					else{
						TreeListDraw(hWnd,hDc,&sPaint.rcPaint);
						}
					}
				else{
					TreeListDraw(hWnd,hDc,&sPaint.rcPaint);
					}

				EndPaint(hWnd,&sPaint);
				}

			return 0;

	case WM_TIMER:												// Timer Funktion

			if(wParam==ID_TOOLTIPCHECK)
				{
				   pData	= GetHandle(hWnd);
				if(pData->uToolTipItem)
					{
					HWND	hTop;

					GetCursorPos(&sPoint);

					sInfo.pt.x = sPoint.x;
					sInfo.pt.y = sPoint.y;

					ScreenToClient(hWnd,&sInfo.pt);
					hTop = WindowFromPoint(sPoint);
																// Ist ein anders Fenster jetzt vorn
					if(hTop && hTop!=hWnd && hTop!=pData->hToolTip)
						{
						LOCK(pData);
						UpdateToolTip(pData,0,0);
						UNLOCK(pData);
						return 0;
						}

					if(pData->uToolTipShow)						// Verz?ertes einblenden
						{
						   pData->uToolTipShow--;
						if(pData->uToolTipShow)return 0;

						LOCK(pData);

						TreeListHitTest(pData,&sInfo);

						if(sInfo.flags&TVHT_ONITEM)
						if(pData->uToolTipItem==U(sInfo.hItem) && pData->uToolTipSub==0)
							{
							TOOLINFO sInfo;

							UNLOCK(pData);

							sInfo.cbSize	= sizeof(sInfo);
							sInfo.hwnd		= pData->hWnd;
							sInfo.uId		= (UINT_PTR)pData->hWnd;

							if(pData->uToolTipItem)
								{
								SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
								}

							SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(pData->sToolTipPos.x,pData->sToolTipPos.y));
							SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

							return 0;
							}

						if(sInfo.flags&TVHT_ONSUBITEM)
						if(pData->uToolTipItem==U(sInfo.hItem) && TVHT_SUBTOCOL(sInfo.flags)==pData->uToolTipSub)
							{
							TOOLINFO sInfo;

							UNLOCK(pData);

							sInfo.cbSize	= sizeof(sInfo);
							sInfo.hwnd		= pData->hWnd;
							sInfo.uId		= (UINT_PTR)pData->hWnd;

							if(pData->uToolTipItem)
								{
								SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
								}

							SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(pData->sToolTipPos.x,pData->sToolTipPos.y));
							SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

							return 0;
							}

																// ToolTip wieder entferenen
						UpdateToolTip(pData,U(sInfo.hItem),sInfo.flags);
						UNLOCK(pData);
						}

					LOCK(pData);

					GetCursorPos(&sInfo.pt);
					ScreenToClient(hWnd,&sInfo.pt);
					TreeListHitTest(pData,&sInfo);
					UpdateToolTip(pData,U(sInfo.hItem),sInfo.flags);

					UNLOCK(pData);
					}
				}
			else if(wParam==ID_AUTOSCROLL)						// Auto Scroll
				{
				pData	= GetHandle(hWnd);

				GetCursorPos(&sPoint);
				ScreenToClient(hWnd,&sPoint);

				if(sPoint.y<(int)pData->uSizeY/2)
					{
					uMode = SB_LINEUP;
					iPos  = pData->uStartPixel-sPoint.y;
					if((pData->cScrollMode&1) && sPoint.y<0)break;
					}
				else{
					uMode = SB_LINEDOWN;
					iPos  = sPoint.y-pData->uSizeY+1;
					if((pData->cScrollMode&1) && iPos>1)break;
					}

				if(pData->cScrollMode&2)						// Nur im Fenster erlaubt
					{
					if(sPoint.x<0 || sPoint.x>=pData->iImagesSizeX)break;
					iPos += pData->iRowHeight;
					}

				if(iPos<0)break;

				if(!(pData->cScrollMode&1))
					{
					iPos /= 3;
					}
				else{
					iPos += 2;
					}

				if(iPos>11)iPos=11;
				   iPos  = 12-iPos;

				   pData->bScrollTicker++;
				if(pData->bScrollTicker<iPos)break;				// Jetzt Scrollen

				pData->bScrollTicker=0;
				TreeListProc(hWnd,WM_VSCROLL,uMode,0);
				}

			return 0;

	case WM_COMMAND:											// Kommando Nachrichnten

		if(wParam==MAKELONG(3,EN_KILLFOCUS) || wParam==MAKELONG(3,EN_RETURN) || wParam==MAKELONG(3,CBN_KILLFOCUS))
			{
			   pData = GetHandle(hWnd);
			if(pData->uEditItem)
				{
				LOCK(pData);
				TreeListEndLabelEdit(pData,(wParam==MAKELONG(3,EN_RETURN))? 2:1);
				UNLOCK(pData);
				if(pData->uEditMode)
				if(wParam!=MAKELONG(3,EN_KILLFOCUS))
					{
					SetFocus(pData->hWnd);
					}
				}
			}
		else if(wParam==MAKELONG(3,EN_ESCAPE))					// ESC-Taste in Edit-Fenster
			{
			   pData = GetHandle(hWnd);
			if(pData->uEditItem)
				{
				LOCK(pData);
				TreeListEndLabelEdit(pData,0);
				UNLOCK(pData);
				if(pData->uEditMode)SetFocus(pData->hWnd);
				}
			}													// Änderung in Edit-Fenster
		else if(wParam==MAKELONG(3,EN_CHANGE) || wParam==MAKELONG(3,CBN_EDITCHANGE) || wParam==MAKELONG(3,CBN_SELCHANGE) || wParam==MAKELONG(3,EN_SETTEXT))
			{
			pData = GetHandle(hWnd);
			pData->cColumnStart = 1;
			}

		return 0;

	case WM_NOTIFY:												// Notify Nachrichnten

			if(wParam==1)										// Nachricht vom Header
				{
				NMHEADER   *pHdr=(NMHEADER*)lParam;
				HDITEM		sItem;
				HDITEM		sTemp;
				RECT		sRect;
				int			iDelta;
				int			iCode;
				int			iSize;
				int			iNext;
				int			iCol;
				int			iSub;

				iCode = pHdr->hdr.code;
																// Hat sich die Spaltenbreite ver?dert
				if(iCode==HDN_ITEMCHANGED && (pHdr->pitem->mask&HDI_WIDTH))
					{
					pData	= GetHandle(hWnd);
					iCol	= pHdr->iItem;

					if(pData->aColumn[iCol].sReal!=pHdr->pitem->cxy)
						{
						iCode = HDN_TRACK;
						}
					}

				if(iCode==HDN_BEGINDRAG)						// Drag&Drop im Header
					{
					pData	= GetHandle(hWnd);

					if(!(pData->uStyleEx&TVS_EX_HEADERDRAGDROP) || pHdr->iItem==0)
						{
						return 1;
						}

					return 0;
					}

				if(iCode==HDN_ENDDRAG)							// Drag&Drop im Header fertig
					{
					if(pHdr->pitem->iOrder==0 || pHdr->iItem==0)
						{
						return 1;
						}

					pData	= GetHandle(hWnd);

					if(!(pData->uStyleEx&TVS_EX_HEADERDRAGDROP))
						{
						return 1;
						}

					PostMessage(hWnd,TVM_SETCOLUMNORDERARRAY,FROM_HEADER,0);

					return 0;
					}

				if(iCode==HDN_BEGINTRACK)						// User will die Spaltenbreite ?dern
					{
					pData	= GetHandle(hWnd);

					if(!pData->cHasFocus)
						{
						SetFocus(pData->hWnd);
						}

					if(pData->uStyleEx&TVS_EX_NOCOLUMNRESIZE)	// Darf der User die Spaltenbreite ?dern
						{
						return 1;
						}

					if(pData->uStyleEx&TVS_EX_FIXEDCOLSIZE)		// Fixe gesammte Spaltenbreite
						{
						iNext = pData->aColumn[pHdr->iItem].bIndex;

						for(iNext++;;iNext++)					// Suche n?hste ver?erbare Spalte
							{
							if(U(iNext)>=pData->uColumnCount)
								{
								return 1;
								}

							iSub = pData->aColumnPos[iNext];

							if(pData->aColumn[iSub].sFixed==0)
								{
								break;
								}
							}
						}

					if(U(pHdr->iItem)<pData->uColumnCount)		// Darf die Spalte ver?dert werden
					if(pData->aColumn[pHdr->iItem].sFixed)
						{
						POINT	sPoint;
						int		iCol;
						int		iSub;

						if(pData->aColumn[pHdr->iItem].sReal || pHdr->iItem<0)
							{
							return 1;
							}

						GetCursorPos(&sPoint);					// Die n?hste ver?derbare Spalte greifen
						ScreenToClient(pData->hHeader,&sPoint);

						PostMessage(pData->hHeader,WM_LBUTTONUP,0,MAKELONG(sPoint.x,sPoint.y));

						iCol = pData->aColumn[pHdr->iItem].bIndex;
						iSub = pData->aColumnPos[iCol-1];

						PostMessage(pData->hHeader,WM_LBUTTONDOWN,0,MAKELONG(pData->aColumnXpos[iSub+1]-2,sPoint.y));

						return 1;
						}
					}

				if(iCode==HDN_DIVIDERDBLCLICK)					// Doppelcklick auf Spaltentrenner
					{
					pData	= GetHandle(hWnd);

					if(!pData->cHasFocus)
						{
						SetFocus(pData->hWnd);
						}

					if(pData->uStyleEx&TVS_EX_NOCOLUMNRESIZE)	// Darf der User die Spaltenbreite ?dern
						{
						return 0;
						}

					if(U(pHdr->iItem)<pData->uColumnCount)		// Darf die Spalte ver?dert werden
					if(pData->aColumn[pHdr->iItem].sFixed)
						{
						return 0;
						}

					LOCK(pData);

					   iSize=TreeListScanColumn(pData,pHdr->iItem);
					if(iSize)									// Spalte auf maximale Textbreite
						{
						sItem.cxy	= iSize;
						pHdr->pitem	= &sItem;
						iCode		= HDN_ENDTRACK;

						if(pData->aColumn[pHdr->iItem].bMinEx)	// Minimale Breite pr?en
						if(pData->aColumn[pHdr->iItem].sMin>sItem.cxy)
							{
							sItem.cxy	 = pData->aColumn[pHdr->iItem].sMin;
							}
						}

					UNLOCK(pData);
					}

				if(iCode==HDN_TRACK || iCode==HDN_ENDTRACK)		// Wurde die Spaltenbreite ver?dert
					{
					pData	= GetHandle(hWnd);

					if(!pData->cHasFocus && iCode==HDN_ENDTRACK)
						{
						SetFocus(pData->hWnd);
						}

					LOCK(pData);

					iCol		= pHdr->iItem;
					sItem.mask	= HDI_WIDTH;
					sItem.cxy	= pHdr->pitem->cxy;

					if(pData->aColumn[iCol].bMinEx)				// Minimale Breite pr?en
					if(pData->aColumn[iCol].sMin>sItem.cxy)
						{
						sItem.cxy		 = pData->aColumn[iCol].sMin;
						pHdr->pitem->cxy = sItem.cxy;

						if(sItem.cxy==pData->aColumn[iCol].sSize)
							{
							UNLOCK(pData);
							return 0;
							}
						}

					if(pData->uStyleEx&TVS_EX_FIXEDCOLSIZE)		// Fixe gesammte Spaltenbreite
						{
						if(iCol==0 && sItem.cxy<=0)
							{
							sItem.cxy		 = 1;
							pHdr->pitem->cxy = 1;

							if(sItem.cxy==pData->aColumn[iCol].sSize)
								{
								UNLOCK(pData);
								return 0;
								}
							}

						iNext = pData->aColumn[iCol].bIndex;

						for(iNext++;;iNext++)					// ?erspringe fixierte Spalten
							{
							if(U(iNext)>=pData->uColumnCount)
								{
								UNLOCK(pData);
								return 0;
								}

							iSub = pData->aColumnPos[iNext];

							if(pData->aColumn[iSub].sFixed==0)
								{
								break;
								}
							}

						iDelta    = pData->aColumn[iCol].sReal-sItem.cxy;
						sTemp.cxy = pData->aColumn[iSub].sReal+iDelta;


						if(iDelta<0)							// N?hste Spalte wird zu klein
							{
							if(sTemp.cxy<0)
								{
								sTemp.cxy  =  0;
								iDelta     = -pData->aColumn[iSub].sReal;
								sItem.cxy  =  pData->aColumn[iCol].sReal-iDelta;
								}

							if(pData->aColumn[iSub].bMinEx)		// Minimale Breite pr?en
							if(pData->aColumn[iSub].sMin>sTemp.cxy)
								{
								iDelta    +=  pData->aColumn[iSub].sMin-sTemp.cxy;
								sTemp.cxy  =  pData->aColumn[iSub].sMin;
								sItem.cxy  =  pData->aColumn[iCol].sReal-iDelta;
								}

							pHdr->pitem->cxy = sItem.cxy;

							if(iDelta>=0)						// Keine Änderung
								{
								UNLOCK(pData);
								return 0;
								}
							}

						if(pData->aColumn[iSub].bWeight)		// Variable Spalte
							{
							pData->iVarSize -= pData->aColumn[iSub].sSize;
							pData->iVarSize += sTemp.cxy;
							}
						else{									// Fixe Spalte
							pData->iFixSize -= pData->aColumn[iSub].sSize;
							pData->iFixSize += sTemp.cxy;
							}

						sTemp.mask	= HDI_WIDTH;

						iDif						= pData->aColumn[iSub].sReal;
						pData->aColumn[iSub].sSize	= (short)sTemp.cxy;
						pData->aColumn[iSub].sReal	= (short)sTemp.cxy;
						Header_SetItem(pData->hHeader,iSub,&sTemp);

																// Breite ver?dert
						if(iDif!=sTemp.cxy && (pData->uStyleEx&TVS_EX_HEADERCHGNOTIFY))
							{
							TV_COLSIZE	sNotify;

 							sNotify.hdr.code			= TVN_COLUMNCHANGED;
							sNotify.uColumn				= iSub;
							sNotify.uIndex				= pData->aColumn[iSub].bIndex;
							sNotify.uPosX				= pData->aColumnXpos[iSub];
							sNotify.iSize				= sTemp.cxy;

							UNLOCK(pData);
							SendNotify(pData,&sNotify.hdr);
							LOCK(pData);
							}
						}


					if(pData->aColumn[iCol].bWeight)			// Ist es eine variable Spalte
						{
						pData->iVarSize -= pData->aColumn[iCol].sSize;
						pData->iVarSize += sItem.cxy;
						}
					else{										// Fixe Spalte
						pData->iFixSize -= pData->aColumn[iCol].sSize;
						pData->iFixSize += sItem.cxy;
						}

					iDif					   = pData->aColumn[iCol].sReal;
					pData->aColumn[iCol].sSize = (short)sItem.cxy;
					pData->aColumn[iCol].sReal = (short)sItem.cxy;
					Header_SetItem(pData->hHeader,iCol,&sItem);

					   iSize = UpdateColumns(pData);
					if(iSize<0x10000)							// Spalten neu zeichnen
						{
						GetClientRect (hWnd,&sRect);
						sRect.left  = iSize;
						sRect.left -= pData->uScrollX;
						InvalidateRect(hWnd,&sRect,FALSE);
						}

					UpdateScrollX(pData);
					UNLOCK(pData);

																// Breite ver?dert
					if(iDif!=sItem.cxy && (pData->uStyleEx&TVS_EX_HEADERCHGNOTIFY))
						{
						TV_COLSIZE	sNotify;

 						sNotify.hdr.code	= TVN_COLUMNCHANGED;
						sNotify.uColumn		= iCol;
						sNotify.uIndex		= pData->aColumn[iCol].bIndex;
						sNotify.uPosX		= pData->aColumnXpos[iCol];
						sNotify.iSize		= sItem.cxy;

						SendNotify(pData,&sNotify.hdr);
						}
					}
																// Weiterleiten an Elternfenster
				if(iCode==HDN_ITEMCLICK || iCode==HDN_ITEMDBLCLICK)
					{
					pData	= GetHandle(hWnd);

					if(!pData->cHasFocus)
						{
						SetFocus(pData->hWnd);
						}

					return SendNotify(pData,&pHdr->hdr);
					}

				if(iCode==NM_RCLICK)							// Rechte Maustaste im Header
					{
					TV_HDRCLICK		sNotify;

					pData = GetHandle(hWnd);
					GetCursorPos(&sNotify.sPoint);
					ScreenToClient(pData->hWnd,&sNotify.sPoint);

					if(SendMessage(pData->hHeader,HDM_HITTEST,0,(LPARAM)&sNotify.sPoint)>=0)
						{
						sNotify.hdr.code = TVN_COLUMNRCLICK;
						return SendNotify(pData,&sNotify.hdr);
						}
					}
				}
			else{												// Tooltip Textabfrage
				NMHDR		   *pHdr = (NMHDR*)lParam;
				NMTTDISPINFO   *pInfo;


				switch(pHdr->code)
					{
				case TTN_GETDISPINFO:

						pData			= GetHandle(hWnd);
						pInfo			= (NMTTDISPINFO*)pHdr;
						pInfo->lpszText = pData->pToolTipText;
						SendMessage(pData->hToolTip,WM_SETFONT,(WPARAM)pData->hFontT,0);
						break;
					}
				}


			return 0;

	case WM_GETDLGCODE:											// Welche Tasten werden im Dialog benutzt

			   pMsg=(MSG*)lParam;
			if(pMsg && pMsg->message==WM_KEYDOWN)
				{
				if(pMsg->wParam==VK_RETURN)
					{
					return DLGC_WANTALLKEYS;
					}

				if(pMsg->wParam==VK_ESCAPE && GetHandle(hWnd)->uMoveItem)
					{
					return DLGC_WANTALLKEYS;
					}
				}

			return DLGC_WANTCHARS|DLGC_WANTARROWS;

	case WM_PASTE:												// Weiterleiten an Edit-Control
	case WM_CLEAR:
	case WM_COPY:
	case WM_CUT:

			pData = GetHandle(hWnd);

			if(pData->hEdit)
				{
				PostMessage(pData->hEdit,uMsg,0,0);
				return 0;
				}

			break;


	case TVM_SETIMAGELIST:										// Die Image-Liste einstellen

			pData	= GetHandle(hWnd);

			LOCK(pData);

			switch((int)wParam)
				{
			case TVSIL_NORMAL:	   lRet=(LPARAM)pData->hImages;
								if(lRet==lParam)break;

									pData->hImages=(HIMAGELIST)lParam;
								if(!pData->hImages)
									{
									pData->iImagesSizeX=0;
									pData->iImagesSizeY=0;
									}
								else{
									IMAGEINFO sInfo;
									ImageList_GetImageInfo(pData->hImages,0,&sInfo);
									pData->iImagesSizeX = sInfo.rcImage.right -sInfo.rcImage.left;
									pData->iImagesSizeY = sInfo.rcImage.bottom-sInfo.rcImage.top;
									if(pData->hHeader && !pData->hHdrImg)
										{
										SendMessage(pData->hHeader,HDM_SETIMAGELIST,0,(LPARAM)pData->hImages);
										}
									}

								if(!pData->iSubImgMode || pData->hSubImg==pData->hImages)
									{
									pData->iSubImgMode  = 0;
									pData->hSubImg      = pData->hImages;
									pData->iSubImgXsize = pData->iImagesSizeX;
									pData->iSubImgYsize = pData->iImagesSizeX;
									}

								if(!pData->cFixedHeight)
									{
									pData->iRowHeight=1;
									UpdateHeight (pData);
									UpdateScrollY(pData);
									}
								else{
									UpdateView(pData);
									}

								break;

			case TVSIL_STATE:	   lRet=(LPARAM)pData->hStates;
								if(lRet==lParam)break;

								if(pData->iStatesMode)
									{
									if(pData->hStates!=THEMEIMGLIST)
										{
										ImageList_Destroy(pData->hStates);
										}

									pData->iStatesMode = 0;
									}

									pData->hStates=(HIMAGELIST)lParam;
								if(!pData->hStates)
									{
									pData->iStatesSizeX=0;
									pData->iStatesSizeY=0;

									if(pData->uStyle&TVS_CHECKBOXES)
										{
										CreateStateImageList(pData,0);
										}
									}
								else if(pData->hStates==THEMEIMGLIST)
									{
									pData->iStatesSizeX = 16;
									pData->iStatesSizeY = 16;
									pData->iStatesMode  = 1;
									if(!pData->hThemeBt)CreateStateImageList(pData,0);
									}
								else{
									IMAGEINFO sInfo;
									ImageList_GetImageInfo(pData->hStates,0,&sInfo);
									pData->iStatesSizeX = sInfo.rcImage.right -sInfo.rcImage.left;
									pData->iStatesSizeY = sInfo.rcImage.bottom-sInfo.rcImage.top;
									pData->iStatesMode  = 0;
									}

								if(!pData->cFixedHeight)
									{
									pData->iRowHeight=1;
									UpdateHeight (pData);
									UpdateScrollY(pData);
									}
								else{
									UpdateView(pData);
									}

								break;

			case TVSIL_CHECK:	   lRet=(LPARAM)pData->hChecks;
								if(lRet==lParam)break;

								if(pData->iChecksMode)
									{
									if(pData->hChecks!=THEMEIMGLIST)
										{
										ImageList_Destroy(pData->hChecks);
										}

									pData->iChecksMode = 0;
									}

									pData->hChecks=(HIMAGELIST)lParam;
								if(!pData->hChecks)
									{
									pData->iChecksSizeX=0;
									pData->iChecksSizeY=0;

									for(uVal=0;uVal<pData->uColumnCount;uVal++)
										{
										if(pData->aColumn[uVal].bEdit<TVSIL_CHECK)continue;
										CreateStateImageList(pData,1);
										break;
										}
									}
								else if(pData->hChecks==THEMEIMGLIST)
									{
									pData->iChecksSizeX = 16;
									pData->iChecksSizeY = 16;
									pData->iChecksMode  = 1;
									if(!pData->hThemeBt)CreateStateImageList(pData,1);
									}
								else{
									IMAGEINFO sInfo;
									ImageList_GetImageInfo(pData->hChecks,0,&sInfo);
									pData->iChecksSizeX = sInfo.rcImage.right -sInfo.rcImage.left;
									pData->iChecksSizeY = sInfo.rcImage.bottom-sInfo.rcImage.top;
									pData->iChecksMode  = 0;
									}

								if(!pData->cFixedHeight)
									{
									pData->iRowHeight=1;
									UpdateHeight (pData);
									UpdateScrollY(pData);
									}
								else{
									UpdateView(pData);
									}

								break;

			case TVSIL_SUBIMAGES:  lRet=(LPARAM)pData->hSubImg;
								if(lRet==lParam)break;

								if(pData->iSubImgMode)
									{
									ImageList_Destroy(pData->hSubImg);
									pData->iSubImgMode = 0;
									}

									pData->hSubImg=(HIMAGELIST)lParam;
								if(!pData->hSubImg || pData->hSubImg==pData->hImages)
									{
									pData->iSubImgMode  = 0;
									pData->hSubImg      = pData->hImages;
									pData->iSubImgXsize = pData->iImagesSizeX;
									pData->iSubImgYsize = pData->iImagesSizeX;
									}
								else{
									IMAGEINFO sInfo;
									ImageList_GetImageInfo(pData->hSubImg,0,&sInfo);
									pData->iSubImgXsize = sInfo.rcImage.right -sInfo.rcImage.left;
									pData->iSubImgYsize = sInfo.rcImage.bottom-sInfo.rcImage.top;
									pData->iSubImgMode  = 1;
									}

								if(!pData->cFixedHeight)
									{
									pData->iRowHeight=1;
									UpdateHeight (pData);
									UpdateScrollY(pData);
									}
								else{
									UpdateView(pData);
									}

								break;

			case TVSIL_HEADER:	   lRet=(LPARAM)pData->hHdrImg;
								if(lRet==lParam)break;

								if(pData->hHeader)
									{
									SendMessage(pData->hHeader,HDM_SETIMAGELIST,0,(LPARAM)((pData->hHdrImg)? pData->hHdrImg:pData->hImages));
									}

								UpdateView(pData);
								break;

			default:			lRet=0;
				}


			UNLOCK(pData);

			return lRet;

	case TVM_GETIMAGELIST:										// Die Image-Liste abfragen

				pData	= GetHandle(hWnd);
			if(!pData)return 0;

			switch((int)wParam)
				{
			case TVSIL_NORMAL:	 lRet=(LPARAM)pData->hImages; break;
			case TVSIL_STATE:	 lRet=(LPARAM)pData->hImages; break;
			case TVSIL_CHECK:	 lRet=(LPARAM)pData->hChecks; break;
			case TVSIL_SUBIMAGES:lRet=(LPARAM)pData->hSubImg; break;
			case TVSIL_HEADER:	 lRet=(LPARAM)pData->hHdrImg; break;
			default:			 lRet=0;
				}

			return lRet;

	case TVM_GETSETOPTION:										// Diverse Optionen einstellen

			pData	= GetHandle(hWnd);
			lRet	= 0;

			switch(U(wParam)&~TVOP_WRITEOPTION)
				{
			case TVOP_AUTOEXPANDOFF:							// Icon Offset f? TVS_EX_AUTOEXPANDICON

				lRet = pData->iAutoAdd;

				if((wParam&TVOP_WRITEOPTION) && lParam!=lRet)
					{
					LOCK(pData);

					pData->iAutoAdd = (int)lParam;
					if(pData->uItemPosCount>0)
						{
						UpdateView(pData);
						}

					UNLOCK(pData);
					}

				break;

			case TVOP_DIABALEDRAGMASK:							// Maske um Drag&Drop zu sperren

				lRet = pData->uNoDrag;

				if((wParam&TVOP_WRITEOPTION) && lParam!=lRet)
					{
					LOCK(pData);
					pData->uNoDrag = U(lParam);
					UNLOCK(pData);
					}

				break;

			case TVOP_DRAGMOVEFLAGS:							// Flags f? Drag-Move-Operationen (TVDM_???)

				lRet = pData->uMoveFlags;

				if((wParam&TVOP_WRITEOPTION) && lParam!=lRet)
					{
					LOCK(pData);
					pData->uMoveFlags = U(lParam);
					UNLOCK(pData);
					}

				break;

			case TVOP_FIXEDLINES:								// Einstellen der fixierten Zeilen

				lRet = pData->uFixedLines;

				if((wParam&TVOP_WRITEOPTION) && lParam!=lRet)
					{
					LOCK(pData);
					pData->uFixedLines = U(lParam);
					UpdateView(pData);
					UNLOCK(pData);
					}

				break;

			case TVOP_FIXEDALWAYS:								// Fixie Trennung immer anzeigen

				if(lParam)lParam=0x80000000;
				lRet = pData->uFixedAlways;

				if((wParam&TVOP_WRITEOPTION) && lParam!=lRet)
					{
					LOCK(pData);
					pData->uFixedAlways = (lParam)? 0x80000000:0;
					UpdateView(pData);
					UNLOCK(pData);
					}

				if(lRet)lRet=1;
				break;

			case TVOP_SELECTIONCOUNT:							// Anzahl der ausgew?lten Eintr?e abfragen

				lRet = pData->uSelectedCount;
				break;

			case TVOP_EDITCLICKMODE:							// Editklickoptionen (Bit0=nur auf Text, Bit1=direkt ohne Auswahl)

				lRet = pData->cEditClickMode;

				if((wParam&TVOP_WRITEOPTION) && lParam!=lRet)
					{
					LOCK(pData);
					pData->cEditClickMode = (BYTE)lParam;
					UNLOCK(pData);
					}

				break;

			case TVOP_ITEMDRAWPROC:								// Callback f? das Zeichen von Eintr?en

				lRet = (LPARAM)pData->pDrawItem;

				if((wParam&TVOP_WRITEOPTION) && lParam!=lRet)
					{
					pData->pDrawItem = (PFNTVDRAW)lParam;
					}

				break;

			case TVOP_ITEMDRAWPARAM:							// Parameter f? das Zeichen von Eintr?en

				lRet = (LPARAM)pData->pDrawParam;

				if(wParam&TVOP_WRITEOPTION)
					{
					pData->pDrawParam = (LPVOID)lParam;
					}

				break;

			case TVOP_MINITEMHEIGHT:							// Minimale H?e eines Eintrages

				lRet = pData->cMinHeight;

				if((wParam&TVOP_WRITEOPTION) && lParam!=lRet)
					{
					LOCK(pData);
					pData->cMinHeight = (char)lParam;
					UpdateHeight (pData);
					UpdateScrollY(pData);
					UNLOCK(pData);
					}

				break;

			case TVOP_WINDOWPROC:								// Callback von der Fensterfunktion

				lRet = (LPARAM)TreeListProc;
				break;
				}

			return lRet;

	case TVM_GETITEMSTATE:										// Die Statusbits eines Eintrags abfragen

			pData = GetHandle(hWnd);
			LOCK(pData);

			if(U(wParam)<=pData->uTreeItemsMax)
				{
					pEntry = pData->pTreeItems[U(wParam)];
				if(!pEntry)lRet = 0;
					{
					lRet = pEntry->uState;
					}
				}
			else{
				lRet = 0;
				}

			UNLOCK(pData);

			return lRet;

	case TVM_GETITEM:											// Eintr?e abfragen

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListGetItem(pData,(TV_ITEM*)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_SETITEM:											// Eintr?e ?dern

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListSetItem(pData,(TV_ITEM*)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_SETMULTIITEMS:										// Vielfach Eintr?e ?dern

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListSetMultiItems(pData,(TV_ITEM*)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_SETMARKLINE:										// Die Markierungslinie setzen

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListSetMarkLine(pData,U(lParam),U(wParam));
			UNLOCK(pData);

			return lRet;

	case TVM_INSERTITEM:										// Eintr?e einf?en

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListInsertItem(pData,(TV_INSERTSTRUCT*)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_DELETEITEM:										// Eintr?e l?chen

			pData = GetHandle(hWnd);
			LOCK(pData);
			uVal  = 1;
			if(U(wParam))
				{
				switch(U(wParam))
					{
				case 0x88:	uVal=2;	break;
				case 0x89:	uVal=5;	break;
				case 0x8A:	uVal=6;	break;
					}
				}

			lRet  = TreeListDeleteItem(pData,U(lParam),uVal);
			UNLOCK(pData);
			return lRet;

	case TVM_MOVEITEM:											// Einen Eintrag verschieben
	case TVM_MOVEITEM1:
	case TVM_MOVEITEM2:
	case TVM_MOVEITEM3:

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListMoveItem(pData,U(lParam),U(wParam),uMsg-TVM_MOVEITEM);
			UNLOCK(pData);

			return lRet;

	case TVM_FINDITEM:											// Eintr?e suchen

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListFindItem(pData,U(wParam),(TV_FIND*)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_DELETECOLUMN:										// L?chen einer Salte im Header

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListDeleteColumn(pData,U(wParam));
			UNLOCK(pData);

			return lRet;

	case TVM_INSERTCOLUMN:										// Einf?en einer Salte im Header

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListInsertColumn(pData,(int)wParam,(TV_COLUMN*)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_GETCOLUMNCOUNT:									// Abfragen der Spaltenanzahl

			pData = GetHandle(hWnd);
			return (LRESULT)pData->uColumnCount;

	case TVM_GETHEADER:											// Abfragen des Header-Fensters

			pData = GetHandle(hWnd);
			return (LRESULT)pData->hHeader;

	case TVM_GETEXTENDEDSTYLE:									// Abfragen der erweiterten Style-Bits

			pData = GetHandle(hWnd);
			return pData->uStyleEx;

	case TVM_SETEXTENDEDSTYLE:									// Einstellen der erweiterten Style-Bits

				pData = GetHandle(hWnd);
			if(!wParam)wParam=0xFFFFFFFF;

			uVal  = pData->uStyleEx &~U(wParam);
			uVal |=	U(lParam)& U(wParam);

			if(pData->uStyleEx!=uVal)							// Gab es Änderungen
				{
				LOCK(pData);
				uChange = pData->uStyleEx^uVal;
				pData->uStyleEx = uVal;

				if(uChange&TVS_EX_AUTOHSCROLL)
					{
					UpdateScrollX(pData);
					}

				if((uChange&TVS_EX_GRAYEDDISABLE) && !pData->cIsEnabled)
					{
					UpdateView(pData);
					}

				if(uChange&(TVS_EX_ITEMLINES|TVS_EX_ALWAYSSCROLLY))
					{
					if(uChange&TVS_EX_ALWAYSSCROLLY)
						{
						pData->uOldPageY	= 0xFFFFFFFF;

						if(uVal&TVS_EX_ALWAYSSCROLLY)
							{
							sScroll.cbSize		= sizeof(SCROLLINFO);
							sScroll.fMask		= SIF_RANGE|SIF_PAGE;
							sScroll.nMin		= 0;
							sScroll.nMax  		= 100;
							sScroll.nPage 		= 10;

							UNLOCK(pData);						// WIN-BUG: Die Scrollbar muss an sein f? SIF_DISABLENOSCROLL
							SetScrollInfo(pData->hWnd,SB_VERT,&sScroll,TRUE);
							LOCK(pData);
							}
						else if(pData->uPageEnties>=pData->uItemPosCount)
							{
							UNLOCK(pData);
							ShowScrollBar(pData->hWnd,SB_VERT,FALSE);
							LOCK(pData);
							}
						}

					UpdateHeight (pData);
					UpdateScrollY(pData);
					}

				if(uChange&(TVS_EX_ALTERNATECOLOR|TVS_EX_SUBSELECT|TVS_EX_MULTISELECT|TVS_EX_FULLROWMARK|TVS_EX_FULLROWITEMS))
					{
					UpdateView(pData);
					}

				if((uChange&~U(lParam))&TVS_EX_SUBSELECT)
					{
					TreeListSelectItem(pData,pData->uSelectedItem,0,TVC_UNKNOWN);
					}

  				if((uChange&TVS_EX_HIDEHEADERS) && pData->hHeader)
					{
					pData->uStartPixel = (pData->uStyleEx&TVS_EX_HIDEHEADERS)? 0:GetSystemMetrics(SM_CYHSCROLL);
					MoveWindow(pData->hHeader,-(int)pData->uScrollX,0,pData->uSizeX+pData->uScrollX,pData->uStartPixel,TRUE);
					UpdateView(pData);
					}

				if(uChange&TVS_EX_ALLOWITEMHIDE)
					{
					UpdateItems(pData,0);
					}

				UNLOCK(pData);
				}

			return uVal;

	case TVM_GETINSERTMARKCOLOR:								// Farben abfragen
							wParam=TVC_INSERT;goto ColGet;
	case TVM_GETLINECOLOR:	wParam=TVC_LINE;  goto ColGet;
	case TVM_GETTEXTCOLOR:	wParam=TVC_TEXT;
	case TVM_GETBKCOLOR:

	ColGet:	if(wParam<0 || wParam>=MAX_COLORS)return 0xFFFFFFFF;
			pData = GetHandle(hWnd);
			return pData->aColors[U(wParam)];


	case TVM_SETINSERTMARKCOLOR:								// Farben einstellen
							wParam=TVC_INSERT;goto ColSet;
	case TVM_SETLINECOLOR:	wParam=TVC_LINE;  goto ColSet;
	case TVM_SETTEXTCOLOR:	wParam=TVC_TEXT;
	case TVM_SETBKCOLOR:

	ColSet:	if(wParam<0 || wParam>=MAX_COLORS)return 0xFFFFFFFF;

			pData = GetHandle(hWnd);

			LOCK(pData);

			lRet=pData->aColors[U(wParam)];

			if((COLORREF)lParam==TV_NOCOLOR)					// Standartfarbe einstellen
				{
				if(pData->aColorChanged[U(wParam)])
					{
					pData->aColorChanged[U(wParam)]=0;
					UpdateColors(pData);
					UpdateView  (pData);
					}
				}
			else{
				pData->aColorChanged[U(wParam)]=1;
				pData->aColors      [U(wParam)]=(COLORREF)lParam;

				if(lRet!=lParam)
					{
					UpdateColors(pData);
					UpdateView  (pData);
					}
				}

			UNLOCK(pData);

			return lRet;

	case TVM_HITTEST:											// Abfragen von Koortinatenpositionen

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListHitTest(pData,(LPTV_HITTESTINFO)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_SELECTCHILDS:										// Mehrere Eintr?e ausw?len

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListSelectChilds(pData,U(lParam),U(wParam));
			UNLOCK(pData);

			return lRet;

	case TVM_SELECTSUBITEM:										// Einen (Sub)Eintrag ausw?len

			pData = GetHandle(hWnd);

			LOCK(pData);
			if(!(pData->uStyleEx&TVS_EX_SUBSELECT))wParam=0;
			lRet=TreeListSelectItem(pData,U(lParam),U(wParam),TVC_UNKNOWN);
			if(lRet>1)lRet=1;
			UNLOCK(pData);

			return lRet;

	case TVM_SELECTDROP:										// Den unterstrichenen Eintrags ausw?len

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListSetTrackItem(pData,U(lParam),U(wParam));
			UNLOCK(pData);

			return lRet;

	case TVM_SELECTITEM:										// Einen Eintrag ausw?len

			pData = GetHandle(hWnd);

			LOCK(pData);

			switch(wParam)
				{
			case TVGN_CARET:		lRet=TreeListSelectItem(pData,U(lParam),0,TVC_UNKNOWN);
									if(lRet>1)lRet=1;
									break;
			case TVGN_DROPHILITE:	lRet=TreeListSetTrackItem(pData,U(lParam),0);
									break;

			case TVGN_FIRSTVISIBLE:	lRet=TreeListEnsureVisible(pData,U(lParam),FIRST_LINE);
									lRet=(lRet<0)? 0:1;
									break;

			default:				lRet=0;
				}

			UNLOCK(pData);

			return lRet;

	case TVM_GETCOLUMNORDERARRAY:								// Spaltensortierung abfragen

			if(!lParam)return 0;

			pData = GetHandle(hWnd);

			if(!pData->hHeader)return 0;

			return Header_GetOrderArray(pData->hHeader,U(wParam),lParam);

	case TVM_SETCOLUMNORDERARRAY:								// Spalten sortieren

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet  = TreeListSetOrderArray(pData,U(wParam),(unsigned*)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_GETCOUNT:											// Anzahl der Eintr?e abfragen

			pData = GetHandle(hWnd);
			return pData->uTreeItemsCount;

	case TVM_GETINDENT:											// Einr?kung abfragen

			pData = GetHandle(hWnd);
			return pData->iIndent;

	case TVM_SETINDENT:											// Einr?kung einstellen

			pData = GetHandle(hWnd);
			lRet  = pData->iIndent;
			if(wParam< 5)wParam= 5;
			if(wParam>64)wParam=64;
			if(lRet!=(LPARAM)wParam)
				{
				LOCK(pData);
				pData->iIndent = (int)wParam;
				UpdateView(pData);
				UNLOCK(pData);
				}

			return lRet;

	case TVM_GETITEMHEIGHT:										// Zeilenh?e abfragen

			pData = GetHandle(hWnd);
			return pData->iRowHeight;

	case TVM_SETITEMHEIGHT:										// Zeilenh?e abfragen

			pData = GetHandle(hWnd);
			lRet  = pData->iRowHeight;

			if(wParam==-1)
				{
				LOCK(pData);
				pData->cFixedHeight=0;
				UpdateHeight (pData);
				UpdateScrollY(pData);
				UNLOCK(pData);
				return lRet;
				}

			if(wParam&1 )
			if(!(pData->uStyleEx&TVS_NONEVENHEIGHT))wParam--;
			if(wParam<  1)wParam=  1;
			if(wParam>256)wParam=256;

			if(lRet!=(LPARAM)wParam)
				{
				LOCK(pData);
				pData->cFixedHeight = 1;
				pData->iRowHeight   = (int)wParam;
				UpdateView(pData);
				UNLOCK(pData);
				}

			return lRet;

	case TVM_GETVISIBLECOUNT:									// Abfragen der sichtbaren Zeilen

			pData = GetHandle(hWnd);
			return pData->uPageEnties;

	case TVM_ENSUREVISIBLE:										// Einen Eintrag ins Sichtfenster legen

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet=TreeListEnsureVisible(pData,U(lParam),(int)wParam);
			UNLOCK(pData);

			return lRet;

	case TVM_ISITEMVISIBLE:										// Ist ein Eintrag sichtbar

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet=TreeListIsVisible(pData,U(lParam),(int)wParam);
			UNLOCK(pData);

			return lRet;

	case TVM_GETNEXTITEM:										// Einen Eintrag suchen

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet=TreeListGetNextItem(pData,U(lParam),(int)wParam);
			UNLOCK(pData);

			return lRet;

	case TVM_GETITEMRECT:										// Das Rechteck eines Eintrages abfragen

			pData = GetHandle(hWnd);
			LOCK(pData);
			uVal=*(unsigned*)lParam;
			lRet=TreeListGetItemRect(pData,uVal,U(wParam),(RECT*)lParam);
			UNLOCK(pData);

			return lRet;

	case TVM_EXPAND:											// Einen Eintrag umklappen

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet = TreeListExpand(pData,U(lParam),U(wParam));
			UNLOCK(pData);
			return lRet;

	case TVM_SETINSERTMARK:										// Einf?emarke eintellen

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListSetInsertMark(pData,U(lParam),(int)wParam);
			UNLOCK(pData);

			return lRet;

	case TVM_SETITEMBKCOLOR:									// Hintergrundfarbe eines Eintrages ?dern

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListSetItemColor(pData,U(wParam&0xFFFFFF),U(wParam)>>24,(COLORREF)lParam,0);
			UNLOCK(pData);

			return lRet;

	case TVM_SETITEMTEXTCOLOR:									// Textfarbe eines Eintrages ?dern

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListSetItemColor(pData,U(wParam&0xFFFFFF),U(wParam)>>24,(COLORREF)lParam,1);
			UNLOCK(pData);

			return lRet;

	case TVM_GETITEMBKCOLOR:									// Hintergrundfarbe eines Eintrages abfragen

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListGetItemColor(pData,U(wParam),U(lParam),0);
			UNLOCK(pData);

			return lRet;

	case TVM_GETITEMTEXTCOLOR:									// Textfarbe eines Eintrages abfragen

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListGetItemColor(pData,U(wParam),U(lParam),1);
			UNLOCK(pData);

			return lRet;

	case TVM_SORTCHILDRENEX:									// Sortieren mit Funktion

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListSortItemsEx(pData,(TV_SORTEX*)lParam,(int)wParam);
			UNLOCK(pData);

			return lRet;

	case TVM_SORTCHILDRENCB:									// Sortieren mit Funktion

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListSortItemsCb(pData,(TV_SORTCB*)lParam,(int)wParam);
			UNLOCK(pData);

			return lRet;

	case TVM_SORTCHILDREN:										// Sortieren der Kindereintr?e

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet=TreeListSortItems(pData,U(lParam),(int)wParam);
			UNLOCK(pData);

			return lRet;

	case TVM_GETITEMOFROW:										// Hole den Eintrag von einer Reihe

			pData = GetHandle(hWnd);

			LOCK(pData);
			if(U(lParam)<pData->uItemPosCount)
					lRet=pData->pItemPos[U(lParam)];
			else	lRet=0;
			UNLOCK(pData);

			return lRet;

	case TVM_GETROWCOUNT:										// Hole die Anzahl der sichtbaren Reihen

			pData = GetHandle(hWnd);

			return pData->uItemPosCount;

	case TVM_GETCOUNTPERPAGE:									// Hole die Anzahl der darstelbaren Reihen

			pData = GetHandle(hWnd);

			return pData->uMaxEnties;

	case TVM_GETROWOFITEM:										// Suche Reihe zu einem Eintrag

			pData = GetHandle(hWnd);

			LOCK(pData);
			if(U(lParam)<=pData->uTreeItemsMax)
				{
					pEntry=pData->pTreeItems[U(lParam)];
				if(!pEntry)
						lRet = -1;
				else	lRet = pEntry->uShowPos-1;
				}
			else{
				lRet=-1;
				}
			UNLOCK(pData);

			return lRet;

	case TVM_EDITLABEL:											// Einen Eintrag editieren

			pData = GetHandle(hWnd);
			if(!(pData->uStyle&TVS_EDITLABELS))return 0;

			LOCK(pData);

			if((wParam&0xFFFFFF00)==TVLE_DONOTIFY)				// Notify starten
				{
				lRet=TreeListStartNotifyEdit(pData,U(lParam),TVIR_EDITCOL(wParam),VK_RETURN,0);
				}
			else{
				pData->cColumnStart = 0;
				lRet=(LRESULT)TreeListEditLabel(pData,U(lParam),U(wParam));
				}

			UNLOCK(pData);

			return lRet;

	case TVM_GETEDITCONTROL:									// Das Handle des Edit-Fensters abfragen

			pData = GetHandle(hWnd);
			return (LRESULT)pData->hEdit;

	case TVM_GETTOOLTIPS:										// Das Handle des ToolTip-Fensters abfragen

			pData = GetHandle(hWnd);
			return (LRESULT)pData->hToolTip;

	case TVM_SETTOOLTIPS:										// Das Handle f? das ToolTip-Fensters setzen

			pData = GetHandle(hWnd);
			lRet  = (LRESULT)pData->hToolTip;
			pData->hToolTip = (HWND)wParam;
			return lRet;

	case TVM_SETUSERDATASIZE:									// Einstellen der Größe der User-Daten


			if(lParam<0 || lParam>0x1000000)return -1;
			pData = GetHandle(hWnd);

			LOCK(pData);

			if(pData->uTreeItemsCount>0)
				{
				lRet = 0;
				}
			else{
				pData->uUserDataSize = U(lParam);
				lRet = lParam;
				}

			UNLOCK(pData);

			return lRet;

	case TVM_GETUSERDATASIZE:									// Abfragen der Größe der User-Daten

			pData = GetHandle(hWnd);
			return pData->uUserDataSize;

	case TVM_GETUSERDATA:										// Einen Zeiger auf die User-Daten holen

			pData = GetHandle(hWnd);

			if(pData->uUserDataSize && U(lParam)<=pData->uTreeItemsMax)
				{
				   pEntry = pData->pTreeItems[U(lParam)];
				if(pEntry)return (LRESULT)(pEntry+1);
				}

			return 0;

	case TVM_AUTOSCROLL:										// Automatisches Scrollen aktivieren

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet=TreeListAutoScroll(pData,U(wParam),U(lParam));
			UNLOCK(pData);
			return lRet;

	case TVM_STARTUPDATE:										// Remote-Update ausf?ren

			pData = GetHandle(hWnd);
			LOCK(pData);

			while(pData->pUpdateFirst)
				{
				TV_ITEM		sItem;
				NMTREEVIEW	sNotify;
				UpdateData *pUpdate;

				pUpdate				= pData->pUpdateFirst;
				pData->pUpdateFirst = (UpdateData*)pUpdate->pNext;

				switch(pUpdate->uType)
					{
				case UPDATE_ITEMS:	UpdateItems   (pData,U(pUpdate->wParam));								break;
				case UPDATE_RECT:	UpdateRect    (pData,U(pUpdate->wParam),U(pUpdate->lParam));			break;
				case UPDATE_VIEW:	UpdateView    (pData);													break;
				case UPDATE_SCRLX:	UpdateScrollX (pData);													break;
				case UPDATE_SCRLY:	UpdateScrollY (pData);													break;
				case UPDATE_SCROLL:	SetScrollPos  (pData->hWnd,U(pUpdate->wParam),U(pUpdate->lParam),TRUE);	break;
				case UPDATE_EXPAND:	TreeListExpand(pData,U(pUpdate->lParam),U(pUpdate->wParam));			break;
				case UPDATE_INSERT:	pData->cNoExpandNf 			= 1;
									sItem.mask					= TVIF_STATE;
									sItem.hItem					= (HTREEITEM)pUpdate->wParam;
									sItem.state					= U(pUpdate->lParam);
									sItem.stateMask				= U(pUpdate->lParam)>>16;
									TreeListSetItem(pData,&sItem);
									pData->cNoExpandNf 			= 0;
									break;
				case UPDATE_STATE:	sItem.mask					= TVIF_STATE;
									sItem.hItem					= (HTREEITEM)pUpdate->wParam;
									sItem.state					= U(pUpdate->lParam);
									sItem.stateMask				= U(pUpdate->lParam)>>16;
									TreeListSetItem(pData,&sItem);
									break;
				case UPDATE_UNSEL:	sNotify.hdr.code			= TVN_SELCHANGED;
									sNotify.action				= TVC_UNKNOWN;
									sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
									sNotify.itemOld.hItem		= (HTREEITEM)U(pUpdate->wParam);
									sNotify.itemOld.stateMask	= 0xFFFFFFFF;
									sNotify.itemOld.state		= TVIS_SELECTED;
									sNotify.itemOld.lParam		= pUpdate->lParam;
									sNotify.itemOld.cChildren	= 0;
									sNotify.itemOld.pszText		= (LPTSTR)-1;
									sNotify.itemOld.cchTextMax	= -1;
									sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|TVIF_PARAM;
									sNotify.itemNew.hItem		= NULL;
									sNotify.itemNew.stateMask	= 0xFFFFFFFF;
									sNotify.itemNew.state		= 0;
									sNotify.itemNew.lParam		= 0;
									sNotify.itemNew.cChildren	= 0;
									sNotify.itemNew.pszText		= (LPTSTR)-1;
									sNotify.itemNew.cchTextMax	= -1;
									sNotify.ptDrag.x			= 0;
									sNotify.ptDrag.y			= 0;
									UNLOCK(pData);
									SendNotify(pData,&sNotify.hdr);	// Bekant geben das der Eintrag nicht mehr ausgew?lt ist
									LOCK(pData);
									break;
					}

				delete pUpdate;
				}

			pData->cUpdateFlag = 0;
			UNLOCK(pData);
			return 0;

	case TVM_SETCOLUMN:											// Einen Spalten-Header einstellen

			pData = GetHandle(hWnd);

			if(pData->hHeader && U(wParam)<pData->uColumnCount)
				{
				HDITEM		sItem;
				TV_COLUMN  *pCol;
				unsigned	uBit;
				unsigned	uCol;
				int			iOld;


				pCol	= (TV_COLUMN*)lParam;
				uCol	= U(wParam);
				iOld	= pData->aColumn[uCol].sReal;

				   uVal = pCol->cx;
				if(uVal==TVCF_LASTSIZE)							// Breite vor Fixierung benutzen
					{
					if(pData->aColumn[uCol].sFixed>0)
						{
						uVal = pData->aColumn[uCol].sFixed;
						}
					else{
						uVal = pData->aColumn[uCol].sReal;
						}
					}

				if(pCol->mask&TVCF_FIXED)						// Die Spalte fixieren
					{
					uBit = pCol->fmt&TVCFMT_FIXED;

					if(uBit!=0 && pData->aColumn[uCol].sFixed==0)
						{
						    pData->aColumn[uCol].sFixed=pData->aColumn[uCol].sReal;
						if(!pData->aColumn[uCol].sFixed)pData->aColumn[uCol].sFixed=100;
						}

					if(uBit==0 && pData->aColumn[uCol].sFixed!=0)
						{
						pData->aColumn[uCol].sFixed=0;
						}
					}

				sItem.mask = 0;
				if(pCol->mask&TVCF_FMT  ){sItem.mask|=HDI_FORMAT; sItem.fmt    =pCol->fmt|HDF_STRING;}
				if(pCol->mask&TVCF_IMAGE){sItem.mask|=HDI_IMAGE;  sItem.iImage =pCol->iImage;        }
				if(pCol->mask&TVCF_WIDTH){sItem.mask|=HDI_WIDTH;  sItem.cxy    =uVal; pData->aColumn[uCol].sSize=(short)sItem.cxy;}
				if(pCol->mask&TVCF_TEXT ){sItem.mask|=HDI_TEXT;   sItem.pszText=pCol->pszText;  sItem.cchTextMax=pCol->cchTextMax;}

				if(sItem.mask)
					{
					lRet=SendMessage(pData->hHeader,HDM_SETITEM,uCol,(LPARAM)&sItem);
					}
				else{
					lRet=1;
					}

				if(lRet && (pCol->mask&TVCF_FMT))				// Hat sich die Ausrichtung ver?dert
					{
					BYTE bAlign;

					switch(pCol->fmt)
						{
					case TVCFMT_CENTER: bAlign = DT_CENTER;	break;
					case TVCFMT_RIGHT:	bAlign = DT_RIGHT;	break;
					default: 			bAlign = DT_LEFT;	break;
						}

					if(pData->aColumn[uCol].bAlign!=bAlign)
						{
						pData->aColumn[uCol].bAlign = bAlign;
						UpdateColRect(pData,uCol);
						}
					}

				if(lRet && (pCol->mask&TVCF_MARK))				// Die Spalte markieren
					{
					RECT		sRect;
					unsigned	uNext;

					   uVal = (pCol->fmt&TVCFMT_MARK)? 1:0;
					if(uVal!=pData->aColumn[uCol].bMark)
						{
						pData->aColumn[uCol].bMark = (BYTE)uVal;
						pData->uMarkedCols		  +=  uVal*2-1;

						uNext		 = pData->aColumn    [uCol ].bNext;
						sRect.left   = pData->aColumnXpos[uCol ];
						sRect.right  = pData->aColumnXpos[uNext]+1;
						sRect.bottom = pData->uSizeY;
						sRect.top    = pData->uScrollY;
						InvalidateRect(pData->hWnd,&sRect,FALSE);
						}
					}

				if(lRet && (pCol->mask&TVCF_OWNERDRAW))			// Die Spalte via Callback zeichnen
					{
					RECT		sRect;
					unsigned	uNext;

					   uVal = (pCol->fmt&TVCFMT_OWNERDRAW)? 1:0;
					if(uVal!=pData->aColumn[uCol].bDraw)
						{
						pData->aColumn[uCol].bDraw = (BYTE)uVal;

						uNext		 = pData->aColumn    [uCol ].bNext;
						sRect.left   = pData->aColumnXpos[uCol ];
						sRect.right  = pData->aColumnXpos[uNext]+1;
						sRect.bottom = pData->uSizeY;
						sRect.top    = pData->uScrollY;
						InvalidateRect(pData->hWnd,&sRect,FALSE);
						}
					}
																// Variablespalten wider auf die volle L?ge zoomen
				if((pCol->mask&TVCF_RECALC) && pData->uColumnCountVar && (sItem.mask&HDI_WIDTH))
					{
					RECT	sRect;
					int		iDelta;
					int		iNum;

					if(pCol->fmt&TVCFMT_RECALC)					// Breite aus allen Spalten
						{
						for(uCol=0,iNum=0;uCol<pData->uColumnCount;uCol++)
							{
							if(pData->aColumn[uCol].bWeight!=0)
							if(pData->aColumn[uCol].sFixed ==0)
								{
								pData->aColumn[uCol].sSize = 0;
								continue;
								}

							iNum += pData->aColumn[uCol].sSize;
							}

						iDelta = pData->uSizeX-iNum;
						ChangeColSize(pData,iDelta);
						}
					else{										// Änderung einer Spalte
						iDelta = iOld-sItem.cxy;

						if((pCol->mask&TVCF_FIXED) && !iOld && sItem.cxy && !pData->aColumn[uCol].sFixed && pData->aColumn[uCol].bWeight)
							{
							pData->aColumn[uCol].sFixed = (short)sItem.cxy;
							ChangeColSize(pData,iDelta);
							pData->aColumn[uCol].sFixed = 0;
							}
						else{
							ChangeColSize(pData,iDelta);
							}
						}

					iNum = UpdateColumns(pData);
					GetClientRect(hWnd,&sRect);
					sRect.left  = iNum;
					sRect.left -= pData->uScrollX;
					sRect.top   = pData->uStartPixel;
					InvalidateRect(hWnd,&sRect,FALSE);
					}
																// Srollbar aktuallisieren
				if(pCol->mask&(TVCFMT_FIXED|TVCF_RECALC|TVCF_WIDTH))
					{
					UpdateScrollX(pData,FALSE);
					}
				}
			else{
				lRet =0;
				}

			return lRet;


	case TVM_GETCOLUMN:											// Einen Spalten-Header abfragen

			pData = GetHandle(hWnd);
			if(pData->hHeader)
				{
				HDITEM		sItem;
				TV_COLUMN  *pCol=(TV_COLUMN*)lParam;

				sItem.mask = 0;
				if(pCol->mask&TVCF_FMT  ){sItem.mask|=HDI_FORMAT;}
				if(pCol->mask&TVCF_IMAGE){sItem.mask|=HDI_IMAGE; }
				if(pCol->mask&TVCF_WIDTH){sItem.mask|=HDI_WIDTH; }
				if(pCol->mask&TVCF_TEXT ){sItem.mask|=HDI_TEXT ;sItem.pszText=pCol->pszText;  sItem.cchTextMax=pCol->cchTextMax;}

				lRet=SendMessage(pData->hHeader,HDM_GETITEM,wParam,(LPARAM)&sItem);

				pCol->mask = 0;

				if(sItem.mask&HDI_FORMAT){pCol->mask|=TVCF_FMT  ; pCol->fmt    =sItem.fmt;    }
				if(sItem.mask&HDI_IMAGE ){pCol->mask|=TVCF_IMAGE; pCol->iImage =sItem.iImage; }
				if(sItem.mask&HDI_WIDTH ){pCol->mask|=TVCF_WIDTH; pCol->cx     =sItem.cxy;    }
				if(sItem.mask&HDI_TEXT  ){pCol->mask|=TVCF_TEXT ; pCol->pszText=sItem.pszText;  pCol->cchTextMax=sItem.cchTextMax;}
				}
			else{
				lRet =0;
				}

			return lRet;

	case TVM_SETFOCUSITEM:										// Focus einstellen

			pData = GetHandle(hWnd);

			LOCK(pData);
			lRet = TreeListSetFocus(pData,U(lParam),U(wParam));
			UNLOCK(pData);

			return lRet;

	case TVM_SETCOLUMNWIDTH:									// Die Spaltenbreite einstellen

			pData = GetHandle(hWnd);

			if(pData->hHeader && U(wParam)<pData->uColumnCount && (int)lParam>=0)
				{
				HDITEM sItem;

				sItem.mask = HDI_WIDTH;
				sItem.cxy  = (int)lParam;
				lRet=SendMessage(pData->hHeader,HDM_SETITEM,wParam,(LPARAM)&sItem);
				}
			else{
				lRet=0;
				}

			return lRet;

	case TVM_COLUMNAUTOEDIT:									// AutoEdit f? eine Spalte einstellen

			pData = GetHandle(hWnd);
			uVal  = (wParam>>11)&0x3F;

			if(uVal>=pData->uColumnCount)
				{
				return 0;
				}

			uMode = (U(wParam)>>TVAE_MODEPOS)&7;

			if(uMode==TVAX_NONE)
				{
				wParam = 0;
				}
			else if(uVal==0 && uMode>=TVAX_CHECK)				// Checkboxes are not allowed in the first column
				{
				return 0;
				}

			LOCK(pData);

			uChange  = pData->aColumn[uVal].bEdit<<TVAE_MODEPOS;
			uChange |= pData->aColumn[uVal].bFlags&TVAE_STATEENABLE;

			pData->aColumn[uVal].bEdit   = (BYTE)(uMode     );
			pData->aColumn[uVal].bFlags  = (BYTE)(wParam    );
			pData->aColumn[uVal].bCbChar = (BYTE)(wParam>>17);
			pData->aColumn[uVal].bCbSize = (BYTE)(wParam>>25);
			pData->aColumn[uVal].pCbData = (void*)lParam;
			pData->aColumn[uVal].iCbIcon = -1;


			if(uMode>=TVAX_CHECK)								// Sollen Checkboxen dargestetllt werden
				{
				if(!pData->hChecks)
					{
					CreateStateImageList(pData,1);
					UpdateHeight(pData);
					}

				if((uChange^wParam)&((TVAE_MODEMASK^TVAE_CHECK^TVAE_CHECKED)|TVAE_STATEENABLE))
					{
					UpdateColRect(pData,uVal);
					}
				}
			else if((uChange&TVAE_MODEMASK)>=TVAE_CHECK)		// Waren Checkboxen dargestetllt
				{
				if((uChange^wParam)&((TVAE_MODEMASK^TVAE_CHECK^TVAE_CHECKED)|TVAE_STATEENABLE))
					{
					UpdateColRect(pData,uVal);
					}
				}

			UNLOCK(pData);

			return 1;

	case TVM_COLUMNAUTOICON:									// Icons f? AutoEdit einstellen

			pData = GetHandle(hWnd);
			uVal  = U(wParam);

			if(uVal>=pData->uColumnCount || !pData->aColumn[uVal].bEdit)
				{
				return 0;
				}

			LOCK(pData);

			pData->aColumn[uVal].iCbIcon = (int)lParam;

			UNLOCK(pData);

			return 1;

	case TVM_GETCOLUMNWIDTH:									// Die Spaltenbreite abfragen

			pData = GetHandle(hWnd);
			if(pData->hHeader)
				{
				HDITEM sItem;

				sItem.mask = HDI_WIDTH;
				sItem.cxy  = (int)lParam;
				SendMessage(pData->hHeader,HDM_GETITEM,wParam,(LPARAM)&sItem);
				lRet = sItem.cxy;
				}
			else{
				lRet=0;
				}

			return lRet;

	case TVM_CREATEDRAGIMAGE:									// Ein Drag-Image erzeugen

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet = (LRESULT)CreateDragImage(pData,U(lParam),U(wParam));
			UNLOCK(pData);
			return lRet;

	case TVM_ENDEDITLABELNOW:									// Die aktuelle Eingabe abbrechen

			pData = GetHandle(hWnd);
			LOCK(pData);
			lRet = (TreeListEndLabelEdit(pData,(wParam)? 0:1))? 1:0;
			UNLOCK(pData);
			return 0;

	case TVM_GETISEARCHSTRING:									// Holt den aktuellen Suchtext

			uDelta = GetTickCount()-uKeyLast;

			if(!lParam)
				{
				return (uDelta<=750)? uKeyPos:0;
				}

			if(uDelta>750)
				{
				((TCHAR*)lParam)[0]=0;
				return FALSE;
				}

			memcpy((TCHAR*)lParam,cKeyData,uKeyPos*sizeof(TCHAR));
			((TCHAR*)lParam)[uKeyPos]=0;
			return TRUE;

	case TVM_GETUNICODEFORMAT:									// Wird gerade UNI-Code verwendet

			#if UNICODE
			return 1;
			#else
			return 0;
			#endif

		}


return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


//*****************************************************************************
//*
//*		TreeListDraw
//*
//*****************************************************************************
//	Zeichnet das Fenster
static void TreeListDraw(HWND hWnd,HDC hDc,RECT *pRect)
{
COLORREF		uEcColor;
COLORREF		uEvColor;
COLORREF		uBkColor;
COLORREF		uBtColor;
COLORREF		uOdColor;
COLORREF		uOcColor;
COLORREF		uFrColor;
COLORREF		uInColor;
COLORREF		uOldColor;
COLORREF		uOutColor;
COLORREF		uNextColor;
COLORREF		uTempColor;
HRGN			hRgnMain;
HRGN			aRgn[MAX_COLUMNS+1];
SIZE			sSize;
RECT			sRect;
RECT			sArea;
RECT			sDummy;
RECT			sButton;
TreeListData   *pData;
BaseItem	   *pTemp;
BaseItem	   *pEntry;
ExtraItem	   *pExtra;
LPCTSTR			pText;
HIMAGELIST		hImgList;
unsigned		uTextSize;
unsigned		uRgnCount;
unsigned		uAutoMask;
unsigned		uFirstPos;
unsigned		uStyleEx;
unsigned		uColMark;
unsigned		uColumn;
unsigned		uState;
unsigned		uStyle;
unsigned		uExtra;
unsigned		uNext;
unsigned		uMode;
unsigned		uMark;
unsigned		uItem;
unsigned		uBits;
unsigned		uLine;
unsigned		uOdd;
unsigned		uNow;
unsigned		uPos;
unsigned		uMax;
int			   *pOffsets;
int				iRnType[MAX_COLUMNS+1];
int				iXscroll;
int				iHeight;
int				iIndent;
int				iDelta;
int				iImage;
int				iShift;
int				iStart;
int				iCount;
int				iLevel;
int				iLast;
int				iSize;
int				iXpos;
int				iYpos;
int				iMaxX;
int				iValT;
int				iValB;
int				iDiff;
int				iAdd;
int				i;





	pData=GetHandle(hWnd);

	LOCK(pData);

	GetClientRect(hWnd,&sRect);

	if(!pRect)pRect=&sRect;

	iXscroll = -(int)pData->uScrollX;
	pOffsets = pData->aColumnXpos;
	hRgnMain = CreateRectRgn(pRect->left,pRect->top,pRect->right,pRect->bottom);

		uMax = pData->uColumnCount;
	if(!uMax)
		{
			aRgn   [  0 ] = CreateRectRgn(sRect.left,sRect.top,sRect.right,sRect.bottom);
			iRnType[  0 ] = CombineRgn(aRgn[0],aRgn[0],hRgnMain,RGN_AND);
			uRgnCount	  = 1;
		}
	else{
		for(uPos=0;uPos<uMax;uPos++)
			{
			uExtra = pData->aColumnPos[uPos  ];
			uNext  = pData->aColumnPos[uPos+1];
			aRgn   [uExtra] = CreateRectRgn(sRect.left+pOffsets[uExtra]+iXscroll,sRect.top,sRect.left+pOffsets[uNext]+iXscroll,sRect.bottom);
			iRnType[uExtra] = CombineRgn(aRgn[uExtra],aRgn[uExtra],hRgnMain,RGN_AND);
			}

		aRgn   [uPos] = CreateRectRgn(sRect.left+pOffsets[uPos]+iXscroll,sRect.top,sRect.right,sRect.bottom);
		iRnType[uPos] = CombineRgn(aRgn[uPos],aRgn[uPos],hRgnMain,RGN_AND);

		uRgnCount = uMax+1;
		}



	iHeight		  = pData->iRowHeight;
	uStyleEx 	  = pData->uStyleEx;
	uStyle	 	  = pData->uStyle;
	iIndent	 	  = pData->iIndent;
	iShift	 	  = pData->iShift;
	uPos	 	  = pData->uScrollY;
	uMax	 	  = pData->uItemPosCount+uPos;
	uNext		  =(pData->uColumnCount<=1)? 1:pData->aColumn[1].bIndex;
	uFirstPos     = pData->aColumnXpos[uNext];

	if(iRnType[0]==NULLREGION)
			iMaxX = pData->iMaxSizeX;
	else	iMaxX = uFirstPos-1;

	if(uStyleEx&TVS_EX_ITEMLINES)
		{
		iHeight--;
		}

	if(uStyleEx&TVS_EX_AUTOEXPANDICON)
		{
		uAutoMask = TVIS_EXPANDED;
		}
	else{
		uAutoMask = 0;
		}

	if(uMax>pData->uItemPosCount)
		{
		uMax = pData->uItemPosCount;
		}


	uBkColor  = pData->aColors[TVC_BK   ];
	uFrColor  = pData->aColors[TVC_FRAME];


	if(pData->uStyleEx&TVS_EX_ALTERNATECOLOR)					// Abwechselnde Farben
		{
		uOdColor = pData->aColors[TVC_ODD    ];
		uEvColor = pData->aColors[TVC_EVEN   ];
		uOcColor = pData->aColors[TVC_COLODD ];
		uEcColor = pData->aColors[TVC_COLEVEN];
		}
	else{
		uOdColor = uBkColor;
		uEvColor = uBkColor;
		uOcColor = pData->aColors[TVC_COLBK];
		uEcColor = pData->aColors[TVC_COLBK];
		}


	if(!pData->cIsEnabled)										// Wenn Fenster gessperrt grau zeichnen
	if( pData->uStyleEx&TVS_EX_GRAYEDDISABLE)
		{
		uBkColor = pData->aColors[TVC_GRAYED];
		uEvColor = uEcColor;
		uOdColor = uOcColor;
		}

	uInColor = pData->aColors[TVC_LINE];
	uBtColor = pData->aColors[TVC_BOX ];
	iStart	 = 0;
	iLast	 = 0;



	sArea.top = sRect.top+pData->uStartPixel;
	SelectObject(hDc,pData->hFontN);
	SelectObject(hDc,hPatternPen);
	SetBkColor	(hDc,uBkColor);
	SetBkMode   (hDc,TRANSPARENT);
	SetTextAlign(hDc,TA_LEFT|TA_TOP);
	SetTextColor(hDc,pData->aColors[TVC_TEXT]);


//******************** Eintr?e zeichnen **************************************


	for(uLine=0;uPos<uMax;uPos++,uLine++)						// Alle Eintr?e ausgeben
		{
		if(uLine<pData->uFixedLines)
			{
			uItem = pData->pItemPos[uLine];
			uOdd  = uLine&1;
			uNow  = uLine;
			}
		else{
			uItem = pData->pItemPos[uPos ];
			uOdd  = uPos&1;
			uNow  = uPos;
			}

			pEntry	= pData->pTreeItems[uItem];
		if(!pEntry)break;

		if((pEntry->uState&TVIS_SELECTED) && (uStyleEx&TVS_EX_FULLROWMARK))
			{
			if(uStyleEx&TVS_EX_ALTERNATECOLOR)
					uOutColor = (uPos&1)? pData->aColors[TVC_MARKODD]:pData->aColors[TVC_MARKEVEN];
			else	uOutColor = pData->aColors[TVC_MARK];

			uMark     = (unsigned)~TVIS_BKCOLOR;
			uColMark  =  0;
			}
		else if(uOdd)											// Farbe wechselweise ?dern
			{
			uColMark  =  pData->aColumn[0].bMark;
			uOutColor = (uColMark)? uOcColor:uOdColor;
			uMark     = 0xFFFFFFFF;
			}
		else{
			uColMark  =  pData->aColumn[0].bMark;
			uOutColor = (uColMark)? uEcColor:uEvColor;
			uMark     = 0xFFFFFFFF;
			}

		sArea.bottom  = sArea.top+pData->iRowHeight;
		sArea.left	  = iXscroll;
		iLevel		  = pEntry->uLevel;

		if(iRnType[0]==NULLREGION)
			{
			goto ExtraDraw;
			}

		uBits		=  pEntry->uState&0xFFFF;
		uBits      |=  pEntry->bFlags<<16;
		uBits      &=  uMark;
		iImage		= (uBits&LVIS_SELECTED)? pEntry->iSelectedImage:pEntry->iImage;
		pText		=  pEntry->pText;
		uTextSize	=  pEntry->uTextSize;

		if(pData->uSelectedSub && uItem==pData->uSelectedItem)
			{
			if(pData->uSelectedCount<=1 || !(pData->uStyleEx&TVS_EX_SUBSELECT))
				{
				uBits &= ~TVIS_SELECTED;
				}
			}

		if(pEntry->bCallback)
			{
			CallbackEntry(pData,pEntry,uItem,pEntry->bCallback,iImage,uTextSize,pText);

				pEntry	= pData->pTreeItems[uItem];
			if(!pEntry)break;
			}

		SelectObject(hDc,aRgn[0]);

		if((uStyleEx&(TVS_EX_ITEMLINES|TVS_EX_FULLROWITEMS)) == (TVS_EX_ITEMLINES|TVS_EX_FULLROWITEMS))
			{
			sButton.left	= 0;
			sButton.right	= iIndent*(iLevel+1)+2;
			sButton.bottom	= sArea.bottom+1;
			sButton.top		= sArea.bottom-2;

			SetBkColor (hDc,uFrColor);
			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);
			}

		if(pData->aColumn[0].bMark)								// Ist die erste Spalte markiert
			{
			uBkColor = pData->aColors[TVC_COLBK];
			}

		SetBkColor(hDc,(uStyleEx&TVS_EX_FULLROWITEMS)? uOutColor:uBkColor);

		if(pData->cHasRootRow)iLevel++;

		if(iLevel<=0)goto NoRootLines;

		if(uStyle&(TVS_HASBUTTONS|TVS_HASLINES))
			{
			iLevel--;
			}

		if(uStyleEx&TVS_EX_FULLROWITEMS)
			{
			sArea.bottom--;
			iAdd = 1;
			}
		else{
			iAdd = 0;
			}

		if(uStyle&TVS_HASLINES)
			{
			pTemp		= pData->pTreeItems[pEntry->uParent];
			sArea.right = sArea.left+1;							// Eine leerer Pixelreihe am Anfang
			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
			sArea.left += iIndent*iLevel+1;


			for(i=iLevel;i>0;i--)								// Bereich vor Schaltfl?hen
				{
				sArea.right = sArea.left;
				sArea.left -= iIndent;

				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				iXpos = sArea.left+iShift;

				if(pTemp)
					{
					if(pTemp->bFlags&TVIX_LINE)					// Zeichne vertikale Linien
						{
						MoveToEx(hDc,iXpos,sArea.top|1,NULL);
						LineTo  (hDc,iXpos,sArea.bottom+iAdd);
						}

					pTemp = pData->pTreeItems[pTemp->uParent];
					}
				}

			sArea.left += iIndent*iLevel;
			}
		else{													// Ohne Linien zeichnen
			if(iLevel>0)
				{
				sArea.right = sArea.left+iIndent*iLevel;
				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
				sArea.left += sArea.right;
				}
			}


		if(uStyle&TVS_HASBUTTONS)								// Fenster mit Schaltfl?hen ?
			{
			sArea.right = sArea.left+iIndent;
			iXpos		= sArea.left+iShift;
			iYpos		= sArea.top+pData->iRowHeight/2;

			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

			if(uStyle&TVS_HASLINES)								// Linien unter Schaltfl?hen
				{
				MoveToEx(hDc,iXpos,sArea.top|1,NULL);

				if(pEntry->bFlags&TVIX_LINE)
						LineTo(hDc,iXpos,sArea.bottom+iAdd);
				else	LineTo(hDc,iXpos,iYpos+1);


				MoveToEx(hDc,iXpos+1+(iYpos&1),iYpos,NULL);
				LineTo  (hDc,sArea.right      ,iYpos);
				}

			if(pEntry->bFlags&TVIX_HASBUTTON)					// Schaltfl?hen zeichnen
				{
				sButton.left    = iXpos-4;
				sButton.top	    = iYpos-4;
				sButton.right   = iXpos+5;
				sButton.bottom  = iYpos+5;

				if(pData->cGlyphOk)								// Thema benutzen
					{
					uState = ((uBits^TVIS_EXPANDED)&(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))? GLPS_CLOSED:GLPS_OPENED;
					pDrawThemeBackg(pData->hTheme,hDc,TVP_GLYPH,uState,&sButton,0);
					}
				else{
					SetBkColor (hDc,uBtColor);
					ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);

					sButton.left   += 1;
					sButton.top	   += 1;
					sButton.right  -= 1;
					sButton.bottom -= 1;

					SetBkColor (hDc,pData->aColors[TVC_BOXBG]);
					ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);

					sButton.left    = iXpos-2;
					sButton.top	    = iYpos  ;
					sButton.right   = iXpos+3;
					sButton.bottom  = iYpos+1;

					SetBkColor (hDc,uInColor);
					ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);

																// '+' statt '-' Schaltfl?he zeichnenen
					if((uBits^TVIS_EXPANDED)&(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))
						{
						sButton.left    = iXpos  ;
						sButton.top	    = iYpos-2;
						sButton.right   = iXpos+1;
						sButton.bottom  = iYpos+3;
						ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);
						}
					}

				SetBkColor(hDc,uBkColor);
				}


			sArea.left += iIndent;
			}
		else if(uStyle&TVS_HASLINES)							// Nur Linien zeichnen ohne Schaltfl?hen
			{
			sArea.right = sArea.left+iIndent;
			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

			iYpos	= sArea.top+pData->iRowHeight/2;
			iXpos	= sArea.left+iShift;
			MoveToEx(hDc,iXpos,sArea.top|1,NULL);

			if(pEntry->uNextItem)
					LineTo(hDc,iXpos,sArea.bottom);
			else	LineTo(hDc,iXpos,iYpos+1);


			MoveToEx(hDc,iXpos+1+(iYpos&1),iYpos,NULL);
			LineTo  (hDc,sArea.right      ,iYpos);

			sArea.left += iIndent;
			}

		if(uStyleEx&TVS_EX_FULLROWITEMS)sArea.bottom++;

NoRootLines:

		if(uStyleEx&TVS_EX_ITEMLINES)							// Linien um den Eintrag zeichnen
			{
			iAdd		= 1;
			sArea.right = sArea.left+1;

			if(uStyleEx&TVS_EX_FULLROWITEMS)
				{
				iStart = sArea.left;
				iAdd   = 0;
				}
			else if(iLevel>=0)
				{
				SetBkColor (hDc,uFrColor);
				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				sArea.left++;
				sArea.bottom--;
				iStart=sArea.left;
				}
			else{
				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				sArea.left++;
				sArea.bottom--;
				iStart=sArea.left-1;
				}
			}
		else{
			iAdd = 0;
			}

		SetBkColor  (hDc,(uBits&TVIS_BKCOLOR)? pEntry->uColorBk:uOutColor);
		SelectObject(hDc,(uBits&TVIS_BOLD   )? pData->hFontB:pData->hFontN);

		if(pData->hStates)										// State-Icons anzeigen
			{
			sArea.right		= sArea.left+pData->iStatesSizeX;
			iYpos			= sArea.top+(iHeight-pData->iStatesSizeY)/2;
			i				= (uBits&LVIS_STATEIMAGEMASK)>>12;

			sArea.right    += iAdd;
			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
			sArea.left     += iAdd;


			if(pData->hStates==THEMEIMGLIST)					// Mit Thema zeichnen
				{
				if(pData->uStyleEx&TVS_EX_BITCHECKBOX)i++;

				if(i>=1 && i<=2)
					{
					if(pData->uCheckedItem!=uItem || pData->uCheckedSub)
						{
						uState = (i==1)? CBS_UNCHECKEDNORMAL:CBS_CHECKEDNORMAL;
						}
					else if(pData->uCheckedDown)
						{
						uState = (i==1)? CBS_UNCHECKEDPRESSED:CBS_CHECKEDPRESSED;
						}
					else{
						uState = (i==1)? CBS_UNCHECKEDHOT:CBS_CHECKEDHOT;
						}

					   iDiff = sArea.bottom-sArea.top;
					   iDiff-= pData->iStatesSizeY;
					if(iDiff > 0)
						{
						iValT	      = sArea.top;
						iValB		  = sArea.bottom;
						sArea.top	 += (iDiff+1)/2;
						sArea.bottom  = sArea.top+pData->iStatesSizeY;

						pDrawThemeBackg(pData->hThemeBt,hDc,BP_CHECKBOX,uState,&sArea,0);

						sArea.top	  = iValT;
						sArea.bottom  = iValB;
						}
					else{
						pDrawThemeBackg(pData->hThemeBt,hDc,BP_CHECKBOX,uState,&sArea,0);
						}
					}
				}
			else{
				ImageList_Draw(pData->hStates,i,hDc,sArea.left,iYpos,ILD_TRANSPARENT);
				}

			sArea.left     += pData->iStatesSizeX;
			iAdd		    = 0;
			}

		if(pData->hImages && iImage!=TV_NOIMAGE)				// Icon zeichnen vom Haupteintrag
			{
			if(iImage>=TV_SECONDICON)							// Sub-Image-Liste verwenden
				{
				if(iImage&TV_NOAUTOEXPAND)
					{
					iImage     &= ~TV_NOAUTOEXPAND;				// Kein Auto-Expant bei diesem Icon
					}
				else if((pEntry->uState&uAutoMask) && pEntry->uFirstChild)
					{
					iImage     += 1;							// Auto-Expant aktivieren
					}

				iImage		   -= TV_SECONDICON;
				sArea.right		= sArea.left+pData->iSubImgXsize;
				iYpos			= sArea.top+(iHeight-pData->iSubImgYsize)/2;
				pEntry->bFlags |= TVIX_HASIMAGE;

				sArea.right    += iAdd;
				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
				sArea.left     += iAdd;
				ImageList_Draw(pData->hSubImg,iImage,hDc,sArea.left,iYpos,ILD_TRANSPARENT|(uBits&(TVIS_OVERLAYMASK|LVIS_CUT)));

				sArea.left     += pData->iSubImgXsize;
				iAdd		    = 0;
				}
			else{												// Haup-Image-Liste verwenden
				if(iImage&TV_NOAUTOEXPAND)
					{
					iImage     &= ~TV_NOAUTOEXPAND;				// Kein Auto-Expant bei diesem Icon
					}
				else if((pEntry->uState&uAutoMask) && pEntry->uFirstChild)
					{
					iImage	   += pData->iAutoAdd;				// Auto-Expant aktivieren
					}

				sArea.right		= sArea.left+pData->iImagesSizeX;
				iYpos			= sArea.top+(iHeight-pData->iImagesSizeY)/2;
				pEntry->bFlags |= TVIX_HASIMAGE;

				sArea.right    += iAdd;
				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
				sArea.left     += iAdd;
				ImageList_Draw(pData->hImages,iImage,hDc,sArea.left,iYpos,ILD_TRANSPARENT|(uBits&(TVIS_OVERLAYMASK|LVIS_CUT)));

				sArea.left     += pData->iImagesSizeX;
				iAdd		    = 0;
				}
			}
		else{
			pEntry->bFlags &= ~TVIX_HASIMAGE;
			}

		sArea.right = uFirstPos;								// Text ausgeben vom Haupteintrag
		iYpos		= sArea.top+(iHeight-pData->iFontHeight)/2;
		uMode		= ETO_OPAQUE;

																// Feld via Callback zeichnen
		if(pData->aColumn[0].bDraw && DrawItem(pData,hDc,0,uNow,uItem,uMode,uBits,pText,uTextSize,pEntry->uColorText,pEntry->lParam,&sArea))
			{
																// Das Feld wurde via callback geszeichnet
			}
		else if(uBits&(TVIS_SELECTED|TVIS_DROPHILITED|TVIS_UNTERLINE|TVIS_TRACKED|TVIS_TEXTCOLOR|TVIS_FOCUSED))
			{													// Das Feld speziel zeichnen
			LPTSTR	pPtr;
			LPINT	pPos;

			AllocTemp(pData,&pPos,&pPtr,uTextSize);
			ExtTextOutW(hDc,0,0,uMode,&sArea,NULL,0,NULL);

			sButton.top		= iYpos;
			sButton.left    = sArea.left+4;
			sButton.right	= sArea.right;
			sButton.bottom	= iYpos+pData->iFontHeight+2;


			if(!uTextSize)										// Feld ohne Text ?
				{
				sButton.right-=2;
				sButton.bottom--;
				pEntry->iTextPixels = 0;
				}
			else{
				if(pData->uStyleEx&TVS_EX_FULLROWMARK)
				if(pData->cHasFocus==0 || uItem!=pData->uSelectedItem || pData->uSelectedSub)
				if((uBits&TVIS_SELECTED) && !(uBits&(TVIS_DROPHILITED|TVIS_FOCUSED)))
					{
					sButton.left-=1;
					}

				DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
				pEntry->iTextPixels = sButton.right-sButton.left;
				}

																// Passt der Text in die Spalte
			if(sButton.left+pEntry->iTextPixels>=(int)(sArea.right-pData->uScrollX))
				{
				   iSize  = sArea.right-pData->uScrollX-sButton.left-2;
				   iSize -= (uBits&TVIS_BOLD)? pData->uTrippleB:pData->uTrippleN;
				if(iSize<3)
					{
					iCount = 0;
					}
				else{
					GetTextExtentExPoint(hDc,pText,uTextSize,iSize,&iCount,pPos,&sSize);
					}
																// Tempor?en Text mit "..." erzeugen
				memcpy(pPtr       ,pText,iCount*sizeof(TCHAR));
				memcpy(pPtr+iCount,_T("..."),4 *sizeof(TCHAR));

				pText		  = pPtr;
				uTextSize	  = iCount+3;
				sButton.right = sArea.right-2;
				}

																// Das Feld selektiert zeichnen
			if((uBits&TVIS_SELECTED) && pData->cHasFocus && uItem==pData->uSelectedItem && !pData->uSelectedSub)
				{
				uTempColor=GetSysColor(COLOR_HIGHLIGHTTEXT);
				SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
				Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
				}
			else{
				if(uBits&TVIS_DROPHILITED)
					{
					uTempColor =     GetSysColor     (COLOR_HIGHLIGHTTEXT);
					SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT   ));
					SelectObject(hDc,GetStockObject  (NULL_PEN          ));
					Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
					}
				else if(uBits&TVIS_SELECTED)					// Ist das Feld ohne Focus ausgew?lt
					{
					if(pData->cHasFocus)
						{
						uTempColor =     GetSysColor     (COLOR_HIGHLIGHTTEXT);
						SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
						SelectObject(hDc,GetStockObject  (NULL_PEN       ));
						}
					else{
						if(uBits&TVIS_TEXTCOLOR)
								uTempColor = pEntry->uColorText;
						else	uTempColor = pData ->aColors[TVC_TEXT];

						if(pData->uStyleEx&TVS_EX_FULLROWMARK)
							{
							SelectObject(hDc,GetStockObject(NULL_BRUSH));
							}
						else{
							if(pData->uStyleEx&TVS_EX_ALTERNATECOLOR)
								{
								SetDCBrushColor(hDc,pData->aColors[TVC_NOFOCUS]);
								SelectObject(hDc,GetStockObject(DC_BRUSH));
								}
							else{
								SelectObject(hDc,GetSysColorBrush(COLOR_3DFACE));
								}

							SelectObject(hDc,GetStockObject(NULL_PEN));
							}
						}

					Rectangle(hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
					}
				else{											// Das Feld normal zeichnen
						 if(uBits&TVIS_TRACKED  )uTempColor = pData ->aColors[TVC_TRACK];
					else if(uBits&TVIS_TEXTCOLOR)uTempColor = pEntry->uColorText;
					else						 uTempColor = pData ->aColors[TVC_TEXT ];
					sButton.right--;
					sButton.left --;
					}

				SelectObject(hDc,hPatternPen);

				if(uBits&TVIS_FOCUSED)							// Einen punktierten Rahmen um den Text zeichnen
					{
					SelectObject(hDc,GetStockObject(NULL_BRUSH));
					Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
					}
				}

			SetTextColor(hDc,uTempColor);
			sButton.left+=pData->iFontOff;
			DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);


			if(uBits&(TVIS_UNTERLINE|TVIS_TRACKED))				// Text unterstreichen
			if(pText && *pText)
				{
				sButton.left   -= pData->iFontOff;
				sButton.right  -= pData->iFontOff;
				sButton.top	   += pData->iFontLine;
				sButton.bottom  = sButton.top+1;
				uOldColor=SetBkColor(hDc,uTempColor);
				ExtTextOutW(hDc,0,0,uMode,&sButton,NULL,0,NULL);
				SetBkColor (hDc,uOldColor);
				}

			SetTextColor(hDc,pData->aColors[TVC_TEXT]);
			}
		else{													// Das Feld normal ausgeben
			if(!pEntry->iTextPixels && uTextSize)
				{
				sButton.top		= iYpos;
				sButton.left    = sArea.left+4;
				sButton.right	= sArea.right;
				sButton.bottom	= iYpos+pData->iFontHeight+2;
																// Textbreite berechen
				DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);

				pEntry->iTextPixels = sButton.right-sButton.left;
				}

			uMode |= ETO_CLIPPED;
																// Ist der Text größer als die Spalte
			if(sArea.left+pEntry->iTextPixels>=(int)(sArea.right-pData->uScrollX))
				{
				LPINT	pPos;

				AllocTemp(pData,&pPos,NULL,uTextSize);

				   iSize  = sArea.right-sArea.left-pData->uScrollX;
				   iSize -= (uBits&TVIS_BOLD)? pData->uTrippleB:pData->uTrippleN;
				if(iSize<3)
					{
					iCount = 0;
					}
				else{
					GetTextExtentExPoint(hDc,pText,uTextSize,iSize,&iCount,pPos,&sSize);
					}

				if(iCount>0)									// Passen noch Buchstaben vor "..."
					{
					sButton.right	= sArea.right;
					sArea.right		= sArea.left+2+pPos[iCount-1];

					ExtTextOut(hDc,sArea.left+2,iYpos,uMode,&sArea,pText,iCount,NULL);

					sArea.left		= sArea.right;
					sArea.right		= sButton.right;

					ExtTextOutW(hDc,sArea.left  ,iYpos,uMode,&sArea,L"...",3,NULL);
					}
				else{
					ExtTextOutW(hDc,sArea.left+2,iYpos,uMode,&sArea,L"...",3,NULL);
					}
				}
			else{
				ExtTextOut(hDc,sArea.left+2,iYpos,uMode,&sArea,pText,uTextSize,NULL);
				}
			}

		   i  = sArea.left-iXscroll;
		   i += pEntry->iTextPixels+5;
		if(i>iMaxX)iMaxX=i;

		if(uStyleEx&TVS_EX_ITEMLINES)							// Linien um den Eintrag zeichnen
			{
			SetBkColor(hDc,uFrColor);

			if(iLast>iStart)									// Ist das Feld nach links einger?kt gegen?er dem oberen
				{
				i = sArea.right;
				sArea.top--;
				sArea.bottom = sArea.top+1;
				sArea.left	 = iStart-1;
				sArea.right	 = iLast;
				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
				sArea.right  = i;
				sArea.top++;
				}

			iLast		  = iStart;								// Linie unter Feld zeichnen
			sArea.top    += iHeight;
			sArea.left	  = iStart;
			sArea.bottom  = sArea.top+1;

			if(uLine+1==pData->uFixedLines && uPos!=uLine+pData->uFixedAlways)
				{
				SetBkColor(hDc,pData->aColors[TVC_FIXED]);
				sArea.left = 0;
				iLast      = 0;
				}

			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

			sArea.top    -= iHeight;
			SetBkColor(hDc,uFrColor);
			}													// Trennlinie bei fixen Breeich
		else if(uLine+1==pData->uFixedLines && uPos!=uLine+pData->uFixedAlways)
			{
			sDummy.top    = sArea .top+iHeight-2;
			sDummy.bottom = sDummy.top+1;
			sDummy.right  = sArea .right;
			sDummy.left	  = 0;

			SetBkColor (hDc,pData->aColors[TVC_FIXED]);
			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sDummy,NULL,0,NULL);
			}

		if(pData->uLineItem==uItem)								// Eine Markierungslinie zeichnen
			{
			if(uLine+1==pData->uFixedLines)SetBkColor(hDc,uFrColor);
			SelectObject(hDc,hPatternBrush);
			PatBlt      (hDc,0,sArea.top,pData->aColumn[0].sReal+1,pData->uLineHeight,PATINVERT);
			}

		//************ Extraeintr?e zeichnen *********************************

ExtraDraw:

		uNextColor		 = uOutColor;							// Hintergrundfarbe f? die n?hste Spalte

		if(pData->aColumn[0].bMark)								// Ist die erste Spalte markiert
			{
			uBkColor = pData->aColors[TVC_BK];
			}

		for(uColumn=1;uColumn<=pData->uColumnCount;uColumn++)	// Extra Spalten zeichnen
			{
			uExtra = pData->aColumnPos[uColumn  ];
			uNext  = pData->aColumnPos[uColumn+1];

			if(pData->aColumn[uExtra].sReal==0)					// Ist die Spalte sichtbar
			if(uColumn<pData->uColumnCount)
				{
				continue;
				}

			if(uColMark!=pData->aColumn[uExtra].bMark)			// Ist die Spalte anderst markiert
			if(!(pEntry->uState&TVIS_SELECTED) || !(uStyleEx&TVS_EX_FULLROWMARK))
				{
				if(uPos&1)
					{
					uColMark  =  pData->aColumn[uExtra].bMark;
					uOutColor = (uColMark)? uOcColor:uOdColor;
					}
				else{
					uColMark  =  pData->aColumn[uExtra].bMark;
					uOutColor = (uColMark)? uEcColor:uEvColor;
					}
				}

			GetRgnBox(aRgn[uExtra],&sButton);

			if(iRnType[uExtra]==NULLREGION)continue;

			SelectObject(hDc,aRgn[uExtra]);

			sArea.left  = pData->aColumnXpos[uExtra];
			sArea.left += iXscroll;


			if(uStyleEx&TVS_EX_ITEMLINES)						// Linie um den Eintrag zeichnen
				{
				SetBkColor(hDc,uFrColor);
																// Linke Linie
				sArea.right   = sArea.left+1;
				sArea.bottom += 1;

				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				sArea.left   += 1;								// Untere Linie
				sArea.top    += iHeight;
				sArea.bottom  = sArea.top+1;
				sArea.right	  = pData->aColumnXpos[uNext];
				if(uColumn<pData->uColumnCount)sArea.right+=iXscroll;

				if(uLine+1==pData->uFixedLines && uPos!=uLine+pData->uFixedAlways)
					{
					SetBkColor(hDc,pData->aColors[TVC_FIXED]);
					}

				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				sArea.top    -= iHeight;
				sArea.bottom -= 1;
				iAdd		  = 1;
				}

			if(sArea.left>(int)pData->uSizeX)break;				// Noch im sichtbaren Bereich

			sArea.right		 = pData->aColumnXpos[uNext];


			if(uColumn<pData->uColumnCount)						// Ist es die letze Spalte ?
				{
				sArea.right += iXscroll;
				pExtra       = pData->pExtraItems[uExtra-1][uItem];
				if(!pExtra)uNextColor=uOutColor;
				}
			else{
				pExtra		 = 0;
				}

			if(!pExtra)											// Leeres Feld zeichnen
				{
				SetBkColor(hDc,uNextColor);
																// Feld via Callback zeichnen
				if(!pData->aColumn[uExtra].bDraw || !DrawItem(pData,hDc,uExtra,uNow,uItem,uMode,uBits,NULL,0,0,pEntry->lParam,&sArea))
					{
					ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
					}
				}
			else{
				iSize		 =  pData->iSubImgXsize;
				hImgList	 =  pData->hSubImg;
				iImage		 =  pExtra->iImage;
				pText		 =  pExtra->pText;
				uTextSize	 =  pExtra->uTextSize;
				uBits		 = (pExtra->uState&0xFFFF);
				uBits	    |= (pExtra->bFlags<<16);
				uBits	    |=  pEntry->uState&TVIS_BASEFLAGS;
				uBits	    &=  uMark;

				if(uExtra!=pData->uSelectedSub)
					{
					uBits &= ~TVIS_SELECTED;
					}

				if(pExtra->bCallback)							// Text ?er Callback holen
					{
					CallbackExtra(pData,pEntry,pExtra,uItem,uExtra,pExtra->bCallback,iImage,uTextSize,pText);
						pExtra = pData->pExtraItems[uExtra-1][uItem];
					if(!pExtra)break;
					}

				uNextColor   = (uBits&TVIS_BKCOLOR)? pExtra->uColorBk:uOutColor;
				SetBkColor(hDc,uNextColor);


				if(pData->aColumn[uExtra].bEdit>=TVAX_CHECK)	// Checkboxen statt Icons
					{
					hImgList =  pData->hChecks;
					iSize	 =  pData->iChecksSizeX;
					iImage   = (pExtra->uState&TVIS_STATEIMAGEMASK)>>12;
					uBits   &= ~TVIS_OVERLAYMASK;

					if(iImage&8)
					if(pData->aColumn[uExtra].bFlags&TVAE_STATEENABLE)
						{
						iImage &= 7;
						}
					}

				if(hImgList && iImage>TV_NOIMAGE)				// Icon in Extraeintrag zeichnen
					{
					sArea.right     = sArea.left+iSize+2;
					iYpos		    = sArea.top+(iHeight-iSize)/2;
					pExtra->bFlags |= TVIX_HASIMAGE;

					SelectObject(hDc,(uBits&TVIS_BOLD)? pData->hFontB:pData->hFontN);
					ExtTextOutW (hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);


					if(hImgList==THEMEIMGLIST)					// Mit Thema zeichnen
						{
						if(pData->uStyleEx&TVS_EX_BITCHECKBOX)iImage++;

						if(iImage==1 || iImage==2)
							{
							if(pData->uCheckedItem!=uItem || pData->uCheckedSub!=uExtra)
								{
								uState = (iImage==1)? CBS_UNCHECKEDNORMAL:CBS_CHECKEDNORMAL;
								}
							else if(pData->uCheckedDown)
								{
								uState = (iImage==1)? CBS_UNCHECKEDPRESSED:CBS_CHECKEDPRESSED;
								}
							else{
								uState = (iImage==1)? CBS_UNCHECKEDHOT:CBS_CHECKEDHOT;
								}

							   iDiff = sArea.bottom-sArea.top;
							   iDiff-= pData->iChecksSizeY;
							if(iDiff > 0)
								{
								iValT	      = sArea.top;
								iValB		  = sArea.bottom;
								sArea.top	 += (iDiff+1)/2;
								sArea.bottom  = sArea.top+pData->iChecksSizeY;

								pDrawThemeBackg(pData->hThemeBt,hDc,BP_CHECKBOX,uState,&sArea,0);

								sArea.top	  = iValT;
								sArea.bottom  = iValB;
								}
							else{
								pDrawThemeBackg(pData->hThemeBt,hDc,BP_CHECKBOX,uState,&sArea,0);
								}
							}
						}
					else{
						ImageList_Draw(hImgList,iImage,hDc,sArea.left+1,iYpos,ILD_TRANSPARENT|(uBits&(TVIS_OVERLAYMASK|LVIS_CUT)));
						}

					sArea.left  += iSize+1;
					sArea.right  = pData->aColumnXpos[uNext];
					sArea.right += iXscroll;
					}
				else{
					pExtra->bFlags &= ~TVIX_HASIMAGE;
					}

				iYpos = sArea.top+(iHeight-pData->iFontHeight)/2;
				SelectObject(hDc,(uBits&TVIS_BOLD)? pData->hFontB:pData->hFontN);
				uMode = ETO_OPAQUE;

																// Feld via Callback zeichnen
				if(pData->aColumn[uExtra].bDraw && DrawItem(pData,hDc,uExtra,uNow,uItem,uMode,uBits,pText,uTextSize,pExtra->uColorText,pEntry->lParam,&sArea))
					{
																// Das Feld wurde via callback geszeichnet
					}
				else if(uBits&(TVIS_SELECTED|TVIS_DROPHILITED|TVIS_UNTERLINE|TVIS_TRACKED|TVIS_TEXTCOLOR|TVIS_FOCUSED))
					{											// Text ausgeben in spezilem Format
					LPTSTR	pPtr;
					LPINT	pPos;

					AllocTemp(pData,&pPos,&pPtr,uTextSize);
					ExtTextOutW(hDc,0,0,uMode,&sArea,NULL,0,NULL);

					sButton.top		= iYpos;
					sButton.left    = sArea.left+4;
					sButton.right	= sArea.right;
					sButton.bottom	= iYpos+pData->iFontHeight+2;

					if(!uTextSize)
						{
						sButton.left--;
						sButton.right-=2;
						sButton.bottom--;
						pExtra->iTextPixels = 0;
						}
					else{
						if(pData->uStyleEx&TVS_EX_FULLROWMARK)
						if(pData->cHasFocus==0 || uItem!=pData->uSelectedItem)
						if((uBits&TVIS_SELECTED) && !(uBits&(TVIS_DROPHILITED|TVIS_FOCUSED)))
							{
							sButton.left-=2;
							}

						DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
						pExtra->iTextPixels = sButton.right-sButton.left;
						}

																// Passt der Text in die Spalte
					if(sButton.left+pExtra->iTextPixels>=sArea.right)
						{
						if(uTextSize>253)uTextSize=253;

						   iSize  = sArea.right-sButton.left-2;
						   iSize -= (uBits&TVIS_BOLD)? pData->uTrippleB:pData->uTrippleN;
						if(iSize<3)
							{
							iCount = 0;
							}
						else{
							GetTextExtentExPoint(hDc,pText,uTextSize,iSize,&iCount,pPos,&sSize);
							}

						memcpy(pPtr       ,pText,iCount*sizeof(TCHAR));
						memcpy(pPtr+iCount,_T("..."),4 *sizeof(TCHAR));

						pText		  = pPtr;
						uTextSize	  = iCount+3;
						sButton.right = sArea.right-2;
						}


					switch(pData->aColumn[uExtra].bAlign)		// Textausrichtung ausgleichen
						{
					case DT_CENTER:	iDelta	= sArea  .right-sArea  .left;
									iDelta -= sButton.right-sButton.left;
									iDelta -= 6;
									iDelta /= 2;
									sButton.right += iDelta;
									sButton.left  += iDelta;
									break;

					case DT_RIGHT:	iDelta	= sArea  .right-sArea  .left;
									iDelta -= sButton.right-sButton.left;
									iDelta -= 6;
									sButton.right += iDelta;
									sButton.left  += iDelta;
									break;

						}
																// Ist das Feld ohne Focus ausgew?lt
					if((uBits&TVIS_SELECTED) && pData->cHasFocus && uItem==pData->uSelectedItem)
						{
						uTempColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
						SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
						Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
						}
					else{
						if(uBits&TVIS_DROPHILITED)
							{
							uTempColor =     GetSysColor     (COLOR_HIGHLIGHTTEXT);
							SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT   ));
							SelectObject(hDc,GetStockObject  (NULL_PEN          ));
							Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
							}
						else if(uBits&TVIS_SELECTED && uItem==pData->uSelectedItem)
							{
							if(uBits&TVIS_TEXTCOLOR)
									uTempColor = pEntry->uColorText;
							else	uTempColor = pData->aColors[TVC_TEXT];

							if(pData->uStyleEx&TVS_EX_FULLROWMARK)
								{
								SelectObject(hDc,GetStockObject(NULL_BRUSH));
								}
							else{
								if(pData->uStyleEx&TVS_EX_ALTERNATECOLOR)
									{
									SetDCBrushColor(hDc,pData->aColors[TVC_NOFOCUS]);
									SelectObject(hDc,GetStockObject(DC_BRUSH));
									}
								else{
									SelectObject(hDc,GetSysColorBrush(COLOR_3DFACE));
									}
								}

							SelectObject(hDc,GetStockObject(NULL_PEN));
							Rectangle(hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
							}
						else{
								 if(uBits&TVIS_TRACKED  )uTempColor = pData ->aColors[TVC_TRACK];
							else if(uBits&TVIS_TEXTCOLOR)uTempColor = pExtra->uColorText;
							else						 uTempColor = pData ->aColors[TVC_TEXT ];
							sButton.right--;
							sButton.left --;
							}

						SelectObject(hDc,hPatternPen);

						if(uBits&TVIS_FOCUSED)					// Punktierten Rahmen um Text zeichnen
							{
							SelectObject(hDc,GetStockObject(NULL_BRUSH));
							Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
							}
						}

					SetTextColor(hDc,uTempColor);
					sButton.left+=pData->iFontOff;

					DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);

					if(uBits&(TVIS_UNTERLINE|TVIS_TRACKED))		// Text unterstreichen
					if(pText && *pText)
						{
						sButton.left   -= pData->iFontOff;
						sButton.right  -= pData->iFontOff;
						sButton.top	   += pData->iFontLine;
						sButton.bottom  = sButton.top+1;
						uOldColor=SetBkColor(hDc,uTempColor);
						ExtTextOutW(hDc,0,0,uMode,&sButton,NULL,0,NULL);
						SetBkColor (hDc,uOldColor);
						}

					SetTextColor(hDc,pData->aColors[TVC_TEXT]);
					}
				else{											// Den Text ganz normal ausgeben
					uMode |= ETO_CLIPPED;

					if(!pExtra->iTextPixels)
						{
						sButton.top		= iYpos;
						sButton.left    = sArea.left+4;
						sButton.right	= sArea.right;
						sButton.bottom	= iYpos+pData->iFontHeight+2;

						if(uTextSize)							// Textbreite berechen
							{
							DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
							pExtra->iTextPixels = sButton.right-sButton.left;
							}
						else{
							pExtra->iTextPixels = 0;
							}
						}
																// Ist der Text größer als die Spalte
					if(sArea.left+pExtra->iTextPixels>=sArea.right)
						{
						LPINT	pPos;

						AllocTemp(pData,&pPos,NULL,uTextSize);

						   iSize  = sArea.right-sArea.left;
						   iSize -= (uBits&TVIS_BOLD)? pData->uTrippleB:pData->uTrippleN;
						if(iSize<3)
							{
							iCount = 0;
							}
						else{
							GetTextExtentExPoint(hDc,pText,uTextSize,iSize,&iCount,pPos,&sSize);
							}

						if(iCount>0)							// Passen noch Buchstaben vor "..."
							{
							sButton.right	= sArea.right;
							sArea.right		= sArea.left+2+pPos[iCount-1];

							ExtTextOut(hDc,sArea.left+2,iYpos,uMode,&sArea,pText,iCount,NULL);

							sArea.left		= sArea.right;
							sArea.right		= sButton.right;

							ExtTextOutW(hDc,sArea.left  ,iYpos,uMode,&sArea,L"...",3,NULL);
							}
						else{
							ExtTextOutW(hDc,sArea.left+2,iYpos,uMode,&sArea,L"...",3,NULL);
							}
						}
					else{
						uMode |= ETO_CLIPPED;

						switch(pData->aColumn[uExtra].bAlign)	// Textausrichtung
							{
						case DT_CENTER:	SetTextAlign(hDc,TA_CENTER|TA_TOP);
										ExtTextOut(hDc,(sArea.right+sArea.left)/2,iYpos,uMode,&sArea,pText,uTextSize,NULL);
										SetTextAlign(hDc,TA_LEFT|TA_TOP);
										break;

						case DT_RIGHT:	SetTextAlign(hDc,TA_RIGHT|TA_TOP);
										ExtTextOut(hDc,sArea.right-2,iYpos,uMode,&sArea,pText,uTextSize,NULL);
										SetTextAlign(hDc,TA_LEFT|TA_TOP);
										break;

						default:		ExtTextOut(hDc,sArea.left+2,iYpos,uMode,&sArea,pText,uTextSize,NULL);
										break;
							}
						}
					}
				}
																// Trennlinie bei fixen Breeich
			if(uLine+1==pData->uFixedLines && uPos!=uLine+pData->uFixedAlways)
			if(!(uStyleEx&TVS_EX_ITEMLINES))
				{
				sDummy.top    = sArea .top+iHeight-2;
				sDummy.bottom = sDummy.top+1;
				sDummy.right  = pData->aColumnXpos[uNext ];
				sDummy.left	  = pData->aColumnXpos[uExtra];

				SetBkColor (hDc,pData->aColors[TVC_FIXED]);
				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sDummy,NULL,0,NULL);
				}

			if(pData->uLineItem==uItem)							// Eine Markierungslinie zeichnen
				{
				if(uLine+1==pData->uFixedLines)SetBkColor(hDc,uFrColor);
				SelectObject(hDc,hPatternBrush);
				PatBlt      (hDc,pData->aColumnXpos[uExtra],sArea.top,pData->aColumn[uExtra].sReal+1,pData->uLineHeight,PATINVERT);
				}
			}

		sArea.top += pData->iRowHeight;

		if(sArea.top>=sRect.bottom)								// Unteres Ende des Fentser erreicht
			{
			break;
			}
		}


	if(sArea.top<sRect.bottom)									// Untere Fl?he ohne Eintr?e f?len
		{
		SelectObject(hDc,hRgnMain);

																// Gibt es markierte Spalten
		if(pData->uMarkedCols>0 && (pData->cIsEnabled || !(pData->uStyleEx&TVS_EX_GRAYEDDISABLE)))
			{
			sRect.right = 0-pData->uScrollX;

			for(uColumn=0;uColumn<pData->uColumnCount;uColumn++)
				{
				uExtra		 = pData->aColumnPos[uColumn  ];
				uNext		 = pData->aColumnPos[uColumn+1];
				uMark		 = pData->aColumn[uExtra].bMark;
				sRect.top    = sArea.top;
				sRect.left   = sRect.right;
				sRect.right  = pData->aColumnXpos[uNext]+uMark;
				sRect.right -= pData->uScrollX;
				if(sRect.right==sRect.left+1)uMark=0;
				SetBkColor (hDc,(uMark)? pData->aColors[TVC_COLBK]:uBkColor);
				ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sRect,NULL,0,NULL);
				}

			sRect.top   = sArea.top;
			sRect.left  = sRect.right;
			sRect.right = pData->uSizeX;
			SetBkColor (hDc,uBkColor);
			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sRect,NULL,0,NULL);
			}
		else{													// Keine markierten Spalten
			sRect.top	= sArea.top;
			SetBkColor (hDc,uBkColor);
			ExtTextOutW(hDc,0,0,ETO_OPAQUE,&sRect,NULL,0,NULL);
			}

		if(pData->uLineItem==U(TVI_LAST))						// Eine Markierungslinie zeichnen
			{
			SelectObject(hDc,hPatternBrush);
			PatBlt      (hDc,pData->aColumnXpos[0],sArea.top,pData->aColumnXpos[pData->uColumnCount],pData->uLineHeight,PATINVERT);
			}
		}


	if(pData->iMaxSizeX!=iMaxX)									// X-Scrollbar aktuallisieren
		{
		   pData->iMaxSizeX=iMaxX;
		if(pData->uColumnCount==0)
		if(pData->iMaxSizeX!=(int)pData->uOldCountX)
			{
			UpdateScrollX(pData);
			}
		}


	UNLOCK(pData);

 	DeleteObject(hRgnMain);

    for(uPos=0;uPos<uRgnCount;uPos++)
		{
        DeleteObject(aRgn[uPos]);
        }

}



