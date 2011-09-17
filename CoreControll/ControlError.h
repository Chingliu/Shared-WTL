#pragma once


class CControlError
{
public:
	CControlError( CString clsname );

// Data members
private:
	CString m_clsname;
	BOOL doreport;

// Function members
private:
	void WinError();
	void WinHandler( BOOL res );
	void NormHandler( BOOL res );

private:
	class OperatorHandler
	{
		friend CControlError;
		CControlError* m_errhost;
	};

// Error handling
public:
	// Interface
	void ThrowReport();
	void NoReport();
	
	// Acess members
	CString log_msg;
	CString log_descr;
	CString win_logmsg;

	BOOL win_error;
	DWORD win_code;

	// Windows Errors
	class : public OperatorHandler
	{
	public:
		// Error operators
		void operator =(BOOL res) { m_errhost->WinHandler(res); };		//removi return BOOL
		void operator =(void* res) { m_errhost->WinHandler(res!=0); };	//removi return void*
	} win_handler;

	// Boolean Errors
	class : public OperatorHandler
	{
	public:
		// Error operators
		void operator =(BOOL res) { m_errhost->NormHandler(res); };
		void operator =(void* res) { m_errhost->NormHandler(res!=0); };
	} bool_handler;
};


class CErrExcept
{
};


class CAutoReport
{
public:
	CAutoReport( const CAutoReport& ) {};
	CAutoReport( CControlError* targ ) : m_targ( targ ) {};
	~CAutoReport() { m_targ->ThrowReport(); };

// Data members
private:
	CControlError* m_targ;
};
