/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_WAVFILE_SOURCE_H
#define INCLUDED_GR_WAVFILE_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Read stream from a Microsoft PCM (.wav) file, output floats
     * \ingroup audio_blk
     *
     * \details
     * Unless otherwise called, values are within [-1;1].
     * Check gr_make_wavfile_source() for extra info.
     */
    class BLOCKS_API wavfile_source : virtual public sync_block
    {
    public:
      // gr::blocks::wavfile_source::sptr
      typedef boost::shared_ptr<wavfile_source> sptr;

      static sptr make(const char *filename, bool repeat = false);

      /*!
       * \brief Read the sample rate as specified in the wav file header
       */
      virtual unsigned int sample_rate() const = 0;

      /*!
       * \brief Return the number of bits per sample as specified in
       * the wav file header. Only 8 or 16 bit are supported here.
       */
      virtual int bits_per_sample() const = 0;

      /*!
       * \brief Return the number of channels in the wav file as
       * specified in the wav file header. This is also the max number
       * of outputs you can have.
       */
      virtual int channels() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_WAVFILE_SOURCE_H */
