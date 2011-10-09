////////////////////////////////////////
// CThiscallThunk:	C++ source file for the CThiscallThunk class
//
// Author:			JAB
// Date:			December 2000
//
// Purpose:			See CThiscallThunk.h			
////////////////////////////////////////

#include "stdafx.h"
#include "CThiscallThunk.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThiscallThunk class
/////////////////////////////////////////////////////////////////////////////

//
// Constructor
//

CThiscallThunk::CThiscallThunk()
{
}

//
// Destructor
//

CThiscallThunk::~CThiscallThunk()
{
}

//
// Setup
//

void CThiscallThunk::_Setup(void *pThat_, void *pFunc_)
{
	// This flavour of thunk is for "thiscall" linkage:
	//
	// The calling function expects us to pop the arguments from the stack -
	// the prototype for the call-back function specifies "__stdcall".
	// Therefore our C++ member function must be declared as using default-linkage
	// ("thiscall", i.e. "not __cdecl"). This in turn implies that it will
	// expect the 'this' pointer to be in ECX, not on the stack
	m_sThunk.bMovEcx = 0xB9;			// MOV ECX,...
	m_sThunk.dwThis = (DWORD) pThat_;	//		...,pThat_ (immediate operand)
	m_sThunk.bJmp = 0xE9;				// JMP ...
	m_sThunk.pProc = (DWORD) pFunc_ - (DWORD) (&m_sThunk + 1);
										//		...func
										//					; (relative jump distance = 
										//					; ppFunc_ - nsi address)

	// The following function is called in a similar situation by ATL (c.f. ATLWIN.H)
	// with the comment: "write block from data cache and flush from instruction cache".
	// The help info for FlushInstructionCache, however, says: "This function is valid
	// only on multiprocessor computers."
	// I'm not sure what the true position is, but we are effectively self-modifying code
	// so perhaps we need to ensure that the processor doesn't have out-of-date instructions
	// in its cache within the address range of our thunk... (see Microsoft Article ID: Q127904)

	BOOL rc = ::FlushInstructionCache(::GetCurrentProcess(), &m_sThunk, sizeof(SThunk));
#ifdef _DEBUG
	// Let's see if the function call will fail...
	if (!rc)
	{
		DWORD dwErr = ::GetLastError();
		ASSERT(!"Flush Instruction Cache error");
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CThiscallThunkCdecl class
/////////////////////////////////////////////////////////////////////////////

//
// Constructor
//

CThiscallThunkCdecl::CThiscallThunkCdecl()
{
}

//
// Destructor
//

CThiscallThunkCdecl::~CThiscallThunkCdecl()
{
}

//
// Setup
//

void CThiscallThunkCdecl::_Setup(void *pThat_, void *pFunc_)
{
	// This flavour of thunk is for "__cdecl" linkage:
	//
	// The calling function expects to pop the arguments from the stack itself -
	// the prototype for the call-back function does not specify "__stdcall"
	// ("CALLBACK", "WINAPI", and "PASCAL" all mean "__stdcall", too.)
	// Therefore our C++ member function must be declared as "__cdecl", which
	// also implies that it expects the 'this' pointer on the stack, not in ECX

	// Note: word op-codes of course have to be notated here little-endianly
	// which can be confusing...

	m_sThunk.wPop = 0x058F;				// POP ...							; save return 
	m_sThunk.dwSaveAddr = (DWORD) &m_dwSaveAddr;
										//		... DWORD PTR[SAVE_ADDR]	;      address
	m_sThunk.bPush = 0x68;				// PUSH ...
	m_sThunk.dwThis = (DWORD) pThat_;	//      ...pThat_					; (immediate operand)
	m_sThunk.bCall = 0xE8;				// CALL ...
	m_sThunk.pProc = (DWORD) pFunc_ - (DWORD) (&m_sThunk.wAddEsp);
										//		...func
										//				; (relative jump distance = 
										//				;		ppFunc_ - nsi address)
	m_sThunk.wAddEsp = 0xC483;			// ADD ESP,...	; remove "this"
	m_sThunk.bFour = 0x04;				//		...4	;		pointer from stack
	m_sThunk.wJmp = 0x25FF;				// JMP
	m_sThunk.dwSaveAddr2 = (DWORD) &m_dwSaveAddr;
										//		...[SAVE_ADDR]

	// The following function is called in a similar situation by ATL (c.f. ATLWIN.H)
	// with the comment: "write block from data cache and flush from instruction cache".
	// The help info for FlushInstructionCache, however, says: "This function is valid
	// only on multiprocessor computers."
	// I'm not sure what the true position is, but we are effectively self-modifying code
	// so perhaps we need to ensure that the processor doesn't have out-of-date instructions
	// in its cache within the address range of our thunk... (see Microsoft Article ID: Q127904)

	BOOL rc = ::FlushInstructionCache(::GetCurrentProcess(), &m_sThunk, sizeof(SThunk));
#ifdef _DEBUG
	// Let's see if the function call will fail...
	if (!rc)
	{
		DWORD dwErr = ::GetLastError();
		ASSERT(!"Flush Instruction Cache error");
	}
#endif
}

