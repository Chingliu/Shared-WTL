#pragma once

#include "Excpt\Excpt.h"
#include "Manager.h"


template<class TMixIn, class TRegister>
class CEngineControllerT :
	public CMessageFilter
{
public:
	CEngineControllerT()
	{
		m_managers = new TRegister;
		ATLVERIFY(m_managers);

		_Module.GetMessageLoop()->AddMessageFilter(this);
		//Exc::SetThrowFunction(true);
		//Exc::SetFrameHandler(true);
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
