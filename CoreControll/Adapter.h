#pragma once

#include "ControlError.h"


class CAdapter
{
// Virtual interface
public:
	virtual void SignalReceive() { ATLASSERT(FALSE); };
	virtual void SignalError( CControlError* err ) { ATLASSERT(FALSE); };
	virtual void OnError( CControlError* err ) = 0;
};
