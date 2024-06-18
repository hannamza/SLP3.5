// DlgVersion.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgVersion.h"
#include "afxdialogex.h"
#include "../Common/Utils/StdioFileEx.h"

// CDlgVersion ��ȭ �����Դϴ�.

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


// CDlgVersion �޽��� ó�����Դϴ�.


BOOL CDlgVersion::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ctrlVersionList.InsertColumn(0, _T("������¥"), LVCFMT_LEFT, 30);
	m_ctrlVersionList.InsertColumn(1, _T("����"), LVCFMT_LEFT, 80);
	m_ctrlVersionList.InsertColumn(2, _T("������"), LVCFMT_LEFT, 80);
	m_ctrlVersionList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	FillVersionList(m_strPrjPath);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDlgVersion::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel = -1;
	DWORD dwVersion;
	nSel = m_ctrlVersionList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
	{
		AfxMessageBox(L"���õ� ������ �����ϴ�.");
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
				// VERSION_TEMP �϶� COUNT ����
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
		// Project File ���� ����
		USERLOG(L"������Ʈ ������ �����ϴµ� �����߽��ϴ�.\n");
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
