#pragma once
#include "DataEquip.h"
#include "DataLocBase.h"
#include "XMakeLink_Def.h"

class CXListDev;
class CDataDevice;
class CDataLocBase;
class CDataEquip;
class CXPatternMst;
class CXDataEm;
class CXDataDev : public CObject
{
public:
	CXDataDev();
	virtual ~CXDataDev();
public:
	SIMPLE_FUNC_IMPL(CString,DevAddress,m_strAddress);
	SIMPLE_FUNC_IMPL(CString,DevKey,m_strKey);
	// 	SIMPLE_FUNC_IMPL(int,DeviceID,m_nDeviceID);
	// 	SIMPLE_FUNC_IMPL(int,FacpID,m_nFacpID);
	// 	SIMPLE_FUNC_IMPL(int,UnitID,m_nUnitID);
	// 	SIMPLE_FUNC_IMPL(int,ChnID,m_nChannelID);

	SIMPLE_FUNC_IMPL(CString,InputFullName,m_strFullInputName);
	SIMPLE_FUNC_IMPL(CString,OutputFullName,m_strFullOutputName);
	SIMPLE_FUNC_IMPL(CString,OutContentsFullName,m_strFullOutCotents);
	SIMPLE_FUNC_IMPL(short,DeviceNum,m_sDeviceNum);
	SIMPLE_FUNC_IMPL(short,FacpNum,m_sFacpNum);
	SIMPLE_FUNC_IMPL(short,UnitNum,m_sUnitNum);
	SIMPLE_FUNC_IMPL(short,ChnNum,m_sChannel);
	SIMPLE_FUNC_IMPL(short,RepeatorNum,m_sRepeatorNum);
	SIMPLE_FUNC_IMPL(short,RelayCount,m_sRelayCount);

	SIMPLE_FUNC_IMPL(CDataLocBase*,DevInputLocation,m_pRefInputLoc);
	SIMPLE_FUNC_IMPL(CDataLocBase*,DevOutputLocation,m_pRefOutputLoc);
	SIMPLE_FUNC_IMPL(CDataEquip*,EqName,m_pRefEqName);
	SIMPLE_FUNC_IMPL(CDataEquip*,EqInput,m_pRefEqInput);
	SIMPLE_FUNC_IMPL(CDataEquip*,EqOutput,m_pRefEqOutput);
	SIMPLE_FUNC_IMPL(CDataEquip*,EqOutContents,m_pRefEqOutContents);
	//SIMPLE_FUNC_IMPL(CString, EquipName, m_strEqName);
	SIMPLE_FUNC_IMPL(CString,EqAddIndex,m_strEqAddIndex);

	SIMPLE_FUNC_IMPL(int,ScreenID,m_nScreenID);
	SIMPLE_FUNC_IMPL(int,RelayIndex,m_nRelayIndex);
	SIMPLE_FUNC_IMPL(BOOL,NeedMakeID,m_bNeedMakeID);


	SIMPLE_FUNC_IMPL(int,BuildIndex,m_nBuildIndex);
	SIMPLE_FUNC_IMPL(int,BTypeIndex,m_nBTypeIndex);
	SIMPLE_FUNC_IMPL(int,StairIndex,m_nStairIndex);
	SIMPLE_FUNC_IMPL(int,FloorIndex,m_nFloorIndex);
	SIMPLE_FUNC_IMPL(int,RoomIndex,m_nRoomIndex);

	int GetFacpID() { return GF_GetFacpID(m_suAddr.dwAddr); }
	int GetUnitID() { return GF_GetUnitID(m_suAddr.dwAddr);}
	int GetChnID() { return GF_GetChnID(m_suAddr.dwAddr); }
	int GetDeviceID() { return GF_GetDevID(m_suAddr.dwAddr); }

	DWORD GetLogicalAddr() { return m_suAddr.dwAddr; }
	void SetLogicalAddr(DWORD dwAddr) { m_suAddr.dwAddr = dwAddr; }
	LPVOID GetRev01Pointer() { return m_lpRev01; }
	LPVOID GetRev02Pointer() { return m_lpRev02; }
	LPVOID GetRev03Pointer() { return m_lpRev03; }
	void SetRev01Pointer(LPVOID pData) { m_lpRev01 = pData; }
	void SetRev02Pointer(LPVOID pData) { m_lpRev02 = pData; }
	void SetRev03Pointer(LPVOID pData) { m_lpRev03 = pData; }

