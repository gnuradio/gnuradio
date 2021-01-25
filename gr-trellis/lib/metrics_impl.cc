/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "metrics_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <stdexcept>

namespace gr {
namespace trellis {

template <class T>
typename metrics<T>::sptr metrics<T>::make(int O,
                                           int D,
                                           const std::vector<T>& TABLE,
                                           digital::trellis_metric_type_t TYPE)
{
    return gnuradio::make_block_sptr<metrics_impl<T>>(O, D, TABLE, TYPE);
}

template <class T>
metrics_impl<T>::metrics_impl(int O,
                              int D,
                              const std::vector<T>& TABLE,
                              digital::trellis_metric_type_t TYPE)
    : block("metrics",
            io_signature::make(1, -1, sizeof(T)),
            io_signature::make(1, -1, sizeof(float))),
      d_O(O),
      d_D(D),
      d_TYPE(TYPE),
      d_TABLE(TABLE)
{
    this->set_relative_rate((uint64_t)d_O, (uint64_t)d_D);
    this->set_output_multiple((int)d_O);
}

template <class T>
void metrics_impl<T>::set_O(int O)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_O = O;
    this->set_relative_rate((uint64_t)d_O, (uint64_t)d_D);
    this->set_output_multiple((int)d_O);
}

template <class T>
void metrics_impl<T>::set_D(int D)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_D = D;
    this->set_relative_rate((uint64_t)d_O, (uint64_t)d_D);
}

template <class T>
void metrics_impl<T>::set_TYPE(digital::trellis_metric_type_t type)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_TYPE = type;
}

template <class T>
void metrics_impl<T>::set_TABLE(const std::vector<T>& table)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_TABLE = table;
}

template <class T>
metrics_impl<T>::~metrics_impl()
{
}


template <class T>
void metrics_impl<T>::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    int input_required = d_D * noutput_items / d_O;
    unsigned ninputs = ninput_items_required.size();
    for (unsigned int i = 0; i < ninputs; i++)
        ninput_items_required[i] = input_required;
}

template <class T>
int metrics_impl<T>::general_work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    int nstreams = input_items.size();

    for (int m = 0; m < nstreams; m++) {
        const T* in = (T*)input_items[m];
        float* out = (float*)output_items[m];

        for (int i = 0; i < noutput_items / d_O; i++) {
            calc_metric(d_O, d_D, d_TABLE, &(in[i * d_D]), &(out[i * d_O]), d_TYPE);
        }
    }

    this->consume_each(d_D * noutput_items / d_O);
    return noutput_items;
}

template class metrics<std::int16_t>;
template class metrics<std::int32_t>;
template class metrics<float>;
template class metrics<gr_complex>;
} /* namespace trellis */
} /* namespace gr */
