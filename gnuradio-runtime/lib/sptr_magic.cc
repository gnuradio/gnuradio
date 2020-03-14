/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/hier_block2.h>
#include <gnuradio/sptr_magic.h>
#include <map>
#include <stdexcept>

#include <gnuradio/thread/thread.h>

namespace gnuradio {

static gr::thread::mutex s_mutex;
typedef std::map<gr::basic_block*, gr::basic_block_sptr> sptr_map;
static sptr_map s_map;

struct disarmable_deleter {
    bool armed;

    disarmable_deleter() { armed = true; }

    void operator()(void* p) const
    {
        if (armed)
            delete static_cast<gr::basic_block*>(p);
    }

    void disarm() { armed = false; }
};

void detail::sptr_magic::create_and_stash_initial_sptr(gr::hier_block2* p)
{
    gr::basic_block_sptr sptr(p, disarmable_deleter());
    gr::thread::scoped_lock guard(s_mutex);
    s_map.insert(sptr_map::value_type(static_cast<gr::basic_block*>(p), sptr));
}

void detail::sptr_magic::cancel_initial_sptr(gr::hier_block2* p)
{
    gr::thread::scoped_lock guard(s_mutex);
    sptr_map::iterator pos = s_map.find(static_cast<gr::basic_block*>(p));
    if (pos == s_map.end())
        return; /* Not in the map, nothing to do */
    gr::basic_block_sptr sptr = pos->second;
    s_map.erase(pos);
    std::get_deleter<disarmable_deleter, gr::basic_block>(sptr)->disarm();
}

gr::basic_block_sptr detail::sptr_magic::fetch_initial_sptr(gr::basic_block* p)
{
    /*
     * If p isn't a subclass of gr::hier_block2, just create the
     * shared ptr and return it.
     */
    gr::hier_block2* hb2 = dynamic_cast<gr::hier_block2*>(p);
    if (!hb2) {
        return gr::basic_block_sptr(p);
    }

    /*
     * p is a subclass of gr::hier_block2, thus we've already created the shared pointer
     * and stashed it away.  Fish it out and return it.
     */
    gr::thread::scoped_lock guard(s_mutex);
    sptr_map::iterator pos = s_map.find(static_cast<gr::basic_block*>(p));
    if (pos == s_map.end())
        throw std::invalid_argument("sptr_magic: invalid pointer!");

    gr::basic_block_sptr sptr = pos->second;
    s_map.erase(pos);
    return sptr;
}
} // namespace gnuradio
