////////////////////////////////////////
// CThread.h:		Header file for the CThread class
//
//
// Author:			JAB
// Date:			December 2001
//
// Purpose:			Derive your class from this if you want it to run in its own thread
//
// Notes:			The StartThread and EndThread methods are to be called from the main
//					thread to control the child thread associated with the class.
//					The ThreadProc method is to be supplied in the derived class, and
//					will be invoked when StartThread is called - ThreadProc naturally will
//					run in the child thread associated with the class.
//					Note that ThreadProc is a non-static member function and comes complete
//					with a valid "this" pointer. The value which it returns can be picked
//					up by the parent thread on the EndThread call.
//					Any methods in your derived class may be called within the child
//					thread or the parent thread. It's entirely up to you to make sure that
//					this is done in a thread-safe fashion.
//					You shouldn't rely on EndThread to stop the child thread. EndThread is
//					designed to be called when you expect the child thread to stop of its
//					own accord, to make sure it does so and to get the thread exit code.
//					EndThread *will* kill the child thread if it fails to die of its own
//					accord, but the method used is described by Microsoft as "a dangerous
//					function".
////////////////////////////////////////

#pragma once

#include <process.h>
#include "CThiscallThunk.h"

//
// Class CThread
//

class CThread
{
public:
	struct SEndThread					// parameter for EndMultipleThreads
	{
		CThread *pThread;				// [in] pointer to the thread to be ended
		bool bResultCode;				// [out] true if thread shut down gracefully
										//       and the exit code was obtained
		DWORD dwThreadExitCode;			// [out] the exit code for the thread
	};

	CThread();					// constructor
	~CThread();					// destructor

	bool StartThread(void *pParms_);	// call this method to start the thread
	bool EndThread(DWORD dwWaitMillisecs_, DWORD &dwThreadExitCode_);
										// erm, call this method to, erm, end the thread
	
	static bool EndMultipleThreads(DWORD dwWaitMillisecs_, int nThreads_,
			SEndThread aEndThreads_[]);

	HANDLE m_hThread;					// the thread handle
	unsigned m_uThreadId;				// the thread ID	

protected:
	virtual unsigned ThreadProc(void *pParms_) = 0;	// derived class must supply a thread proc

	CThiscallThunk m_cThunk;			// thunk for calling the thread proc

private:
// nobbled stuff
	CThread(const CThread& rhs);			// copy constructor
	CThread& operator=(const CThread& rhs);	// assignment
};

