#pragma once


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
	}
	~CEngineControllerT()
	{
		delete m_managers;
	}

// Data members
protected:
	TRegister* m_managers;
	//TMixIn*

// Overrides
private:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if( pMsg->hwnd==_wndMain )
		{
			if( pMsg->message==WMU_SIGNAL_SEND )
			{
				::SetEvent( (HANDLE) pMsg->wParam );
			}/* else if( pMsg->message==WMU_SIGNAL_POST ) {

			}*/
		}
		return FALSE;
	}
};
