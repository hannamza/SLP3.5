#pragma once

#include "MapSystemData.h"
#include "MapLocation.h"
#include "ManagerLocation.h"
// CRelayTableData
class CManagerEquip;
class CManagerPattern;
class CManagerPS;
class CManagerPump;
class CManagerAutoLogic;
class CManagerEmergency;
class CDataPS;
class CDataPump;
class CDataEquip;
class CDataUnit;
class CDataFacp;
class CDataChannel;
class CDataLocBase;
class CDataPattern;
class CTreeListCtrl;
class YAdoDatabase;
class CDataDevice;
class CDataLinked;
class CDataEmBc;
class CDataFacpRelay;
class CManagerFacpRelay;
class CExcelWrapper;
class CManagerLocation;
class CDataLocBase;
class CRelayTableData : public CObject
{
public:
	CRelayTableData();
	virtual ~CRelayTableData();

	SIMPLE_FUNC_IMPL(CString, PrjName, m_strPrjName);
	SIMPLE_FUNC_IMPL(CString, SiteName, m_strSiteName);
	SIMPLE_FUNC_IMPL(CString, SitePhone, m_strSitePhone);
	SIMPLE_FUNC_IMPL(CString, PrjMaker, m_strPrjMaker);
	SIMPLE_FUNC_IMPL(WORD, PrjMajorNum, m_wPrjMajorNum);
	SIMPLE_FUNC_IMPL(WORD, PrjMinorNum, m_wPrjMinorNum);
	//SIMPLE_FUNC_IMPL(CString, PrjEnvSetPath, m_strPrjEnvSetPath);
	SIMPLE_FUNC_IMPL(CString, DBName, m_strDBName);
	SIMPLE_FUNC_IMPL(CString, DBPwd, m_strDBPwd);
	SIMPLE_FUNC_IMPL(CString, DBUser, m_strDBUser);
	SIMPLE_FUNC_IMPL(COleDateTime, PrjCreateDate, m_dtCreate);
	SIMPLE_FUNC_IMPL(COleDateTime, PrjModifyDate, m_dtModify);
	SIMPLE_FUNC_IMPL(CString, PrjModifier, m_strPrjModifier);
	SIMPLE_FUNC_IMPL(YAdoDatabase *, PrjDB, m_pDB);
	SIMPLE_FUNC_IMPL(CString, CurrentUser, m_strCurrentUser);
	SIMPLE_FUNC_IMPL(BOOL, ProjectOpened, m_bOpened);
	SIMPLE_FUNC_IMPL(BOOL, ProjectChanged, m_bChangeFlag);
	SIMPLE_FUNC_IMPL(int, LastRelayIndex, m_nLastRelayIndex);

	SIMPLE_FUNC_IMPL(BOOL, IsComparedData, m_bIsComparedData);	//20240326 GBM - 메모리 누수 오류 수정
	SIMPLE_FUNC_IMPL(BOOL, NewEquipmentTypeAdded, m_bNewEquipmentTypeAdded);

	CStringList* GetRelayTableList() { return &m_strFileNameList; }

	// [KHS 2021-1-11 18:24:48] 
	// 수신기,유닛,채널,회로의 ID와 번호를 동일하게 하기위해
	// SLP3의 이전버전에서 ID와 회로번호를 통일하지 않음
	// Relay Index를 추가하면서 동일하게 수정
	BOOL									m_bIDEqualNumber;///< ID와 번호가 동일함
	int										m_nAllocRelayIndexType;///< ID와 번호가 동일함

	int										m_nLastRelayIndex;///< m_nAllocRelayIndexType가 부분 생성일때 사용안함
															///< 생성된 아이디가 INT_MAX보다 클때 m_nAllocRelayIndexType는 부분 생성
	YAdoDatabase					*		m_pDB;

	CMapSystemData							m_MapSystemData;
	// [KHS 2020-6-9 14:41:31] 
	// ID로 디바이스를 검색하는데 시간이 너무 오래 걸려 Map을 하나 생성한다.
	CMapIDSystemData						m_MapIDSystemData;
	
	// Excel파일 리스트
	CStringList		m_strFileNameList;

	// 프로젝트 정보
	CString			m_strPrjName;
	CString			m_strSiteName;
	CString			m_strSitePhone;
	CString			m_strPrjMaker;
	WORD			m_wPrjMajorNum;
	WORD			m_wPrjMinorNum;
	DWORD			m_dwPrjVersion;
	//CString			m_strPrjEnvSetPath; //< 환경설정에서 설정된 Project의 Root Path, 개별 Project의 Base Path가 아님
	CString			m_strDBName;
	CString			m_strDBPwd;
	CString			m_strDBUser;
	COleDateTime	m_dtCreate;
	COleDateTime	m_dtModify;
	CString			m_strPrjModifier;

