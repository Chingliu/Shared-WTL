#pragma once


namespace Err
{
	struct sErrorData
	{
		CString log_msg;	// primary importance
		CString log_descr;	// secondary importance
	};


	class CException
	{
	protected:
		CException( sErrorData& data ) : err_data(data), log_msg(data.log_msg), log_descr(data.log_descr) {}
	public:
		sErrorData& err_data;
		CString& log_msg;
		CString& log_descr;
	};

	class CExceptionWorker : public CException
	{
	public:
		CExceptionWorker( sErrorData& data ) : CException( data ) {}
	};

	class CExceptionScoped : public CException
	{
	public:
		sErrorData m_data;
		CExceptionScoped( sErrorData& data ) : m_data( data ), CException( m_data ) {}
	};


	template<typename T>
	class CErrorOperator : public sErrorData
	{
	protected:
		CErrorOperator();// intances not allowed
		DWORD m_win_code;

	// Function members
	private:
		void WinHandler( bool res );
		void NormHandler( bool res );

		class OperatorHandler
		{
			friend CErrorOperator;
			CErrorOperator* m_errhost;
		};

	// Interface
	public:
		void ResetMessages();// for clearing a previous error content

		// ------------------------------------------------------------------

		// >>> Win32 errors
		class : public OperatorHandler // HRESULT
		{
		public:
			void operator =(HRESULT res) { m_errhost->WinHandler(SUCCEEDED(res)); }
		} hr_handler;

		class : public OperatorHandler // BOOL
		{
		public:
			void operator =(BOOL res)	{ m_errhost->WinHandler(res!=FALSE); }
			void operator =(void* res)	{ m_errhost->WinHandler(res!=nullptr); }
		} win_handler;

		// >>> Normal errors
		class : public OperatorHandler // (true/false)
		{
		public:
			void operator =(bool res)	{ m_errhost->NormHandler(res); }
			void operator =(void* res)	{ m_errhost->NormHandler(res!=nullptr); }
		} bool_handler;
	};


	class CWorkerOpError : public CErrorOperator<CWorkerOpError>
	{
	public:
		void ThrowError();
	};

	class CEngineOpError : public CErrorOperator<CEngineOpError>
	{
	public:
		void ThrowError();
	};
}