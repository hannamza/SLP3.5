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

// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// SysLinker.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include "SysLinker.h"
#include "../Common/Control/DTreeCtrl.h"

#include <vector>
using namespace std;

class COutputWnd;

CString g_strAppPath = L"";
ST_CONFIG g_stConfig;

TVORDER g_tvInput; //< Treeview의 입력 트리뷰의 사용자 보기 설정
TVORDER g_tvOutput;
COutputWnd *	g_pLogWnd = nullptr;
BOOL			g_bLogWrite  = FALSE;

BOOL g_bRequirePtnManualCheck = FALSE;
CString		GF_GetLocationKey(int nType, CString strBuild, CString strBType 
	, CString strStair , CString strFloor , CString strRoom )
{
	CString strKey = L"";
	CString strtemp = L"";

	strtemp = strBuild;
	strtemp.Remove(' '); //공백제거
	if (strtemp.GetLength() <= 0)
		strBuild = L"-";
	
	strtemp = strBType;
	strtemp.Remove(' '); //공백제거
	if (strtemp.GetLength() <= 0)
		strBType = L"-";
	strtemp = strStair;
	strtemp.Remove(' '); //공백제거
	if (strtemp.GetLength() <= 0)
		strStair = L"-";
	strtemp = strFloor;
	strtemp.Remove(' '); //공백제거
	if (strtemp.GetLength() <= 0)
		strFloor = L"-";

	strtemp = strRoom;
	strtemp.Remove(' '); //공백제거
	if (strtemp.GetLength() <= 0)
		strRoom = L"-";

	switch (nType)
	{
	case LT_BUILD:
		strKey.Format(L"%s.-.-.-.-.%d"
			, strBuild, LT_BUILD);
		break;
	case LT_BTYPE:
		strKey.Format(L"%s.%s.-.-.-.%d"
			, strBuild, strBType, LT_BTYPE
		);
		break;
	case LT_STAIR:
		strKey.Format(L"%s.%s.%s.-.-.%d"
			, strBuild, strBType, strStair, LT_STAIR
		);
		break;
	case LT_FLOOR:
		strKey.Format(L"%s.%s.%s.%s.-.%d"
			, strBuild, strBType, strStair, strFloor, LT_FLOOR
		);
		break;
	case LT_ROOM:
		strKey.Format(L"%s.%s.%s.%s.%s.%d"
			, strBuild, strBType, strStair, strFloor, strRoom, LT_ROOM
		);
		break;
	}
	return strKey;
}


CString		GF_GetSysDataKey(int nType, int nFacpNum, int nUnitNum 
	, int nChannel , int nRelayNum )
{
	CString strKey = L"";
	if (nFacpNum < 0)
		return strKey;

	strKey.Format(L"%02d.%02d.%d.%03d.%d" , nFacpNum , nUnitNum, nChannel , nRelayNum , nType);
	return strKey;
}

CString		GF_GetIDSysDataKey(int nType, int nFID, int nUID
	, int nCID, int nRID)
{
	CString strKey = L"";
	if (nFID < 1)
		return strKey;
	strKey.Format(L"%02d.%02d.%d.%03d.%d", nFID, nUID, nCID, nRID, nType);
	return strKey;
}

CString		GF_GetVersionFolderName(WORD wMajor, WORD wMinor)
{
	CString strFolder;
	strFolder.Format(L"%s %d.%d", F3_VERSIONFOLDER_NAME, wMajor, wMinor);
	return strFolder;
}

CString		GF_GetDatabaseName(CString strPrjName, BOOL bData)
{
	CString strName;
	if(bData)
		strName.Format(L"%s_DATA.MDF", strPrjName);
	else
		strName.Format(L"%s_LOG.LDF", strPrjName);

	return strName;
}


vector<CString>  GF_SplitString(CString strString, CString strToken)
{
	int pos = 0;
	CString str;
	vector<CString> vtRet;

// 	while ((str = strString.Tokenize(strToken, pos)) != L"")
// 	{
// 		vtRet.push_back(str);
// 	}


 	while (AfxExtractSubString(str, strString, pos, strToken.GetAt(0)))
 	{
 		// Prepare to move to the next substring
 		pos++;
 		vtRet.push_back(str);
 	}
	return vtRet;
}
size_t GetSizeOfFile(const std::wstring& path)
{
	struct _stat fileinfo;
	_wstat(path.c_str(), &fileinfo);
	return fileinfo.st_size;
}

