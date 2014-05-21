/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FEC_DECODER_H
#define INCLUDED_FEC_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/block.h>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/format.hpp>

namespace gr {
  namespace fec {

    /*!
     * \brief General FEC decoding block that takes in a decoder
     * variable object (derived from gr::fec::general_decoder) for use
     * in a flowgraph.
     *
     * \ingroup error_coding_blk
     *
     * \details
     * This block uses a decoder variable object (derived from
     * gr::fec::generic_decoder) to decode data within a
     * flowgraph. This block interacts with the general FECAPI
     * architecture to handle all passing all input and output data in
     * a flowgraph. The decoder variable takes care of understanding
     * the requirements, data types and sizes, and boundary conditions
     * of the specific FEC decoding algorithm.
     *
     * Generally, this block is used within the fec.extended_decoder
     * Python block to handle some input/output formatting issues. In
     * the FECAPI, the decoder variable sets properties like the input
     * and output types and sizes and whether the output is packed or
     * unpacked bytes. The fec.extended_decoder uses this information
     * to set up an gr::hier_block2 structure to make sure the I/O to
     * the variable is handled consistently, such as to make sure all
     * inputs are floats with one soft symbol per item and the outputs
     * are unpacked bytes with the bit in the LSB.
     *
     * See gr::fec::generic_decoder for detail on what information an
     * FECAPI variable object can set if using this block directly and
     * not as part of the fec.extended_decoder.
     */
    class FEC_API decoder : virtual public block
    {
    public:
      typedef boost::shared_ptr<decoder> sptr;
      typedef boost::shared_array<unsigned char> buf_sptr;

      /*!
       * Create the FEC decoder block by taking in the FECAPI decoder
       * object as well as input and output sizes.
       *
       * \param my_decoder An FECAPI decoder object (See gr::fec::generic_decoder).
       * \param input_item_size The size of the input items (often the my_decoder object can tell us this).
       * \param output_item_size The size of the output items (often the my_decoder object can tell us this).
       */
      static sptr make(generic_decoder::sptr my_decoder,
                       size_t input_item_size,
                       size_t output_item_size);

      virtual int general_work(int noutput_items,
                               gr_vector_int& ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items) = 0;
      virtual int fixed_rate_ninput_to_noutput(int ninput) = 0;
      virtual int fixed_rate_noutput_to_ninput(int noutput) = 0;
      virtual void forecast(int noutput_items,
                            gr_vector_int& ninput_items_required) = 0;
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DECODER_H */
