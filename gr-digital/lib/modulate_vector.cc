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
* suitable data vectors and arguments you should be able to use any of
* the GNU Radio modulator blocks as a modulator. Be careful that some
* modulators expect packed words, some expect unpacked words, and some
* have unique expectations, requirements, or limitations.

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

namespace gr {
  namespace digital {
    std::vector<gr_complex> modulate_vector_bc(basic_block_sptr modulator,
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
