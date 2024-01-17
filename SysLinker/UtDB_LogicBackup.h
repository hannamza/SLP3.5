#pragma once
class YAdoDatabase;
class CUtDB_LogicBackup
{
public:
	CUtDB_LogicBackup(YAdoDatabase * pDB);
	~CUtDB_LogicBackup();

	YAdoDatabase *	m_pRefDB;


public:
	int WriteExcel(CString strFile);
};