	// global Data info
	// equip
	std::shared_ptr<CManagerEquip>		m_spRefInputEquipManager;
	std::shared_ptr<CManagerEquip>		m_spRefEqNameManager;
	std::shared_ptr<CManagerEquip>		m_spRefOutputEquipManager;
	std::shared_ptr<CManagerEquip>		m_spRefContentsEquipManager;
	std::shared_ptr<CManagerEquip>		m_spRefPumpEquipManager;
	std::shared_ptr<CManagerEquip>		m_spRefPSEquipManager;
	std::shared_ptr<CManagerEquip>		m_spRefPmpNameEquipManager;

	std::shared_ptr<CManagerEmergency>	m_spEmergencyManager;

	//std::shared_ptr<CManagerPattern>	m_spOnlyPattern; /// Excel의 비고 또는 사용자가 직접추가 하는 패턴
	
	std::shared_ptr<CManagerPattern>	m_spUserAddPattern; ///프로젝트 생성 시 조건에 의해 만들어진 패턴 목록
															/// + m_spOnlyPattern
	
	std::shared_ptr<CManagerPS>			m_spPresureSwitch;
	std::shared_ptr<CManagerPump>		m_spPump;

	std::shared_ptr<CManagerAutoLogic>	m_spAutoLogicManager;

	std::shared_ptr<CManagerFacpRelay>	m_spContactFacp;

	std::map<CString, CDataUnit*>		m_MapUnitNum; //< KEY : facpnum.unitNum , Data : Reference
	std::map<int, CDataFacp*>			m_MapFacpNum; //< KEY : FcapNum  , Data : Reference

	CString		m_strCurrentUser;

	BOOL		m_bChangeFlag;//< 프로젝트 변경 Flag
	BOOL		m_bOpened; //< Project Opened 

	BOOL	m_bIsComparedData;	//20240326 GBM - 메모리 누수 오류 수정, 비교 데이터 여부 Flag
	BOOL	m_bNewEquipmentTypeAdded;	//20240422 GBM - 설비 정의가 추가되었을 때만 중계기 일람표 저장하기 위한 flag

protected:
	//CMapLocation							m_MapLocation;
	std::shared_ptr<CManagerLocation>	m_spLocation;
public:
	/*!
	 * @fn         Temp_CvtChangeRelayID
	 * @version    1.0
	 * @date       2021-1-11 12:51:01
	 * @param      
	 * @brief      수신기ID,유닛ID,채널ID,회로ID를 임의의 번호로 생성했는데 관리하기 불편해서 
					수신기ID== 수신기번호, 유닛ID는 유닛번호,회로ID는 회로번호로 수정
					이 함수는 기존에 임의로 생성된 ID를 수신기ID== 수신기번호 등으로 수정하는 함수로
					기존에 버전의 프로젝트에서 사용된다.
	 * @return     return
	 * @note 
	 */
	//int Temp_CvtChangeRelayID();


	int CreatePrjTable(); // 사용안함


	void SetChangeFlag(BOOL bChange) { m_bChangeFlag = bChange; }
	BOOL GetChangeFlag() { return m_bChangeFlag; }
	void SetPrjVersion(WORD wMajor, WORD wMinor) { m_dwPrjVersion = MAKELPARAM(wMinor, wMajor); m_wPrjMinorNum = wMinor; m_wPrjMajorNum = wMajor; }
	int ProcessDeviceTableList(CWnd *pPrgTagetWnd = NULL);
	int ProcessDeviceTable(CString strPath,int &nRelayIndex , int nAllCnt , int nAllStep, BOOL bEIInit, CWnd *pPrgTagetWnd = NULL);
	void VersionUp(BOOL bMajor = FALSE) { if (bMajor) { m_wPrjMajorNum++; m_wPrjMinorNum = 0; } else m_wPrjMinorNum++; }
	//////////////////////////////////////////////////////////////////////////
	// Add Data : 
	void AddUnitMap(CString strKey , CDataUnit *pUnit) { m_MapUnitNum[strKey] = pUnit; };
	void AddSysMap(CString strKey, CString strIDKey, CDataSystem * pSys);
	void RemoveSysMap(CString strKey, CString strIDKey);
	void RemoveSysIDMap(CString strIDKey);
	void RemoveSysMap(CDataSystem * pSys);
	void RemoveFacpMap(int nFacpNum) {m_MapFacpNum.erase(nFacpNum);}
	void RemoveUnitMap(CString strKey) { m_MapUnitNum.erase(strKey); }
	void AddFacpMapData(int nNum, CDataFacp*pFacp) { m_MapFacpNum[nNum] = pFacp; }
	void AddSystemMapData(CString strKey, CDataSystem * pData) { m_MapSystemData[strKey] = pData; }
	void AddDeviceTable(CString strPath);
	CDataLocBase * AddNewLocation(CString strBuild, CString strBtype
		, CString strStair, CString strFloor, CString strRoom
	);
	CDataLocBase * AddLocation_Old(CString strBuild,CString strBtype
		,CString strStair,CString strFloor,CString strRoom
	);
	
