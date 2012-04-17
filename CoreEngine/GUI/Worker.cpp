#include "stdafx.h"
#include "Worker.h"


__declspec(thread) CWorker::WorkContext
	CWorker::g_threadCtx			= { 0 };
DWORD CWorker::g_dwMainThread		= ::GetCurrentThreadId();


CWorker::CWorker()
{
	m_thread_race = false;
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
	CrThreadAutoInstallHelper crashrpt;

	ASSERT(g_threadCtx.bRunThread==false);
	g_threadCtx.bRunThread = true;

	// runs guarded callback
	m_ticktime = ::GetTickCount();
	WorkGuard();

	ASSERT(g_threadCtx.bRunThread==true);
	g_threadCtx.bRunThread = false;
	m_thread_race = false;
}

LONG CALLBACK WorkVectoredHandler(PEXCEPTION_POINTERS ExceptionInfo)// The handler should not call functions that acquire synchronization objects or allocate memory
{
	if( CWorker::g_threadCtx.bVecEnter )// handles only the first exception
	{
		CWorker::g_threadCtx.bVecEnter = false;
		Log::OutputStackPoints();
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void CWorker::WorkGuard()
{
	ASSERT(g_threadCtx.bRunGuard==false);
	g_threadCtx.bRunGuard = true;

	// install exception monitor
	g_threadCtx.bVecEnter = true;
	PVOID hVectorHandle = ::AddVectoredExceptionHandler(1, WorkVectoredHandler);

	// work guarded run
	try
	{
		m_single_cbk();
	} catch( CWorkError::CWorkException& e ) {
		OnErrorReport( e.perr );
	}
	::RemoveVectoredExceptionHandler( hVectorHandle );

	ASSERT(g_threadCtx.bRunGuard==true);
	g_threadCtx.bRunGuard = false;
}

// Interface
void CWorker::DirectWork( util::Callback<void()> runcbk )
{
	ASSERT(runcbk);

	m_single_cbk = runcbk;
	WorkGuard();
}

void CWorker::ThreadWork( util::Callback<void()> runcbk )
{
	ENSURE(m_thread_race==false);// system not designed to this kind of heavy-work!  ->  data-race problems
	m_thread_race = true;
	ASSERT(::GetCurrentThreadId() == CWorker::g_dwMainThread);// might not be the case; but it seems really dangerous  ->  data-race problems
	ASSERT(runcbk);

	m_single_cbk = runcbk;
	HANDLE hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _ThreadProcThunk,
			this, 0, NULL);
	::CloseHandle(hThread);
}

void CWorker::MinWait( DWORD ms )
{
	ASSERT(g_threadCtx.bRunThread==true);// checks if calling from a Work thread spawned by ThreadWork() cause else m_ticktime is not initialized

	DWORD dif = ::GetTickCount()-m_ticktime;
	if( ms>dif )
		::Sleep( ms-dif );
}