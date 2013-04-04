/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_OFDM_FRAME_EQUALIZER_VCVC_H
#define INCLUDED_OFDM_FRAME_EQUALIZER_VCVC_H

#include <digital_ofdm_equalizer_base.h>
#include <digital/api.h>
#include <gr_tagged_stream_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief OFDM frame equalizer
     * \ingroup ofdm_blk
     *
     * Performs equalization in one or two dimensions on a tagged OFDM frame.
     * Input: a tagged series of OFDM symbols.
     * Output: The same as the input, but equalized.
     */
    class DIGITAL_API ofdm_frame_equalizer_vcvc : virtual public gr_tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<ofdm_frame_equalizer_vcvc> sptr;

      /*!
       * \param equalizer The equalizer object that will do the actual work
       * \param len_tag_key Length tag key
       * \param propagate_channel_state If true, the channel state after the last symbol
       *                                will be added to the first symbol as a tag
       */
      static sptr make(
	   digital_ofdm_equalizer_base_sptr equalizer,
	   const std::string &len_tag_key = "frame_len",
	   bool propagate_channel_state=false
      );
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_OFDM_FRAME_EQUALIZER_VCVC_H */

