/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "viterbi_combined_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
typename viterbi_combined<IN_T, OUT_T>::sptr
viterbi_combined<IN_T, OUT_T>::make(const fsm& FSM,
                                    int K,
                                    int S0,
                                    int SK,
                                    int D,
                                    const std::vector<IN_T>& TABLE,
                                    digital::trellis_metric_type_t TYPE)
{
    return gnuradio::get_initial_sptr(
        new viterbi_combined_impl<IN_T, OUT_T>(FSM, K, S0, SK, D, TABLE, TYPE));
}

template <class IN_T, class OUT_T>
viterbi_combined_impl<IN_T, OUT_T>::viterbi_combined_impl(
    const fsm& FSM,
    int K,
    int S0,
    int SK,
    int D,
    const std::vector<IN_T>& TABLE,
    digital::trellis_metric_type_t TYPE)
    : block("viterbi_combined<IN_T,OUT_T>",
            io_signature::make(1, -1, sizeof(IN_T)),
            io_signature::make(1, -1, sizeof(OUT_T))),
      d_FSM(FSM),
      d_K(K),
      d_S0(S0),
      d_SK(SK),
      d_D(D),
      d_TABLE(TABLE),
      d_TYPE(TYPE) //,
                   // d_trace(FSM.S()*K)
{
    this->set_relative_rate(1, (uint64_t)d_D);
    this->set_output_multiple(d_K);
}

template <class IN_T, class OUT_T>
void viterbi_combined_impl<IN_T, OUT_T>::set_K(int K)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_K = K;
    this->set_output_multiple(d_K);
}

template <class IN_T, class OUT_T>
void viterbi_combined_impl<IN_T, OUT_T>::set_D(int D)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_D = D;
    this->set_relative_rate(1, (uint64_t)d_D);
}

template <class IN_T, class OUT_T>
void viterbi_combined_impl<IN_T, OUT_T>::set_FSM(const fsm& FSM)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_FSM = FSM;
}

template <class IN_T, class OUT_T>
void viterbi_combined_impl<IN_T, OUT_T>::set_S0(int S0)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_S0 = S0;
}

template <class IN_T, class OUT_T>
void viterbi_combined_impl<IN_T, OUT_T>::set_SK(int SK)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_SK = SK;
}

template <class IN_T, class OUT_T>
void viterbi_combined_impl<IN_T, OUT_T>::set_TYPE(digital::trellis_metric_type_t type)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_TYPE = type;
}

template <class IN_T, class OUT_T>
void viterbi_combined_impl<IN_T, OUT_T>::set_TABLE(const std::vector<IN_T>& table)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_TABLE = table;
}

template <class IN_T, class OUT_T>
viterbi_combined_impl<IN_T, OUT_T>::~viterbi_combined_impl()
{
}

template <class IN_T, class OUT_T>
void viterbi_combined_impl<IN_T, OUT_T>::forecast(int noutput_items,
                                                  gr_vector_int& ninput_items_required)
{
    int input_required = d_D * noutput_items;
    unsigned ninputs = ninput_items_required.size();
    for (unsigned int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = input_required;
    }
}

template <class IN_T, class OUT_T>
int viterbi_combined_impl<IN_T, OUT_T>::general_work(
    int noutput_items,
    gr_vector_int& ninput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    int nstreams = input_items.size();
    int nblocks = noutput_items / d_K;

    for (int m = 0; m < nstreams; m++) {
        const IN_T* in = (const IN_T*)input_items[m];
        OUT_T* out = (OUT_T*)output_items[m];

        for (int n = 0; n < nblocks; n++) {
            viterbi_algorithm_combined(d_FSM.I(),
                                       d_FSM.S(),
                                       d_FSM.O(),
                                       d_FSM.NS(),
                                       d_FSM.OS(),
                                       d_FSM.PS(),
                                       d_FSM.PI(),
                                       d_K,
                                       d_S0,
                                       d_SK,
                                       d_D,
                                       d_TABLE,
                                       d_TYPE,
                                       &(in[n * d_K * d_D]),
                                       &(out[n * d_K]));
        }
    }

    this->consume_each(d_D * noutput_items);
    return noutput_items;
}

template class viterbi_combined<std::int16_t, std::uint8_t>;
template class viterbi_combined<std::int16_t, std::int16_t>;
template class viterbi_combined<std::int16_t, std::int32_t>;
template class viterbi_combined<std::int32_t, std::uint8_t>;
template class viterbi_combined<std::int32_t, std::int16_t>;
template class viterbi_combined<std::int32_t, std::int32_t>;
template class viterbi_combined<float, std::uint8_t>;
template class viterbi_combined<float, std::int16_t>;
template class viterbi_combined<float, std::int32_t>;
template class viterbi_combined<gr_complex, std::uint8_t>;
template class viterbi_combined<gr_complex, std::int16_t>;
template class viterbi_combined<gr_complex, std::int32_t>;


} /* namespace trellis */
} /* namespace gr */
