#pragma once
#ifdef _WIN32
#include <iostream>
#include <windows.h>

inline std::ostream& blue( std::ostream &s )
{
    HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hStdout, FOREGROUND_BLUE
                             | FOREGROUND_GREEN | FOREGROUND_INTENSITY );
    return s;
}

inline std::ostream& red( std::ostream &s )
{
    HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hStdout,
                             FOREGROUND_RED | FOREGROUND_INTENSITY );
    return s;
}

inline std::ostream& green( std::ostream &s )
{
    HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hStdout,
                             FOREGROUND_GREEN | FOREGROUND_INTENSITY );
    return s;
}

inline std::ostream& yellow( std::ostream &s )
{
    HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hStdout,
                             FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY );
    return s;
}

inline std::ostream& white( std::ostream &s )
{
    HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hStdout,
                             FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
    return s;
}

inline std::ostream& reset( std::ostream&s )
{
    HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hStdout,
                             FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
    return s;
}
struct color {
    color( WORD attribute ) :m_color( attribute ) {};
    WORD m_color;
};

template <class _Elem, class _Traits>
std::basic_ostream<_Elem, _Traits>&
operator<<( std::basic_ostream<_Elem, _Traits>& i, color& c )
{
    HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hStdout, c.m_color );
    return i;
}
#else
//the following are UBUNTU/LINUX ONLY terminal color codes.
#define reset   "\033[0m"
#define black   "\033[30m"      /* Black */
#define red     "\033[31m"      /* Red */
#define green   "\033[32m"      /* Green */
#define yellow  "\033[33m"      /* Yellow */
#define blue    "\033[34m"      /* Blue */
#define magenta "\033[35m"      /* Magenta */
#define cyan    "\033[36m"      /* Cyan */
#define white   "\033[37m"      /* White */
#define boldblack   "\033[1m\033[30m"      /* Bold Black */
#define boldred     "\033[1m\033[31m"      /* Bold Red */
#define boldgreen   "\033[1m\033[32m"      /* Bold Green */
#define boldyellow  "\033[1m\033[33m"      /* Bold Yellow */
#define boldblue    "\033[1m\033[34m"      /* Bold Blue */
#define boldmagenta "\033[1m\033[35m"      /* Bold Magenta */
#define boldcyan   "\033[1m\033[36m"      /* Bold Cyan */
#define boldwhite   "\033[1m\033[37m"      /* Bold White */
#endif
// Copyleft Vincent Godin