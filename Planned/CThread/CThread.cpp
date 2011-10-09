////////////////////////////////////////
// CThread.cpp:	C++ source file for the CThread class
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
//
////////////////////////////////////////

#include "stdafx.h"
#include "CThread.h"

////////////////////////////////////////
// Class CThread implementation
////////////////////////////////////////

//
// Constructor
//

CThread::CThread()
{
	TRACE("CThread::CThread\n");
	m_hThread = NULL;
	m_uThreadId = 0;
	m_cThunk.Setup(this, ThreadProc);
}

//
// Destructor
//

CThread::~CThread()
{
	TRACE("CThread::~CThread\n");
	// If this destructor is called in the context of the "parent" thread then it will
	// ensure that the "child" thread is terminated by calling EndThread.
	// However, it is possible to use a design whereby responsibility for the CThread
	// object is passed to the "child" thread, so that this destructor will be called
	// in the context of the "child" thread. In that case we must not call EndThread,
	// since that will cause the thread to hang waiting for itself to die (in DEBUG
	// versions) or to forcibly kill itself (in RELEASE versions) rather than the
	// desired behaviour which would be for the ThreadProc to gracefully exit sometime
	// after the CThread destructor has finished executing.
	if (m_hThread != NULL)
	{
		if (m_uThreadId != ::GetCurrentThreadId())
		{
			// We are running in the context of the "parent" (or other non-child) thread.
			// Ensure the "child" thread terminated properly
			DWORD dwExitCode;
			VERIFY(EndThread(1000L, dwExitCode));
		}
		else
		{
			// We are running in the context of the "child" thread.
			// Just close the handle - to prevent a resource leak - this does
			// not affect the running of the thread
			VERIFY(::CloseHandle(m_hThread));		// release the handle
		}
	} 
}

//
// StartThread
//

bool CThread::StartThread(void *pParms_)
{
	TRACE("CThread::StartThread\n");
	bool rc = true;

	if (m_hThread != NULL)
	{
		rc = false;
	}

	if (rc)
	{
		// If the following line produces a compiler error ("'_beginthreadex' : undeclared identifier")
		// then you probably need to change your project to a multi-threaded model in
		// <Projects/Settings/C/C++/Code Generation/Use run-time library>.
		m_hThread = (HANDLE) _beginthreadex(
				NULL,
				0,
				(unsigned int (__stdcall *)(void *pParms_)) &m_cThunk,
				NULL,
				0U,
				&m_uThreadId);
		if (m_hThread == NULL)
		{
			ASSERT(false);
			rc = false;
		}
		else
		{
			TRACE("CThread::StartThread: new thread handle: 0x%.04X; ID: 0x%.04X\n",
					m_hThread, m_uThreadId);
		}
	}

	return rc;
} 

//
// EndThread:	monitor the ending of the thread. Called when the thread is expected to die,
//				to make sure that it does, to check its return code and close the handle.
//				If it doesn't die within a certain time, we will terminate the thread
//				forcibly. Although this is "dangerous" according to Microsoft, it is
//				less dangerous than allowing the thread to go on running when resources
//				upon which it relies may well be getting released...
//
// Parameters:	dwWaitMillisecs_: time to wait for thread to die of its own accord
//								before killing it, or INFINITE if there is to be no time limit.
//				uThreadExitCode_ [out]: the exit code returned by the thread, if available 
//				returns:		true if the thread has terminated of its own accord and the
//								thread exit code has been placed in uThreadExitCode.
//								false otherwise.
//
// Note:		in a DEBUG build, we will never kill the thread forcibly, on the assumption
//				that the thread may have hit an ASSERT or be at a breakpoint in the debugger.
//				

