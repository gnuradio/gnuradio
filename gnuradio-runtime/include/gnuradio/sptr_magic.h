/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_SPTR_MAGIC_H
#define INCLUDED_GR_RUNTIME_SPTR_MAGIC_H

#include <gnuradio/api.h>
#include <boost/shared_ptr.hpp>

namespace gr {
class basic_block;
class hier_block2;
} // namespace gr

namespace gnuradio {
namespace detail {

class GR_RUNTIME_API sptr_magic
{
public:
    static boost::shared_ptr<gr::basic_block> fetch_initial_sptr(gr::basic_block* p);
    static void create_and_stash_initial_sptr(gr::hier_block2* p);
    static void cancel_initial_sptr(gr::hier_block2* p);
};
} // namespace detail

/*
 * \brief New!  Improved!  Standard method to get/create the
 * boost::shared_ptr for a block.
 */
template <class T>
boost::shared_ptr<T> get_initial_sptr(T* p)
{
    return boost::dynamic_pointer_cast<T, gr::basic_block>(
        detail::sptr_magic::fetch_initial_sptr(p));
}
} // namespace gnuradio

#endif /* INCLUDED_GR_RUNTIME_SPTR_MAGIC_H */
