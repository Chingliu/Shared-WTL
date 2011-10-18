#pragma once

#include "Adapter.h"
#include "Worker.h"


// Mensagem de sinalização
#define WMU_SIGNAL (WM_APP + 0)


class CController
	: public CWorker
{
public:
	CController( CAdapter* adapter, PCTSTR classInfo );
	friend class CEngineController;

// Data members
private:
	CAdapter* m_adapter;
	CWindow m_msgHost;
	INT_PTR m_clientID;

// Signal
protected:
	void SendSignal();
	void SendReturn();
	void SendError();

// Overrides
protected:
	virtual void SignalReceive() {};
	virtual void ErrorHandler( BOOL threaded );
};
