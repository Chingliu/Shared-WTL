#pragma once

#include "sciter-x-host-callback.h"


#ifdef SCITER_LEGACY_SUPPORT
	namespace sciter
	{
		template <typename BASE>
		struct host : notification_handler<BASE> {};
	}

	#define SciterClassName SciterClassNameW
	#define SCITER_DLL L"sciter-x.dll"
#else
	#define SCITER_DLL L"sciter32.dll"
#endif


typedef CWinTraitsOR<WS_CHILD | WS_VISIBLE, 0, // WS_EX_TRANSPARENT WS_EX_COMPOSITED WS_CLIPCHILDREN WS_CLIPSIBLINGS
				CNullTraits> CSciterWinTraits; // so we can pass styles at Create()

class CSciterHostWnd :
	public CWindowImpl<CSciterHostWnd, CWindow, CSciterWinTraits>,
	public sciter::host<CSciterHostWnd>
{
public:
	CSciterHostWnd();

// Registering/superclass mechanics, same as DECLARE_WND_SUPERCLASS but with some problems corrections
public:
	struct SciteClassRegister : // should have no data members!
		public ATL::CWndClassInfo, 
		public AtlModuleRegisterWndClassInfoParamW
	{
		static BOOL GetClassInfoEx(_In_opt_ HINSTANCE hinst, _In_z_ PCXSTR lpszClass, _Out_ WNDCLASSEX* lpwcx)
		{
			if( !hinst )
				return FALSE;

			if( CString(::SciterClassName())==lpszClass )
			{
				hinst = ::GetModuleHandle(SCITER_DLL);
				ATLENSURE(hinst);

			#ifdef DEBUG
				BOOL res = ::GetClassInfoExW(hinst, lpszClass, lpwcx);
				ATLENSURE(res);
				//lpwcx->style &= ~CS_OWNDC;
				//lpwcx->hbrBackground = (HBRUSH) ::GetStockObject(DKGRAY_BRUSH);
				return res;
			#endif
			}

			return ::GetClassInfoExW(hinst, lpszClass, lpwcx);
		}

		ATOM Register(_In_ WNDPROC* p)
		{
			return AtlModuleRegisterWndClassInfoT<SciteClassRegister>(
				&_AtlBaseModule, &_AtlWinModule, this, p, *this);
		}
	};

	static SciteClassRegister& GetWndClassInfo()
	{
		static CString strSuperClass( ::SciterClassName() );// must be global

		static ATL::CWndClassInfo wc = 
		{
			{ sizeof(WNDCLASSEX), 0, StartWindowProc,
				0, 0, NULL, NULL, NULL, NULL, NULL, L"SCITER_host" WND_CLASS_SUFIX, NULL },
			strSuperClass, NULL, NULL, TRUE, 0, _T("")
		};
		return *static_cast<SciteClassRegister*>(&wc);
	}

// Data members
private:
	bool m_frame_composited;

// Maps
public:
	BEGIN_MSG_MAP(CSciterHostWnd)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
	END_MSG_MAP()

// Overrides and mix-ins
public:
	HWND get_hwnd() { return m_hWnd; };
	HINSTANCE get_resource_instance() { return ModuleHelper::GetResourceInstance(); };
	LRESULT on_callback_host(LPSCN_CALLBACK_HOST pnmld);// can be switched to virtual/override form

// Handler prototypes
protected:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnEraseBkgnd(CDCHandle dc);
};
