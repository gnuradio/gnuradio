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

#ifndef INCLUDED_FEC_GENERIC_DECODER_H
#define INCLUDED_FEC_GENERIC_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/logger.h>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

namespace gr {
  namespace fec {

    /*!
     * \brief Parent class for FECAPI objects.
     *
     * \ingroup error_coding_blk
     *
     * \details
     *
     * Parent of a decoder variable class for FECAPI that will fit
     * into the gr::fec::decoder block to handle FEC decoding. This
     * class provides the basic information required to fit into the
     * FECAPI structure. It provides information about input and
     * output data types, potential data conversions, and a few other
     * parameters useful to establish the decoder's behavior.
     *
     * We create objects from FECAPI-derived classes to go into the
     * actual GNU Radio decoder block. Each object contains its own
     * state and so there should be a one-to-one mapping of an FECAPI
     * object and a GR decoder block. Sharing these objects is not
     * guaranteed to be thread-safe.
     *
     * This is a pure virtual class and must be derived from by a
     * child class.
     *
     * \sa gr::fec::code::cc_decoder
     * \sa gr::fec::code::ccsds_decoder
     */
    class FEC_API generic_decoder
    {
    protected:
      gr::logger_ptr d_logger;

    public:
      friend class decoder;
      virtual void generic_work(void *inbuffer, void *outbuffer) = 0;
      static int base_unique_id;
      int my_id;
      int unique_id();
      std::string d_name;
      std::string alias(){ return (boost::format("%s%d")%d_name%unique_id()).str(); }

    public:
      typedef boost::shared_ptr<generic_decoder> sptr;

      generic_decoder(void) {};
      generic_decoder(std::string name);
      virtual ~generic_decoder();

      /*!
       * Returns the rate of the code. For every r input bits, there
       * is 1 output bit, so the rate is 1/r. Used for setting things
       * like the encoder block's relative rate.
       *
       * This function MUST be reimplemented by the child class.
       */
      virtual double rate() = 0;

      /*!
       * Returns the input size in items that the decoder object uses
       * to decode a full frame. Often, this number is the number of
       * bits per frame if the input format is unpacked. If the block
       * expects packed bytes, then this value should be the number of
       * bytes (number of bits / 8) per input frame.
       *
       * The child class MUST implement this function.
       */
      virtual int get_input_size() = 0;

      /*!
       * Returns the output size in items that the decoder object
       * produces after decoding a full frame. Often, this number is
       * the number of bits in the outputted frame if the input format
       * is unpacked. If the block produces packed bytes, then this
       * value should be the number of bytes (number of bits / 8) per
       * frame produced. This value is generally something like
       * get_input_size()/R for a 1/R rate code.
       *
       * The child class MUST implement this function.
       */
      virtual int get_output_size() = 0;

      /*!
       * Sets up history for the decoder when the decoder is required
       * to look ahead in the data stream in order to finish
       * its processing.
       *
       * The child class MAY implement this function. If not
       * reimplemented, it returns 0.
       */
      virtual int get_history();

      /*!
       * Some decoders require the input items to float around a
       * particular soft value. We can set that floating value by
       * setting this value to return some non-zero number.
       *
       * The fec.extended_decoder block will use this to create an
       * add_const_ff block before the decoder block to adjust all
       * input samples appropriately.
       *
       * The child class MAY implement this function. If not
       * reimplemented, it returns 0.
       */
      virtual float get_shift();

      /*!
       * Sets the size of an input item, as in the size of a char or
       * float item.
       *
       * The child class SHOULD implement this function. If not
       * reimplemented, it returns sizeof(float) as the decoders
       * typically expect floating point input types.
       */
      virtual int get_input_item_size();

      /*!
       * Sets the size of an output item, as in the size of a char or
       * float item.
       *
       * The child class SHOULD implement this function. If not
       * reimplemented, it returns sizeof(char) as the decoders
       * typically expect to produce bits or bytes.
       */
      virtual int get_output_item_size();

      /*!
       * Set up a conversion type required to setup the data properly
       * for this decoder. The decoder itself will not implement the
       * conversion and expects an external wrapper (e.g.,
       * fec.extended_decoder) to read this value and "do the right
       * thing" to format the data.
       *
       * The default behavior is 'none', which means no conversion is
       * required. Whatever the get_input_item_size() value returns,
       * the input is expected to conform directly to this.
       *
       * This may also return 'uchar', which indicates that the
       * wrapper should convert the standard float samples to unsigned
       * characters, either hard sliced or 8-bit soft symbols. See
       * gr::fec::code::cc_decoder as an example decoder that uses
       * this conversion format.
       *
       * If 'packed_bits', the block expects the inputs to be packed
       * hard bits. Each input item is a unsigned char where each of
       * the 8-bits is a hard bit value.
       *
       * The child class SHOULD implement this function. If not
       * reimplemented, it returns "none".
       */
      virtual const char* get_input_conversion();

      /*!
       * Set up a conversion type required to understand the output
       * style of this decoder. Generally, follow-on processing
       * expects unpacked bits, so we specify the conversion type here
       * to indicate what the wrapper (e.g., fec.extended_decoder)
       * should do to convert the output samples from the decoder into
       * unpacked bits.
       *
       * The default behavior is 'none', which means no conversion is
       * required. This should mean that the output data is produced
       * from this decoder as unpacked bit.
       *
       * If 'unpack', the block produces packed bytes that should be
       * unpacked by the wrapper. See gr::fec::code::ccsds_decoder as
       * an example of a decoder that produces packed bytes.
       *
       * The child class SHOULD implement this function. If not
       * reimplemented, it returns "none".
       */
      virtual const char* get_output_conversion();

      /*!
       * Updates the size of a decoded frame.
       *
       * The child class MUST implement this function and interpret
       * how the \p frame_size information affects the block's
       * behavior. It should also provide bounds checks.
       */
      virtual bool set_frame_size(unsigned int frame_size) = 0;
      

      /*!
       * Get repetitions to decode.
       *
       * The child class should implement this function and return the
       * number of iterations required to decode.
       */
      virtual float get_iterations(){ return -1; }
    };

    /*! see generic_decoder::get_output_size() */
    FEC_API int get_decoder_output_size(generic_decoder::sptr my_decoder);

    /*! see generic_decoder::get_input_size() */
    FEC_API int get_decoder_input_size(generic_decoder::sptr my_decoder);

    /*! see generic_decoder::get_shift() */
    FEC_API float get_shift(generic_decoder::sptr my_decoder);

    /*! see generic_decoder::get_history() */
    FEC_API int get_history(generic_decoder::sptr my_decoder);

    /*! see generic_decoder::get_input_item_size() */
    FEC_API int get_decoder_input_item_size(generic_decoder::sptr my_decoder);

    /*! see generic_decoder::get_output_item_size() */
    FEC_API int get_decoder_output_item_size(generic_decoder::sptr my_decoder);

    /*! see generic_decoder::get_input_conversion() */
    FEC_API const char* get_decoder_input_conversion(generic_decoder::sptr my_decoder);

    /*! see generic_decoder::get_output_conversion() */
    FEC_API const char* get_decoder_output_conversion(generic_decoder::sptr my_decoder);

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_GENRIC_DECODER_H */
