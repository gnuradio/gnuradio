/* -*- c++ -*- */
/*
 * Copyright 2007,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_CVSD_DECODE_BS_IMPL_H
#define INCLUDED_VOCODER_CVSD_DECODE_BS_IMPL_H

#include <gnuradio/vocoder/cvsd_decode_bs.h>

namespace gr {
namespace vocoder {

class cvsd_decode_bs_impl : public cvsd_decode_bs
{
private:
    //! Member functions required by the encoder/decoder
    //! \brief Rounding function specific to CVSD
    //! \return the input value rounded to the nearest integer
    int cvsd_round(double input);

    //! \brief A power function specific to CVSD data formats
    //! \return (radix)^power, where radix and power are short integers
    unsigned int cvsd_pow(short radix, short power);

    //! \brief Sums number of 1's in the input
    //! \return the number of 1s in the four bytes of an input unsigned integer
    unsigned char cvsd_bitwise_sum(unsigned int input);

    short d_min_step;
    short d_max_step;
    double d_step_decay;
    double d_accum_decay;

    int d_K; //!< \brief Size of shift register; the number of output bits remembered in
             //!< shift register
    int d_J; //!< \brief Number of bits in the shift register that are equal; size of run
             //!< of 1s, 0s

    short d_pos_accum_max;
    short d_neg_accum_max;

    int d_accum;                //!< \brief Current value of internal reference
    int d_loop_counter;         //!< \brief Current value of the loop counter
    unsigned int d_runner;      //!< \brief Current value of the shift register
    unsigned int d_runner_mask; //!< \brief Value of the mask to access the last J bits of
                                //!< the shift register
    short d_stepsize;           //!< \brief Current value of the step sizer

public:
    cvsd_decode_bs_impl(short min_step = 10,
                        short max_step = 1280,
                        double step_decay = 0.9990234375,
                        double accum_decay = 0.96875,
                        int K = 32,
                        int J = 4,
                        short pos_accum_max = 32767,
                        short neg_accum_max = -32767);
    ~cvsd_decode_bs_impl() override;

    short min_step() override { return d_min_step; }
    short max_step() override { return d_max_step; }
    double step_decay() override { return d_step_decay; }
    double accum_decay() override { return d_accum_decay; }
    int K() override { return d_K; }
    int J() override { return d_J; }
    short pos_accum_max() override { return d_pos_accum_max; }
    short neg_accum_max() override { return d_neg_accum_max; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_CVSD_DECODE_BS_IMPL_H */
