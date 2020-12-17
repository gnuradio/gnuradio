/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_RMS_FF_IMPL_H
#define INCLUDED_BLOCKS_RMS_FF_IMPL_H

#include <gnuradio/blocks/rms_ff.h>

namespace gr {
namespace blocks {

/*!
 * \brief RMS average power
 * \ingroup math_blk
 */
class rms_ff_impl : public rms_ff
{
private:
    double d_alpha, d_beta, d_avg;

public:
    rms_ff_impl(double alpha = 0.0001);
    ~rms_ff_impl() override;

    void set_alpha(double alpha) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_RMS_FF_IMPL_H */
