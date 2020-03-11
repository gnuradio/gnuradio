/* -*- c++ -*- */
/*
 * Copyright 2004,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef MUTE_IMPL_H
#define MUTE_IMPL_H

#include <gnuradio/blocks/mute.h>

namespace gr {
namespace blocks {

template <class T>
class mute_impl : public mute_blk<T>
{
private:
    bool d_mute;

public:
    mute_impl(bool mute);
    ~mute_impl();

    bool mute() const { return d_mute; }
    void set_mute(bool mute) { d_mute = mute; }
    void set_mute_pmt(pmt::pmt_t msg) { set_mute(pmt::to_bool(msg)); }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* MUTE_IMPL_H */
