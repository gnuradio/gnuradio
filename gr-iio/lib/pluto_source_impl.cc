/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pluto_source_impl.h"
#include <iio.h>

#include <cstdio>
#include <string>

namespace gr {
namespace iio {


pluto_source::sptr pluto_source::make(const std::string& uri, unsigned long buffer_size)
{
    fmcomms2_source::sptr block = fmcomms2_source::make(
        uri.empty() ? pluto_source_impl::get_uri() : uri, { true, true }, buffer_size);

    return gnuradio::make_block_sptr<pluto_source_impl>(block);
    // return gnuradio::get_initial_sptr(new pluto_source_impl(block));
}

std::string pluto_source_impl::get_uri()
{
    struct iio_scan_context* ctx = iio_create_scan_context("usb", 0);
    if (!ctx)
        throw std::runtime_error("Unable to create scan context");

    struct iio_context_info** info;
    int ret = iio_scan_context_get_info_list(ctx, &info);
    if (ret < 0) {
        iio_scan_context_destroy(ctx);
        throw std::runtime_error("Unable to scan for Pluto devices");
    }

    if (ret == 0) {
        iio_context_info_list_free(info);
        iio_scan_context_destroy(ctx);
        throw std::runtime_error("No Pluto device found");
    }

    if (ret > 1) {
        printf("More than one Pluto found:\n");

        for (unsigned int i = 0; i < (size_t)ret; i++) {
            printf("\t%d: %s [%s]\n",
                   i,
                   iio_context_info_get_description(info[i]),
                   iio_context_info_get_uri(info[i]));
        }

        printf("We will use the first one.\n");
    }

    std::string uri(iio_context_info_get_uri(info[0]));
    iio_context_info_list_free(info);
    iio_scan_context_destroy(ctx);

    return uri;
}

pluto_source_impl::pluto_source_impl(fmcomms2_source::sptr block)
    : hier_block2("pluto_source",
                  io_signature::make(0, 0, 0),
                  io_signature::make(1, 1, sizeof(gr_complex))),
      fmcomms2_source_f32c(true, false, block)
{
}

void pluto_source_impl::set_frequency(unsigned long long frequency)
{
    fmcomms2_source_f32c::set_frequency(frequency);
}

void pluto_source_impl::set_samplerate(unsigned long samplerate)
{
    fmcomms2_source_f32c::set_samplerate(samplerate);
}

void pluto_source_impl::set_gain_mode(const std::string& mode)
{
    fmcomms2_source_f32c::set_gain_mode(0, mode);
}

void pluto_source_impl::set_gain(double gain) { fmcomms2_source_f32c::set_gain(0, gain); }

void pluto_source_impl::set_quadrature(bool quadrature)
{
    fmcomms2_source_f32c::set_quadrature(quadrature);
}
void pluto_source_impl::set_rfdc(bool rfdc)
{
    fmcomms2_source_f32c::set_quadrature(rfdc);
}
void pluto_source_impl::set_bbdc(bool bbdc)
{
    fmcomms2_source_f32c::set_quadrature(bbdc);
}
// void pluto_source_impl::set_filter_source(const std::string& filter_source)
// {
//     fmcomms2_source_f32c::set_filter_source(filter_source);
// }
// void pluto_source_impl::set_filter_filename(const std::string& filter_filename)
// {
//     fmcomms2_source_f32c::set_filter_filename(filter_filename);
// }
// void pluto_source_impl::set_fpass(float fpass) {
// fmcomms2_source_f32c::set_fpass(fpass); } void pluto_source_impl::set_fstop(float
// fstop) { fmcomms2_source_f32c::set_fstop(fstop); }

void pluto_source_impl::set_filter_params(const std::string& filter_source,
                                          const std::string& filter_filename,
                                          float fpass,
                                          float fstop)
{
    fmcomms2_source_f32c::set_filter_params(filter_source, filter_filename, fpass, fstop);
};

} // namespace iio
} // namespace gr
