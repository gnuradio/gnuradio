/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PROBE_DENSITY_B_IMPL_H
#define INCLUDED_GR_PROBE_DENSITY_B_IMPL_H

#include <gnuradio/digital/probe_density_b.h>

namespace gr {
namespace digital {

class probe_density_b_impl : public probe_density_b
{
private:
    double d_alpha;
    double d_beta;
    double d_density;

public:
    probe_density_b_impl(double alpha);
    ~probe_density_b_impl();

    /*!
     * \brief Returns the current density value
     */
    double density() const { return d_density; }

    /*!
     * \brief Set the average filter constant
     */
    void set_alpha(double alpha);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_PROBE_DENSITY_B_IMPL_H */
