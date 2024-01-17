#pragma once
#include "DataLocBase.h"

// CDataLocBType 명령 대상입니다.
class CDataLocStair;
typedef std::map<CString,CDataLocStair*> CMapLocStair;
class CDataLocBType : public CDataLocBase
{
public:
	CDataLocBType();
	virtual ~CDataLocBType();

	CMapLocStair		m_mapStair;
	CByteArray			m_arrID;

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
	INT_PTR GetWholeStair(CString strStair);
	CDataLocStair * AddStairData(CString strStair,int nSid = -1); /// 기존 이름으로 있으면 가져오고 없으면 추가
	virtual CDataLocBase * GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid);

	CDataLocBase * GetLocation(int nType
		,CString strBuild
		,CString strBType = L""
		,CString strStair = L""
		,CString strFloor = L""
		,CString strRoom = L""
	);

	CDataLocBase * FindStairData(int nBid,int nBtype,int nSid);
	int FillLocationTree(CTreeCtrl * pCtrl,HTREEITEM hParent);
	int FillLocationTree2(CTreeCtrl * pCtrl,int nLocType,HTREEITEM hParent);
};