	CDataEquip * AddNewEquip(CString strEquipName, int nType);
	CDataSystem * AddSystemDeviceDataByNum(int nFacpNum, int nUnitNum, int nChannelNum, int nRelayNum
		, CDataLocBase	* pInputLoc,CDataLocBase	* pOutputLoc
		, CDataEquip * pEqInput, CDataEquip * pEqOut, CDataEquip * pEqOutContents
		, CDataPattern * pPattern
		, CDataEquip * pEqName, CString strAddEqIdx
		, int nRepeator
		, int nScrID
		, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
		, int nRelayIndex
		// 속도 개선 때문에 추가 [1/17/2022 KHS]
		,int nFid = -1 ,int nUid = -1,int nCid = -1 ,int nRid = -1
	);

	CDataSystem * AddSystemDeviceDataByID(
		int nFacpID, int nUnitID, int nChannelID, int nRelayID, int nRelayNum
		, int nInBid, int nInBtype, int nInSid, int nInFlid, int nInRmid
		, int nOutBid, int nOutBtype, int nOutSid, int nOutFlid, int nOutRmid
		, int nInId, int nOutId, int nContId
		, int nEqNameID, CString strAddEqIdx
		, int nRepeator
		, int nScrId
		, CString strKey
		, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
		, int nRelayIndex
	);

	CDataSystem * AddSystemDeviceDataByID2(
		int nFacpID, int nUnitID, int nChannelID, int nRelayID, int nRelayNum
		, int nLocType , CString strBuild , CString strBtype , CString strStair,CString strFloor, CString strRoom
		, int nOutBid, int nOutBtype, int nOutSid, int nOutFlid, int nOutRmid
		, int nInId, int nOutId, int nContId
		, int nEqNameID, CString strAddEqIdx
		, int nRepeator
		, int nScrId
		, CString strKey
		, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
		, int nRelayIndex
	);

	CDataSystem * AddSystemDeviceDataByROM(
		int nFacpNum, int nUnitNum, int nChannelNum, int nRelayNum
		, CDataEquip * pInType, CDataEquip * pOutType, CDataEquip * pEqName, CDataEquip * pCont
		, CDataLocBase * pInLoc, CDataLocBase * pOutLoc
		, CString strTempName
		, CString strEquip, CString strAddIdx 
		, int nRelayIndex
	);


	CDataSystem * AddSystemFacpDataByNum(int nFacpNum, int nFacpID = -1, int nFacpType = -1
		, CString strFacpName = L""
	);

	CDataSystem * AddSystemUnitDataByNum(int nFacpNum, int nUnitNum , int nUnitID =-1
		,int nUnitType=-1 , CString strUnitName =L""
	);
	CDataSystem * AddSystemUnitDataByID(int nFacpID, int nUnitNum, int nUnitID
		, int nUnitType, CString strUnitName
	);
	CDataSystem * AddSystemChannelDataByNum(int nFacpNum, int nUnitNum, int nChnNum
		, int nChnID = -1, CString strChnName = L""
	);
	CDataSystem * AddSystemChannelDataByID(int nFacpID, int nUnitID, int nChnNum
		, int nChnID, CString strChnName
	);


	int FindEquipID(CString strEqName, int nType);
	CDataEquip *  FindEquipData(CString strEqName, int nType);
	CDataPattern *  FindPatternData(CString strPattern);
	
	int InitFasSysData(std::shared_ptr<CManagerEquip>		spRefInEquipManager
		, std::shared_ptr<CManagerEquip>		spRefEqNameManager
		, std::shared_ptr<CManagerEquip>		spRefOutEquipManager
		, std::shared_ptr<CManagerEquip>		spRefContEquipManager
		, std::shared_ptr<CManagerEquip>		spRefPmpEquipManager
		, std::shared_ptr<CManagerEquip>		spRefPSEquipManager
		, std::shared_ptr<CManagerEquip>		spRefPSPmpNameManager
	);

	int CopyAllEquipContainer(
		std::shared_ptr<CManagerEquip>		spRefInEquipManager
		, std::shared_ptr<CManagerEquip>		spRefEqNameManager
		, std::shared_ptr<CManagerEquip>		spRefOutEquipManager
		, std::shared_ptr<CManagerEquip>		spRefContEquipManager
		, std::shared_ptr<CManagerEquip>		spRefPmpEquipManager
		, std::shared_ptr<CManagerEquip>		spRefPSEquipManager
		, std::shared_ptr<CManagerEquip>		spRefPSPmpNameManager
	);

	int CopyEquipContainer(
		int nEqType , std::shared_ptr<CManagerEquip> spEquipManager
	);
	int CvtPathNameToFacpNum(CString strPath);
	int FillDeviceTreeList(CTreeListCtrl *pCtrl);
	int FillLinkedTreeList(CTreeListCtrl *pCtrl);
	int FillLocationTree(CTreeCtrl* pCtrl);
	int FillLocationTree2(CTreeCtrl* pCtrl,int nLocType); // 계단까지
	int FillDeviceList(CListCtrl* pList, short sFacp = -1, short sUnit = -1, short sChn = -1);
	int FillDeviceList(CListCtrl* pList,CDataLocBase *pLoc);
	int ProcessingPattern(
		CWnd *	pPrgWnd
		, bool bBuild
		, bool bBType, bool bBTypeBuild
		, bool bStair, bool bStairBuild, bool bStairBType
		, bool bFloor, bool bFloorBuild, bool bFloorBType, bool bFloorStair
		, bool bRoom, bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor
		, bool bEquip, bool bEquipBuild, bool bEquipBType, bool bEquipStair, bool bEquipFloor
	);
	int FillConditionTree(CTreeCtrl* pCtrl, CPtrList * pItemList , BOOL bInput);


