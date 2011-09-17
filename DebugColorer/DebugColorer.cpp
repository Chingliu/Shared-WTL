#include "stdafx.h"
#include "DebugColorer.h"


CDebugColorer* CDebugColorer::m_dynaDlg = NULL;

CDebugColorer::CDebugColorer()
	: m_ControlRedir( this, 1 )
{
	m_refcolor = NULL;
	m_refbrush = NULL;
	m_refpen = NULL;
}

void CDebugColorer::ShowColorDialog( CWindow wndrepaint, BOOL behind )
{
	if( m_dynaDlg!=NULL )
		return;

	m_dynaDlg = new CDebugColorer();
	m_dynaDlg->m_repaint = wndrepaint;
	m_dynaDlg->Create( behind ? NULL : _wndMain.m_hWnd );
}

LRESULT CDebugColorer::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	CRect rc;
	GetClientRect( &rc );

	m_ColorSpectrum.Create( ModuleHelper::GetModuleInstance(),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,             // styles 
        rc,												// control rect
        m_hWnd,                                         // parent window
        9001,                                           // control id
        RGB(0,255,0),									// initial color
        CXColorSpectrumCtrl::XCOLOR_TOOLTIP_NONE);		// tooltip format //XCOLOR_TOOLTIP_RGB
	m_ControlRedir.SubclassWindow( m_ColorSpectrum.m_hWnd );

	ShowWindow( SW_SHOWNORMAL );
	Invalidate();

	return TRUE; // set focus to default control
}

LRESULT CDebugColorer::OnColor(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ATLTRACE( "R:%d ", GetRValue(wParam) );
	ATLTRACE( "G:%d ", GetGValue(wParam) );
	ATLTRACE( "B:%d ", GetBValue(wParam) );
	ATLTRACE( "\n" );

	if( m_refcolor!=NULL )
	{
		*m_refcolor = (COLORREF) wParam;
	}

	if( m_refbrush!=NULL )
	{
		if( !m_refbrush->IsNull() )
			m_refbrush->DeleteObject();
		m_refbrush->CreateSolidBrush( (COLORREF) wParam );
	}

	if( m_refpen!=NULL )
	{
		m_refpen->DeleteObject();
		m_refpen->CreatePen( PS_SOLID, 0, (COLORREF) wParam );
	}

	m_repaint.Invalidate();
	_wndMain.SetFocus();

	return 0;
}


void CDebugColorer::SelectClrref( COLORREF* refcolor )
{
	m_dynaDlg->m_refcolor = refcolor;
	m_dynaDlg->m_ColorSpectrum.SetRGB( *refcolor );
}

void CDebugColorer::SelectBrush( CBrush* refbrush )
{
	LOGBRUSH color;
	refbrush->GetLogBrush( &color );
	m_dynaDlg->m_ColorSpectrum.SetRGB( color.lbColor );
	m_dynaDlg->m_refbrush = refbrush;
}

void CDebugColorer::SelectPen( CPen* refpen )
{
	LOGPEN color;
	refpen->GetLogPen( &color );
	m_dynaDlg->m_ColorSpectrum.SetRGB( color.lopnColor );
	m_dynaDlg->m_refpen = refpen;
}

LRESULT CDebugColorer::OnSetFocus(HWND hwndOldFocus)
{
	_wndMain.ValidateRect( NULL );

	SetMsgHandled( FALSE );
	return 0;
}