HTREEITEM	GF_FindTreeByText(CTreeCtrl* pTree, HTREEITEM hItem, CString strFind,BOOL bFindChild)
{
	HTREEITEM hitemFind, hItemChile, hItemSibling;
	CString strText = L"";
	hitemFind = hItemChile = hItemSibling = NULL;

	strText = pTree->GetItemText(hItem);
	if (strText.CompareNoCase(strFind) == 0)
		hitemFind = hItem;
	else
	{
		hItemChile = pTree->GetChildItem(hItem);
		if (bFindChild && hItemChile)
		{
			hitemFind = GF_FindTreeByText(pTree, hItemChile, strFind,bFindChild);
			if (hitemFind != nullptr)
				return hitemFind;
		}
		// 형제노드를 찾는다.
		hItemSibling = pTree->GetNextSiblingItem(hItem);
		if (hitemFind == NULL && hItemSibling)
		{
			hitemFind = GF_FindTreeByText(pTree, hItemSibling, strFind,bFindChild);
		}
	}
	return hitemFind;
}


/*
	GF_FindTreeByPath
	CString strPath : 찾을 위치 (AA\\BB\\CC)

*/
HTREEITEM	GF_FindTreeByPath(CTreeCtrl* pTree,HTREEITEM hParent,int nLevel,std::vector<CString> vtArray)
{
	HTREEITEM hItemChile;
	CString strText = L"",strFind;

	if(nLevel >= vtArray.size())
		return hParent;
	strFind = vtArray[nLevel];
	if(strFind.GetLength() <= 0)
		return hParent;

	hItemChile = pTree->GetChildItem(hParent);
	if(hItemChile == nullptr)
		return nullptr;

	do 
	{
		strText = pTree->GetItemText(hItemChile);
		if(strText.CompareNoCase(strFind) == 0)
		{
			nLevel ++;
			return GF_FindTreeByPath(pTree,hItemChile,nLevel,vtArray);
		}
		hItemChile = pTree->GetNextSiblingItem(hItemChile);
	} while(hItemChile != nullptr);
	return nullptr;
}

HTREEITEM GF_FindTreeByData(CTreeCtrl* pTree, HTREEITEM hItem,DWORD_PTR dwData)
{
	HTREEITEM hitemFind, hItemChile, hItemSibling;
	DWORD_PTR dwItem = 0;
	hitemFind = hItemChile = hItemSibling = NULL;

	if (hItem != TVI_ROOT)
		dwItem = pTree->GetItemData(hItem);
	else
		dwItem = 0; 
	if (dwItem == dwData)
	{
		hitemFind = hItem;
	}
	else
	{
		// 자식 노드를 찾는다.
		hItemChile = pTree->GetChildItem(hItem);
		if (hItemChile)
		{
			hitemFind = GF_FindTreeByData(pTree, hItemChile, dwData);

		}

		// 형제노드를 찾는다.
		hItemSibling = pTree->GetNextSiblingItem(hItem);
		if (hitemFind == NULL && hItemSibling)
		{
			hitemFind = GF_FindTreeByData(pTree, hItemSibling, dwData);
		}
	}

	return hitemFind;
}

ST_TREEITEM * GF_FindTreeItem(std::vector<ST_TREEITEM*> vtName, CString strName)
{
	int i; 
	size_t tSize;
	ST_TREEITEM * pData;
	tSize = vtName.size();
	for (i = 0; i < tSize; i++)
	{
		pData = vtName[i];
		if (pData == nullptr)
			continue;
		if (pData->pData == nullptr)
			continue;
		if (strName.CompareNoCase((LPCTSTR)pData->pData) == 0)
			return pData;
	}
	return nullptr;
}


