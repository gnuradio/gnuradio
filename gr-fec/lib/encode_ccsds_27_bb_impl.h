/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_ENCODE_CCSDS_27_BB_IMPL_H
#define INCLUDED_FEC_ENCODE_CCSDS_27_BB_IMPL_H

#include <gnuradio/fec/encode_ccsds_27_bb.h>

namespace gr {
namespace fec {

class FEC_API encode_ccsds_27_bb_impl : public encode_ccsds_27_bb
{
private:
    unsigned char d_encstate;

public:
    encode_ccsds_27_bb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ENCODE_CCSDS_27_BB_IMPL_H */
