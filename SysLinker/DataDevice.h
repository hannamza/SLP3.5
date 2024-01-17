#pragma once

// CDataDevice 명령 대상입니다.
class CDataLocation;
class CDataEquip;
class CDataPattern;
class CDataLinked;
class CDataLocBase;

class CDataDevice : public CObject
{
public:
	CDataDevice();
	CDataDevice(
		int nFacpID, int nUnitID, int nChannelID, int nRelayID
		, CString strKey
		,CDataLocBase	* pInputLoc,CDataLocBase	* pOutputLoc
		, CDataEquip * pEqInput, CDataEquip * pEqOut, CDataEquip * pEqOutContents
		, CDataEquip * pEqName, CString strAddEqIdx
		, int nRepeator
		, int nScrID
		, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
		, int nRelayIndex
	);
	virtual ~CDataDevice();


	SIMPLE_FUNC_IMPL(CString, DevAddress, m_strAddress);
	SIMPLE_FUNC_IMPL(CString, DevKey, m_strKey);
	SIMPLE_FUNC_IMPL(int, DeviceID, m_nDeviceID);
	SIMPLE_FUNC_IMPL(int, FacpID, m_nFacpID);
	SIMPLE_FUNC_IMPL(int, UnitID, m_nUnitID);
	SIMPLE_FUNC_IMPL(int, ChnID, m_nChannelID);

	SIMPLE_FUNC_IMPL(CString, InputFullName, m_strFullInputName);
	SIMPLE_FUNC_IMPL(CString, OutputFullName, m_strFullOutputName);
	SIMPLE_FUNC_IMPL(CString, OutContentsFullName, m_strFullOutCotents);
 	SIMPLE_FUNC_IMPL(short, DeviceNum, m_sDeviceNum);
 	SIMPLE_FUNC_IMPL(short, FacpNum, m_sFacpNum);
 	SIMPLE_FUNC_IMPL(short, UnitNum, m_sUnitNum);
 	SIMPLE_FUNC_IMPL(short, ChnNum, m_sChannel);
	SIMPLE_FUNC_IMPL(short, RepeatorNum, m_sRepeatorNum);
	SIMPLE_FUNC_IMPL(short, RelayCount, m_sRelayCount);

	SIMPLE_FUNC_IMPL(CDataLocBase*, DevInputLocation, m_pRefInputLoc);
	SIMPLE_FUNC_IMPL(CDataLocBase*, DevOutputLocation, m_pRefOutputLoc);
	SIMPLE_FUNC_IMPL(CDataEquip*, EqName, m_pRefEqName);
	SIMPLE_FUNC_IMPL(CDataEquip*, EqInput, m_pRefEqInput);
	SIMPLE_FUNC_IMPL(CDataEquip*, EqOutput, m_pRefEqOutput);
	SIMPLE_FUNC_IMPL(CDataEquip*, EqOutContents, m_pRefEqOutContents);
	//SIMPLE_FUNC_IMPL(CString, EquipName, m_strEqName);
	SIMPLE_FUNC_IMPL(CString, EqAddIndex, m_strEqAddIndex);

	SIMPLE_FUNC_IMPL(int, ScreenID, m_nScreenID);
	SIMPLE_FUNC_IMPL(int, RelayIndex, m_nRelayIndex);
	SIMPLE_FUNC_IMPL(BOOL, NeedMakeID, m_bNeedMakeID);

	INT_PTR			GetLinkItemCount(){return m_ptrLinkList.GetCount();}
	CPtrList	* GetLinkedList() { return &m_ptrLinkList; }

protected:
	BOOL		m_bNeedMakeID;
	int		m_nRelayIndex;
	CString m_strAddress; /// Map 키 , 주소 0000-0000(수신기-유닛-계통-회로)
	CString m_strKey; // Map 키 , GF_GetSysDataKey의 리턴값(수신기.유닛.계통.회로.타입)
	short m_sFacpNum;
	short m_sUnitNum;
	short m_sChannel;
	short m_sDeviceNum;

	int m_nFacpID;
	int m_nUnitID;
	int m_nChannelID;
	int m_nDeviceID;

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
	CPtrList				m_ptrLinkList; //< CDataLinked 

