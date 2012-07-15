// defines.h

#define JOIN( X, Y ) JOIN2(X,Y)
#define JOIN2( X, Y ) X##Y


#ifdef __ATLCOLL_H__
	typedef CAtlMap<CString, bool, CStringRefElementTraits<CString>> CAtlStringMap;
#endif


#undef BEGIN_MSG_MAP
#define BEGIN_MSG_MAP BEGIN_MSG_MAP_EX
#define WMU_DEBUG_MSG (WM_APP + 585)


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


#define DWORD_FILL(byte_size)										\
	static_assert((byte_size)%sizeof(DWORD) == 0,					\
					"Byte size is not DWORD multiple");			\
	struct {														\
		DWORD JOIN(FILL_GAP, JOIN(__LINE__,JOIN(_, byte_size)))		\
		[(byte_size)/sizeof(DWORD)];								\
	};
