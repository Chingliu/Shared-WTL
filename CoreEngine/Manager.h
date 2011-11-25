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


// Manager linked - links with a single adapter
template<class TApdater>
class CManagerLinked
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



// ---------------------------------------------------------------------------------------------------------

// What if we allow non-managers classes to also be able to have adapters?
class CAdapterLink
{
// Local interface
protected:
	void SendSignal();
	void PostSignal();
};
