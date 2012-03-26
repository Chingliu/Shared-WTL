#pragma once

#include "callback.h"
#include "Log.h"
#include "WorkError.h"


using util::Callback;

class CWorker
{
public:
	CWorker();
	
// Static members
public:
	static						DWORD g_dwMainThread;	//global ID of the main thread which loaded app/module
	static __declspec(thread)	bool g_bRunGuard;		//flags if running thread is Work guarded

// Data members
protected:
	HANDLE m_hThread;
	DWORD m_ticktime;
	Callback<void()> m_single_cbk;

// Function members
private:
	void Run();
	void WorkGuard();

// Interface - work process 
protected:
	CWorkError err;

	void DirectWork( Callback<void()> runcbk );
	void ThreadWork( Callback<void()> runcbk );
	void MinWait( DWORD ms ); //call it at end of the callback
};
