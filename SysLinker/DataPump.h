#pragma once

// CDataPump 명령 대상입니다.
class CDataLinked;
class CDataDevice;
class CDataPump : public CObject
{
public:
	CDataPump();
	virtual ~CDataPump();
	SIMPLE_FUNC_IMPL(CString, PumpName, m_strName);
	SIMPLE_FUNC_IMPL(CString, PumpLcd, m_strLcd);
	SIMPLE_FUNC_IMPL(int, FacpID, m_nFacpID);
	SIMPLE_FUNC_IMPL(int, PumpID, m_nPumpID);
	SIMPLE_FUNC_IMPL(int, PumpType, m_nPType);
	SIMPLE_FUNC_IMPL(int, Pcb, m_nPcb);
	CPtrList * GetLinkList() { return &m_ptrLinkList; }
protected:
	int m_nFacpID;
	int m_nPumpID;
	int m_nPType;
	int m_nPcb;
	CString m_strName;
	CString m_strLcd;
	CPtrList	m_ptrLinkList;

public:
	void CopyData(CDataPump * pSrc);
	int SetPumpData(int nFacpID, int nPump, int nPType, CString strName, CString strLcd, int nPcb = -1);
	void AddLink(CDataLinked * pLink);
	void AddLink(CDataDevice * pDev, byte btLinkType, byte btLogicType, int nLogicID);
	POSITION FindLinkByID(int nLnkType, int nFacpID, int nUnitID, int nChnID, int nRelayID, BOOL bCheckLnkType=1);
	POSITION FindLink(CDataLinked* pLink);
	void DeleteLink(int nPType, int nFacp, int nUnit, int nChn, int nDev);
	void DeleteLink(CDataLinked* pLink);
	void RemoveAllLink();
	BOOL IsEqual(CDataPump * pSrc);
};


