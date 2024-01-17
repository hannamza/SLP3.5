// MapLocation.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "MapLocation.h"
#include <vector>
#include <algorithm>

// CMapLocation

CMapLocation::CMapLocation()
{
}

CMapLocation::~CMapLocation()
{
}


// CMapLocation 멤버 함수

CDataLocation * CMapLocation::AddLocation(
	int nType
	, CString strKey
	, CString strBuild
	, CString strBType
	, CString strStair
	, CString strFloor
	, CString strRoom
)
{
	int nID = -1;
	CDataLocation * pData = nullptr;
	pData = new CDataLocation(nType, strKey);
	switch (nType)
	{
	case LT_BUILD:
		if (SetBuildData(pData, strBuild) < 0)
		{
			delete pData;
			pData = nullptr;
		}
		break;
	case LT_BTYPE:
		if (SetBTypeData(pData, strBuild, strBType) < 0)
		{
			delete pData;
			pData = nullptr;
		}
		break;
	case LT_STAIR:
		if (SetStairData(pData, strBuild, strBType , strStair) < 0)
		{
			delete pData;
			pData = nullptr;
		}
		break;
	case LT_FLOOR:
		if (SetFloorData(pData, strBuild, strBType, strStair,strFloor) < 0)
		{
			delete pData;
			pData = nullptr;
		}
		break;
	case LT_ROOM:
		if (SetRoomData(pData, strBuild, strBType, strStair, strFloor,strRoom) < 0)
		{
			delete pData;
			pData = nullptr;
		}
		break;
	}
	
	return pData;
}

// 
// INT_PTR CMapLocation::GetWholeLocID(
// 	int nType 
// 	, CString strBuild
// 	, CString strBType
// 	, CString strStair
// 	, CString strFloor
// 	, CString strRoom
// )
// {
// 	INT_PTR nRet = -1;
// 	switch (nType)
// 	{
// 	case LT_BUILD:
// 		return GetWholeBuild(strBuild);
// 	case LT_BTYPE:
// 		strLocation = strBuild + strBType;
// 		break;
// 	case LT_STAIR:
// 		strLocation = strBuild + strBType + strStair;
// 		break;
// 	case LT_FLOOR:
// 		strLocation = strBuild + strBType + strStair + strFloor;
// 		break;
// 	case LT_ROOM:
// 		strLocation = strBuild + strBType + strStair + strFloor + strRoom;
// 		break;
// 	}
// 	return INT_PTR();
// }


INT_PTR CMapLocation::GetWholeBuild(CString strName)
{
	INT_PTR nPrev, nNew, nRet = 0;
	int i = 0 , nSize =0; 
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;
	if (size() <= 0)
		return 1;
	
	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != LT_BUILD)
			continue;
		if (pData->GetBuildName() == strName)
			return -1;
		vtID.push_back(pData->GetBuildID());
		nSize++;
	}
	sort(vtID.begin(), vtID.end());
	if (nSize <= 0)
		return 1;

	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	return nRet;
}


INT_PTR CMapLocation::GetWholeBType(CString strBuild, CString strBType)
{
	INT_PTR nPrev, nNew, nRet = 0;
	int i = 0, nSize = 0;
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;
	if (size() <= 0)
		return 1;

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != LT_BTYPE)
			continue;
		if (pData->GetBuildName() != strBuild)
			continue;
		if (pData->GetBTypeName() == strBType)
			return -1;

		vtID.push_back(pData->GetBuildID());
		nSize++;
	}
	sort(vtID.begin(), vtID.end());
	if (nSize <= 0)
		return 1;

	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	return nRet;
}


INT_PTR CMapLocation::GetWholeStair(CString strBuild, CString strBType, CString strStair)
{
	INT_PTR nPrev, nNew, nRet = 0;
	int i = 0, nSize = 0;
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;
	if (size() <= 0)
		return 1;

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != LT_STAIR)
			continue;
		if (pData->GetBuildName() != strBuild)
			continue;
		if (pData->GetBTypeName() != strBType)
			continue;
		if (pData->GetStairName() == strStair)
			return -1;
		vtID.push_back(pData->GetBuildID());
		nSize++;
	}
	sort(vtID.begin(), vtID.end());
	if (nSize <= 0)
		return 1;

	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	return nRet;
}


