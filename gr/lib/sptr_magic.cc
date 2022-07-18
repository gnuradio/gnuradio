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

#include <gnuradio/hier_block.h>
#include <gnuradio/sptr_magic.h>
#include <map>
#include <stdexcept>

#include <mutex>

namespace gnuradio {

static std::mutex s_mutex;
typedef std::map<gr::node*, gr::node_sptr> sptr_map;
static sptr_map s_map;

struct disarmable_deleter {
    bool armed;

    disarmable_deleter() { armed = true; }

    void operator()(void* p) const
    {
        if (armed)
            delete static_cast<gr::node*>(p);
    }

    void disarm() { armed = false; }
};

void detail::sptr_magic::create_and_stash_initial_sptr(gr::hier_block* p)
{
    gr::node_sptr sptr(p, disarmable_deleter());
    std::scoped_lock guard(s_mutex);
    s_map.insert(sptr_map::value_type(static_cast<gr::node*>(p), sptr));
}

void detail::sptr_magic::cancel_initial_sptr(gr::hier_block* p)
{
    std::scoped_lock guard(s_mutex);
    sptr_map::iterator pos = s_map.find(static_cast<gr::node*>(p));
    if (pos == s_map.end())
        return; /* Not in the map, nothing to do */
    gr::node_sptr sptr = pos->second;
    s_map.erase(pos);
    std::get_deleter<disarmable_deleter, gr::node>(sptr)->disarm();
}

gr::node_sptr detail::sptr_magic::fetch_initial_sptr(gr::node* p)
{
    /*
     * If p isn't a subclass of gr::hier_block, just create the
     * shared ptr and return it.
     */
    gr::hier_block* hb2 = dynamic_cast<gr::hier_block*>(p);
    if (!hb2) {
        return gr::node_sptr(p);
    }

    /*
     * p is a subclass of gr::hier_block, thus we've already created the shared pointer
     * and stashed it away.  Fish it out and return it.
     */
    std::scoped_lock guard(s_mutex);
    sptr_map::iterator pos = s_map.find(static_cast<gr::node*>(p));
    if (pos == s_map.end())
        throw std::invalid_argument("sptr_magic: invalid pointer!");

    gr::node_sptr sptr = pos->second;
    s_map.erase(pos);
    return sptr;
}
} // namespace gnuradio
