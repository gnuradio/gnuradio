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
 * constellation object.
 *
 * Does not support constellations of dimensionality higher than 1
 *
 * npwr sets the noise power to use for soft decoding.
 * this is equivalent to 10**(-SNR/10) if the constellation is
 * received at unit average power. defaults to -1 allowing npwr
 * and LUT in the constellation object to be used as-is.
 * values above 0 will recalculate LUT and set the constellation object.
 * noise powers in the range of 0.01-1 are common.
 *
 */
class DIGITAL_API constellation_soft_decoder_cf : virtual public sync_interpolator
{
public:
    // gr::digital::constellation_soft_decoder_cf::sptr
    typedef std::shared_ptr<constellation_soft_decoder_cf> sptr;

    /*!
     * \brief Make constellation decoder block.
     *
     * \param constellation A constellation derived from class
     * 'constellation'. Use base() method to get a shared pointer to
     * this base class type.
     * \param npwr sets expected noise power, default -1 set unused.
     */
    static sptr make(constellation_sptr constellation, float npwr = -1);

    /*!
     * \brief Set the noise power used for soft-decision LLR normalization.
     *
     * For standard QPSK with symbol points at {±1±j}/√2, the average symbol energy is 1.
     * The soft-decision LLRs are automatically scaled by 1/√2 so that the effective
     * noise variance remains consistent with N0 when the LLRs are passed to downstream
     * FEC decoders (Viterbi, LDPC, Turbo, etc.).
     *
     * \param npwr noise power. Default = 1.0 (unit average symbol energy).
     */
    virtual void set_npwr(float npwr) = 0;

    /*!
     * Set a new constellation object for decoding
     *
     * \param constellation A constellation derived from class
     * 'constellation'. Use base() method to get a shared pointer to
     * this base class type.
     */
    virtual void set_constellation(constellation_sptr constellation) = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_SOFT_DECODER_CF_H */