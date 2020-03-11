/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_LDPC_PAR_MTRX_ENCODER_H
#define INCLUDED_LDPC_PAR_MTRX_ENCODER_H

#include <gnuradio/fec/encoder.h>
#include <gnuradio/fec/fec_mtrx.h>
#include <gnuradio/fec/ldpc_H_matrix.h>
#include <string>
#include <vector>

namespace gr {
namespace fec {
namespace code {

class FEC_API ldpc_par_mtrx_encoder : virtual public generic_encoder
{
public:
    static generic_encoder::sptr make(std::string alist_file, unsigned int gap = 0);
    static generic_encoder::sptr make_H(const code::ldpc_H_matrix::sptr H_obj);

    virtual double rate() = 0;
    virtual bool set_frame_size(unsigned int frame_size) = 0;
    virtual int get_output_size() = 0;
    virtual int get_input_size() = 0;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_LDPC_PAR_MTRX_ENCODER_H */
