#include "stdafx.h"
#include "Manager.h"
#include "Tools\Util.h"


DWORD CManager::g_dwThreadUI	= get_thread_id();
bool CManager::g_bRunGuard		= false;

CManager::CManager()
{
}

__declspec(naked) DWORD WINAPI _ThreadProcThunk(void*)
{
	__asm
	{
		pop		eax
		pop		ecx
		push	eax
		xor		eax, eax
		jmp CManager::Run
	}
}

void CManager::Run()
{
	m_ticktime = ::GetTickCount();
	WorkGuard();
	::CloseHandle(m_hThread);
}

void CManager::WorkGuard()
{
	ATLASSERT(CManager::g_bRunGuard==false);

	CManager::g_bRunGuard = true;
	try
	{
		m_single_cbk();
	} catch(...) {
		__debugbreak();
	}
	CManager::g_bRunGuard = false;
}

// Interface
void CManager::DirectWork( Callback<void()> runcbk )
{
	m_single_cbk = runcbk;
	WorkGuard();
}

void CManager::ThreadWork( Callback<void()> runcbk )
{
	CManager::AssureThreadUI();
	ATLASSERT(runcbk);

	m_single_cbk = runcbk;
	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _ThreadProcThunk,
			this, 0, NULL);
}

void CManager::MinWait( DWORD ms )
{
	// TODO: check if calling from a Work thread spawned by ThreadWork()
	DWORD dif = ::GetTickCount()-m_ticktime;
	if( ms>dif )
		::Sleep( ms-dif );
}

void CManager::AddAdapter(CAdapter* adapter)
{
}

void CManager::AssureThreadUI()
{
	ATLASSERT( get_thread_id() == CManager::g_dwThreadUI );
}

void CManager::AssureGuardedWork()
{
	ATLASSERT( CManager::g_bRunGuard );
}
