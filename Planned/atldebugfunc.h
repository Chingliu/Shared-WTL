/**
 * Function Level trace class.
 * Trace with indent and function arguments.
 *
 * Use the following two macros (which are active in
 * DEBUG builds):
 *
 * ATLTRACEFUNC
 * ATLTRACEFUNCLOG
 *
 * ATLTRACEFUNC:
 * Syntax:
 *   ATLTRACEFUNC("function")
 *   ATLTRACEFUNC("function", "format", "arg1", "arg2", ...)
 * Creates scope level trace. Will make two entries
 * using ::OutputDebugString(). One at creation and one at scope
 * exit.
 * Define this macro at the top of every function to trace.
 *
 * ATLTRACEFUNCLOG:
 * Syntax:
 *   ATLTRACEFUNCLOG("function", "format", "arg1", "arg2", ...)
 * Adds an additional log to ::OutputDebugString(), but
 * keeps the indent.
 *
 * @autor: Bjarke Viksoe
 */
#ifdef _DEBUG
#pragma once
class CAtlTraceFunc
{
public:
  CAtlTraceFunc() 
  { 
    m_pstr = NULL;
    m_indent++;
  }
  ~CAtlTraceFunc() 
  { 
    TraceIndent();
    ATLTRACE("%s complete\n", m_pstr); 
    m_indent--; 
  };
  void _cdecl Trace(LPCSTR lpszFunc, LPCSTR lpszFormat = NULL, ...)
  {
    m_pstr = lpszFunc; 
    // Indent debug output
    TraceIndent();
    // Add function name to output buffer
    char szBuffer[512];
    strcpy(szBuffer, lpszFunc);
    int len = strlen(szBuffer);
    if( lpszFormat ) {
       // Append a divider string
       strcat(szBuffer, " - ");
       len += 3;
       // Add format string
       va_list args;
       va_start(args, lpszFormat);
       len += _vsnprintf(szBuffer + len, sizeof(szBuffer) - len - 2, lpszFormat, args);
       va_end(args);
    }
    szBuffer[len] = '\n';
    szBuffer[len+1] = '\0';
    ::OutputDebugStringA(szBuffer);
  }
  void inline TraceIndent() const
  {
    const char chIndent = '-';
    char szBuffer[32];
    char* p = szBuffer;
    for( unsigned int i=0; i<m_indent; i++ ) *p++ = chIndent;
    *p++ = '\0';
    ::OutputDebugStringA(szBuffer);
  };
  LPCSTR m_pstr;
  static unsigned short m_indent;
};
__declspec(selectany) unsigned short CAtlTraceFunc::m_indent = 0;
#define ATLTRACEFUNC CAtlTraceFunc aaa; aaa.Trace
#define ATLTRACEFUNCLOG aaa.Trace
#else
#define ATLTRACEFUNC void(0)
#define ATLTRACEFUNCLOG void(0)
#endif // _DEBUG
