#pragma once
#ifndef COMMON_DEFINES_H
#define COMMON_DEFINES_H
#include <type_traits>
#include <cstddef>
namespace prudens
{
    #if defined(_WIN32) && defined(_DEBUG)
    #include <crtdbg.h>
    #define TRACE(...)  _CrtDbgReport( _CRT_WARN, __FILE__, __LINE__, "min_max_heap",__VA_ARGS__  );
    #else
    #define TRACE(...) printf(__VA_ARGS__);
    #endif


    #if defined(_DEBUG) || defined(DEBUG)
    #define ASSERT(p) assert(p)
    #else 
    #define ASSERT( p ) (void)0;
    #endif // DEBUG

    template <class T, std::size_t N>
     std::size_t countof( T( & )[N] )
    {
        return N;
    }


}
#endif // !COMMON_DEFINES_H
