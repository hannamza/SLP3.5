#pragma once
/*!
 * @class      CDataAutoMake
 * @author     KHS
 * @version    1.0
 * @date       2020-2-24 13:29:45
 * @brief      연동데이터 자동생성을 위해 자동으로 생성되는 연동데이터를 저장한다.
 * @note 
 
 * Contact  hskim0602@gmail.com
 */
class CDataSystem;
//class CDataAutoPattern; // old - device사용
class CDataDevice;
class CDataPattern;
class CDataEmBc;
class CDataLinked;
class CDataNewAutoPtn; // new - device대신에 link  사용
class CRelayTableData;
class CDataAutoMake
{
public:
	CDataAutoMake();
	~CDataAutoMake();
	SIMPLE_FUNC_IMPL(CString, IDKey, m_strIDKey);
	SIMPLE_FUNC_IMPL(CDataSystem*, SysData, m_pSysData);
	//SIMPLE_FUNC_IMPL(CPtrList *, PatternList, &m_ptrAutoPattern);
	
	CPtrList * GetRefPatternList() { return &m_ptrAutoPattern; }
	CPtrList * GetRefEtcLinkList() { return &m_ptrEtcLink; }
	int	GetSysDataType();
protected:
	CDataSystem				*	m_pSysData; //< Device 또는 출력내용
											//< CDataDevice로 해도 되지만 Tree에 데이터 구성할 때 ItemData에 이 클래스의 레퍼런스를 설정하기 위해
											//< CDataDevice의 Link List에 Relay 들어간다.
	CString						m_strIDKey;
	CPtrList					m_ptrAutoPattern; //<CDataAutoPattern의 리스트
												//< 해당 어드레스가 있는지 확인
	CPtrList					m_ptrEtcLink; ///< 비상방송 , 펌프,압력스위치 등의 연동데이터

public:
	void InitAutoData(CDataSystem * pSys, BOOL bInit = TRUE);
	int CopySysData(CDataSystem *pSrc, CDataSystem * pTraget, int nType);
	int CopyDevice(CDataSystem *pSrc, CDataSystem * pTraget);
	/*!
	 * @fn         AddTempPattern
	 * @version    1.0
	 * @date       2020-6-24 08:26:43
	 * @param      
	 * @brief      
				
	 * @return     return
	 * @note 
	 */
	int AddTempPattern(CRelayTableData * pRefTable , CDataPattern * pPtn, CDataDevice * pTargetDev);
	int AddTempPattern(CRelayTableData * pRefTable, CDataPattern * pPtn
		, int nFacp, int nUnit, int nChn, int nDev
		);
	int AddEmergencyLink(CDataEmBc * pEm, CDataDevice * pSrc, int nLogicID);
	CDataNewAutoPtn * FindPattern(CDataPattern * pPtn);
	CDataLinked * FindLinked(BYTE btLinkType 
		, int nFacp , int nUnit , int nChn , int nDev
		, CDataDevice * pSrc);
};

