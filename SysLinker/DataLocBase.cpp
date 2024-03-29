// DataLocBase.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DataLocBase.h"


// CDataLocBase

CDataLocBase::CDataLocBase()
{
	m_sLocType = LT_NONE;

	SetBuildingInfo(L"-",-1);
	SetBTypeInfo(L"-",-1);
	SetStairInfo(L"-",-1,-1);
	SetFloorInfo(L"-",-1);
	SetRoomInfo(L"-",-1);
}

CDataLocBase::~CDataLocBase()
{
	RemoveAllData();
}


// CDataLocBase ��� �Լ�

CDataLocBase * CDataLocBase::AddNewLocation(
	int nType
	,CString strBuild,int nBid
	,CString strBType,int nTid
	,CString strStair,int nSid
	,CString strFloor,int nFid
	,CString strRoom,int nRid
	,int nFlNum,BYTE btMiddle
)
{
	return nullptr;
}

void CDataLocBase::RemoveAllData()
{
}
// CDataLocBuild ��� �Լ�

CString CDataLocBase::GetLocationString()
{
	CString strLocation = L"";
	CString strBuild,strStair,strBType,strFloor,strRoom;
	strBuild = strStair = strBType = strFloor = strRoom = L"";

	if(m_strBuildName != L"-" && m_strBuildName != L" ")
		strBuild = m_strBuildName;
	if(m_strBTypeName != L"-"&& m_strBTypeName != L" ")
		strBType = m_strBTypeName;
	if(m_strStairName != L"-"&& m_strStairName != L" ")
		strStair = m_strStairName;
	if(m_strFloorName != L"-"&& m_strFloorName != L" ")
		strFloor = m_strFloorName;
	if(m_strRoomName != L"-"&& m_strRoomName != L" ")
		strRoom = m_strRoomName;
	strBuild.TrimLeft();
	strBType.TrimLeft();
	strStair.TrimLeft();
	strFloor.TrimLeft();
	strRoom.TrimLeft();

	strBuild.TrimRight();
	strBType.TrimRight();
	strStair.TrimRight();
	strFloor.TrimRight();
	strRoom.TrimRight();

	CString arr[5];
	arr[0] = strBuild;
	arr[1] = strBType;
	arr[2] = strStair;
	arr[3] = strFloor;
	arr[4] = strRoom;

	strLocation = L"";
	for(int i = 0; i < 5; i++)
	{
		if(i == m_sLocType)
			break;
		if(arr[i].GetLength() <= 0)
			continue;
		strLocation += arr[i];
		strLocation += L" ";
	}
	strLocation.TrimRight();
	return strLocation;
}

