#include "stdafx.h"
#include "WorkError.h"

#include <comdef.h>


CWorkError::CWorkError()
{
	hr_handler.m_errhost = this;
	win_handler.m_errhost = this;
	bool_handler.m_errhost = this;
}

void CWorkError::ThrowError()
{
	ASSERT(false);
	throw CWorkException(this);
}

void CWorkError::WinHandler( bool res )
{
	if( !res )
		ThrowError();
}

void CWorkError::NormHandler( bool res )
{
	if( !res )
		ThrowError();
}