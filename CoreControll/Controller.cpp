#include "stdafx.h"
#include "Controller.h"


CController::CController( CAdapter* adapter, PCTSTR classInfo )
	: CWorker( classInfo )
{
	m_adapter = adapter;
}

// Signals

void CController::SendSignal()
{
	m_msgHost.PostMessage( WMU_SIGNAL, m_clientID );
}

void CController::SendReturn()
{
	m_msgHost.SendMessage( WMU_SIGNAL, m_clientID );
}

void CController::SendError()
{
	m_msgHost.PostMessage( WMU_SIGNAL, m_clientID, (LPARAM) &err );
}

void CController::ErrorHandler( BOOL threaded )
{
	if( threaded )
		SendError();
	else
		m_adapter->OnError( &err );
}