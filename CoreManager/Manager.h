#pragma once

#include "callback.h"
#include "Adapter.h"


using util::Callback;

class CManager
{
public:
	CManager();
	
// Data members
private:
	HANDLE m_hThread;
	DWORD m_ticktime;
	Callback<void()> m_single_cbk;

// Function members
private:
	void Run();

// Work process interface
protected:
	void DirectWork( Callback<void()> runcbk );
	void ThreadWork( Callback<void()> runcbk );
	void MinWait( DWORD ms ); //call it at end of the callback

// Interface
public:
	void AddAdapter(CAdapter* adapter);
};
