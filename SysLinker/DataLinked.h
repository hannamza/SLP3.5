#pragma once

// CDataLinked ��� ����Դϴ�.
class CDataEmBc;
class CDataDevice;
class CDataPattern;
class CDataLinked : public CObject
{
public:
	CDataLinked();
	virtual ~CDataLinked();

	SIMPLE_FUNC_IMPL(int, SrcFacp, m_nSrcFacp);
	SIMPLE_FUNC_IMPL(int, SrcUnit, m_nSrcUnit);
	SIMPLE_FUNC_IMPL(int, SrcChn, m_nSrcChn);
	SIMPLE_FUNC_IMPL(int, SrcDev, m_nSrcDev);
	SIMPLE_FUNC_IMPL(int, TgtFacp, m_nTgtFacp);
	SIMPLE_FUNC_IMPL(int, TgtUnit, m_nTgtUnit);
	SIMPLE_FUNC_IMPL(int, TgtChn, m_nTgtChn);
	SIMPLE_FUNC_IMPL(int, TgtDev, m_nTgtDev);
	SIMPLE_FUNC_IMPL(byte, LinkType, m_btLinkType);
	SIMPLE_FUNC_IMPL(int, LogicID, m_nLogicID);
	SIMPLE_FUNC_IMPL(byte, LogicType, m_btLogicType);

	int m_nSrcFacp; //< ID
	int m_nSrcUnit;
	int m_nSrcChn;
	int m_nSrcDev;

	byte m_btLinkType; //<1 : ���� , 2:�߰��

	byte m_btLogicType; //< ���� Ÿ�� , ��ü�ڵ�, �����ڵ�, ���� (LOGIC_MANUAL,LOGIC_ALL_AUTO,LOGIC_ONE_AUTO
	//< ������ �� 1: �����߰��� ,2: ���� �ڵ� , 3: ��ü �ڵ�(�϶�ǥ ������ �����̸����� ������)
	int m_nLogicID; //< ���� ���̵� , �����϶��� 0

	int m_nTgtFacp; //< ���� ���̵� �Ǵ� ���ű� ���̵� 
	int m_nTgtUnit;
	int m_nTgtChn;
	int m_nTgtDev;


	void SetLinkData(int nTgtFacp, int nTgtUnit, int nTgtChn, int nTgtDev
		, byte btLinkType, byte btLogicType, int nLogicID
		, int nSrcFacp = -1, int nSrcUnit = -1, int nSrcChn = -1, int nSrcDev = -1
	);
	void SetLinkData(CDataEmBc * pEm
		,  byte btLogicType, int nLogicID
		, CDataDevice * pSrc);
	void SetLinkData(CDataPattern * pPtn
		, byte btLogicType, int nLogicID
		, CDataDevice * pSrc);
	BOOL IsEqual(CDataLinked* pLink);
	int CopyData(CDataLinked * pSrc);
};