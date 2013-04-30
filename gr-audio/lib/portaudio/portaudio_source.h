/* -*- c++ -*- */
/*
 * Copyright 2006-2011,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_AUDIO_PORTAUDIO_SOURCE_H
#define INCLUDED_AUDIO_PORTAUDIO_SOURCE_H

#include <gnuradio/audio/source.h>
#include <gnuradio/buffer.h>
#include <gnuradio/thread/thread.h>
#include <string>
#include <portaudio.h>
#include <stdexcept>

namespace gr {
  namespace audio {

    PaStreamCallback portaudio_source_callback;

    /*!
     * \brief Audio source using PORTAUDIO
     * \ingroup audio_blk
     *
     * Input samples must be in the range [-1,1].
     */
    class portaudio_source : public source
    {
      friend PaStreamCallback portaudio_source_callback;

      unsigned int d_sampling_rate;
      std::string  d_device_name;
      bool         d_ok_to_block;
      bool         d_verbose;

      unsigned int d_portaudio_buffer_size_frames; // number of frames in a portaudio buffer

      PaStream           *d_stream;
      PaStreamParameters  d_input_parameters;

      gr::buffer_sptr        d_writer;  // buffer used between work and callback
      gr::buffer_reader_sptr d_reader;

      gr::thread::mutex              d_ringbuffer_mutex;
      gr::thread::condition_variable d_ringbuffer_cond;
      bool                      d_ringbuffer_ready;

      // random stats
      int d_noverruns;  // count of overruns

      void output_error_msg(const char *msg, int err);
      void bail(const char *msg, int err) throw (std::runtime_error);
      void create_ringbuffer();

    public:
      portaudio_source(int sampling_rate,
                       const std::string device_name,
                       bool ok_to_block);

      ~portaudio_source();

      bool check_topology(int ninputs, int noutputs);

      int work(int ninput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_PORTAUDIO_SOURCE_H */