std::wstring LoadUtf8FileToString(const std::wstring& filename)
{
	std::wstring buffer;            // stores file contents
	FILE* f;
	errno_t err = _wfopen_s(&f , filename.c_str(), L"rtS, ccs=UTF-8");

	// Failed to open file
	if (f == NULL)
	{
		// ...handle some error...
		return buffer;
	}

	size_t filesize = GetSizeOfFile(filename);

	// Read entire file contents in to memory
	if (filesize > 0)
	{
		buffer.resize(filesize);
		size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, f);
		buffer.resize(wchars_read);
		buffer.shrink_to_fit();
	}

	fclose(f);

	return buffer;
}

BOOL GF_IsExistFile(CString strFullPath)
{
#ifdef _UNICODE 
	return(_waccess_s(strFullPath, 0) != -1);
#else 
	return (_access_s(strFullPath, 0) != -1);
#endif
}

CString  GF_CopyDir(CString strTo, CString strFrom)
{
	CString strError = L"";
	SHFILEOPSTRUCT shStruct;
	TCHAR pszFrom[4096] = { 0 }, pszTo[4096] = { 0 };
	ZeroMemory(&shStruct, sizeof(SHFILEOPSTRUCT));

	_tcscpy_s(pszTo, 4096, strTo);
	_tcscpy_s(pszFrom, 4096, strFrom);
	pszTo[strTo.GetLength()] = '\0';
	pszTo[strTo.GetLength() + 1] = '\0';
	pszFrom[strFrom.GetLength()] = '\0';
	pszFrom[strFrom.GetLength() + 1] = '\0';

	// [KHS 2020-1-2 14:24:35] 
	// FOF_MULTIDESTFILES 옵션을 주어야 strTo폴더가 있을때 strTo 디렉토리 밑으로 안들어간다.
	// strFrom폴더를 strTo폴더명으로 복사해서 넣는다.
	// 
	shStruct.fFlags = FOF_NOCONFIRMATION | FOF_MULTIDESTFILES | FOF_SILENT;
	//shStruct.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	shStruct.hwnd = NULL;
	shStruct.pTo = pszTo;
	shStruct.pFrom = pszFrom;
	shStruct.wFunc = FO_COPY;
	shStruct.fAnyOperationsAborted = NULL;
	shStruct.hNameMappings = NULL;
	shStruct.lpszProgressTitle = NULL;
	int nRet = SHFileOperation(&shStruct);
	if (nRet != 0)
	{
		strError = GF_GetSHErrorString(nRet);
	}
	return strError;
}


CString  GF_DeleteDir(CString strPath)
{
	CString strError = L"";
	SHFILEOPSTRUCT shStruct;
	TCHAR pszFrom[4096] = { 0 };
	ZeroMemory(&shStruct, sizeof(SHFILEOPSTRUCT));

	_tcscpy_s(pszFrom, 4096, strPath);
	pszFrom[strPath.GetLength()] = '\0';
	pszFrom[strPath.GetLength() + 1] = '\0';

	// [KHS 2020-1-2 14:24:35] 
	// FOF_MULTIDESTFILES 옵션을 주어야 strTo폴더가 있을때 strTo 디렉토리 밑으로 안들어간다.
	// strFrom폴더를 strTo폴더명으로 복사해서 넣는다.
	// 
	//shStruct.fFlags = FOF_MULTIDESTFILES ;
	shStruct.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	shStruct.hwnd = NULL;
	shStruct.pTo = NULL;
	shStruct.pFrom = pszFrom;
	shStruct.wFunc = FO_DELETE;
	shStruct.fAnyOperationsAborted = NULL;
	shStruct.hNameMappings = NULL;
	shStruct.lpszProgressTitle = NULL;
	int nRet = SHFileOperation(&shStruct);
	if (nRet != 0)
	{
		strError = GF_GetSHErrorString(nRet);
	}
	return strError;
}

