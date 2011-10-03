#pragma once

// Debug options
#ifdef DEBUG
	#define MIXFADE_TRACE
	#define MIXFADE_CLIP
#endif


#ifdef MIXFADE_TRACE
	#include <typeinfo>
#endif
#ifdef MIXFADE_CLIP
	#include "ToolsDrawing\ImageHelpers.h"
#endif



// Fading funcionality:
// -it is NOT a cross fade between two different images
// -this class provides fade IN and OUT of the same image over the duration, so you can NOT change the image being faded during the transition

template<class T>
class CMixFadeSimple
{
public:
	enum FadeWay { FadeOut, FadeIn }; //move out of the class declaration?

	CMixFadeSimple()
	{
		m_bSetupArea = false;
		m_bAnimating = false;
		m_uFadeInitial = m_uFadePhase = 0;

		memset(&m_BlendFormat, 0, sizeof(BLENDFUNCTION));
		m_BlendFormat.BlendOp = AC_SRC_OVER; 
		m_BlendFormat.BlendFlags = 0; 
		m_BlendFormat.SourceConstantAlpha = 255;
		m_BlendFormat.AlphaFormat = AC_SRC_ALPHA;
	};

private:
	enum { TIMERID = 6 };
	enum { SPEED = 15 }; // ms

// Interface
public:
	inline bool IsAreaSetuped() { return m_bSetupArea; }
	inline bool IsAnimating() { return m_bAnimating; }

	// >>> Can be called event at the constructor
	// >>> Method parameters
	// rcTarget		: target bliting area and render size, define here an offset from origin corner (0,0) where bliting should occur
	// canBeSame	: for when SetupRenderArea() is called multiple times at WM_CREATE always with the same size
	void SetupRenderArea(const CRect& rcTarget, bool canBeSame=false)
	{
		if( canBeSame && m_bSetupArea )
			return;
		if( m_render_ddb && (rcTarget.Width()>m_render_rc.Width() || rcTarget.Height()>m_render_rc.Height()) )
			ATLVERIFY( m_render_ddb.DeleteObject() ); //maybe leak BUG, might not be deleting cause it is selected in the DC
		if( !m_render_ddb )
			m_render_ddb.CreateBitmap(rcTarget.Width(), rcTarget.Height(), 1, 32, NULL);
		if( !m_render_dc )
			m_render_dc.CreateCompatibleDC();
		m_render_dc.SelectBitmap(m_render_ddb);

		m_render_rc = rcTarget;
		m_bSetupArea = true;
	};

	void RenderDraw(CDCHandle& dcTo)
	{
		ATLASSERT(!m_render_dc.IsNull());
		m_render_dc.FillSolidRect(&m_render_rc, 0); //clears the device surface
		dcTo = m_render_dc;

	#ifdef MIXFADE_CLIP

	#endif
	};

	// >>> As we cache the render, we get some special behaviors:
	//	- if currently animating the same FadeWay being requested, will just ignore the call
	//	- if currently animating and the oposite FadeWay is requested, will calculate the delta fade, so won't start from 255
	void BeginFadeAnimation(FadeWay way, UINT duration=400) //bool bForceSoft=false can be a possible additional flag
	{
		ATLASSERT(!m_render_dc.IsNull());

		if( m_bAnimating && way==m_eDirection )
			return;

		m_uFadeInitial = m_bAnimating ? 255-m_uFadePhase : 255;
		m_bAnimating = true;
		m_eDirection = way;
		m_uDuration = duration;
		m_dwFadeTick = ::GetTickCount();

		T* pT = static_cast<T*>(this);
		pT->SetTimer(TIMERID, SPEED);

	#ifdef MIXFADE_TRACE
		CString trace;
		trace.Format(L"\n%s >>> FadeInitial: %d", CString(typeid(T).name()), m_uFadeInitial);
		ATLTRACE(trace);
	#endif
	};

	void AnimationDraw(HDC dcRender)
	{
		ATLASSERT(!m_render_dc.IsNull());
		ATLASSERT(m_bAnimating);
		if( !IsAnimating() ) return;

		// New fade value for blending
		const float FadeTickRate = 255.0f / (float(m_uDuration) / float(SPEED));
		const float delta = (::GetTickCount() - m_dwFadeTick) / SPEED * FadeTickRate;
		m_uFadePhase = m_uFadeInitial - delta;
		if( m_uFadePhase < 0 )
			m_uFadePhase = 0;

	#ifdef MIXFADE_TRACE
		CString trace;
		trace.Format(L"\n%s >>> Delta: %f / Phase: %d", CString(typeid(T).name()), delta, m_eDirection==FadeOut ? m_uFadePhase : 255-m_uFadePhase);
		ATLTRACE(trace);
	#elif 0
		::OutputDebugString(trace);
	#endif

		// Blend the target
		m_BlendFormat.SourceConstantAlpha = m_eDirection==FadeOut ? m_uFadePhase : 255-m_uFadePhase;
		CDCHandle(dcRender).AlphaBlend(m_render_rc.left, m_render_rc.top, m_render_rc.Width(), m_render_rc.Height(), m_render_dc, 0, 0, m_render_rc.Width(), m_render_rc.Height(), m_BlendFormat);

		// Is animation done
		if( m_uFadePhase == 0 )
		{
			m_bAnimating = false;
			T* pT = static_cast<T*>(this);
			pT->KillTimer(TIMERID);
		}
	};

// Data members
protected:
	bool m_bSetupArea;
	bool m_bAnimating;
	FadeWay m_eDirection;
	UINT m_uDuration;

	int m_uFadeInitial;	// Static initial Fade
	int m_uFadePhase;	// Fade phase being in animation
	DWORD m_dwFadeTick;	// Start Fade tick

	CRect m_render_rc;
	CBitmap m_render_ddb;
	CDC m_render_dc;	// After the bitmap so it doesn't leak at destruction

	BLENDFUNCTION m_BlendFormat; // just the size of a DWORD =)

// Maps
public:
	BEGIN_MSG_MAP(CMixFadeSimple<T>)
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