	// [KHS 2020-3-6 16:21:51] 
	// ROM 데이터 - 역변환 임시 데이터
	BYTE					m_btTempLinkCnt;
	BYTE			*		m_pLinkStartPointer;
	BYTE					m_btInType;
	BYTE					m_btOutType;
	CString					m_strTempName;
	// 	// Pattern
// 	CString		m_strPtnName;
// 	CString		m_strPtnNum;

// 	// 위치
// 	CString m_strBuildName; // 동
// 	INT_PTR	m_nBuildId;
// 	CString m_strStairName; // 계단
// 	INT_PTR	m_nStairId;
// 	CString m_strFloorName; // 층
// 	INT_PTR	m_nFloorId;
// 	CString m_strRoomName;
// 	INT_PTR	m_nRoomId;

// 	// 입력 타입, 입력 이름
// 	CString m_strInpuType;
// 	INT_PTR	m_nInputTypeId;
// 
// 	// output type ,name
// 	CString m_strOutputType;
// 	INT_PTR	m_nOutputTypeId;
// 	CString m_strOutName;

	


public:
	void ReMakeAddress();
	LPVOID GetRev01Pointer() { return m_lpRev01; }
	LPVOID GetRev02Pointer() { return m_lpRev02; }
	LPVOID GetRev03Pointer() { return m_lpRev03; }
	void SetRev01Pointer(LPVOID pData) { m_lpRev01 = pData; }
	void SetRev02Pointer(LPVOID pData) { m_lpRev02 = pData; }
	void SetRev03Pointer(LPVOID pData) { m_lpRev03 = pData; }
	void SetAddressInfo(int nFacpID, int nUnitID, int nChannelID, int nRelayID
		, int nFacpNum, int nUnitNum, int nChnNum,int nRelayNum
	);
	void SetLocationInfo(CDataLocBase	* pInputLoc,CDataLocBase	* pOutputLoc);
	void SetEquipInfo(CDataEquip * pEqInput,CDataEquip * pEqName, CString strAddEqIdx
		, CDataEquip * pEqOut, CDataEquip * pEqOutContents);
	void SetDeviceData(
		int nFacpID, int nUnitID, int nChannelID, int nRelayID 
		, CString strKey
		,CDataLocBase	* pInputLoc,CDataLocBase	* pOutputLoc
		, CDataEquip * pEqInput, CDataEquip * pEqOut, CDataEquip * pEqOutContents
		, CDataEquip * pEqName, CString strAddEqIdx
		, int nRepeator
		, int nScrID
		, LPVOID lpRev01 , LPVOID lpRev02 , LPVOID lpRev03
		, int nRelayIndex
	);
	void SetDeviceFromRomData(
		int nFacpID, int nUnitID, int nChannelID, int nRelayID
		, CString strKey
		, CDataEquip * pInType, CDataEquip * pOutType, CDataEquip * pEqName, CDataEquip * pCont
		,CDataLocBase * pInLoc,CDataLocBase * pOutLoc
		, CString strTempName
		, CString strEquip, CString strAddIdx
		, int nFacpNum, int nUnitNum, int nChnNum, int nRelayNum
		, int nRelayIndex
	);
	CString MakeFullName(int nType);
	CString MakeCompareName(int nDuplicationNumber);
	CString MakeAddressRemoveName();
	int RemoveAllLink();
	int RemoveLinkByType(int nType); //
	void AddLink(BOOL bFirstAdd, CDataLinked * pLink);
	void AddLink(BOOL bFirstAdd, CDataDevice * pDev
		, byte btLinkType, byte btLogicType, int nLogicID
	);
	void AddLink(BOOL bFirstAdd, int nTgtFacp, int nTgtUnit, int nTgtChn, int nTgtDev
		, byte btLinkType, byte btLogicType, int nLogicID
	);
	POSITION FindLink(CDataLinked* pLink);
	POSITION FindLinkByID(int nLnkType , int nFacpID, int nUnitID, int nChnID, int nRelayID , BOOL bCheckLnkType= TRUE);
	void DeleteLink(CDataLinked* pLink);
	void DeleteLinkByID(int nLnkType, int nFacpID, int nUnitID, int nChnID, int nRelayID);
	BOOL IsEqual(CDataDevice* pDev);
	BOOL IsEqualName(CDataDevice* pDev);
	BOOL IsEqualAddress(CDataDevice* pDev);
	int CopyData(CDataDevice * pSrc);
	int CopyExceptLink(CDataDevice * pSrc);
	int CopyLink(CDataDevice * pSrc , int nType); // 전체 : 0 , LOGIC_MANUAL -1 , LOGIC_ALL_AUTO-2
	CString GetInputTypeName();
	CString GetOutputTypeName();
	CString GetEquipName();
	CString GetEquipLocationName();
	CString GetOutContentsName();
	CString GetInputLocationName();
	CString GetOutputLocationName();
	CDataLinked * GetLink(int nLnkType
		, int nFacpID, int nUnitID, int nChnID, int nRelayID
		, BOOL bCheckLnkType = TRUE);
};


