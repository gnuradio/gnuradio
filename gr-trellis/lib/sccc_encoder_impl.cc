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

#include "sccc_encoder_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
typename sccc_encoder<IN_T, OUT_T>::sptr
sccc_encoder<IN_T, OUT_T>::make(const fsm& FSMo,
                                int STo,
                                const fsm& FSMi,
                                int STi,
                                const interleaver& INTERLEAVER,
                                int blocklength)
{
    return gnuradio::get_initial_sptr(new sccc_encoder_impl<IN_T, OUT_T>(
        FSMo, STo, FSMi, STi, INTERLEAVER, blocklength));
}

template <class IN_T, class OUT_T>
sccc_encoder_impl<IN_T, OUT_T>::sccc_encoder_impl(const fsm& FSMo,
                                                  int STo,
                                                  const fsm& FSMi,
                                                  int STi,
                                                  const interleaver& INTERLEAVER,
                                                  int blocklength)
    : sync_block("sccc_encoder<IN_T,OUT_T>",
                 io_signature::make(1, 1, sizeof(IN_T)),
                 io_signature::make(1, 1, sizeof(OUT_T))),
      d_FSMo(FSMo),
      d_STo(STo),
      d_FSMi(FSMi),
      d_STi(STi),
      d_INTERLEAVER(INTERLEAVER),
      d_blocklength(blocklength)
{
    this->set_output_multiple(d_blocklength);
    d_buffer.resize(d_blocklength);
}

template <class IN_T, class OUT_T>
sccc_encoder_impl<IN_T, OUT_T>::~sccc_encoder_impl()
{
}

template <class IN_T, class OUT_T>
int sccc_encoder_impl<IN_T, OUT_T>::work(int noutput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    for (int b = 0; b < noutput_items / d_blocklength; b++) {
        const IN_T* in = (const IN_T*)input_items[0] + b * d_blocklength;
        OUT_T* out = (OUT_T*)output_items[0] + b * d_blocklength;

        int STo_tmp = d_STo;
        for (int i = 0; i < d_blocklength; i++) {
            d_buffer[i] = d_FSMo.OS()[STo_tmp * d_FSMo.I() + in[i]];
            STo_tmp = (int)d_FSMo.NS()[STo_tmp * d_FSMo.I() + in[i]];
        }

        int STi_tmp = d_STi;
        for (int i = 0; i < d_blocklength; i++) {
            int k = d_INTERLEAVER.INTER()[i];
            out[i] = (OUT_T)d_FSMi.OS()[STi_tmp * d_FSMi.I() + d_buffer[k]];
            STi_tmp = (int)d_FSMi.NS()[STi_tmp * d_FSMi.I() + d_buffer[k]];
        }
    }
    return noutput_items;
}
template class sccc_encoder<std::uint8_t, std::uint8_t>;
template class sccc_encoder<std::uint8_t, std::int16_t>;
template class sccc_encoder<std::uint8_t, std::int32_t>;
template class sccc_encoder<std::int16_t, std::int16_t>;
template class sccc_encoder<std::int16_t, std::int32_t>;
template class sccc_encoder<std::int32_t, std::int32_t>;


} /* namespace trellis */
} /* namespace gr */
