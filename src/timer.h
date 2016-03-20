#pragma once
#include <chrono>
#include <thread>
#include <mutex>
#include <time_cvt.h>
#include <memory>
#include <queue>
#include <functional>
#include <atomic>
#include "time_cvt.h"
#include "min_max_heap.hpp"
#include <limits>
namespace prudens
{
    uint64_t now()
    {
        return duration_cast<milliseconds>(
            system_clock::now() - time_point<system_clock>() ).count();
    }
    typedef  std::function<void( uint32_t, void* ) > timer_callback;
    struct timer_msg
    {
#define MAX_MACRO_COMPILE_SUPPORT
        uint32_t timer_id;
        uint32_t elapsed;
        uint64_t expired;
        bool repeat;
        void* userdata;
        timer_callback action;
        timer_msg() :timer_id( 0 ),
            elapsed( 0 ),
            expired( std::numeric_limits<uint32_t>::max MAX_MACRO_COMPILE_SUPPORT() ),
            repeat( false ),
            userdata( nullptr ) {}

        bool operator < ( const timer_msg& other )const
        {
            return this->elapsed > other.elapsed;
        }
    };
    class timer
    {
    public:

        class TimerQueue
        {
        public:
            TimerQueue() {
                _stop.exchange( false );
            }
            void run()
            {
                while ( true )
                {
                    if (_stop)
                    {
                        return;
                    }
                    while ( true )
                    {
                        if (_stop)
                        {
                            return;
                        }
                        timer_msg msg;
                        uint64_t t = now();
                        {
                            std::lock_guard<std::mutex> guard( _mutex );
                            if ( !_timers.empty() )
                            {
                                msg = _timers.top();
                            }
                        }
                        if ( msg.expired >= t )
                        {
                            break;
                        }
                        if ( msg.action )
                        {
                            msg.action( msg.timer_id, msg.userdata );
                        }
                        {
                            std::lock_guard<std::mutex> guard( _mutex );
                            if ( !_timers.empty() )
                            {
                                _timers.pop();
                                if ( msg.repeat )
                                {
                                    msg.expired = msg.elapsed + now();
                                    _timers.push( msg );
                                }
                            }

                        }
                    }
                    int64_t sleep_time = 30;//列表为空的时候就睡眠30ms
                    {
                        std::lock_guard<std::mutex> guard( _mutex );
                        if ( !_timers.empty() )
                        {
                            sleep_time = _timers.top().expired - now();
                        }
                    }
                    std::this_thread::sleep_for( milliseconds( sleep_time ) );
                }
            }

            void push( timer_msg& msg )
            {
                std::lock_guard<std::mutex> guard( _mutex );
                _timers.push( msg );
            }
            void clear()
            {
                std::lock_guard<std::mutex> guard( _mutex );
                while ( !_timers.empty() )
                {
                    _timers.pop();
                }
            }
            void remove( uint32_t time_id )
            {
                std::lock_guard<std::mutex> guard( _mutex );
                _timers.remove_if( [&] ( const timer_msg& msg )
                {
                    return msg.timer_id == time_id;
                } );
            }
        private:
            std::atomic_bool _stop;
            prudens::max_min_heap<timer_msg> _timers;
            std::mutex _mutex;
        };

        timer()
        {
            _queue = std::make_shared<TimerQueue>();
            std::thread( [] ( std::shared_ptr<TimerQueue> queue )
            {
                queue->run();
            },_queue ).detach();
        }

        ~timer()
        {
            _queue->clear();
        }

        uint32_t start( int elapsed_ms, bool repeat, timer_callback action,void* userdata )
        {
            timer_msg msg;
            msg.elapsed = elapsed_ms ;
            msg.expired = elapsed_ms + now();
            msg.repeat = repeat;
            msg.userdata = userdata;
            msg.timer_id = s_timer_id++;
            msg.action = action;
            _queue->push( msg );
            return msg.timer_id;
        }

        void remove( uint32_t time_id )
        {
            _queue->remove( time_id );
        }
    private:
        std::shared_ptr<TimerQueue> _queue;
        static uint32_t s_timer_id;
    };
    uint32_t timer::s_timer_id = 1;
}