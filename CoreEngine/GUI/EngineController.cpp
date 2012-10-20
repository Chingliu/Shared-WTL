#include "stdafx.h"
#include "EngineController.h"

#include "Manager.h"


CEngineSignalWnd CEngineSignalWnd::s_msgwnd;

LRESULT CEngineSignalWnd::OnSignalSend(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	reinterpret_cast<CManagerSignaler*>(wParam)->OnSignal();
	return 0;
}

LRESULT CEngineSignalWnd::OnSignalPost(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}