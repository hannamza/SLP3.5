// DlgChildDelay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgChildDelay.h"
#include "afxdialogex.h"


// CDlgChildDelay 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildDelay, CDialogEx)

CDlgChildDelay::CDlgChildDelay(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DELAY, pParent)
{
	m_pParent = nullptr;
	m_nCurGroupNum = 0;
}

CDlgChildDelay::~CDlgChildDelay()
{
}

void CDlgChildDelay::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChildDelay, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgChildDelay::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgChildDelay::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDlgChildDelay::OnBnClickedButtonDel)
END_MESSAGE_MAP()


// CDlgChildDelay 메시지 처리기입니다.

BOOL CDlgChildDelay::CreateDlg(CWnd * pParent)
{
#ifndef ENGLISH_MODE
	return Create(IDD_DIALOG_DELAY, pParent);
#else
	return Create(IDD_DIALOG_DELAY, pParent);
#endif
}

void CDlgChildDelay::SetParentForMessage(CView* pParent)
{
	m_pParent = pParent;
}

void CDlgChildDelay::SetGroupInfo(unsigned short nGroupNum)
{
	CString strTemp = _T("");

	//현재 그룹 번호 적용
	m_nCurGroupNum = nGroupNum;

	//그룹 번호로 데이터 찾음
	GROUP_INFO* pGi = nullptr;
	auto& giMap = CNewInfo::Instance()->m_gmi.giMap;
	auto iter = giMap.find(nGroupNum);

	if (iter != giMap.end())
	{
		pGi = iter->second;
	}

	//그룹 번호
	CEdit* pGroupNum = (CEdit*)GetDlgItem(IDC_EDIT_GROUP_NUM);
	if (pGi != nullptr)
	{
		strTemp.Format(_T("%03d"), nGroupNum);
		pGroupNum->SetWindowTextW(strTemp);
	}
	else
	{
		pGroupNum->SetWindowTextW(_T(""));
	}

	//그룹 이름
	CEdit* pGroupName = (CEdit*)GetDlgItem(IDC_EDIT_GROUP_NAME);
	if (pGi != nullptr)
	{
		strTemp = CCommonFunc::CharToWCHAR(pGi->groupName);
		pGroupName->SetWindowTextW(strTemp);
	}
	else
	{
		pGroupName->SetWindowTextW(_T(""));
	}

	//조건
	CEdit* pCondition = (CEdit*)GetDlgItem(IDC_EDIT_MEET_NUM);
	if (pGi != nullptr)
	{
		strTemp.Format(_T("%d"), pGi->meetConditonCount);
		pCondition->SetWindowTextW(strTemp);
	}
	else
	{
		pCondition->SetWindowTextW(_T(""));
	}

	UpdateData(FALSE);
}

void CDlgChildDelay::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strTemp = _T("");
	// 그룹번호
	// 그룹번호를 관리해서 없는 번호 할당
	m_nCurGroupNum = CNewInfo::Instance()->GetNewGroupNum();
	CEdit* pGroupNum = (CEdit*)GetDlgItem(IDC_EDIT_GROUP_NUM);
	strTemp.Format(_T("%03d"), m_nCurGroupNum);
	pGroupNum->SetWindowTextW(strTemp);

	// 그룹이름
	// 그룹이름 + 그룹번호 기본 이름 생성
	CEdit* pGroupName = (CEdit*)GetDlgItem(IDC_EDIT_GROUP_NAME);
#ifndef ENGLISH_MODE
	strTemp.Format(_T("새_그룹_%03d"), m_nCurGroupNum);
#else
	strTemp.Format(_T("NEW_GROUP_%03d"), nGroupNum);
