#pragma once


template<class T, bool TInvalidate=true>
class CMixHover
{
public:
	CMixHover()
	{
		m_bMouseTracking = FALSE;
	}

// Data members
protected:
	BOOL m_bMouseTracking;

// Override interface
public:
	void OnBeginHover() { }
	void OnEndHover() { }

// Maps
public:
	BEGIN_MSG_MAP(CMixHover<T>)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
	END_MSG_MAP()

// Handler prototypes
public:
	void OnMouseMove(UINT nFlags, CPoint point)
	{
		if( !m_bMouseTracking )
		{
			T* pT = static_cast<T*>(this);

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = pT->m_hWnd;

			if( ::_TrackMouseEvent(&tme) )
			{
				m_bMouseTracking = TRUE;
				pT->OnBeginHover();
				if( TInvalidate )
					pT->Invalidate();
			}
		}

		SetMsgHandled( FALSE );
	}

	LRESULT OnMouseLeave()
	{
		m_bMouseTracking = FALSE;

		T* pT = static_cast<T*>(this);
		pT->OnEndHover();
		if( TInvalidate )
			pT->Invalidate();

		SetMsgHandled( FALSE );
		return TRUE;
	}
};
