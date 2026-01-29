#pragma once

#include <wincred.h>
#pragma comment(lib, "Advapi32.lib")

class CWinCredManager : public TSingleton<CWinCredManager>
{
public:
	CWinCredManager();
	~CWinCredManager();

	bool SaveLoginToCredMan(const CString& email, const CString& password);
	bool LoadLoginFromCredMan(CString& outEmail, CString& outPassword);
	bool DeleteLoginFromCredMan();
};

