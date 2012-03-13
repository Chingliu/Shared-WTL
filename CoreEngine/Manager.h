#pragma once

#include "Worker.h"
#include "Adapter.h"


// Signal message
#define WMU_SIGNAL_SEND (WM_APP + 0)
#define WMU_SIGNAL_POST (WM_APP + 1)


// Manager base - standard work interface
class CManager
	: public CWorker
{
public:
	CManager();

// Interface
protected:
	static void AssureThreadUI();	 //is DirectWork() or direct call from the main thread
	static void AssureGuardedWork(); //is running through the Work interface
};


// Manager linked
template<class TApdater>
class CManagerLinked // links with a single adapter
	: public CManager, public CAdapterLink
{
protected:
	TApdater* m_linked_adapter;

// Interface
protected:
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
protected:
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
