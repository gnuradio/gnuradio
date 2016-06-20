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

#ifndef INCLUDED_FEC_ASYNC_DECODER_H
#define INCLUDED_FEC_ASYNC_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/block.h>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace fec {

    /*!
     * \brief Creates the decoder block for use in GNU Radio
     * flowgraphs from a given FEC API object derived from the
     * generic_decoder class.
     * \ingroup error_coding_blk
     *
     * \details
     *
     * Decodes frames received as async messages over a message
     * port. This decoder deployment expects messages of soft decision
     * symbols in and can produce either packed, PDU messages (\p
     * packed = True) or messages full of unpacked bits (\p packed =
     * False).
     *
     * This decoder works off a full message as one frame or block to
     * decode. The message length is used to calculate the frame
     * length. To support this, the decoder variable used will have
     * had its frame_size set. This block treats that initial
     * frame_size value as the maximum transmission unit (MTU) and
     * will not process frames larger than that after being decoded.
     *
     * The packed PDU form of this deployment is designed to work well
     * with other PDU-based blocks to operate within the processing
     * flow of data packets or frames.
     *
     * Due to differences in how data is packed and processed, this
     * block also offers the ability to change the direction of how
     * bits are packed. All inputs messages are one soft decision per
     * item. By default, the \p rev_pack mode is set to True. Using
     * this setup allows the async block to behave with PDUs in the
     * same operation and format as the tagged stream decoders. That
     * is, putting the same data into both the tagged stream decoder
     * deployment and this with the default setting should produce the
     * same data.
     *
     * Because the block handles data as a full frame per message,
     * this decoder deployment cannot work with any decoders that
     * require history. For example, the gr::fec::code::cc_decoder
     * decoder in streaming mode requires an extra rate*(K-1) bits to
     * complete the decoding, so it would have to wait for the next
     * message to come in and finish processing. Therefore, the
     * streaming mode of the CC decoder is not allowed. The other
     * three modes will work with this deployment since the frame is
     * self-contained for decoding.
     */
    class FEC_API async_decoder : virtual public block
    {
    public:
      typedef boost::shared_ptr<async_decoder> sptr;

      /*!
       * Build the PDU-based FEC decoder block from an FECAPI decoder object.
       *
       * \param my_decoder An FECAPI decoder object child of the generic_decoder class.
       * \param packed Sets output to packed bytes if true; otherwise, 1 bit per byte.
       * \param rev_pack If packing bits, should they be reversed?
       * \param mtu The Maximum Transmission Unit (MTU) of the output
       *            frame that the block will be able to
       *            process. Specified in bytes and defaults to 1500.
       */
      static sptr make(generic_decoder::sptr my_decoder,
                       bool packed=false, bool rev_pack=true,
                       int mtu=1500);

      virtual int general_work(int noutput_items,
                               gr_vector_int& ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items) = 0;
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ASYNC_DECODER_H */
