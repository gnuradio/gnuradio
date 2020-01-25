/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_ENCODE_CCSDS_27_BB_H
#define INCLUDED_FEC_ENCODE_CCSDS_27_BB_H

#include <gnuradio/fec/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace fec {

/*! \brief A rate 1/2, k=7 convolutional encoder for the CCSDS standard
 * \ingroup error_coding_blk
 *
 * \details
 * This block performs convolutional encoding using the CCSDS standard
 * polynomial ("Voyager").
 *
 * The input is an MSB first packed stream of bits.
 *
 * The output is a stream of symbols 0 or 1 representing the encoded data.
 *
 * As a rate 1/2 code, there will be 16 output symbols for every input byte.
 *
 * This block is designed for continuous data streaming, not packetized data.
 * There is no provision to "flush" the encoder.
 */

class FEC_API encode_ccsds_27_bb : virtual public sync_interpolator
{
public:
    // gr::fec::encode_ccsds_27_bb::sptr
    typedef boost::shared_ptr<encode_ccsds_27_bb> sptr;

    static sptr make();
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ENCODE_CCSDS_27_BB_H */
