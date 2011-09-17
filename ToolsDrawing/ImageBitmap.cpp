/// ImageBitmap.cpp : implementation file

#include "stdafx.h"
#include "ImageBitmap.h"


//**CONFIGURATION************************************************************

// +++ ESSENTIAL VALUES +++
	PCTSTR szDEFAULT_RESTYPE = _T("PNG");

//***************************************************************************

CImageBitmap::CImageBitmap()
{
}

CImageBitmap::~CImageBitmap()
{
}

// Interface
BOOL CImageBitmap::LoadImageResource( UINT nIDRes, PCTSTR type )
{
	if( m_hBitmap )
		DeleteObject();

	HRSRC hRsrc = FindResource( _Module.GetResourceInstance(), MAKEINTRESOURCE(nIDRes), type ? type : szDEFAULT_RESTYPE );
	HRESULT hr = ::GetLastError();
	ATLASSERT( hRsrc );

	HGLOBAL hResData = ::LoadResource( _Module.GetResourceInstance(), hRsrc );
	ATLASSERT( hResData );
	m_dwSize = SizeofResource( _Module.GetResourceInstance(), hRsrc );

	m_hGlobal = GlobalAlloc( GMEM_MOVEABLE, m_dwSize );
	ATLASSERT( m_hGlobal );
	LPVOID pvData = GlobalLock(m_hGlobal);
	LPVOID pvSrc = LockResource(hResData);
	ATLASSERT( pvData && pvSrc );

	CopyMemory( pvData, pvSrc, m_dwSize );
	FreeResource( hResData );

	return RenderOlePicture();
}

// OleLoadPicture is probably not working with PNG, so should be used to load JPG and BMP
BOOL CImageBitmap::RenderOlePicture()
{
	HRESULT hr;
	GlobalUnlock(m_hGlobal);

#if 0
	VARIANT vt;
	VariantInit(&vt); //initialize the variant
	vt.vt = VT_BSTR; //set to BSTR string
	vt.bstrVal = SysAllocString( L"D:\\test.bmp" );
	IDispatch* disp;
	hr = ::OleLoadPictureFile(vt, &disp);
#endif

	LPSTREAM pstm;
	hr = CreateStreamOnHGlobal(m_hGlobal, TRUE, &pstm);
	ATLASSERT(hr==S_OK && pstm);

	LPPICTURE pPicture;
	hr = ::OleLoadPicture(pstm, m_dwSize, FALSE, IID_IPicture, (LPVOID*)&pPicture);
	ATLASSERT(hr==S_OK && pPicture);

	pstm->Release();
	if( FAILED(hr) )
		return FALSE;

	HBITMAP hBitmap;
	hr = pPicture->get_Handle((OLE_HANDLE FAR *) &hBitmap);
	ATLASSERT(hr==S_OK && hBitmap);
	if( FAILED(hr) )
		return FALSE;

	Attach(hBitmap);
	GetMeasures();
	return TRUE;
}