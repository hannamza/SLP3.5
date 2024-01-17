// DataAutoLogic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataAutoLogic.h"


// CDataAutoLogic

CDataAutoLogic::CDataAutoLogic()
{
	m_nLgId = 0;
	m_nInType = 0;
	m_nOutType = 0;
	m_nEqName = 0;
	m_nOutContents = 0;

	m_btAllFloor = 0;
	m_btEmergency = 0;
	m_btOutput = 0;
	m_btPluseNFloor = 0;

	m_btMatchBuild = 0;
	m_btMatchBType = 0;
	m_btMatchStair = 0;
	m_btMatchFloor = 0;
	m_btMatchRoom = 0;

	m_btUnderBasic = 0;
	m_btUnderClassBuild = 0;
	m_btUnderClassBType = 0;
	m_btUnderClassStair = 0;
	m_btUnder1F = 0;
	m_btUnderB1F = 0;

	m_btUnderParking = 0;

}

CDataAutoLogic::~CDataAutoLogic()
{
}


// CDataAutoLogic 멤버 함수
void CDataAutoLogic::SetAutoLogic(
	int nId, int nIntype, int nOuttype, int nName, int nCont
	, BYTE btEmer, BYTE btAll, BYTE btout, BYTE btNFloor
	, BYTE btMatchBuild, BYTE btMatchBType, BYTE btMatchStair, BYTE btMatchFloor, BYTE btMatchRoom
	, BYTE btUnderBasic, BYTE btUnderBuild, BYTE btUnderBType, BYTE btUnderStair, BYTE btUnder1F, BYTE btUnderB1F
	, BYTE btUnderParking
	//, CString strName
)
{
	m_nLgId = nId;
	m_nInType = nIntype;
	m_nOutType = nOuttype;
	m_nEqName = nName;
	m_nOutContents = nCont;


	m_btEmergency = btEmer;
	m_btAllFloor = btAll;
	m_btOutput = btout;
	m_btPluseNFloor = btNFloor;

	m_btMatchBuild = btMatchBuild;
	m_btMatchBType = btMatchBType;
	m_btMatchStair = btMatchStair;
	m_btMatchFloor = btMatchFloor;
	m_btMatchRoom = btMatchRoom;

	m_btUnderBasic = btUnderBasic;
	m_btUnderClassBuild = btUnderBuild;
	m_btUnderClassBType = btUnderBType;
	m_btUnderClassStair = btUnderStair;
	m_btUnder1F = btUnder1F;
	m_btUnderB1F = btUnderB1F;

	m_btUnderParking = btUnderParking;
}