/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef METRICS_IMPL_H
#define METRICS_IMPL_H

#include <gnuradio/trellis/metrics.h>

namespace gr {
namespace trellis {

template <class T>
class metrics_impl : public metrics<T>
{
private:
    int d_O;
    int d_D;
    digital::trellis_metric_type_t d_TYPE;
    std::vector<T> d_TABLE;

public:
    metrics_impl(int O,
                 int D,
                 const std::vector<T>& TABLE,
                 digital::trellis_metric_type_t TYPE);
    ~metrics_impl();

    int O() const { return d_O; }
    int D() const { return d_D; }
    digital::trellis_metric_type_t TYPE() const { return d_TYPE; }
    std::vector<T> TABLE() const { return d_TABLE; }

    void set_O(int O);
    void set_D(int D);
    void set_TYPE(digital::trellis_metric_type_t type);
    void set_TABLE(const std::vector<T>& table);

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* METRICS_IMPL_H */
