// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif


#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS
//#include <vld.h>

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.


#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����
#include <afxdlgs.h>
#include <afx.h>

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

#include <memory>
#include <vector>
#include <map>

//20240129 GBM start - �߰� ��� ���
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Defines.h"	
#include "TSingleton.h"
#include "CommonFunc.h"
#include "Log.h"
#include "NewInfo.h"
#include "NewDBManager.h"
#include "NewExcelManager.h"
//20240129 GBM end

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif
#include "../include/Common_def.h"
#include "../Common/Utils/clasimpl.h"

//20250617 GBM start - ���� ���������� �ϰ� ���� ���
#include "ManualLinkManager.h"
//20250617 GBM end

#define MAX_QUERY_STRING_SIZE	5000		

// [KHS 2020-1-16 17:28:02] 
// ������ �޼���
#define UWM_BASE							WM_USER + 12000

#define UWM_REG_OUTVIEW_DROPWND				UWM_BASE +1 //< ��� VIEW TREE�� DROP ������ ��� - Ÿ��(Ʈ��,����Ʈ,
#define UWM_REG_PTNVIEW_DROPWND				UWM_BASE +2 //< ���� VIEW TREE�� DROP ������ ���
#define UWM_REG_EMVIEW_DROPWND				UWM_BASE +3 //< ����� VIEW TREE�� DROP ������ ��� - Ÿ��(Ʈ��,����Ʈ,
#define UWM_REG_PUMPVIEW_DROPWND			UWM_BASE +4 //< ���� VIEW TREE�� DROP ������ ���
#define UWM_REG_PSWITCHVIEW_DROPWND			UWM_BASE +5 //< �з½���ġ VIEW TREE�� DROP ������ ��� - Ÿ��(Ʈ��,����Ʈ,
#define UWM_REG_CONTACTVIEW_DROPWND			UWM_BASE +6 //< ���� VIEW TREE�� DROP ������ ���

#define UWM_DKP_INPUTVIEW_ITEMCHANGE		UWM_BASE +11 //< �Է� VIEW TREE ���� ������ MainFrame���� ����
#define UWM_DKP_OUTPUTVIEW_ITEMCHANGE		UWM_BASE +12 //< ��� VIEW TREE ���� ������ MainFrame���� ����
#define UWM_DKP_PTNPUTVIEW_ITEMCHANGE		UWM_BASE +13 //< ���� VIEW TREE ���� ������ MainFrame���� ����

#define UWM_DKP_INPUTVIEW_REFRESH			UWM_BASE +21 //< ���� VIEW TREE ���� ������ MainFrame���� ����
#define UWM_DKP_OUTPUTVIEW_REFRESH			UWM_BASE +22 //< ���� VIEW TREE ���� ������ MainFrame���� ����
#define UWM_DKP_PATTERN_REFRESH				UWM_BASE +23 //< ���� VIEW TREE ���� ������ MainFrame���� ����
#define UWM_DKP_EMERGENCY_REFRESH			UWM_BASE +24 //< ���� VIEW TREE ���� ������ MainFrame���� ����
#define UWM_DKP_PUMP_REFRESH				UWM_BASE +25 //< ���� VIEW TREE ���� ������ MainFrame���� ����
#define UWM_DKP_PSWITCH_REFRESH				UWM_BASE +26 //< ���� VIEW TREE ���� ������ MainFrame���� ����
#define UWM_DKP_CONTACT_REFRESH				UWM_BASE +27 //< ���� VIEW TREE ���� ������ MainFrame���� ����

#define UWM_CHILDFRAME_CLOSEENABLE			UWM_BASE +31 //< Child Frame �ݱ� ��� Enable/Disable
#define UWM_CHILDPANE_TREESELCHANGE			UWM_BASE +32 //< PS,PUMP FORM���� TREE CONTROL�� ���ú���
#define UWM_CHILDPANE_SELDATACHANGE			UWM_BASE +43 //< PS , PUMP ���� â���� ������ ������ �߻�

#define UWM_RELAYEDITFORM_TREESELCHANGE		UWM_BASE +51 //< RELAY EDIT FORM���� TREE ���� ����
														//< WPARAM : ����,CHECK ���� ���� 
														//< LPARAM : ���� ������ ��� VT_ITEM
#define UWM_RELAYEDITFORM_PREVIEWCHANGE		UWM_BASE +61 //< RELAY EDIT FORM���� �̸����� ��������
#define UWM_RELAYEDITFORM_DATACHANGE		UWM_BASE +62 //< RELAY EDIT FORM���� ����/���� ��������

