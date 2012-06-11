#pragma once


class CWorkError
{
public:
	CWorkError();

	class CWorkException
	{
	public:
		CWorkException(CWorkError* err) : perr(err) {}
		CWorkError* perr;
	};

// Function members
private:
	void WinHandler( bool res );
	void NormHandler( bool res );

// Error handling
private:
	class OperatorHandler
	{
		friend CWorkError;
		CWorkError* m_errhost;
	};

public:
	// Interface -----------------------------------------------------------------
	inline void ThrowError();

	// Acess members -------------------------------------------------------------
	CString log_msg;	// importance level: 1
	CString log_descr;	// importance level: 2

	// Handlers ------------------------------------------------------------------
	// >>> Win32 errors
	class : public OperatorHandler // HRESULT
	{
	public:
		void operator =(HRESULT res) { m_errhost->WinHandler(SUCCEEDED(res)); }
	} hr_handler;

	class : public OperatorHandler // BOOL
	{
	public:
		void operator =(BOOL res)	{ m_errhost->WinHandler(res!=FALSE); } //there can be functions which do not return exactly TRUE, but a number > TRUE, probably when returning a DWORD
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
