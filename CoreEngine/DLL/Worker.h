#pragma once

#include "WorkError.h"

#include "../util_excpt.h"
#include "../util_callback.h"


/*
Generic base class for worker classes by providing:
- error handling mechanism
- guarded execution to trap exceptions triggered by the error mechanism
*/
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
	util::Callback<void()> m_single_cbk;

// Function members
private:
	void Run();
	void WorkGuard();

// Interface - work process
protected:
	CWorkError err;

	void DirectWork( util::Callback<void()> runcbk );
	void ThreadWork( util::Callback<void()> runcbk );
	void MinWait( DWORD ms ); //call it at end of the callback

// Interface - overridable
protected:
	virtual void OnErrorReport() {}
};
