// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif


#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS
//#include <vld.h>

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원
#include <afxdlgs.h>
#include <afx.h>

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

#include <memory>
#include <vector>
#include <map>

//20240129 GBM start - 추가 기능 헤더
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

//20250617 GBM start - 수동 연동데이터 일괄 편집 기능
#include "ManualLinkManager.h"
//20250617 GBM end

#define MAX_QUERY_STRING_SIZE	5000		

// [KHS 2020-1-16 17:28:02] 
// 윈도우 메세지
#define UWM_BASE							WM_USER + 12000

#define UWM_REG_OUTVIEW_DROPWND				UWM_BASE +1 //< 출력 VIEW TREE에 DROP 윈도우 등록 - 타입(트리,리스트,
#define UWM_REG_PTNVIEW_DROPWND				UWM_BASE +2 //< 패턴 VIEW TREE에 DROP 윈도우 등록
#define UWM_REG_EMVIEW_DROPWND				UWM_BASE +3 //< 비상방송 VIEW TREE에 DROP 윈도우 등록 - 타입(트리,리스트,
#define UWM_REG_PUMPVIEW_DROPWND			UWM_BASE +4 //< 펌프 VIEW TREE에 DROP 윈도우 등록
#define UWM_REG_PSWITCHVIEW_DROPWND			UWM_BASE +5 //< 압력스위치 VIEW TREE에 DROP 윈도우 등록 - 타입(트리,리스트,
#define UWM_REG_CONTACTVIEW_DROPWND			UWM_BASE +6 //< 펌프 VIEW TREE에 DROP 윈도우 등록

#define UWM_DKP_INPUTVIEW_ITEMCHANGE		UWM_BASE +11 //< 입력 VIEW TREE 선택 변경을 MainFrame으로 전송
#define UWM_DKP_OUTPUTVIEW_ITEMCHANGE		UWM_BASE +12 //< 출력 VIEW TREE 선택 변경을 MainFrame으로 전송
#define UWM_DKP_PTNPUTVIEW_ITEMCHANGE		UWM_BASE +13 //< 패턴 VIEW TREE 선택 변경을 MainFrame으로 전송

#define UWM_DKP_INPUTVIEW_REFRESH			UWM_BASE +21 //< 패턴 VIEW TREE 선택 변경을 MainFrame으로 전송
#define UWM_DKP_OUTPUTVIEW_REFRESH			UWM_BASE +22 //< 패턴 VIEW TREE 선택 변경을 MainFrame으로 전송
#define UWM_DKP_PATTERN_REFRESH				UWM_BASE +23 //< 패턴 VIEW TREE 선택 변경을 MainFrame으로 전송
#define UWM_DKP_EMERGENCY_REFRESH			UWM_BASE +24 //< 패턴 VIEW TREE 선택 변경을 MainFrame으로 전송
#define UWM_DKP_PUMP_REFRESH				UWM_BASE +25 //< 패턴 VIEW TREE 선택 변경을 MainFrame으로 전송
#define UWM_DKP_PSWITCH_REFRESH				UWM_BASE +26 //< 패턴 VIEW TREE 선택 변경을 MainFrame으로 전송
#define UWM_DKP_CONTACT_REFRESH				UWM_BASE +27 //< 패턴 VIEW TREE 선택 변경을 MainFrame으로 전송

#define UWM_CHILDFRAME_CLOSEENABLE			UWM_BASE +31 //< Child Frame 닫기 기능 Enable/Disable
#define UWM_CHILDPANE_TREESELCHANGE			UWM_BASE +32 //< PS,PUMP FORM에서 TREE CONTROL의 선택변경
#define UWM_CHILDPANE_SELDATACHANGE			UWM_BASE +43 //< PS , PUMP 정보 창에서 데이터 수정이 발생

#define UWM_RELAYEDITFORM_TREESELCHANGE		UWM_BASE +51 //< RELAY EDIT FORM에서 TREE 선택 변경
														//< WPARAM : 선택,CHECK 변경 여부 
														//< LPARAM : 선택 아이템 목록 VT_ITEM
#define UWM_RELAYEDITFORM_PREVIEWCHANGE		UWM_BASE +61 //< RELAY EDIT FORM에서 미리보기 눌렀을때
#define UWM_RELAYEDITFORM_DATACHANGE		UWM_BASE +62 //< RELAY EDIT FORM에서 저장/삭제 눌렀을때

#define UWM_VIEWFORM_RELAY					UWM_BASE + 71
// #define UWM_VIEWFORM_PATTERN				UWM_BASE + 72
// #define UWM_VIEWFORM_EM					UWM_BASE + 73
// #define UWM_VIEWFORM_PUMP				UWM_BASE + 74
// #define UWM_VIEWFORM_PSWITCH				UWM_BASE + 75
// #define UWM_VIEWFORM_EQUIP				UWM_BASE + 76


