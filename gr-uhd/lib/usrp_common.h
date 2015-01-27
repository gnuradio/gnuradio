/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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
 */

#ifndef INCLUDED_GR_UHD_USRP_COMMON_H
#define INCLUDED_GR_UHD_USRP_COMMON_H

#include <pmt/pmt.h>
#include <boost/dynamic_bitset.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/convert.hpp>
#include <iostream>

namespace gr {
  namespace uhd {
    typedef boost::function< ::uhd::sensor_value_t (const std::string&)> get_sensor_fn_t;

    static const double LOCK_TIMEOUT = 1.5; // s

    //! Helper function for msg_handler_command:
    // - Extracts command and the command value from the command PMT
    // - Returns true if the command PMT is well formed
    // - If a channel is given, return that as well, otherwise set the channel to -1
    static bool _unpack_chan_command(
        std::string &command,
        pmt::pmt_t &cmd_val,
        int &chan,
        const pmt::pmt_t &cmd_pmt
    ) {
      try {
        chan = -1; // Default value
        if (pmt::is_tuple(cmd_pmt) and (pmt::length(cmd_pmt) == 2 or pmt::length(cmd_pmt) == 3)) {
          command = pmt::symbol_to_string(pmt::tuple_ref(cmd_pmt, 0));
          cmd_val = pmt::tuple_ref(cmd_pmt, 1);
          if (pmt::length(cmd_pmt) == 3) {
            chan = pmt::to_long(pmt::tuple_ref(cmd_pmt, 2));
          }
        }
        else if (pmt::is_pair(cmd_pmt)) {
          command = pmt::symbol_to_string(pmt::car(cmd_pmt));
          cmd_val = pmt::cdr(cmd_pmt);
          if (pmt::is_pair(cmd_val)) {
            chan = pmt::to_long(pmt::car(cmd_val));
            cmd_val = pmt::cdr(cmd_val);
          }
        }
        else {
          return false;
        }
      } catch (pmt::wrong_type w) {
        return false;
      }
      return true;
    }

    //! Helper function for msg_handler_command:
    // - Sets a value in vector_to_update to cmd_val, depending on chan
    // - If chan is a positive integer, it will set vector_to_update[chan]
    // - If chan is -1, it depends on minus_one_updates_all:
    //   - Either set vector_to_update[0] or
    //   - Set *all* entries in vector_to_update
    // - Returns a dynamic_bitset, all indexes that where changed in
    //   vector_to_update are set to 1
    template <typename T>
    static boost::dynamic_bitset<> _update_vector_from_cmd_val(
        std::vector<T> &vector_to_update,
        int chan,
        const T cmd_val,
        bool minus_one_updates_all = false
    ) {
      boost::dynamic_bitset<> vals_updated(vector_to_update.size());
      if (chan == -1) {
        if (minus_one_updates_all) {
          for (size_t i = 0; i < vector_to_update.size(); i++) {
            if (vector_to_update[i] != cmd_val) {
              vals_updated[i] = true;
              vector_to_update[i] = cmd_val;
            }
          }
          return vals_updated;
        }
        chan = 0;
      }
      if (vector_to_update[chan] != cmd_val) {
        vector_to_update[chan] = cmd_val;
        vals_updated[chan] = true;
      }

      return vals_updated;
    }


    /*! \brief Components common to USRP sink and source.
     *
     * \param device_addr Device address + options
     * \param stream_args Stream args (cpu format, otw format...)
     * \param ts_tag_name If this block produces or consumes stream tags, enter the corresponding tag name here
     */
    class usrp_common_impl
    {
    public:
      usrp_common_impl(
          const ::uhd::device_addr_t &device_addr,
          const ::uhd::stream_args_t &stream_args,
          const std::string &ts_tag_name
      ) :
        _stream_args(stream_args),
        _nchan(stream_args.channels.size()),
        _stream_now(_nchan == 1 and ts_tag_name.empty()),
        _start_time_set(false)
      {
        if(stream_args.cpu_format == "fc32")
          _type = boost::make_shared< ::uhd::io_type_t >(::uhd::io_type_t::COMPLEX_FLOAT32);
        if(stream_args.cpu_format == "sc16")
          _type = boost::make_shared< ::uhd::io_type_t >(::uhd::io_type_t::COMPLEX_INT16);
        _dev = ::uhd::usrp::multi_usrp::make(device_addr);
      };

      ~usrp_common_impl() {};

    protected:
      /*! \brief Wait until a timeout or a sensor returns 'locked'.
       *
       * If a given sensor is not found, this still returns 'true', so we don't throw
       * errors or warnings if a sensor wasn't implemented.
       */
      bool _wait_for_locked_sensor(
          std::vector<std::string> sensor_names,
          const std::string &sensor_name,
          get_sensor_fn_t get_sensor_fn
      ){
        if (std::find(sensor_names.begin(), sensor_names.end(), sensor_name) == sensor_names.end())
          return true;

        boost::system_time start = boost::get_system_time();
        boost::system_time first_lock_time;

        while (true) {
          if ((not first_lock_time.is_not_a_date_time()) and
              (boost::get_system_time() > (first_lock_time + boost::posix_time::seconds(LOCK_TIMEOUT)))) {
            break;
          }

          if (get_sensor_fn(sensor_name).to_bool()) {
            if (first_lock_time.is_not_a_date_time())
              first_lock_time = boost::get_system_time();
          }
          else {
            first_lock_time = boost::system_time(); //reset to 'not a date time'

            if (boost::get_system_time() > (start + boost::posix_time::seconds(LOCK_TIMEOUT))){
              return false;
            }
          }

          boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }

        return true;
      }

      void _update_stream_args(const ::uhd::stream_args_t &stream_args_)
      {
        ::uhd::stream_args_t stream_args(stream_args_);
        if (stream_args.channels.empty()) {
          stream_args.channels = _stream_args.channels;
        }
        if (stream_args.cpu_format != _stream_args.cpu_format ||
            stream_args.channels.size() != _stream_args.channels.size()) {
          throw std::runtime_error("Cannot change I/O signatures while updating stream args!");
        }
        _stream_args = stream_args;
      }

      //! Shared pointer to the underlying multi_usrp object
      ::uhd::usrp::multi_usrp::sptr _dev;
      ::uhd::stream_args_t _stream_args;
      boost::shared_ptr< ::uhd::io_type_t > _type;
      //! Number of channels (i.e. number of in- or outputs)
      size_t _nchan;
      bool _stream_now;
      ::uhd::time_spec_t _start_time;
      bool _start_time_set;
    };

  } /* namespace uhd */
} /* namespace gr */

#endif /* INCLUDED_GR_UHD_USRP_COMMON_H */