INT_PTR CMapLocation::GetWholeFloor(CString strBuild, CString strBType
	, CString strStair, CString strFloor)
{
	INT_PTR nPrev, nNew, nRet = 0;
	int i = 0, nSize = 0;
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;
	if (size() <= 0)
		return 1;

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != LT_FLOOR)
			continue;
		if (pData->GetBuildName() != strBuild)
			continue;
		if (pData->GetBTypeName() != strBType)
			continue;
		if (pData->GetStairName() != strStair)
			continue;
		if (pData->GetFloorName() == strFloor)
			return -1;

		vtID.push_back(pData->GetBuildID());
		nSize++;
	}
	sort(vtID.begin(), vtID.end());
	if (nSize <= 0)
		return 1;

	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	return nRet;
}

INT_PTR CMapLocation::GetWholeRoom(CString strBuild, CString strBType
	, CString strStair, CString strFloor, CString strRoom)
{
	INT_PTR nPrev, nNew, nRet = 0;
	int i = 0, nSize = 0;
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;
	if (size() <= 0)
		return 1;

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != LT_ROOM)
			continue;
		if (pData->GetBuildName() != strBuild)
			continue;
		if (pData->GetBTypeName() != strBType)
			continue;
		if (pData->GetStairName() != strStair)
			continue;
		if (pData->GetFloorName() != strFloor)
			continue;
		if (pData->GetRoomName() == strRoom)
			return -1;

		vtID.push_back(pData->GetBuildID());
		nSize++;
	}
	sort(vtID.begin(), vtID.end());
	if (nSize <= 0)
		return 1;

	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	return nRet;
}



INT_PTR CMapLocation::SetBuildData(CDataLocation * pLoc, CString strName)
{
	INT_PTR nPrev, nNew, nRet = 0;
	int i = 0, nSize = 0;
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;
	if (size() <= 0)
	{
		pLoc->SetBuildingInfo(strName, 1);
		return 1;
	}

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != LT_BUILD)
			continue;
		if (pData->GetBuildName() == strName)
			return -1;
		vtID.push_back(pData->GetBuildID());
		nSize++;
	}
	sort(vtID.begin(), vtID.end());
	if (nSize <= 0)
	{
		pLoc->SetBuildingInfo(strName, 1);
		return 1;
	}

	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;

	pLoc->SetBuildingInfo(strName, nRet);
	return nRet;
}


INT_PTR CMapLocation::SetBTypeData(CDataLocation * pLoc, CString strBuild, CString strBType)
{
	INT_PTR nPrev, nNew, nRet = 0;
	INT_PTR nBuild =0;
	int i = 0, nSize = 0;
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		
		if (pData->GetBuildName() != strBuild)
			continue;
		nBuild = pData->GetBuildID();
		if (pData->GetLocType() != LT_BTYPE)
			continue;
		if (pData->GetBTypeName() == strBType)
			return -1;
		vtID.push_back(pData->GetBTypeID());
		nSize++;
	}
	sort(vtID.begin(), vtID.end());
	if (nBuild <= 0)
	{
		// 추가
		//AfxMessageBox(L"건물정보 아이디를 찾을 수 없습니다. 건물종류 정보를 입력할 수 없습니다.");
		USERLOG(L"%s의 아이디를 찾을 수 없습니다. \n" , strBuild);
		return -1;
	}

	if (nSize <= 0)
	{
		pLoc->SetLocationInfo(strBuild, nBuild, strBType, 1);
		return 1;
	}

	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	pLoc->SetLocationInfo(strBuild, nBuild, strBType, nRet);
	return nRet;
}


