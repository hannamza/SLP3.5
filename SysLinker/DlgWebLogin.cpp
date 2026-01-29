// DlgWebLogin.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgWebLogin.h"
#include "afxdialogex.h"
#include "SysLinker.h"

#include <regex>

UINT ThreadLoginProcess(LPVOID pParam)
{
	CDlgWebLogin* dwl = (CDlgWebLogin*)pParam;
	dwl->m_strErr = _T("");
	DWORD nStatus;
	int nRet = CWebConnManager::Instance()->LoginProcess(dwl->m_strEditEmail, dwl->m_strEditPassword, nStatus);
	if (nRet != LOGIN_SUCCEEDED)
	{
		//로그인이 실패했을 경우 에러 메세지를 표시
		dwl->m_strErr = dwl->MakeErrorMsg(nRet, nStatus);
	}

	dwl->m_pProgressBarDlg->PostMessage(WM_CLOSE);
	SetEvent(dwl->m_hThreadHandle);

	return 0;
}

// CDlgWebLogin 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgWebLogin, CDialogEx)

#ifndef ENGLISH_MODE
CDlgWebLogin::CDlgWebLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_WEB_LOGIN, pParent)
	, m_strEditEmail(_T(""))
	, m_strEditPassword(_T(""))
{
	m_pProgressBarDlg = nullptr;
}
#else
CDlgWebLogin::CDlgWebLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_WEB_LOGIN_EN, pParent)
{

}
#endif

CDlgWebLogin::~CDlgWebLogin()
{
}

void CDlgWebLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_strEditEmail);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strEditPassword);
}


BEGIN_MESSAGE_MAP(CDlgWebLogin, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgWebLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgWebLogin::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgWebLogin 메시지 처리기입니다.

BOOL CDlgWebLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CButton* pButton;

	if (g_stConfig.nAccountSave >= 1)
	{
		pButton = (CButton*)GetDlgItem(IDC_CHECK_SAVE_ACCOUNT);
		pButton->SetCheck(TRUE);

		if (!CWinCredManager::Instance()->LoadLoginFromCredMan(m_strEditEmail, m_strEditPassword))
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(_T("E-MAIL / PASSWORD 로드에 실패했습니다."));
#else
			AfxMessageBox(_T("Failed to load E-MAIL / PASSWORD."));
#endif
		}

		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgWebLogin::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	// 이메일, 비밀번호 입력 여부 검사
	if (m_strEditEmail.IsEmpty())
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("E-MAIL을 입력해 주세요."));
#else
		AfxMessageBox(_T("Please enter your E-MAIL."));
#endif
		return;
	}
	else if (m_strEditPassword.IsEmpty())
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("PASSWORD를 입력해 주세요."));
#else
		AfxMessageBox(_T("Please enter your PASSWORD."));
#endif
		return;
	}

	// 이메일 형식 검사 (rough 하게)

	if (!CheckEmailFormat(m_strEditEmail))
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("올바른 이메일 형식이 아닙니다."));
#else
		AfxMessageBox(_T("This is not a valid E-MAIL format."));
#endif
		return;
	}

	// 이 시점에서 위에서 한번은 이메일 형식을 검사했으니 Windows Credential Manager를 통해서 저장

	// E-MAIL / PASSWORD 저장 여부 저장
	CButton* pButton;

	pButton = (CButton*)GetDlgItem(IDC_CHECK_SAVE_ACCOUNT);
	if (pButton->GetCheck())
	{
		g_stConfig.nAccountSave = 1;
	}
	else
	{
		g_stConfig.nAccountSave = 0;
	}
	
	theApp.ConfigSave();

	// Windows Credential Manager에 저장
	if (g_stConfig.nAccountSave)
	{
		if (!CWinCredManager::Instance()->SaveLoginToCredMan(m_strEditEmail, m_strEditPassword))
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(_T("E-MAIL / PASSWORD 저장에 실패했습니다."));
			Log::Trace("E-MAIL / PASSWORD 저장에 실패했습니다.");
#else
			AfxMessageBox(_T("Failed to save E-MAIL / PASSWORD."));
			Log::Trace("Failed to save E-MAIL / PASSWORD.");
#endif
		}
	}

	// 로그인 유효성 검사
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
#ifndef ENGLISH_MODE
	CString strMsg = _T("로그인이 진행 중입니다.");
#else
	CString strMsg = _T("Login is in progress.");
#endif
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadLoginProcess, this);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

	// 에러 메세지가 있으면 화면에 표시하고 리턴
	if (!m_strErr.IsEmpty())
	{
		AfxMessageBox(m_strErr);
		Log::Trace("%s", CCommonFunc::WCharToChar(m_strErr.GetBuffer(0)));
		return;
	}

	m_pProgressBarDlg = nullptr;

	CDialogEx::OnOK();
}

