#pragma once
class CNewInfo : public TSingleton<CNewInfo>
{
public:
	CNewInfo();
	~CNewInfo();

public:
	F4APPENDIX_INFO m_fi;
	EQUIPMENT_INFO m_ei;
};