#include "stdafx.h"
#include "Log.h"


namespace Log
{
	void Printer::SetPrefix( PCTSTR pstr )	{ m_prefix = pstr; }
	void Printer::SetImmediateLog( bool b )	{ m_immediate = b; }
	void Printer::SetPrinterFilter( TFPrinterCbk filter ) { m_filter = filter; }

	Printer::Printer()
	{
		m_immediate = false;
		m_filter = util::NullCallback();
	}

	void Printer::ImmediateLogpoint(Workpoint* pPoint)
	{
		if( m_immediate )
			PrintF_Output(pPoint->m_szFmt, pPoint->m_pParams, CHECKPOINT);
	}

	void Printer::PrintF_Output(PCTSTR szFmt, const int* pParams, EOutputType type, CString& out_line)
	{
		CString strline;
		strline.FormatV(szFmt, (va_list) pParams);

		bool append = true;
		if( m_filter )
			append = m_filter(strline, type);

		if( append )
			out_line = strline;
	}


	void SetGlobalOutput(TFOutputCbk cbk)
	{
		s_output = cbk;
	}
	void OutputStackPoints()
	{
		// fill a list with log lines
		CAtlArray<Log::LogLine> list;
		auto lAddLine = [&list]() -> CString& {
			Log::LogLine& line = list[list.Add()];
			line.type = Log::CHECKPOINT;
			return line.linestr;
		};

		for( const Workpoint* pPoint = Log::s_pTop; pPoint; pPoint = pPoint->m_pPrev )
			pPoint->m_pPrinter.PrintF_Output( pPoint->m_szFmt, pPoint->m_pParams, Log::CHECKPOINT, lAddLine() );

		// outputs list
		ENSURE(Log::s_output);
		s_output(list, Log::CHECKPOINT);
	}


	__declspec(thread) Workpoint* s_pTop = nullptr;
	TFOutputCbk s_output = util::NullCallback();

	void PrintF(PCTSTR szFmt, const int* pParams)// used in PUTLOG
	{
		//ENSURE(Log::s_pPrinter);
		//Log::s_pPrinter->PrintF_Internal(szFmt, pParams, Log::PUTLOG);
	}

} // namespace Log