#define UWM_VIEWFORM_RELAY					UWM_BASE + 71
// #define UWM_VIEWFORM_PATTERN				UWM_BASE + 72
// #define UWM_VIEWFORM_EM					UWM_BASE + 73
// #define UWM_VIEWFORM_PUMP				UWM_BASE + 74
// #define UWM_VIEWFORM_PSWITCH				UWM_BASE + 75
// #define UWM_VIEWFORM_EQUIP				UWM_BASE + 76


//#define UWM_AUTOMAKE_PROGRESS				UWM_BASE +101 //< RELAY EDIT FORM���� ����/���� ��������

#define UWM_VIEWFORM_ADDITEM				UWM_BASE + 81
#define UWM_VIEWFORM_CHANGEITEM				UWM_BASE + 82
#define UWM_VIEWFORM_DELITEM				UWM_BASE + 83

#define UWM_ERRORCHECK_NOTIFY				UWM_BASE + 91
#define UWM_ERRORCHECK_CLOSE				UWM_BASE + 92
#define UWM_ERRORCHECK_CREATELINK			UWM_BASE + 93

#define _DBG_MAKE_TIME_ 1


#define _USE_PSWITCH_	1
#define _USE_STAIR_NUM_ 0
#define _USE_OUTPUTWND_	1


#define TM_PROG_TIMER		10101
// Broad Cast �޼��� 
enum UBCM_DATAINT //< WPARAM�� INT TYPE
{
	FORM_PRJ_INIT = 1,
	FORM_PRJ_NEW,
	FORM_PRJ_OPEN,
	FORM_PRJ_CLOSE,
	FORM_PRJ_REFESH, //< ������ ���� ���� �� ���� ��ħ

};
static const UINT UDBC_ALLDATA_INIT = RegisterWindowMessage(L"F923ECC4-6E7C-4448-8245-BFE9E078AE2B"); //< ������Ʈ ����/�ݱ�/���� �϶�

#ifndef ENGLISH_MODE
#define CHANGETREE_INPUT_TEXT			_T("�Է�")
#define CHANGETREE_OUTPUT_TEXT			_T("���")
#define CHANGETREE_PATTERN_TEXT			_T("����ϴ�����")
#else
#define CHANGETREE_INPUT_TEXT			_T("INPUT")
#define CHANGETREE_OUTPUT_TEXT			_T("OUTPUT")
#define CHANGETREE_PATTERN_TEXT			_T("PATTERN IN USE")
#endif

enum 
{
	DATA_ADD = 1, 
	DATA_SAVE = 2,
	DATA_DEL = 3,
	DATA_ALL = 4,
};

enum FormViewStyle
{
	FV_MAKELINKER,
	FV_MAKEPATTERN,
	FV_PSWITCH,
	FV_SWITCH,
	FV_LINKVIEW,
	FV_PUMP,
	FV_EQUIP,
	FV_USER,
	FV_ACCESS,
	FV_EMERGENCY,
	FV_LOCATION,
	FV_FACP,
	FV_UNIT,
	FV_RELAYEDIT,
	FV_AUTOMAKE,
	FV_LOGICEDIT,
	FV_LOADRELAYTABLE,
	FV_ERRORCHECK,
	FV_COUNT,
};

/*
1. ���������� �����ִ� �κ��� 
   ������ ���� �Ǵ� ����,
   ���� �Ҷ��� �ٸ� �۾��� �� �ʿ䰡 ���� (ID ������� �����̴ϱ� , ���� Num ,���� �ѹ��� �ٲ��� ID�� �״�� 
   �����͸� �����Ų��.(number , map Key)
*/
enum 
{
	CHANGE_LK_INPUT = 1,
	CHANGE_LK_OUTPUT=2,
	CHANGE_LK_PATTERN=3,
	CHANGE_LK_FACP = 4,
	CHANGE_LK_UNIT = 5,
	CHANGE_LK_CHN = 6,
	CHANGE_LK_DEV = 7,
};

enum
{
	CHANGE_ACTION_ADD		= 1,
	CHANGE_ACTION_KEEP		= 2,
	CHANGE_ACTION_UPDATE	= 3,
	CHANGE_ACTION_DEL = 4,
	CHANGE_ACTION_FAILED = 5,
};

