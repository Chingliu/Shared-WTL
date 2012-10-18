#pragma once

#include "Manager.h"


template<class TMixIn, class TRegister>
class CEngineControllerT :
	public CMessageFilter
{
public:
	CEngineControllerT()
	{
		_Module.GetMessageLoop()->AddMessageFilter(this);

		m_managers = new TRegister;
		ENSURE(m_managers);

		/*CWorkError err;
		err.log_msg = L"Couldn't allocate managers. Out of memory!";
		err.bool_handler = m_managers;*/
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
		if( pMsg->hwnd==_wndMain )
		{
			if( pMsg->message==WMU_SIGNAL_SEND )
			{
				reinterpret_cast<CManagerSignaler*>(pMsg->wParam)->OnSignal();
				::SetEvent( reinterpret_cast<HANDLE>(pMsg->lParam) );
			}/* else if( pMsg->message==WMU_SIGNAL_POST ) {

			}*/
		}
		return FALSE;
	}
};
