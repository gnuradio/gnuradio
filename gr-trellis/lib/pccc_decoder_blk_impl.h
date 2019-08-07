/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
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


#ifndef PCCC_DECODER_BLK_IMPL_H
#define PCCC_DECODER_BLK_IMPL_H

#include <gnuradio/trellis/pccc_decoder_blk.h>

namespace gr {
namespace trellis {

template <class T>
class pccc_decoder_blk_impl : public pccc_decoder_blk<T>
{
private:
    fsm d_FSM1;
    int d_ST10;
    int d_ST1K;
    fsm d_FSM2;
    int d_ST20;
    int d_ST2K;
    interleaver d_INTERLEAVER;
    int d_blocklength;
    int d_repetitions;
    siso_type_t d_SISO_TYPE;
    std::vector<float> d_buffer;

public:
    pccc_decoder_blk_impl(const fsm& FSM1,
                          int ST10,
                          int ST1K,
                          const fsm& FSM2,
                          int ST20,
                          int ST2K,
                          const interleaver& INTERLEAVER,
                          int blocklength,
                          int repetitions,
                          siso_type_t SISO_TYPE);
    ~pccc_decoder_blk_impl();

    fsm FSM1() const { return d_FSM1; }
    fsm FSM2() const { return d_FSM2; }
    int ST10() const { return d_ST10; }
    int ST1K() const { return d_ST1K; }
    int ST20() const { return d_ST20; }
    int ST2K() const { return d_ST2K; }
    interleaver INTERLEAVER() const { return d_INTERLEAVER; }
    int blocklength() const { return d_blocklength; }
    int repetitions() const { return d_repetitions; }
    siso_type_t SISO_TYPE() const { return d_SISO_TYPE; }

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* PCCC_DECODER_BLK_IMPL_H */
