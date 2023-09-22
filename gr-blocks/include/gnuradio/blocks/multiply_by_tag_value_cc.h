/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_H
#define INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief output = input * complex constant
 * \ingroup math_operators_blk
 *
 * \details
 * The complex constant used by this block is found from a tag
 * with the name specified by \p tag_name. The tag must contain a
 * float/double or complex PMT value that will be converted into a
 * gr_complex value. All input data is multiplied by this
 * value until a new tag with an update value is found. The block
 * starts with a value of '1.0' for the multiplier constant.
 */
class BLOCKS_API multiply_by_tag_value_cc : virtual public sync_block
{
public:
    // gr::blocks::multiply_by_tag_value_cc::sptr
    typedef std::shared_ptr<multiply_by_tag_value_cc> sptr;

    /*!
     * \brief Create an instance of multiply_by_tag_value_cc
     * \param tag_name Tag's key that it will use to get the
     *                 multiplicative constant.
     * \param vlen Vector length of incoming stream
     */
    static sptr make(const std::string& tag_name, size_t vlen = 1);

    /*!
     * Get the current multiplicative constant.
     * This block does not allow external setters.
     */
    virtual gr_complex k() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_H */
