#pragma once
//#include "Excel.h"
//using namespace ExcelIF;

#include "Excel12.h"
using namespace Excel_Interface;

enum EN_APU_EXCELALIGN
{
	APU_EXCEL_LEFT,
	APU_EXCEL_CENTER,
	APU_EXCEL_RIGHT
};

/*$PAGE*/
/*
**************************************************************************************************************
*                                            STRUCT TYPE
**************************************************************************************************************
*/

struct ST_APU_EXCEL_EXTRA
{
	int                 row;           // ROW
	int                 col;           // COLUMN
	int                 color;         // FONT COLOR
	int                 size;          // FONT SIZE
	bool                is_bold;       // BOLD ¿©ºÎ
	TCHAR               font_name[21]; // FONT NAME
};

class CExcelWrapper
{
public:
	CExcelWrapper(void);
	~CExcelWrapper(void);

	CApplication		m_ExcelApp;
	CWorkbooks			m_Books;
	CWorkbook			m_WorkBook;
	CWorksheets			m_Sheets;
	CWorksheet			m_WorkSheet;	

	//_Application		m_ExcelApp;
	//Workbooks			m_Books;
	//_Workbook			m_WorkBook;
	//Worksheets			m_Sheets;
	//_Worksheet			m_WorkSheet;	

	bool				m_bVisible;

	COleSafeArray		m_DataArray;
	CObArray			m_ExtraArray;
	int					m_nRowCount;
	int					m_nColCount;

	CString				m_FontName;
	int					m_FontSize;

	protected:
	/*
	* INTERFACE FUNCTION
	*/
	CString GetCellName(int nRow, int nCol);
	void    SetArrayData(COleSafeArray* pArray, int nRow, int nCol, int nWidth, int nHeight);

public:
	/*
	* INTERFACE FUNCTION
	*/
	bool Create(bool bVisible = false, int nWorkSheet = 1, 
		        int nSize = 9, CString strFontName = L"±¼¸²Ã¼");
	bool Open(CString& strFile, bool bVisible = false, int nWorkSheet = 1, 
		      int nSize = 9, CString strFontName = L"±¼¸²Ã¼");
	void Close();

	void SavaAs(CString& strFile);
	void SaveCopyAs(CString& strFile);

	void Print(int nStart, int nCount);

	bool SetWorkSheetChange(int nSheet = 1);
	bool SetWorkSheetChange(CString strName);
	bool GetWorkSheet(int nSheet = 1);
	void SetSheetName(int nSheet, CString strName);
	CString GetSheetName(int nSheet);

	void SetItemText(int nRow, int nCol, CString strText);
	void SetItemText(CString strCell, CString strText);
	void SetItemText(int nRow, int nCol, CString strText, bool bBold, 
		             int nSize = 9, int nTextColor = 0, CString strFontName =L"±¼¸²Ã¼");

	void SetItemFont(int nRow, int nCol, bool bBold = false, 
		             int nSize = 9, int nTextColor = 0, CString strFontName = L"±¼¸²Ã¼");
	void SetItemFont(int nRow, int nCol, int nWidth, int nHeight, bool bBold = false, 
		             int nSize = 9, int nTextColor = 0, CString strFontName = L"±¼¸²Ã¼");

	void SetItemBackColor(int nRow, int nCol, int nWidth, int nHeight, int nBackColor = 255);

	CString GetItemText(int nRow, int nColumn);
	CString GetItemText(int nRow, CString strCol);
	CString GetItemText(CString strCell);

	COleSafeArray GetItemRange(int nStartRow,int nStartCol,int nEndRow,int nEndCol);
	COleSafeArray GetItemRange(CString strStartCell,CString strEndCell);

	void OpenData(int nRows, int nCols);
	void SetData(int nRow, int nCol, CString strData, int nTextColor = -1,
		         bool bBold = false, int nSize = 9, CString strFontName = L"±¼¸²Ã¼");
	void CloseData(int nRow, int nCol);
	
	void SetMergeCells(int nRow, int nCol, int nWidth, int nHeight, int nAlign = APU_EXCEL_LEFT);
	void SetBorderWeight(int nRow, int nCol, int nWidth, int nHeight, int nWeight = 2);

	int GetSheetCount() { return m_Sheets.get_Count(); }
	int AddWorkSheet(CString strName);
};

