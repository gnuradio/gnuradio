#pragma once

#if 0
#include <moodycamel/blockingconcurrentqueue.h>

namespace gr {

/**
 * @brief Blocking Multi-producer Single-consumer Queue class
 *
 * @tparam T Data type of items in queue
 */
template <typename T>
class concurrent_queue
{
public:
    bool push(const T& msg)
    {
        q.enqueue(msg);
        return true;
    }

    // Non-blocking
    bool try_pop(T& msg) { return q.try_dequeue(msg); }
    bool pop(T& msg)
    {
        q.wait_dequeue(msg);
        return true;
    }
    void clear()
    {
        T msg;
        bool done = false;
        while (!done)
            done = !q.try_dequeue(msg);
    }
    size_t size_approx() { return q.size_approx(); }

private:
    moodycamel::BlockingConcurrentQueue<T> q;
};
} // namespace gr
#else

#include <condition_variable>
#include <chrono>
#include <deque>
#include <iostream>
#include <mutex>
using namespace std::chrono_literals;
namespace gr {

/**
 * @brief Blocking Multi-producer Single-consumer Queue class
 *
 * @tparam T Data type of items in queue
 */
template <typename T>
class concurrent_queue
{
public:
    bool push(const T& msg)
    {
        std::unique_lock<std::mutex> l(_mutex);
        // std::scoped_lock l(_mutex);
        _queue.push_back(msg);
        l.unlock();
        _cond.notify_one();

        return true;
    }

    // Non-blocking
    bool try_pop(T& msg)
    {
        std::unique_lock<std::mutex> l(_mutex);
        if (!_queue.empty()) {
            msg = _queue.front();
            _queue.pop_front();
            return true;
        }
        else {
            return false;
        }
    }
    bool pop(T& msg)
    {
#if 1
        std::unique_lock<std::mutex> l(_mutex);
        _cond.wait(l,
                   [this] { return !_queue.empty(); }); // TODO - replace with a waitfor
        msg = _queue.front();
        _queue.pop_front();
        return true;
#else
        std::unique_lock<std::mutex> l(_mutex);
        if (_cond.wait_for(l, 10us, [this] { return !_queue.empty(); })) {
            msg = _queue.front();
            _queue.pop_front();
            return true;
        }
        else { // timeout
            return false;
        }
#endif
    }
    void clear()
    {
        std::unique_lock<std::mutex> l(_mutex);
        _queue.clear();
    }

private:
    std::deque<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
};
} // namespace gr
#endif
