#include "stdafx.h"
#include "Worker.h"


CWorker::CWorker( PCTSTR classInfo )
	: err( classInfo )
{
}

DWORD CWorker::Run()
{
	Work( TRUE );
	CloseHandle(m_hThread);
	m_hThread = NULL;
	return S_OK;
}

void CWorker::Work( BOOL threaded )
{
	if( threaded )
		m_ticktime = ::GetTickCount();
	else
		m_ticktime = 0xffffffff;

	try {
		WorkCallback();
	} catch( CErrExcept ) {
		ErrorHandler( threaded );
	} catch( CAutoReport& ) {
		ATLVERIFY( FALSE );
	} catch(...) {
		ATLVERIFY( FALSE );
	}
}


// Work process

void CWorker::ThreadWork()
{
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _ThreadProcThunk<CWorker>,
			this, 0, &m_dwThreadId);
}

void CWorker::DirectWork()
{
	Work( FALSE );
}

void CWorker::MinWait( DWORD ms )
{
	DWORD dif = ::GetTickCount()-m_ticktime;
	if( ms>dif )
		::Sleep( ms-dif );
}