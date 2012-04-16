#pragma once


namespace Exc
{
	class MonitorRaw
	{
	protected:
		DWORD m_pOpaque[2]; // impl details
		static EXCEPTION_DISPOSITION HandlerStat(EXCEPTION_RECORD* pExc, PVOID, CONTEXT* pCpuCtx);

	public:

		MonitorRaw();

		virtual bool Handle(EXCEPTION_RECORD* pExc, CONTEXT* pCpuCtx) { return false; }
		virtual void AbnormalExit() {}

		void NormalExit(); // *MUTS* be called upon normal exit!!!
	};


	struct Monitor :public MonitorRaw
	{
		// Automatically handles scope exit
		~Monitor();
	};

}