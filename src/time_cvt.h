#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <xlocale>
#if defined(WIN32) || defined(_WIN32)
#if _MSC_VER < 1800
#error "should use VS2013 or above version implementation"
#endif
#else
#if __cplusplus < 201103L
#error "should use C++11 implementation"
#endif
#endif//
namespace prudens{
    using namespace std::chrono;
    uint64_t timestamp()
    {
        /*system_clock::now() is current time by second,
          and default construct is start GMT time */
        return duration_cast<seconds>(
            system_clock::now() - time_point<system_clock>() ).count();

        // return time( nullptr );
    }

    std::string timestamptostring( uint64_t ts )
    {
        std::time_t t = ts;
        std::tm tm = *std::localtime( &t );
        ::std::stringstream  ss;
        ss.imbue( std::locale( "zh-CN" ) );
        ss << std::put_time( &tm, " %c" );
        return ss.str();
    }

    // º∆ ±∆˜
    class ctime
    {
    public:
        ctime() : m_begin( high_resolution_clock::now() ) {}
        void reset() { m_begin = high_resolution_clock::now(); }

        //ƒ¨»œ ‰≥ˆ∫¡√Î
        int64_t elapsed() const
        {
            return duration_cast<milliseconds>( high_resolution_clock::now() - m_begin ).count();
        }

        //Œ¢√Î
        int64_t elapsed_micro() const
        {
            return duration_cast<microseconds>( high_resolution_clock::now() - m_begin ).count();
        }

        //ƒ…√Î
        int64_t elapsed_nano() const
        {
            return duration_cast<nanoseconds>( high_resolution_clock::now() - m_begin ).count();
        }

        //√Î
        int64_t elapsed_seconds() const
        {
            return duration_cast<seconds>( high_resolution_clock::now() - m_begin ).count();
        }

        //∑÷
        int64_t elapsed_minutes() const
        {
            return duration_cast<minutes>( high_resolution_clock::now() - m_begin ).count();
        }

        // ±
        int64_t elapsed_hours() const
        {
            return duration_cast<hours>( high_resolution_clock::now() - m_begin ).count();
        }

    private:
        time_point<high_resolution_clock> m_begin;
    };

}