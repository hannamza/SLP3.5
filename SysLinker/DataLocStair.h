#pragma once
#include "DataLocBase.h"

// CDataLocStair 명령 대상입니다.
class CDataLocFloor;
typedef std::map<CString,CDataLocFloor*> CMapLocFloor;
class CDataLocStair : public CDataLocBase
{
public:
	CDataLocStair();
	virtual ~CDataLocStair();
	CByteArray			m_arrID;
	CMapLocFloor	m_mapFloor;
	virtual void RemoveAllData();
	virtual CDataLocBase * AddNewLocation(
		int nType
		,CString strBuild,int nBid
		,CString strBType = L"",int nTid = -1
		,CString strStair = L"",int nSid = -1
		,CString strFloor = L"",int nFid = -1
		,CString strRoom = L"",int nRid = -1
		,int nFlNum = N_FLOOR_NONE_NAME,BYTE btMiddle = 0
	);

	INT_PTR GetWholeFloor(CString strFloor);
	CDataLocFloor * AddFloorData(CString strFloor,int nFid
		,int nFlNum = N_FLOOR_NONE_NAME,BYTE btMiddle = 0
	); /// 기존 이름으로 있으면 가져오고 없으면 추가
	virtual CDataLocBase * GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid);

	CDataLocBase * GetLocation(int nType
		,CString strBuild
		,CString strBType = L""
		,CString strStair = L""
		,CString strFloor = L""
		,CString strRoom = L""
	);

	CDataLocBase * FindFloorData(int nBid,int nBtype,int nSid,int nFlid);
	int FillLocationTree(CTreeCtrl * pCtrl,HTREEITEM hParent);
	int FillLocationTree2(CTreeCtrl * pCtrl,int nLocType,HTREEITEM hParent);
};


