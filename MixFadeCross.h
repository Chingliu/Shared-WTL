#pragma once


// Debug options
#ifdef DEBUG
	#define MIXFADE_TRACE
	#define MIXFADE_CLIP
#else
	#undef MIXFADE_TRACE
	#undef MIXFADE_CLIP
#endif

#ifdef MIXFADE_TRACE
	#include <typeinfo>
#endif
#ifdef MIXFADE_CLIP
	#include "ToolsDrawing\ImageHelpers.h"
#endif



// Fading funcionality:
// -a cross fade ..

template<class T>
class CMixFadeCross
{
public:
	CMixFadeCross()
	{
		m_bSetupArea = false;
		m_bAnimating = false;
	};

private:
	enum { TIMERID = 94 };
	enum { SPEED = 15 }; // ms

// Interface
public:
	inline bool IsAreaSetuped() { return m_bSetupArea; }
	inline bool IsAnimating() { return m_bAnimating; }

	void SetupRenderArea(const CRect& rcTarget, bool canBeSame=false)
	{
		if( canBeSame && m_bSetupArea )
			return;

		m_rc_area = rcTarget;
		m_bSetupArea = true;
	}

	void SwapDirection()
	{
	}

// Data members
protected:
	bool m_bSetupArea;
	bool m_bAnimating;

	CRect m_rc_area;
	CBitmap m_render1;
	CBitmap m_render2;

// Maps
public:
	BEGIN_MSG_MAP(CMixFadeCross)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

// Handler prototypes
public:
	void OnTimer(UINT nIDEvent)
	{
		if( nIDEvent == TIMERID )
		{
			T* pT = static_cast<T*>(this);
			pT->Invalidate(FALSE);
			pT->UpdateWindow();
			return;
		}
		SetMsgHandled(FALSE);
	};
};