CString GF_GetSHErrorString(int nError)
{
	CString strError = L"";
#ifndef ENGLISH_MODE
	switch (nError)
	{
	case 0x71:
		strError = L"소스 파일과 대상 파일은 같은 파일입니다.";
		break;
	case 0x72:
		strError = L"소스 버퍼에 여러 파일 경로가 지정되었지만 대상 파일 경로는 하나만 지정되었습니다.";
		break;
	case 0x73:
		strError = L"이름 바꾸기 조작이 지정되었지만 대상 경로가 다른 디렉토리입니다. 대신 이동 조작을 사용하십시오.";
		break;
	case 0x74:
		strError = L"소스는 루트 디렉토리이며 이동하거나 이름을 바꿀 수 없습니다.";
		break;
	case 0x75:
		strError = L"사용자가 작업을 취소했거나 적절한 플래그가 SHFileOperation 에 제공된 경우 자동으로 취소되었습니다.";
		break;
	case 0x76:
		strError = L"대상은 소스의 하위 트리입니다.";
		break;
	case 0x78:
		strError = L"보안 설정이 소스에 대한 액세스를 거부했습니다.";
		break;
	case 0x79:
		strError = L"소스 또는 대상 경로가 MAX_PATH를 초과했거나 초과했습니다.";
		break;
	case 0x7A:
		strError = L"작업에 여러 대상 경로가 포함되어 이동 작업의 경우 실패할 수 있습니다.";
		break;
	case 0x7C:
		strError = L"소스 또는 대상 또는 둘 다의 경로가 유효하지 않습니다.";
		break;
	case 0x7D:
		strError = L"원본과 대상은 동일한 상위 폴더를 갖습니다.";
		break;
	case 0x7E:
		strError = L"대상 경로는 기존 파일입니다.";
		break;
	case 0x80:
		strError = L"대상 경로는 기존 폴더입니다.";
		break;
	case 0x81:
		strError = L"파일 이름이 MAX_PATH를 초과했습니다.";
		break;
	case 0x82:
		strError = L"대상은 읽기 전용 CD-ROM이며 포맷되지 않은 것 같습니다.";
		break;
	case 0x83:
		strError = L"대상이 읽기 전용 DVD이며 포맷되지 않았을 수 있습니다.";
		break;
	case 0x84:
		strError = L"대상은 쓰기 가능한 CD-ROM이며 포맷되지 않은 것 같습니다.";
		break;
	case 0x85:
		strError = L"작업과 관련된 파일이 대상 미디어 또는 파일 시스템에 비해 너무 큽니다.";
		break;
	case 0x86:
		strError = L"소스는 읽기 전용 CD-ROM이며 포맷되지 않은 것 같습니다.";
		break;
	case 0x87:
		strError = L"소스는 읽기 전용 DVD이며 포맷되지 않은 것 같습니다.";
		break;
	case 0x88:
		strError = L"소스는 쓰기 가능한 CD-ROM이며 포맷되지 않은 것 같습니다.";
		break;
	case 0xB7:
		strError = L"작업 중 MAX_PATH를 초과했습니다.";
		break;
	case 0x402:
		strError = L"알 수없는 오류가 발생했습니다. 일반적으로 소스 또는 대상의 경로가 잘못 되었기 때문입니다. Windows Vista 이상에서는이 오류가 발생하지 않습니다.";
		break;
	case 0x10000:
		strError = L"대상에서 지정되지 않은 오류가 발생했습니다.";
		break;
	case 0x10000 | 0x74:
		strError = L"대상은 루트 디렉토리이며 이름을 바꿀 수 없습니다.";
		break;
	default:
		strError = L"알 수없는 오류가 발생했습니다.";
		break;
	}
#else
	switch (nError)
	{
	case 0x71:
		strError = L"The source and destination files are the same file.";
		break;
	case 0x72:
		strError = L"There are multiple file paths specified in the source buffer, but only one destination file path is specified.";
		break;
	case 0x73:
		strError = L"Although the rename operation was specified, the directory has a different destination path. Use the move operation instead.";
		break;
	case 0x74:
		strError = L"The source is a root directory, which cannot be moved or renamed.";
		break;
	case 0x75:
		strError = L"The operation was canceled by the user, or silently canceled if an appropriate flag was sent to SHFileOperation.";
		break;
	case 0x76:
		strError = L"The destination is a subtree of the source.";
		break;
	case 0x78:
		strError = L"Security settings denied access to the source.";
		break;
	case 0x79:
		strError = L"The source or destination path exceeded or would exceed MAX_PATH.";
		break;
	case 0x7A:
		strError = L"The move operation may fail because the operation involves multiple destination paths.";
		break;
	case 0x7C:
		strError = L"The path in the source and/or destination is invalid.";
		break;
	case 0x7D:
		strError = L"The source and destination have the same parent folder.";
		break;
	case 0x7E:
		strError = L"The destination path is an existing file.";
		break;
	case 0x80:
		strError = L"The destination path is an existing folder.";
		break;
	case 0x81:
		strError = L"The name of the file exceeds MAX_PATH.";
		break;
	case 0x82:
		strError = L"The destination is a read-only CD-ROM and is likely not formatted.";
		break;
	case 0x83:
		strError = L"The destination is a read-only DVD and is likely not formatted.";
		break;
	case 0x84:
		strError = L"The destination is a writable CD-ROM and is likely not formatted.";
		break;
	case 0x85:
		strError = L"The file involved in the operation is too large for the destination media or file system.";
		break;
	case 0x86:
		strError = L"The source is a read-only CD-ROM and is likely not formatted.";
		break;
	case 0x87:
		strError = L"The source is a read-only DVD and is likely not formatted.";
		break;
	case 0x88:
		strError = L"The source is a writable CD-ROM and is likely not formatted.";
		break;
	case 0xB7:
		strError = L"Exceeded MAX_PATH during the operation.";
		break;
	case 0x402:
		strError = L"An unknown error has occurred. This typically happens due to an invalid path in the source or destination. This error does not occur on Windows Vista or later.";
		break;
	case 0x10000:
		strError = L"An unspecified error has occurred on the destination.";
		break;
	case 0x10000 | 0x74:
		strError = L"The destination is a root directory, which cannot be renamed.";
		break;
	default:
		strError = L"An unknown error has occurred.";
		break;
	}
#endif
	return strError;
}


