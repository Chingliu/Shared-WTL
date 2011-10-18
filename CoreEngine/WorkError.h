#pragma once


class CWorkError
{
public:
	CWorkError();

// Data members
private:

// Function members
private:
	void WinHandler( bool res );
	void NormHandler( bool res );

private:
	class OperatorHandler
	{
		friend CWorkError;
		CWorkError* m_errhost;
	};

// Error handling
public:
	// Interface -----------------------------------------------------------------
	// Acess members -------------------------------------------------------------
	// Handlers ------------------------------------------------------------------
	class : public OperatorHandler
	{
	public:
		void operator =(HRESULT res) { m_errhost->WinHandler(SUCCEEDED(res)); }
	} hr_handler;

	// Win32 Errors (BOOL)
	class : public OperatorHandler
	{
	public:
		// Error operators
		void operator =(BOOL res)	{ m_errhost->WinHandler(res==TRUE); }		//removi return BOOL
		void operator =(void* res)	{ m_errhost->WinHandler(res!=nullptr); }	//removi return void*
	} win_handler;

	// Boolean Errors (true/false)
	class : public OperatorHandler
	{
	public:
		// Error operators
		void operator =(bool res)	{ m_errhost->NormHandler(res); }
		void operator =(void* res)	{ m_errhost->NormHandler(res!=nullptr); }
	} bool_handler;
};
