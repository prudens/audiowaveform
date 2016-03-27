#pragma once
#ifndef MIN_MAX_HEAP_H
#define MIN_MAX_HEAP_H

#include <algorithm>
#include <vector>

namespace prudens{
    template<class T, class Cmp = std::less<T>>
    class max_min_heap
    {
        typedef size_t index;
    public:

        max_min_heap()
        {
        }

        ~max_min_heap()
        {
        }

        void pop()
        {
            if ( data_.empty() )
            {
                throw std::range_error( " data is already empty!" );
            }
            data_[0] = std::move( data_.back() );
            data_.pop_back();
            adjust( 0 );// sort again
        }

        T top()const
        {
            if ( data_.empty() )
            {
                throw std::range_error( " data is already empty!" );
            }
            return data_[0];
        }

        T& top()
        {
            if ( data_.empty() )
            {
                throw std::range_error( " data is already empty!" );
            }
            return data_[0];
        }


        void push( T &data )
        {
            data_.insert( data_.begin(), data );
            adjust( 0 );
        }

        void push( T&&data )
        {
            data_.insert( data_.begin(), data );
            adjust( 0 );
        }

        size_t size()const _NOEXCEPT{ return data_.size(); }

        bool empty() const _NOEXCEPT{ return  data_.empty(); }
        void clear()_NOEXCEPT{ data_.clear(); }
        void remove_if( std::function<bool( const T&value )> func )
        {
            while ( true )
            {
                auto it = std::find_if( data_.begin(), data_.end(), func );
                if ( it != data_.end() )
                {
                    data_.erase( it );
                }
                else
                {
                    break;
                }
            }
        }
    protected:
        void adjust( index node )
        {
            size_t size = data_.size();
            while ( true )
            {
                index l = left( node );
                index r = right( node );
                index next = node;

                if ( l < size && less_( data_[l], data_[node] ) )
                {
                    next = l;
                }
                if ( r < size && less_( data_[r], data_[next] ) )
                {
                    next = r;
                }
                if ( next != node )
                {
                    std::swap( data_[node], data_[next] );
                    node = next;
                }
                else
                {
                    break;
                }
            }
        }
        index left( index node )const _NOEXCEPT{ return node * 2 + 1; }
        index right( index node )const _NOEXCEPT{ return ( node + 1 ) * 2; }
    private:
        std::vector<T> data_;// 第一个数据不要了，方便处理节点关系。
        Cmp less_;
    };


    template<class T>
    max_min_heap<T> BuildHeap( T *arr, size_t size )
    {
        max_min_heap<T> heap;
        for ( size_t i = 0; i < size; i++ )
        {
            heap.push( arr[i] );
        }
        return heap;
    }
}
#endif // !MIN_MAX_HEAP_H