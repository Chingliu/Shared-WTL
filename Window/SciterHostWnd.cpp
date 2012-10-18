#include "stdafx.h"
#include "SciterHostWnd.h"

#include <atldwm.h>


CSciterHostWnd::CSciterHostWnd()
{
	//sciter::debug_output::debug_output t;
}

LRESULT CSciterHostWnd::on_callback_host(LPSCN_CALLBACK_HOST pnmld)
{
#ifdef DEBUG
	switch( pnmld->channel )
	{
		case 0: // 0 - stdin, read from stdin requested, put string into pnmld->r
			break;
		case 1: // 1 - stdout, "stdout << something" requested, pnmld->p1 is
				//     string to output.
			{
				CString str = pnmld->p1.to_string().c_str();
				OutputDebugString(str);
			}
			break;
		case 2: // 2 - stderr, "stderr << something" requested or error happened, 
				//     pnmld->p1 is string to output.
			{
				CString str = pnmld->p1.to_string().c_str();
				OutputDebugString(str);
			}
			break;
		default: // view.callback(channel,p1,p2) call from script
			break;
	}
#endif

	return 0;
}

#ifndef SCITER_LEGACY_SUPPORT
bool CSciterHostWnd::load_file(LPCWSTR uri)
{
	bool bOK = ::SciterLoadFile( m_hWnd, L"res_sciter\\" + CString(uri) )!=0;
	ASSERT(bOK);

	return bOK;
}

LRESULT CSciterHostWnd::on_load_data(LPSCN_LOAD_DATA pnmld)
{
	return LOAD_OK;// do default loading if data not set
}
#endif

int CSciterHostWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_frame_composited = _wndMain.GetExStyle() & WS_EX_COMPOSITED;

	DefWindowProc();
	setup_callback();
	::SciterSetOption(m_hWnd, SCITER_TRANSPARENT_WINDOW, CDwm().DwmIsCompositionEnabled());
	::SciterSetOption(m_hWnd, SCITER_FONT_SMOOTHING, 3);

	SetMsgHandled(FALSE);
	return TRUE;
}

BOOL CSciterHostWnd::OnEraseBkgnd(CDCHandle dc)
{
	if( !m_frame_composited )
		SetMsgHandled(FALSE);
	return FALSE;
}
