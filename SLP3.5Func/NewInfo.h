#pragma once
class CNewInfo : public TSingleton<CNewInfo>
{
public:
	CNewInfo();
	~CNewInfo();

public:
	PROJECT_INFO m_pi;
	F4APPENDIX_INFO m_fi;
};