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

#ifndef INCLUDED_FEC_ENCODER_H
#define INCLUDED_FEC_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/block.h>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace fec {

    /*!
     * \brief Creates the encoder block for use in GNU Radio
     * flowgraphs from a given FECAPI object derived from the
     * generic_encoder class.
     * \ingroup error_coding_blk
     *
     * \details
     *
     * Generally, we would use the fec.extended_encoder Python
     * implementation to instantiate this. The extended_encoder wraps
     * up a few more details, like taking care of puncturing as well
     * as the encoder itself.
     */
    class FEC_API encoder : virtual public block
    {
    public:
      typedef boost::shared_ptr<encoder> sptr;

      /*!
       * Build the FEC encoder block from an FECAPI encoder object.
       *
       * \param my_encoder An FECAPI encoder object child of the generic_encoder class.
       * \param input_item_size size of a block of data for the encoder.
       * \param output_item_size size of a block of data the encoder will produce.
       */
      static sptr make(generic_encoder::sptr my_encoder,
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

#endif /* INCLUDED_FEC_ENCODER_H */
