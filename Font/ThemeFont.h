#pragma once

#include "WindowFont.h"


class CThemeFont
	: public CWindowFont, public CDwm
{
public:
	CThemeFont();

// Interface
public:
	void CreateThemeFont( CTheme* themebase, UINT nType=0, PCTSTR face=nullptr );
	int MeasureText( CWindow source, CString text );
	void DrawText( CDCHandle dc, CRect& rcText, CString text );

// Acess members
public:
	DTTOPTS dto;
	UINT uFormat;

// Data members
public:
	CTheme* m_themebase; //pointer because it avoids an obscure bug when having the original handle destroyed
};