	int MakePatternBuild();
	int MakePatternBType(bool bBTypeBuild);
	int MakePatternStair(bool bStairBuild, bool bStairBType);
	int MakePatternFloor(bool bFloorBuild, bool bFloorBType, bool bFloorStair);
	int MakePatternRoom (bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor);
	int MakePatternEquip(bool bEquipBuild, bool bEquipBType, bool bEquipStair, bool bEquipFloor);
	int AddDeviceToLocPattern();
// 	int AddDeviceToLocPattern(bool bBuild
// 		, bool bBType, bool bBTypeBuild
// 		, bool bStair, bool bStairBuild, bool bStairBType
// 		, bool bFloor, bool bFloorBuild, bool bFloorBType, bool bFloorStair
// 		, bool bRoom, bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor
// 		, bool bEquip, bool bEquipBuild, bool bEquipBType, bool bEquipStair, bool bEquipFloor);
// 	
// 	
	int AddEquipToEquipPattern(CDataPattern * pPtn);
	int FillPatternList (CListCtrl * pList);
	int ProcessingEmergency(
		bool bBuild
		, bool bBType, bool bBTypeBuild
		, bool bStair, bool bStairBuild, bool bStairBType
		, bool bFloor, bool bFloorBuild, bool bFloorBType, bool bFloorStair
		, bool bRoom, bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor
	);
	int MakeEmergencyBuild();
	int MakeEmergencyBType(bool bBTypeBuild);
	int MakeEmergencyStair(bool bStairBuild, bool bStairBType);
	int MakeEmergencyFloor(bool bFloorBuild, bool bFloorBType, bool bFloorStair);
	int MakeEmergencyRoom(bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor);
	int FillEmergencyList(CListCtrl* pList);
	int SplitLocation(CString strKey, CString &strBuild, CString &strBType
		, CString &strStair, CString &strFloor, CString &strRoom, int &nType);

	/*!
	 * @fn         SetProjectInfo
	 * @version    1.0
	 * @date       2020-1-20 11:20:05
	 * @param      
				CString strPrjName : Project Name
				CString strSiteName : 현장 이름
				COleDateTime dtCreate : 프로젝트 생성 날짜
				CString strMaker : 프로젝트 생성자
				CString strSitePhone: 현장 연락처
				CString strDBName: 데이터베이스 이름

	 * @brief      
				프로젝트 정보 설정
	 * @return     return
	 * @note 
	 */
	void SetProjectInfo(CString strPrjName, CString strSiteName, COleDateTime dtCreate
		, CString strMaker, CString strSitePhone
		, CString strDBName
	);
	HTREEITEM InsertTreeRoot(CTreeCtrl* pCtrl, CPtrList * pItemList);

	static BOOL m_bEquipSortAsc;
	static int EquipSortCompare(const void * pItem1, const void * pItem2);
	void SortEquipName(std::shared_ptr<CManagerEquip> spEquip);
	int FillDeviceTree(CTreeCtrl* pCtrl, int nViewType , CPtrList * pItemList , BOOL bInputName);
	int FillDeviceTreeByLocation(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName);
	int FillDeviceTreeByAddress(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName);
	int FillDeviceTreeByInputType(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName);
	int FillDeviceTreeByOutputType(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName);
	int FillDeviceTreeByEquipName(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName);
	int FillDeviceTreeContentsName(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName);
	int FillDeviceTreeByCustom(CTreeCtrl* pCtrl , CPtrList * pItemList , TVORDER *pTo , BOOL bOutput);
	int OpenPrjDatabase(CString strDBSource, CString strDBName, DWORD dwPort
		, CString strUser, CString strPwd);
	int InsertPrjBaseData();
	int InsertPrjBaseEquipDB();
	int InsertPrjBaseRelayTable();
	int InsertPrjBaseLocationDB();
	int InsertPrjBasePatternDB();
	int InsertPrjBaseEmergencyDB();
	int InsertPrjBaseAutoLogicDB();
	int InsertPrjBaseFacpContactDB();
	int InsertPrjBasePumpDB();
	int InsertPrjBasePSDB();

	int FillPatternTree(CTreeCtrl * pCtrl , CPtrList * pItemList);

	////////////////////////////////////////////////////////////////////////////////////////////
	// Converter Function
	int CvtFacpNumToID(short nNum);
	short CvtFacpIDToNum(int nID);
	int CvtUnitNumToID(short nFacpNum, short nUnitNum);
	short CvtUnitIDToNum(int nFacpID, int nUnitID);
	short CvtChannelIDToNum(int nFacpID, int nUnitID, int nChnID);
	int CvtChannelNumToID(short nFacpNum, short nUnitNum, short nChnNum);
	short CvtRelayIDToNum(int nFacpID, int nUnitID, int nChnID, int nRelayID);
	int CvtRelayNumToID(short nFacpNum, short nUnitNum, short nChnNum, short nRelayNum);

