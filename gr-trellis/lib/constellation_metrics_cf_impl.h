/* -*- c++ -*- */
/*
 * Copyright 2004,2010-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_IMPL_H
#define INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_IMPL_H

#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/constellation_metrics_cf.h>

namespace gr {
namespace trellis {

class constellation_metrics_cf_impl : public constellation_metrics_cf
{
private:
    digital::constellation_sptr d_constellation;
    digital::trellis_metric_type_t d_TYPE;
    unsigned int d_O;
    unsigned int d_D;

public:
    constellation_metrics_cf_impl(digital::constellation_sptr constellation,
                                  digital::trellis_metric_type_t TYPE);
    ~constellation_metrics_cf_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_IMPL_H */
