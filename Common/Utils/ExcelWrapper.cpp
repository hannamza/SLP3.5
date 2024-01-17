#include "StdAfx.h"
#include "ExcelWrapper.h"


CExcelWrapper::CExcelWrapper(void)
	: m_bVisible(false)
	, m_nRowCount(0)
	, m_nColCount(0)
	, m_FontSize(9)
	, m_FontName("굴림체")
{
}



CExcelWrapper::~CExcelWrapper(void)
{
	Close();

}

bool CExcelWrapper::Create(bool bVisible /*= false*/, int nWorkSheet /*= 1*/, 
					   int nFontSize /*= 9*/, CString strFontName /*= "굴림체"*/)
{
	COleVariant vOpt((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	/*
	* SET FONT INFORMATION
	*/
	m_FontName = strFontName;
	m_FontSize = nFontSize;

	/*
	* START(CREATE) EXCEL
	*/
    m_ExcelApp.CreateDispatch(L"Excel.Application");
    if (!m_ExcelApp)
    {
        AfxMessageBox(L"엑셀 시작이 실패하였습니다.");
        return false;
    }

    m_Books = m_ExcelApp.get_Workbooks();
    m_WorkBook = m_Books.Add(vOpt);

	/*
	* 첫번재 SHEET
	*/
	m_Sheets = m_WorkBook.get_Worksheets();
    m_WorkSheet = m_Sheets.get_Item(COleVariant((short) nWorkSheet));

	if (bVisible)
	{
		m_ExcelApp.put_Visible(TRUE);
		m_ExcelApp.put_UserControl(TRUE);

		m_bVisible = true;
	}

	return true;
}

bool CExcelWrapper::Open(CString& strFile, bool bVisible /*= false*/, int nWorkSheet /*= 1*/, 
					 int nFontSize /*= 9*/, CString strFontName /*= "굴림체"*/)
{
	COleVariant vOpt((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	/*
	* SET FONT INFORMATION
	*/
	m_FontName = strFontName;
	m_FontSize = nFontSize;

	/*
	* START(CREATE) EXCEL
	*/
    m_ExcelApp.CreateDispatch(L"Excel.Application");
    if (!m_ExcelApp)
    {
        AfxMessageBox(L"엑셀 시작이 실패하였습니다.");
        return false;
    }
	m_ExcelApp.put_Visible(bVisible);
	m_ExcelApp.put_DisplayAlerts(false);
	m_Books = m_ExcelApp.get_Workbooks();
	m_WorkBook = m_Books.Open(strFile, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, 
		                      vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt);

	/*
	* GET SHEETS
	*/
	m_Sheets = m_WorkBook.get_Worksheets();
	m_WorkSheet = m_Sheets.get_Item(COleVariant((short) nWorkSheet));

	return true;
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : Close
* DESCRIPTION   : CLOSE EXCEL APPLICATION
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::Close()
{	
	CString     strFile;
	COleVariant vOpt((long)DISP_E_PARAMNOTFOUND, VT_ERROR);   

	if (!m_bVisible)
	{
		m_WorkBook.Close(COleVariant((short)FALSE), vOpt, vOpt);
		m_Books.Close();
		m_ExcelApp.Quit();
	}
	
	m_WorkSheet.ReleaseDispatch();
	m_Sheets.ReleaseDispatch();
	m_WorkBook.ReleaseDispatch();
	m_Books.ReleaseDispatch();
	m_ExcelApp.ReleaseDispatch();	
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SavaAs
* DESCRIPTION   : SAVE AS
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SavaAs(CString& strFile)
{
	COleVariant vTrue((short)TRUE),
                vFalse((short)FALSE),
                vOpt((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	//xlWorkbookNormal
    m_WorkBook.SaveAs(COleVariant(strFile), COleVariant((long)51), 
                      vOpt, vOpt, vOpt, vFalse, FALSE, vOpt, vOpt, vOpt, vOpt, vOpt);
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SaveCopyAs
* DESCRIPTION   : SAVE COPY AS
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SaveCopyAs(CString& strFile)
{
	m_WorkBook.SaveCopyAs((COleVariant)strFile);
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : Print
* DESCRIPTION   : PRINT
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::Print(int nStart, int nCount)
{
	CWorksheet  printSheet;
	COleVariant vTrue((short)TRUE), vFalse((short)FALSE);

	for ( int ii = nStart; ii < (nStart + nCount); ii++)
	{
		printSheet = m_Sheets.get_Item(COleVariant((short)(ii)));

		//printSheet.PrintOut((COleVariant)(long)1, (COleVariant)(long)100, (COleVariant)(long)1, vFalse, vTrue, vFalse, (COleVariant)(long)1, vFalse);
		printSheet._PrintOut((COleVariant)(long)1, (COleVariant)(long)100, (COleVariant)(long)1, vFalse, vTrue, vFalse, (COleVariant)(long)1, vFalse);

		printSheet.ReleaseDispatch(); 
	}
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetWorkSheetChange
* DESCRIPTION   : SET WORKSHEET CHANGE
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
bool CExcelWrapper::SetWorkSheetChange(int nSheet /*= 1*/)
{
	/*
	* SHEET가 없으면 추가함
	*/
	if (m_Sheets.get_Count() >= nSheet)
	    m_WorkSheet = m_Sheets.get_Item(COleVariant((short) nSheet));
	else
	{
		// [KHS 2019-11-24 14:34:19] 
		// 해당 sheet 가 없으면 리턴
		return false;
// 		VARIANT     varAfter;
// 		int         nSheet = m_Sheets.get_Count();
// 		COleVariant vOpt((long)DISP_E_PARAMNOTFOUND, VT_ERROR);  
// 
// 		m_WorkSheet = m_Sheets.get_Item(COleVariant((short) nSheet));
// 
//         varAfter.vt = VT_DISPATCH;
// 		varAfter.pdispVal = m_WorkSheet.m_lpDispatch;
// 
// 		m_WorkSheet = m_Sheets.Add(vOpt, varAfter, vOpt, vOpt);
	}
	return true;
}

bool CExcelWrapper::SetWorkSheetChange(CString strName)
{
	/*
	* SHEET가 없으면 추가함
	*/
	CWorksheet ws;
	int nCnt, i;
	CString str;
	nCnt = m_Sheets.get_Count();
	for(i = 0 ; i < nCnt ; i ++)
	{
		ws = m_Sheets.get_Item(COleVariant((short)(i+1)));
		str = ws.get_Name();
		if(strName == str)
		{
			m_WorkSheet = ws;
			return true; 
		}
	}

	return false;
}
/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : GetWorkSheet
* DESCRIPTION   : GET WORKSHEET CHANGE
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
bool CExcelWrapper::GetWorkSheet(int nSheet /*= 1*/)
{
	/*
	* SHEET가 없으면 추가함
	*/
	if (m_Sheets.get_Count() >= nSheet)
	{
	    m_WorkSheet = m_Sheets.get_Item(COleVariant((short) nSheet));
		return true;
	}
	else
		return false;
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetSheetName
* DESCRIPTION   :SGET SHEET NAME
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetSheetName(int nSheet, CString strName)
{
	if (m_Sheets.get_Count() >= nSheet)
	{
		CWorksheet workSheet;	

		workSheet = m_Sheets.get_Item(COleVariant((short) nSheet));

		workSheet.put_Name(strName);
	}
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : GetSheetName
* DESCRIPTION   :SGET SHEET NAME
*
* ARGUMENTS     :
*
* RETURNS       :
*
* NOTES         :
**************************************************************************************************************
*/
CString CExcelWrapper::GetSheetName(int nSheet)
{
	if (m_Sheets.get_Count() >= nSheet)
	{
		CWorksheet workSheet;

		workSheet = m_Sheets.get_Item(COleVariant((short)nSheet));

		return workSheet.get_Name();
	}
	else
		return _T("");
}
/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetItemText
* DESCRIPTION   : SET ITEM TEXT
*
* ARGUMENTS     : nRow : 1부터 시작
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetItemText(int nRow, int nCol, CString strText)
{
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString     strHead;
	
	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);

	range.put_Value2((COleVariant)strText);

	range.ReleaseDispatch();
}

void CExcelWrapper::SetItemText(CString strCell, CString strText)
{
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString     strHead;

	range = m_WorkSheet.get_Range((COleVariant)strCell, vOpt);

	range.put_Value2((COleVariant)strText);

	range.ReleaseDispatch();
}
/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetItemText
* DESCRIPTION   : SET ITEM TEXT
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetItemText(int nRow, int nCol, CString strText, bool bBold,
							int nSize /*= 9*/, int nTextColor /*= 0*/, CString strFontName /*= "굴림체"*/)
{
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString     strHead;
	CFontEx      oFont;
	
	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);

	range.put_Value2((COleVariant)strText);

	/*
	* SET FONT
	*/
	oFont = range.get_Font();

	oFont.put_Color(COleVariant((short)nTextColor));
	oFont.put_Name((COleVariant)strFontName);
	oFont.put_Size((COleVariant)(short)nSize);

	if (bBold)
	    oFont.put_Bold(COleVariant((short)1));
	else
		oFont.put_Bold(COleVariant((short)0));

	range.ReleaseDispatch();
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetArrayData
* DESCRIPTION   : SET ARRAY DATA
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetArrayData(COleSafeArray* pArray, int nRow, int nCol, int nWidth, int nHeight)
{
	CRange       range;	
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString     strHead;
	CFontEx      oFont;
	
	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);
	range = range.get_Resize((COleVariant)(short)nHeight, (COleVariant)(short)nWidth);
	range.put_Value2(*pArray);
	pArray->Detach();

	/*
	* SET FONT
	*/
	oFont = range.get_Font();
	oFont.put_Name((COleVariant)m_FontName);
	oFont.put_Size((COleVariant)(short)m_FontSize);

	/*
	* 사이즈를 글자 길이 만큼, 폰트 사이즈 9 설정
	*/
	range = range.get_EntireColumn();
    range.AutoFit();

	range.ReleaseDispatch();
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetColumn
* DESCRIPTION   : SET HEAD
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetItemFont(int nRow, int nCol, bool bBold /*= false*/,
							int nSize /*= 9*/, int nTextColor /*= 0*/, CString strFontName /*= "굴림체"*/)
{
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString     strHead;
	CFontEx      oFont;
	
	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);
	/*
	* SET FONT
	*/
	oFont = range.get_Font();

	oFont.put_Color(COleVariant((short)nTextColor));
	oFont.put_Name((COleVariant)strFontName);
	oFont.put_Size((COleVariant)(short)nSize);

	if (bBold)
		oFont.put_Bold(COleVariant((short)1));
	else
		oFont.put_Bold(COleVariant((short)0));

	range.ReleaseDispatch();
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetColumn
* DESCRIPTION   : SET HEAD
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetItemFont(int nRow, int nCol, int nWidth, int nHeight, bool bBold /*= false*/, 
							int nSize /*= 9*/, int nTextColor /*= 0*/, CString strFontName /*= "굴림체"*/)
{
	CRange       range;	
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString     strHead;
	CFontEx      oFont;
	
	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);
	range = range.get_Resize((COleVariant)(short)nHeight, (COleVariant)(short)nWidth);

	/*
	* SET FONT 설정
	*/
	oFont = range.get_Font();
	oFont.put_Color(COleVariant((short)nTextColor));
	oFont.put_Name((COleVariant)strFontName);
	oFont.put_Size((COleVariant)(short)nSize);

	if (bBold)
	    oFont.put_Bold(COleVariant((short)1));
	else
		oFont.put_Bold(COleVariant((short)0));

	range.ReleaseDispatch();	
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetItemBackground
* DESCRIPTION   : SET ITEM BACKGROUND COLOR
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetItemBackColor(int nRow, int nCol, int nWidth, int nHeight, int nBackColor /*= 255*/)
{
	CRange       range;
	CBorders     borders;
	CString     strHead;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CInteriorEx    interior;

	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);
	range = range.get_Resize((COleVariant)(short)nHeight, (COleVariant)(short)nWidth);
	
	/*
	* SET BACKGROUND
	*/
	interior = range.get_Interior();
	interior.put_ColorIndex(COleVariant((short)nBackColor));

	range.ReleaseDispatch();
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetColumn
* DESCRIPTION   : SET HEAD
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
CString CExcelWrapper::GetItemText(int nRow, int nCol)
{
	CString     strText;
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString     strHead;

	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);

	strText  = range.get_Value2();

	range.ReleaseDispatch();

	return strText;
}

CString CExcelWrapper::GetItemText(int nRow, CString strCol)
{
	CString     strText;
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	CString     strHead;

	strHead.Format(L"%s%d",strCol,nRow);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);

	strText  = range.get_Value2();

	range.ReleaseDispatch();
	return strText;
}

COleSafeArray CExcelWrapper::GetItemRange(int nStartRow,int nStartCol, int nEndRow , int nEndCol)
{
	CString     strStart , strEnd;
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND,VT_ERROR);
	CString     strHead;
	COleSafeArray sa;
	strStart = GetCellName(nStartRow,nStartCol);
	strEnd = GetCellName(nEndRow,nEndCol);

	range = m_WorkSheet.get_Range((COleVariant)strStart,vOpt);
	range = range.get_Resize(COleVariant((long)nEndRow - nStartRow + 1),COleVariant((long)nEndCol - nStartCol + 1));
	sa.Attach(range.get_Value2());
	range.ReleaseDispatch();
	return sa;
}

COleSafeArray CExcelWrapper::GetItemRange(CString strStartCell,CString strEndCell)
{
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND,VT_ERROR);
	CString     strHead;
	COleSafeArray sa;

	range = m_WorkSheet.get_Range((COleVariant)strStartCell,(COleVariant)strEndCell);
	sa.Attach(range.get_Value2());
	range.ReleaseDispatch();
	return sa;
}

CString CExcelWrapper::GetItemText(CString strCell)
{
	CString     strText;
	CRange       range;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	range = m_WorkSheet.get_Range((COleVariant)strCell, vOpt);
	strText  = range.get_Value2();
	range.ReleaseDispatch();
	return strText;
}
/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : OpenData
* DESCRIPTION   : SET DATA START
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::OpenData(int nRows, int nCols)
{    
    DWORD dwElements[2];

    dwElements[0]= nRows;  
    dwElements[1]= nCols; 

	m_nRowCount = nRows;
	m_nColCount = nCols;	

    m_DataArray.Create(VT_VARIANT, 2, dwElements);	
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetData
* DESCRIPTION   : SET DATA
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetData(int nRow, int nCol, CString strData, int nTextColor /*= -1*/,
						bool bBold /*= false*/, int nSize /*= 9*/, CString strFontName /*= "굴림체"*/)
{
	long nIndex[2];

	nIndex[0] = nRow;   
	nIndex[1] = nCol;

	m_DataArray.PutElement(nIndex, (COleVariant)strData);

	if (nTextColor >= 0)
	{
		ST_APU_EXCEL_EXTRA* pObj = new ST_APU_EXCEL_EXTRA;

		pObj->row = nRow;
		pObj->col = nCol;
		pObj->color = nTextColor;
		pObj->size = nSize;
		pObj->is_bold = bBold;
		wsprintf(pObj->font_name, strFontName);

		m_ExtraArray.Add((CObject*)pObj);
	}
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : CloseData
* DESCRIPTION   : SET DATA CLOSE
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::CloseData(int nRow, int nCol)
{
	SetArrayData(&m_DataArray, nRow, nCol, m_nColCount, m_nRowCount);

	m_DataArray.Detach();
	m_DataArray.Destroy();

	/*
	* OPTION PROCESS
	*/
	int                 nRealRow;
	int                 nRealCol;
	ST_APU_EXCEL_EXTRA* pObj;

	for (int ii = 0; ii < m_ExtraArray.GetSize(); ii++)
	{
		pObj = (ST_APU_EXCEL_EXTRA*)m_ExtraArray[ii];

		if (pObj)
		{
			nRealRow = pObj->row + nRow;
			nRealCol = pObj->col + nCol;

			SetItemFont(nRealRow, nRealCol, pObj->is_bold, pObj->size, pObj->color, (LPCTSTR)pObj->font_name);

			delete pObj;
			pObj = NULL;
		}
	}

	m_ExtraArray.RemoveAll();
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : GetCellName
* DESCRIPTION   : GET CELL NAME
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/

CString ColumnNumberToLetter( long lColumnNumber )
{
	CString sColumn;

	while ( lColumnNumber )
	{
		lColumnNumber--;
		sColumn = static_cast< TCHAR >( _T( 'A' ) + 
			( lColumnNumber % 26 ) ) +
			sColumn;
		lColumnNumber /= 26;
	}
	return ( sColumn );
}

CString CExcelWrapper::GetCellName(int nRow, int nCol)
{
	CString strName;

	strName.Format(L"%s%d",ColumnNumberToLetter(nCol),nRow);

	return strName;
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetMergeCells
* DESCRIPTION   : SET MERGE CELLS
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetMergeCells(int nRow, int nCol, int nWidth, int nHeight, int nAlign /*= APU_EXCEL_LEFT*/)
{
	CRange       range;
	CString     strHead;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);

	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);
	range = range.get_Resize((COleVariant)(short)nHeight, (COleVariant)(short)nWidth);

	range.put_MergeCells(COleVariant((short)1));
	range.put_VerticalAlignment(COleVariant((short)0xFFFFEFF4));

	switch(nAlign)
	{
	    case APU_EXCEL_CENTER:
			range.put_HorizontalAlignment(COleVariant((short)0xFFFFEFF4));
			break;
		case APU_EXCEL_RIGHT:
			range.put_HorizontalAlignment(COleVariant((short)0xFFFFEFC8));
			break;			
		default:
			range.put_HorizontalAlignment(COleVariant((short)0xFFFFEFDD));
			break;
	}	

	range.ReleaseDispatch();
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetBorderWeight
* DESCRIPTION   : SET MERGE CELLS
*
* ARGUMENTS     : 
*
* RETURNS       : 
*
* NOTES         : 
**************************************************************************************************************
*/
void CExcelWrapper::SetBorderWeight(int nRow, int nCol, int nWidth, int nHeight, int nWeight /*= 2*/)
{
	CRange       range;
	CBorders     borders;
	CString     strHead;
	COleVariant vOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);

	strHead = GetCellName(nRow, nCol);

	range = m_WorkSheet.get_Range((COleVariant)strHead, vOpt);
	range = range.get_Resize((COleVariant)(short)nHeight, (COleVariant)(short)nWidth);
	
	borders = range.get_Borders();		 
    borders.put_Weight(COleVariant((short)nWeight));

	range.ReleaseDispatch();
}
int CExcelWrapper::AddWorkSheet(CString strSheetName)
{
	VARIANT     varAfter;
	int         nSheet = m_Sheets.get_Count();
	COleVariant vOpt((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	//m_WorkSheet = m_Sheets.Add();// (COleVariant((short)nSheet));

	varAfter.vt = VT_DISPATCH;
	varAfter.pdispVal = m_WorkSheet.m_lpDispatch;

	m_WorkSheet = m_Sheets.Add(vOpt, varAfter, vOpt, vOpt);
	m_WorkSheet.put_Name(strSheetName);
	return 0;
}