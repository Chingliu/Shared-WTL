#pragma once


class CWorkError
{
public:
	CWorkError();

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
	// Acess members -------------------------------------------------------------
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
