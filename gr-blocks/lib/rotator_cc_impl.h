/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_ROTATOR_CC_IMPL_H
#define INCLUDED_BLOCKS_ROTATOR_CC_IMPL_H

#include <gnuradio/blocks/rotator.h>
#include <gnuradio/blocks/rotator_cc.h>
#include <queue>

namespace gr {
namespace blocks {

struct phase_inc_update_t {
    uint64_t offset;
    double phase_inc;
};

bool cmp_phase_inc_update_offset(phase_inc_update_t lhs, phase_inc_update_t rhs)
{
    return lhs.offset > rhs.offset;
};

typedef std::priority_queue<phase_inc_update_t,
                            std::vector<phase_inc_update_t>,
                            decltype(&cmp_phase_inc_update_offset)>
    phase_inc_queue_t;

/*!
 * \brief Complex rotator
 * \ingroup math_blk
 */
class rotator_cc_impl : public rotator_cc
{
private:
    rotator d_r;
    bool d_tag_inc_updates;
    phase_inc_queue_t d_inc_update_queue;
    gr::thread::mutex d_mutex;

    void handle_cmd_msg(pmt::pmt_t msg);

public:
    rotator_cc_impl(double phase_inc = 0.0, bool tag_inc_updates = false);
    ~rotator_cc_impl() override;

    void set_phase_inc(double phase_inc) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_ROTATOR_CC_IMPL_H */