void CDataLocBase::SetFloorInfo(CString strName,INT_PTR nId,int nFlNum2,BYTE btMiddle)
{
	BOOL bRet = FALSE;
	//int nFlNum= 1000;
	m_strFloorName = strName;
	m_nFloorID = nId;
	m_nFlNum = nFlNum2;
	m_btMiddleFlag = btMiddle;
	if(nFlNum2 >= N_FLOOR_NONE_NAME)
	{
		if(strName != L"" && strName != L"-")
		{
			if((strName.Find(L"����") >= 0) || (strName.Find(L"����") >= 0)
				|| (strName.Find(L"B") >= 0) || (strName.Find(L"b") >= 0))
			{
				strName.Replace(L"����",L"");
				strName.Replace(L"����",L"");
				strName.Replace(L"����",L"");
				strName.Replace(L"B",L"");
				strName.Replace(L"b",L"");
				strName.Replace(L"��",L"");
				strName.Replace(L"F",L"");
				try
				{
					m_nFlNum = _wtoi(strName) * -1;
				}
				catch(...)
				{
					m_nFlNum = N_FLOOR_NONE_NAME;
				}
			}
			// 1.4���� ���� R��,PH�� �и� [11/3/2021 KHS]
			else if((strName.Find(L"R") >= 0) || (strName.Find(L"r") >= 0) || (strName.Find(L"����") >= 0))
			{
				strName.Replace(L"����",L"");
				strName.Replace(L"R",L"");
				strName.Replace(L"��",L"");
				strName.Replace(L"F",L"");
				try
				{
					bRet = GF_Text2Number(strName,m_nFlNum);
					if(bRet == FALSE)
						m_nFlNum = N_FLOOR_R_NONE_NUMBER;
					else
						m_nFlNum += N_FLOOR_R_NONE_NUMBER;
				}
				catch(...)
				{
					m_nFlNum = N_FLOOR_R_NONE_NUMBER;
				}
			}
			else if((strName.Find(L"��ž") >= 0) || (strName.Find(L"PH") >= 0) || (strName.Find(L"ph") >= 0)
				|| (strName.Find(L"Ph") >= 0) || (strName.Find(L"pH") >= 0)
				)
			{

				//strName.Replace(L"��ž", L"");
				//strName.Replace(L"r", L"");
				strName.Replace(L"PH",L"");
				strName.Replace(L"ph",L"");
				strName.Replace(L"��",L"");
				strName.Replace(L"F",L"");
				try
				{
					bRet = GF_Text2Number(strName,m_nFlNum);
					if(bRet == FALSE)
						m_nFlNum = N_FLOOR_PH_NONE_NUMBER;
					else
						m_nFlNum += N_FLOOR_PH_NONE_NUMBER;
				}
				catch(...)
				{
					m_nFlNum = N_FLOOR_PH_NONE_NUMBER;
				}
			}
			else
			{
				strName.Replace(L"��",L"");
				strName.Replace(L"F",L"");
				try
				{
					bRet = GF_Text2Number(strName,m_nFlNum);
					if(bRet == FALSE)
						m_nFlNum = N_FLOOR_NONE_NAME;
				}
				catch(...)
				{
					m_nFlNum = N_FLOOR_NONE_NAME;
				}
			}
		}
	}
}

void CDataLocBase::SetLocationInfo(
	CString strBuild,INT_PTR nBuildID
	,CString strBType,INT_PTR nBtypeID
	,CString strStair,INT_PTR nStair
	,int nStairNum
	,CString strFloor,INT_PTR nFloor
	,CString strRoom,INT_PTR nRoom
	,int nFlNum,BYTE btMiddle
)
{
	m_strBuildName = strBuild;
	m_strBTypeName = strBType;
	m_strStairName = strStair;
	m_strFloorName = strFloor;
	m_strRoomName = strRoom;

	m_nBuildID = nBuildID;
	m_nBTypeID = nBtypeID;
	m_nStairID = nStair;
	m_nStairNum = nStairNum;
	m_nFloorID = nFloor;
	m_nFlNum = nFlNum;
	m_btMiddleFlag = btMiddle;

	if(strFloor != L"" && strFloor != L"-")
	{
		// �� ���ڸ� �Է����� �ʾ����� �����Ѵ�.
		if(nFlNum == N_FLOOR_NONE_NAME)
			SetFloorInfo(strFloor,nFloor);
	}
	m_nRoomID = nRoom;
}


BOOL CDataLocBase::IsSame(CDataLocBase * pSrc)
{
	CString strLocation = L"";
	CString strBuild,strStair,strBType,strFloor,strRoom;
	strBuild = strStair = strBType = strFloor = strRoom = L"";
	if(pSrc == nullptr)
		return FALSE;
	return TRUE;
}


BOOL CDataLocBase::CopyData(CDataLocBase * pSrc)
{
	m_sLocType = pSrc->GetLocType();

	SetLocationInfo(pSrc->GetBuildName(),pSrc->GetBuildID(),pSrc->GetBTypeName(),pSrc->GetBTypeID()
		,pSrc->GetStairName(),pSrc->GetStairID(),pSrc->GetStairNum()
		,pSrc->GetFloorName(),pSrc->GetFloorID()
		,pSrc->GetRoomName(),pSrc->GetRoomID(),pSrc->GetFloorNumber(),pSrc->GetMiddleFloor()
	);
	return TRUE;
}