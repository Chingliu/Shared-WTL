#pragma once


class CSimpleBitmap
	: public CBitmap
{
// Interface
public:
	void LoadBitmap( UINT resID );
	void LoadGdiplusResImage( _U_STRINGorID bitmap, _U_STRINGorID type = (UINT) RT_BITMAP );
	void DrawBitmap( HDC dc, CRect& rect, bool disabled = false );

	using CBitmap::operator=;

// Function member
public:
	void GetMeasures();

// Acess members
public:
	CSize size;
	CPoint ptcenter;
};
