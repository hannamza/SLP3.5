#include "stdafx.h"
#include "WebConnManager.h"

CWebConnManager::CWebConnManager()
{
	InitAccountInfo();
	InitLoginResponseInfo();
}


CWebConnManager::~CWebConnManager()
{
}

void CWebConnManager::InitAccountInfo()
{
	m_ai.email = _T("");
	m_ai.grade = -1;
}

void CWebConnManager::InitLoginResponseInfo()
{
	m_lri.id = 0;
	m_lri.email = _T("");
	m_lri.name = _T("");
	m_lri.role = _T("");
	m_lri.token = _T("");
	m_lri.cdt = _T("");
	m_lri.edt = _T("");
	m_lri.environmentName = _T("");
	m_lri.agencyName = _T("");
	m_lri.phone = _T("");
	m_lri.confirmYN = _T("");
	m_lri.appName = _T("");
	m_lri.appDt = _T("");
	m_lri.appYN = _T("");
	m_lri.blockYN = _T("");
	m_lri.delYN = _T("");
	m_lri.cName = _T("");
	m_lri.eName = _T("");
}

BOOL CWebConnManager::CheckInternetConnected()
{
	BOOL bRet = FALSE;
	DWORD flags;

	// flag로 자세한 사항을 알 수 있으나 인터넷 사용 여부만 확인하면 되므로 상관없음
	if (InternetGetConnectedState(&flags, 0))
	{
		bRet = TRUE;
	}

	return bRet;
}

BOOL CWebConnManager::QueryStatusCode(HINTERNET hReq, DWORD& statusCode)
{
	statusCode = 0;
	DWORD len = sizeof(statusCode);
	return HttpQueryInfo(hReq,
		HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&statusCode, &len, nullptr) == TRUE;
}

BOOL CWebConnManager::ReadAllResponse(HINTERNET hReq, std::string& out)
{
	out.clear();
	char buf[4096];
	DWORD dwRead = 0;

	while (InternetReadFile(hReq, buf, sizeof(buf), &dwRead) && dwRead > 0)
		out.append(buf, buf + dwRead);

	if (out.length() == 0)
		return FALSE;

	return TRUE;
}

