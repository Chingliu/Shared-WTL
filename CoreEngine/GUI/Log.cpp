#include "stdafx.h"
#include "Log.h"


namespace Log
{
	void Printer::SetPrefix( PCTSTR pstr )	{ m_prefix = pstr; }
	void Printer::SetImmediateLog( bool b )	{ m_immediate = b; }
	void Printer::SetPrintFilter( TFPrinterCbk filter ) { m_filter = filter; }

	Printer::Printer()
	{
		m_immediate = true;
		m_filter = util::NullCallback();
	}

	void Printer::ImmediatePoint(Workpoint* pPoint)
	{
		if( m_immediate )
			OutputSingleLine(CHECKPOINT, pPoint->m_szFmt, pPoint->m_pParams);
	}
	void Printer::OutputSingleLine(EOutputType type, PCTSTR szFmt, const int* pParams)
	{
		LogLine line = { type };
		if( SPrintF_Line(line, szFmt, pParams) )
		{
			CAtlArray<Log::LogLine> list;
			list.Add(line);

			ENSURE(Log::s_output);
			s_output(list, type);
		}
	}

	bool Printer::SPrintF_Line(LogLine& out_line, PCTSTR szFmt, const int* pParams)
	{
		out_line.linestr.FormatV(szFmt, (va_list) pParams);

		if( m_filter && !m_filter(out_line) )
			return false;
		return true;
	}


	void SetGlobalOutput(TFOutputCbk cbk)
	{
		s_output = cbk;
	}
	void OutputStackPoints()
	{
		// fill a list with log lines
		CAtlArray<Log::LogLine> list;
		auto lAddLine = [&list]() -> Log::LogLine& {
			Log::LogLine& line = list[list.Add()];
			line.type = Log::CHECKPOINT;
			return line;
		};

		for( const Workpoint* pPoint = Log::s_pTop; pPoint; pPoint = pPoint->m_pPrev )
			pPoint->m_pPrinter.SPrintF_Line( lAddLine(), pPoint->m_szFmt, pPoint->m_pParams );

		// outputs list
		ENSURE(Log::s_output);
		s_output(list, Log::CHECKPOINT);
	}


// internal

	// globals
	__declspec(thread) Workpoint* s_pTop = nullptr;
	TFOutputCbk s_output = util::NullCallback();

	// used in PUTLOG
	void PrintF_Work(PCTSTR szFmt, const int* pParams, Printer& printer)
	{
		printer.OutputSingleLine( PUTLOG, szFmt, pParams );
	}
	void PrintF(PCTSTR szFmt, const int* pParams)
	{
		//ENSURE(Log::s_pPrinter);
		//Log::s_pPrinter->PrintF_Internal(szFmt, pParams, Log::PUTLOG);
	}

} // namespace Log
