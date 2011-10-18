#include "stdafx.h"
#include "ControlError.h"

#include <comdef.h>


CControlError::CControlError( CString clsname )
{
	m_clsname = clsname;
	doreport = TRUE;

	win_error = false;
	win_handler.m_errhost = this;
	bool_handler.m_errhost = this;
}

void CControlError::WinError()
{
	if( win_error )
	{
		win_code = ::GetLastError();
		win_logmsg = _com_error(win_code).ErrorMessage();
	}
}

void CControlError::WinHandler( BOOL res )
{
	if( !res )
	{
		ATLASSERT( FALSE );
		win_error = true;
		throw CAutoReport( this );
	}
}

void CControlError::NormHandler( BOOL res )
{
	if( !res )
	{
		ATLASSERT( FALSE );
		throw CAutoReport( this );
	}
}

void CControlError::ThrowReport()
{
	if( doreport )
	{
		WinError();
		throw CErrExcept();
	}
	doreport = TRUE;
}

void CControlError::NoReport()
{
	doreport = FALSE;
}
