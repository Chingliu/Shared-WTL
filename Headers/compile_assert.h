// compile_assert.h

#pragma once


/*
namespace st_assert
{
    template <bool> struct STATIC_ASSERT_FAILURE;
    template <> struct STATIC_ASSERT_FAILURE<true> { enum { value = 1 }; };

    template<int x> struct static_assert_test{};
}

#define COMPILE_ASSERT(x) \
    typedef ::st_assert::static_assert_test<\
        sizeof(::st_assert::STATIC_ASSERT_FAILURE< (bool)( x ) >)>\
            JOIN(_static_assert_typedef, __LINE__)


#define VERIFY_EXPLICIT_CAST(from, to) COMPILE_ASSERT(sizeof(from) == sizeof(to))
*/

// as of VS2010
#define COMPILE_ASSERT(x) static_assert(x, "Compile assertion");
