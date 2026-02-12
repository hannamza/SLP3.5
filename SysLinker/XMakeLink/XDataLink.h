#pragma once
class CXDataDev;
class CXDataEm;
class CXDataLink
{
public:
	CXDataLink();
	~CXDataLink();
	SIMPLE_FUNC_IMPL(int,LogicID,m_nLogicID);

	//CXDataDev	*	m_pRefDev;
	int				m_nLinkType;  //LK_TYPE_EMERGENCY,LK_TYPE_PATTERN,LK_TYPE_RELEAY
	LPVOID			m_lpRefData;
	int				m_nLogicID;

	void SetDevLinkData(int nLogicID,CXDataDev * pDev);
	void SetEmLinkData(int nLogicID,CXDataEm * pEm);
	void CopyData(CXDataLink * pSrc);

	int GetFacpID();
	int GetUnitID();
	int GetChnID();
	int GetDeviceID();
	int GetEmID();
};