#endif
	pGroupName->SetWindowTextW(strTemp);

	// 조건

	// 할당 안된 상태 -> 사용자가 선택하도록 
	CEdit* pCondition = (CEdit*)GetDlgItem(IDC_EDIT_MEET_NUM);
	pCondition->SetWindowTextW(_T("0"));

	UpdateData(FALSE);

	// 새 그룹 생성
	GROUP_INFO* pNewGi = new GROUP_INFO;
	memset(pNewGi->groupName, 0, PROJECT_NAME_LENGTH);
	sprintf(pNewGi->groupName, CCommonFunc::WCharToChar(strTemp.GetBuffer(0)));
	pNewGi->meetConditonCount = 0;

	CNewInfo::Instance()->m_gmi.giMap[m_nCurGroupNum] = pNewGi;

	m_pParent->SendMessage(UWM_DELAYFORM_ADD_GROUP, NULL, NULL);
}


void CDlgChildDelay::OnBnClickedButtonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strTemp = _T("");

	// 그룹번호
	int nGroupNum = -1;
	CEdit* pGroupNum = (CEdit*)GetDlgItem(IDC_EDIT_GROUP_NUM);
	pGroupNum->GetWindowTextW(strTemp);
	nGroupNum = _wtoi(strTemp);

	// 그룹번호가 없으면 리턴
	if (nGroupNum == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("저장할 그룹 정보가 없습니다."));
#else
		AfxMessageBox(_T("There is no group information to save."));
#endif
		return;
	}

	// 그룹이름
	CString strGroupName = _T("");
	CEdit* pGroupName = (CEdit*)GetDlgItem(IDC_EDIT_GROUP_NAME);
	pGroupName->GetWindowTextW(strTemp);
	strGroupName = strTemp;

	// 조건
	char nCondition = -1;
	CEdit* pCondition = (CEdit*)GetDlgItem(IDC_EDIT_MEET_NUM);
	pCondition->GetWindowTextW(strTemp);
	nCondition = _wtoi(strTemp);

	// 그룹 정보 가져옴
	auto iter = CNewInfo::Instance()->m_gmi.giMap.find(nGroupNum);
	if (iter == CNewInfo::Instance()->m_gmi.giMap.end())
	{
		return;
	}
	GROUP_INFO* pGi = iter->second;

	// 조건이 현재 설정된 입력 회로 개수보다 많으면 리턴
	int nInputCircuitCount = pGi->inputEquipVec.size();
	if (nCondition > nInputCircuitCount)
	{
		CString strMsg = _T("");
#ifndef ENGLISH_MODE
		strMsg.Format(_T("필요 동작 수(%d)는 입력 회로 개수들의 합(%d)보다 클 수 없습니다."), nCondition, nInputCircuitCount);
#else
		strMsg.Format(_T("The number of required operations(%d) cannot exceed the sum of the number of input circuits(%d)."), nCondition, nInputCircuitCount);
#endif
		AfxMessageBox(strMsg);
		return;
	}

	// 그룹이름, 조건을 적용
	strcpy_s(pGi->groupName, CCommonFunc::WCharToChar(strGroupName.GetBuffer(0)));
	pGi->meetConditonCount = nCondition;

	// 부모 창을 거쳐 트리에 바뀐 그룹이름을 적용
	m_pParent->SendMessage(UWM_DELAYFORM_SAVE_GROUP, NULL, NULL);
}


void CDlgChildDelay::OnBnClickedButtonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 그룹번호
	CString strGroupNum = _T("");
	int nGroupNum = -1;
	CEdit* pGroupNum = (CEdit*)GetDlgItem(IDC_EDIT_GROUP_NUM);
	pGroupNum->GetWindowTextW(strGroupNum);
	nGroupNum = _wtoi(strGroupNum);

	// 그룹번호가 없으면 리턴
	if (nGroupNum == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("삭제할 그룹 정보가 없습니다."));
#else
		AfxMessageBox(_T("There is no group information to delete."));
#endif
		return;
	}

	// 메모리 삭제
	CNewInfo::Instance()->InitGroupMapInfo(nGroupNum);

	// 부모 창을 거쳐 트리에서 삭제
	m_pParent->SendMessage(UWM_DELAYFORM_DEL_GROUP, NULL, NULL);
}
