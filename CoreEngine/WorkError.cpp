#include "stdafx.h"
#include "WorkError.h"


CWorkError::CWorkError()
{
	hr_handler.m_errhost = this;
	win_handler.m_errhost = this;
	bool_handler.m_errhost = this;
}

void CWorkError::WinHandler( bool res )
{
}

void CWorkError::NormHandler( bool res )
{
}