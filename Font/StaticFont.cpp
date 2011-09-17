#include "stdafx.h"
#include "StaticFont.h"


CStaticFont::CStaticFont()
{
	m_openurl = FALSE;
	m_hCursor = NULL;
}

void CStaticFont::StaticStyle( UINT nType )
{
	UINT nSize = LOWORD( nType );
	if(nSize!=0)
		m_lf.lfHeight = -(LONG) nSize;
	if(nType & CWindowFont::typeBold)
		m_lf.lfWeight = m_lf.IsBold() ? 0:FW_BOLD;
	if(nType & CWindowFont::typeItalic)
		m_lf.lfItalic = !m_lf.lfItalic;
	if(nType & CWindowFont::typeUnderline)
		m_lf.lfUnderline = !m_lf.lfUnderline;
	if(nType & CWindowFont::typeDoubleHeight)
		m_lf.lfHeight *= 2;
	RedrawStatic( TRUE );
}

void CStaticFont::StaticFace( PCTSTR face )
{
	_tcsncpy( m_lf.lfFaceName, face, LF_FACESIZE );
	RedrawStatic( TRUE );
}

void CStaticFont::StaticColor( COLORREF crrText )
{
	m_crrText = crrText;
}

void CStaticFont::SetResourceText( UINT resID )
{
	CString label;
	label.LoadString( resID );
	SetWindowText( label );
}

void CStaticFont::SetLink( LPCSTR url )
{
	if( url )
	{
		m_openurl = TRUE;
		m_url = url;
	}
	m_hCursor = ::LoadCursor( 0, MAKEINTRESOURCE(IDC_HAND) );
	ModifyStyle( 0, SS_NOTIFY );
}

void CStaticFont::RedrawStatic( BOOL refont )
{
	ATLASSERT(IsWindow()!=FALSE);

	if( refont )
	{
		if( m_fn.IsNull()==FALSE )
			m_fn.DeleteObject();

		BOOL bCreated = m_fn.CreateFontIndirect( &m_lf )!=NULL;
		ATLASSERT(bCreated);
		SetFont( m_fn, TRUE );
	} else {
		CRect invrect;
		GetWindowRect( &invrect );

		CWindow parent = GetParent();
		parent.ScreenToClient( &invrect );
		parent.InvalidateRect( &invrect );
	}
}

BOOL CStaticFont::SubclassWindow( HWND hWnd )
{
	if( !CWindowImpl::SubclassWindow(hWnd) )
		return FALSE;

	m_hBrush = (HBRUSH) ::GetStockObject( NULL_BRUSH );
	m_crrText = GetSysColor( COLOR_WINDOWTEXT );

	m_lf = GetFont();
	RedrawStatic( TRUE );

	return TRUE;
}

void CStaticFont::OnBeginHover()
{
	// Handler só funciona com SS_NOTIFY
	// por isso não usa variavel condicional
	m_oldText = m_crrText;
	StaticStyle( CWindowFont::typeUnderline );
	StaticColor( RGB(22,109,204) );
}

void CStaticFont::OnEndHover()
{
	StaticStyle( CWindowFont::typeUnderline );
	StaticColor( m_oldText );

	RedrawStatic();
}

void CStaticFont::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_openurl )
		ShellExecute( NULL, L"open", m_url, NULL, NULL, SW_SHOWNORMAL );
}

LRESULT CStaticFont::OnCtlColorStatic(CDCHandle dc, HWND hwndChild)
{
	dc.SetTextColor(m_crrText);
	dc.SetBkMode(TRANSPARENT);
	return (LRESULT) m_hBrush;
}
