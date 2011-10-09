#pragma once

#include "callback.h"
#include "Adapter.h"
#include "WorkError.h"
#include "Log.h"


using util::Callback;

class CManager
{
public:
	CManager();
	
// Data members
private:
	static						DWORD g_dwThreadUI; //global ID of the main thread
	static __declspec(thread)	bool g_bRunGuard;	//flags if running thread is Work guarded

	HANDLE m_hThread;
	DWORD m_ticktime;
	Callback<void()> m_single_cbk;

// Function members
private:
	void Run();
	void WorkGuard();

// Work process interface
protected:
	CWorkError err;

	void DirectWork( Callback<void()> runcbk );
	void ThreadWork( Callback<void()> runcbk );
	void MinWait( DWORD ms ); //call it at end of the callback

// Interface
public:
	void AddAdapter(CAdapter* adapter);

	static void AssureThreadUI();	 //is DirectWork() or direct call from the main thread
	static void AssureGuardedWork(); //is running through the Work interface
};
