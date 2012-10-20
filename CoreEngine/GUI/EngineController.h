#pragma once

#define WMU_SIGNAL_SEND (WM_APP + 0)
#define WMU_SIGNAL_POST (WM_APP + 1)


class CEngineSignalWnd
	: public CWindowImpl<CEngineSignalWnd>
{
public:
	static CEngineSignalWnd s_msgwnd;

// Maps
public:
	BEGIN_MSG_MAP(CMsgWindow)
		MESSAGE_HANDLER_EX(WMU_SIGNAL_SEND, OnSignalSend)
		MESSAGE_HANDLER_EX(WMU_SIGNAL_POST, OnSignalPost)
	END_MSG_MAP()

// Handler prototypes
protected:
	LRESULT OnSignalSend(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSignalPost(UINT uMsg, WPARAM wParam, LPARAM lParam);
};


template<class TMixIn, class TRegister>
class CEngineControllerT
{
public:
	CEngineControllerT()
	{
		ENSURE(CEngineSignalWnd::s_msgwnd.Create(HWND_MESSAGE));
		ENSURE(m_managers = new TRegister );

		/*CWorkError err;
		err.log_msg = L"Couldn't allocate managers. Out of memory!";
		err.bool_handler = m_managers;*/
	}
	~CEngineControllerT()
	{
		delete m_managers;
		CEngineSignalWnd::s_msgwnd.DestroyWindow();
	}

// Data members
protected:
	TRegister* m_managers;
};
