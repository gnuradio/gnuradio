/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_NOAA_HRPT_DECODER_IMPL_H
#define INCLUDED_NOAA_HRPT_DECODER_IMPL_H

#include <gnuradio/noaa/hrpt_decoder.h>

namespace gr {
  namespace noaa {

    class hrpt_decoder_impl : public hrpt_decoder
    {
    private:
      // Configuration
      bool d_verbose;
      bool d_output_files;

      // Frame-level state
      unsigned short d_current_word;
      unsigned int   d_word_num;
      int            d_frames_seen;

      // Minor frame number
      int d_current_mfnum;
      int d_expected_mfnum;
      int d_seq_errs;

      // Spacecraft address
      int d_address;

      // Minor frame timestamp
      int d_day_of_year;
      int d_milliseconds;
      int d_last_time;

      void process_mfnum();
      void process_address();
      void process_day_of_year();
      void process_milli1();
      void process_milli2();
      void process_milli3();

    public:
      hrpt_decoder_impl(bool verbose, bool output_files);
      ~hrpt_decoder_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace noaa */
} /* namespace gr */

#endif /* INCLUDED_NOAA_HRPT_DECODER_IMPL_H */
