/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_INTERLEAVED_CHAR_TO_COMPLEX_H
#define INCLUDED_BLOCKS_INTERLEAVED_CHAR_TO_COMPLEX_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of interleaved chars to a stream of complex
 * \ingroup type_converters_blk
 */
class BLOCKS_API interleaved_char_to_complex : virtual public sync_decimator
{
public:
    // gr::blocks::interleaved_char_to_complex::sptr
    typedef std::shared_ptr<interleaved_char_to_complex> sptr;

    /*!
     * Build an interleaved char to complex block.
     */
    static sptr make(bool vector_input = false, float scale_factor = 1.0f);

    virtual void set_scale_factor(float new_value) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_INTERLEAVED_CHAR_TO_COMPLEX_H */
