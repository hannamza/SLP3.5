#pragma once


// CDataEquip

class CDataEquip : public CObject
{

public:
	CDataEquip();
	virtual ~CDataEquip();

	SIMPLE_FUNC_IMPL(CString, EquipName, m_strEquipName);
	SIMPLE_FUNC_IMPL(CString, EquipDesc, m_strEquipDesc);
	SIMPLE_FUNC_IMPL(CString, FileName, m_strFileName);
	SIMPLE_FUNC_IMPL(INT_PTR, EquipID, m_nEqID);
	SIMPLE_FUNC_IMPL(INT_PTR, EquipType, m_nEqType);
protected:
	INT_PTR			m_nEqID;
	CString			m_strEquipName; 
	CString			m_strEquipDesc;
	INT_PTR			m_nEqType;
	CString			m_strFileName;
public:
	void SetData(int nID 
		, int nType
		, CString strName, CString strDesc
		, CString strFileName
	);
	void CopyData(CDataEquip * pSrc);
};


