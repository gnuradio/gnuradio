/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CONSTELLATION_ENCODER_BC_H
#define INCLUDED_DIGITAL_CONSTELLATION_ENCODER_BC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace digital {

/*!
 * \brief Constellation Encoder
 * \ingroup symbol_coding_blk
 *
 * \details
 * Encode index of constellation points into a constellation's complex
 * space based on the map of the \p constellation object.
 */
class DIGITAL_API constellation_encoder_bc : virtual public sync_interpolator
{
public:
    // gr::digital::constellation_encoder_bc::sptr
    typedef std::shared_ptr<constellation_encoder_bc> sptr;

    /*!
     * \brief Make constellation encoder block.
     *
     * \param constellation A constellation derived from class
     * 'constellation'. Use base() method to get a shared pointer to
     * this base class type.
     */
    static sptr make(constellation_sptr constellation);

    /*!
     * Set a new constellation object for encoding
     *
     * \param constellation A constellation derived from class
     * 'constellation'. Use base() method to get a shared pointer to
     * this base class type.
     */
    virtual void set_constellation(constellation_sptr constellation) = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_ENCODER_BC_H */
