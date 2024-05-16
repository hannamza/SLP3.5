// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// SysLinker.h : SysLinker 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "ManagerEquip.h"

// CSysLinkerApp:
// 이 클래스의 구현에 대해서는 SysLinker.cpp을 참조하십시오.
//
class CRelayTableData;
class YAdoDatabase;
class CManagerEquip;
class CChildFrame;
class CDlgProgress;
struct ST_VIEWFORM_SELECTITEM
{
	BOOL				bInit;
	FormViewStyle		nViewType; ///< 열리고 있는 View의 타입
	int					nItemType;///< 선택된 아이템 타입
	DWORD_PTR			dwData;///< 선택된 아이템 데이터
	BOOL				bOpenComplete;
};
extern ST_VIEWFORM_SELECTITEM g_stViewSelectItem;

class CSysLinkerApp : public CWinAppEx
{
public:
	CSysLinkerApp();

	ULONG_PTR m_gdiplusToken;

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	CMultiDocTemplate		*	m_pTempleMakeLink; //< 항상화면에 표시
	CMultiDocTemplate		*	m_pTempleMakePtn;
	CMultiDocTemplate		*	m_pTemplePSwitch;
	CMultiDocTemplate		*	m_pTempleSwitch;
	CMultiDocTemplate		*	m_pTempleLinkView;
	CMultiDocTemplate		*	m_pTemplePump;
	CMultiDocTemplate		*	m_pTempleEquip;
	CMultiDocTemplate		*	m_pTempleUser;
	CMultiDocTemplate		*	m_pTempleAccess;
	CMultiDocTemplate		*	m_pTempleEmergency;
	CMultiDocTemplate		*	m_pTempleLocation;
	CMultiDocTemplate		*	m_pTempleFacp;
	CMultiDocTemplate		*	m_pTempleUnit;
	CMultiDocTemplate		*	m_pTempleRelayEdit;
	CMultiDocTemplate		*	m_pTempleAutoMake;
	CMultiDocTemplate		*	m_pTempleLogicEdit;
	CMultiDocTemplate		*	m_pTempleLoadRealy;
	CMultiDocTemplate		*	m_pTempleErrorCheck;

	// [KHS 2019-11-24 16:49:51] 
	// data
	std::shared_ptr<CManagerEquip>		m_spInputEquipManager;
	std::shared_ptr<CManagerEquip>		m_spEqNameManager;
	std::shared_ptr<CManagerEquip>		m_spOutputEquipManager;
	std::shared_ptr<CManagerEquip>		m_spContentsEquipManager;
	std::shared_ptr<CManagerEquip>		m_spPumpEquipManager;
	std::shared_ptr<CManagerEquip>		m_spPSEquipManager;
	std::shared_ptr<CManagerEquip>		m_spPmpNameEquipManager;
	CRelayTableData				*		m_pFasSysData; /// 중계기 일람표의 모든데이터

	CString								m_strPrjRootFolder;

	// [KHS 2019-12-4 17:54:41] 
	// Main Database 
	YAdoDatabase				*		m_pMainDb;

	// [KHS 2020-1-15 08:53:31] 
	// Program Login 
	CString								m_strProgramLoginUser;
	BOOL								m_bProgramLogin;

	// [KHS 2020-2-19 16:51:22] 
	// Progress Dlg
	CDlgProgress				*		m_pDlgProg;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

	public:
	CRelayTableData * GetRelayTableData() { return m_pFasSysData; }
	YAdoDatabase * GetMainDatabase() { return m_pMainDb; }
	afx_msg void OnHomeLogin();
	afx_msg void OnHomeProjectClose();
	afx_msg void OnHomeProjectNew();
	afx_msg void OnHomeProjectOpen();
	afx_msg void OnHomeProjectSave();
	afx_msg void OnBasicEnvset();
	afx_msg void OnBasicSetAccesslevel();
	afx_msg void OnBasicSetDraw();
	afx_msg void OnBasicSetEquip();
	afx_msg void OnBasicSetFacp();
	afx_msg void OnBasicSetInoutType();
	afx_msg void OnBasicSetPump();
	afx_msg void OnBasicSetUnit();
	afx_msg void OnBasicSetUser();
	afx_msg void OnBasicSetUsergroup();
	afx_msg void OnBasicSetLogicEdit();
	afx_msg void OnBasicSetEnv();
	//afx_msg void OnFacpCreateLink();
	afx_msg void OnFacpEditLink();
	afx_msg void OnFacpEmmergency();
	afx_msg void OnFacpLinkPs();
	afx_msg void OnFacpLinkPump();
	afx_msg void OnFacpLocation();
	afx_msg void OnFacpPattern();
	afx_msg void OnFacpPreviewLink();
	afx_msg void OnFacpReverseLink();
	afx_msg void OnFacpSetFacppwd();

