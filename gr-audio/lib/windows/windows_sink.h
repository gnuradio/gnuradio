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

#ifndef INCLUDED_AUDIO_WINDOWS_SINK_H
#define INCLUDED_AUDIO_WINDOWS_SINK_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX		// stops windef.h defining max/min under cygwin

#include <windows.h>
#include <mmsystem.h>

#include <gnuradio/audio/sink.h>
#include <string>

namespace gr {
  namespace audio {

    /*!
     * \brief audio sink using winmm mmsystem (win32 only)
     * \ingroup audio_blk
     *
     * input signature is one or two streams of floats.
     * Input samples must be in the range [-1,1].
     */
    class windows_sink : public sink
    {
      int         d_sampling_freq;
      std::string d_device_name;
      int         d_fd;
      LPWAVEHDR  *d_buffers;
      DWORD       d_chunk_size;
      DWORD	      d_buffer_size;
      bool        d_ok_to_block;
      HWAVEOUT    d_h_waveout;
      HANDLE      d_wave_write_event;
      WAVEFORMATEX wave_format;

    protected:
      int string_to_int(const std::string & s);
      int open_waveout_device(void);
      int write_waveout(LPWAVEHDR lp_wave_hdr);
      MMRESULT is_format_supported(LPWAVEFORMATEX pwfx, UINT uDeviceID);
      bool is_number(const std::string& s);
      UINT find_device(std::string szDeviceName);

    public:
      windows_sink(int sampling_freq,
                   const std::string device_name,
                   bool ok_to_block);
      ~windows_sink();

      int work(int noutput_items,
               gr_vector_const_void_star & input_items,
               gr_vector_void_star & output_items);
    };

  } /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_WINDOWS_SINK_H */
