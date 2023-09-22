/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_CPFSK_BC_H
#define INCLUDED_ANALOG_CPFSK_BC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace analog {

/*!
 * \brief Perform continuous phase 2-level frequency shift keying modulation
 * on an input stream of unpacked bits.
 * \ingroup modulators_blk
 * \ingroup deprecated_blk
 */
class ANALOG_API cpfsk_bc : virtual public sync_interpolator
{
public:
    // gr::analog::cpfsk_bc::sptr
    typedef std::shared_ptr<cpfsk_bc> sptr;

    /*!
     * \brief Make a CPFSK block.
     *
     * \param k modulation index
     * \param ampl output amplitude
     * \param samples_per_sym	number of output samples per input bit
     */
    static sptr make(float k, float ampl, int samples_per_sym);

    virtual void set_amplitude(float amplitude) = 0;
    virtual float amplitude() = 0;
    virtual float freq() = 0;
    virtual float phase() = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_CPFSK_BC_H */
