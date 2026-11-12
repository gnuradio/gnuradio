/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_AMP_BAL_IMPL_H
#define INCLUDED_CHANNELS_AMP_BAL_IMPL_H

#include <gnuradio/channels/amp_bal.h>

namespace gr {
namespace channels {

class amp_bal_impl : public amp_bal
{
private:
    double d_alpha;
    double d_avg_i; // mean of I^2
    double d_avg_q; // mean of Q^2

public:
    amp_bal_impl(double alpha);

    double alpha() const override;
    void set_alpha(double alpha) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_AMP_BAL_IMPL_H */