//#define UWM_AUTOMAKE_PROGRESS				UWM_BASE +101 //< RELAY EDIT FORM에서 저장/삭제 눌렀을때

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
// Broad Cast 메세지 
enum UBCM_DATAINT //< WPARAM에 INT TYPE
{
	FORM_PRJ_INIT = 1,
	FORM_PRJ_NEW,
	FORM_PRJ_OPEN,
	FORM_PRJ_CLOSE,
	FORM_PRJ_REFESH, //< 데이터 변경 됐을 때 새로 고침

};
static const UINT UDBC_ALLDATA_INIT = RegisterWindowMessage(L"F923ECC4-6E7C-4448-8245-BFE9E078AE2B"); //< 프로젝트 열기/닫기/생성 일때

#ifndef ENGLISH_MODE
#define CHANGETREE_INPUT_TEXT			_T("입력")
#define CHANGETREE_OUTPUT_TEXT			_T("출력")
#define CHANGETREE_PATTERN_TEXT			_T("사용하는패턴")
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
1. 변경정보를 보여주는 부분은 
   현상태 유지 또는 삭제,
   유지 할때는 다른 작업을 할 필요가 없다 (ID 기반으로 움직이니깐 , 유닛 Num ,계통 넘버가 바껴도 ID는 그대로 
   데이터만 변경시킨다.(number , map Key)
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
	L"전체",
	L"패턴내 출력개수 초과",
	L"글자 수 초과",
	L"층번호 중복",
	L"입력타입 없음",
	L"연동 출력개수 초과",
	L"사용되지 않는 출력",
	L"출력없는 입력",
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
	int nErrorData; // 오류 종류에 따라 - 오류 패턴개수,출력개수,글자수,층번호,사용되지않는 출력(0),출력없는입력(0)
	CString strDesc;
};


// [KHS 2020-7-1 09:38:09] 
// Drag Tree의 사용자 보기 설정 정보
typedef std::vector<int> TVORDER;



class CDataDevice;
/*!
 * @class      ST_CHANGE_LINKDATA
 * @author     KHS
 * @version    1.0
 * @date       2020-1-31 10:22:47
 * @brief      변경되는 데이터를 저장하는 구조체
			리스트로 저장한다.
 * @note 
 
 * Contact  hskim0602@gmail.com
 */
struct ST_CHANGE_LINKDATA
{
	BYTE			btInOutType; //< In: 1 , Out : 2 , Pattern : 3
	// 출력일때 
	// 1. Source에서 삭제
	// 2. Source에 추가
	// 3. 변경 
	// 4. 삭제
	BYTE			btChangeType;//< 1:Add , 2: 유지 ,3:변경 4: remove
	
	LPVOID			pRefInput;	///< 기존에 저장되어 있는(Map 또는 Pattern) 데이터의 reference
								///< btInOutType이 CHANGE_LK_INPUT 일때 NULL
								///< btInOutType이 CHANGE_LK_OUTPUT 일때  New Source Device(새로할당한다) - 변경/삭제만 저장된 Pattern
								///< btInOutType일때는 Pattern일때 New Pattern(새로할당한다) - 변경/삭제만 저장된 Pattern
	LPVOID			pRefOld;	///< Old Data : 변경전
								///< CHANGE_LK_INPUT,CHANGE_LK_PATTERN는 CDataDevice
								///< CHANGE_LK_OUTPUT일때는 CDataLinked
	LPVOID			pChange;	//< 변경 내용 , 
								///< CHANGE_LK_INPUT,CHANGE_LK_PATTERN는 CDataDevice
								///< CHANGE_LK_OUTPUT일때는 CDataLinked
};

/*
압력스위치 , 펌프 연동데이터 트리에 저장하는 데이터
*/
struct ST_TREEPANE_DATA
{
	int nType;		// TTYPE_DEV_FACP , ....
	LPVOID pData;
};

/*
rom file 버전

*/
struct ST_ROMVERSION
{
	BYTE btVer[3];
};

struct ST_ROMITEM
{
	BYTE btType; // Rom or reverseInfo 파일
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
	// TREE VIEW CUSTOM 보기
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

// Key : Old CDataDevice의 Key를 사용한다.
//		기존에 변경된 deivce에서 Key를 가져와 Map에서 찾는다
//		Map 데이터는 수정된 데이터를 저장한다.
typedef std::map<CString, CDataDevice*> CMapChangeDevice;//< 변경되는 디바이스 전체 목록 - in/out 모두 저장 , Tree에서 reference로 사용한다.


// [KHS 2020-8-14 10:20:04] 
// 중계기 일람표 업데이트 시 사용하는 container
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

extern TVORDER g_tvInput; //< Treeview의 입력 트리뷰의 사용자 보기 설정
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


