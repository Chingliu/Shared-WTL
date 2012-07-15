#include "stdafx.h"
#include "Manager.h"


void CManager::AssureThreadUI()
{
	ASSERT( ::GetCurrentThreadId() == CWorker::g_dwMainThread );
}
void CManager::AssureWorkGuarded()
{
	ASSERT( CWorker::IsWorkingGuard() );
}
void CManager::AssureWorkThreaded()
{
	ASSERT( CWorker::IsWorkingGuard() );
	ASSERT( CWorker::IsWorkThreaded() );
}


CManager::CManager()
{
	logger.SetPrintFilter( BIND_MEM_CB(&CManager::PrinterFilter, this) );
}

bool CManager::PrinterFilter(Log::LogLine& lineref)// we are in the Manager context associated to the calling Printer, so just append the prefix
{
	ASSERT( !logger.m_prefix.IsEmpty() );

	CString prefix;
	if( lineref.type==Log::CHECKPOINT )
		prefix.Format( L"  %s: ", logger.m_prefix );
	else
		prefix.Format( L"%s: ", logger.m_prefix );
	lineref.linestr = prefix += lineref.linestr;

	return true;// accepts line
}

void CManager::OnErrorReport( const CWorkErrorData& errd, Log::LogList& chks_list )// thrown work-exception, now outputs its error message
{
	Log::LogList list;
	
	// setup message with the error parameters (source_err)
	list.AddText<Log::WORK_ERROR>();
	list.AddText<Log::WORK_ERROR>( L"[Application exception] ---------------------" );
	list.AddText<Log::WORK_ERROR>( L"> Context (top is the most recent call)" );
	list.Append( chks_list );

	list.AddText<Log::WORK_ERROR>( L"> Error details" );
	if( errd.log_msg.IsEmpty() && errd.log_descr.IsEmpty() )
		list.AddText<Log::WORK_ERROR>( L"  Lazy programmer has provided none!" );
	else
	{
		if( !errd.log_msg.IsEmpty() )
			list.AddText<Log::WORK_ERROR>( L"  err.msg  -> " + errd.log_msg );
		if( !errd.log_descr.IsEmpty() )
			list.AddText<Log::WORK_ERROR>( L"  err.desc -> " + errd.log_descr );
	}
	list.AddText<Log::WORK_ERROR>( L"---------------------------------------------" );

	// calls application output handler
	ASSERT(Log::s_output);
	Log::s_output(list, Log::ERROR_BLOCK);
}


void CAdapterLink::SendSignal()
{
	CAtlList<int> test;
	ASSERT( ::GetCurrentThreadId() != CWorker::g_dwMainThread ); //only for threaded work else will cause a dead-lock!

	CHandle event_lock( ::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE | DELETE) );
	_wndMain.PostMessage( WMU_SIGNAL_SEND, (WPARAM) (HANDLE) event_lock );// not using SendMessage because it goes directly to the WinProc so can not be PreTranslated
	::WaitForSingleObject( event_lock, INFINITE );
}

void CAdapterLink::PostSignal()
{
	_wndMain.PostMessage( WMU_SIGNAL_POST );// no PostThreadMessage because a modal loop might cause the messages to be lost, yet Im not sure if this way is 100% safe
}