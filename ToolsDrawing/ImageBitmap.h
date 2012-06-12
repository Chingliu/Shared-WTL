#pragma once

#include "SimpleBitmap.h"


// CImageBitmap: image render

class CImageBitmap
	: public CSimpleBitmap
{
public:
	CImageBitmap();

// Interface
public:
	BOOL LoadImageResource( UINT nIDRes, PCTSTR type=nullptr );

// Function Members
private:
	BOOL RenderOlePicture();

// Data members
protected:
	HGLOBAL m_hGlobal;
	DWORD m_dwSize;
};
