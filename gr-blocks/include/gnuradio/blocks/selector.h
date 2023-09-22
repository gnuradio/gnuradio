/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
 * Connect the sink at input index to the source at output index.
 *
 * Samples from other input ports are consumed and dumped at a rate
 * which is limited by (but not necessarily the same as) the rate of
 * samples copied from the active input to the active output.
 *
 * Other output ports produce no samples.
 *
 */
class BLOCKS_API selector : virtual public block
{
public:
    typedef std::shared_ptr<selector> sptr;

    /*!
     * Create new selector block and return a shared pointer to it
     *
     * \param itemsize size of the input and output items
     * \param input_index the initially active input index
     * \param output_index the initially active output index
     */
    static sptr
    make(size_t itemsize, unsigned int input_index, unsigned int output_index);

    /*!
     * When enabled is set to false, no output samples are produced.
     * Otherwise samples are copied to the selected output port
     */
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
