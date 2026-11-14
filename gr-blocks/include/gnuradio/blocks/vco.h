/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 * Copyright 2026 Jay Bhatt
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_VCO_H
#define INCLUDED_GR_VCO_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief VCO - Voltage controlled oscillator
 * \ingroup modulators_blk
 * \ingroup waveform_generators_blk
 *
 * \details
 * input: float stream of control voltages; output: T (float or gr_complex) oscillator output
 */
template <typename T>
class BLOCKS_API vco : virtual public sync_block
{
public:
    // gr::blocks::vco<T>::sptr
    typedef std::shared_ptr<vco<T>> sptr;

    /*!
     * \brief VCO - Voltage controlled oscillator
     *
     * \param sampling_rate sampling rate (Hz)
     * \param sensitivity units are radians/sec/volt
     * \param amplitude output amplitude
     */
    static sptr make(double sampling_rate, double sensitivity, double amplitude);
};

using vco_f = vco<float>;
using vco_c = vco<gr_complex>;

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_VCO_H */
