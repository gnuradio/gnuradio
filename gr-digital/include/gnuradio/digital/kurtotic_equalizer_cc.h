/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_H
#define INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace digital {

/*!
 * \brief Implements a kurtosis-based adaptive equalizer on complex stream
 * \ingroup equalizers_blk
 *
 * \details
 * Warning: This block does not yet work.
 *
 * "Y. Guo, J. Zhao, Y. Sun, "Sign kurtosis maximization based blind
 * equalization algorithm," IEEE Conf. on Control, Automation,
 * Robotics and Vision, Vol. 3, Dec. 2004, pp. 2052 - 2057."
 */
class DIGITAL_API kurtotic_equalizer_cc : virtual public sync_decimator
{
protected:
    virtual gr_complex error(const gr_complex& out) = 0;
    virtual void update_tap(gr_complex& tap, const gr_complex& in) = 0;

public:
    // gr::digital::kurtotic_equalizer_cc::sptr
    typedef boost::shared_ptr<kurtotic_equalizer_cc> sptr;

    static sptr make(int num_taps, float mu);

    virtual float gain() const = 0;
    virtual void set_gain(float mu) = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_H */
