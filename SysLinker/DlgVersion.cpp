// DlgVersion.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgVersion.h"
#include "afxdialogex.h"
#include "../Common/Utils/StdioFileEx.h"

// CDlgVersion 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgVersion, CDialogEx)

CDlgVersion::CDlgVersion(CString strPrjName, CString strPrjPath, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_VERSION, pParent)
{
	m_wMajor = 1;
	m_wMinor = 0;
	m_wLastMajor = 1;
	m_wLastMinor = 0;
	m_strPrjPath = strPrjPath;
	m_strPrjName = strPrjName;
}

CDlgVersion::~CDlgVersion()
{
}

void CDlgVersion::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION_LIST, m_ctrlVersionList);
}


BEGIN_MESSAGE_MAP(CDlgVersion, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgVersion::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgVersion 메시지 처리기입니다.


BOOL CDlgVersion::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ctrlVersionList.InsertColumn(0, _T("수정날짜"), LVCFMT_LEFT, 30);
	m_ctrlVersionList.InsertColumn(1, _T("버전"), LVCFMT_LEFT, 80);
	m_ctrlVersionList.InsertColumn(2, _T("수정자"), LVCFMT_LEFT, 80);
	m_ctrlVersionList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	FillVersionList(m_strPrjPath);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgVersion::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = -1;
	DWORD dwVersion;
	nSel = m_ctrlVersionList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
	{
		AfxMessageBox(L"선택된 버전이 없습니다.");
		return;
	}
	dwVersion = (int)m_ctrlVersionList.GetItemData(nSel);
	m_wMinor = LOWORD(dwVersion);
	m_wMajor = HIWORD(dwVersion);
	CDialogEx::OnOK();
}

int CDlgVersion::FillVersionList(CString strPath)
{
	int nIdx = 0;
	CString strFile;
	CString strUpperFile, strTempUpper;
	CString strFindCriteria;
	CFileFind find;
	if (strPath.Right(1) != '\\')
		strPath += L"\\";
	strFindCriteria.Format(L"%s*.*", strPath);
	BOOL bResult = find.FindFile(strFindCriteria);
	while (bResult)
	{
		bResult = find.FindNextFile();
		strFile = find.GetFilePath();
		if (find.IsDots())
			continue;
		if (find.IsDirectory())
		{
			strUpperFile = strFile;
			strUpperFile.MakeUpper();
			if (strUpperFile.Find(L"VERSION") >= 0)
			{
				// VERSION_TEMP 일때 COUNT 무시
				if (strUpperFile.Find(L"TEMP") >= 0)
					continue; 
				if (SetVersionList(nIdx, strFile) > 0)
					nIdx++;
			}
		}
	}
	return nIdx;
}

int CDlgVersion::SetVersionList(int nRow, CString strVerFile)
{
	std::vector<CString> vtArr;
	CStdioFileEx file;
	DWORD dwData =0;
	WORD wMajor, wMinor;
	CString strtemp, strVersion;
	if (!file.Open(strVerFile, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File 생성 실패
		USERLOG(L"프로젝트 파일을 생성하는데 실패했습니다.\n");
		return 0;
	}

	file.ReadString(strtemp); //GetPrjMajorNum GetPrjMinorNum
	strVersion = strtemp;
	vtArr = GF_SplitString(strtemp, L".");
	wMajor = _wtoi(vtArr[0]);
	wMinor = _wtoi(vtArr[1]);
	m_wLastMajor = max(m_wLastMajor, wMajor);
	m_wLastMinor = max(m_wLastMinor, wMinor);
	dwData = MAKELPARAM(wMinor, wMajor);
	file.ReadString(strtemp); //GetPrjModifyDate
	m_ctrlVersionList.InsertItem(nRow, strtemp);
	m_ctrlVersionList.SetItemText(nRow, 1, strVersion);
	file.ReadString(strtemp); //GetPrjModifier
	m_ctrlVersionList.SetItemText(nRow, 2, strtemp);
	m_ctrlVersionList.SetItemData(nRow, dwData);
	file.Close();
	return 1;
}
