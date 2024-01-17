#pragma once
#include "DataLocBase.h"

// CManagerLocation 명령 대상입니다.
class CDataLocBuild;
class CManagerLocation : public CObject
{
public:
	CManagerLocation();
	virtual ~CManagerLocation();
	CPtrList		m_ptrBuildList;
	CByteArray		m_arrID;

	void RemoveAllData();

	// 새로운 위치정보를 등록할 때 사용 - Project 생성 시 
	CDataLocBase *  AddNewLocation(
		int nType
		,CString strBuild			,int nBid
		,CString strBType = L""		,int nTid =-1
		,CString strStair = L""		,int nSid = -1
		,CString strFloor = L""		,int nFid = -1
		,CString strRoom = L""		,int nRid = -1
		,int nFlNum = N_FLOOR_NONE_NAME,BYTE btMiddle = 0
	);
	


	INT_PTR GetWholeBuild(CString strName);

	CDataLocBuild * AddNewBuildData(CString strName,int nBid); /// 기존 이름으로 있으면 가져오고 없으면 추가
	CDataLocBuild * FindBuildData(CString strName);
	CDataLocBuild * FindBuildData(int nBuild);
	CDataLocBase * GetBuildData(int nBid);

// 
// 	INT_PTR GetWholeBuild(CString strName);
// 	INT_PTR GetWholeBType(CString strBuild,CString strBType);
// 	INT_PTR GetWholeStair(CString strBuild,CString strBType,CString strStair);
// 	INT_PTR GetWholeFloor(CString strBuild,CString strBType,CString strStair,CString strFloor);
// 	INT_PTR GetWholeRoom(CString strBuild,CString strBType,CString strStair,CString strFloor,CString strRoom);
// 
// 	INT_PTR SetBuildData(CDataLocation * pLoc,CString strName);
// 	INT_PTR SetBTypeData(CDataLocation * pLoc,CString strBuild,CString strBType);
// 	INT_PTR SetStairData(CDataLocation * pLoc,CString strBuild,CString strBType,CString strStair);
// 	INT_PTR SetFloorData(CDataLocation * pLoc,CString strBuild,CString strBType,CString strStair,CString strFloor);
// 	INT_PTR SetRoomData (CDataLocation * pLoc,CString strBuild,CString strBType,CString strStair,CString strFloor,CString strRoom);

	CDataLocBase * GetLocation(int nBid,int nBtype = -1,int nSid = -1,int nFlid = -1,int nRmid = -1);
	//CDataLocBase * GetLocation(CString strLocKey);
	CDataLocBase * GetLocation(int nType
		,CString strBuild
		,CString strBType = L""
		,CString strStair = L""
		,CString strFloor = L""
		,CString strRoom = L""
	);

	CPtrList * GetBuildList() { return &m_ptrBuildList; }

	int FillLocationTree(CTreeCtrl * pCtrl);
	int FillLocationTree2(CTreeCtrl * pCtrl,int nLocType);

 	CString	CvtBuildIDToName(int nBid);
 	CString	CvtBTypeIDToName(int nBid,int nBtype);
 	CString CvtStairIDToName(int nBid,int nBtype,int nSid);
 	CString CvtFloorIDToName(int nBid,int nBtype,int nSid,int nFlid);
 	CString CvtRoomIDToName(int nBid,int nBtype,int nSid,int nFlid,int nRmid);

	int	CvtBuildNameToID(CString strBuild);
	int	CvtBTypeNameToID(CString strBuild,CString strBType);
	int CvtStairNameToID(CString strBuild,CString strBType,CString strStair);
	int CvtFloorNameToID(CString strBuild,CString strBType,CString strStair,CString strFloor);
	int CvtRoomNameToID (CString strBuild,CString strBType,CString strStair,CString strFloor,CString strRoom);
};


