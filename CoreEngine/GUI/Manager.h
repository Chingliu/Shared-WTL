#pragma once

#include "Worker.h"
#include "Adapter.h"

#define WMU_SIGNAL_SEND (WM_APP + 0)
#define WMU_SIGNAL_POST (WM_APP + 1)


// Manager base - no adapter
class CManager
	: public CWorker
{
protected:
	CManager();
	bool PrinterFilter(Log::LogLine&);

// Interface
protected:
	// static ones - these checks can be made at ANY place
	static void ASSERT_ThreadUI();		//is DirectWork() or direct call from the main thread
	static void ASSERT_WorkGuarded();	//is running through the Worker interface
	static void ASSERT_WorkThreaded();	//is running through a Worker thread

// Overrides
protected:
	virtual void OnErrorReport( const CWorkErrorData& source_err, Log::LogList& chks_list ) override;
};


// Manager linked - has adapter
template<class TAdapter>
class CManagerLinked
	: public CManager, protected CManagerSignaler
{
public:
	typedef TAdapter tdAdapter;
	TAdapter* m_linked_adapter;

private:
	typedef void(TAdapter::*tdSignalFunc)();
	tdSignalFunc m_signal_callback;

	void OnSignal() override
	{
		(m_linked_adapter->*m_signal_callback)();
	}

// Interface
public:
	void SetAdapter(TAdapter* adapter)
	{
		m_linked_adapter = adapter;
	}

	void SendSignal(tdSignalFunc func)
	{
		m_signal_callback = func;
		if( IsWorkThreaded() )
			SendEngineSignal();
		else
			ASSERT(false);// consider calling the function directly, so no SendSignal() right!
	}
};

template<class TAdapter>
class CManagerLinkedN
	: public CManager, protected CManagerSignaler
{
public:
	typedef TAdapter tdAdapter;
	CAtlArray<TAdapter*> m_linked_adapters;

private:
	typedef void(TAdapter::*tdSignalFunc)();
	tdSignalFunc m_signal_callback;
	void OnSignal() override
	{
		for( UINT i=0; i<m_linked_adapters.GetCount(); ++i )
			(m_linked_adapters[i]->*m_signal_callback)();
	}

// Interface
public:
	void AddAdapter(TAdapter* adapter)
	{
		m_linked_adapters.Add(adapter);
	}

	void SendSignal(tdSignalFunc func)
	{
		m_signal_callback = func;
		if( IsWorkThreaded() )
			SendEngineSignal();
		else
			OnSignal();
	}
};


// ---------------------------------------------------------------------------------------------------------

class CManagerSignaler // What if we allow non-managers classes to also be able to have adapters?
{
protected:
	void SendEngineSignal()
	{
		ASSERT( ::GetCurrentThreadId() != CWorker::g_dwMainThread );//only for threaded work else will cause a dead-lock!
		CHandle event_lock( ::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE | DELETE) );
		_wndMain.PostMessage( WMU_SIGNAL_SEND, (WPARAM) this, (LPARAM) (HANDLE) event_lock );// here the pointer type is correct, so no static_cast<CManagerSignaler*>(this)
		::WaitForSingleObject( event_lock, INFINITE );// not using SendMessage because it goes directly to the WinProc so can not be PreTranslated
	}

	void PostEngineSignal()// NYI
	{
		_wndMain.PostMessage( WMU_SIGNAL_POST );// no PostThreadMessage because a modal loop might cause the messages to be lost, yet not sure if this way is 100% safe
	}

public:
	// Derived receives the signal through this pure virtual
	virtual void OnSignal() = 0;
};
