#include "stdafx.h"
#include "SimpleBitmap.h"


void CSimpleBitmap::GetMeasures()
{
	BITMAP binfo;
	ENSURE( GetBitmap(binfo) );
	size.SetSize( binfo.bmWidth, binfo.bmHeight );
	ptcenter.SetPoint( binfo.bmWidth/2, binfo.bmHeight/2 );
}

void CSimpleBitmap::LoadBitmap( _U_STRINGorID resID )
{
	//DeleteObject();
	ENSURE( CBitmap::LoadBitmap(resID) );
	GetMeasures();
}

void CSimpleBitmap::LoadGdiplusResImage( _U_STRINGorID resID, _U_STRINGorID type )
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
		if( !res.Load(type, resID) ) return;

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
		pBitmap = new Gdiplus::Bitmap(_Module.GetResourceInstance(), (LPCWSTR) (UINT) resID.m_lpstr);
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

bool CSimpleBitmap::LoadImageResource( _U_STRINGorID resID, _U_STRINGorID type )
{
	ASSERT(type.m_lpstr);
	if( m_hBitmap )
		DeleteObject();

	CResource res;
	ENSURE( res.Load(type, resID) );
	DWORD dwSize = res.GetSize();

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	ENSURE( hGlobal );

	LPVOID pvData = GlobalLock(hGlobal);
	LPVOID pvSrc = res.Lock();
	ENSURE( pvData && pvSrc );

	memcpy(pvData, pvSrc, dwSize);
	return RenderOlePicture(hGlobal, dwSize);
}

// OleLoadPicture is probably not working with PNG, so should be used to load JPG and BMP
bool CSimpleBitmap::RenderOlePicture(HGLOBAL hGlobal, DWORD dwSize)
{
	HRESULT hr;
	GlobalUnlock(hGlobal);

#if 0
	VARIANT vt;
	VariantInit(&vt); //initialize the variant
	vt.vt = VT_BSTR; //set to BSTR string
	vt.bstrVal = SysAllocString( L"D:\\test.bmp" );
	IDispatch* disp;
	hr = ::OleLoadPictureFile(vt, &disp);
#endif

	LPSTREAM pstm;
	hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	ENSURE( hr==S_OK && pstm );

	LPPICTURE pPicture;
	hr = ::OleLoadPicture(pstm, dwSize, FALSE, IID_IPicture, (LPVOID*)&pPicture);
	ASSERT(hr==S_OK && pPicture);

	pstm->Release();
	if( FAILED(hr) )
		return false;

	HBITMAP hBitmap;
	hr = pPicture->get_Handle((OLE_HANDLE FAR *) &hBitmap);
	ASSERT(hr==S_OK && hBitmap);
	if( FAILED(hr) )
		return false;

	Attach(hBitmap);
	GetMeasures();
	return true;
}



void CSimpleBitmap::DrawBitmap( HDC dc, CRect& rect, bool disabled  )
{
	CDCHandle(dc).DrawState( CPoint(rect.CenterPoint() - ptcenter), size,
					*this, disabled?DSS_DISABLED:DSS_NORMAL );
}
