/* -*- c++ -*- */
/*
 * Copyright 2023 Martin Hübner
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DIGITAL_AFSK_MOD_BF_H
#define INCLUDED_DIGITAL_AFSK_MOD_BF_H

#include <gnuradio/digital/api.h>
#include <gnuradio/endianness.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace digital {

/*!
 * \brief Transforms a byte stream into an AFSK modulated Audio signal. Input
 * can either be unpacked bits (one bit per byte) or normal bytes. In that case
 * the block can handle unpacking them.
 * \ingroup digital
 *
 */
class DIGITAL_API afsk_mod_bf : virtual public gr::sync_interpolator
{
public:
    typedef std::shared_ptr<afsk_mod_bf> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of digital::afsk_mod_bf.
     *
     * To avoid accidental use of raw pointers, digital::afsk_mod_bf's
     * constructor is in a private implementation
     * class. digital::afsk_mod_bf::make is the public interface for
     * creating new instances.
     */
    static sptr make(int samp_rate = 48000,
                     float freq_zero = 1200.0,
                     float freq_one = 2200.0,
                     int bit_rate = 1200,
                     bool bits = true,
                     endianness_t endianness = GR_LSB_FIRST);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_AFSK_MOD_BF_H */