	//////////////////////////////////////////////////////////////////////////
	// Location Convert 
	CString CvtBuildIDToName(short nBid);
	CString CvtBTypeToName(short nBid, short nBtype);
	CString CvtStairIDToName(short nBid, short nBtype, short nSid);
	CString CvtFloorIDToName(short nBid, short nBtype, short nSid, short nFid);
	CString CvtRoomIDToName(short nBid , short nBtype , short nSid , short nFid , short nRid);
	/////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// Get
	/*!
	 * @fn         GetProjectFullPath
	 * @version    1.0
	 * @date       2020-1-20 11:30:03
	 * @param      BOOL bEditPrjPath : 현재 편집중인 프로젝트이 FullPath를 가져올려면 이값은 TRUE
	 * @brief      현재 프로젝트의 Version Folder 까지의 Path
	 * @return     return
	 * @note 
	 */
	CString GetProjectVersionPath(BOOL bEditPrjPath =TRUE, WORD wMajor=0, WORD wMinor=0);
	CDataFacp * GetFacpByID(int nFacpID);
	CDataFacp * GetFacpByNum(int nFacpNum);
	CDataUnit * GetUnitByID(int nFacpID , int nUnitID);
	CDataUnit * GetUnitByNum(int nFacpNum, int nUnitNum);
	CDataChannel * GetChannelByID(int nFacpID, int nUnitID, int nChnID);
	CDataChannel * GetChannelByNum(int nFacpNum, int nUnitNum, int nChnNum);
	CDataChannel * GetChannel(CString strKey);
	CDataDevice* GetDevice(CString strKey);
	CDataDevice* GetDeviceByIDKey(CString strIDKey);
	CDataDevice* GetDeviceByID(int nFacpID, int nUnitID, int nChnID, int nRelayID);
	CDataSystem * GetIDSystemItem(int nType, int nFacpID, int nUnitID=-1
		, int nChnID = -1, int nRelayID = -1);
	CDataSystem * GetSystemItem(CString strKey);
	CDataPattern* GetPattern(int nPtnID);
	CDataEmBc * GetEmergency(int nID);
	CDataEmBc * GetEmergency(CString strBuild , CString strStair , CString strFloor);
	CDataPump * GetPump(int nFacp, int nID);
	CDataFacpRelay * GetContactFacp(int nFacp, int nID);
	CDataPS * GetPSwitch(int nFacp, int nID);
	CDataLocBase * GetLocation(int nBid, int nBtype = -1, int nSid = -1, int nFlid = -1, int nRmid = -1) { return m_spLocation->GetLocation(nBid,nBtype,nSid,nFlid,nRmid); }
	std::map<int, CDataFacp*> * GetFacpMap() { return	&m_MapFacpNum; }
	std::map<CString, CDataUnit*> *GetUnitMap() { return	&m_MapUnitNum; }
	std::shared_ptr<CManagerPattern> GetPatternManager() { return m_spUserAddPattern; }
	std::shared_ptr<CManagerPump> GetPumpManager() { return m_spPump; }
	std::shared_ptr<CManagerPS> GetPSwitchManager() { return m_spPresureSwitch; }
	std::shared_ptr<CManagerFacpRelay> GetFacpContactManager() { return m_spContactFacp; }
	std::shared_ptr <CManagerEquip> GetEquipManager(int nType)
	{
		switch (nType)
		{
		case ET_INPUTTYPE:
			return m_spRefInputEquipManager;
		case ET_EQNAME:
			return m_spRefEqNameManager;
		case ET_OUTCONTENTS:
			return m_spRefContentsEquipManager;
		case ET_OUTPUTTYPE:
			return m_spRefOutputEquipManager;
		case ET_PUMPTYPE:
			return m_spRefPumpEquipManager;
		case ET_PSTYPE:
			return m_spRefPSEquipManager;
		case ET_PMPNAME:
			return m_spRefPmpNameEquipManager;
			break;
		}
		return nullptr;
	}

	int GetUnitCountInFacp(int nFacpNum);
	CDataEquip * GetEquipData(int nType, int nID);
	std::shared_ptr<CManagerLocation> GetLocationManager() { return m_spLocation; }
	//CMapLocation * GetLocationMap() { return &m_MapLocation; }
	CMapSystemData * GetSystemData() { return &m_MapSystemData; }
	CMapIDSystemData * GetIDSystemData() { return &m_MapIDSystemData; }
	std::shared_ptr<CManagerAutoLogic> GetAutoLogicManager() { return m_spAutoLogicManager; }
	std::shared_ptr<CManagerEmergency> GetEmergencyManager() { return m_spEmergencyManager; }
	int GetWholeDeviceID(int nFacpNum, int nUnitNum, int nChannelNum , int nDevNum);
	int GetWholeUnitID(int nFacpNum, int nUnitNum);
	int GetWholeFacpID(int nFacpNum);
	int GetWholeChnID(int nFacpNum, int nUnitNum, int nChnNum);
	int GetNewRelayIndex();
	int GetWholeRelayIndex();
	void IncreaseRelayIndex();

