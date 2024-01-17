#pragma once
#include "DataLocBase.h"

// CDataLocRoom 명령 대상입니다.

class CDataLocRoom : public CDataLocBase
{
public:
	CDataLocRoom();
	virtual ~CDataLocRoom();
	//INT_PTR	m_nBuildID;

	
// 	virtual void RemoveAllData();
// 
// 	virtual CDataLocBase * AddNewLocation(
// 		int nType
// 		,CString strBuild
// 		,CString strBType = L""
// 		,CString strStair = L""
// 		,CString strFloor = L""
// 		,CString strRoom = L""
// 	);

// 	virtual CDataLocBase * AddLocation(
// 		int nType
// 		,CString strBuild
// 		,CString strBType = L""
// 		,CString strStair = L""
// 		,CString strFloor = L""
// 		,CString strRoom = L""
// 	);
};


