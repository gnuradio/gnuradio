/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#include <noaa_hrpt_decoder.h>
#include <noaa_hrpt.h>
#include <gr_io_signature.h>
#include <cstdio>

static const char *hrpt_ids[] = {
  "000000",
  "NOAA11",
  "000002",
  "NOAA16",
  "000004",
  "000005",
  "000006",
  "NOAA15",
  "000008",
  "NOAA12",
  "000010",
  "NOAA17",
  "000012",
  "NOAA18",
  "000014",
  "NOAA19"
};

noaa_hrpt_decoder_sptr
noaa_make_hrpt_decoder(bool verbose, bool output_files)
{
  return gnuradio::get_initial_sptr(new noaa_hrpt_decoder(verbose, output_files));
}

noaa_hrpt_decoder::noaa_hrpt_decoder(bool verbose, bool output_files)
  : gr_sync_block("noaa_hrpt_decoder",
		  gr_make_io_signature(1, 1, sizeof(short)),
		  gr_make_io_signature(0, 0, 0)),
    d_verbose(verbose),
    d_output_files(output_files),
    d_word_num(0),
    d_frames_seen(0),
    d_current_mfnum(0),
    d_expected_mfnum(0),
    d_seq_errs(0),
    d_address(0),
    d_day_of_year(0),
    d_milliseconds(0),
    d_last_time(0)
{
  // Start of capture processing here
}

int
noaa_hrpt_decoder::work(int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  const unsigned short *in = (const unsigned short*)input_items[0];

  int i = 0;
  while (i < noutput_items) {
    d_current_word = in[i++] & 0x3FF;
    d_word_num++;

    // Per HRPT word processing here

    switch (d_word_num) {
    case 7:
      process_mfnum();
      process_address();
      break;

    case 9:
      process_day_of_year();
      break;

    case 10:
      process_milli1();
      break;

    case 11:
      process_milli2();
      break;

    case 12:
      process_milli3();
      break;

    default:
      break;
    }

    if (d_word_num == HRPT_MINOR_FRAME_WORDS) {

      // End of minor frame processing here
      d_frames_seen++;
      d_word_num = 0;
      fprintf(stderr, "\n");
    }
  }

  return i;
}

void
noaa_hrpt_decoder::process_mfnum()
{
  d_current_mfnum = (d_current_word & 0x180) >> 7;

  if (d_verbose)
    fprintf(stderr, "MF:");

  if (d_current_mfnum != d_expected_mfnum && d_frames_seen > 0) {
    d_seq_errs++;

    if (d_verbose)
      fprintf(stderr, "*");
  }
  else
    if (d_verbose)
      fprintf(stderr, " ");
  
  if (d_verbose)
    fprintf(stderr, "%i  ", d_current_mfnum);
  d_expected_mfnum = (d_current_mfnum == 3) ? 1 : d_current_mfnum+1;
}

void
noaa_hrpt_decoder::process_address()
{
  d_address = ((d_current_word & 0x078) >> 3) & 0x000F;

  if (d_verbose)
    fprintf(stderr, "SA: %s  ", hrpt_ids[d_address]);
}

void
noaa_hrpt_decoder::process_day_of_year()
{
  d_day_of_year = d_current_word >> 1;

  if (d_verbose)
    fprintf(stderr, "DOY: %3i  ", d_day_of_year);
}

void
noaa_hrpt_decoder::process_milli1()
{
  d_milliseconds = (d_current_word & 0x7F) << 20;
}

void
noaa_hrpt_decoder::process_milli2()
{
  d_milliseconds |= (d_current_word << 10);
}

void
noaa_hrpt_decoder::process_milli3()
{
  d_milliseconds |= d_current_word;
  int delta = d_milliseconds - d_last_time;
  d_last_time = d_milliseconds;

  if (d_verbose)
    fprintf(stderr, "MS: %8i  DT: %8i", d_milliseconds, delta);
}

noaa_hrpt_decoder::~noaa_hrpt_decoder()
{
  // End of capture processing here

  if (d_verbose) {
    fprintf(stderr, "Frames seen:     %10i\n", d_frames_seen);
    fprintf(stderr, "Sequence errors: %10i\n", d_seq_errs);
  }
}
