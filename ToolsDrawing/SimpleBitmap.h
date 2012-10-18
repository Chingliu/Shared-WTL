#pragma once


class CSimpleBitmap
	: public CBitmap
{
// Interface
public:
	void DrawBitmap( HDC dc, CRect& rect, bool disabled = false );
	using CBitmap::operator=;

	// Loads with standard resource loading + GetMeasures()
	void LoadBitmap( _U_STRINGorID resID );

	// Loads with GDI+
	void LoadGdiplusResImage( _U_STRINGorID resID, _U_STRINGorID type = (UINT) RT_BITMAP );

	// Loads with OleLoadPicture method that loads JPG and BMP
	bool LoadImageResource( _U_STRINGorID resID, _U_STRINGorID type = L"PNG" );

// Function member
protected:
	void GetMeasures();
	bool RenderOlePicture(HGLOBAL hGlobal, DWORD dwSize);

// Acess members
public:
	CSize size;
	CPoint ptcenter;
};
