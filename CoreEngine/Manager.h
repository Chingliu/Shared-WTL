#pragma once

#include "Worker.h"
#include "Log.h"
#include "Adapter.h"


class CManager
	: public CWorker
{
public:
	CManager();

// Interface
public:
	void AddAdapter(CAdapter* adapter);

	static void AssureThreadUI();	 //is DirectWork() or direct call from the main thread
	static void AssureGuardedWork(); //is running through the Work interface
};
