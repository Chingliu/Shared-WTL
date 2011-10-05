#include "stdafx.h"
#include "Manager.h"
#include "Tools\Util.h"


DWORD CManager::m_dwUIthread;

CManager::CManager()
{
	m_dwUIthread = get_thread_id();
	ATLASSERT(m_dwUIthread);
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
	m_single_cbk();
	::CloseHandle(m_hThread);
}


// Interface
void CManager::DirectWork( Callback<void()> runcbk )
{
	runcbk();
}

void CManager::ThreadWork( Callback<void()> runcbk )
{
	ATLASSERT(runcbk);
	m_single_cbk = runcbk;

	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _ThreadProcThunk,
			this, 0, NULL);
}

void CManager::MinWait( DWORD ms )
{
	DWORD dif = ::GetTickCount()-m_ticktime;
	if( ms>dif )
		::Sleep( ms-dif );
}

void CManager::AddAdapter(CAdapter* adapter)
{
}

void CManager::ASSERT_IsThreadUI()
{
	ATLASSERT( get_thread_id() == CManager::m_dwUIthread );
}

void CManager::ASSERT_IsGuarded()
{
}