INT_PTR CMapLocation::SetStairData(CDataLocation * pLoc, CString strBuild, CString strBType, CString strStair)
{
	INT_PTR nPrev, nNew, nRet = 0;
	int i = 0, nSize = 0;
	int nNum = 0;

	INT_PTR nBuild = 0 , nBtype = 0; 
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	std::vector<int> vtNum;
	nPrev = nNew = 0;

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetBuildName() != strBuild)
			continue;
		nBuild = pData->GetBuildID();
		if (pData->GetBTypeName() != strBType)
			continue;
		nBtype = pData->GetBTypeID();
		if (pData->GetLocType() != LT_STAIR)
			continue; 
		if (pData->GetStairName() == strStair)
			return -1;
		vtID.push_back((int)pData->GetStairID());
		vtNum.push_back(pData->GetStairNum());
		nSize++;
	}

	if (nBuild <= 0 || nBtype <= 0)
	{
		//AfxMessageBox(L"건물정보 아이디 또는 건물종류 아이디를 가져오는데 실패했습니다.");
		USERLOG(L"건물정보(%s) , 건물종류(%s)의 아이디를 찾을 수 없습니다. \n", strBuild , strBType);
		return -1;
	}

	sort(vtID.begin(), vtID.end());
	if (nSize <= 0)
	{
		if (strStair == L"" || strStair == L"-")
			nNum = 0;
		else
			nNum = 1;
		pLoc->SetLocationInfo(strBuild, nBuild, strBType, nBtype , strStair , 1 , nNum);
		return 1;
	}

	for (i = 0; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;

	sort(vtNum.begin(), vtNum.end());
	for (i = 0; i < vtNum.size(); i++)
	{
		nNew = vtNum[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nNum = nNew + 1;

	pLoc->SetLocationInfo(strBuild, nBuild, strBType, nBtype, strStair, nRet ,nNum);
	return nRet;
}


INT_PTR CMapLocation::SetFloorData(CDataLocation * pLoc, CString strBuild, CString strBType
	, CString strStair, CString strFloor)
{
	INT_PTR nPrev, nNew, nRet = 0;
	INT_PTR nBuild=0, nBtype=0, nStair=0,nSNum=0;
	int i = 0, nSize = 0;
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		
		if (pData->GetBuildName() != strBuild)
			continue;
		nBuild = pData->GetBuildID();
		if (pData->GetBTypeName() != strBType)
			continue;
		nBtype = pData->GetBTypeID();
		if (pData->GetStairName() != strStair)
			continue;
		nStair = pData->GetStairID();
		nSNum = pData->GetStairNum();
		if (pData->GetLocType() != LT_FLOOR)
			continue; 
		if (pData->GetFloorName() == strFloor)
			return -1;

		vtID.push_back(pData->GetFloorID());
		nSize++;
	}

	if (nBuild <= 0 || nBtype <= 0 || nStair <=0)
	{
		USERLOG(L"건물정보(%s) , 건물종류(%s), 계단정보(%s)의 아이디를 찾을 수 없습니다. \n"
			, strBuild, strBType , strStair);
		return -1;
	}

	if (nSize <= 0)
	{
		pLoc->SetLocationInfo(strBuild, (INT_PTR)nBuild, strBType, (INT_PTR)nBtype
			, strStair, (INT_PTR)nStair , (INT_PTR)nSNum, strFloor , 1
		);
		return 1;
	}

	// 층 ID Sort
	sort(vtID.begin(), vtID.end());
	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	pLoc->SetLocationInfo(strBuild, (INT_PTR)nBuild, strBType, (INT_PTR)nBtype
		, strStair, (INT_PTR)nStair, (INT_PTR)nSNum, strFloor, (INT_PTR)nRet
	);
	return nRet;
}

INT_PTR CMapLocation::SetRoomData(CDataLocation * pLoc, CString strBuild, CString strBType
	, CString strStair, CString strFloor, CString strRoom)
{
	INT_PTR nPrev, nNew, nRet = 0;
	INT_PTR nBuild = 0, nBtype = 0, nStair = 0,nFloor=0;
	int i = 0, nSize = 0 , nSNum;
	CDataLocation * pData = nullptr;
	std::vector<INT_PTR> vtID;
	nPrev = nNew = 0;

	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		
		if (pData->GetBuildName() != strBuild)
			continue;
		nBuild = pData->GetBuildID();
		if (pData->GetBTypeName() != strBType)
			continue;
		nBtype = pData->GetBTypeID();
		if (pData->GetStairName() != strStair)
			continue;
		nStair = pData->GetStairID();
		nSNum = pData->GetStairNum();
		if (pData->GetFloorName() != strFloor)
			continue;
		nFloor = pData->GetFloorID();
		if (pData->GetLocType() != LT_ROOM)
			continue;
		if (pData->GetRoomName() == strRoom)
			return -1;

		vtID.push_back(pData->GetRoomID());
		nSize++;
	}

	if (nBuild <= 0 || nBtype <= 0 || nStair <= 0 || nFloor <= 0)
	{
		USERLOG(L"건물정보(%s) , 건물종류(%s), 계단정보(%s),층정보(%s)의 아이디를 찾을 수 없습니다. \n"
			, strBuild, strBType, strStair,strFloor
		); 
		return -1;
	}

	sort(vtID.begin(), vtID.end());
	if (nSize <= 0)
	{
		pLoc->SetLocationInfo(strBuild, nBuild, strBType, nBtype
			, strStair, nStair, nSNum,strFloor, nFloor, strRoom, 1
		);
		return 1;
	}

	for (; i < vtID.size(); i++)
	{
		nNew = vtID[i];
		if (nNew - nPrev >= 2)
			return nPrev + 1;
		nPrev = nNew;
	}
	nRet = nNew + 1;
	pLoc->SetLocationInfo(strBuild, nBuild, strBType, nBtype
		, strStair, nStair, nSNum, strFloor, nFloor, strRoom, nRet
	);
	return nRet;
}

