#pragma once


template<class T>
class CMixParentBkg
{
public:
	CMixParentBkg()
	{
	};

// Data members
protected:
	CImage m_imgBack; // Bitmap background

// Interface
public:
	// TODO: It might be the case where the background needed is not from the direct parent,
	//		 but one that is higher in the parent hierarchy, so it will need an additional parameter: HWND hwndParent, or maybe an int
	void DrawBackground(HDC dc)
	{
		T* pT = static_cast<T*>(this);
		CClientRect rc(pT->m_hWnd);

		if( m_imgBack.IsNull() )
		{
			// Get and adjusts RECT's
			CWindow wndParent(pT->GetParent());
			CWindowRect rcParent(wndParent);
			CWindowRect rcBltSource(pT->m_hWnd);
			::MapWindowPoints(NULL, wndParent, (LPPOINT) &rcBltSource, 2);

			// Create CImage
			CImage imgPrint;
			imgPrint.Create(rcParent.Width(), rcParent.Height(), 32, CImage::createAlphaChannel);
			m_imgBack.Create(rcBltSource.Width(), rcBltSource.Height(), 32, CImage::createAlphaChannel);

			// Copy a portion of the parent DC as this window background
			CDCHandle dc_print( imgPrint.GetDC() );
			CDCHandle dc_back( m_imgBack.GetDC() );

			wndParent.Print(dc_print, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);
			imgPrint.ReleaseDC();
			imgPrint.BitBlt(dc_back, rc, rcBltSource.TopLeft() );
			m_imgBack.ReleaseDC();
		}
		m_imgBack.BitBlt(dc, 0, 0);
	};
};

