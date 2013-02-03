#ifndef BlockQueue_H
#define BlockQueue_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <thread>

using namespace std;

template <typename T> class BlockQueue
{
    unsigned max_size = 0;
    const int prod_wait_time = 500;
    const int cons_wait_time = 500;

    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_is_empty;
    std::condition_variable cond_is_full;

    typedef std::unique_lock<std::mutex> unilock;

public:
    BlockQueue<T>(){}

    BlockQueue<T>(int capacity) {
        max_size = capacity;
    }

    virtual ~BlockQueue<T>() {

    }

    const int size() {
        return queue_.size();
    }

    const bool try_push(T const &val)
    {
        unilock l(mutex_);

        cout<<"try_push: queue size "<<queue_.size()<<", max_size "<<max_size<< endl;

        if(cond_is_full.wait_for(l, chrono::milliseconds(prod_wait_time),
                [this] { return queue_.size() != max_size; })) {

            bool wake = queue_.empty(); // we may need to wake consumer
            queue_.push(val);
            if (wake) cond_is_empty.notify_one();

cout<<"pushed "<<val<<" queue_ size " << queue_.size() << endl;;

            return true;
        }

        return false;
    }


    void push(T const &val) {
        while(!try_push(val))
            this_thread::sleep_for(chrono::milliseconds(prod_wait_time));
    }


    const bool _pop(T& result)
    {
        unilock l(mutex_);

        cout<<"_pop: queue size "<<queue_.size()<<", max_size "<<max_size<< endl;

        if(cond_is_empty.wait_for(l, chrono::milliseconds(cons_wait_time),
                [this] { return queue_.size() > 0; })) {

cout<<"ready to pop"<<endl;

            result = queue_.front();
            queue_.pop();

            if(queue_.size() == max_size - 1)
                cond_is_full.notify_one();

            return true;
        }

        return false;
    }


    const bool try_pop(T& result) {
        return _pop(result);
    }


    T pop() {
        T ret;

        while(_pop(ret) == false) {
            this_thread::sleep_for(chrono::milliseconds(cons_wait_time));
        }

        return ret;
    }

};


#endif