BOOL GF_FindFile(CString strPath , CString strFile)
{
	CString strtemp;
	CFileFind finder;
	BOOL bResult;
	if (strPath.Right(1) != '\\')
		strPath += L"\\";
	bResult = finder.FindFile(strPath + L"*.*");
	while (bResult)
	{
		bResult = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
			continue;
		strtemp = finder.GetFileName();
		if (strtemp.CompareNoCase(strFile) == 0)
			return TRUE;
	}
	return FALSE;
}


BYTE GF_FacpType(CString strFType)
{
	for (int i = FACP_TYPE_NONE; i < FACP_TYPE_COUNT; i++)
	{
		CString strFacpType = _T("");
		strFacpType.Format(_T("%s"), g_szFacpTypeString[i]);

		if (strFType.CompareNoCase(strFacpType) == 0)
			return i;
	}

	return FACP_TYPE_NONE;
}

BYTE GF_UnitType(CString strUType)
{
	for (int i = UNIT_TYPE_NONE; i < UNIT_TYPE_COUNT; i++)
	{
		CString strUnitType = _T("");
		strUnitType.Format(_T("%s"), g_szUnitTypeString[i]);

		if (strUnitType.CompareNoCase(strUType) == 0)
			return i;
	}

	return UNIT_TYPE_P_TYPE_FACP;
}

int GF_Unicode2ASCII(WCHAR *uni, char *ascii, int nBuffSize)
{
	USES_CONVERSION;
	strcpy_s(ascii, nBuffSize, W2A(uni));
	int nLen = (int)strlen(ascii);

// 	ascii = W2A(uni);
// 	int nLen = WideCharToMultiByte(CP_ACP, 0, uni, -1, NULL, 0, NULL, NULL);
// 	WideCharToMultiByte(CP_ACP, 0, uni, -1, ascii, nLen, 0, 0);
	return nLen;
}

int GF_ASCII2Unicode(char *ascii, WCHAR *uni, int nBuffSize)
{
	int nLen = 0;
	USES_CONVERSION;
	wcscpy_s(uni, nBuffSize, A2W(ascii));
	nLen = (int)wcslen(uni);
	//LPTSTR lpStr = ascii;
// 	nLen = MultiByteToWideChar(CP_ACP, 0, ascii, -1, NULL, NULL);
// 	MultiByteToWideChar(CP_ACP, 0, ascii, -1, uni, nLen);
	return nLen;
}


