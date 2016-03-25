#pragma once
#include <limits>
#include <condition_variable>    // std::condition_variable
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>
#include <atomic>
#include "min_max_heap.hpp"//仿照消息队列queue，添加了几个函数，他是最小堆。代码就不上传了
#if defined(_WIN32) && defined(_DEBUG)
#define TRACE(...)  _CrtDbgReport( _CRT_WARN, __FILE__, __LINE__, "min_max_heap",__VA_ARGS__  );
#else
#define TRACE(...) printf(__VA_ARGS__);
#endif
namespace snail
{
    namespace audio
    {
        using namespace  std::chrono;
        using namespace std::placeholders;
        uint64_t now()
        {
            return duration_cast<milliseconds>(
                system_clock::now() - time_point<system_clock>() ).count();
        }
        typedef  std::function<void( uint32_t ) > TaskExecute;
        struct Task
        {
#define MAX_MACRO_COMPILE_SUPPORT
            uint32_t id;
            uint32_t elapsed;
            uint64_t expired;
            bool repeat;
            TaskExecute execute;
            Task() :id( 0 ),
                elapsed( 0 ),
                expired( std::numeric_limits<uint64_t>::max MAX_MACRO_COMPILE_SUPPORT() ),
                repeat( false )
            {
            }

            bool operator < ( const Task& other )const
            {
                return this->elapsed < other.elapsed;
            }
        };
        class AsynTask
        {
        public:
            class TaskQueue
            {
            public:
                TaskQueue()
                {
                    _stop.store( false );
                    _flag.store( false );
                }
                ~TaskQueue()
                {
                    stop();
                }
                void Run()
                {
                    while ( true )
                    {
                        while ( true )
                        {
                            if ( _stop )
                            {
                                return;
                            }
                            Task task;
                            uint64_t t = now();
                            {
                                std::lock_guard<std::mutex> guard( _lock );
                                if ( !_tasks.empty() )
                                {
                                    task = _tasks.top();
                                }
                                if ( task.expired < t )
                                {
                                    _tasks.pop();
                                    if ( task.repeat )
                                    {
                                        task.expired = task.elapsed + t;
                                        _tasks.push( task );
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }

                            if ( task.execute )
                            {
                                task.execute( task.id );
                            }
                        }
                        if ( _stop )
                        {
                            return;
                        }
                        int32_t sleep_time = std::numeric_limits<int32_t>::max MAX_MACRO_COMPILE_SUPPORT();//列表为空的时候就睡眠30ms

                        std::unique_lock <std::mutex> lck( _lock );
                        if ( !_tasks.empty() )
                        {
                            sleep_time = static_cast<int32_t>( _tasks.top().expired - now() );
                            if ( sleep_time < 0 )
                            {
                                continue;
                            }
                        }
                        if ( _flag.load() == false )
                        {
                            _cond_variable.wait_for( lck, milliseconds( sleep_time ),
                                                     [this] ()->bool { return this->_flag.load(); } );
                        }
                        if ( _stop )
                        {
                            return;
                        }
                        _flag.store( false );
                    }
                }

                void push( Task& task )
                {
                    std::unique_lock <std::mutex> lck( _lock );
                    _tasks.push( task );
                    _flag.store( true );
                    _cond_variable.notify_all();

                }

                void clear()
                {
                    std::lock_guard<std::mutex> guard( _lock );
                    while ( !_tasks.empty() )
                    {
                        _tasks.pop();
                    }
                }

                void remove( uint32_t task_id )
                {
                    std::lock_guard<std::mutex> guard( _lock );
                    _tasks.remove_if( [&] ( const Task& task )
                    {
                        return task.id == task_id;
                    } );
                }

                void stop()
                {
                    std::unique_lock <std::mutex> lck( _lock );
                    _flag.store( true );
                    _stop.store( true );
                    _cond_variable.notify_all();
                }
            private:
                std::atomic_bool _stop;
                max_min_heap<Task> _tasks;
                std::mutex _lock;
                std::condition_variable _cond_variable;
                std::atomic_bool _flag;
            };

            AsynTask()
            {
                _task_queue = std::make_shared<TaskQueue>();
                std::thread( [] ( std::shared_ptr<TaskQueue> queue )
                {
                    queue->Run();
                }, _task_queue ).detach();
            }

            ~AsynTask()
            {
                _task_queue->stop();
            }

            uint32_t PostTask( int elapsed_ms, bool repeat, TaskExecute execute )
            {
                Task task;
                task.elapsed = elapsed_ms;
                task.expired = elapsed_ms + now();
                task.repeat = repeat;
                task.id = s_task_id++;
                task.execute = execute;
                _task_queue->push( task );
                return task.id;
            }

            void RemoveTask( uint32_t task_id )
            {
                _task_queue->remove( task_id );
            }

            void RemoveAllTask()
            {
                _task_queue->clear();
            }
        private:

            std::shared_ptr<TaskQueue> _task_queue;
            static uint32_t s_task_id;
        };
        uint32_t AsynTask::s_task_id = 1;
    }
}