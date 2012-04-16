#include "stdafx.h"
#include "WorkError.h"

#include <comdef.h>

#include "Log.h"
#include "../util_excpt.h"


CWorkError::CWorkError()
{
	hr_handler.m_errhost = this;
	win_handler.m_errhost = this;
	bool_handler.m_errhost = this;
}

void CWorkError::ThrowError()
{
	throw CWorkException(this);
}

void CWorkError::WinHandler( bool res )
{
	if( !res )
		throw CWorkException(this);
}

void CWorkError::NormHandler( bool res )
{
	if( !res )
		throw CWorkException(this);
}