int GF_ASCII2Unicode(char *ascii, CString &str, int nBuffSize)
{
	int nLen = 0;
	USES_CONVERSION;
	//wcscpy_s(uni, nBuffSize, A2W(ascii));
	str = A2W(ascii);
	nLen = str.GetLength();
	//LPTSTR lpStr = ascii;
	// 	nLen = MultiByteToWideChar(CP_ACP, 0, ascii, -1, NULL, NULL);
	// 	MultiByteToWideChar(CP_ACP, 0, ascii, -1, uni, nLen);
	return nLen;
}

void GF_CreateFolder(CString csPath)
{
	// UpdateData(TRUE);
	// csPath = m_csTopFolderName + csPath;

	CString csPrefix(_T("")), csToken(_T(""));
	int nStart = 0, nEnd;
	while ((nEnd = csPath.Find('\\', nStart)) >= 0)
	{
		CString csToken = csPath.Mid(nStart, nEnd - nStart);
		CreateDirectory(csPrefix + csToken, NULL);

		csPrefix += csToken;
		csPrefix += _T("\\");
		nStart = nEnd + 1;
	}
	csToken = csPath.Mid(nStart);
	CreateDirectory(csPrefix + csToken, NULL);
}

WORD wTable[] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a,
	0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294,
	0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462,
	0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509,
	0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695,
	0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
	0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948,
	0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4,
	0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b,
	0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f,
	0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
	0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046,
	0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290,
	0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e,
	0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691,
	0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
	0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d,
	0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16,
	0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8,
	0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e,
	0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93,
	0x3eb2, 0x0ed1, 0x1ef0}; 

WORD GF_CRC16(WORD wCrc , BYTE bData)
{
	return wTable[(wCrc >> 8) & 0xFF] ^ (wCrc << 8) ^(bData & 0xFF);
}

TCHAR *szNumber[] = {
	L"0",
	L"1",
	L"2",
	L"3",
	L"4",
	L"5",
	L"6",
	L"7",
	L"8",
	L"9",
};

BOOL GF_Text2Number(CString strNumber , int &nRet)
{
	vector<int> vn;
	int nSize, i , x;
	CString str;
	nSize = strNumber.GetLength();
	for (i = nSize -1; i >= 0; i --)
	{
		str = strNumber.GetAt(i);
		for (x = 0; x < 10; x++)
		{
			if (str == szNumber[x])
			{
				vn.push_back(x);
				break;
			}
		}
	}
	nRet = 0; 
	nSize = (int)vn.size();
	if (nSize <= 0)
		return FALSE;

	for (i = 0; i < nSize; i++)
	{
		nRet += (int)pow(10, i) * vn[i];
	}

	return TRUE;
}

void GF_AddLog(CString strLog)
{
	theApp.InsertLog(strLog);
	if (g_bLogWrite)
		WriteMainLog(strLog);
}

void GF_AddLog(const WCHAR * szFmt, ...)
{
	CString     strLog;
	va_list     argptr;
	int         cnt;
	SYSTEMTIME  tm;
	WCHAR        szLog[10240] = { 0 };
	GetLocalTime(&tm);
	strLog.Format(L"%sLog\\MainLog%04d%02d%02d.log", g_strAppPath, tm.wYear, tm.wMonth, tm.wDay);

	va_start(argptr, szFmt);
	cnt = vswprintf_s(szLog, szFmt, argptr);
	va_end(argptr);
	theApp.InsertLog(szLog);
	
	if (g_bLogWrite)
		WriteMainLog(strLog);
}


void GF_AddDebug(CString strLog)
{
	theApp.InsertDebug(strLog);
	if (g_bLogWrite)
		WriteMainLog(strLog);
	TRACE(strLog + L"\n");
}

void GF_AddDebug(const WCHAR * szFmt, ...)
{
	CString     strLog;
	va_list     argptr;
	int         cnt;
	SYSTEMTIME  tm;
	WCHAR        szLog[10240] = { 0 };
	GetLocalTime(&tm);
	strLog.Format(L"%sLog\\MainLog%04d%02d%02d.log", g_strAppPath, tm.wYear, tm.wMonth, tm.wDay);

	va_start(argptr, szFmt);
	cnt = vswprintf_s(szLog, szFmt, argptr);
	va_end(argptr);
	theApp.InsertDebug(szLog);

	if (g_bLogWrite)
		WriteMainLog(szLog);
	strLog = szLog;
	TRACE(strLog + L"\n");

}

