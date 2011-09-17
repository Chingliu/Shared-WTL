#pragma once

#include "MixHover.h"
#include "WindowFont.h"


class CStaticFont
	: public CWindowImpl<CStaticFont, CStatic>, public CMixHover<CStaticFont>
{
public:
	CStaticFont();

// Interface
public:
	void StaticStyle( UINT nType );
	void StaticFace( PCTSTR face );
	void StaticColor( COLORREF crrText );

	void RedrawStatic( BOOL refont=FALSE );
	void SetResourceText( UINT resID );
	void SetLink( LPCSTR url = NULL );

// Data members
private:
	CLogFont m_lf;
	CWindowFont m_fn;

	// color
	COLORREF m_oldText;
	COLORREF m_crrText;
	HBRUSH m_hBrush;
	// link
	BOOL m_openurl;
	HCURSOR	m_hCursor;
	CString m_url;

// Function Members
private:

// Maps
public:
	BEGIN_MSG_MAP_EX(CStaticFont)
		MSG_OCM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)

		CHAIN_MSG_MAP(CMixHover<CStaticFont>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

// Virtual overrides
public:
	void OnBeginHover();
	void OnEndHover();
	BOOL SubclassWindow( HWND hWnd );

// Handler prototypes
public:
	void OnLButtonDown(UINT nFlags, CPoint point);
	LRESULT OnCtlColorStatic(CDCHandle dc, HWND hwndChild);
};
