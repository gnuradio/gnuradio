/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_AUDIO_WINDOWS_SINK_H
#define INCLUDED_AUDIO_WINDOWS_SINK_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX		// stops windef.h defining max/min under cygwin

#include <windows.h>
#include <mmsystem.h>

#include <gr_sync_block.h>
#include <string>


class audio_windows_sink;
typedef boost::shared_ptr <audio_windows_sink> audio_windows_sink_sptr;

audio_windows_sink_sptr
audio_windows_make_sink (int sampling_freq, const std::string dev = "");

/*!
 * \brief audio sink using winmm mmsystem (win32 only)
 *
 * input signature is one or two streams of floats.
 * Input samples must be in the range [-1,1].
 */

class audio_windows_sink : public gr_sync_block
{
  friend
    audio_windows_sink_sptr
  audio_windows_make_sink (int sampling_freq, const std::string device_name);

  int    	d_sampling_freq;
  std::string   d_device_name;
  int		d_fd;
  short        *d_buffer;
  int           d_chunk_size;
  HWAVEOUT      d_h_waveout;
  HGLOBAL       d_h_wave_hdr;
  LPWAVEHDR     d_lp_wave_hdr;
  HANDLE        d_wave_write_event;

protected:
  int
  string_to_int (const std::string & s);
  audio_windows_sink (int sampling_freq, const std::string device_name = "");
  int
  open_waveout_device (void);
  int
  write_waveout (HPSTR lp_data, DWORD dw_data_size);

public:
  ~audio_windows_sink ();

  int
  work (int noutput_items,
	gr_vector_const_void_star & input_items,
	gr_vector_void_star & output_items);
};

#endif /* INCLUDED_AUDIO_WINDOWS_SINK_H */
