#include "stdafx.h"
#include "Worker.h"
#include "Error.h"

#include <comdef.h>


namespace Err
{
	template<typename T>
	CErrorOperator<T>::CErrorOperator()
	{
		hr_handler.m_errhost = this;
		win_handler.m_errhost = this;
		bool_handler.m_errhost = this;
	}

	template<typename T>
	void CErrorOperator<T>::ResetMessages()
	{
		m_win_code = 0;
		log_msg.Empty();
		log_descr.Empty();
	}

	template<typename T>
	void CErrorOperator<T>::WinHandler( bool res )
	{
		ASSERT( CWorker::IsWorkingGuard() );// class should be only used in a 'guarded' thread stack, TODO: write-up a note in what situation its beeing used this way

		if( !res )
		{
			// TBD
			//m_win_code = ::GetLastError();
			//win_logmsg = _com_error(m_win_code).ErrorMessage();
			static_cast<T*>(this)->ThrowError();
		}
	}

	template<typename T>
	void CErrorOperator<T>::NormHandler( bool res )
	{
		ASSERT( CWorker::IsWorkingGuard() );// class should be only used in a 'guarded' thread stack, TODO: write-up a note in what situation its beeing used this way

		if( !res )
			static_cast<T*>(this)->ThrowError();
	}


	void CWorkerOpError::ThrowError()
	{
		throw CExceptionWorker( *this );
	}

	void CEngineOpError::ThrowError()
	{
		throw CExceptionScoped( *this );
	}

	template CErrorOperator<CWorkerOpError>;
	template CErrorOperator<CEngineOpError>;
}
