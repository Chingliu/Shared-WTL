#include "stdafx.h"
#include "WindowFont.h"


CWindowFont::CWindowFont()
{
}

void CWindowFont::StyleLogfont( UINT nType, PCTSTR face )
{
	// Amend the LOGFONT members
	UINT nSize = LOWORD( nType );
	if(nSize!=0)
		m_lf.lfHeight = -(LONG) nSize;
	if(nType & typeBold)
		m_lf.lfWeight = FW_BOLD;
	if(nType & typeItalic)
		m_lf.lfItalic = TRUE;
	if(nType & typeUnderline)
		m_lf.lfUnderline = TRUE;
	if(nType & typeDoubleHeight)
		m_lf.lfHeight *= 2;
	if(nType & typeAliased)
		m_lf.lfQuality = CLEARTYPE_QUALITY;
		//m_lf.lfQuality = ANTIALIASED_QUALITY;
	
	if(face)
		_tcsncpy( m_lf.lfFaceName, face, LF_FACESIZE );
}

void CWindowFont::CreateStyled( UINT nType, PCTSTR face )
{
	StyleLogfont( nType, face );
	CreateFontIndirect( &m_lf );
}

void CWindowFont::Create( CWindow wnd, UINT nType, PCTSTR face )
{
	ATLASSERT(wnd);
	ATLASSERT(wnd.IsWindow());

	HFONT hFont = wnd.GetFont();
	ATLASSERT(hFont);

	// Create the new font
	m_lf = hFont;
	CreateStyled(nType, face);
}

/*
BOOL CWindowFont::Apply( CWindow* pWndParent, UINT nControlID, UINT nType, PCTSTR face )
{
	if( !Create(pWndParent, nType, face) )
		return FALSE;

	CWindow wndControl = pWndParent->GetDlgItem(nControlID);
	ATLASSERT(wndControl!=NULL);
	wndControl.SetFont(m_hFont, TRUE);  //Redraw adianta?
	ATLASSERT( FALSE );					//Sim?

	return TRUE;
}
*/