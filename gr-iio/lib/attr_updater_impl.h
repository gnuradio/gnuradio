/* -*- c++ -*- */
/*
 * Copyright 2019 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_ATTR_UPDATER_IMPL_H
#define INCLUDED_IIO_ATTR_UPDATER_IMPL_H

#include <gnuradio/iio/attr_updater.h>

#include <mutex>
#include <string>

namespace gr {
namespace iio {

class attr_updater_impl : public attr_updater
{
private:
    const std::string d_attribute;
    std::string d_value;
    const pmt::pmt_t d_port;
    unsigned int d_interval_ms;
    bool d_finished;
    bool d_updated;
    pmt::pmt_t d_msg;
    std::mutex d_mtx;
    std::shared_ptr<gr::thread::thread> d_thread;

    void run();

public:
    attr_updater_impl(const std::string attribute,
                      const std::string value,
                      unsigned int interval_ms);
    ~attr_updater_impl();

    void set_value(std::string value);
    bool start();
    bool stop();
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_UPDATER_IMPL_H */
