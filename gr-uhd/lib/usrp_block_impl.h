/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_UHD_BLOCK_IMPL_H
#define INCLUDED_GR_UHD_BLOCK_IMPL_H

#include <gnuradio/uhd/usrp_block.h>
#include <pmt/pmt.h>
#include <uhd/usrp/multi_usrp.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/bind.hpp>


namespace gr {
  namespace uhd {

      static const size_t ALL_MBOARDS = ::uhd::usrp::multi_usrp::ALL_MBOARDS;
      static const size_t ALL_CHANS = ::uhd::usrp::multi_usrp::ALL_CHANS;
      static const std::string ALL_GAINS = ::uhd::usrp::multi_usrp::ALL_GAINS;

#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
      static const std::string ALL_LOS = ::uhd::usrp::multi_usrp::ALL_LOS;
#else
      static const std::string ALL_LOS;
#endif
    class usrp_block_impl : virtual public usrp_block
    {
     public:
      typedef boost::function< ::uhd::sensor_value_t (const std::string&)> get_sensor_fn_t;
      typedef boost::function<void(const pmt::pmt_t &, int, const pmt::pmt_t &)> cmd_handler_t;

      static const double LOCK_TIMEOUT;

      /**********************************************************************
       * Public API calls (see usrp_block.h for docs)
       **********************************************************************/
      // Getters
      ::uhd::sensor_value_t get_mboard_sensor(const std::string &name, size_t mboard);
      std::vector<std::string> get_mboard_sensor_names(size_t mboard);
      std::string get_time_source(const size_t mboard);
      std::vector<std::string> get_time_sources(const size_t mboard);
      std::string get_clock_source(const size_t mboard);
      std::vector<std::string> get_clock_sources(const size_t mboard);
      double get_clock_rate(size_t mboard);
      ::uhd::time_spec_t get_time_now(size_t mboard = 0);
      ::uhd::time_spec_t get_time_last_pps(size_t mboard);
      ::uhd::usrp::multi_usrp::sptr get_device(void);
      std::vector<std::string> get_gpio_banks(const size_t mboard);
      boost::uint32_t get_gpio_attr(
          const std::string &bank,
          const std::string &attr,
          const size_t mboard = 0
      );
      size_t get_num_mboards();

      // Setters
      void set_clock_config(const ::uhd::clock_config_t &clock_config, size_t mboard);
      void set_time_source(const std::string &source, const size_t mboard);
      void set_clock_source(const std::string &source, const size_t mboard);
      void set_clock_rate(double rate, size_t mboard);
      void set_time_now(const ::uhd::time_spec_t &time_spec, size_t mboard);
      void set_time_next_pps(const ::uhd::time_spec_t &time_spec);
      void set_time_unknown_pps(const ::uhd::time_spec_t &time_spec);
      void set_command_time(const ::uhd::time_spec_t &time_spec, size_t mboard);
      void set_user_register(const uint8_t addr, const uint32_t data, size_t mboard);
      void clear_command_time(size_t mboard);
      void set_gpio_attr(
          const std::string &bank,
          const std::string &attr,
          const boost::uint32_t value,
          const boost::uint32_t mask,
          const size_t mboard
      );

      // RPC
      void setup_rpc();

      /**********************************************************************
       * Structors
       * ********************************************************************/
      virtual ~usrp_block_impl();
     protected:
      /*! \brief Components common to USRP sink and source.
       *
       * \param device_addr Device address + options
       * \param stream_args Stream args (cpu format, otw format...)
       * \param ts_tag_name If this block produces or consumes stream tags, enter the corresponding tag name here
       */
      usrp_block_impl(
          const ::uhd::device_addr_t &device_addr,
          const ::uhd::stream_args_t &stream_args,
          const std::string &ts_tag_name
      );

      /**********************************************************************
       * Command Interface
       **********************************************************************/
      //! Receives commands and handles them
      void msg_handler_command(pmt::pmt_t msg);

      //! For a given argument, call the associated handler, or if none exists,
      // show a warning through the logging interface.
      void dispatch_msg_cmd_handler(const pmt::pmt_t &cmd, const pmt::pmt_t &val, int chan, pmt::pmt_t &msg);