	afx_msg void OnUpdateHomeLogin(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHomeProjectClose(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHomeProjectNew(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHomeProjectOpen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHomeProjectSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicEnvset(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetAccesslevel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetDraw(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetEquip(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetFacp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetInoutType(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetPump(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetUnit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetUser(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetUsergroup(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBasicSetEditLogic(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateFacpCreateLink(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpEditLink(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpEmmergency(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpLinkPs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpLinkPump(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpLocation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpPattern(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpPreviewLink(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpReverseLink(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFacpSetFacppwd(CCmdUI *pCmdUI);


	int SetSystemPath();
	int DocumentTemplateInit();
	CView* OpenFormView(FormViewStyle iStyle);
	BOOL CloseFormView(FormViewStyle iStyle);
	BOOL PostMessageFormView(FormViewStyle iStyle , UINT uMsg , WPARAM wp , LPARAM lp);
	BOOL PostMessageAllView(UINT uMsg, WPARAM wp, LPARAM lp);
	int SetActiveView(CChildFrame * pChild);
	CView* GetSysLinkerView(CMultiDocTemplate* pDocTemp);
	CDocument* GetSysLinkerDoc(CMultiDocTemplate* pDocTemp);

	int LoadEquipBaseData();
	int ConfigLoad();
	int ConfigSave();
	int CreateProject();
	int CreateProjectFromRom(CString strRomPath, CPtrList *pPtrList , BOOL bUseRvFile);
	int OpenProject(CString strPrjName, CString strPrjFullPath, DWORD dwVer , BOOL bVerTemp);
	int CloseProject();
	int RemoveTemplate();
	/*!
	 * @fn         CreateProjectFolder
	 * @version    1.0
	 * @date       2019-12-20 10:15:56
	 * @param      
	 * @brief      새로운 프로젝트를 만든다.
	 * @return    성공 시 1 , 실패시 0
	 * @note 
		1. 프로젝트 명으로 폴더 생성
		2. 프로젝트 명안에 VERSION_TEMP 생성 (버전 저장시에 VERSION 1.0 폴더 생성)
	 */
	int CreateProjectFolder();

	/*!
	 * @fn         CopyBaseFile
	 * @version    1.0
	 * @date       2019-12-20 10:25:18
	 * @param      
	 * @brief      설비 image 복사 , Database복사
	 * @return     return
	 * @note 
			1. VERSION_TEMP로 SYMBOL 복사
			2. 중계기 일람표
			3. PROGRAM ROOT\DB\에서 BASE.DAT,BASE.LOG를 다른이름으로 VERSION_TEMP 복사
			Database 이름: ProjectName_data.mdf , ProjectName_LOG.ldf
	*/
	int CopyBaseFile(CString strPrjName , CString strTargetSymbolPath, CString strTargetRelayTablePath
					,CString strTargetDBPath
	);

	/*!
	 * @fn         CreateProjectDatabase
	 * @version    1.0
	 * @date       2019-12-20 10:26:46
	 * @param      
	 * @brief      데이터베이스 등록
	 * @return     return
	 * @note 
			1. VERSION_TEMP\DB에 프로젝트명_DATA.MDF , 프로젝트명_DATA.LDF를 DBMS에 등록
			2. PROJECT DB 연결
			3. 기본 데이터 입력
	 */
	int CreateProjectDatabase();

	/*!
	 * @fn         SaveProjectInfoFile
	 * @version    1.0
	 * @date       2019-12-20 10:32:44
	 * @param      
	 * @brief      프로젝트의 정보를 저장한다.
	 * @return     파일생성 실패시 0을 리턴한다.
	 * @note 
			1. Project 이름
			2. 현장명
			3. 현장담당자 연락처
			4. 프로젝트 생성자
			5. 프로제트 버전
			6. 프로젝트 Paath : 설정에서 설정한 Project Path이후부터
			7. Database이름
			8. Database 사용자
			9. Database 비밀번호
			10. 생성날짜
	 */
	int SaveProjectInfoFile(CString strCurrentPrjRootFolder);
	/*!
	 * @fn         SaveVersionInfoFile
	 * @version    1.0
	 * @date       2019-12-20 10:37:07
	 * @param      
	 * @brief      해당버전의 정보 저장
	 * @return     파일 생성 오류시 0
	 * @note 
			1. Project 버전
			2. Project 수정일
			3. Project 수정자
	 */
	int SaveVersionInfoFile(WORD wMajor , WORD wMinor , CString strCurrentPrjRootFolder , BOOL bVerTemp = TRUE);

	int OpenProjectInfoFile(CRelayTableData * pFasSysData, CString strPrjRootFolder , CString strPrjName);
	int OpenVersionInfoFile(CRelayTableData * pFasSysData, WORD wMajor, WORD wMinor, CString strPrjRootFolder, CString strPrjName, BOOL bVerTemp = TRUE);
	int OpenBaseDatabase();
	int OpenProjectDatabase(CRelayTableData * pFasSysData);

	int RegisterBasicDB();
	/*!
	 * @fn         CopyProjectVersionTemp
	 * @version    1.0
	 * @date       2020-1-02 09:40:01
	 * @param      
	 * @brief      
	 * @return     return
	 * @note 
				프로젝트 열때 선택된 버전의 프로젝트를 편집하기 위한 temp 폴더로 복사한다.
	 */
	int CopyProjectVersionTemp(CString strPrjName, CString strPrjFullPath, DWORD dwVer);

	/*!
	 * @fn         CopyVersionTempToVersion
	 * @version    1.0
	 * @date       2020-1-02 09:41:47
	 * @param      
	 * @brief      
	 * @return     return
	 * @note 
				프로젝트 저장시 현재 편집하고 있는 프로젝트를 저장한다.
	 */
	int CopyVersionTempToVersion(CString strPrjName, CString strPrjFullPath, WORD wMajor , WORD wMinor);

	afx_msg void OnFacpRelayedit();
	afx_msg void OnUpdateFacpRelayedit(CCmdUI *pCmdUI);
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
	int MakeCustomTreeViewItem(BOOL bOutView);
	int SaveCustomTreeViewItem(BOOL bOutView);
	int ViewFormSelectItem(FormViewStyle nViewType, int nItemType,DWORD_PTR dwData);

	/*!
	 * @fn         Thread_ViewSelectItem
	 * @version    1.0
	 * @date       2020-7-17 11:09:23
	 * @param      
	 * @brief      OpenDocumentFile로 Document와 View를 여는데 시간이 걸려 바로 사용할 수 없다.
				View가 완전히 다열릴때까지 기다렸다가 View에 접근해서 사용한다.
	 * @return     return
	 * @note 
	 */
	static DWORD Thread_ViewSelectItem(LPVOID lpData);
	//ST_VIEWFORM_SELECTITEM m_stViewSelectItem;
	int	ViewSelectItem();
	void SetFormViewInitComplete(FormViewStyle nViewType) {
		if (g_stViewSelectItem.bInit && g_stViewSelectItem.nViewType == nViewType)
			g_stViewSelectItem.bOpenComplete = TRUE;
	};

	afx_msg void OnHomeProjectTable();
	afx_msg void OnUpdateHomeProjectTable(CCmdUI *pCmdUI);
	void InsertLog(CString strLog);
	void InsertDebug(CString strLog);
	afx_msg void OnErrorCheck();
	afx_msg void OnUpdateErrorCheck(CCmdUI *pCmdUI);

	BOOL InitEquipmentInfoTable(CString strEquipmentDefinitionFile);	//20240516 GBM - 기본 DB의 설비 정의를 초기화
};

extern CSysLinkerApp theApp;
