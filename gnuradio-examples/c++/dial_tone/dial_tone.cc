/*
 * Copyright 2006,2008 Free Software Foundation, Inc.
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

#include <dial_tone.h>
#include <gr_io_signature.h>
#include <gr_sig_source_f.h>
#include <audio_alsa_sink.h>

// Shared pointer constructor
dial_tone_sptr make_dial_tone()
{
  return gnuradio::get_initial_sptr(new dial_tone());
}

// Hierarchical block constructor, with no inputs or outputs
dial_tone::dial_tone() : 
    gr_top_block("dial_tone")
{
    gr_sig_source_f_sptr src0 = gr_make_sig_source_f(48000, GR_SIN_WAVE, 350, 0.1);
    gr_sig_source_f_sptr src1 = gr_make_sig_source_f(48000, GR_SIN_WAVE, 440, 0.1);
    audio_alsa_sink_sptr sink = audio_alsa_make_sink(48000);

    connect(src0, 0, sink, 0);
    connect(src1, 0, sink, 1);
}
