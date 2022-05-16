/* -*- c++ -*- */
/*
 * Copyright 2007,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_PMT_POOL_H
#define INCLUDED_PMT_POOL_H

#include <condition_variable>
#include <pmt/api.h>
#include <cstddef>
#include <mutex>
#include <vector>

namespace pmt {

/*!
 * \brief very simple thread-safe fixed-size allocation pool
 *
 * FIXME may want to go to global allocation with per-thread free list.
 * This would eliminate virtually all lock contention.
 */
class PMT_API pmt_pool
{

    struct PMT_API item {
        struct item* d_next;
    };

    using scoped_lock = std::unique_lock<std::mutex>;
    mutable std::mutex d_mutex;
    std::condition_variable d_cond;

    size_t d_itemsize;
    size_t d_alignment;
    size_t d_allocation_size;
    size_t d_max_items;
    size_t d_n_items;
    item* d_freelist;
    std::vector<char*> d_allocations;

public:
    /*!
     * \param itemsize size in bytes of the items to be allocated.
     * \param alignment alignment in bytes of all objects to be allocated (must be
     *power-of-2). \param allocation_size number of bytes to allocate at a time from the
     *underlying allocator. \param max_items is the maximum number of items to allocate.
     *If this number is exceeded, the allocate blocks.  0 implies no limit.
     */
    pmt_pool(size_t itemsize,
             size_t alignment = 16,
             size_t allocation_size = 4096,
             size_t max_items = 0);
    ~pmt_pool();

    void* malloc();
    void free(void* p);
};

} /* namespace pmt */

#endif /* INCLUDED_PMT_POOL_H */
