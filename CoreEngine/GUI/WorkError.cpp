#include "stdafx.h"
#include "Worker.h"
#include "WorkError.h"

#include <comdef.h>


CWorkError::CWorkError()
{
	hr_handler.m_errhost = this;
	win_handler.m_errhost = this;
	bool_handler.m_errhost = this;
}

void CWorkError::ResetMessages()
{
	log_msg.Empty();
	log_descr.Empty();
}

void CWorkError::ThrowError()
{
	ASSERT( false );
	ASSERT( CWorker::IsWorkingGuard() );// class should be only used in a 'guarded' thread stack

	throw CWorkException(this);
}

void CWorkError::WinHandler( bool res )
{
	ASSERT( CWorker::IsWorkingGuard() );// class should be only used in a 'guarded' thread stack

	if( !res )// TODO: add win-error messages
		ThrowError();
}

void CWorkError::NormHandler( bool res )
{
	ASSERT( CWorker::IsWorkingGuard() );// class should be only used in a 'guarded' thread stack

	if( !res )
		ThrowError();
}
