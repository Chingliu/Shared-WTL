// defines.h

#define JOIN( X, Y ) JOIN2(X,Y)
#define JOIN2( X, Y ) X##Y


#ifdef __ATLCOLL_H__
	typedef CAtlMap<CString, bool, CStringRefElementTraits<CString>> CAtlStringMap;
#endif


#undef BEGIN_MSG_MAP
#define BEGIN_MSG_MAP BEGIN_MSG_MAP_EX
#define WMU_DEBUG_MSG (WM_APP + 585)
#define VERIFY(x) ATLVERIFY(x)


#ifdef _DEBUG
	#define ASSERT(x) do { if (!(x)) __debugbreak(); } while (false)
	#define DEBUG_ONLY(f) (f)
	#define RELEASE_ONLY(f) __noop
	#define WND_CLASS_SUFIX L"-APPCLASS_MIDI_DEBUG"
#else
	#define ASSERT(x) __noop
	#define DEBUG_ONLY(f) __noop
	#define RELEASE_ONLY(f) (f)
	#define WND_CLASS_SUFIX L"-APPCLASS_MIDI_RELEASE"
#endif


#undef ATLENSURE // ATL way throw an exception, what we really want is to terminate the application
#define ATLENSURE(x) ENSURE(x)
#ifdef DEBUG
	#define ENSURE(x) ASSERT(x)
#else
	#ifdef _CRASHRPT_H_
		#define ENSURE(x) do { if (!(x)) crEmulateCrash(CR_CPP_TERMINATE_CALL); } while (false)
	#else
		#define ENSURE(x) do { if (!(x)) terminate(); } while (false)
	#endif
#endif


namespace WTL
{
	namespace RunTimeHelper
	{
		inline bool IsWin8()
		{
			OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
			BOOL bRet = ::GetVersionEx(&ovi);
			return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6) && (ovi.dwMinorVersion >= 2));
		}
	}
}
