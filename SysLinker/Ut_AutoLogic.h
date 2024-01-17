#pragma once
class YAdoDatabase;

class CUt_AutoLogic
{
public:
	CUt_AutoLogic(YAdoDatabase * pDB);
	~CUt_AutoLogic();



	YAdoDatabase *	m_pRefDB;


public:
	int WriteOldDataExcel(CString strFile);

	int ChangeOldTableToNewTable();
};

