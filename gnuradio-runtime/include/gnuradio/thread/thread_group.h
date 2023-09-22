/* -*- c++ -*- */
/*
 * Copyright (C) 2001-2003 William E. Kempf
 * Copyright (C) 2007 Anthony Williams
 * Copyright 2008,2009 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

/*
 * This was extracted from Boost 1.35.0 and fixed.
 */

#ifndef INCLUDED_THREAD_GROUP_H
#define INCLUDED_THREAD_GROUP_H

#include <gnuradio/api.h>
#include <gnuradio/thread/thread.h>

#include <shared_mutex>
#include <unordered_set>
#include <functional>
#include <memory>

namespace gr {
namespace thread {

class GR_RUNTIME_API thread_group
{
public:
    thread_group() = default;
    ~thread_group() = default;
    thread_group(const thread_group&) = delete;
    thread_group& operator=(const thread_group&) = delete;

    boost::thread* create_thread(const std::function<void()>& threadfunc);
    void add_thread(std::unique_ptr<boost::thread> thrd) noexcept;
    void remove_thread(boost::thread* thrd) noexcept;
    void join_all();
    void interrupt_all();
    size_t size() const noexcept;

private:
    std::unordered_set<std::unique_ptr<boost::thread>> m_threads;
    mutable std::shared_mutex m_mutex;
};

} /* namespace thread */
} /* namespace gr */

#endif /* INCLUDED_THREAD_GROUP_H */
