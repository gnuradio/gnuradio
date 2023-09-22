/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_PWR_SQUELCH_CC_H
#define INCLUDED_ANALOG_PWR_SQUELCH_CC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/analog/squelch_base_cc.h>
#include <cmath>

namespace gr {
namespace analog {

/*!
 * \brief gate or zero output when input power below threshold
 * \ingroup level_controllers_blk
 */
class ANALOG_API pwr_squelch_cc : public squelch_base_cc, virtual public block
{
protected:
    void update_state(const gr_complex& in) override = 0;
    bool mute() const override = 0;

public:
    // gr::analog::pwr_squelch_cc::sptr
    typedef std::shared_ptr<pwr_squelch_cc> sptr;

    /*!
     * \brief Make power-based squelch block.
     *
     * \param db threshold (in dB) for power squelch
     * \param alpha Gain of averaging filter. Defaults to 0.0001.
     * \param ramp attack / release time in samples; a sinusodial ramp
     *             is used. set to 0 to disable.
     * \param gate if true, no output if no squelch tone.
     *             if false, output 0's if no squelch tone (default).
     *
     * The block will emit a tag with the key pmt::intern("squelch_sob")
     * with the value of pmt::PMT_NIL on the first item it passes, and with
     * the key pmt::intern("squelch_eob") on the last item it passes.
     */
    static sptr make(double db, double alpha = 0.0001, int ramp = 0, bool gate = false);

    std::vector<float> squelch_range() const override = 0;

    virtual double threshold() const = 0;
    virtual void set_threshold(double db) = 0;
    virtual void set_alpha(double alpha) = 0;

    int ramp() const override = 0;
    void set_ramp(int ramp) override = 0;
    bool gate() const override = 0;
    void set_gate(bool gate) override = 0;
    bool unmuted() const override = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PWR_SQUELCH_CC_H */
