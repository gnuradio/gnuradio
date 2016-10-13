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

#ifndef INCLUDED_AUDIO_WINDOWS_SOURCE_H
#define INCLUDED_AUDIO_WINDOWS_SOURCE_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX		// stops windef.h defining max/min under cygwin

#include <windows.h>
#include <mmsystem.h>

#include <gnuradio/audio/source.h>
#include <string>

#include <boost/lockfree/spsc_queue.hpp>

namespace gr {
  namespace audio {

    /*!
     * \brief audio source using winmm mmsystem (win32 only)
     * \ingroup audio_blk
     *
     * Output signature is one or two streams of floats.
     * Output samples will be in the range [-1,1].
     */
    class windows_source : public source
    {
      int          d_sampling_freq;
      std::string  d_device_name;
      int          d_fd;
      LPWAVEHDR   *lp_buffers;
      DWORD        d_chunk_size;
      DWORD        d_buffer_size;
      HWAVEIN      d_h_wavein;
      WAVEFORMATEX wave_format;

		protected:
			int string_to_int(const std::string & s);
			int open_wavein_device(void);
			MMRESULT is_format_supported(LPWAVEFORMATEX pwfx, UINT uDeviceID);
			bool is_number(const std::string& s);
			UINT find_device(std::string szDeviceName);
			boost::lockfree::spsc_queue<LPWAVEHDR> buffer_queue{ 100 };

    public:
      windows_source(int sampling_freq,
                     const std::string device_name = "");
      ~windows_source();

      int work(int noutput_items,
               gr_vector_const_void_star & input_items,
               gr_vector_void_star & output_items);
    };

		static void CALLBACK read_wavein(
			HWAVEIN   hwi,
			UINT      uMsg,
			DWORD_PTR dwInstance,
			DWORD_PTR dwParam1,
			DWORD_PTR dwParam2
		);

  } /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_WINDOWS_SOURCE_H */
