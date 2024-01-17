#pragma once

// CDataPS ��� ����Դϴ�.

class CDataLinked;
class CDataDevice;
class CDataPattern;
class CDataEmBc;
class CDataPump;
class CDataPS : public CObject
{
public:
	CDataPS();
	virtual ~CDataPS();
	SIMPLE_FUNC_IMPL(CString, PSwitchName, m_strName);
	SIMPLE_FUNC_IMPL(CString, PSwitchLcd, m_strLcd);
	SIMPLE_FUNC_IMPL(int, FacpID, m_nFacpID);
	SIMPLE_FUNC_IMPL(int, PSwitchID, m_nPSwitchID);
	SIMPLE_FUNC_IMPL(int, PSwitchType, m_nPType);
	SIMPLE_FUNC_IMPL(int, Pcb, m_nPcb);
	CPtrList * GetLinkList() {return &m_ptrLinkList;}
protected:
	int m_nFacpID;
	int m_nPSwitchID;
	int m_nPType;
	int m_nPcb;
	CString m_strName;
	CString m_strLcd;
	CPtrList	m_ptrLinkList;
public:
	int SetPSwitchData(int nFacpID, int nPSwitch, int nPType, CString strName, CString strLcd, int nPcb);
	void AddLink(CDataLinked * pLink);
	void AddLink(CDataDevice * pDev, byte btLinkType, byte btLogicType, int nLogicID);
	POSITION FindLinkByID(int nLnkType, int nFacpID, int nUnitID, int nChnID, int nRelayID, BOOL bCheckLnkType= 1);
	POSITION FindLink(CDataLinked* pLink);
	void DeleteLink(CDataLinked* pLink);
	void DeleteLink(int nPType, int nFacp , int nUnit , int nChn , int nDev);
	void RemoveAllLink();
	void CopyData(CDataPS * pSrc);
	BOOL IsEqual(CDataPS * pSrc);
};


