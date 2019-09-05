/*
 * Copyright 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_RUNTIME_RUNTIME_LOG_THREADSAFE_QUEUE_H
#define INCLUDED_GR_RUNTIME_RUNTIME_LOG_THREADSAFE_QUEUE_H
#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <utility>
namespace gr {
namespace log {
using lock_t = std::unique_lock<std::mutex>;
template <typename T>
class threadsafe_queue
{
private:
    std::queue<T> the_queue;
    mutable std::mutex the_mutex;
    std::condition_variable the_condition_variable;
    bool empty() const
    {
        lock_t lock(the_mutex);
        return the_queue.empty();
    }

public:
    void push(T&& data) noexcept
    {
        lock_t lock(the_mutex);
        the_queue.push(std::move(data));
        lock.unlock();
        the_condition_variable.notify_one();
    }

    bool empty() noexcept
    {

        lock_t lock(the_mutex);
        return the_queue.empty();
    }
    /*! Blocking popping the front element from the queue
     */
    T& pop() noexcept
    {
        lock_t lock(the_mutex);
        while (the_queue.empty()) {
            the_condition_variable.wait(lock);
        }
        auto& temp = the_queue.front();
        the_queue.pop();
        return temp;
    }
    T& pop_or_throw()
    {
        lock_t lock(the_mutex);
        if (the_queue.empty()) {
            throw std::out_of_range("queue empty");
        }
        auto& temp = the_queue.front();
        the_queue.pop();
        return temp;
    }
};
} // namespace log
} // namespace gr
#endif /* INCLUDED_GR_RUNTIME_RUNTIME_LOG_THREADSAFE_QUEUE_H */