bool CDlgWebLogin::CheckEmailFormat(const CString& email)
{
	CString s(email);
	s.Trim();                 // 앞뒤 공백 제거
	if (s.IsEmpty()) return false;

	// 실무용: 너무 엄격하지 않게, 흔한 오류를 걸러내는 패턴
	// - 로컬 파트: 영숫자 + ._%+- 허용 (연속 점/앞뒤 점은 별도 체크)
	// - 도메인: 라벨(영숫자/하이픈) + 점 + TLD(2~63)

	// 제일 앞 글자는 영문 대/소문자 + 숫자 1글자된 1글자, ?:는 비캡쳐 그룹으로 매칭된 전체 글자는 메모리에 저장하지만 매칭된 부분부분의 그룹 문자열은 저장하지 않는다는 의미
	// 그후 0~62자까지는 영문 대/소문자 + 숫자 + 특수문자 로 구성되는데 (?: ...)가 ?로 묶였으므로 없을 수도 있음, @은 반드시 존재
	// @이후는 앞서와 @이후 첫 글자는 영문 대/소문자 + 숫자 1글자된 1글자가 반드시 있고 이후에는 0~62자의 영문 대/소문자 + 숫자로 구성되고 (?: ...)가 ?로 묶였으므로 없을 수도 있음
	// .은 반드시 존재하며 .이후에는 영문 대/소문자 + 숫자로 구성된 2~63개의 문자가 존재

	static const std::wregex re(
		LR"(^[A-Za-z0-9](?:[A-Za-z0-9._%+\-]{0,62}[A-Za-z0-9])?@(?:[A-Za-z0-9](?:[A-Za-z0-9\-]{0,61}[A-Za-z0-9])?\.)+[A-Za-z]{2,63}$)",
		std::regex::ECMAScript
	);

	std::wstring ws(s.GetString());

	// 추가 흔한 케이스 방어: 연속된 점("..") 금지
	if (ws.find(L"..") != std::wstring::npos) return false;

	return std::regex_match(ws, re);
}

CString CDlgWebLogin::MakeErrorMsg(int nErrorType, int nStatusCode)
{
	CString strErr = _T("");
	CString strStatusCode = _T("");

	switch (nErrorType)
	{
	case INTERNET_OPEN_FAILED:
		strErr = _T("INTERNET_OPEN_FAILED");
		break;

	case INTERNET_CONNECT_FAILED:
		strErr = _T("INTERNET_CONNECT_FAILED");
		break;

	case HTTP_OPEN_REQUEST_FAILED:
		strErr = _T("HTTP_OPEN_REQUEST_FAILED");
		break;

	case HTTP_SEND_REQUEST_FAILED:
		strErr = _T("HTTP_SEND_REQUEST_FAILED");
		break;

	case SYNTAX_ERROR:
#ifndef ENGLISH_MODE
		strErr = _T("올바르지 않은 이메일 혹은 비밀번호를 입력했습니다.");
#else
		strErr = _T("You entered an incorrect email or password.");
#endif
		break;

	case NO_USER:
#ifndef ENGLISH_MODE
		strErr = _T("등록되지 않은 이메일 혹은 잘못된 비밀번호입니다.\r\n회원이 아니시면 LIST에서 회원 가입을 해주세요.");
#else
		strErr = _T("This is an unregistered email address or incorrect password.\r\nIf you are not a member, please register as a member at LIST.");
#endif
		break;

	case EMPTY_RESPONSE_BODY:
		strErr = _T("EMPTY_RESPONSE_BODY");
		break;

	case JSON_PARSING_FAILED:
		strErr = _T("JSON_PARSING_FAILED");
		break;

	case ACCOUNT_REQUIRING_EMAIL_VERIFICATION:
#ifndef ENGLISH_MODE
		strErr = _T("이메일 인증을 하지 않은 계정입니다.");
#else
		strErr = _T("This account does not have email verification.");
#endif
		break;

	case ACCOUNT_REQUIRING_ADMIN_APPROVAL:
#ifndef ENGLISH_MODE
		strErr = _T("관리자 승인을 대기 중인 계정입니다.");
#else
		strErr = _T("This account is awaiting administrator approval.");
#endif
		break;

	case BANNED_ACCOUNT:
#ifndef ENGISH_MODE
		strErr = _T("로그인이 차단된 계정입니다.");
#else
		strErr = _T("This account has been blocked from logging in.");
#endif
		break;

	case EMPTY_TOKEN_VALUE:
		strErr = _T("EMPTY_TOKEN_VALUE");
		break;

	case UNDEFINED_RESPONSE:
		strStatusCode.Format(_T("%d"), nStatusCode);
#ifndef ENGLISH_MODE
		strErr = _T("정의되지 않은 Response 입니다. Response Code : ") + strStatusCode;
#else
		strErr = _T("Undefined response. Response code : ") + strStatusCode;
#endif
		break;

	default:
		break;
	}

	return strErr;
}

void CDlgWebLogin::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
