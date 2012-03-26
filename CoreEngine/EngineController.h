#pragma once

#include "Manager.h"


template<class TMixIn, class TRegister>
class CEngineControllerT :
	public CMessageFilter
{
public:
	CEngineControllerT()
	{
		m_managers = new TRegister;
		ATLENSURE(m_managers);

		_Module.GetMessageLoop()->AddMessageFilter(this);
	}
	~CEngineControllerT()
	{
		delete m_managers;
	}

// Data members
protected:
	TRegister* m_managers;

// Overrides
private:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if( pMsg->hwnd==_wndMain && pMsg->message==WMU_SIGNAL_SEND )
		{
			::SetEvent( (HANDLE) pMsg->wParam );
		}

		if( pMsg->hwnd==_wndMain && pMsg->message==WMU_SIGNAL_POST )
		{
		}

		return FALSE;
	}
};
