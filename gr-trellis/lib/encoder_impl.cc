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

#include "encoder_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
typename encoder<IN_T, OUT_T>::sptr
encoder<IN_T, OUT_T>::make(const fsm& FSM, int ST, int K)
{
    return gnuradio::get_initial_sptr(
        new encoder_impl<IN_T, OUT_T>(FSM, ST, K, K >= 0 ? false : true));
}

template <class IN_T, class OUT_T>
encoder_impl<IN_T, OUT_T>::encoder_impl(const fsm& FSM, int ST, int K, bool B)
    : sync_block("encoder<IN_T,OUT_T>",
                 io_signature::make(1, 1, sizeof(IN_T)),
                 io_signature::make(1, 1, sizeof(OUT_T))),
      d_FSM(FSM),
      d_ST(ST),
      d_K(K),
      d_B(B)
{
}

template <class IN_T, class OUT_T>
void encoder_impl<IN_T, OUT_T>::set_FSM(const fsm& FSM)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_FSM = FSM;
}

template <class IN_T, class OUT_T>
void encoder_impl<IN_T, OUT_T>::set_ST(int ST)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_ST = ST;
}

template <class IN_T, class OUT_T>
void encoder_impl<IN_T, OUT_T>::set_K(int K)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    d_K = K;
}

template <class IN_T, class OUT_T>
encoder_impl<IN_T, OUT_T>::~encoder_impl()
{
}

template <class IN_T, class OUT_T>
int encoder_impl<IN_T, OUT_T>::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(this->d_setlock);
    int ST_tmp = 0;

    if (d_B) { // blockwise operation
        int nblocks = noutput_items / d_K;
        const IN_T* in = (const IN_T*)input_items[0];
        OUT_T* out = (OUT_T*)output_items[0];
        for (int n = 0; n < nblocks; n++) {
            ST_tmp = d_ST;
            for (int i = 0; i < d_K; i++) {
                out[n * d_K + i] =
                    (OUT_T)d_FSM.OS()[ST_tmp * d_FSM.I() + in[n * d_K + i]];
                ST_tmp = (int)d_FSM.NS()[ST_tmp * d_FSM.I() + in[n * d_K + i]];
            }
        }
        return nblocks * d_K;
    }      // end blockwise operation
    else { // streaming operation
        const IN_T* in = (const IN_T*)input_items[0];
        OUT_T* out = (OUT_T*)output_items[0];
        ST_tmp = d_ST;
        for (int i = 0; i < noutput_items; i++) {
            out[i] = (OUT_T)d_FSM.OS()[ST_tmp * d_FSM.I() + in[i]];
            ST_tmp = (int)d_FSM.NS()[ST_tmp * d_FSM.I() + in[i]];
        }
        d_ST = ST_tmp;
        return noutput_items;
    } // end streaming operation
}
template class encoder<std::uint8_t, std::uint8_t>;
template class encoder<std::uint8_t, std::int16_t>;
template class encoder<std::uint8_t, std::int32_t>;
template class encoder<std::int16_t, std::int16_t>;
template class encoder<std::int16_t, std::int32_t>;
template class encoder<std::int32_t, std::int32_t>;


} /* namespace trellis */
} /* namespace gr */
