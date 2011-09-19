#pragma once


ATLINLINE HBITMAP AtlLoadGdiplusImage(ATL::_U_STRINGorID bitmap, ATL::_U_STRINGorID type = (UINT) 0)
{
   USES_CONVERSION;
   static bool s_bInitied = false;
   if( !s_bInitied ) {
      s_bInitied = true;
      Gdiplus::GdiplusStartupInput gsi;
      Gdiplus::GdiplusStartupOutput gso;
      ULONG_PTR uToken = 0;
      Gdiplus::GdiplusStartup(&uToken, &gsi, &gso);
   }
   Gdiplus::Bitmap* pBitmap = NULL;
   if( HIWORD(bitmap.m_lpstr) != NULL ) {
      // Load from filename
      pBitmap = new Gdiplus::Bitmap(T2CW(bitmap.m_lpstr)); 
   }
   else if( type.m_lpstr != NULL && type.m_lpstr != RT_BITMAP ) {
      // Loading PNG, JPG resources etc
      WTL::CResource res;
      if( !res.Load(type, bitmap) ) return NULL;
      DWORD dwSize = res.GetSize();
      HANDLE hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
      if( hMemory == NULL ) return NULL;
      ::memcpy(::GlobalLock(hMemory), res.Lock(), dwSize);
      ::GlobalUnlock(hMemory);
      IStream* pStream = NULL; 
      if( FAILED( ::CreateStreamOnHGlobal(hMemory, TRUE, &pStream) ) ) {
         ::GlobalFree(hMemory);
         return FALSE;
      }
      pBitmap = new Gdiplus::Bitmap(pStream);
      pStream->Release();
   }
   else {
      // This only loads BMP resources
      pBitmap = new Gdiplus::Bitmap(_Module.GetResourceInstance(), (LPCWSTR) (UINT) bitmap.m_lpstr);
   }
   if( pBitmap == NULL ) return NULL;
   HBITMAP hBitmap = NULL;
   pBitmap->GetHBITMAP(NULL, &hBitmap); 
   delete pBitmap;
   return hBitmap;
}

ATLINLINE BOOL AtlReplaceColorDib32(HDC hDC, HBITMAP hBitmap, DWORD dwKey, DWORD dwReplace = 0UL, DWORD dwShowColor = 0xFF000000)
{
   ::GdiFlush();
   BITMAP bm = { 0 };
   ::GetObject(hBitmap, sizeof(BITMAP), &bm);
   int cy = abs(bm.bmHeight);
   BITMAPINFO bmi = { 0 };
   bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   BOOL bRes = ::GetDIBits(hDC, hBitmap, 0, cy, NULL, &bmi, DIB_RGB_COLORS);
   if( !bRes ) return FALSE;
   LPVOID pSrcBits = ::LocalAlloc(LPTR, bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * sizeof(DWORD));
   if( pSrcBits == NULL ) return FALSE;
   LPDWORD pBits = (LPDWORD) pSrcBits;
   bmi.bmiHeader.biBitCount = 32;  // Ask for ARGB codec
   bmi.bmiHeader.biCompression = BI_RGB;
   bRes = ::GetDIBits(hDC, hBitmap, 0, cy, pSrcBits, &bmi, DIB_RGB_COLORS);
   if( !bRes || bmi.bmiHeader.biBitCount != 32 ) {
      ::GlobalFree(pBits);
      return FALSE;
   }
   for( int y = 0; y < abs(bmi.bmiHeader.biHeight); y++ ) {
      for( int x = 0; x < bmi.bmiHeader.biWidth; x++ ) {
         if( *pBits != dwKey ) 
            *pBits = *pBits | dwShowColor; 
         else 
            *pBits = dwReplace;
         pBits++;
      }
   }
   bRes = ::SetDIBits(hDC, hBitmap, 0, cy, pSrcBits, &bmi, DIB_RGB_COLORS);
   ::GlobalFree(pBits);
   return bRes;
}

ATLINLINE HFONT AtlGetDefaultShellFont()
{
   static CFont s_font;
   if( s_font.IsNull() ) {
      CLogFont lf;
      lf.SetMessageBoxFont();
      s_font.CreateFontIndirect(&lf);
   }
   return s_font;
}


//////////////////////////////////////////////////////////////////////
// GDI+ ImageProperty helper function
//

#define PropertyTagExifRating            0x4746
#define PropertyTagExifRatingPercent     0x4746


#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)

ATLINLINE void GdipGetBitmapProperty(Gdiplus::Image* pImage, UINT uPropId, CString& sResult)
{
   ATLASSERT(pImage);
   UINT size = pImage->GetPropertyItemSize(uPropId);
   if( size == 0 ) return;
   Gdiplus::PropertyItem* pPropItem = (Gdiplus::PropertyItem*) malloc(size);
   ATLASSERT(pPropItem);
   pImage->GetPropertyItem(uPropId, size, pPropItem);
   if( pPropItem->type == PropertyTagTypeASCII ) sResult = (LPCSTR) pPropItem->value;
   free(pPropItem);
}

#endif   // _WTL_USE_CSTRING

