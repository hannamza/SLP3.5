#pragma once
#include "DataLocBase.h"

// CDataLocBuild 명령 대상입니다.
class CDataLocBType;
typedef std::map<CString,CDataLocBType*> CMapLocBType;
class CDataLocBuild : public CDataLocBase
{
public:
	CDataLocBuild();
	virtual ~CDataLocBuild();
	CMapLocBType	m_mapBType;

	CByteArray		m_arrID;
	virtual void RemoveAllData();

	CDataLocBase * AddNewLocation(
		int nType
		,CString strBuild,int nBid
		,CString strBType = L"",int nTid = -1
		,CString strStair = L"",int nSid = -1
		,CString strFloor = L"",int nFid = -1
		,CString strRoom = L"",int nRid = -1
		,int nFlNum = N_FLOOR_NONE_NAME,BYTE btMiddle = 0
	);
	INT_PTR GetWholeBtypeId(CString strBType);
	CDataLocBType * AddBTypeData(CString strBType,int nTid); /// 기존 이름으로 있으면 가져오고 없으면 추가

	virtual CDataLocBase * GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid);
	CDataLocBase * GetLocation(int nType
		,CString strBuild
		,CString strBType = L""
		,CString strStair = L""
		,CString strFloor = L""
		,CString strRoom = L""
	);

	CDataLocBase * FindBTypeData(int nBid,int nBtype);

	int FillLocationTree(CTreeCtrl * pCtrl,HTREEITEM hParent);
	int FillLocationTree2(CTreeCtrl * pCtrl,int nLocType,HTREEITEM hParent);

};


