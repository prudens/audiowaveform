#pragma once
#include <limits>
#include <condition_variable>    // std::condition_variable
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>
#include <atomic>
#include "min_max_heap.hpp"
#define TRACE(...)  _CrtDbgReport( _CRT_WARN, __FILE__, __LINE__, "min_max_heap",__VA_ARGS__  );
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
            expired( std::numeric_limits<uint64_t>::max MAX_MACRO_COMPILE_SUPPORT() ),
            repeat( false ),
            userdata( nullptr ) {}

        bool operator < ( const timer_msg& other )const
        {
            return this->elapsed < other.elapsed;
        }
    };
    class timer
    {
    public:
        class TimerQueue
        {
        public:
            TimerQueue() {
                _stop.store( false );
                _flag.store( false );
            }
            void run()
            {
                while ( true )
                {
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
                    int32_t sleep_time = std::numeric_limits<int32_t>::max MAX_MACRO_COMPILE_SUPPORT ();//列表为空的时候就睡眠30ms
                    {
                        std::lock_guard<std::mutex> guard( _mutex );
                        if ( !_timers.empty() )
                        {
                            sleep_time = static_cast<int32_t>(_timers.top().expired - now());
                            if (sleep_time<0)
                            {
                                continue;
                            }
                        }
                    }
                    if (_stop)
                    {
                        return;
                    }
                    std::unique_lock <std::mutex> lck( _mutex );
                   // std::this_thread::sleep_for( milliseconds( sleep_time ) );
                    if ( _cv.wait_for( lck, milliseconds( sleep_time ), [this] ()->bool { return this->_flag.load(); } ) )
                    {
                        TRACE( "cv is time out:%d\n", sleep_time );
                    }
                    else
                    {
                        TRACE("cv is run\n");
                    }
                    _flag.store( false );
                }
            }

            void push( timer_msg& msg )
            {
                {
                    std::lock_guard<std::mutex> guard( _mutex );
                    _timers.push( msg );
                }
                {
                    std::unique_lock <std::mutex> lck( _mutex );
                    _flag.store( true);
                    _cv.notify_all();
                }
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
            void test()
            {
                std::lock_guard<std::mutex> guard( _mutex );
                timer_msg msg;
                msg.elapsed = 0;
                msg.expired = 10000;
                msg.repeat = false;
                for ( int i = 0; i < 100; i++ )
                {
                    msg.expired += 1;
                    _timers.push( msg );
                }
                while (!_timers.empty())
                {
                    
                    TRACE( "%u\n ", _timers.top().expired );
                    _timers.pop();
                }
            }
        private:
            std::atomic_bool _stop;
            prudens::max_min_heap<timer_msg> _timers;
            std::mutex _mutex;
            std::condition_variable _cv;
            std::atomic_bool _flag;
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
        void test()
        {
            _queue->test();
            
        }
    private:
        std::shared_ptr<TimerQueue> _queue;
        static uint32_t s_timer_id;
    };
    uint32_t timer::s_timer_id = 1;
}