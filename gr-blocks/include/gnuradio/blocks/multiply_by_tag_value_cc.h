/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
      typedef boost::shared_ptr<multiply_by_tag_value_cc> sptr;

      /*!
       * \brief Create an instance of multiply_by_tag_value_cc
       * \param tag_name Tag's key that it will use to get the
       *                 multiplicative constant.
       * \param vlen Vector length of incoming stream
       */
      static sptr make(const std::string& tag_name,
                       size_t vlen=1);

      /*!
       * Get the current multiplicative constant.
       * This block does not allow external setters.
       */
      virtual gr_complex k() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_H */
