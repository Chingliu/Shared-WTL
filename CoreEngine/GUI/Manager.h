#pragma once

#include "Worker.h"
#include "Adapter.h"


// Signal message
#define WMU_SIGNAL_SEND (WM_APP + 0)
#define WMU_SIGNAL_POST (WM_APP + 1)


// Manager base - standard work interface with no adapter
class CManager
	: public CWorker
{
protected:
	CManager();
	bool PrinterFilter(Log::LogLine&);

// Interface static so these checks can be made at ANY place
protected:
	static void AssureThreadUI();		//is DirectWork() or direct call from the main thread
	static void AssureWorkGuarded();	//is running through the Worker interface
	static void AssureWorkThreaded();	//is running through a Worker thread

// Overrides
protected:
	virtual void OnErrorReport( const CWorkErrorData& source_err, Log::LogList& chks_list ) override;
};


// Manager linked
template<class TApdater>
class CManagerLinked // links with a single adapter
	: public CManager, public CAdapterLink
{
protected:
	TApdater* m_linked_adapter;

// Interface
public:
	void SetAdapter(TApdater* adapter)
	{
		m_linked_adapter = adapter;
	}
};

template<class TApdater>
class CManagerLinkedN // links with any number of adapters
	: public CManager, public CAdapterLink
{
protected:
	CAtlArray<TApdater*> m_linked_adapters;
	
// Interface
public:
	void AddAdapter(TApdater* adapter)
	{
		m_linked_adapters.Add(adapter);
	}
};


// ---------------------------------------------------------------------------------------------------------

// What if we allow non-managers classes to also be able to have adapters?
class CAdapterLink// why not have this interface directly into CManager?
{
// Local interface
protected:
	void SendSignal();
	void PostSignal();
};
