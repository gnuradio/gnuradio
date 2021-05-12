/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_CUSTOM_LOCK_H
#define INCLUDED_GR_CUSTOM_LOCK_H

#include <gnuradio/api.h>
#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>

namespace gr {

/*!
 * Custom lock interface. Objects should implement this interface in order to
 * use the custom_lock object below.
 */
class custom_lock_if
{
public:
    /*!
     * This function will be executed on construction of the custom lock.
     */
    virtual void on_lock(gr::thread::scoped_lock& lock) = 0;

    /*!
     * This function will be executed on destruction of the custom lock.
     */
    virtual void on_unlock() = 0;
};

/*!
 * Write me!
 */
class custom_lock
{
public:
    explicit custom_lock(gr::thread::mutex& mutex, std::shared_ptr<custom_lock_if> locker)
        : d_mutex(mutex), d_lock(mutex), d_locker(locker)
    {
        d_locker->on_lock(d_lock);
    }

    ~custom_lock() { d_locker->on_unlock(); }

    // Disallow copying and assignment
    custom_lock(custom_lock const&) = delete;
    custom_lock& operator=(custom_lock const&) = delete;

private:
    gr::thread::mutex& d_mutex;
    gr::thread::scoped_lock d_lock;
    std::shared_ptr<custom_lock_if> d_locker;
};

} /* namespace gr */

#endif /* INCLUDED_GR_CUSTOM_LOCK_H */