#pragma once


class CWindowFont
	: public CFont
{
public:
	CWindowFont();

public:
    // Font styles
    typedef enum tagEType
    {
        typeNormal			= 0x00 << 16,
        typeBold			= 0x01 << 16,
        typeItalic			= 0x02 << 16,
        typeUnderline		= 0x04 << 16,
        typeDoubleHeight	= 0x08 << 16,
        typeAliased			= 0x10 << 16,
		typeMessageBox		= 0x20 << 16,
		typeThemeCaption	= 0x40 << 16,
    } EType;

// Function members
protected:
	void StyleLogfont( UINT nType, PCTSTR face );

// Interface
public:
	CLogFont m_lf;
	void CreateStyled( UINT nType, PCTSTR face=nullptr );

public:
	// Supposed to just create the font based in font of the given window, plus applying new styles
	void Create( CWindow wnd, UINT nType=0, PCTSTR face=nullptr );

	// DEPRECATED >>> Create a font with the method above and apply to a child static control
	BOOL Apply( CWindow pWndParent, UINT nControlID, UINT nType=0, PCTSTR face=nullptr );
};
