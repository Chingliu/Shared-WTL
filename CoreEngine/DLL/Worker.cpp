#include "stdafx.h"
#include "Worker.h"


DWORD CWorker::g_dwMainThread	= ::GetCurrentThreadId();
bool CWorker::g_bRunGuard		= false;

CWorker::CWorker()
{
}

__declspec(naked) DWORD WINAPI _ThreadProcThunk(void*)
{
	__asm
	{
		pop		eax
		pop		ecx
		push	eax
		xor		eax, eax
		jmp CWorker::Run
	}
}

void CWorker::Run()
{
	m_ticktime = ::GetTickCount();
	WorkGuard();
	::CloseHandle(m_hThread);
}

void CWorker::WorkGuard()
{
	ATLASSERT(CWorker::g_bRunGuard==false);
	CWorker::g_bRunGuard = true;// g_bRunGuard has __declspec(thread) which may not work for dynamically loaded DLLs
	ATLASSERT(CWorker::g_bRunGuard); //rmv

	class LogExceptMonitor : public Exc::Monitor
	{
		virtual bool Handle(EXCEPTION_RECORD* pExc, CONTEXT* pCpuCtx)
		{
			//Log::PrintCheckpoints();
			return false;
		}
	};

	LogExceptMonitor monitor;
	try
	{
		m_single_cbk();
	} catch( CWorkError::CWorkException& ) {
		ATLASSERT(false);
	}

	ATLASSERT(CWorker::g_bRunGuard); //rmv
	CWorker::g_bRunGuard = false;
	ATLASSERT(CWorker::g_bRunGuard==false); //rmv
}

// Interface
void CWorker::DirectWork( util::Callback<void()> runcbk )
{
	ATLASSERT(runcbk);

	m_single_cbk = runcbk;
	WorkGuard();
}

void CWorker::ThreadWork( util::Callback<void()> runcbk )
{
	ATLASSERT(::GetCurrentThreadId() == CWorker::g_dwMainThread);
	ATLASSERT(runcbk);

	m_single_cbk = runcbk;
	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _ThreadProcThunk,
			this, 0, NULL);
}

void CWorker::MinWait( DWORD ms )
{
	// TODO: check if calling from a Work thread spawned by ThreadWork()
	DWORD dif = ::GetTickCount()-m_ticktime;
	if( ms>dif )
		::Sleep( ms-dif );
}