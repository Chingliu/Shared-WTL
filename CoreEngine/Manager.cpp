#include "stdafx.h"
#include "Manager.h"


CManager::CManager()
{
}

void CManager::AddAdapter(CAdapter* adapter)
{
}

void CManager::AssureThreadUI()
{
	ATLASSERT( ::GetCurrentThreadId() == CManager::g_dwMainThread );
}

void CManager::AssureGuardedWork()
{
	ATLASSERT( CManager::g_bRunGuard );
}
