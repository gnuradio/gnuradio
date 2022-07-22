/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * The magic here is a way of constructing std::shared_ptr<> to blocks, in a way
 * that allows the constructors of those blocks to reference them.
 * This is only used for blocks that derive from hier_block, so that they can
 * create and connect sub-blocks in their constructor.
 */

#pragma once

#include <gnuradio/api.h>
#include <memory>

namespace gr {
class node;
class hier_block;
} // namespace gr

namespace gnuradio {
namespace detail {

class GR_RUNTIME_API sptr_magic
{
public:
    static std::shared_ptr<gr::node> fetch_initial_sptr(gr::node* p);
    static void create_and_stash_initial_sptr(gr::hier_block* p);
    static void cancel_initial_sptr(gr::hier_block* p);
};
} // namespace detail

/*
 * \brief New!  Improved!  Standard method to get/create the
 * std::shared_ptr for a block.
 */
template <class T>
std::shared_ptr<T> get_initial_sptr(T* p)
{
    return std::dynamic_pointer_cast<T, gr::node>(
        detail::sptr_magic::fetch_initial_sptr(p));
}

/*
 * \brief GNU Radio version of std::make_shared<> that also provides magic. For
 * blocks that don't reference self() in any constructor it's equivalent to
 * std::make_shared<>.
 */
template <typename T, typename... Args>
std::shared_ptr<T> make_block_sptr(Args&&... args)
{
    return get_initial_sptr(new T(std::forward<Args>(args)...));
}
} // namespace gnuradio
