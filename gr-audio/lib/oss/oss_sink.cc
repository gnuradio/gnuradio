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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "audio_registry.h"
#include <oss_sink.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>

namespace gr {
  namespace audio {

    sink::sptr
    oss_sink_fcn(int sampling_rate,
                 const std::string &device_name,
                 bool ok_to_block)
    {
      return sink::sptr
        (new oss_sink(sampling_rate, device_name, ok_to_block));
    }

    static std::string
    default_device_name()
    {
      return prefs::singleton()->get_string
        ("audio_oss", "default_output_device", "/dev/dsp");
    }

    oss_sink::oss_sink(int sampling_rate,
                       const std::string device_name,
                       bool ok_to_block)
      : sync_block("audio_oss_sink",
                      io_signature::make(1, 2, sizeof(float)),
                      io_signature::make(0, 0, 0)),
        d_sampling_rate(sampling_rate),
        d_device_name(device_name.empty() ? default_device_name() : device_name),
        d_fd(-1), d_buffer(0), d_chunk_size(0)
    {
      if((d_fd = open(d_device_name.c_str(), O_WRONLY)) < 0) {
        fprintf(stderr, "audio_oss_sink: ");
        perror(d_device_name.c_str());
        throw std::runtime_error("audio_oss_sink");
      }

      double CHUNK_TIME =
        std::max(0.001,
                 prefs::singleton()->get_double("audio_oss", "latency", 0.005));

      d_chunk_size = (int)(d_sampling_rate * CHUNK_TIME);
      set_output_multiple(d_chunk_size);

      d_buffer = new short[d_chunk_size * 2];

      int format = AFMT_S16_NE;
      int orig_format = format;
      if(ioctl(d_fd, SNDCTL_DSP_SETFMT, &format) < 0) {
        std::cerr << "audio_oss_sink: " << d_device_name << " ioctl failed\n";
        perror(d_device_name.c_str ());
        throw std::runtime_error("audio_oss_sink");
      }

      if(format != orig_format) {
        fprintf(stderr, "audio_oss_sink: unable to support format %d\n", orig_format);
        fprintf(stderr, "  card requested %d instead.\n", format);
      }

      // set to stereo no matter what.  Some hardware only does stereo
      int channels = 2;
      if(ioctl(d_fd, SNDCTL_DSP_CHANNELS, &channels) < 0 || channels != 2) {
        perror("audio_oss_sink: could not set STEREO mode");
        throw std::runtime_error("audio_oss_sink");
      }

      // set sampling freq
      int sf = sampling_rate;
      if(ioctl(d_fd, SNDCTL_DSP_SPEED, &sf) < 0) {
        std::cerr << "audio_oss_sink: "
                  << d_device_name << ": invalid sampling_rate "
                  << sampling_rate << "\n";
        sampling_rate = 8000;
        if(ioctl(d_fd, SNDCTL_DSP_SPEED, &sf) < 0) {
          std::cerr << "audio_oss_sink: failed to set sampling_rate to 8000\n";
          throw std::runtime_error("audio_oss_sink");
        }
      }
    }

    oss_sink::~oss_sink()
    {
      close(d_fd);
      delete [] d_buffer;
    }

    int
    oss_sink::work(int noutput_items,
                   gr_vector_const_void_star &input_items,
                   gr_vector_void_star &output_items)
    {
      const float  *f0, *f1;

      switch(input_items.size()) {
      case 1:              // mono input
        f0 = (const float *)input_items[0];

        for(int i = 0; i < noutput_items; i += d_chunk_size) {
          for(int j = 0; j < d_chunk_size; j++) {
            d_buffer[2*j+0] = (short) (f0[j] * 32767);
            d_buffer[2*j+1] = (short) (f0[j] * 32767);
          }
          f0 += d_chunk_size;
          if(write(d_fd, d_buffer, 2 * d_chunk_size * sizeof (short)) < 0)
            perror("audio_oss_sink: write");
        }
        break;

      case 2:             // stereo input
        f0 = (const float *) input_items[0];
        f1 = (const float *) input_items[1];

        for(int i = 0; i < noutput_items; i += d_chunk_size) {
          for(int j = 0; j < d_chunk_size; j++) {
            d_buffer[2*j+0] = (short)(f0[j] * 32767);
            d_buffer[2*j+1] = (short)(f1[j] * 32767);
          }
          f0 += d_chunk_size;
          f1 += d_chunk_size;
          if(write(d_fd, d_buffer, 2 * d_chunk_size * sizeof (short)) < 0)
            perror("audio_oss_sink: write");
        }
        break;
      }

      return noutput_items;
    }

  } /* namespace audio */
} /* namespace gr */
