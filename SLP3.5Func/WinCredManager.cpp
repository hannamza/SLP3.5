#include "stdafx.h"
#include "WinCredManager.h"


CWinCredManager::CWinCredManager()
{
}


CWinCredManager::~CWinCredManager()
{
}

bool CWinCredManager::SaveLoginToCredMan(const CString& email, const CString& password)
{
	CREDENTIALW cred{};
	cred.Type = CRED_TYPE_GENERIC;
	cred.TargetName = WINDOWS_CREDENTIAL_MANAGER_KEY;
	cred.Persist = CRED_PERSIST_LOCAL_MACHINE; // 또는 ENTERPRISE
	cred.UserName = (LPWSTR)email.GetString();

	// UTF-16 문자열 그대로 저장
	cred.CredentialBlob = (LPBYTE)password.GetString();
	cred.CredentialBlobSize = (DWORD)((password.GetLength() + 1) * sizeof(wchar_t));

	return CredWriteW(&cred, 0) == TRUE;
}

bool CWinCredManager::LoadLoginFromCredMan(CString& outEmail, CString& outPassword)
{
	PCREDENTIALW pcred = nullptr;
	if (!CredReadW(WINDOWS_CREDENTIAL_MANAGER_KEY, CRED_TYPE_GENERIC, 0, &pcred))
		return false;

	if (pcred->UserName)
		outEmail = pcred->UserName;

	if (pcred->CredentialBlob && pcred->CredentialBlobSize > 0)
		outPassword = (LPCWSTR)pcred->CredentialBlob;

	CredFree(pcred);
	return true;
}

bool CWinCredManager::DeleteLoginFromCredMan()
{
	return CredDeleteW(WINDOWS_CREDENTIAL_MANAGER_KEY, CRED_TYPE_GENERIC, 0) == TRUE;
}