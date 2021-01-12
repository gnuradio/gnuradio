/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_CTCSS_SQUELCH_FF_H
#define INCLUDED_ANALOG_CTCSS_SQUELCH_FF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/analog/squelch_base_ff.h>
#include <gnuradio/block.h>

namespace gr {
namespace analog {

/*!
 * \brief gate or zero output if CTCSS tone not present
 * \ingroup level_controllers_blk
 */
class ANALOG_API ctcss_squelch_ff : public squelch_base_ff, virtual public block
{
protected:
    void update_state(const float& in) override = 0;
    bool mute() const override = 0;

public:
    // gr::analog::ctcss_squelch_ff::sptr
    typedef std::shared_ptr<ctcss_squelch_ff> sptr;

    /*!
     * \brief Make CTCSS tone squelch block.
     *
     * \param rate gain of the internal frequency filters.
     * \param freq frequency value to use as the squelch tone.
     * \param level threshold level for the squelch tone.
     * \param len length of the frequency filters.
     * \param ramp attack / release time in samples; a sinusodial ramp
     *             is used. set to 0 to disable.
     * \param gate if true, no output if no squelch tone.
     *             if false, output 0's if no squelch tone.
     */
    static sptr make(int rate, float freq, float level, int len, int ramp, bool gate);

    std::vector<float> squelch_range() const override = 0;
    virtual float level() const = 0;
    virtual void set_level(float level) = 0;
    virtual int len() const = 0;
    virtual float frequency() const = 0;
    virtual void set_frequency(float frequency) = 0;

    int ramp() const override = 0;
    void set_ramp(int ramp) override = 0;
    bool gate() const override = 0;
    void set_gate(bool gate) override = 0;
    bool unmuted() const override = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_CTCSS_SQUELCH_FF_H */