void WriteMainLog(CString str)
{
	CString     strLog;
	SYSTEMTIME  tm;
	FILE        *fpLog = NULL;
	WCHAR        szLog[10240] = { 0 };

	GetLocalTime(&tm);
	strLog.Format(L"%sLog\\MainLog%04d%02d%02d.log", g_strAppPath, tm.wYear, tm.wMonth, tm.wDay);

	int nError = _wfopen_s(&fpLog, strLog, L"a+, ccs=UTF-8");
	if (fpLog != NULL)
	{
		fwprintf(fpLog, L"[%02d:%02d:%02d.%03d] %s\n",
			tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds,
			str.GetBuffer());

		fclose(fpLog);
	}
}

void WriteMainLog(const WCHAR * szFmt, ...)
{
	CString     strLog;
	va_list     argptr;
	int         cnt;
	SYSTEMTIME  tm;
	FILE        *fpLog = NULL;
	WCHAR        szLog[10240] = { 0 };

	GetLocalTime(&tm);
	strLog.Format(L"%sLog\\MainLog%04d%02d%02d.log", g_strAppPath, tm.wYear, tm.wMonth, tm.wDay);

	va_start(argptr, szFmt);
	cnt = vswprintf_s(szLog, szFmt, argptr);
	va_end(argptr);

	int nError = _wfopen_s(&fpLog, strLog, L"a+, ccs=UTF-8");
	if (fpLog != NULL)
	{
		fwprintf(fpLog, L"[%02d:%02d:%02d.%03d] %s\n",
			tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds,
			szLog);

		fclose(fpLog);
	}
}

CString GF_GetSafeArrayValue(COleSafeArray * pSa,int nRow,int nCol)
{
	CString strRet;
	long lNumRows;
	long lNumCols;
	long index[2];
	INT_PTR nTemp;
	VARIANT val;

	if(pSa == nullptr)
		return L"";

	pSa->GetUBound(1,&lNumRows);
	pSa->GetUBound(2,&lNumCols);

	if(nRow < 1 || nRow > lNumRows)
		return L"";
	if(nCol < 1 || nCol > lNumCols)
		return L"";
	index[0] = nRow;
	index[1] = nCol;
	pSa->GetElement(index,&val);
	strRet = L"";
	switch(val.vt)
	{
	case VT_R8:
		nTemp = val.dblVal;
		strRet.Format(L"%d",nTemp);
		break;
	case VT_BSTR:
		strRet.Format(L"%s",val.bstrVal);
		break;
	case VT_UNKNOWN:
		strRet = L"";
		break;
	case VT_EMPTY:
		strRet = L"";
		break;
	case VT_NULL:
		strRet = _T("");
		break;
	case VT_I1:
		strRet.Format(_T("%d"),V_I1(&val));
		break;
	case VT_I2:
		strRet.Format(_T("%hd"),V_I2(&val));
		break;
	case VT_I4:
		strRet.Format(_T("%d"),V_I4(&val));
		break;
	case VT_I8:
		strRet.Format(_T("%ld"),V_I8(&val));
		break;
	case VT_R4:
		nTemp = V_R4(&val);
		strRet.Format(L"%d",nTemp); 
		break;
		// 	case VT_BOOL:
		// 		return strBOOL(V_BOOL(&var));
		// 	case VT_VARIANT:
		// 		strRet = _T("VT_VARIANT");
		// 		break;

	case VT_UI1:
		strRet.Format(_T("%d"),(unsigned short)V_UI1(&val));
		break;
	case VT_UI2:
		strRet.Format(_T("%d"),(unsigned short)V_UI2(&val));
		break;
	case VT_UI4:
		strRet.Format(_T("%d"),(unsigned short)V_UI4(&val));
		break;
	case VT_UI8:
		strRet.Format(_T("%ld"),(unsigned short)V_UI8(&val));
		break;
	case VT_CY:
		strRet = COleCurrency(val).Format();
		break;
	}
	return strRet;

}