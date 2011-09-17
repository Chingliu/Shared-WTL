#pragma once

#include "ToolsSystem\Thread.h"
#include "ControlError.h"


class CWorker
	: private CThreadImpl<CWorker>
{
public:
	CWorker( PCTSTR classInfo );

// Data members
private:
	DWORD m_ticktime;

// Function members
private:
	DWORD Run();
	void Work( BOOL threaded );

// Work process
protected:
	void ThreadWork();
	void DirectWork();
	void MinWait( DWORD min );

// Error handling
public:
	CControlError err;

// Overrides
protected:
	virtual void WorkCallback() = 0;
	virtual void ErrorHandler( BOOL threaded ) = 0;
};
