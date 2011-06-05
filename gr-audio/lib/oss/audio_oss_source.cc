/* -*- c++ -*- */
/*
 * Copyright 2004-2011 Free Software Foundation, Inc.
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

#include "gr_audio_registry.h"
#include <audio_oss_source.h>
#include <gr_io_signature.h>
#include <gr_prefs.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>

AUDIO_REGISTER_SOURCE(REG_PRIO_LOW, oss)(
    int sampling_rate, const std::string &device_name, bool ok_to_block
){
    return audio_source::sptr(new audio_oss_source(sampling_rate, device_name, ok_to_block));
}

static std::string
default_device_name ()
{
  return gr_prefs::singleton()->get_string("audio_oss", "default_input_device", "/dev/dsp");
}

audio_oss_source::audio_oss_source (int sampling_rate,
				    const std::string device_name,
				    bool ok_to_block)
  : gr_sync_block ("audio_oss_source",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (1, 2, sizeof (float))),
    d_sampling_rate (sampling_rate),
    d_device_name (device_name.empty() ? default_device_name() : device_name),
    d_fd (-1), d_buffer (0), d_chunk_size (0)
{
  if ((d_fd = open (d_device_name.c_str (), O_RDONLY)) < 0){
    fprintf (stderr, "audio_oss_source: ");
    perror (d_device_name.c_str ());
    throw std::runtime_error ("audio_oss_source");
  }

  double CHUNK_TIME =
    std::max(0.001, gr_prefs::singleton()->get_double("audio_oss", "latency", 0.005));

  d_chunk_size = (int) (d_sampling_rate * CHUNK_TIME);
  set_output_multiple (d_chunk_size);

  d_buffer = new short [d_chunk_size * 2];

  int format = AFMT_S16_NE;
  int orig_format = format;
  if (ioctl (d_fd, SNDCTL_DSP_SETFMT, &format) < 0){
    std::cerr << "audio_oss_source: " << d_device_name << " ioctl failed\n";
    perror (d_device_name.c_str ());
    throw std::runtime_error ("audio_oss_source");
  }

  if (format != orig_format){
    fprintf (stderr, "audio_oss_source: unable to support format %d\n", orig_format);
    fprintf (stderr, "  card requested %d instead.\n", format);
  }

  // set to stereo no matter what.  Some hardware only does stereo
  int channels = 2;
  if (ioctl (d_fd, SNDCTL_DSP_CHANNELS, &channels) < 0 || channels != 2){
    perror ("audio_oss_source: could not set STEREO mode");
    throw std::runtime_error ("audio_oss_source");
  }

  // set sampling freq
  int sf = sampling_rate;
  if (ioctl (d_fd, SNDCTL_DSP_SPEED, &sf) < 0){
    std::cerr << "audio_oss_source: "
	      << d_device_name << ": invalid sampling_rate "
	      << sampling_rate << "\n";
    sampling_rate = 8000;
    if (ioctl (d_fd, SNDCTL_DSP_SPEED, &sf) < 0){
      std::cerr << "audio_oss_source: failed to set sampling_rate to 8000\n";
      throw std::runtime_error ("audio_oss_source");
    }
  }
}

audio_oss_source::~audio_oss_source ()
{
  close (d_fd);
  delete [] d_buffer;
}

int
audio_oss_source::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  float *f0 = (float *) output_items[0];
  float *f1 = (float *) output_items[1];	// will be invalid if this is mono output

  const int shorts_per_item = 2;		// L + R
  const int bytes_per_item = shorts_per_item * sizeof (short);

  // To minimize latency, never return more than CHUNK_TIME
  // worth of samples per call to work.  

  noutput_items = std::min (noutput_items, d_chunk_size);

  int	base = 0;
  int	ntogo = noutput_items;
  
  while (ntogo > 0){
    int nbytes = std::min (ntogo, d_chunk_size) * bytes_per_item;
    int result_nbytes = read (d_fd, d_buffer, nbytes);

    if (result_nbytes < 0){
      perror ("audio_oss_source");
      return -1;		// say we're done
    }

    if ((result_nbytes & (bytes_per_item - 1)) != 0){
      fprintf (stderr, "audio_oss_source: internal error.\n");
      throw std::runtime_error ("internal error");
    }

    int result_nitems = result_nbytes / bytes_per_item;

    // now unpack samples into output streams

    switch (output_items.size ()){
      case 1:			// mono output
	for (int i = 0; i < result_nitems; i++){
	  f0[base+i] = d_buffer[2*i+0] * (1.0 / 32767);
	}
	break;

      case 2:			// stereo output	
	for (int i = 0; i < result_nitems; i++){
	  f0[base+i] = d_buffer[2*i+0] * (1.0 / 32767);
	  f1[base+i] = d_buffer[2*i+1] * (1.0 / 32767);
	}
	break;

      default:
	assert (0);
      }

    ntogo -= result_nitems;
    base += result_nitems;
  }

  return noutput_items - ntogo;
}
