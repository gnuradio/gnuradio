/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/iio/pluto_utils.h>
#include <gnuradio/logger.h>
#include <stdexcept>

#ifdef LIBIIO_V1
#include <iio/iio.h>
#else
#include <iio.h>
#endif


namespace gr {
namespace iio {

std::string get_pluto_uri()
{
    auto logger = gr::logger("pluto_utils::get_pluto_uri");
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
        logger.info("More than one Pluto found:");

        for (unsigned int i = 0; i < (size_t)ret; i++) {
            logger.info("\t{:d}: {:s} [{:s}]",
                        i,
                        iio_context_info_get_description(info[i]),
                        iio_context_info_get_uri(info[i]));
        }

        logger.info("We will use the first one.");
    }

    std::string uri(iio_context_info_get_uri(info[0]));
    iio_context_info_list_free(info);
    iio_scan_context_destroy(ctx);

    return uri;
}
} // namespace iio
} // namespace gr
