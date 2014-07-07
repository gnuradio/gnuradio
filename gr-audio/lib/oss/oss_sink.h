/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_AUDIO_OSS_SINK_H
#define INCLUDED_AUDIO_OSS_SINK_H

#include <gnuradio/audio/sink.h>
#include <string>

namespace gr {
  namespace audio {

    /*!
     * \brief audio sink using OSS
     * \ingroup audio_blk
     *
     * input signature is one or two streams of floats.
     * Input samples must be in the range [-1,1].
     */
    class oss_sink : public sink
    {
      int         d_sampling_rate;
      std::string d_device_name;
      int         d_fd;
      short      *d_buffer;
      int         d_chunk_size;

    public:
      oss_sink(int sampling_rate,
               const std::string device_name = "",
               bool ok_to_block = true);
      ~oss_sink();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_OSS_SINK_H */