bool CThread::EndThread(DWORD dwWaitMillisecs_, DWORD &dwThreadExitCode_)
{
#ifdef _DEBUG
	if (dwWaitMillisecs_ == INFINITE) dwWaitMillisecs_ = 10000UL;
										// looping 10 sec waits will allow me when debugging
										// to put in a breakpoint and force the thread
										// to be killed if I so wish
	bool bForever = true;				// set this to "false" in the debugger to force end thread
										// otherwise we will wait forever, 'cos the child thread
										// might be held up at a debugger breakpoint
tryagain:
#endif // #ifdef _DEBUG

	bool rc = false;					// guilty until proved innocent!
	bool bThreadEnded = false;			// keep track of whether it has ended or not

	if (m_hThread != NULL)
	{
		DWORD dwErr = 0;
		DWORD dwRc = ::WaitForSingleObject(m_hThread, dwWaitMillisecs_);
		switch (dwRc)
		{
		case WAIT_OBJECT_0:		// the thread has died as it ought
			bThreadEnded = true;			// remember that it has ended
			if (!::GetExitCodeThread(m_hThread, &dwThreadExitCode_))
			{
				// failed to get exit code
				dwErr = ::GetLastError();
				TRACE("CThread::EndThread: GetExitCodeThread error %lu\n", dwErr);
				ASSERT(false);				// this should never happen
			}
			else
			{
				// got exit code - return it to caller
				TRACE("CThread::EndThread: thread 0x%.02X exit code %ld\n",
						m_uThreadId, dwThreadExitCode_);
				rc = true;					// success!
			}
			break;

		case WAIT_FAILED:		// the WaitForSingleObject has failed
								// and will have set Last Error
			dwErr = GetLastError();
			TRACE("CThread::EndThread: wait failed with error code %ld\n", dwErr);
			ASSERT(false);		// this should never happen
			break;

		default:				// unexpected return from WaitForSingleObject
			TRACE("CThread::EndThread: unexpected wait return code %lu\n", dwRc);
			ASSERT(false);		// this should never happen
			break;

		case WAIT_TIMEOUT:		// the thread is refusing to die
			TRACE("CThread::EndThread: thread 0x%.02X is refusing to die...\n",
					m_uThreadId);
#ifdef _DEBUG
			if (bForever) goto tryagain;
#endif // #ifdef _DEBUG
			break;
		}
		if (!bThreadEnded)
		{
			TRACE("CThread::EndThread: terminating thread forcibly...\n");
			VERIFY(::TerminateThread(m_hThread, 2));	// "a dangerous function"
		}

		VERIFY(::CloseHandle(m_hThread));		// release the handle
		m_hThread = NULL;
		m_uThreadId = 0;
	}
	return rc;
}