	void SetIDSystem(CString strKey , CDataSystem *pSys);
	void SetFacpMap(int nNum, CDataFacp * pFacp) { m_MapFacpNum[nNum] = pFacp; }
	void SetUnitMap(CString strKey, CDataUnit * pUnit) { m_MapUnitNum[strKey] = pUnit; }
	int FillSelectCondTreeList(CTreeListCtrl* pCtrl, CPtrList * pPtrCondList, CPtrList * pSelectedItems);
	CMapSystemData MakeSelectData(CPtrList* pPtrCondList);
	int InsertLinkedTable(CDataDevice * pDev, CDataLinked * pLink);
	int ChangeLinkedTable(CDataDevice * pDev, CDataLinked * pLink);
	
	int DeleteLinkInfo(CDataDevice * pDev,CDataLinked * pLink);
	CDataPattern * AddNewPattern(int nPtnId, CString strPtnName, int nPtype,int nManualMake, CPtrList * pDevList);
	CDataPattern * ChangePattern(CDataPattern * pPtn,int nManualMakeStatus, CPtrList * pPtnItemList);
	CDataPattern * ChangePattern(CDataPattern * pPtn, int nManualMakeStatus, std::vector<CDataLinked*>& dataLinkedVec);		//20240604 GBM - 스레드 전환을 위한 매서드 오버로드
	BOOL DeletePattern(CDataPattern * pPtn);
	BOOL DeletePatternInMemory(CDataPattern * pPtn);
	BOOL DeleteDeviceInMemory(CDataDevice * pDev);

	BOOL DeleteSystemInMemory(CDataSystem * pSys);
	//////////////////////////////////////////////////////////////////////////
	// Database Load
	// EQUIP --> LOCATION --> PATTERN --> FACP --> UNIT-->CHANNEL --> RELAY
	// --> LINK --> 
	int CheckColumn(CString strTable, CString strColumn, BOOL bCreate = FALSE, CString strType = L"");
	int CheckAutoLogicColumn(CString strTable, CString strColumn, BOOL bCreate = FALSE, CString strType = L"");
	int CheckAddColumn(CString strTable, CString strColumn, BOOL bCreate = FALSE, CString strType = L"");
	int TempFunc_CheckAutoLogicTable();
	int TempFunc_CheckTempSaveTable();
	int TempFunc_CheckTempUsedPtnTable();
	int TempFunc_CheckFacpContactTable();
	int TempFunc_CheckStoredProcedure(CString strName, int nVersion);
	int TempFunc_MakeStoredProcedure(CString strDefine, int nVersion, BOOL bCreate);

	// Index 추가 - 기존에 프로젝트는 인덱스가 들어가 있지 않다 
	int TempFunc_CheckIndex();
	int TempFunc_DropIndex();

	int LoadProjectDatabase();
	int LoadEquip();
	int LoadFacp();
	int LoadUnit();
	int LoadChannel();
	int LoadRelay();
	/*!
	 * @fn         LoadRelayAndLink
	 * @version    1.0
	 * @date       2020-10-16 20:00:16
	 * @param      
	 * @brief      전체 회로와 연동정보를 SQL문으로 한번에 가져온다.
					따로따로 가져올때 보다 속도 향상을 위해
	 * @return     return
	 * @note 
	 */
	int LoadRelayAndLink();
	int LoadLocation();
	int LoadLocBuild();
	int LoadLocBtype();
	int LoadLocStair();
	int LoadLocFloor();
	int LoadLocRoom();
	int LoadPatternMst();
	int LoadPatternItem();
	int LoadEmBroadcast();
	int LoadPump();
	int LoadPresureSwitch();

	int LoadLinkRelay();
	int LoadLinkPump();
	int LoadLinkPSwitch();
	int LoadSwtich();


	int LoadAccess();
	int LoadFunction();
	int LoadGroup();
	int LoadAutMakeLogic();
	int LoadFacpContact();

	int ReduceDatabase();

