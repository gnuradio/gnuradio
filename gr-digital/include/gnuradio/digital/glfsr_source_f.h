/* -*- c++ -*- */
/*
 * Copyright 2007,2012,2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_GLFSR_SOURCE_F_H
#define INCLUDED_GR_GLFSR_SOURCE_F_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Galois LFSR pseudo-random source generating float outputs -1.0 - 1.0.
 * \ingroup waveform_generators_blk
 */
class DIGITAL_API glfsr_source_f : virtual public sync_block
{
public:
    // gr::digital::glfsr_source_f::sptr
    typedef std::shared_ptr<glfsr_source_f> sptr;

    /*!
     * Make a Galois LFSR pseudo-random source block.
     *
     * \param degree Degree of shift register must be in [1, 64]. If mask
     *               is 0, the degree determines a default mask (see
     *               digital_impl_glfsr.cc for the mapping).
     * \param repeat Set to repeat sequence.
     * \param mask   Allows a user-defined bit mask for indexes of the shift
     *               register to feed back.
     * \param seed   Initial setting for values in shift register.
     */
    static sptr
    make(unsigned int degree, bool repeat = true, uint64_t mask = 0, uint64_t seed = 1);

    virtual uint64_t period() const = 0;
    virtual uint64_t mask() const = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_GLFSR_SOURCE_F_H */