ATLINLINE void GdipGetBitmapProperty(Gdiplus::Image* pImage, UINT uPropId, long& lResult)
{
   ATLASSERT(pImage);
   UINT size = pImage->GetPropertyItemSize(uPropId);
   if( size == 0 ) return;
   Gdiplus::PropertyItem* pPropItem = (Gdiplus::PropertyItem*) malloc(size);         
   ATLASSERT(pPropItem);
   pImage->GetPropertyItem(uPropId, size, pPropItem);
   if( pPropItem->type == PropertyTagTypeShort ) lResult = * (short*) pPropItem->value;
   if( pPropItem->type == PropertyTagTypeLong ) lResult = * (long*) pPropItem->value;
   free(pPropItem);
}

ATLINLINE Gdiplus::Status GdipCreateDIBFromGdiplusBitmap(Gdiplus::Bitmap* pBitmap, HBITMAP* hbmReturn)
{
   Gdiplus::Status status;
   UINT uWidth = pBitmap->GetWidth();
   UINT uHeight = pBitmap->GetHeight();
   Gdiplus::Rect rect(0, 0, uWidth, uHeight);
   Gdiplus::PixelFormat format = pBitmap->GetPixelFormat();
   int bits_per_pixel;
   switch( format ) {
   case PixelFormat1bppIndexed: bits_per_pixel = 1; break;
   case PixelFormat4bppIndexed: bits_per_pixel = 4; break;
   case PixelFormat8bppIndexed: bits_per_pixel = 8; break;
   case PixelFormat16bppARGB1555:
   case PixelFormat16bppGrayScale:
   case PixelFormat16bppRGB555:
   case PixelFormat16bppRGB565: bits_per_pixel = 16; break;
   case PixelFormat24bppRGB:    bits_per_pixel = 24; break;
   case PixelFormat32bppARGB:
   case PixelFormat32bppPARGB:
   case PixelFormat32bppRGB:    bits_per_pixel = 32; break;
   case PixelFormat48bppRGB:    bits_per_pixel = 48; break;
   case PixelFormat64bppARGB:
   case PixelFormat64bppPARGB:  bits_per_pixel = 64; break;
   default: return Gdiplus::InvalidParameter;
   }
   Gdiplus::BitmapData data;
   status = pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, format, &data);
   if( status != Gdiplus::Ok ) return status;
   HBITMAP ret = ::CreateBitmap(uWidth, uHeight, 1, bits_per_pixel, data.Scan0);
   status = pBitmap->UnlockBits(&data);
   if( ret == NULL ) return Gdiplus::Win32Error;
   if( status != Gdiplus::Ok ) {
      ::DeleteObject(ret);
      return status;
   }
   *hbmReturn = ret;
   return Gdiplus::Ok;
}

ATLINLINE Gdiplus::Bitmap* GdipGetPngBitmapFromResource(ATL::_U_STRINGorID bitmap, ATL::_U_STRINGorID type = (UINT) 0)
{
   CResource res;
   if( !res.Load(type, bitmap) ) return NULL;
   DWORD dwSize = res.GetSize();
   HANDLE hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
   if( hMemory == NULL ) return NULL;
   ::memcpy(::GlobalLock(hMemory), res.Lock(), dwSize);
   ::GlobalUnlock(hMemory);
   IStream* pStream = NULL; 
   if( FAILED( ::CreateStreamOnHGlobal(hMemory, TRUE, &pStream) ) ) {
      ::GlobalFree(hMemory);
      return FALSE;
   }
   Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(pStream);
   pStream->Release();
   return pBitmap;
}

ATLINLINE void ClipBitmapImage( CBitmapHandle bmp )
{
	CImage img;
	img.Attach(bmp);

	HBITMAP hDDB;
	if( img.IsDIBSection() )
	{
		DIBSECTION ds;
		GetObject( img, sizeof(ds), &ds );
		ds.dsBmih.biCompression = BI_RGB; // change compression from BI_BITFIELDS to BI_RGB

		// Convert the DIB to a device dependent bitmap(i.e., DDB)
		HDC hDC = ::GetDC(NULL);
		hDDB = CreateDIBitmap( hDC, &ds.dsBmih, CBM_INIT,
			ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS );
		::ReleaseDC(NULL, hDC);
	} else {
		hDDB = bmp;
	}

	
	// Open the clipboard
	::OpenClipboard(NULL);
	::EmptyClipboard();

	// Place the handle to the DDB on the clipboard
	::SetClipboardData(CF_BITMAP, hDDB);

	// Do not delete the hDDB handle, the clipboard owns it
	::CloseClipboard();
	img.Detach();
}

ATLINLINE void ClipBitmapImage( Bitmap* bmp )
{
	HBITMAP hbmp;
	if( GdipCreateDIBFromGdiplusBitmap(bmp, &hbmp)!=Gdiplus::Ok )
	{
		ATLASSERT(FALSE);
		return;
	}
	ClipBitmapImage(hbmp);
}

ATLINLINE void ClipImageList( CImageList& imglist )
{
	IMAGEINFO imginfo;
	imglist.GetImageInfo( 0, &imginfo );
	CRect rc = imginfo.rcImage;

	CWindowDC dc( NULL );
	CMemoryDC memDC( dc, rc );
	memDC.FillSolidRect( &rc, RGB(255,255,255) );
	imglist.Draw(memDC, 0, 0, 0, 0 );

	::OpenClipboard( NULL );
	::EmptyClipboard();
	::SetClipboardData(CF_BITMAP, memDC.m_bmp.m_hBitmap);
	::CloseClipboard();
}