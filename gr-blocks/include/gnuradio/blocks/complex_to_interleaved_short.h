/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_COMPLEX_TO_INTERLEAVED_SHORT_H
#define INCLUDED_BLOCKS_COMPLEX_TO_INTERLEAVED_SHORT_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of complex to a stream of interleaved shorts.
 * \ingroup type_converters_blk
 *
 * \details
 * The output stream contains shorts with twice as many output
 * items as input items. For every complex input item, we produce
 * two output shorts that contain the real part and imaginary part
 * converted to shorts:
 *
 * \li output[0][n] = static_cast<short>(input[0][m].real());
 * \li output[0][n+1] = static_cast<short>(input[0][m].imag());
 */
class BLOCKS_API complex_to_interleaved_short : virtual public sync_interpolator
{
public:
    // gr::blocks::complex_to_interleaved_short::sptr
    typedef std::shared_ptr<complex_to_interleaved_short> sptr;

    /*!
     * Build a complex to interleaved shorts block.
     */
    static sptr make(bool vector = false, float scale_factor = 1.0f);

    virtual void set_scale_factor(float new_value) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_COMPLEX_TO_INTERLEAVED_SHORT_H */
