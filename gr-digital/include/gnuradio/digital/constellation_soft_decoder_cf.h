/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CONSTELLATION_SOFT_DECODER_CF_H
#define INCLUDED_DIGITAL_CONSTELLATION_SOFT_DECODER_CF_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace digital {

/*!
 * \brief Constellation Decoder
 * \ingroup symbol_coding_blk
 *
 * \details
 * Decode a constellation's points from a complex space to soft
 * bits based on the map and soft decision LUT of the \p
 * consetllation object.
 */
class DIGITAL_API constellation_soft_decoder_cf : virtual public sync_interpolator
{
public:
    // gr::digital::constellation_soft_decoder_cf::sptr
    typedef boost::shared_ptr<constellation_soft_decoder_cf> sptr;

    /*!
     * \brief Make constellation decoder block.
     *
     * \param constellation A constellation derived from class
     * 'constellation'. Use base() method to get a shared pointer to
     * this base class type.
     */
    static sptr make(constellation_sptr constellation);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_SOFT_DECODER_CF_H */
