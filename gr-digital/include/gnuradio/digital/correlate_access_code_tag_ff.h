/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2011,2012,2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_H
#define INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>
#include <string>

namespace gr {
  namespace digital {

    /*!
     * \brief Examine input for specified access code, one bit at a time.
     * \ingroup packet_operators_blk
     *
     * \details
     * input:  stream of floats (generally, soft decisions)
     * output: unaltered stream of bits (plus tags)
     *
     * This block annotates the input stream with tags. The tags have
     * key name [tag_name], specified in the constructor. Used for
     * searching an input data stream for preambles, etc., by slicing
     * the soft decision symbol inputs.
     */
    class DIGITAL_API correlate_access_code_tag_ff : virtual public sync_block
    {
    public:
      // gr::digital::correlate_access_code_tag_ff::sptr
      typedef boost::shared_ptr<correlate_access_code_tag_ff> sptr;

      /*!
       * \param access_code is represented with 1 byte per bit,
       *                    e.g., "010101010111000100"
       * \param threshold maximum number of bits that may be wrong
       * \param tag_name key of the tag inserted into the tag stream
       */
      static sptr make(const std::string &access_code,
		       int threshold,
		       const std::string &tag_name);
      
      /*!
       * \param access_code is represented with 1 byte per bit,
       *                    e.g., "010101010111000100"
       */
      virtual bool set_access_code(const std::string &access_code) = 0;
      virtual void set_threshold(int threshold) = 0;
      virtual void set_tagname(const std::string &tagname) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_H */
