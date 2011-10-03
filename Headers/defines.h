// defines.h

#define JOIN( X, Y ) JOIN2(X,Y)
#define JOIN2( X, Y ) X##Y
//#include "compile_assert.h"


#ifdef __ATLCOLL_H__
	typedef CAtlMap<CString, BOOL, CStringRefElementTraits<CString>> CAtlStringMap;
#endif


#undef BEGIN_MSG_MAP
#define BEGIN_MSG_MAP BEGIN_MSG_MAP_EX


#ifdef _DEBUG
	#define DEBUG_ONLY(f)      (f)
	#define WND_CLASS_SUFIX L"-WTL-CLASS-DEBUG"
#else
	#define DEBUG_ONLY(f)      ((void)0)
	#define WND_CLASS_SUFIX L"-WTL-CLASS-RELEASE"
#endif


#define DWORD_FILL(byte_size)								\
	static_assert((byte_size)%sizeof(DWORD) == 0,			\
					"Byte size is not a DWORD multiple");	\
	struct {												\
		JOIN(DWORD fill_gap, __LINE__)						\
		[(byte_size)/sizeof(DWORD)];						\
	};
