#pragma once

#include "Controller.h"


class CEngineController
{
public:
	CEngineController( CWindow* GUIhost );

// Data members
private:
	CAtlArray<CController*> m_clients;
	CWindow* m_GUIhost;

// Interface
public:
	void RegisterController( CController* client );

// Maps
public:
	BEGIN_MSG_MAP(CEngineController)
		MESSAGE_HANDLER(WMU_SIGNAL, SignalMessage)
	END_MSG_MAP()

// Handler prototypes
public:
	LRESULT SignalMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
};
