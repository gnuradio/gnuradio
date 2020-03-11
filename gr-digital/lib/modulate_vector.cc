/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 *
 */

/*
* Generate a modulated transmit vector corresponding to a particular
* data sequence, resampling rate, and shaping filter. The output is
* suitable for use as a candidate filter for the corr_est block, or
* just for prototyping.
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

#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/digital/modulate_vector.h>
#include <gnuradio/filter/fir_filter_blk.h>
#include <gnuradio/top_block.h>

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
