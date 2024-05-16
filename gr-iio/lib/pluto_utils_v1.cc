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

#include <iio/iio.h>

#include <stdexcept>

namespace gr {
namespace iio {

std::string get_pluto_uri()
{
    auto logger = gr::logger("pluto_utils::get_pluto_uri");

    struct iio_scan* scan = iio_scan(nullptr, "usb");
    int scan_error = iio_err(scan);
    if (scan_error) {
        // The scan is NOT valid
        throw std::runtime_error("Unable to perform iio scan. Error code: " +
                                 std::to_string(scan_error));
    }

    size_t scan_count = iio_scan_get_results_count(scan);
    if (scan_count == 0) {
        iio_scan_destroy(scan);
        throw std::runtime_error("No Pluto device found");
    }

    if (scan_count > 1) {
        logger.info("More than one Pluto found:");

        for (size_t i = 0; i < scan_count; ++i) {
            logger.info("\t{:d}: {:s} [{:s}]",
                        i,
                        iio_scan_get_description(scan, i),
                        iio_scan_get_uri(scan, i));
        }

        logger.info("We will use the first one.");
    }

    std::string uri(iio_scan_get_uri(scan, 0));
    iio_scan_destroy(scan);

    return uri;
}
} // namespace iio
} // namespace gr
