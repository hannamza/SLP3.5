#pragma once

#include <map>
#include "DataLocation.h"

// CMapLocation 명령 대상입니다.

class CMapLocation : public std::map<CString, CDataLocation*>
{
public:
	CMapLocation();
	virtual ~CMapLocation();
	CDataLocation * AddLocation(
		int nType
		, CString strKey
		, CString strBuild
		, CString strBType = L""
		, CString strStair = L""
		, CString strFloor = L""
		, CString strRoom = L""
	);

	// 
	/*!
	 * @fn         GetWholeLocID
	 * @version    1.0
	 * @date       2019-12-02 19:47:04
	 * @param      
	 * @brief      이미 있는 위치 정보는 -1을 반환한다.
	 * @return     return
	 * @note 
	 */
// 	INT_PTR GetWholeLocID(
// 		int nType
// 		, CString strBuild
// 		, CString strBType = L""
// 		, CString strStair = L""
// 		, CString strFloor = L""
// 		, CString strRoom = L""
// 	);
	INT_PTR GetWholeBuild(CString strName);
	INT_PTR GetWholeBType(CString strBuild, CString strBType);
	INT_PTR GetWholeStair(CString strBuild, CString strBType, CString strStair);
	INT_PTR GetWholeFloor(CString strBuild, CString strBType, CString strStair, CString strFloor);
	INT_PTR GetWholeRoom(CString strBuild, CString strBType, CString strStair, CString strFloor, CString strRoom);

	INT_PTR SetBuildData(CDataLocation * pLoc , CString strName);
	INT_PTR SetBTypeData(CDataLocation * pLoc, CString strBuild, CString strBType);
	INT_PTR SetStairData(CDataLocation * pLoc, CString strBuild, CString strBType, CString strStair);
	INT_PTR SetFloorData(CDataLocation * pLoc, CString strBuild, CString strBType, CString strStair, CString strFloor);
	INT_PTR SetRoomData (CDataLocation * pLoc, CString strBuild, CString strBType, CString strStair, CString strFloor, CString strRoom);

	CDataLocation * GetLocation(int nBid, int nBtype, int nSid, int nFlid, int nRmid);
	CDataLocation * GetLocation(CString strLocKey);
	

	CString	CvtBuildIDToName(int nBid);
	CString	CvtBTypeIDToName(int nBid, int nBtype);
	CString CvtStairIDToName(int nBid, int nBtype, int nSid);
	CString CvtFloorIDToName(int nBid, int nBtype, int nSid, int nFlid);
	CString CvtRoomIDToName(int nBid, int nBtype, int nSid, int nFlid, int nRmid);

};