	// [KHS 2020-3-6 09:08:22] 
	// From Rom
	int LoadFromRom(short nFNum, int &nRelayIndex, CPtrList * pPtrList, BYTE * pMain, BYTE * pLcd, BYTE *pEmer);
	UINT LoadEquipFromRom(CPtrList * ptrEqList, BYTE * pLcd);
	UINT LoadRelayFromRom(int nFacpNum, int &nRelayIndex, CPtrList * ptrEqList, BYTE * pMain, BYTE * pLcd);
	UINT Load63Unit0ChnFromRom(int nFacpNum, CPtrList * ptrEqList, BYTE * pMain, BYTE * pLcd);
	UINT Load63Unit1ChnFromRom(int nFacpNum, CPtrList * ptrEqList, BYTE * pMain, BYTE * pLcd);
	UINT LoadPatternFromRom(int nFacpNum, CPtrList * ptrEqList,BYTE * pMain);
	UINT LoadEmergencyFromRom(int nFacpNum, BYTE * pEmer);
	int FindEquipWithRomData(int nType,CString strMsg);
	int AddEquipWithRomData(int nType, int nID, CString strMsg, CString strSymbol);
	int GetWholeEquipID(int nType);
	int GetWholeEmergencyID();
	int GetWholeAutoLogicID();
	int GetWholeFacpContactID(int nFacp);
	int CvtEquipRomIDToDatabeseID(int nType, CPtrList * ptrEqList, BYTE btType);
	BOOL CvtStringToLocation(CString strName, CStringArray *pStrArr);
	int SetDataFromRomInDevice(int nFacpNum);

	// [2023/9/6 16:52:56 KHS] 
	// 수동 출력 저장하기
	int MakeManualOutput(CString strPath);


	// [KHS 2020-12-9 18:20:05] 
	// 역변환 정보파일 사용  - 생성
	int MakeRvEquipInfo(CString strPath);
	int MakeRvLocationInfo(CString strPath);
	int MakeRvEmergencyInfo(CString strPath);
	int MakeRvPatternInfo(CString strPath);
	int MakeRvContactInfo(CString strPath);

	// [KHS 2021-2-4 13:25:29] 
	// 역변환정보 불러오기
	int LoadFromRomUseReverseInfo(CString strPath);
	int LoadFromRvEqInfo(CString strRomPath);
	int LoadFromRvEmergencyInfo(CString strRomPath);
	int LoadFromRvPatternInfo(CString strRomPath);
	int LoadFromRvContactInfo(CString strRomPath);
	int LoadFromRvRelayInfo(CString strRomPath);
	int LoadFromRvLocInfo(CString strRomPath);

	//UINT LoadFromRvMainRom(short nFNum, BYTE * pMain, BYTE * pLcd, BYTE *pEmer);
	int LoadFromRomforReverseFile(short nFNum, BYTE * pMain, BYTE * pLcd, BYTE *pEmer);
	UINT LoadFromRvRelayRom(int nFacpNum, BYTE * pMain, BYTE * pLcd);
	UINT LoadFromRvPatternRom(int nFacpNum, BYTE * pMain, BYTE * pLcd);
	UINT LoadFromRv63Unit0ChnRom(int nFacpNum, BYTE * pMain, BYTE * pLcd);
	UINT LoadFromRv63Unit1ChnRom(int nFacpNum, BYTE * pMain, BYTE * pLcd);

	int LoadRomEquip();
	int LoadRomFacp();
	int LoadRomUnit();
	int LoadRomChannel();
	int LoadRomRelay();
	int LoadRomLocation();
	int LoadRomLocBuild();
	int LoadRomLocBtype();
	int LoadRomLocStair();
	int LoadRomLocFloor();
	int LoadRomLocRoom();
	int LoadRomPatternMst();
	int LoadRomPatternItem();
	int LoadRomEmBroadcast();
	int LoadRomPump();
	int LoadRomPresureSwitch();

	int LoadRomLinkRelay();
	int LoadRomLinkPump();
	int LoadRomLinkPSwitch();
	int LoadRomSwtich();


	int RemoveAllData();
	int RemoveSystemData();
	int RemoveLocation();
	int RemovePattern();
	int RemoveAutoLogic();
	int RemovePump();
	int RemovePs();
	int RemoveEmergency();
	int RemoveEquip(int nType = ET_ALL);
	int RemoveContactFacp();

	int InsertLinkedToTreeList(CTreeListCtrl * pCtrl, HTREEITEM hParent,CDataDevice * pDevice);
	int ParsingBasicData(int nFacpID, short sFNum, CExcelWrapper * pxls);
	int ParsingEmergencyData(CExcelWrapper * pxls, std::shared_ptr<CManagerEmergency> spEmergencyManager);
	int ParsingAutoLogicData(CExcelWrapper * pxls, std::shared_ptr<CManagerAutoLogic> spAutoLogic);
	int ParsingFacpContactData(CExcelWrapper * pxls, std::shared_ptr<CManagerFacpRelay> spFacpContact);
	int AddDeviceToLocationTree(CTreeCtrl * pCtrl,HTREEITEM hParent,CDataLocBase* pLoc,CPtrList * ptrItemList,BOOL bInputName);
	int AddDeviceToLocationTree_V2(CTreeCtrl * pCtrl,CPtrList * ptrItemList,BOOL bInputName);

	int MakeConstructorTable(CString strPath);

