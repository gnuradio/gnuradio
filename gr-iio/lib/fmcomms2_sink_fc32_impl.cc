/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/blocks/float_to_short.h>
#include <gnuradio/iio/fmcomms2_sink_fc32.h>
#include <gnuradio/io_signature.h>

using namespace gr::blocks;

namespace gr {
namespace iio {

fmcomms2_sink_f32c::fmcomms2_sink_f32c(bool tx1_en,
                                       bool tx2_en,
                                       fmcomms2_sink::sptr sink_block)
    : hier_block2("fmcomms2_sink_f32c",
                  io_signature::make((int)tx1_en + (int)tx2_en,
                                     (int)tx1_en + (int)tx2_en,
                                     sizeof(gr_complex)),
                  io_signature::make(0, 0, 0)),
      fmcomms2_block(sink_block)
{
    basic_block_sptr hier = shared_from_this();
    unsigned int num_streams = (int)tx1_en + (int)tx2_en;

    for (unsigned int i = 0; i < num_streams; i++) {
        float_to_short::sptr f2s1 = float_to_short::make(1, 32768.0);
        float_to_short::sptr f2s2 = float_to_short::make(1, 32768.0);
        complex_to_float::sptr c2f = complex_to_float::make();

        connect(hier, i, c2f, 0);
        connect(c2f, 0, f2s1, 0);
        connect(c2f, 1, f2s2, 0);
        connect(f2s1, 0, sink_block, i * 2);
        connect(f2s2, 0, sink_block, i * 2 + 1);
    }
}

fmcomms2_sink_f32c::sptr fmcomms2_sink_f32c::make(const std::string& uri,
                                                  const std::vector<bool>& ch_en,
                                                  unsigned long buffer_size,
                                                  bool cyclic)
{
    fmcomms2_sink::sptr block = fmcomms2_sink::make(uri, ch_en, buffer_size, cyclic);

    return gnuradio::get_initial_sptr(new fmcomms2_sink_f32c(
        (ch_en.size() > 0 && ch_en[0]), (ch_en.size() > 1 && ch_en[1]), block));
}


void fmcomms2_sink_f32c::set_len_tag_key(const std::string& len_tag_key)
{
    fmcomms2_block->set_len_tag_key(len_tag_key);
}
void fmcomms2_sink_f32c::set_bandwidth(unsigned long bandwidth)
{
    fmcomms2_block->set_bandwidth(bandwidth);
}
void fmcomms2_sink_f32c::set_frequency(unsigned long long frequency)
{
    fmcomms2_block->set_frequency(frequency);
}
void fmcomms2_sink_f32c::set_samplerate(unsigned long samplerate)
{
    fmcomms2_block->set_samplerate(samplerate);
}
void fmcomms2_sink_f32c::set_attenuation(size_t chan, double attenuation)
{
    fmcomms2_block->set_attenuation(chan, attenuation);
}
void fmcomms2_sink_f32c::set_filter_params(const std::string& filter_source,
                                           const std::string& filter_filename,
                                           float fpass,
                                           float fstop)
{
    fmcomms2_block->set_filter_params(filter_source, filter_filename, fpass, fstop);
}

} /* namespace iio */
} /* namespace gr */
