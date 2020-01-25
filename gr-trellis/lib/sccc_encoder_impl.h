/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef SCCC_ENCODER_IMPL_H
#define SCCC_ENCODER_IMPL_H

#include <gnuradio/trellis/sccc_encoder.h>

namespace gr {
namespace trellis {

template <class IN_T, class OUT_T>
class sccc_encoder_impl : public sccc_encoder<IN_T, OUT_T>
{
private:
    fsm d_FSMo;
    int d_STo;
    fsm d_FSMi;
    int d_STi;
    interleaver d_INTERLEAVER;
    int d_blocklength;
    std::vector<int> d_buffer;

public:
    sccc_encoder_impl(const fsm& FSMo,
                      int STo,
                      const fsm& FSMi,
                      int STi,
                      const interleaver& INTERLEAVER,
                      int blocklength);
    ~sccc_encoder_impl();

    fsm FSMo() const { return d_FSMo; }
    int STo() const { return d_STo; }
    fsm FSMi() const { return d_FSMi; }
    int STi() const { return d_STi; }
    interleaver INTERLEAVER() const { return d_INTERLEAVER; }
    int blocklength() const { return d_blocklength; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* SCCC_ENCODER_IMPL_H */
