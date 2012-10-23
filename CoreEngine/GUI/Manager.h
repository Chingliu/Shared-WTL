#pragma once

#include "Worker.h"
#include "Adapter.h"
#include "EngineController.h"


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
		if( ::GetCurrentThreadId() != CWorker::g_dwMainThread )
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
		if( ::GetCurrentThreadId() != CWorker::g_dwMainThread )
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
		CEngineSignalWnd::s_msgwnd.SendMessage( WMU_SIGNAL_SEND, (WPARAM) this );// here the pointer type is correct, so no static_cast<CManagerSignaler*>(this)
	}

	void PostEngineSignal()// NYI
	{
		ASSERT( false );
		CEngineSignalWnd::s_msgwnd.PostMessage( WMU_SIGNAL_POST );// yes, its survives system inner-loops
	}

public:
	// Derived receives the signal through this pure virtual
	virtual void OnSignal() = 0;
};