/* static */ bool CThread::EndMultipleThreads(DWORD dwWaitMillisecs_, int nThreads_,
		SEndThread aEndThreads_[])
{
	bool bRc = true;					// innocent till proved guilty
	bool bAbort = false;

	DWORD dwStartTime = ::GetTickCount();
	DWORD dwWaitRemaining = dwWaitMillisecs_;

	int *aThreadIndices = new int[nThreads_];	// index position of each thread in pThreads_, pResultCodes_
												// and pThreadExitCodes_
	HANDLE *aThreadHandles = new HANDLE[nThreads_];	// positions correspond to those in aThreadStatii

	// Initialise our control arrays and output result code array
	for (int i = 0; i < nThreads_; i += 1)
	{
		aEndThreads_[i].bResultCode = false;		// guilty till proved innocent
		aThreadIndices[i] = i;					// initial mapping is one-to-one
		aThreadHandles[i] = aEndThreads_[i].pThread->m_hThread;
	}

	// Weed out any threads which are already known not to be running
	// The arrays are left-shunted as necessary to eliminate the entries to be weeded out
	int nActiveThreads = 0;
	for (i = 0; i < nThreads_; i += 1)
	{
		if (aThreadHandles[i])
		{
			aThreadIndices[nActiveThreads] = aThreadIndices[i];
			aThreadHandles[nActiveThreads++] = aThreadHandles[i];
		}
	}

	while (nActiveThreads && !bAbort)
	{
#ifdef _DEBUG
		if (dwWaitMillisecs_ == INFINITE) dwWaitRemaining = 10000UL;
										// looping 10 sec waits will allow me when debugging
										// to put in a breakpoint and force the thread
										// to be killed if I so wish
		bool bForever = true;			// set this to "false" in the debugger to force end thread
										// otherwise we will wait forever, 'cos the child thread
										// might be held up at a debugger breakpoint
	tryagain:
#endif // #ifdef _DEBUG

		bool bThreadEnded = false;			// keep track of whether it has ended or not

		DWORD dwErr = 0;
		DWORD dwRc = ::WaitForMultipleObjects(nActiveThreads, aThreadHandles, FALSE, dwWaitRemaining);
		switch (dwRc)
		{
		case WAIT_FAILED:		// the WaitForMultipleObjects has failed
								// and will have set Last Error
			dwErr = GetLastError();
			TRACE("CThread::EndThread: wait failed with error code %ld\n", dwErr);
			ASSERT(false);		// this should never happen
			bAbort = true;		// I give up
			bRc = false;		// return an error
			break;

		case WAIT_TIMEOUT:		// the thread is refusing to die
			TRACE("CThread::EndThread: one or more threads refusing to die...\n");
#ifdef _DEBUG
			if (bForever) goto tryagain;
#endif // #ifdef _DEBUG
			bAbort = true;		// I give up
			bRc = false;		// return an error
			break;

		default:						// check which thread has ended
			int nThread; 
			if ((nThread = dwRc - WAIT_OBJECT_0) >= 0 && nThread < nActiveThreads)
			{
				// We have detected the ending of a thread
				int nIndex = aThreadIndices[nThread];
				SEndThread &sEndThread = aEndThreads_[nIndex];
				DWORD &dwThreadExitCode = sEndThread.dwThreadExitCode;
				CThread *pThread = sEndThread.pThread;
				if (!::GetExitCodeThread(pThread->m_hThread, &dwThreadExitCode))
				{
					// failed to get exit code
					dwErr = ::GetLastError();
					TRACE("CThread::EndThread: GetExitCodeThread error %lu\n", dwErr);
					bRc = false;				// return an error (but don't abort)
					ASSERT(false);				// this should never happen
				}
				else
				{
					// got exit code - return it to caller
					TRACE("CThread::EndThread: thread 0x%.02X exit code %ld\n",
							pThread->m_uThreadId, dwThreadExitCode);
					sEndThread.bResultCode = true;					// success!
				}

				// Close the windows thread handle and tidy up the CThread object
				VERIFY(::CloseHandle(pThread->m_hThread));
				pThread->m_hThread = NULL;
				pThread->m_uThreadId = 0;

				// Delete this thread from our arrays and shunt over
				nActiveThreads -= 1;
				while (nThread < nActiveThreads)
				{
					aThreadIndices[nThread] = aThreadIndices[nThread + 1];
					aThreadHandles[nThread] = aThreadHandles[nThread + 1];
					nThread += 1;
				}
			}
			else
			{
				TRACE("CThread::EndThread: unexpected wait return code %lu\n", dwRc);
				ASSERT(false);		// this should never happen
				bAbort = true;		// I give up
				bRc = false;		// return an error
			}
			break;
		}
		if (!bAbort)
		{
			DWORD dwElapsed = ::GetTickCount() - dwStartTime;
			if (dwWaitMillisecs_ != INFINITE)
			{
				if (dwWaitRemaining <= dwElapsed)
					break;
				else
					dwWaitRemaining -= dwElapsed;
			}
		}
	}

	// Any threads which have not ended of their own free will will now be forcibly terminated
	while(nActiveThreads--)
	{
		CThread *pThread = aEndThreads_[aThreadIndices[nActiveThreads]].pThread;
		TRACE("CThread::EndThread: terminating thread 0x%.02X forcibly...\n", pThread->m_uThreadId);
		if (!::TerminateThread(pThread->m_hThread, 2))	// "a dangerous function"
		{
			DWORD dwErr = ::GetLastError();
			TRACE("CThread::EndThread: TerminateThread error %lu\n", dwErr);
			bRc = false;				// return an error
			ASSERT(false);				// this should never happen
		}
		else
		{
			// Close the windows thread handle and tidy up the CThread object
			VERIFY(::CloseHandle(pThread->m_hThread));
			pThread->m_hThread = NULL;
			pThread->m_uThreadId = 0;
		}
	}

	delete[] aThreadIndices;
	delete[] aThreadHandles;

	return bRc;
}

