#include "stdafx.h"
#include "CsvBulkWriter.h"


CCsvBulkWriter::CCsvBulkWriter()
{
}


CCsvBulkWriter::~CCsvBulkWriter()
{
}

bool CCsvBulkWriter::Open(const CString& path)
{
	m_path = path;
	if (!m_file.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		return false;

	// UTF-8 BOM은 선택(없어도 됨). 있으면 엑셀로 볼 때 편함.
	// unsigned char bom[3] = {0xEF, 0xBB, 0xBF};
	// m_file.Write(bom, 3);

	m_buf.Empty();
	return true;
}

void CCsvBulkWriter::Close()
{
	Flush();
	if (m_file.m_hFile != CFile::hFileNull)
		m_file.Close();
}

void CCsvBulkWriter::WriteHeaderOnce()
{
	// 헤더 쓸거면 BULK INSERT에서 FIRSTROW=2
	AppendLine(L"SRC_FACP,SRC_UNIT,SRC_CHN,SRC_RLY,LINK_TYPE,LG_TYPE,LG_ID,"
		L"TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY,ADD_USER");
}

void CCsvBulkWriter::AppendRow(int src_facp, int src_unit, int src_chn, int src_rly,
	int link_type, int lg_type, int lg_id,
	int tgt_facp, int tgt_unit, int tgt_chn, int tgt_rly,
	const CString& add_user)
{
	CString u = EscapeCsv(add_user);

	CString line;
	line.Format(L"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s",
		src_facp, src_unit, src_chn, src_rly,
		link_type, lg_type, lg_id,
		tgt_facp, tgt_unit, tgt_chn, tgt_rly,
		u.GetString()
	);

	AppendLine(line);
}

void CCsvBulkWriter::Flush()
{
	if (m_buf.IsEmpty()) return;

	// CRLF로 통일(ROWTERMINATOR=0x0d0a 권장)
	CString out = m_buf;
	m_buf.Empty();

	// UTF-8로 변환해 파일에 쓰기
	int bytes = WideCharToMultiByte(CP_UTF8, 0, out, out.GetLength(), nullptr, 0, nullptr, nullptr);
	if (bytes > 0)
	{
		std::vector<char> tmp(bytes);
		WideCharToMultiByte(CP_UTF8, 0, out, out.GetLength(), tmp.data(), bytes, nullptr, nullptr);
		m_file.Write(tmp.data(), bytes);
	}
}

// CSV 필드 이스케이프: 콤마/따옴표/개행 있으면 "..."로 감싸고 "는 ""로
CString CCsvBulkWriter::EscapeCsv(const CString& in)
{
	CString s = in;
	s.Replace(L"\"", L"\"\"");

	bool needQuote =
		(s.Find(L',') >= 0) ||
		(s.Find(L'"') >= 0) ||
		(s.Find(L'\r') >= 0) ||
		(s.Find(L'\n') >= 0);

	if (needQuote)
		s = L"\"" + s + L"\"";
	return s;
}

void CCsvBulkWriter::AppendLine(const CString& line)
{
	m_buf += line;
	m_buf += L"\r\n"; // CRLF

					  // 버퍼가 어느 정도 커지면 한번에 write
	if (m_buf.GetLength() > 1 * 1024 * 1024) // 1MB 정도
		Flush();
}