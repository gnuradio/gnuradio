/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "encode_ccsds_27_bb_impl.h"
#include <gnuradio/io_signature.h>

#include <gnuradio/fec/viterbi.h>

namespace gr {
namespace fec {

encode_ccsds_27_bb::sptr encode_ccsds_27_bb::make()
{
    return gnuradio::get_initial_sptr(new encode_ccsds_27_bb_impl());
}

encode_ccsds_27_bb_impl::encode_ccsds_27_bb_impl()
    : sync_interpolator("encode_ccsds_27_bb",
                        io_signature::make(1, 1, sizeof(char)),
                        io_signature::make(1, 1, sizeof(char)),
                        16) // Rate 1/2 code, packed to unpacked conversion
{
    d_encstate = 0;
}

int encode_ccsds_27_bb_impl::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    unsigned char* in = (unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    d_encstate = encode(out, in, noutput_items / 16, d_encstate);

    return noutput_items;
}

} /* namespace fec */
} /* namespace gr */
