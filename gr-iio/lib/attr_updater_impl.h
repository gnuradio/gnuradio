/* -*- c++ -*- */
/*
 * Copyright 2019 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_IIO_ATTR_UPDATER_IMPL_H
#define INCLUDED_IIO_ATTR_UPDATER_IMPL_H

#include <gnuradio/iio/attr_updater.h>

#include <mutex>

namespace gr {
  namespace iio {

    class attr_updater_impl : public attr_updater
    {
     private:
      const std::string d_attribute;
      std::string d_value;
      const pmt::pmt_t d_port;
      bool d_update_on_change;
      unsigned int d_interval_ms;
      bool d_finished;
      bool d_updated;
      pmt::pmt_t d_msg;
      std::mutex d_mtx;
      boost::shared_ptr<gr::thread::thread> d_thread;

      void run();

     public:
      attr_updater_impl(const std::string attribute, const std::string value, unsigned int interval_ms);
      ~attr_updater_impl();

      void set_value(std::string value);
      bool start();
      bool stop();
    };

  } // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_UPDATER_IMPL_H */
