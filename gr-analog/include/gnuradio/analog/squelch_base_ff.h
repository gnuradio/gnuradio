/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_SQUELCH_BASE_FF_H
#define INCLUDED_ANALOG_SQUELCH_BASE_FF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace analog {

/*!
 * \brief basic squelch block; to be subclassed for other squelches.
 * \ingroup level_blk
 */
class ANALOG_API squelch_base_ff : virtual public block
{
protected:
    virtual void update_state(const float& sample) = 0;
    virtual bool mute() const = 0;

public:
    squelch_base_ff(){};
    virtual int ramp() const = 0;
    virtual void set_ramp(int ramp) = 0;
    virtual bool gate() const = 0;
    virtual void set_gate(bool gate) = 0;
    virtual bool unmuted() const = 0;

    virtual std::vector<float> squelch_range() const = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_SQUELCH_BASE_FF_H */
