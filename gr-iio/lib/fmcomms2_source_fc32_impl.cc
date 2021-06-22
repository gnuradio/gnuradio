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

#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/short_to_float.h>
#include <gnuradio/iio/fmcomms2_source_fc32.h>
#include <gnuradio/io_signature.h>

using namespace gr::blocks;

namespace gr {
namespace iio {

fmcomms2_source_f32c::fmcomms2_source_f32c(bool rx1_en,
                                           bool rx2_en,
                                           fmcomms2_source::sptr src_block)
    : hier_block2("fmcomms2_f32c",
                  io_signature::make(0, 0, 0),
                  io_signature::make((int)rx1_en + (int)rx2_en,
                                     (int)rx1_en + (int)rx2_en,
                                     sizeof(gr_complex))),
      fmcomms2_block(src_block)
{
    basic_block_sptr hier = shared_from_this();
    unsigned int num_streams = (int)rx1_en + (int)rx2_en;

    for (unsigned int i = 0; i < num_streams; i++) {
        short_to_float::sptr s2f1 = short_to_float::make(1, 2048.0);
        short_to_float::sptr s2f2 = short_to_float::make(1, 2048.0);
        float_to_complex::sptr f2c = float_to_complex::make(1);

        connect(src_block, i * 2, s2f1, 0);
        connect(src_block, i * 2 + 1, s2f2, 0);
        connect(s2f1, 0, f2c, 0);
        connect(s2f2, 0, f2c, 1);
        connect(f2c, 0, hier, i);
    }
}

fmcomms2_source_f32c::sptr fmcomms2_source_f32c::make(const std::string& uri,
                                                      const std::vector<bool>& ch_en,
                                                      unsigned long buffer_size)
{
    fmcomms2_source::sptr block = fmcomms2_source::make(uri, ch_en, buffer_size);

    return gnuradio::get_initial_sptr(new fmcomms2_source_f32c(
        (ch_en.size() > 0 && ch_en[0]), (ch_en.size() > 1 && ch_en[1]), block));
}

void fmcomms2_source_f32c::set_len_tag_key(const std::string& len_tag_key)
{
    fmcomms2_block->set_len_tag_key(len_tag_key);
}
void fmcomms2_source_f32c::set_frequency(unsigned long long frequency)
{
    fmcomms2_block->set_frequency(frequency);
}
void fmcomms2_source_f32c::set_samplerate(unsigned long samplerate)
{
    fmcomms2_block->set_samplerate(samplerate);
}
void fmcomms2_source_f32c::set_gain_mode(size_t chan, const std::string& mode)
{
    fmcomms2_block->set_gain_mode(chan, mode);
}
void fmcomms2_source_f32c::set_gain(size_t chan, double gain)
{
    fmcomms2_block->set_gain(chan, gain);
}
void fmcomms2_source_f32c::set_quadrature(bool quadrature)
{
    fmcomms2_block->set_quadrature(quadrature);
}
void fmcomms2_source_f32c::set_rfdc(bool rfdc) { fmcomms2_block->set_rfdc(rfdc); }
void fmcomms2_source_f32c::set_bbdc(bool bbdc) { fmcomms2_block->set_bbdc(bbdc); }
void fmcomms2_source_f32c::set_filter_params(const std::string& filter_source,
                                             const std::string& filter_filename,
                                             float fpass,
                                             float fstop)
{
    fmcomms2_block->set_filter_params(filter_source, filter_filename, fpass, fstop);
}


} /* namespace iio */
} /* namespace gr */
