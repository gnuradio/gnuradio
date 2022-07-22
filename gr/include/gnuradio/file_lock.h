// Replace boost::interprocess::file_lock
// Portions copied from boost under the following copyright
//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////


/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 * This file is part of GNU Radio
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once


// TODO: Add Windows compatible wrapping (rip from boost::file_lock)
#include <fcntl.h>


namespace gr {

/**
 * @brief File lock class
 *
 * Stripped down version of boost::interprocess::file_lock
 *
 */
class file_lock
{
    using file_handle_t = int;

public:
    file_lock() noexcept : m_file_hnd(file_handle_t(-1)) {}
    file_lock(const std::string& filename)
    {
        m_file_hnd = ::open(filename.c_str(), O_RDWR);

        if (m_file_hnd == -1) {
            throw std::runtime_error("Unable to open requested file lock");
        }
    }

    void lock()
    {
        if (!acquire_file_lock(m_file_hnd)) {
            throw std::runtime_error("Unable to acquire file lock");
        }
    }
    void unlock()
    {
        if (!release_file_lock(m_file_hnd)) {
            throw std::runtime_error("Unable to release file lock");
        }
    }


    bool acquire_file_lock(file_handle_t hnd)
    {
        struct ::flock lock;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        return -1 != ::fcntl(hnd, F_SETLKW, &lock);
    }

    bool release_file_lock(file_handle_t hnd)
    {
        struct ::flock lock;
        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        return -1 != ::fcntl(hnd, F_SETLK, &lock);
    }

private:
    file_handle_t m_file_hnd;
};

} // namespace gr