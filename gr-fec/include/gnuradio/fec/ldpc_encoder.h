/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_LDPC_ENCODER_H
#define INCLUDED_LDPC_ENCODER_H

#include <gnuradio/fec/encoder.h>
#include <string>
#include <vector>

namespace gr {
namespace fec {

class FEC_API ldpc_encoder : virtual public generic_encoder
{
public:
    static generic_encoder::sptr make(std::string alist_file);

    virtual double rate() = 0;
    virtual bool set_frame_size(unsigned int frame_size) = 0;
    virtual int get_output_size() = 0;
    virtual int get_input_size() = 0;
};

} // namespace fec
} // namespace gr

#endif /* INCLUDED_LDPC_ENCODER_H */
