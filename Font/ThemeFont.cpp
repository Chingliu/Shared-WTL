#include "stdafx.h"
#include "ThemeFont.h"

#include "ToolsDrawing\ImageHelpers.h"


CThemeFont::CThemeFont()
{
	m_themebase = nullptr;
	uFormat = DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX;

	dto.dwSize = sizeof(DTTOPTS);
	dto.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_GLOWSIZE; //DrawThemeTextEx() always draws the glow; set iGlowSize to 0 to not have it
	dto.iGlowSize = 10;
	dto.crText = 0; //was white
}

void CThemeFont::CreateThemeFont( CTheme* themebase, UINT nType, PCTSTR face )
{
	ATLASSERT(themebase);
	m_themebase = themebase;

	// Determine what font to use for the text.
	if( themebase->IsThemeNull() || (nType & typeMessageBox) )
		m_lf.SetMessageBoxFont();
	else if( nType & typeThemeCaption )
		themebase->GetThemeSysFont( TMT_CAPTIONFONT, &m_lf );
	else
		themebase->GetThemeSysFont( TMT_MSGBOXFONT, &m_lf );

	CreateStyled(nType, face);
}

int CThemeFont::MeasureText( CString text, CWindow source )
{
	CRect rcMeasure;
	CWindowDC dc( source );
	dc.SelectFont( *this );

	if( DwmIsCompositionEnabled() )
	{
		ATLASSERT(m_themebase);
		// GetTextExtentPoint32() ?
		m_themebase->GetThemeTextExtent(dc, 0, 0, text, -1,
										uFormat, NULL, &rcMeasure );
	} else {
		dc.DrawText( text, -1, &rcMeasure, uFormat | DT_CALCRECT );
	}
	ATLASSERT(rcMeasure.left==0);
	return rcMeasure.right;// rcMeasure.Width();
}

void CThemeFont::DrawText( CDCHandle dc, CRect& rcText, CString text )
{
	if( IsDwmSupported() )
	{
		// Set up a memory DC and bitmap that we'll draw into
		CDC dcMem;
		dcMem.CreateCompatibleDC( dc );
		
		BITMAPINFO dib = {0};
		dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		dib.bmiHeader.biWidth = rcText.Width();
		dib.bmiHeader.biHeight = -rcText.Height();
		dib.bmiHeader.biPlanes = 1;
		dib.bmiHeader.biBitCount = 32;
		dib.bmiHeader.biCompression = BI_RGB;

		// Set up the DC
		CBitmap bmp;
		bmp.CreateDIBSection( dc, &dib, DIB_RGB_COLORS,
								NULL, NULL, 0 );
		HBITMAP hBmpOld = dcMem.SelectBitmap( bmp );

		// Draw the text
		CRect rcText2 = rcText;
		rcText2 -= rcText2.TopLeft(); // same rect but with (0,0) as the top-left

		dcMem.SelectFont( *this );
		HRESULT hr = m_themebase->DrawThemeTextEx( dcMem, 0, 0, text, -1,
						  uFormat, rcText2, &dto );
		ATLVERIFY(hr==S_OK);

		// Blit the text to the screen
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.AlphaFormat = AC_SRC_ALPHA;
		bf.SourceConstantAlpha = 255;
		dc.AlphaBlend( rcText.left, rcText.top, rcText.Width(), rcText.Height(),
			dcMem, 0, 0, rcText.Width(), rcText.Height(), bf );
		//dc.BitBlt( rcText.left, rcText.top, rcText.Width(), rcText.Height(), 
		//	dcMem, 0, 0, SRCCOPY );

		dcMem.SelectBitmap(hBmpOld);
	} else {
		// Set up the DC
		dc.SelectFont( *this );
		dc.SetBkMode( TRANSPARENT );

		// Draw the text
		dc.DrawText( text, -1, rcText, uFormat );
	}
}
