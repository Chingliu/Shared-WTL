#pragma once


struct CWorkErrorData
{
	// Acess members -------------------------------------------------------------
	CString log_msg;	// importance level: 1
	CString log_descr;	// importance level: 2
};


class CWorkError
	: public CWorkErrorData
{
public:
	CWorkError();

	class CWorkException		// to be mantained inside here for better recognization, 
		: public CWorkErrorData // mainly cause CWorkError exception is directly thrown from CWorkError
	{					
	public:
		CWorkException(const CWorkError* err) : CWorkErrorData(*err) {}
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
	// Interface functions -------------------------------------------------------
	void ResetMessages();
	inline void ThrowError();

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
