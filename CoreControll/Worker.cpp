#include "stdafx.h"
#include "Worker.h"


CWorker::CWorker( PCTSTR classInfo )
	: CThreadImpl( CREATE_SUSPENDED ), err( classInfo )
{
}

DWORD CWorker::Run()
{
	Work( TRUE );
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
	Resume();
}

void CWorker::DirectWork()
{
	Work( FALSE );
}

void CWorker::MinWait( DWORD min )
{
	DWORD dif = ::GetTickCount()-m_ticktime;
	if( min>dif )
		::Sleep( min-dif );
}