      //! Register a new handler for command key \p cmd
      void register_msg_cmd_handler(const pmt::pmt_t &cmd, cmd_handler_t handler);


      // Default handlers
      void _cmd_handler_freq(const pmt::pmt_t &freq, int chan, const pmt::pmt_t &msg);
      void _cmd_handler_looffset(const pmt::pmt_t &lo_offset, int chan, const pmt::pmt_t &msg);
      void _cmd_handler_gain(const pmt::pmt_t &gain, int chan, const pmt::pmt_t &msg);
      void _cmd_handler_antenna(const pmt::pmt_t &ant, int chan, const pmt::pmt_t &msg);
      void _cmd_handler_rate(const pmt::pmt_t &rate, int chan, const pmt::pmt_t &msg);
      void _cmd_handler_tune(const pmt::pmt_t &tune, int chan, const pmt::pmt_t &msg);
      void _cmd_handler_bw(const pmt::pmt_t &bw, int chan, const pmt::pmt_t &msg);
      void _cmd_handler_lofreq(const pmt::pmt_t &lofreq, int chan, const pmt::pmt_t &msg);
      void _cmd_handler_dspfreq(const pmt::pmt_t &dspfreq, int chan, const pmt::pmt_t &msg);

      /**********************************************************************
       * Helpers
       **********************************************************************/
      bool _check_mboard_sensors_locked();

      void _update_stream_args(const ::uhd::stream_args_t &stream_args_);

      // should be const, doesn't work though 'cause missing operator=() for tune_request_t
      void _update_curr_tune_req(::uhd::tune_request_t &tune_req, int chan);

      /*! \brief Wait until a timeout or a sensor returns 'locked'.
       *
       * If a given sensor is not found, this still returns 'true', so we don't throw
       * errors or warnings if a sensor wasn't implemented.
       *
       * \returns true if the sensor locked in time or doesn't exist
       */
      bool _wait_for_locked_sensor(
          std::vector<std::string> sensor_names,
          const std::string &sensor_name,
          get_sensor_fn_t get_sensor_fn
      );

      //! Helper function for msg_handler_command:
      // - Extracts command and the command value from the command PMT
      // - Returns true if the command PMT is well formed
      // - If a channel is given, return that as well, otherwise set the channel to -1
      static bool _unpack_chan_command(
          std::string &command,
          pmt::pmt_t &cmd_val,
          int &chan,
          const pmt::pmt_t &cmd_pmt
      );

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

      //! Like set_center_freq(), but uses _curr_freq and _curr_lo_offset
      virtual ::uhd::tune_result_t _set_center_freq_from_internals(size_t chan, pmt::pmt_t direction) = 0;

      //! Calls _set_center_freq_from_internals() on all channels
      void _set_center_freq_from_internals_allchans(pmt::pmt_t direction);

      /**********************************************************************
       * Members
       *********************************************************************/
      //! Shared pointer to the underlying multi_usrp object
      ::uhd::usrp::multi_usrp::sptr _dev;
      ::uhd::stream_args_t _stream_args;
      boost::shared_ptr< ::uhd::io_type_t > _type;
      //! Number of channels (i.e. number of in- or outputs)
      size_t _nchan;
      bool _stream_now;
      ::uhd::time_spec_t _start_time;
      bool _start_time_set;

      /****** Command interface related **********/
      //! Stores a list of commands for later execution
      std::vector<pmt::pmt_t> _pending_cmds;
      //! Shadows the last value we told the USRP to tune to for every channel
      // (this is not necessarily the true value the USRP is currently tuned to!).
      std::vector< ::uhd::tune_request_t > _curr_tune_req;
      boost::dynamic_bitset<> _chans_to_tune;

      //! Stores the individual command handlers
      ::uhd::dict<pmt::pmt_t, cmd_handler_t> _msg_cmd_handlers;
    };

  } /* namespace uhd */
} /* namespace gr */

#endif /* INCLUDED_GR_UHD_BLOCK_IMPL_H */
