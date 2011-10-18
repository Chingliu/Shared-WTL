#pragma once

#include "callback.h"
#include "WorkError.h"


using util::Callback;

class CWorker
{
public:
	CWorker();
	
// Data members
protected:
	static						DWORD g_dwMainThread;	//global ID of the main thread
	static __declspec(thread)	bool g_bRunGuard;		//flags if running thread is Work guarded

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
};
