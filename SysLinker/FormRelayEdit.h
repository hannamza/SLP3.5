#pragma once

#include "../Common/Control/SplitterCtrl.h"
#include "../Common/Control/DTreeCtrl.h"
#include "afxcmn.h"
// CFormRelayEdit
/*!
* @class      CFormRelayEdit
* @author     KHS
* @version    1.0
* @date       2020-1-17 14:13:47
* @brief      회로,수신기,계통,유닛의 편집 기능(추가,수정,삭제)
* @note
1. 편집기능
2. 변경되는 부분 표시
3. 변경 적용 여부
* Contact  hskim0602@gmail.com
*/

// CFormRelayEdit 폼 뷰입니다.
class CDlgChildEditRelay;
class CDlgChildRelayTree;
class CDlgChildChangeView;
class CRelayTableData;
class CDlgChildEditMain;
class CDataSystem;
class CFormRelayEdit : public CFormView
{
	DECLARE_DYNCREATE(CFormRelayEdit)

protected:
	CFormRelayEdit();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormRelayEdit();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMRELAYEDIT };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	SplitterCtrl m_SpMain;
	SplitterCtrl m_SpTop; //< 회로 트리 , 정보 , 수정
	SplitterCtrl m_SpBottom; //< 변경 전후 표시

	CDlgChildRelayTree		*	m_pDlgTopTreePane; //< Check Tree Pane , 수신기별 유닛별 계통별 회로 표시
	CDlgChildEditMain		*	m_pDlgTopMainPane; //< 선택항목 정보 표시
//	CDlgChildEditRelay			*	m_pDlgTopEditPane; //< 선택항목 정보 수정 - 수신기,유닛,계통,회로번호,위치정보,입/출력타입,출력내용 편집
	CDlgChildChangeView		*	m_pDlgBottomChangePane; // 선택항목 정보 수정시 영향을 미치는 항목 표시

	CRelayTableData			*	m_pRefFasSysData;
	ST_TREEITEM				*	m_pCurData; //< 현재 선택된 아이템
	//ST_TREEITEM					m_NewData; //< 복사된 아이템 - 편집된 데이터가 들어간다.
	ST_TREEITEM				*	m_pNewData;
// 	VT_HITEM				*	m_pvtCurData;
// 	VT_HITEM				*	m_pvtNewData;

	CPtrList					m_ptrChangeList; //< 변경정보를 저장하는 리스트
	CMapChangeDevice			m_mapChangeDevice; //< 변경되는 Device 맵 , Key : 이전 Map Key

	CPtrList					m_ptrSysRefChangeList; ///< 수신기,유닛,채널 만 저장한다.
													///< 
	BOOL						m_bChangeFlag;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	int InitForm();
	int InitSplitter();
	int SetSplitterPos();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnRelayTreeSelChanged(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnEditDlgPreview(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnEditDlgDataChange(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);

	void CopyTreeItem(ST_TREEITEM * pSource, ST_TREEITEM * pDest);
	void DeleteTreeItem(ST_TREEITEM * pNewData);
	void SetNewTreeItem(int nType, ST_TREEITEM * pNewData);
	/*!
	 * @fn         MakeChangeDeviceMap
	 * @version    1.0
	 * @date       2020-1-31 10:44:24
	 * @param      
				ST_TREEITEM * pNewData : 

	 * @brief      
				현재 선택된 아이템(m_pCurData)의 Address
			
	 * @return     return
	 * @note 
	 */
	int MakeChangeDeviceMap(ST_TREEITEM * pNewData , int nDefaultAction);
	int MakeInputChangeData(ST_TREEITEM * pNewData, int nDefaultAction);
	int MakeInputLinkChangeData(ST_TREEITEM * pNewData);
	int MakeOutputChangeData(ST_TREEITEM * pNewData, int nDefaultAction);
	int MakePatternChangeData(ST_TREEITEM * pNewData, int nDefaultAction);

	int MakeDeleteDeviceMap(ST_TREEITEM * pCurData);
	int MakeInputDeleteData(ST_TREEITEM * pCurData);
	int MakeOutputDeleteData(ST_TREEITEM * pCurData);
	int MakeUsedLinkDeleteData(ST_TREEITEM * pCurData);
	int MakePatternDeleteData(ST_TREEITEM * pCurData);


	void RemoveChangeDevice();
	BOOL GetIDNum(ST_TREEITEM* pTreeItem, int &nFid, int &nUid, int &nCid, int &nDevid
		, int &nFNum, int &nUNum, int &nCNum, int &nDevNum
	);


	CDataSystem * DataAddFacp(YAdoDatabase * pDB);
	CDataSystem * DataAddUnit(YAdoDatabase * pDB);
	CDataSystem * DataAddChn(YAdoDatabase * pDB);
	CDataSystem * DataAddRelay(YAdoDatabase * pDB);
	int DataSaveFacp(YAdoDatabase * pDB);
	int DataSaveChn(YAdoDatabase * pDB);
	int DataSaveUnit(YAdoDatabase * pDB);
	int DataSaveRelay(YAdoDatabase * pDB);
	int DataDeleteFacp(YAdoDatabase * pDB);
	int DataDeleteChn(YAdoDatabase * pDB);
	int DataDeleteUnit(YAdoDatabase * pDB);
	int DataDeleteRelay(YAdoDatabase * pDB);
	int DataAdd();
	int DataSave();
	int DataDelete();
	int SaveAffectedByChange(YAdoDatabase * pDB);
	int ApplyChangeSystemMemory();
	int DeleteAffectedByChange(YAdoDatabase * pDB);
	int ApplyDeleteSystemMemory();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	// [KHS 2020-5-11 14:34:09] 
	// 새로 변경할 ID/Number를 입력한다.
	// int nbF, int nbU, int nbC, int nbD
	// , int iF, int iU, int iC, int iD
	int GetChangeNumber(CDataDevice * pDev, int nbF, int nbU, int nbC, int nbD
		, int iF, int iU, int iC, int iD
		, int &nFid, int &nUid, int &nCid, int &nDevid
		, int &nFNum, int &nUNum, int &nCNum, int &nDevNum);
	// [KHS 2020-5-11 14:34:09] 
	// 새로 변경할 ID/Number를 입력한다.
	// int nbF, int nbU, int nbC, int nbD
	// , int iF, int iU, int iC, int iD
	// [KHS 2020-5-13 11:35:28] 
	// 사용안함
	int ChangeDevice(int nType, LPVOID pNewSrc, LPVOID pOldData, int nbF, int nbU, int nbC, int nbD
		, int iF, int iU, int iC, int iD
		, int &nFid, int &nUid, int &nCid, int &nDevid
	);

	void RefreshAllData();
	int IsSameAddress(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData);
	CTabCtrl m_ctrlTab;
};


