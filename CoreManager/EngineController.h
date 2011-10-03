#pragma once


template<class TMgrRegister>
class CEngineControllerT :
	public CMessageFilter
{
public:
	CEngineControllerT()
	{
		m_managers = new TMgrRegister;
		ATLVERIFY(m_managers);

		_Module.GetMessageLoop()->AddMessageFilter(this);
	}
	~CEngineControllerT()
	{
		delete m_managers;
	}

// Data members
protected:
	TMgrRegister* m_managers;

// Overrides
private:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}
};