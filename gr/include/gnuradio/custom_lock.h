/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/api.h>
#include <gnuradio/logger.h>

#include <mutex>

namespace gr {

/*!
 * Custom lock interface. Objects should implement this interface in order to
 * use the custom_lock object below. The interface defines two functions that,
 * as their names suggest, are called when the lock is locked and unlocked
 * respectively.
 */
class custom_lock_if
{
public:
    virtual ~custom_lock_if(){};

    /*!
     * This function will be executed on construction of the custom lock.
     */
    virtual void on_lock(std::unique_lock<std::mutex>& lock) = 0;

    /*!
     * This function will be executed on destruction of the custom lock.
     */
    virtual void on_unlock() = 0;
};

/*!
 * Class that defines a lock using a mutex and a "locker" that implements the
 * custom_lock_if interface. The interface defines an on_lock() function that
 * is executed when the lock is locked and an on_unlock() function that the
 * is called when the lock is unlocked. Calls to these two functions are
 * delegated to the locker object.
 */
class custom_lock
{
public:
    explicit custom_lock(std::mutex& mutex, custom_lock_if* locker)
        : d_lock(mutex), d_locker(locker)
    {
        d_locker->on_lock(d_lock);
    }

    ~custom_lock() { d_locker->on_unlock(); }

    // Disallow copying and assignment
    custom_lock(custom_lock const&) = delete;
    custom_lock& operator=(custom_lock const&) = delete;

private:
    std::unique_lock<std::mutex> d_lock;
    std::shared_ptr<custom_lock_if> d_locker;
};

} /* namespace gr */
