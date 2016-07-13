/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FEC_ASYNC_ENCODER_H
#define INCLUDED_FEC_ASYNC_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/block.h>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace fec {

    /*!
     * \brief Creates the encoder block for use in GNU Radio
     * flowgraphs with async message from a given FEC API object
     * derived from the generic_encoder class.
     * \ingroup error_coding_blk
     *
     * \details
     *
     * Encodes frames received as async messages or as a PDU over a
     * message port. This encoder works off a full message as one
     * frame or block to encode. The message length is used as the
     * frame length. To support this, the encoder variable used will
     * have had its frame_size set. This block treats that initial
     * frame_size value as the maximum transmission unit (MTU) and
     * will not process frames larger than that.
     *
     * This deployment works off messages and expects them to either
     * be messages full of unpacked bits or PDU messages, which means
     * full bytes of a frame from the higher layers, including things
     * like headers, tails, CRC check bytes, etc. For handling PDUs,
     * set the \p packed option of this deployment block to True. The
     * block will then use the FEC API to properly unpack the bits
     * from the PDU, pass it through the encoder, and repack them to
     * output the PDUs for the next stage of processing.
     *
     * The packed PDU form of this deployment is designed to work well
     * with other PDU-based blocks to operate within the processing
     * flow of data packets or frames.
     *
     * Due to differences in how data is packed and processed, this
     * block also offers the ability to change the direction of how
     * bits are unpacked and packed, where reading or writing from the
     * LSB or MSB. By default, the \p rev_unpack and \p rev_pack modes
     * are set to True. Using this setup allows the async block to
     * behave with PDUs in the same operation and format as the tagged
     * stream encoders. That is, putting the same data into both the
     * tagged stream encoder deployment and this with these default
     * settings should produce the same data.
     */
    class FEC_API async_encoder : virtual public block
    {
    public:
      typedef boost::shared_ptr<async_encoder> sptr;

      /*!
       * Build the PDU-based FEC encoder block from an FECAPI encoder object.
       *
       * \param my_encoder An FECAPI encoder object child of the generic_encoder class.
       * \param packed True if working on packed bytes (like PDUs).
       * \param rev_unpack Reverse the unpacking order from input bytes to bits.
       * \param rev_pack Reverse the packing order from bits to output bytes.
       * \param mtu The Maximum Transmission Unit (MTU) of the input
       *            frame that the block will be able to
       *            process. Specified in bytes and defaults to 1500.
       */
      static sptr make(generic_encoder::sptr my_encoder,
                       bool packed=false,
                       bool rev_unpack=true, bool rev_pack=true,
                       int mtu=1500);

      virtual int general_work(int noutput_items,
                               gr_vector_int& ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items) = 0;
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ASYNC_ENCODER_H */