CDataLocation * CMapLocation::GetLocation(int nBid, int nBtype, int nSid, int nFlid, int nRmid)
{
	CDataLocation * pData = nullptr;
	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetBuildID() != nBid)
			continue;;
		if (pData->GetBTypeID() != nBtype)
			continue;;
		if (pData->GetStairID() != nSid)
			continue;;
		if (pData->GetFloorID() != nFlid)
			continue;;
		if (pData->GetRoomID() != nRmid)
			continue;;
		return pData;
	}
	return nullptr;
}


CDataLocation * CMapLocation::GetLocation(CString strLocKey)
{
	return (*this)[strLocKey];
	//return nullptr;
}

CString CMapLocation::CvtBuildIDToName(int nBid)
{
	CDataLocation * pData = nullptr;
	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != PTN_BUILD)
			continue; 
		if (pData->GetBuildID() != nBid)
			continue;;
		return pData->GetBuildName();
	}
	return L"-";
}


CString CMapLocation::CvtBTypeIDToName(int nBid, int nBtype)
{
	CDataLocation * pData = nullptr;
	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != PTN_BTYPE)
			continue;
		if (pData->GetBuildID() != nBid)
			continue;;
		if (pData->GetBTypeID() != nBtype)
			continue;;
		return pData->GetBTypeName();
	}
	return L"-";
}

CString CMapLocation::CvtStairIDToName(int nBid, int nBtype, int nSid)
{
	CDataLocation * pData = nullptr;
	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != PTN_STAIR)
			continue;
		if (pData->GetBuildID() != nBid)
			continue;;
		if (pData->GetBTypeID() != nBtype)
			continue;;
		if (pData->GetStairID() != nSid)
			continue;;
		return pData->GetStairName();
	}
	return L"-";
}

CString CMapLocation::CvtFloorIDToName(int nBid, int nBtype, int nSid, int nFlid)
{
	CDataLocation * pData = nullptr;
	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != PTN_FLOOR)
			continue;
		if (pData->GetBuildID() != nBid)
			continue;;
		if (pData->GetBTypeID() != nBtype)
			continue;;
		if (pData->GetStairID() != nSid)
			continue;;
		if (pData->GetFloorID() != nFlid)
			continue;;
		return pData->GetFloorName();
	}
	return L"-";
}

CString CMapLocation::CvtRoomIDToName(int nBid, int nBtype, int nSid, int nFlid, int nRmid)
{
	CDataLocation * pData = nullptr;
	iterator it;
	for (it = begin(); it != end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (pData->GetLocType() != PTN_ROOM)
			continue;
		if (pData->GetBuildID() != nBid)
			continue;;
		if (pData->GetBTypeID() != nBtype)
			continue;;
		if (pData->GetStairID() != nSid)
			continue;;
		if (pData->GetFloorID() != nFlid)
			continue;;
		if (pData->GetRoomID() != nRmid)
			continue;;
		return pData->GetRoomName();
	}
	return L"-";
}
