#include "stdafx.h"
#include "Manager.h"


void CManager::ASSERT_ThreadUI()
{
	ASSERT( ::GetCurrentThreadId() == CWorker::g_dwMainThread );
}
void CManager::ASSERT_WorkGuarded()
{
	ASSERT( CWorker::IsWorkingGuard() );
}
void CManager::ASSERT_WorkThreaded()
{
	ASSERT( CWorker::IsWorkingGuard() );
	ASSERT( CWorker::IsWorkingThread() );
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

void CManager::OnErrorReport( const Err::sErrorData& err_data, Log::LogList& chks_list )// thrown/catched work-exception, now build the output list and send to output
{
	Log::LogList list;
	
	// setup message with the error parameters (errd)
	list.AddText<Log::WORK_ERROR>();
	list.AddText<Log::WORK_ERROR>( L"[Not-fatal error] ---------------------------" );
	list.AddText<Log::WORK_ERROR>( L"DETAILS" );
	if( err_data.log_msg.IsEmpty() && err_data.log_descr.IsEmpty() )
		list.AddText<Log::WORK_ERROR>( L"  Lazy programmer has provided none!" );
	else
	{
		if( !err_data.log_msg.IsEmpty() )
			list.AddText<Log::WORK_ERROR>( L"  msg  -> " + err_data.log_msg );
		if( !err_data.log_descr.IsEmpty() )
			list.AddText<Log::WORK_ERROR>( L"  desc -> " + err_data.log_descr );
	}
	if( chks_list.GetCount() )
	{
		list.AddText<Log::WORK_ERROR>( L"CONTEXT" );
		list.Append( chks_list );
	}
	list.AddText<Log::WORK_ERROR>( L"---------------------------------------------" );

	// calls application output handler
	ASSERT(Log::s_output);
	Log::s_output(list, Log::ERROR_BLOCK);
}
