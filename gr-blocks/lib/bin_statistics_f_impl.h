/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_BIN_STATISTICS_F_IMPL_H
#define INCLUDED_GR_BIN_STATISTICS_F_IMPL_H

#include <gnuradio/blocks/bin_statistics_f.h>
#include <gnuradio/feval.h>
#include <gnuradio/message.h>
#include <gnuradio/msg_queue.h>

namespace gr {
namespace blocks {

class bin_statistics_f_impl : public bin_statistics_f
{
private:
    enum state_t { ST_INIT, ST_TUNE_DELAY, ST_DWELL_DELAY };

    const size_t d_vlen;
    msg_queue::sptr d_msgq;
    feval_dd* d_tune;
    size_t d_tune_delay;
    size_t d_dwell_delay;
    double d_center_freq;

    state_t d_state;
    size_t d_delay; // nsamples remaining to state transition

    void enter_init();
    void enter_tune_delay();
    void enter_dwell_delay();
    void leave_dwell_delay();

protected:
    std::vector<float> d_max; // per bin maxima

    size_t vlen() const { return d_vlen; }
    double center_freq() const { return d_center_freq; }
    msg_queue::sptr msgq() const { return d_msgq; }

    virtual void reset_stats();
    virtual void accrue_stats(const float* input);
    virtual void send_stats();

public:
    bin_statistics_f_impl(unsigned int vlen,
                          msg_queue::sptr msgq,
                          feval_dd* tune,
                          size_t tune_delay,
                          size_t dwell_delay);
    ~bin_statistics_f_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_BIN_STATISTICS_F_IMPL_H */
