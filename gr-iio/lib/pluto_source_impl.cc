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

#include <string>

namespace gr {
namespace iio {


pluto_source::sptr pluto_source::make(const std::string& uri, unsigned long buffer_size)
{
    return gnuradio::make_block_sptr<pluto_source_impl>(
        uri.empty() ? pluto_source_impl::get_uri() : uri, buffer_size);
    // return gnuradio::get_initial_sptr(new pluto_source_impl(block));
}

std::string pluto_source_impl::get_uri()
{
    iio_scan_context* ctx = iio_create_scan_context("usb", 0);
    if (!ctx)
        throw std::runtime_error("Unable to create scan context");

    iio_context_info** info;
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

pluto_source_impl::pluto_source_impl(const std::string& uri, unsigned long buffer_size)
    : fmcomms2_source_impl(device_source_impl::get_context(uri),
                           { true, false, false, false },
                           buffer_size)
{
}

void pluto_source_impl::set_len_tag_key(const std::string& len_tag_key)
{
    fmcomms2_source_impl::set_len_tag_key(len_tag_key);
}
void pluto_source_impl::set_frequency(unsigned long long frequency)
{
    fmcomms2_source_impl::set_frequency(frequency);
}
void pluto_source_impl::set_samplerate(unsigned long samplerate)
{
    fmcomms2_source_impl::set_samplerate(samplerate);
}
void pluto_source_impl::set_gain_mode(size_t chan, const std::string& mode)
{
    fmcomms2_source_impl::set_gain_mode(chan, mode);
}
void pluto_source_impl::set_gain(size_t chan, double gain_value)
{
    fmcomms2_source_impl::set_gain(chan, gain_value);
}
void pluto_source_impl::set_quadrature(bool quadrature)
{
    fmcomms2_source_impl::set_quadrature(quadrature);
}
void pluto_source_impl::set_rfdc(bool rfdc) { fmcomms2_source_impl::set_rfdc(rfdc); }
void pluto_source_impl::set_bbdc(bool bbdc) { fmcomms2_source_impl::set_bbdc(bbdc); }
void pluto_source_impl::set_filter_params(const std::string& filter_source,
                                          const std::string& filter_filename,
                                          float fpass,
                                          float fstop)
{
    fmcomms2_source_impl::set_filter_params(filter_source, filter_filename, fpass, fstop);
}


} // namespace iio
} // namespace gr
