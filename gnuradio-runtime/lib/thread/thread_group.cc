/* -*- c++ -*- */
/*
 * Copyright (C) 2001-2003 William E. Kempf
 * Copyright (C) 2007 Anthony Williams
 * Copyright 2008 Free Software Foundation, Inc.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

/*
 * This was extracted from Boost 1.35.0 and fixed.
 */

#include <gnuradio/thread/thread_group.h>
#include <cassert>
#include <memory>

namespace gr {
namespace thread {

thread_group::thread_group() {}

thread_group::~thread_group() {}

boost::thread* thread_group::create_thread(const std::function<void()>& threadfunc)
{
    // No scoped_lock required here since the only "shared data" that's
    // modified here occurs inside add_thread which does scoped_lock.
    auto thrd = std::make_unique<boost::thread>(threadfunc);
    auto thrdp = thrd.get();
    add_thread(std::move(thrd));
    return thrdp;
}

void thread_group::add_thread(std::unique_ptr<boost::thread> thrd)
{
    std::scoped_lock guard(m_mutex);

    // For now we'll simply ignore requests to add a thread object
    // multiple times. Should we consider this an error and either
    // throw or return an error value?
    auto it = std::find(m_threads.begin(), m_threads.end(), thrd);
    assert(it == m_threads.end());
    if (it == m_threads.end())
        m_threads.push_back(std::move(thrd));
}

void thread_group::remove_thread(boost::thread* thrd)
{
    std::scoped_lock guard(m_mutex);

    // For now we'll simply ignore requests to remove a thread
    // object that's not in the group. Should we consider this an
    // error and either throw or return an error value?
    auto it = std::find_if(
        m_threads.begin(),
        m_threads.end(),
        [&thrd](std::unique_ptr<boost::thread>& it) -> bool { return thrd == it.get(); });
    assert(it != m_threads.end());
    if (it != m_threads.end())
        m_threads.erase(it);
}

void thread_group::join_all()
{
    std::shared_lock<std::shared_mutex> guard(m_mutex);
    for (auto& thrd : m_threads) {
        thrd->join();
    }
}

void thread_group::interrupt_all()
{
    std::shared_lock<std::shared_mutex> guard(m_mutex);
    for (auto& thrd : m_threads) {
        thrd->interrupt();
    }
}

size_t thread_group::size() const
{
    std::shared_lock<std::shared_mutex> guard(m_mutex);
    return m_threads.size();
}

} /* namespace thread */
} /* namespace gr */
