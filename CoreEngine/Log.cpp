#include "stdafx.h"
#include "Log.h"

namespace Log
{
	__declspec(thread) Checkpoint* Checkpoint::s_pTop = nullptr;


	void PrintF_Internal(PCTSTR szFmt, const int* pParams, PTSTR szBuf, size_t nLenBuf, size_t nData)
	{
		int nLen = _vsntprintf(szBuf+nData, nLenBuf-nData-3, szFmt, (va_list) pParams);
		_tcscpy(szBuf+nLen+nData, _T("\r\n"));
		OutputDebugString(szBuf);
	}

	void PrintCheckpoints()
	{
		TCHAR sz[0x200];
		sz[0] = _T('\t');

		for (const Checkpoint* pCheckPoint = Checkpoint::s_pTop; pCheckPoint; pCheckPoint = pCheckPoint->m_pPrev)
			PrintF_Internal(pCheckPoint->m_szFmt, pCheckPoint->m_pParams, sz, _countof(sz), 1);

	}

	void PrintF(PCTSTR szFmt, const int* pParams)
	{
		TCHAR sz[0x200];
		PrintF_Internal(szFmt, pParams, sz, _countof(sz), 0);
	}


} // namespace Log

// For tests
void TstLog()
{
	{
		CHECKPOINT(_T("WorkerThread=%d") << GetCurrentThreadId())

		CHECKPOINT(_T("Processing I/O"))

		{
			CHECKPOINT(_T("Request from client ID=%u") << 246)

			{
				CHECKPOINT(_T("Sending a file. Path=%s") << _T("C:\\Blablabla.bin"))
				{
					PUTLOG(_T("Can't open file. Error=%u") << GetLastError());
					Log::PrintCheckpoints();
				}
			}
		}

	}
}

