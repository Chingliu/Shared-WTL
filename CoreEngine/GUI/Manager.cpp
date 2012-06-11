#include "stdafx.h"
#include "Manager.h"


void CManager::AssureThreadUI()
{
	ASSERT( ::GetCurrentThreadId() == CWorker::g_dwMainThread );
}
void CManager::AssureWorkGuarded()
{
	ASSERT( CWorker::IsWorking() );
}
void CManager::AssureWorkThreaded()
{
	ASSERT( CWorker::IsWorking() );
	ASSERT( CWorker::IsWorkThread() );
}


CManager::CManager()
{
	logger.SetPrintFilter( BIND_MEM_CB(&CManager::PrinterFilter, this) );
}

bool CManager::PrinterFilter(Log::LogLine& lineref)// we are in the Manager context associated to the calling Printer, so just append the prefix
{
	ASSERT( !logger.m_prefix.IsEmpty() );

	CString prefix;
	prefix.Format( L"%s - ", logger.m_prefix );
	lineref.linestr = prefix += lineref.linestr;

	return true;
}

void CManager::OnErrorReport( CWorkError* source_err )// intercepts a thrown work-exception and outputs its error message
{
	CAtlArray<Log::LogLine> list;
	auto lAddToList = [&list](CString str) {
		Log::LogLine& line = list[list.Add()];
		line.type = Log::WORK_ERROR;
		line.linestr = str;
	};

	// setup message with the error parameters (source_err)
	lAddToList( CString() );
	lAddToList( L"---Error details-------------------------" );
	if( !source_err->log_msg.IsEmpty() )
		lAddToList( CString(L"err.msg  -> ") + source_err->log_msg );
	if( !source_err->log_descr.IsEmpty() )
		lAddToList( CString(L"err.desc -> ") + source_err->log_descr );

	// calls application output handler
	Log::s_output(list, Log::WORK_ERROR);
}


void CAdapterLink::SendSignal()
{
	ATLASSERT( ::GetCurrentThreadId() != CWorker::g_dwMainThread ); //only for threaded work else will cause a dead-lock!

	CHandle event_lock( ::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE | DELETE) );
	_wndMain.PostMessage( WMU_SIGNAL_SEND, (WPARAM) (HANDLE) event_lock );// not using SendMessage because it goes directly to the WinProc so can not be PreTranslated
	::WaitForSingleObject( event_lock, INFINITE );
}

void CAdapterLink::PostSignal()
{
	_wndMain.PostMessage( WMU_SIGNAL_POST );// no PostThreadMessage because a modal loop might cause the messages to be lost, yet Im not sure if this way is 100% safe
}