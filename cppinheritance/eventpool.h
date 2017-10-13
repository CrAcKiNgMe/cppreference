#pragma  once
#include <queue>

extern int nExitFlag;

class CGeeMeeEvent
{
	typedef string(*PThunk)( std::map<string, string>& par);
	unsigned long ulID;
	PThunk		  m_thunk;
	std::map<string, string>        m_para;
public:
	CGeeMeeEvent(PThunk thunk,  std::map<string, string>& para)
	{
		m_thunk = thunk;
		m_para = para;
	}

	string Run()
	{
		return m_thunk(m_para);
	}

	CGeeMeeEvent()
	{
		;
	}
};

class CGeeMeeEventPool//balance load
{
#define POOL_CAPACITY 50
	unsigned int					m_uPrevRightNumber;
	std::queue<CGeeMeeEvent>		m_WaittingQueue;
	std::queue<DWORD>				m_uWorkerQueue;

	unsigned int					m_nCurrentPoolSize;
	unsigned int					m_nCurrentWokers;//正在工作的
	unsigned int					m_nMaxPoolSize;

	HANDLE							m_hSemaphore;
	CRITICAL_SECTION				m_cs;
	HANDLE							m_hThread[POOL_CAPACITY];

public:

	CGeeMeeEventPool(int nInitCount = 20, int nMaxCount = 50, int nTimeout = -1)
	{
		m_nCurrentPoolSize = 0;
		m_nCurrentWokers = 0;
		m_nMaxPoolSize = nMaxCount;
		m_hSemaphore = CreateSemaphore(NULL, nInitCount, nMaxCount, NULL);
		InitializeCriticalSection(&m_cs);
		Log("m_hSemaphore %08x", m_hSemaphore);


		for (int i = 0; i < nInitCount; ++i)
		{
			unsigned int id;
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, worker, (void*)this, 0, &id);

			if (hThread != 0)
			{
				m_hThread[i] = hThread;
				m_nCurrentPoolSize += 1;
			}

		}

	};

	void OnExit()
	{
		long prevcnt = 0;
		int nRet = ReleaseSemaphore(m_hSemaphore, 1, &prevcnt);
		Log("EndThread ReleaseSemaphore %d", prevcnt);
		while (prevcnt < 50 && nRet)
		{
			nRet = ReleaseSemaphore(m_hSemaphore, 1, &prevcnt);
		}


		WaitForMultipleObjects(m_nMaxPoolSize, m_hThread, TRUE, INFINITE);
	}


	~CGeeMeeEventPool(){};


	unsigned int			m_uCurrentLeader;//Leader ID	//


public:

	bool AddPool(const CGeeMeeEvent& connection)
	{
		m_WaittingQueue.push(connection);

		long lPrevCnt;
		ReleaseSemaphore(m_hSemaphore, 1, &lPrevCnt);
		return true;

	}

	bool RemovePool(DWORD dwWorkerId)
	{
		;
	}

	bool GrowPoolSize(DWORD dwSize)
	{
		EnterCriticalSection(&m_cs);
		for (int i = 0; i < dwSize; ++i)
		{
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, worker, (void*)this, 0, NULL);
			if (hThread)
			{
				m_hThread[m_nCurrentPoolSize] = hThread;
				m_nCurrentPoolSize += 1;
			}
		}
		LeaveCriticalSection(&m_cs);

		return true;
	}

	void IncreseWorkersByOne()
	{
		EnterCriticalSection(&m_cs);
		m_nCurrentWokers += 1;
		LeaveCriticalSection(&m_cs);
	}


	void DecreseWorkersByOne()
	{
		EnterCriticalSection(&m_cs);
		m_nCurrentWokers -= 1;
		LeaveCriticalSection(&m_cs);
	}

	bool GetConnection(CGeeMeeEvent& tmp)
	{
		bool bret = false;
		EnterCriticalSection(&m_cs);
		if (m_WaittingQueue.size() != 0)
		{
			bret = true;

			tmp = m_WaittingQueue.front();
			m_WaittingQueue.pop();
		}

		LeaveCriticalSection(&m_cs);

		return bret;
	}



	static unsigned int __stdcall worker(void* param)
	{

		Log("BeginThread(pool thread )[%d]", GetCurrentThreadId());

		CGeeMeeEventPool *pThis = (CGeeMeeEventPool *)param;

		while (1)
		{
			Log("worker %d", GetCurrentThreadId());
			//*//

			WaitForSingleObject(pThis->m_hSemaphore, INFINITE);


			if (nExitFlag)
			{
				Log("EndThread(pool thread )[%d]", GetCurrentThreadId());
				return -1;
			}


			pThis->IncreseWorkersByOne();


			if (pThis->m_nCurrentPoolSize <= pThis->m_nCurrentWokers)
			{
				if (pThis->m_nCurrentWokers = pThis->m_nMaxPoolSize)
				{

					pThis->GrowPoolSize(1);
				}
			}


			CGeeMeeEvent event;
			if (pThis->GetConnection(event))
			{
				event.Run();
				pThis->DecreseWorkersByOne();
			}

			Sleep(10);
		}



	}

};