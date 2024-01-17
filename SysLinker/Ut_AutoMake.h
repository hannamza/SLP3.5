#pragma once
class YAdoDatabase;

class CUt_AutoMake
{
public:
	CUt_AutoMake();
	~CUt_AutoMake();

protected:
	YAdoDatabase	*		m_pUtDB;
	BOOL			*		m_pbStop;



};