	/*!
	 * @fn         MakeLinkData
	 * @version    1.0
	 * @date       2020-3-03 15:17:00
	 * @param      CString strPath : 절대 경로 ex) C:\SLP3\PROJECT\프로젝트명\RELEASE\
	 * @brief      
	 * @return     return
	 * @note 
	 */
	int MakeLinkData(CString strPath);
	int MakeX2RMainRom(CString strPath, ST_MAINROM * pMainRom, BYTE *pRomBuff, BYTE * pMsgBuff
		, UINT &uRomOffset, UINT &uMsgOffset);
	UINT AddPatternPointerAddrX2MainRom(
		  int nFNum, CPtrList * pList
		, BYTE *pBuff, BYTE * pMsgBuff
		, UINT &uRomOffset, UINT &uMsgOffset);
	UINT AddPointerAddrX2MainRom(
		  int nFacpNum,int nUnit,int nChn , int nRelay
		, int nInput, int nOutput, CPtrList * pList, CString strName
		, BYTE *pBuff, BYTE * pMsgBuff
		, UINT &uRomOffset, UINT &uMsgOffset); 

	/*!
	 * @fn         AddPumpDataX2MainRom 
	 * @version    1.0
	 * @date       2020-3-03 10:27:20
	 * @param      
	 * @brief      매뉴얼 상에서는 63유닛 0채널인데 델파이 소스상에서는 63유닛 1채널이다
					0 채널에는 PS값이 들어간다.
	 * @return     return
	 * @note 
	 */
	int Add63Unit0ChnX2MainRom(int nFNum,CFile *pFnCrt, CFile *pFnRom, ST_MAINROM * pMainRom, BYTE *pRomBuff, BYTE * pMsgBuff
		, UINT &uRomOffset, UINT &uMsgOffset);
	/*!
	 * @fn         AddRelayAddrX2MainRom
	 * @version    1.0
	 * @date       2020-3-03 10:28:27
	 * @param      
	 * @brief      매뉴얼 상에서는 63유닛 1채널에 들어가는 값인데 
					델파이 소스상에서는 펌프데이터가 들어간다.
	 * @return     return
	 * @note 
	 */
	int Add63Unit1ChnX2MainRom(int nFNum, CFile *pFnCrt,CFile *pFnRom, ST_MAINROM * pMainRom, BYTE *pRomBuff, BYTE * pMsgBuff
		, UINT &uRomOffset, UINT &uMsgOffset);
	/*!
	 * @fn         AddPatternAddrX2MainRom
	 * @version    1.0
	 * @date       2020-3-03 10:30:25
	 * @param      
	 * @brief      매뉴얼 상에서는 63유닛 2채널에 들어가는 값인데
	 * @return     return
	 * @note 
	 */
	UINT AddPatternAddrX2MainRom(int nFNum, CFile *pFnCrt, CFile *pFnRom, ST_MAINROM * pMainRom, BYTE *pRomBuff, BYTE * pMsgBuff
		, UINT &uRomOffset, UINT &uMsgOffset);
	UINT AddEtcDataX2LcdRom(int nFNum, BYTE * pMsgBuff, UINT &uMsgOffset);
	UINT AddEmergencyRom(BYTE * pEmBuff, UINT &uEmOffset);

	int WriteRomFile(CString strPath, BYTE *pBuffer, UINT uSize, BOOL bCreate, UINT uStartOffset = 0 );
	int MakeLcdRom();
	int MakeEmerRom();
	int FindRomFileList(CString strName, CPtrList * pPtrRomList);
	int CreateFromRom(CString strRomPath, CPtrList *pPtrRomList, BOOL bUseRvFile);
	int LoadFile(CString strFile, BYTE* pBuff);
	int ParsingEquipInfo(CExcelWrapper * pxls);
	int ParsingTableSheet(CExcelWrapper * pxls);
	int ChangeDevName(int nDataType, int nId);
	//void SendProgressStep(CWnd *pTargetWnd, int nProg, int nAllCnt, int nAllStep, int nDetailAll, int nDetailStep);
	void SendProgStep(CWnd *pTargetWnd, int nProgType, int nDetailAll, int nDetailStep);
	int DeleteLinkFromAllInput(CDataLinked * pLink);
	int DeleteLinkFromAllInput(int nLinkType , int nFID , int nUID , int nCID,int nRID);


//////////////////////////////////////////////////////////////////////////
	// [2023/7/24 13:13:46 KHS] 
	// 오류 검출
	// 1. 글자 수 초과 --> 오류 , 
	// 2. 패턴 내 출력 개수 --> 오류 , 
	// 3. 층 번호 중복 --> 오류
	// 4. 연동 출력 개수 --> 오류
	// 5. 사용되지 않은 출력 --> 경고
	// 6. 출력 없는 입력 --> 경고

	// 1. 글자 수 초과
	int Check_NameSize();

	// 2. 패턴 내 출력 개수
	int Check_OutputCountInPattern();

	// 3. 층 번호 중복
	int Check_DuplicationLevelNum();

	// 4. 연동 출력 개수
	int Check_OutputCountInInput();

	// 5. 사용되지 않은 출력
	int Check_UnusedOutputCount();

	// 6. 출력 없는 입력
	int Check_InputWithNoOutput();


//////////////////////////////////////////////////////////////////////////

	//20240808 GBM start - 현재 수신기 / 유닛 타입 정보 얻음
	void GetFacpAndUnitType();
	//20240808 GBM end
};


