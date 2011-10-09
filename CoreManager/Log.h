#pragma once

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#define _SECURE_SCL 0

#ifndef ASSERT
#	ifdef _DEBUG
#		define ASSERT(x) do { if (!(x)) __debugbreak(); } while (false)
#	else
#		define ASSERT(x) __noop
#	endif
#endif

#pragma warning (disable: 4200) // zero-sized arrays

namespace Log
{
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
		Feed& operator << (CString& val) {
			Consume<PCTSTR>(val);
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

	void PrintF(PCTSTR szFmt, const int* pParams);

	struct Checkpoint {

		__declspec(thread) static Checkpoint* s_pTop;

		Checkpoint* m_pPrev;
		PCTSTR m_szFmt;
		const int* m_pParams;

		template <size_t nSize>
		Checkpoint(const Record<nSize>& rec)
			:m_szFmt(rec.m_szFmt)
			,m_pParams(rec.m_pParams)
		{
			m_pPrev = s_pTop;
			s_pTop = this;
		}

		~Checkpoint()
		{
			ASSERT(this == s_pTop);
			s_pTop = m_pPrev;
		}
	};

	void PrintCheckpoints();

} // namespace Log


// Internally-used macro
#define BUILD_FMT_RECORD(rec, expr) \
	Log::Record<sizeof(Log::SizeCalc() << expr)> rec; \
	rec << expr;

// Macros to generate random identifiers
#define RAND_IDENTIFIER_2(x,y) x##y
#define RAND_IDENTIFIER_1(x,y) RAND_IDENTIFIER_2(x,y)
#define RAND_IDENTIFIER(prefix) RAND_IDENTIFIER_1(prefix, __LINE__)


// Put this in the log
#define PUTLOG(expr) \
	do { \
		BUILD_FMT_RECORD(RAND_IDENTIFIER(recAutoToLog), expr) \
		Log::PrintF(RAND_IDENTIFIER(recAutoToLog).m_szFmt, RAND_IDENTIFIER(recAutoToLog).m_pParams); \
	}while (false)

// Declare a checkpoint
#define CHECKPOINT(expr) \
	BUILD_FMT_RECORD(RAND_IDENTIFIER(var##_Record), expr) \
	Log::Checkpoint RAND_IDENTIFIER(var)(RAND_IDENTIFIER(var##_Record));
