#include "stdafx.h"
#include "Manager.h"


CManager::CManager()
{
}

void CManager::AssureThreadUI()
{
	ATLASSERT( ::GetCurrentThreadId() == CWorker::g_dwMainThread );
}

void CManager::AssureGuardedWork()
{
	ATLASSERT( CManager::g_bRunGuard );
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