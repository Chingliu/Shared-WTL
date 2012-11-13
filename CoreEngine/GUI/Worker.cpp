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
		jmp CWorker::ThreadRun
	}
}

void CWorker::ThreadRun()
{
#ifndef DEBUG
#ifdef _CRASHRPT_H_
	CrThreadAutoInstallHelper crashrpt;
#endif
#endif

	ASSERT(g_threadCtx.bRunThread==false);
	g_threadCtx.bRunThread = true;

	// calls internal guard routine
	m_ticktime = ::GetTickCount();
	WorkGuard();

	ASSERT(g_threadCtx.bRunThread==true);
	g_threadCtx.bRunThread = false;
	m_thread_race = false;// releases SpinThreadEnd
}

LONG CALLBACK WorkVectoredHandler(PEXCEPTION_POINTERS ExceptionInfo)// The handler should not call functions that acquire synchronization objects or allocate memory
{
	if( CWorker::g_threadCtx.bVecEnter )// handles only the first exception cause catch(){} handlers can trigger/rethrow exceptions
	{									// tested ::RemoveVectoredExceptionHandler and it dont work here
		CWorker::g_threadCtx.bVecEnter = false;
		Log::SaveStackCheckpoints(*CWorker::g_threadCtx.pLogList);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void CWorker::WorkGuard()
{
	ASSERT(g_threadCtx.bRunGuard==false);// nesting work guards? not rdy for it yet, should work with different threads (ThreadWork)
	g_threadCtx.bRunGuard = true;

	// setup logging
	Log::LogList chks_list;
	g_threadCtx.pLogList = &chks_list;
	err.ResetMessages();

	// install exception monitor
	g_threadCtx.bVecEnter = true;
	PVOID hVectorHandle = ::AddVectoredExceptionHandler(1, WorkVectoredHandler);

	// work guarded run
	try
	{
		m_single_cbk();
	} catch( Err::CException& e ) {
		OnErrorReport( e.err_data, chks_list );
	} catch(...) {
		ENSURE(false);
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
	ENSURE(m_thread_race==false);// same Worker spawning another Worker-thread; system not designed to this kind of heavy-work!  ->  data-race problems
	m_thread_race = true;		 // you better think a GUI sync alternative
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

void CWorker::SpinThreadEnd()
{
	while( m_thread_race );
}

bool CWorker::IsThreading()
{
	return m_thread_race;
}