	CString GetLocBuildName() {
		if(m_pRefInputLoc)return m_pRefInputLoc->GetBuildName(); return L"";
	}
	CString GetLocBtypeName() {
		if(m_pRefInputLoc)return m_pRefInputLoc->GetBTypeName(); return L"";
	}
	CString GetLocStairName() {
		if(m_pRefInputLoc)return m_pRefInputLoc->GetStairName(); return L"";
	}
	CString GetLocFloorName() {
		if(m_pRefInputLoc)return m_pRefInputLoc->GetFloorName(); return L"";
	}
	CString GetLocRoomName() {
		if(m_pRefInputLoc)return m_pRefInputLoc->GetRoomName(); return L"";
	}
	int		GetLocFloorNumber() {
		if(m_pRefInputLoc)return m_pRefInputLoc->GetFloorNumber(); return 0;
	}

	int GetInType();


public:
	BOOL		m_bInput; // 해당 회로가 설비명인지 출력설명인지 구분
						  // [2025/7/31 9:54:16 KHS] 
						  // 설비명일 때 - 연동데이터를 저장한다.
						  //	std::shared_ptr<CzListDev> m_spListLinkOutput; // 출력회로
						  //	std::shared_ptr<CzListDev> m_spListLinkEtc;// 접점,패턴,비상방송등...
						  //	std::unordered_set<DWORD>	m_LinkDevSet;
	//CXMapDev					m_MapDev;
	CXMapLink					m_MapLink;
	CPtrList					m_ptrPatternList;
	CPtrList					m_ptrEtcList;
	//std::bitset<UINT_MAX>		m_bitoutput;
protected:
	BOOL		m_bNeedMakeID;
	int		m_nRelayIndex;
	CString m_strAddress; /// Map 키 , 주소 0000-0000(수신기-유닛-계통-회로)
	CString m_strKey; // Map 키 , GF_GetSysDataKey의 리턴값(수신기.유닛.계통.회로.타입)
	short m_sFacpNum;
	short m_sUnitNum;
	short m_sChannel;
	short m_sDeviceNum;

	// 	int m_nFacpID;
	// 	int m_nUnitID;
	// 	int m_nChannelID;
	// 	int m_nDeviceID;
	SU_LOGICAL_ADDR m_suAddr;

	//중계기의 회로수
	short m_sRepeatorNum;  /// 중계기의 시작 회로번호
	short m_sRelayCount; /// 중계기 회로수

	int m_nScreenID;
	//
	CDataLocBase	*		m_pRefInputLoc;
	CDataLocBase	*		m_pRefOutputLoc;

	CDataEquip		*		m_pRefEqInput;
	CDataEquip		*		m_pRefEqOutput;
	CDataEquip		*		m_pRefEqOutContents;


	// 설비 번호
	//CString	m_strEqName;
	CDataEquip		*		m_pRefEqName;
	CString					m_strEqAddIndex; // 설비 번호 , 방에 감지기1,감지기2 에서 숫자 부분

	CString					m_strFullInputName;
	CString					m_strFullOutputName;
	CString					m_strFullOutCotents;
	// [KHS 2020-8-10 15:02:49] 
	//Key : 입력타입.설비명.출력타입.출력내용.위치정보.중복대비번호(중복일때번호입력)
	CString					m_strCompareName;


	// Reserve
	LPVOID					m_lpRev01;
	LPVOID					m_lpRev02;
	LPVOID					m_lpRev03;	//< 임시로 사용 , Rom에서 읽어온 연동데이터를 임시로 저장한다.
										//< Address(number-facp.unit.channel,relay)의 리스트를 저장한다.
										//< Rom에서 읽을 때 CPtrList의 포인터를 생성
										//< Rom 데이터를 다 읽고 나서 CDataLinked로 수정한다.


										// [KHS 2020-3-6 16:21:51] 
										// ROM 데이터 - 역변환 임시 데이터
	BYTE					m_btTempLinkCnt;
	BYTE			*		m_pLinkStartPointer;
	BYTE					m_btInType;
	BYTE					m_btOutType;
	CString					m_strTempName;

	// [2025/11/13 11:06:12 KHS] 
	// Cz로 시작하는 위치 정보의 Index를 구한 다음 설정한다.
	// 이 값들은 건물,종류,계단,층,실 이름을 비교할 때 String비교하면 속도가 떨어져서 Int값을 비교하기 위해 생성한다.
	int						m_nBuildIndex;
	int						m_nBTypeIndex;
	int						m_nStairIndex;
	int						m_nFloorIndex;
	int						m_nRoomIndex;
public:
	BOOL CopyData(CDataDevice * pPnt); // 연동데이터 제외
	BOOL CopyData(CXDataDev * pPnt); // 연동데이터 제외
	void RemoveAllData();
	int CompareData(CDataDevice * pPnt);
	int CompareData(CXDataDev * pPnt);

	//BOOL AddLinkList(std::unordered_set<DWORD> devset);
	BOOL AddLinkMap(CXMapLink * pMapLink);
	BOOL AddLinkPattern(CXPatternMst * pPtn);
	BOOL AddLinkEtc(CXDataEm * pEm,int nLogicID);
	BOOL RemoveMapLink();
};

