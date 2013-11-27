/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TAGS_STROBE_H
#define INCLUDED_GR_TAGS_STROBE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Send tags at defined interval
     * \ingroup debug_blk
     *
     * \details
     * Sends a tag with key 'strobe' and a user-defined value (as a
     * PMT) every \p nsamps number of samples. Useful for
     * testing/debugging the tags system.
     *
     * Because tags are sent with a data stream, this is a source
     * block that acts identical to a null_source block.
     */
    class BLOCKS_API tags_strobe : virtual public sync_block
    {
    public:
      // gr::blocks::tags_strobe::sptr
      typedef boost::shared_ptr<tags_strobe> sptr;

      /*!
       * Make a tags stobe block to send tags with value \p value
       * every \p nsamps number of samples.
       *
       * \param sizeof_stream_item size of the stream items in bytes.
       * \param value The value of the tags to send, as a PMT.
       * \param nsamps the number of samples between each tag.
       * \param key The tag key to sent
       */
      static sptr make(size_t sizeof_stream_item,
                       pmt::pmt_t value, uint64_t nsamps, pmt::pmt_t key = pmt::intern("strobe"));

      /*!
       * Reset the value of the tags being sent.
       * \param value The value of the tags to send as a PMT.
       */
      virtual void set_value(pmt::pmt_t value) = 0;

      /*!
       * Change the tag key we are sending
       */
      virtual void set_key(pmt::pmt_t key) = 0;

      /*!
       * Get the value of the tags being sent.
       */
      virtual pmt::pmt_t value() const = 0;

      /*!
       * Get the key of the tags being sent.
       */
      virtual pmt::pmt_t key() const = 0;

      /*!
       * Reset the sending interval.
       * \param nsamps the number of samples between each tag
       */
      virtual void set_nsamps(uint64_t nsamps) = 0;

      /*!
       * Get the number of samples between the tag strobe.
       */
      virtual uint64_t nsamps() const = 0;

    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TAGS_STROBE_H */
