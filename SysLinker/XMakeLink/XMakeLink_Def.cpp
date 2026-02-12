#include "stdafx.h"
#include "../../include/Common_def.h"
#include "XMakeLink_Def.h"

GFX_SubSet_Fn g_subset = GFX_SubSetScalar;
CXLocStrMap	g_MapIdxBuild;
CXLocStrMap	g_MapIdxBtype;
CXLocStrMap	g_MapIdxFloor;
CXLocStrMap	g_MapIdxStair;
CXLocStrMap	g_MapIdxRoom;


BOOL Less_than(int a,int b) { return a < b; }
BOOL Greater_than(int a,int b) { return a > b; }
BOOL Less_equal(int a,int b) { return a <= b; }
BOOL Greater_equal(int a,int b) { return a >= b; }
BOOL Equal_equal(int a,int b) { return a == b; }
BOOL None_none(int a,int b) { return TRUE; }


UINT GF_SetAddr(int nFacpID,int nUnitID,int nChnID,int nDevID)
{
	SU_LOGICAL_ADDR suAddr;
	suAddr.dwAddr = 0;
	suAddr.stAddr.dwFacpID = nFacpID - 1;
	suAddr.stAddr.dwUnitID = nUnitID - 1;
	suAddr.stAddr.dwChnID = nChnID - 1;
	suAddr.stAddr.dwDevID = nDevID - 1;
	return suAddr.dwAddr;
}

UINT GF_GetFacpID(DWORD dwAddr)
{
	SU_LOGICAL_ADDR suAddr;
	suAddr.dwAddr = dwAddr;
	return suAddr.stAddr.dwFacpID + 1;
}

UINT GF_GetUnitID(DWORD dwAddr)
{
	SU_LOGICAL_ADDR suAddr;
	suAddr.dwAddr = dwAddr;
	return suAddr.stAddr.dwUnitID+1;
}

UINT GF_GetChnID(DWORD dwAddr)
{
	SU_LOGICAL_ADDR suAddr;
	suAddr.dwAddr = dwAddr;
	return suAddr.stAddr.dwChnID + 1;
}

UINT GF_GetDevID(DWORD dwAddr)
{
	SU_LOGICAL_ADDR suAddr;
	suAddr.dwAddr = dwAddr;
	return suAddr.stAddr.dwDevID + 1;
}

BOOL GF_SetBit(uint64_t * pBuf,int nIdx,BOOL bValue)
{
	size_t wordIndex = nIdx >> 6;       // /64
	size_t offset = nIdx & 63;       // %64
	uint64_t mask = (1ULL << offset);

	if(bValue)
		pBuf[wordIndex] |= mask;   // 비트 1로
	else
		pBuf[wordIndex] &= ~mask;  // 비트 0으로
	return TRUE;
}

BOOL GF_GetBit(uint64_t * pBuf,int nIdx)
{
	size_t wordIndex = nIdx >> 6;
	size_t offset = nIdx & 63;

	return (pBuf[wordIndex] >> offset) & 1ULL;
}


void WriteXMakeLinkLog(const WCHAR * szFmt,...)
{
	CString     strLog;
	va_list     argptr;
	int         cnt;
	SYSTEMTIME  tm;
	FILE        *fpLog = NULL;
	WCHAR        szLog[10240] = { 0 };

	GetLocalTime(&tm);
	strLog.Format(L"%sLog\\XMakeLinkLog%04d%02d%02d.log",g_strAppPath,tm.wYear,tm.wMonth,tm.wDay);

	va_start(argptr,szFmt);
	cnt = vswprintf_s(szLog,szFmt,argptr);
	va_end(argptr);

	int nError = _wfopen_s(&fpLog,strLog,L"a+, ccs=UTF-8");
	if(fpLog != NULL)
	{
		fwprintf(fpLog,L"[%02d:%02d:%02d.%03d] %s\n",
			tm.wHour,tm.wMinute,tm.wSecond,tm.wMilliseconds,
			szLog);

		fclose(fpLog);
	}
}