enum 
{
	ERR_CHECK_SIMPLE,
	ERR_CHECK_CREATELINK,
	ERR_CHECK_MAKEAUTOLINK,
};
enum
{
	CHK_ALL_DATA = 0,
	CHK_PATTERN_CNT,
	CHK_TEXT_CNT,
	CHK_LEVEL_DUP,
	CHK_NOINPUT,
	CHK_OUTPUT_CNT,
	CHK_NOUSE_OUTPUT,
	CHK_INPUT_WITHOUT_OUTPUT,
	CHK_CNT,
};

#ifndef ENGLISH_MODE
static CString g_strErrChkText[CHK_CNT] = {
	L"��ü",
	L"���ϳ� ��°��� �ʰ�",
	L"���� �� �ʰ�",
	L"����ȣ �ߺ�",
	L"�Է�Ÿ�� ����",
	L"���� ��°��� �ʰ�",
	L"������ �ʴ� ���",
	L"��¾��� �Է�",
};
#else
static CString g_strErrChkText[CHK_CNT] = {
	L"ALL",
	L"EXCEEDED THE OUTPUT COUNT FOR THE PATTERN",
	L"CHARACTER LIMIT EXCEEDED",
	L"DUPLICATE FLOOR NUMBER",
	L"NO INPUT TYPE",
	L"EXCEEDED THE INTERLOCK OUTPUT COUNT",
	L"UNUSED OUTPUT",
	L"INPUT WITHOUT OUTPUT",
};
#endif

struct ST_ERRCHECK
{
	BYTE btType;
	BYTE btError;
	LPVOID lpData;
	int nErrorData; // ���� ������ ���� - ���� ���ϰ���,��°���,���ڼ�,����ȣ,�������ʴ� ���(0),��¾����Է�(0)
	CString strDesc;
};


// [KHS 2020-7-1 09:38:09] 
// Drag Tree�� ����� ���� ���� ����
typedef std::vector<int> TVORDER;



class CDataDevice;
/*!
 * @class      ST_CHANGE_LINKDATA
 * @author     KHS
 * @version    1.0
 * @date       2020-1-31 10:22:47
 * @brief      ����Ǵ� �����͸� �����ϴ� ����ü
			����Ʈ�� �����Ѵ�.
 * @note 
 
 * Contact  hskim0602@gmail.com
 */
struct ST_CHANGE_LINKDATA
{
	BYTE			btInOutType; //< In: 1 , Out : 2 , Pattern : 3
	// ����϶� 
	// 1. Source���� ����
	// 2. Source�� �߰�
	// 3. ���� 
	// 4. ����
	BYTE			btChangeType;//< 1:Add , 2: ���� ,3:���� 4: remove
	
	LPVOID			pRefInput;	///< ������ ����Ǿ� �ִ�(Map �Ǵ� Pattern) �������� reference
								///< btInOutType�� CHANGE_LK_INPUT �϶� NULL
								///< btInOutType�� CHANGE_LK_OUTPUT �϶�  New Source Device(�����Ҵ��Ѵ�) - ����/������ ����� Pattern
								///< btInOutType�϶��� Pattern�϶� New Pattern(�����Ҵ��Ѵ�) - ����/������ ����� Pattern
	LPVOID			pRefOld;	///< Old Data : ������
								///< CHANGE_LK_INPUT,CHANGE_LK_PATTERN�� CDataDevice
								///< CHANGE_LK_OUTPUT�϶��� CDataLinked
	LPVOID			pChange;	//< ���� ���� , 
								///< CHANGE_LK_INPUT,CHANGE_LK_PATTERN�� CDataDevice
								///< CHANGE_LK_OUTPUT�϶��� CDataLinked
};

/*
�з½���ġ , ���� ���������� Ʈ���� �����ϴ� ������
*/
struct ST_TREEPANE_DATA
{
	int nType;		// TTYPE_DEV_FACP , ....
	LPVOID pData;
};

/*
rom file ����

*/
struct ST_ROMVERSION
{
	BYTE btVer[3];
};

struct ST_ROMITEM
{
	BYTE btType; // Rom or reverseInfo ����
	ST_ROMVERSION	stVer;
	TCHAR			szFile[MAX_PATH + 1];
};

typedef struct
{
	//////////////////////////////////////////////////////////////////////////
	// Project [PROJECT]
	TCHAR	szPrjPath[MAX_PATH + 1]; ///< PATH=

									 //////////////////////////////////////////////////////////////////////////
									 // Database [DATABASE]
	int		nDBPort;
	TCHAR	szSource[MAX_PATH + 1];
	TCHAR	szDBName[D_MAX_NAME + 1];
	TCHAR	szDBUser[D_MAX_NAME + 1];
	TCHAR	szDBPass[D_MAX_NAME + 1];
	DWORD	dwTimeOut; 
	//////////////////////////////////////////////////////////////////////////
	// Serial Port [SERIAL]
	int		nSerialPort;
	int		nBaudrate;
	int		nDataBit;
	int		nStopbit;
	int		nParitybit;

	//////////////////////////////////////////////////////////////////////////
	// TREE VIEW CUSTOM ����
	TCHAR	szInput[MAX_PATH + 1];
	TCHAR	szOutput[MAX_PATH + 1];
}ST_CONFIG;

