/*
* Copyright 2015 Free Software Foundation, Inc.
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
*
*/

/*
* Generate a modulated transmit vector corresponding to a particular
* data sequence, resampling rate, and shaping filter. The output is
* suitable for use as a candidate filter for the correlate_and_sync
* block, or just for prototyping.
*
* It accepts a sptr to a modulator block as an argument; given
* suitable data vectors and arguments you should be able to use any
* of the GNU Radio modulator blocks as a modulator. Be careful that
* some modulators expect packed words, some expect unpacked words,
* and some have unique expectations, requirements, or limitations.

* modulator as: gr.block or gr.hier_block
* taps as: list or array
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/blocks/vector_source_b.h>
#include <gnuradio/blocks/vector_sink_c.h>
#include <gnuradio/filter/fir_filter_ccf.h>
#include <gnuradio/top_block.h>

#include <gnuradio/digital/modulate_vector.h>

/* The modulator is assumed to produce data at the
 * desired sps already. The shaping filter taps are
 * already designed; thus, sps should be redundant.
 *
 * What format should incoming symbols be? I guess
 * you have to account for whatever the modulators
 * are designed to take -- generally, unpacked chars.
 *
 * TODO: get JC and TR to deprecate the redundant MSK
 * modulators.
 *
 * Note: Some modulators (MSK) do not support fractional sps.
 *       To get around this, we could use the pfb arb
 *       resampler kernel to resample the output of the
 *       modulator while channel filtering.
 *
 *       If you do this, the taps provided have to be PFB-
 *       compatible, or they have to be rejiggered to make
 *       them thus. Actually, a utility function that created
 *       PFB-friendly taps would be incredibly, hugely useful
 *       just in general.
 *
 *       Right now, let's assume that the modulator works at
 *       fractional SPS, and that the symbol shaping filter
 *       taps are already generated for that same SPS and
 *       can be applied by an ordinary filter.
 *
 */

namespace gr {
    namespace digital {
        std::vector<gr_complex> modulate_vector_bc(
                basic_block_sptr modulator,
                std::vector<uint8_t> data,
                std::vector<float> taps)
        {
            blocks::vector_source_b::sptr vector_src = blocks::vector_source_b::make(data);
            filter::fir_filter_ccf::sptr filter = filter::fir_filter_ccf::make(1, taps);
            blocks::vector_sink_c::sptr vector_sink = blocks::vector_sink_c::make();

            top_block_sptr tb = make_top_block("modulate_vector");

            tb->connect(vector_src, 0, modulator, 0);
            tb->connect(modulator, 0, filter, 0);
            tb->connect(filter, 0, vector_sink, 0);

            tb->run();

            return vector_sink->data();
        }
    } /* namespace digital */
} /* namespace gr */