int CWebConnManager::LoginProcess(CStringW& email, CStringW& pwd, DWORD& outStatusCode)
{
	int nRet = UNDEFINED_RESPONSE;

	DWORD nStatus = 0;
	CStringA strJson;
	strJson.Empty();

	HINTERNET hOpen = InternetOpenW(L"LIST", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hOpen) 
		return INTERNET_OPEN_FAILED;

	// 타임아웃 설정
	DWORD timeoutMs = 10000;
	InternetSetOption(hOpen, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
	InternetSetOption(hOpen, INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
	InternetSetOption(hOpen, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

	HINTERNET hConn = InternetConnectW(
		hOpen,
		L"list.gfs.co.kr",
		INTERNET_DEFAULT_HTTPS_PORT, // 443
		NULL, NULL,
		INTERNET_SERVICE_HTTP,
		0, 0);

	if (!hConn) 
	{ 
		InternetCloseHandle(hOpen); 
		return INTERNET_CONNECT_FAILED; 
	}

	// 경로는 "호스트 제외"한 부분만
	LPCWSTR path = L"/apina/Login/v1/Login";

	DWORD flags =
		INTERNET_FLAG_SECURE |
		INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_NO_CACHE_WRITE |
		INTERNET_FLAG_NO_COOKIES;

	HINTERNET hReq = HttpOpenRequestW(
		hConn,
		L"POST",
		path,
		L"HTTP/1.1",
		NULL, NULL,
		flags,
		0);

	if (!hReq)
	{
		InternetCloseHandle(hConn);
		InternetCloseHandle(hOpen);
		return HTTP_OPEN_REQUEST_FAILED;
	}

	// JSON 바디 (보통 서버는 UTF-8 기대)
	CStringA emailUtf8, pwdUtf8;
	{
		// 간단 변환(정교하게 하려면 WideCharToMultiByte로 CP_UTF8)
		emailUtf8 = CW2A(email, CP_UTF8);
		pwdUtf8 = CW2A(pwd, CP_UTF8);
	}

	CStringA body;
	body.Format("{\"email\":\"%s\",\"pwd\":\"%s\"}", (LPCSTR)emailUtf8, (LPCSTR)pwdUtf8);

	CStringA headers;
	headers += "Content-Type: application/json\r\n";
	headers += "Accept: application/json\r\n";

	BOOL ok = HttpSendRequestA(
		hReq,
		headers,
		(DWORD)headers.GetLength(),
		(LPVOID)(LPCSTR)body,
		(DWORD)body.GetLength());

	if (!ok)
	{
		// 네트워크/TLS/인증서/프록시 등으로 실패 가능
		DWORD err = GetLastError();
		// TODO: 로그 기록
		InternetCloseHandle(hReq);
		InternetCloseHandle(hConn);
		InternetCloseHandle(hOpen);
		return HTTP_SEND_REQUEST_FAILED;
	}

	// 로그인 성공/실패 판단 구간
	QueryStatusCode(hReq, nStatus);

	std::string resp;
	std::string utf8;
	switch (nStatus)
	{
	case HTTP_STATUS_BAD_REQUEST:
		nRet = SYNTAX_ERROR;
		break;

	case HTTP_STATUS_NOT_FOUND:
		nRet = NO_USER;
		break;

	case HTTP_STATUS_OK:	
		if (!ReadAllResponse(hReq, resp))
		{
			nRet = EMPTY_RESPONSE_BODY;
			break;
		}
		strJson = CStringA(resp.c_str());
		utf8 = strJson.GetString();
		if (ParseAccoutInfoFromUtf8Json(utf8))
		{
			if (m_lri.confirmYN == "N")
			{
				nRet = ACCOUNT_REQUIRING_EMAIL_VERIFICATION;
			}
			else if (m_lri.appYN == "N")
			{
				nRet = ACCOUNT_REQUIRING_ADMIN_APPROVAL;
			}
			else if (m_lri.blockYN == "Y")
			{
				nRet = BANNED_ACCOUNT;
			}
			else if (m_lri.token.IsEmpty())
			{
				nRet = EMPTY_TOKEN_VALUE;
			}
			else
			{
				nRet = LOGIN_SUCCEEDED;
			}
		}
		else
		{
			nRet = JSON_PARSING_FAILED;
		}
		break;

	default:
		break;
	}

	outStatusCode = nStatus;

	InternetCloseHandle(hReq);
	InternetCloseHandle(hConn);
	InternetCloseHandle(hOpen);

	return nRet;
}

CStringW CWebConnManager::Utf8ToW(const std::string& s)
{
	int wlen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
	CStringW out;
	wchar_t* buf = out.GetBuffer(wlen);
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), buf, wlen);
	out.ReleaseBuffer(wlen);
	return out;
}

CStringW CWebConnManager::GetWStringOrEmpty(const json& j, const char* key)
{
	auto it = j.find(key);
	if (it == j.end() || it->is_null()) return L"";
	if (it->is_string()) return Utf8ToW(it->get<std::string>());
	// 숫자/기타 타입이면 문자열로 변환하고 싶을 때
	return Utf8ToW(it->dump());
}

int CWebConnManager::GetIntOrZero(const json& j, const char* key)
{
	auto it = j.find(key);
	if (it == j.end() || it->is_null()) return 0;
	if (it->is_number_integer()) return it->get<int>();
	if (it->is_string()) return atoi(it->get<std::string>().c_str());
	return 0;
}

BOOL CWebConnManager::ParseAccoutInfoFromUtf8Json(const std::string& utf8Json)
{
	try
	{
		// 계정 정보를 채우기 전에 초기화
		InitLoginResponseInfo();

		json j = json::parse(utf8Json);

		m_lri.id = GetIntOrZero(j, "id");
		m_lri.email = GetWStringOrEmpty(j, "email");
		m_lri.name = GetWStringOrEmpty(j, "name");
		m_lri.role = GetWStringOrEmpty(j, "role");

		m_lri.token = GetWStringOrEmpty(j, "token");
		m_lri.cdt = GetWStringOrEmpty(j, "cdt");
		m_lri.edt = GetWStringOrEmpty(j, "edt");

		m_lri.environmentName = GetWStringOrEmpty(j, "environmentName");
		m_lri.agencyName = GetWStringOrEmpty(j, "agencyName");
		m_lri.phone = GetWStringOrEmpty(j, "phone");

		m_lri.confirmYN = GetWStringOrEmpty(j, "confirmYN");
		m_lri.appName = GetWStringOrEmpty(j, "appName");
		m_lri.appDt = GetWStringOrEmpty(j, "appDt");
		m_lri.appYN = GetWStringOrEmpty(j, "appYN");
		m_lri.blockYN = GetWStringOrEmpty(j, "blockYN");
		m_lri.delYN = GetWStringOrEmpty(j, "delYN");
		m_lri.cName = GetWStringOrEmpty(j, "cName");
		m_lri.eName = GetWStringOrEmpty(j, "eName");

		return TRUE;
	}
	catch (...)
	{
		return FALSE;
	}
}