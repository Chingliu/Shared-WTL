#pragma once

#pragma warning (disable: 4200) // zero-sized arrays

#include "../util_callback.h"


namespace Log
{
// internal
	struct Workpoint;
	class Printer;

	template <typename T>
	struct StackAligned {
		int m_pBuf[(sizeof(T) + sizeof(int)-1) / sizeof(int)];
	};

	struct Feed
	{
		int* m_pPtr;
		template <typename T> void Consume(const T& val) {
			*((T*) m_pPtr) = val;
			m_pPtr += sizeof(StackAligned<T>) / sizeof(int);
		}

		template <typename T> Feed& operator << (const T& val) {
			Consume(val);
			return *this;
		}
		Feed& operator << (CStringA& val) {
			Consume<PCSTR>((PCSTR) val);
			return *this;
		}
		Feed& operator << (CStringW& val) {
			Consume<PCWSTR>(val);
			return *this;
		}
		Feed& operator << (PCSTR val) {
			Consume<PCSTR>(val);
			return *this;
		}
		Feed& operator << (PCWSTR val) {
			Consume<PCWSTR>(val);
			return *this;
		}
	};

	template <size_t nSize>
	struct Record {
		PCTSTR m_szFmt;
		int m_pParams[nSize / sizeof(int)];

		Feed operator << (PCTSTR szFmt) {
			m_szFmt = szFmt;
			Feed feed;
			feed.m_pPtr = m_pParams;
			return feed;
		}
	};

	struct SizeCalc {
		template <size_t nSize>
		struct Nested {
			char m_pBuf[nSize];
			Nested<nSize + sizeof(StackAligned<PCSTR>)> operator << (PCSTR);
			Nested<nSize + sizeof(StackAligned<PCWSTR>)> operator << (PCWSTR);
			template <typename T> Nested<nSize + sizeof(StackAligned<T>)> operator << (const T&);
		};

		Nested<0> operator << (PCTSTR szFmt);
	};

	enum EOutputType
	{
		CHECKPOINT,	// Workpoint
		PUTLOG,
		WORK_ERROR,
		MISC
	};

	struct LogLine
	{
		EOutputType type;
		CString linestr;
	};

	void PrintF(PCTSTR szFmt, const int* pParams);
	void PrintF_Work(PCTSTR szFmt, const int* pParams, Printer& printer);
	typedef util::Callback< void(CAtlArray<LogLine>& /*list*/, EOutputType/*chunk*/) > TFOutputCbk;


	//extern __declspec(thread) class Printer* s_pPrinter;
	extern __declspec(thread) Workpoint* s_pTop;
	extern TFOutputCbk s_output;

	struct Workpoint
	{
		Workpoint* m_pPrev;
		class Printer& m_pPrinter;
		PCTSTR m_szFmt;
		const int* m_pParams;

		template <size_t nSize>
		Workpoint(const Record<nSize>& rec, Printer& printer)
			:m_pPrinter(printer)
			,m_szFmt(rec.m_szFmt)
			,m_pParams(rec.m_pParams)
		{
			m_pPrev = Log::s_pTop;
			Log::s_pTop = this;

			m_pPrinter.ImmediatePoint(this);
		}

		~Workpoint()
		{
			ENSURE(this == Log::s_pTop);
			Log::s_pTop = m_pPrev;
		}
	};


// public

	class Printer
	{
	public:
		Printer();
		typedef util::Callback< bool/*accepts line?*/(LogLine&/*line ref*/) > TFPrinterCbk;

	// interface
	public:
		void SetPrefix( PCTSTR pstr );
		void SetImmediateLog( bool b );
		void SetPrintFilter( TFPrinterCbk filter );
		CString log_output;

	// printer functions
	public:
		void ImmediatePoint(Workpoint* pPoint);
		void OutputSingleLine(EOutputType type, PCTSTR szFmt, const int* pParams);

	// internal
	public:
		bool SPrintF_Line(LogLine& out_line, PCTSTR szFmt, const int* pParams);// filter and format the line

		CString m_prefix;
		bool m_immediate;
		TFPrinterCbk m_filter;
	};


// interface
	void SetGlobalOutput(TFOutputCbk cbk);
	void OutputStackPoints();	// outputs the log chain of the current thread


// TBD
	/*
	struct Logpoint
	{
		template <size_t nSize>
		void Setup(const Record<nSize>& rec)
		{
			..
		}
	};

	struct Checkpoint
	{
		Checkpoint* m_pPrev;
		PCTSTR m_szFmt;
		const int* m_pParams;

		template <size_t nSize>
		Checkpoint(const Record<nSize>& rec)
			:m_szFmt(rec.m_szFmt)
			,m_pParams(rec.m_pParams)
		{
			m_pPrev = Log::s_pTop;
			Log::s_pTop = this;

			ENSURE(Log::s_pPrinter);
			Log::s_pPrinter->LogImmediateCheckpoint(this);
		}

		~Checkpoint()
		{
			ENSURE(this == Log::s_pTop);
			Log::s_pTop = m_pPrev;
		}
	};*/

} // namespace Log


// Internally-used macro
#define BUILD_FMT_RECORD(rec, expr) \
	Log::Record<sizeof(Log::SizeCalc() << expr)> rec; \
	rec << expr;

// Macros to generate random identifiers
#define RAND_IDENTIFIER_2(x,y) x##y
#define RAND_IDENTIFIER_1(x,y) RAND_IDENTIFIER_2(x,y)
#define RAND_IDENTIFIER(prefix) RAND_IDENTIFIER_1(prefix, __LINE__)


// Checkpoint - no working context
/*#define CHECKPOINT(expr) \
	BUILD_FMT_RECORD(RAND_IDENTIFIER(var##_Record), expr) \
	Log::Checkpoint RAND_IDENTIFIER(var)(RAND_IDENTIFIER(var##_Record));
*/

// Work context logging - has an associated Printer (local 'logger' variable)
#define WORK_PUTLOG(expr) \
	do { \
		BUILD_FMT_RECORD(RAND_IDENTIFIER(recAutoToLog), expr) \
		Log::PrintF_Work(RAND_IDENTIFIER(recAutoToLog).m_szFmt, RAND_IDENTIFIER(recAutoToLog).m_pParams, logger); \
	}while (false)

#define WORK_CHECKPOINT(expr) \
	BUILD_FMT_RECORD(RAND_IDENTIFIER(var##_Record), expr) \
	Log::Workpoint RAND_IDENTIFIER(var)(RAND_IDENTIFIER(var##_Record), logger);