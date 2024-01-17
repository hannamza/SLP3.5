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
* @brief      ȸ��,���ű�,����,������ ���� ���(�߰�,����,����)
* @note
1. �������
2. ����Ǵ� �κ� ǥ��
3. ���� ���� ����
* Contact  hskim0602@gmail.com
*/

// CFormRelayEdit �� ���Դϴ�.
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
	CFormRelayEdit();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	SplitterCtrl m_SpTop; //< ȸ�� Ʈ�� , ���� , ����
	SplitterCtrl m_SpBottom; //< ���� ���� ǥ��

	CDlgChildRelayTree		*	m_pDlgTopTreePane; //< Check Tree Pane , ���ű⺰ ���ֺ� ���뺰 ȸ�� ǥ��
	CDlgChildEditMain		*	m_pDlgTopMainPane; //< �����׸� ���� ǥ��
//	CDlgChildEditRelay			*	m_pDlgTopEditPane; //< �����׸� ���� ���� - ���ű�,����,����,ȸ�ι�ȣ,��ġ����,��/���Ÿ��,��³��� ����
	CDlgChildChangeView		*	m_pDlgBottomChangePane; // �����׸� ���� ������ ������ ��ġ�� �׸� ǥ��

	CRelayTableData			*	m_pRefFasSysData;
	ST_TREEITEM				*	m_pCurData; //< ���� ���õ� ������
	//ST_TREEITEM					m_NewData; //< ����� ������ - ������ �����Ͱ� ����.
	ST_TREEITEM				*	m_pNewData;
// 	VT_HITEM				*	m_pvtCurData;
// 	VT_HITEM				*	m_pvtNewData;

	CPtrList					m_ptrChangeList; //< ���������� �����ϴ� ����Ʈ
	CMapChangeDevice			m_mapChangeDevice; //< ����Ǵ� Device �� , Key : ���� Map Key

	CPtrList					m_ptrSysRefChangeList; ///< ���ű�,����,ä�� �� �����Ѵ�.
													///< 
	BOOL						m_bChangeFlag;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
				���� ���õ� ������(m_pCurData)�� Address
			
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
	// ���� ������ ID/Number�� �Է��Ѵ�.
	// int nbF, int nbU, int nbC, int nbD
	// , int iF, int iU, int iC, int iD
	int GetChangeNumber(CDataDevice * pDev, int nbF, int nbU, int nbC, int nbD
		, int iF, int iU, int iC, int iD
		, int &nFid, int &nUid, int &nCid, int &nDevid
		, int &nFNum, int &nUNum, int &nCNum, int &nDevNum);
	// [KHS 2020-5-11 14:34:09] 
	// ���� ������ ID/Number�� �Է��Ѵ�.
	// int nbF, int nbU, int nbC, int nbD
	// , int iF, int iU, int iC, int iD
	// [KHS 2020-5-13 11:35:28] 
	// ������
	int ChangeDevice(int nType, LPVOID pNewSrc, LPVOID pOldData, int nbF, int nbU, int nbC, int nbD
		, int iF, int iU, int iC, int iD
		, int &nFid, int &nUid, int &nCid, int &nDevid
	);

	void RefreshAllData();
	int IsSameAddress(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData);
	CTabCtrl m_ctrlTab;
};


