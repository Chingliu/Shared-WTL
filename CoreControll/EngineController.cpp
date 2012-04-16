#include "stdafx.h"
#include "EngineController.h"


CEngineController::CEngineController( CWindow* GUIhost )
{
	m_GUIhost = GUIhost;
}

void CEngineController::RegisterController( CController* client )
{
	ATLASSERT( m_GUIhost->IsWindow() );
	client->m_msgHost = *m_GUIhost;
	client->m_clientID = m_clients.Add( client );
}

LRESULT CEngineController::SignalMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	CController* control = m_clients.GetAt( wParam );
	CControlError* err = (CControlError*) lParam;

	if( err )
	{
		control->m_adapter->SignalError( err );
	} else {
		control->m_adapter->SignalReceive();
		control->SignalReceive();
	}

	return TRUE;
}