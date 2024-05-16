// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// SysLinker.h : SysLinker ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "ManagerEquip.h"

// CSysLinkerApp:
// �� Ŭ������ ������ ���ؼ��� SysLinker.cpp�� �����Ͻʽÿ�.
//
class CRelayTableData;
class YAdoDatabase;
class CManagerEquip;
class CChildFrame;
class CDlgProgress;
struct ST_VIEWFORM_SELECTITEM
{
	BOOL				bInit;
	FormViewStyle		nViewType; ///< ������ �ִ� View�� Ÿ��
	int					nItemType;///< ���õ� ������ Ÿ��
	DWORD_PTR			dwData;///< ���õ� ������ ������
	BOOL				bOpenComplete;
};
extern ST_VIEWFORM_SELECTITEM g_stViewSelectItem;

class CSysLinkerApp : public CWinAppEx
{
public:
	CSysLinkerApp();

	ULONG_PTR m_gdiplusToken;

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	CMultiDocTemplate		*	m_pTempleMakeLink; //< �׻�ȭ�鿡 ǥ��
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
	CRelayTableData				*		m_pFasSysData; /// �߰�� �϶�ǥ�� ��絥����

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
	 * @brief      ���ο� ������Ʈ�� �����.
	 * @return    ���� �� 1 , ���н� 0
	 * @note 
		1. ������Ʈ ������ ���� ����
		2. ������Ʈ ��ȿ� VERSION_TEMP ���� (���� ����ÿ� VERSION 1.0 ���� ����)
	 */
	int CreateProjectFolder();

	/*!
	 * @fn         CopyBaseFile
	 * @version    1.0
	 * @date       2019-12-20 10:25:18
	 * @param      
	 * @brief      ���� image ���� , Database����
	 * @return     return
	 * @note 
			1. VERSION_TEMP�� SYMBOL ����
			2. �߰�� �϶�ǥ
			3. PROGRAM ROOT\DB\���� BASE.DAT,BASE.LOG�� �ٸ��̸����� VERSION_TEMP ����
			Database �̸�: ProjectName_data.mdf , ProjectName_LOG.ldf
	*/
	int CopyBaseFile(CString strPrjName , CString strTargetSymbolPath, CString strTargetRelayTablePath
					,CString strTargetDBPath
	);

	/*!
	 * @fn         CreateProjectDatabase
	 * @version    1.0
	 * @date       2019-12-20 10:26:46
	 * @param      
	 * @brief      �����ͺ��̽� ���
	 * @return     return
	 * @note 
			1. VERSION_TEMP\DB�� ������Ʈ��_DATA.MDF , ������Ʈ��_DATA.LDF�� DBMS�� ���
			2. PROJECT DB ����
			3. �⺻ ������ �Է�
	 */
	int CreateProjectDatabase();

	/*!
	 * @fn         SaveProjectInfoFile
	 * @version    1.0
	 * @date       2019-12-20 10:32:44
	 * @param      
	 * @brief      ������Ʈ�� ������ �����Ѵ�.
	 * @return     ���ϻ��� ���н� 0�� �����Ѵ�.
	 * @note 
			1. Project �̸�
			2. �����
			3. �������� ����ó
			4. ������Ʈ ������
			5. ������Ʈ ����
			6. ������Ʈ Paath : �������� ������ Project Path���ĺ���
			7. Database�̸�
			8. Database �����
			9. Database ��й�ȣ
			10. ������¥
	 */
	int SaveProjectInfoFile(CString strCurrentPrjRootFolder);
	/*!
	 * @fn         SaveVersionInfoFile
	 * @version    1.0
	 * @date       2019-12-20 10:37:07
	 * @param      
	 * @brief      �ش������ ���� ����
	 * @return     ���� ���� ������ 0
	 * @note 
			1. Project ����
			2. Project ������
			3. Project ������
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
				������Ʈ ���� ���õ� ������ ������Ʈ�� �����ϱ� ���� temp ������ �����Ѵ�.
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
				������Ʈ ����� ���� �����ϰ� �ִ� ������Ʈ�� �����Ѵ�.
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
	 * @brief      OpenDocumentFile�� Document�� View�� ���µ� �ð��� �ɷ� �ٷ� ����� �� ����.
				View�� ������ �ٿ��������� ��ٷȴٰ� View�� �����ؼ� ����Ѵ�.
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

	BOOL InitEquipmentInfoTable(CString strEquipmentDefinitionFile);	//20240516 GBM - �⺻ DB�� ���� ���Ǹ� �ʱ�ȭ
};

extern CSysLinkerApp theApp;
