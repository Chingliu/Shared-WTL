#include "stdafx.h"
#include "SimpleBitmap.h"


void CSimpleBitmap::GetMeasures()
{
	BITMAP binfo;
	GetBitmap( &binfo );
	size.SetSize( binfo.bmWidth, binfo.bmHeight );
	ptcenter.SetPoint( binfo.bmWidth/2, binfo.bmHeight/2 );
}

void CSimpleBitmap::LoadBitmap( UINT resID )
{
	//DeleteObject();
	ATLVERIFY( CBitmap::LoadBitmap(resID) );
	GetMeasures();
}

void CSimpleBitmap::LoadGdiplusResImage( _U_STRINGorID bitmap, _U_STRINGorID type )
{
	Gdiplus::Bitmap* pBitmap = nullptr;

	// Loading from a file
	/*if( HIWORD(bitmap.m_lpstr) != NULL ) {
		// Load from filename
		pBitmap = new Gdiplus::Bitmap(T2CW(bitmap.m_lpstr)); 
	} else*/

	// Loading PNG, JPG resources etc
	if( type.m_lpstr!=NULL && type.m_lpstr!=RT_BITMAP )
	{
		CResource res;
		if( !res.Load(type, bitmap) ) return;

		DWORD dwSize = res.GetSize();
		HANDLE hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
		if( hMemory==NULL ) return;

		::memcpy(::GlobalLock(hMemory), res.Lock(), dwSize);
		::GlobalUnlock(hMemory);

		IStream* pStream = nullptr; 
		if( FAILED(::CreateStreamOnHGlobal(hMemory, TRUE, &pStream)) )
		{
			::GlobalFree(hMemory);
			return;
		}
		pBitmap = new Gdiplus::Bitmap(pStream);
		pStream->Release();
	}

	// This only loads BMP resources
	else
	{
		pBitmap = new Gdiplus::Bitmap(_Module.GetResourceInstance(), (LPCWSTR) (UINT) bitmap.m_lpstr);
	}

	if( pBitmap==nullptr ) return;
	HBITMAP hBitmap = NULL;
	pBitmap->GetHBITMAP(NULL, &hBitmap);
	size.SetSize( pBitmap->GetWidth(), pBitmap->GetHeight() );
	ptcenter.SetPoint( size.cx>>1, size.cy>>1 );

	delete pBitmap;

	Attach( hBitmap );
	GetMeasures();
}

void CSimpleBitmap::DrawBitmap( HDC dc, CRect& rect, bool disabled  )
{
	CDCHandle(dc).DrawState( CPoint(rect.CenterPoint() - ptcenter), size,
					*this, disabled?DSS_DISABLED:DSS_NORMAL );
}