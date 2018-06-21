/* -*- c++ -*- */
/* Copyright 2015-2016 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_HEADER_BUFFER_H
#define INCLUDED_DIGITAL_HEADER_BUFFER_H

#include <gnuradio/digital/api.h>
#include <vector>
#include <stdint.h>
#include <stdlib.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Helper class for handling payload headers.
     * \ingroup packet_operators_blk
     *
     * \details
     *
     * This class is used by the header format blocks (e.g.,
     * digital::header_format_default) to make it easier to deal with
     * payload headers. This class functions in two different ways
     * depending on if it is used in a transmitter or receiver. When
     * used in a transmitter, this class helps us build headers out of
     * the fields of the protocol. When used in a receiver, this class
     * helps us parse the received bits into the protocol's fields.
     *
     * This page describes how to work with the different modes,
     * transmit or receive. The class is instructed as to which mode
     * it is in by how the constructor is called. If the constructor
     * is passed a valid array (non NULL), then it is in transmit mode
     * and will pack this buffer with the header fields. If that
     * buffer is NULL, the object is in receive mode.
     *
     * \section header_buffer_tx Transmit Mode
     *
     * When passed a valid buffer in the constructor, this object is in
     * transmit mode. We can then use the add_field[N] functions to
     * add new fields to this header. The buffer MUST be large enough
     * to hold the full header. As this class is meant to work mostly
     * with the digital::header_format_default and child
     * classes, the header length can be read from
     * digital::header_format_default::header_nbytes().
     *
     * Each field is a specific length of 8, 16, 32, or 64 bits that
     * are to be transmitted in network byte order. We can adjust the
     * direction of the bytes by setting the byte-swap flag, \p bs, to
     * true or false.
     *
     * The length argument (\p len) for all add_field[N] calls is the
     * number of bytes actually accounted for in the data
     * structure. Often, we would use the full size of the field,
     * which is sizeof(dtype), and the add_field[N] call defaults to
     * len=N. Occasionally, we may need to use fewer bytes than
     * actually represented by the data type. An example would be the
     * access code used in the header_format_default, which is a
     * uint64_t type but may have fewer bytes used in the actual
     * access code.
     *
     * The function that calls this class is expected to handle the
     * memory handling of the buffer -- both allocating and
     * deallocating.
     *
     * As simple example of using this class in transmit mode:
     *
     * \verbatim
     uint8_t* buffer = (uint8_t*)volk_malloc(header_nbytes(),
                                             volk_get_alignment());

     header_buffer hdr(buffer);
     hdr.add_field64(sync_word, sync_word_len);
     hdr.add_field16(payload_length);
     hdr.add_field8(header_flags);
     hdr.add_field8(header_options);

     // Do something with the header

     volk_free(buffer);
     \endverbatim
     *
     * In this example, the header contains four fields:
     *
     * \verbatim
       |0                           15|16          23|24          31|
       |                          sync word                         |
       |                                                            |
       |           length             |     flags    |   options    |
       \endverbatim
     *
     * The sync word can be up to 64-bits, but the add_field64 is also
     * passed the number of actual bytes in the sync word and so could
     * be fewer.
     *
     * \section header_buffer_rx Receive Mode
     *
     * In receive mode, we build up the header as bits are received by
     * inserting them with insert_bit. We can find out how long the
     * current header is, in bits, using the call to length(). If the
     * header is of the appropriate length, we can then start
     * extracting the fields from it. When we are done with the
     * current header, call clear() to reset the internal buffer to
     * empty, which will mean that length() returns 0.
     *
     * The header fields are extracted using the extract_field[N]
     * functions. Like the add_field[N] functions, we specify the size
     * (in bits) of the field we are extracting. We pass this function
     * the bit-position of the expected field in the received header
     * buffer. The extract_field[N] assumes that the number of bits
     * for the field is N, but we can tell the function to use fewer
     * bits if we want. Setting the length parameter of these
     * functions greater than N is illegal, and it will throw an
     * error.
     *
     * For example, given a header of | length | seq. num. | where the
     * length is 16 bits and the sequence number is 32 bits, we would
     * use:
     *
     * \verbatim
          uint16_t len = d_hdr_reg.extract_field16(0);
          uint32_t seq = d_hdr_reg.extract_field32(16);
       \endverbatim
     *
     * The extract_field functions are specific to data types of the
     * field and the number of bits for each field is inferred by the
     * data type. So extract_field16 assumes we want all 16 bits in
     * the field represented.
     *
     * Some headers have fields that are not standard sizes of
     * integers, like a 1 bit, 4 bit, or even 12 bit fields. We can
     * ask for fewer bits for each field. say:
     *
     * \verbatim
       |0          15|16     19|20          31|
       |     len     |  flags  |   options    |
       \endverbatim
     *
     * We would use the following extraction functions:
     *
     * \verbatim
          uint16_t len   = d_hdr_reg.extract_field16(0);
          uint8_t  flags = d_hdr_reg.extract_field8(16, 4);
          uint16_t opts  = d_hdr_reg.extract_field16(20, 12);
       \endverbatim
     *
     * \sa header_format_default
     * \sa header_format_counter
     * \sa header_format_crc
     */
    class DIGITAL_API header_buffer
    {
    private:
      size_t d_offset;
      uint8_t *d_buffer;

      std::vector<bool> d_input;

    public:
      /*!
       * Create a header buffer object with a pre-allocated buffer, \p
       * buffer, to hold the formatted header data.
       *
       * If \p buffer is set to NULL, then this object is in receive
       * mode meant to receive bits from an incoming data stream and
       * provide the ability to extract fields. In this mode, calls to
       * add_field are invalid and will be nops.
       */
      header_buffer(uint8_t *buffer=NULL);

      /*!
       * Class destructor.
       */
      ~header_buffer();

      /*!
       * Clears the header.
       *
       * In transmit mode, this resets the current offset so new
       * add_field functions start adding data to the start of the
       * buffer.
       *
       * In receive mode, this clears the buffer that we have inserted
       * bits in to.
       */
      void clear();


      /*!
       * In transmit mode, this returns the length of the data in
       * the buffer (not the allocated buffer length).
       *
       * In receiving mode, this returns the current length in bits of
       * the received header.
       */
      size_t length() const;

      /*!
       * Returns a constant pointer to the buffer.
       */
      const uint8_t* header() const;

      /*!
       * Add an 8-bit field to the header.
       *
       * \param data The 8-bit data item.
       * \param len Length (in bits) of \p data.
       * \param bs Set to 'true' to byte swap the data.
       */
      void add_field8(uint8_t data, int len=8, bool bs=false);

      /*!
       * Add an 16-bit field to the header.
       *
       * \param data The 16-bit data item.
       * \param len Length (in bits) of \p data.
       * \param bs Set to 'true' to byte swap the data.
       */
      void add_field16(uint16_t data, int len=16, bool bs=false);

      /*!
       * Add an 32-bit field to the header.
       *
       * \param data The 32-bit data item.
       * \param len Length (in bits) of \p data.
       * \param bs Set to 'true' to byte swap the data.
       */
      void add_field32(uint32_t data, int len=32, bool bs=false);

      /*!
       * Add an 64-bit field to the header.
       *
       * \param data The 64-bit data item.
       * \param len Length (in bits) of \p data.
       * \param bs Set to 'true' to byte swap the data.
       */
      void add_field64(uint64_t data, int len=64, bool bs=false);



      /*****************************************************
       *   Receive mode to build a header from bits        *
       *****************************************************/

      /*!
       * Insert a new bit on the back of the input buffer. This
       * function is used in receive mode to add new bits as they are
       * received for later use of the extract_field functions.
       *
       * \param bit New bit to add.
       */
      void insert_bit(int bit);

      /*!
       * Returns up to an 8-bit field in the packet header.
       *
       * \param pos Bit position of the start of the field.
       * \param len The number of bits in the field.
       * \param bs Set to 'true' to byte swap the data.
       */
      uint8_t extract_field8(int pos, int len=8, bool bs=false);

      /*!
       * Returns up to a 16-bit field in the packet header.
       *
       * \param pos Bit position of the start of the field.
       * \param len The number of bits in the field.
       * \param bs Set to 'true' to byte swap the data.
       */
      uint16_t extract_field16(int pos, int len=16, bool bs=false);

      /*!
       * Returns up to a 32-bit field in the packet header.
       *
       * \param pos Bit position of the start of the field.
       * \param len The number of bits in the field.
       * \param bs Set to 'true' to byte swap the data.
       */
      uint32_t extract_field32(int pos, int len=32, bool bs=false);

      /*!
       * Returns up to a 64-bit field in the packet header.
       *
       * \param pos Bit position of the start of the field.
       * \param len The number of bits in the field.
       * \param bs Set to 'true' to byte swap the data.
       */
      uint64_t extract_field64(int pos, int len=64, bool bs=false);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_BUFFER_H */
