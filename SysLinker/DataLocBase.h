#pragma once

// CDataLocBase 명령 대상입니다.

class CDataLocBase : public CObject
{
public:
	CDataLocBase();
	virtual ~CDataLocBase();

	SIMPLE_FUNC_IMPL(short,LocType,m_sLocType);
	SIMPLE_FUNC_IMPL(CString,BuildName,m_strBuildName);
	SIMPLE_FUNC_IMPL(INT_PTR,BuildID,m_nBuildID);
	SIMPLE_FUNC_IMPL(CString,BTypeName,m_strBTypeName);
	SIMPLE_FUNC_IMPL(INT_PTR,BTypeID,m_nBTypeID);

	SIMPLE_FUNC_IMPL(CString,StairName,m_strStairName);
	SIMPLE_FUNC_IMPL(INT_PTR,StairID,m_nStairID);
	SIMPLE_FUNC_IMPL(int,StairNum,m_nStairNum);
	SIMPLE_FUNC_IMPL(CString,FloorName,m_strFloorName);
	SIMPLE_FUNC_IMPL(INT_PTR,FloorID,m_nFloorID);
	SIMPLE_FUNC_IMPL(int,FloorNumber,m_nFlNum);
	SIMPLE_FUNC_IMPL(BYTE,MiddleFloor,m_btMiddleFlag);

	SIMPLE_FUNC_IMPL(CString,RoomName,m_strRoomName);
	SIMPLE_FUNC_IMPL(INT_PTR,RoomID,m_nRoomID);
protected:
	short m_sLocType; // 건물 층 ,...

	CString m_strBuildName;
	INT_PTR	m_nBuildID;

	CString m_strBTypeName;
	INT_PTR	m_nBTypeID;

	CString m_strStairName;
	INT_PTR	m_nStairID;
	// [KHS 2020-5-22 08:31:32] 
	// 사용하지 않음
	int		m_nStairNum;


	CString m_strFloorName;
	INT_PTR	m_nFloorID;
	int		m_nFlNum; //< 1층은 0 , 지하는 -1부터 시작
					  //< 지층,지하,B : 분리하여 숫자 * -1
					  //< 층 정보가 없을 때는  1000 N_FLOOR_NONE_NAME
					  //< 
	BYTE	m_btMiddleFlag; //< 중층 플래그 ,

	CString m_strRoomName;
	INT_PTR	m_nRoomID;


public:
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

	virtual CDataLocBase * GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid) {
		return nullptr;
	};

	CString GetLocationString();

	BOOL IsSame(CDataLocBase * pSrc);
	BOOL CopyData(CDataLocBase * pSrc);
	void SetLocationInfo(
		CString strBuild,INT_PTR nBuildID
		,CString strBType = L"-",INT_PTR nBtypeID = -1
		,CString strStair = L"-",INT_PTR nStair = -1
		,int nStairNum = -1
		,CString strFloor = L"-",INT_PTR nFloor = -1
		,CString strRoom = L"-",INT_PTR nRoom = -1
		//, int nFlNum, BYTE btMiddle
		,int nFlNum = N_FLOOR_NONE_NAME,BYTE btMiddle = 0

	);

	void SetBuildingInfo(CString strName,INT_PTR nId) { m_strBuildName = strName; m_nBuildID = nId; };
	void SetBTypeInfo(CString strName,INT_PTR nId) { m_strBTypeName = strName; m_nBTypeID = nId; };
	void SetStairInfo(CString strName,INT_PTR nId,int nStairNum) { m_strStairName = strName; m_nStairID = nId; m_nStairNum = nStairNum; };
	void SetFloorInfo(CString strName,INT_PTR nId,int nFlNum = N_FLOOR_NONE_NAME,BYTE btMiddle = 0);
	void SetRoomInfo(CString strName,INT_PTR nId) { m_strRoomName = strName; m_nRoomID = nId; };

};


