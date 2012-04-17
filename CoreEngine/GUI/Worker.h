#pragma once

#include "Log.h"
#include "WorkError.h"


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
	struct WorkContext
	{
		// -bRunGuard:	flags if running from a Worker guarded callback
		// -bRunThread:	flags if its a Worker spawned thread
		bool bRunGuard;	//running thread is Work guarded
		bool bRunThread;//running thread was spawned by a Worker instance
		
		bool bVecEnter;
	};

	static __declspec(thread)	WorkContext g_threadCtx;	//__declspec(thread) may not work for dynamically loaded DLLs
	static						DWORD g_dwMainThread;		//global ID of the main thread which loaded app/module


	// context of the current thread:
	static bool IsWorking()		{ return g_threadCtx.bRunGuard; }
	static bool IsWorkThread()	{ return g_threadCtx.bRunThread; }

// Data members
private:
	DWORD m_ticktime;
	util::Callback<void()> m_single_cbk;
	bool m_thread_race;

// Function members
private:
	void Run();
	void WorkGuard();

// Interface - work process
protected:
	CWorkError err;
	Log::Printer logger;

	void DirectWork( util::Callback<void()> runcbk );
	void ThreadWork( util::Callback<void()> runcbk );
	void MinWait( DWORD ms ); //call it at end of the callback

// Interface - overridable
protected:
	virtual void OnErrorReport( CWorkError* source_err ) {}
};
