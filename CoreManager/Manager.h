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
	static DWORD m_dwUIthread;

	HANDLE m_hThread;
	DWORD m_ticktime;
	Callback<void()> m_single_cbk;

// Function members
private:
	void Run();

// Work process interface
protected:
	CWorkError err;

	void DirectWork( Callback<void()> runcbk );
	void ThreadWork( Callback<void()> runcbk );
	void MinWait( DWORD ms ); //call it at end of the callback

// Interface
public:
	void AddAdapter(CAdapter* adapter);
	void ASSERT_IsThreadUI();
	void ASSERT_IsGuarded();
};
