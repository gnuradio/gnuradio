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

#include "viterbi_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
namespace trellis {

template <class T>
typename viterbi<T>::sptr viterbi<T>::make(const fsm& FSM, int K, int S0, int SK)
{
    return gnuradio::get_initial_sptr(new viterbi_impl<T>(FSM, K, S0, SK));
}

template <class T>
viterbi_impl<T>::viterbi_impl(const fsm& FSM, int K, int S0, int SK)
    : block("viterbi",
            io_signature::make(1, -1, sizeof(float)),
            io_signature::make(1, -1, sizeof(T))),
      d_FSM(FSM),
      d_K(K),
      d_S0(S0),
      d_SK(SK) //,
               // d_trace(FSM.S()*K)
{
    this->set_relative_rate(1, (uint64_t)d_FSM.O());
    this->set_output_multiple(d_K);
}

template <class T>
void viterbi_impl<T>::set_FSM(const fsm& FSM)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_FSM = FSM;
    this->set_relative_rate(1, (uint64_t)d_FSM.O());
}

template <class T>
void viterbi_impl<T>::set_K(int K)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_K = K;
    this->set_output_multiple(d_K);
}

template <class T>
void viterbi_impl<T>::set_S0(int S0)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_S0 = S0;
}

template <class T>
void viterbi_impl<T>::set_SK(int SK)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_SK = SK;
}

template <class T>
viterbi_impl<T>::~viterbi_impl()
{
}

template <class T>
void viterbi_impl<T>::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    int input_required = d_FSM.O() * noutput_items;
    unsigned ninputs = ninput_items_required.size();
    for (unsigned int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = input_required;
    }
}

template <class T>
int viterbi_impl<T>::general_work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    int nstreams = input_items.size();
    int nblocks = noutput_items / d_K;

    for (int m = 0; m < nstreams; m++) {
        const float* in = (const float*)input_items[m];
        T* out = (T*)output_items[m];

        for (int n = 0; n < nblocks; n++) {
            viterbi_algorithm(d_FSM.I(),
                              d_FSM.S(),
                              d_FSM.O(),
                              d_FSM.NS(),
                              d_FSM.OS(),
                              d_FSM.PS(),
                              d_FSM.PI(),
                              d_K,
                              d_S0,
                              d_SK,
                              &(in[n * d_K * d_FSM.O()]),
                              &(out[n * d_K]));
        }
    }

    this->consume_each(d_FSM.O() * noutput_items);
    return noutput_items;
}

template class viterbi<std::uint8_t>;
template class viterbi<std::int16_t>;
template class viterbi<std::int32_t>;
} /* namespace trellis */
} /* namespace gr */
