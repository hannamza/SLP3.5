

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

// ascii definitions
#define     ASCII_BEL               0x07
#define     ASCII_BS                0x08
#define     ASCII_LF                0x0A
#define     ASCII_CR                0x0D
#define     ASCII_XON               0x11
#define     ASCII_XOFF              0x13

#define     MAX_BUFFER              4096

// Parity Bits
enum enParity
{
    enNOPARITY,
    enODDPARITY,
    enEVENPARITY,
    enMARKPARITY,
    enSPACEPARITY
};

// Stop Bits
enum enStopBits
{
    enONESTOPBIT,
    enONE5STOPBITS,
    enTWOSTOPBITS,
};

// Flow Control
enum enFlowControl
{
    enNOFLOWCTRL,
    enCTSRTSFLOWCTRL,
    enCTSDTRFLOWCTRL,
    enDSRRTSFLOWCTRL,
    enDSRDTRFLOWCTRL,
    enXONXOFFFLOWCTRL
};

#include "SyncObj.h"

typedef int CALLBACK FNREADPROC(LPVOID, int);
typedef FNREADPROC *LPFNREADPROC;


class CSerialPort //: public CObject 
{

public:
	CSerialPort();
	virtual ~CSerialPort();
public:
    int GetPort(void) { return m_nPort; }
    DWORD GetBaudRate(void) { return m_dcb.BaudRate; }
    void SetPort(int nPort = 1) { m_nPort = nPort; }
	void Initialize(void);
	virtual int ProcessingData(BYTE *lpData, UINT dwLen);
	BOOL SetByteSize(BYTE nByteSize = DATABITS_8);
	BOOL SetStopBits(BYTE nStopBits);
	BOOL SetParityBits(BYTE nParity);
	BOOL SetBaudRate(DWORD dwBaudRate = CBR_57600);

	BOOL GetOverlappedResult(OVERLAPPED &overlapped, DWORD &dwBytesTransferred, 
        BOOL bWait);
	BOOL Flush(void);
    BOOL ClearReadBuffer(void) { return Purge(PURGE_RXCLEAR); }
    BOOL ClearWriteBuffer(void) { return Purge(PURGE_TXCLEAR); }
	BOOL Open(int nPort = -1, DWORD dwBaud = CBR_57600, enParity parity = enNOPARITY, 
        BYTE DataBits = DATABITS_8, enStopBits stopbits = enONESTOPBIT, 
        enFlowControl fc = enNOFLOWCTRL, BOOL bOverlapped = TRUE);
	static DWORD CommReadProc(LPVOID lpData);
	BOOL StartCommThread(LPFNREADPROC lpfnNotify = NULL, int nPriority = THREAD_PRIORITY_NORMAL);
	BOOL ReadEx(void *lpBuf, DWORD dwCount);
	DWORD Read(void *lpBuf, DWORD dwMaxLen, BOOL bOverlapped = TRUE);
	BOOL Read(void *lpBuf, DWORD dwCount, OVERLAPPED &overlapped);
	virtual void OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped);
	static void WINAPI _OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped);
	BOOL WriteEx(const void *lpBuf, DWORD dwCount);
	BOOL Write(const void *lpBuf, DWORD dwCount, OVERLAPPED &overlapped);
	BOOL WaitEvent(DWORD &dwMask, OVERLAPPED &overlapped);
	BOOL WaitEvent(DWORD &dwMask);
	BOOL Escape(DWORD dwFunc = CLRDTR);
    BOOL ClearError(COMSTAT &cs);
    BOOL ClearError(DWORD &dwErrors);
	static BOOL SetDefaultConfig(int nPort, COMMCONFIG& config);
	BOOL Purge(DWORD dwFlags = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	BOOL SetReadTimeouts(DWORD nInterval, DWORD nMultiplier = 0, DWORD nConstant = 0);
	BOOL SetWriteTimeouts(DWORD &dwTimeouts);
	BOOL SetReadTimeouts(DWORD &dwTimeouts);
	BOOL SetTimeouts(void);
	BOOL SetTimeouts(COMMTIMEOUTS &timeouts);
	BOOL GetTimeouts(COMMTIMEOUTS &timeouts);
	BOOL Setup(DWORD dwInQueue = 4096, DWORD dwOutQueue = 4096);
	BOOL SetConfig(COMMCONFIG &cfg);
	BOOL GetMask(DWORD &dwMask);
	BOOL SetMask(DWORD dwMask = EV_RXCHAR);
	BOOL SetState(DCB &dcb);
	BOOL GetStatus(COMSTAT &status);
    BOOL GetState(DCB &dcb);
	DWORD Write(LPVOID lpBuf, DWORD dwLen, BOOL bOverlapped = TRUE);
	BOOL GetModemStatus(DWORD &dwModemStatus);
	BOOL GetConfig(COMMCONFIG &cfg);
	static BOOL GetDefaultConfig(int nPort, COMMCONFIG &cfg);
	virtual void Close(void);

    BOOL IsOpen() const { return m_hComm != INVALID_HANDLE_VALUE; };
    BOOL IsConnected() const { return m_bConnected; };
    DWORD GetLastError(void) { return m_dwLastError; }
    DWORD GetCommErrors(void) { return m_dwCommErrors; }
	virtual void	OnClose();

protected:
    LPFNREADPROC        m_lpfnReadProc;

//    enParity            m_Parity;
//    BYTE                m_DataBits;
//    enStopBits          m_StopBits;
//    enFlowControl       m_Fc;
protected:
    int                 m_nPort;
    DCB                 m_dcb;
    DWORD               m_dwBaudRate;
    CWinThread *        m_pThread;
    BOOL                m_bConnected;
    DWORD               m_dwLastError;
    DWORD               m_dwCommErrors;
    CSyncObj            m_csWrite;          // Write Sync Object.
    HANDLE              m_hComm;            // Comm Port Handle
    BOOL                m_bOverlapped;      // Overlapped IO
    OVERLAPPED          m_ovlRead;
    OVERLAPPED          m_ovlWrite;
	//DWORD				m_dwMinReceiveTime; // 일정 시간동안 Serial로 데이터를 못받으면 connection 끊김으로 처리
	//DECLARE_DYNAMIC(CSerialPort)
};

#endif // __SERIALPORT_H__
