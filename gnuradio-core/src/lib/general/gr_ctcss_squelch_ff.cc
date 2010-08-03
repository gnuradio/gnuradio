/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010 Free Software Foundation, Inc.
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

#include <gr_ctcss_squelch_ff.h>

static float ctcss_tones[] = {
   67.0,  71.9,  74.4,  77.0,  79.7,  82.5,  85.4,  88.5,  91.5,  94.8, 
   97.4, 100.0, 103.5, 107.2, 110.9, 114.8, 118.8, 123.0, 127.3, 131.8,
  136.5, 141.3, 146.2, 151.4, 156.7, 162.2, 167.9, 173.8, 179.9, 186.2, 
  192.8, 203.5, 210.7, 218.1, 225.7, 233.6, 241.8, 250.3
};

static int max_tone_index = 37;

gr_ctcss_squelch_ff_sptr
gr_make_ctcss_squelch_ff(int rate, float freq, float level, int len, int ramp, bool gate)
{
  return gnuradio::get_initial_sptr(new gr_ctcss_squelch_ff(rate, freq, level, len, ramp, gate));
}

int gr_ctcss_squelch_ff::find_tone(float freq)
{
  for (int i = 0; i <= max_tone_index; i++)
    if (ctcss_tones[i] == freq) // FIXME: make almost equal
      return i;

  return -1;
}

gr_ctcss_squelch_ff::gr_ctcss_squelch_ff(int rate, float freq, float level, int len, int ramp, bool gate) : 
    gr_squelch_base_ff("ctcss_squelch_ff", ramp, gate)
{
  d_freq = freq;
  d_level = level;

  // Default is 100 ms detection time
  if (len == 0)
    d_len = (int)(rate/10.0);
  else
    d_len = len;

  int i = find_tone(freq);

  // Non-standard tones or edge tones get 2% guard band, otherwise
  // guards are set at adjacent ctcss tone frequencies
  float f_l, f_r;
  if (i == -1 || i == 0)
    f_l = freq*0.98;
  else
    f_l = ctcss_tones[i-1];
    
  if (i == -1 || i == max_tone_index)
    f_r = freq*1.02;
  else
    f_r = ctcss_tones[i+1];

  d_goertzel_l = gri_goertzel(rate, d_len, f_l);
  d_goertzel_c = gri_goertzel(rate, d_len, freq);
  d_goertzel_r = gri_goertzel(rate, d_len, f_r);

  d_mute = true;
}

std::vector<float> gr_ctcss_squelch_ff::squelch_range() const
{
  std::vector<float> r(3);
  r[0] = 0.0;
  r[1] = 1.0;
  r[2] = (r[1]-r[0])/100; // step size

  return r;
}

void gr_ctcss_squelch_ff::update_state(const float &in)
{
  d_goertzel_l.input(in);
  d_goertzel_c.input(in);
  d_goertzel_r.input(in);

  float d_out_l, d_out_c, d_out_r;
  if (d_goertzel_c.ready()) {
      d_out_l = abs(d_goertzel_l.output());
      d_out_c = abs(d_goertzel_c.output());
      d_out_r = abs(d_goertzel_r.output());
  
      //printf("d_out_l=%f d_out_c=%f d_out_r=%f\n", d_out_l, d_out_c, d_out_r);
      d_mute = (d_out_c < d_level || d_out_c < d_out_l || d_out_c < d_out_r);
  }
}