struct ST_REMOVE_LINKED
{
	int nSrcFacp;
	int nSrcUnit;
	int nSrcChn;
	int nSrcDev;
	byte btLinkType;
	int nTgtFacp;
	int nTgtUnit;
	int nTgtChn;
	int nTgtDev;
};

// Key : Old CDataDevice�� Key�� ����Ѵ�.
//		������ ����� deivce���� Key�� ������ Map���� ã�´�
//		Map �����ʹ� ������ �����͸� �����Ѵ�.
typedef std::map<CString, CDataDevice*> CMapChangeDevice;//< ����Ǵ� ����̽� ��ü ��� - in/out ��� ���� , Tree���� reference�� ����Ѵ�.


// [KHS 2020-8-14 10:20:04] 
// �߰�� �϶�ǥ ������Ʈ �� ����ϴ� container
class CRUpdateItem;
typedef std::vector<CRUpdateItem *> VT_RUPDATEITEM;

CString		GF_GetLocationKey(int nType , CString strBuild , CString strBType = L"" 
	, CString strStair = L"", CString strFloor = L"", CString strRoom = L"");
CString		GF_GetSysDataKey(int nType , int nFacpNum , int nUnitNum = -1
	, int nChannel = -1, int nRelayNum = -1);

CString		GF_GetIDSysDataKey(int nType, int nFID, int nUID = -1
	, int nCID = -1, int nRID = -1);

CString		GF_GetVersionFolderName(WORD wMajor, WORD wMinor);
CString		GF_GetDatabaseName(CString strPrjName , BOOL bData);

std::vector<CString> GF_SplitString(CString strString, CString strToken);
BOOL GF_IsExistFile(CString strFullPath);
CString		GF_CopyDir(CString strTo, CString strFrom);
CString		GF_DeleteDir(CString strPath);
CString		GF_GetSHErrorString(int nError);
BOOL		GF_FindFile(CString strPath, CString strFile);
std::wstring LoadUtf8FileToString(const std::wstring& filename);
BYTE		GF_FacpType(CString strFType);
BYTE		GF_UnitType(CString strUType);
ST_TREEITEM * GF_FindTreeItem(std::vector<ST_TREEITEM*> vtName, CString strName);

HTREEITEM	GF_FindTreeByText(CTreeCtrl* pTree, HTREEITEM hItem, CString strFind , BOOL bFindChild = TRUE);
HTREEITEM	GF_FindTreeByPath(CTreeCtrl* pTree,HTREEITEM hParent,int nLevel,std::vector<CString> vtArray);

HTREEITEM	GF_FindTreeByData(CTreeCtrl* pTree, HTREEITEM hItem, DWORD_PTR dwData);
void GF_CreateFolder(CString csPath);

int GF_Unicode2ASCII(WCHAR *uni, char *ascii , int nBuffSize);
int GF_ASCII2Unicode(char *ascii, WCHAR *uni, int nBuffSize);
int GF_ASCII2Unicode(char *ascii, CString &str, int nBuffSize);

WORD GF_CRC16(WORD wCrc, BYTE bData);

BOOL GF_Text2Number(CString strNumber, int &nRet);

#define _USE_UNICODE_REVS_INFO_ 0

#if _USE_OUTPUTWND_
class COutputWnd;
extern COutputWnd *	g_pLogWnd;
#endif

extern BOOL			g_bLogWrite;
void GF_AddLog(CString str);
void GF_AddLog(const WCHAR * szFmt, ...);
void GF_AddDebug(CString str);
void GF_AddDebug(const WCHAR * szFmt, ...);
void WriteMainLog(CString str);
void WriteMainLog(const WCHAR * szFmt, ...);

CString GF_GetSafeArrayValue(COleSafeArray * pSa,int nRow,int nCol);

extern CString g_strAppPath;
extern ST_CONFIG g_stConfig;

extern TVORDER g_tvInput; //< Treeview�� �Է� Ʈ������ ����� ���� ����
extern TVORDER g_tvOutput;

extern BOOL g_bRequirePtnManualCheck;

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


