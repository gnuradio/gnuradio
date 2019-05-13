/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_SELECTOR_H
#define INCLUDED_GR_SELECTOR_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief output[output_index][i] = input[input_index][i]
 * \ingroup misc_blk
 *
 * \details
 * Connect the sink at input index to the source at output index
 * Samples from other input ports are consumed and dumped
 * Other output ports produce no samples
 *
 */
class BLOCKS_API selector : virtual public block
{
public:
    typedef boost::shared_ptr<selector> sptr;

    static sptr
    make(size_t itemsize, unsigned int input_index, unsigned int output_index);

    // When enabled is set to false, no output samples are produced
    // Otherwise samples are copied to the selected output port
    virtual void set_enabled(bool enable) = 0;
    virtual bool enabled() const = 0;

    virtual void set_input_index(unsigned int input_index) = 0;
    virtual int input_index() const = 0;

    virtual void set_output_index(unsigned int output_index) = 0;
    virtual int output_index() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_SELECTOR_H */
