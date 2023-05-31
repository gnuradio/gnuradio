/* -*- c++ -*- */
/*
 * Copyright (C) 2001-2003 William E. Kempf
 * Copyright (C) 2007 Anthony Williams
 * Copyright 2008 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

/*
 * This was extracted from Boost 1.35.0 and fixed.
 */

#include <gnuradio/thread/thread_group.h>
#include <cassert>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>

namespace gr {
namespace thread {

boost::thread* thread_group::create_thread(const std::function<void()>& threadfunc)
{
    auto thrd = std::make_unique<boost::thread>(threadfunc);
    if (!thrd) {
        throw std::runtime_error("could not create thread");
    }
    auto thrdp = thrd.get();

    std::scoped_lock guard(m_mutex);
    m_threads.insert(std::move(thrd));
    return thrdp;
}

void thread_group::add_thread(std::unique_ptr<boost::thread> thrd) noexcept
{
    std::scoped_lock guard(m_mutex);

    // we currently don't care whether a thread was already present; std::unordered_set
    // guarantees we only have one copy in m_threads
    m_threads.insert(std::move(thrd));
}

void thread_group::remove_thread(boost::thread* thrd) noexcept
{
    std::scoped_lock guard(m_mutex);

    // For now we'll simply ignore requests to remove a thread
    // object that's not in the group. Should we consider this an
    // error and either throw or return an error value?
    m_threads.erase(std::unique_ptr<boost::thread>(thrd));
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

size_t thread_group::size() const noexcept
{
    std::shared_lock<std::shared_mutex> guard(m_mutex);
    return m_threads.size();
}

} /* namespace thread */
} /* namespace gr */
