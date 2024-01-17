#pragma once

#define         Q_MAX_SIZE              8192
#define         Q_BUF_SIZE              1504
#define         Q_IDX_MASK              Q_MAX_SIZE-1


template <typename T>
class CBaseSingleton : public T
{
	// Singleton interface
private:
	static T* m_pOneAndOnlySingleton;
	static CCriticalSection m_crSingletonLock;
	void Lock()   { m_crSingletonLock.Lock(); }
	void Unlock() { m_crSingletonLock.Unlock(); }
public:
	static inline T* GetInstance()
	{
		if (!m_pOneAndOnlySingleton)
		{
			m_pOneAndOnlySingleton = new T;
		}
		return m_pOneAndOnlySingleton;
	}
	static inline void DestroySingleton()
	{
		if (m_pOneAndOnlySingleton)
		{
			try { delete m_pOneAndOnlySingleton; } catch (...) { NULL; }
			m_pOneAndOnlySingleton = NULL;
		}
	}
private:
	CBaseSingleton(void) { }
	virtual ~CBaseSingleton(void) { }
	CBaseSingleton(const CBaseSingleton& ObjSrc) { }
	const CBaseSingleton& operator=(CBaseSingleton const& ObjSrc) { }
};

// Singleton interface

template <typename T>
T* CBaseSingleton<T>::m_pOneAndOnlySingleton = NULL;

template <typename T>
CCriticalSection CBaseSingleton<T>::m_crSingletonLock;

class CEventQueue
{
public:
	CEventQueue(void);
	virtual ~CEventQueue(void);

public:
	UINT    WriteQueue(BYTE btType ,LPVOID lpData, UINT nSize);
	int    ReadQueue(BYTE &btType ,LPVOID lpData);

	void RemoveAll();
protected:
	CCriticalSection        m_csLock;
	int             		m_idxRD;
	int             		m_idxWD;
	int             		m_lenData[Q_MAX_SIZE];
	char            		m_szData[Q_MAX_SIZE][Q_BUF_SIZE];
};
typedef  CBaseSingleton<CEventQueue> TheQueueBuffer;
