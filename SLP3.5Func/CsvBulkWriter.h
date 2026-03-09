#pragma once
class CCsvBulkWriter
{
public:
	CCsvBulkWriter();
	~CCsvBulkWriter();

	bool Open(const CString& path);
	void Close();
	void WriteHeaderOnce();
	void AppendRow(int src_facp, int src_unit, int src_chn, int src_rly,
		int link_type, int lg_type, int lg_id,
		int tgt_facp, int tgt_unit, int tgt_chn, int tgt_rly,
		const CString& add_user);
	void Flush();

	const CString& GetPath() const { return m_path; }

private:
	CFile m_file;
	CString m_buf;
	CString m_path;

	CString EscapeCsv(const CString& in);
	void AppendLine(const CString& line);
};

