#pragma once
#include <Wininet.h>
#pragma comment(lib, "wininet.lib")

using json = nlohmann::json;

class CWebConnManager : public TSingleton<CWebConnManager>
{
public:
	CWebConnManager();
	~CWebConnManager();

	// valuable
	ACCOUNT_INFO m_ai;
	LOGIN_RESPONSE_INFO m_lri;

	// method
	void InitAccountInfo();
	void InitLoginResponseInfo();
	BOOL CheckInternetConnected();
	BOOL QueryStatusCode(HINTERNET hReq, DWORD& statusCode);
	BOOL ReadAllResponse(HINTERNET hReq, std::string& out);
	int LoginProcess(CStringW& email, CStringW& pwd, DWORD& outStatusCode);
	CStringW Utf8ToW(const std::string& s);
	CStringW GetWStringOrEmpty(const json& j, const char* key);
	int GetIntOrZero(const json& j, const char* key);
	BOOL ParseAccoutInfoFromUtf8Json(const std::string& utf8Json);
};

