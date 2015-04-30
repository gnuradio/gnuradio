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

#include "usrp_block_impl.h"
#include <boost/make_shared.hpp>

using namespace gr::uhd;

const double usrp_block_impl::LOCK_TIMEOUT = 1.5;

/**********************************************************************
 * Structors
 *********************************************************************/
usrp_block::usrp_block(
    const std::string &name,
    gr::io_signature::sptr input_signature,
    gr::io_signature::sptr output_signature
) : sync_block(name, input_signature, output_signature)
{
  // nop
}

usrp_block_impl::usrp_block_impl(
  const ::uhd::device_addr_t &device_addr,
  const ::uhd::stream_args_t &stream_args,
  const std::string &ts_tag_name
) : _stream_args(stream_args),
    _nchan(stream_args.channels.size()),
    _stream_now(_nchan == 1 and ts_tag_name.empty()),
    _start_time_set(false),
    _curr_freq(stream_args.channels.size(), 0.0),
    _curr_lo_offset(stream_args.channels.size(), 0.0),
    _curr_gain(stream_args.channels.size(), 0.0),
    _chans_to_tune(stream_args.channels.size())
{
  if(stream_args.cpu_format == "fc32")
    _type = boost::make_shared< ::uhd::io_type_t >(::uhd::io_type_t::COMPLEX_FLOAT32);
  if(stream_args.cpu_format == "sc16")
    _type = boost::make_shared< ::uhd::io_type_t >(::uhd::io_type_t::COMPLEX_INT16);
  _dev = ::uhd::usrp::multi_usrp::make(device_addr);

  _check_mboard_sensors_locked();

  // Set up message ports:
  message_port_register_in(pmt::mp("command"));
  set_msg_handler(
      pmt::mp("command"),
      boost::bind(&usrp_block_impl::msg_handler_command, this, _1)
  );
}

usrp_block_impl::~usrp_block_impl()
{
  // nop
}

/**********************************************************************
 * Helpers
 *********************************************************************/
void usrp_block_impl::_update_stream_args(const ::uhd::stream_args_t &stream_args_)
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

bool usrp_block_impl::_wait_for_locked_sensor(
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

bool usrp_block_impl::_unpack_chan_command(
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

bool usrp_block_impl::_check_mboard_sensors_locked()
{
  bool clocks_locked = true;

  // Check ref lock for all mboards
  for (size_t mboard_index = 0; mboard_index < _dev->get_num_mboards(); mboard_index++) {
    std::string sensor_name = "ref_locked";
    if (_dev->get_clock_source(mboard_index) == "internal") {
      continue;
    }
    else if (_dev->get_clock_source(mboard_index) == "mimo") {
      sensor_name = "mimo_locked";
    }
    if (not _wait_for_locked_sensor(
            get_mboard_sensor_names(mboard_index),
            sensor_name,
            boost::bind(&usrp_block_impl::get_mboard_sensor, this, _1, mboard_index)
        )) {
      GR_LOG_WARN(d_logger, boost::format("Sensor '%s' failed to lock within timeout on motherboard %d.") % sensor_name % mboard_index);
      clocks_locked = false;
    }
  }

  return clocks_locked;
}

void
usrp_block_impl::_set_center_freq_from_internals_allchans()
{
  for (size_t chan = 0; chan < _nchan; chan++) {
    if (_chans_to_tune[chan]) {
      _set_center_freq_from_internals(chan);
    }
  }
}


/**********************************************************************
 * Public API calls
 *********************************************************************/
::uhd::sensor_value_t
usrp_block_impl::get_mboard_sensor(const std::string &name,
                                   size_t mboard)
{
  return _dev->get_mboard_sensor(name, mboard);
}

std::vector<std::string>
usrp_block_impl::get_mboard_sensor_names(size_t mboard)
{
  return _dev->get_mboard_sensor_names(mboard);
}

void
usrp_block_impl::set_clock_config(const ::uhd::clock_config_t &clock_config,
                                 size_t mboard)
{
  return _dev->set_clock_config(clock_config, mboard);
}

void
usrp_block_impl::set_time_source(const std::string &source,
                                const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
  return _dev->set_time_source(source, mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

std::string
usrp_block_impl::get_time_source(const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
  return _dev->get_time_source(mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

std::vector<std::string>
usrp_block_impl::get_time_sources(const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
  return _dev->get_time_sources(mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

void
usrp_block_impl::set_clock_source(const std::string &source,
                                 const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
  return _dev->set_clock_source(source, mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

std::string
usrp_block_impl::get_clock_source(const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
  return _dev->get_clock_source(mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

std::vector<std::string>
usrp_block_impl::get_clock_sources(const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
  return _dev->get_clock_sources(mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

double
usrp_block_impl::get_clock_rate(size_t mboard)
{
  return _dev->get_master_clock_rate(mboard);
}

void
usrp_block_impl::set_clock_rate(double rate, size_t mboard)
{
  return _dev->set_master_clock_rate(rate, mboard);
}

::uhd::time_spec_t
usrp_block_impl::get_time_now(size_t mboard)
{
  return _dev->get_time_now(mboard);
}

::uhd::time_spec_t
usrp_block_impl::get_time_last_pps(size_t mboard)
{
  return _dev->get_time_last_pps(mboard);
}

void
usrp_block_impl::set_time_now(const ::uhd::time_spec_t &time_spec,
                             size_t mboard)
{
  return _dev->set_time_now(time_spec, mboard);
}

void
usrp_block_impl::set_time_next_pps(const ::uhd::time_spec_t &time_spec)
{
  return _dev->set_time_next_pps(time_spec);
}

void
usrp_block_impl::set_time_unknown_pps(const ::uhd::time_spec_t &time_spec)
{
  return _dev->set_time_unknown_pps(time_spec);
}

void
usrp_block_impl::set_command_time(const ::uhd::time_spec_t &time_spec,
                                 size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_COMMAND_TIME_API
  return _dev->set_command_time(time_spec, mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

void
usrp_block_impl::clear_command_time(size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_COMMAND_TIME_API
  return _dev->clear_command_time(mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

void
usrp_block_impl::set_user_register(const uint8_t addr,
                                    const uint32_t data,
                                    size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_USER_REGS_API
  _dev->set_user_register(addr, data, mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

::uhd::usrp::multi_usrp::sptr
usrp_block_impl::get_device(void)
{
  return _dev;
}

/**********************************************************************
 * External Interfaces
 *********************************************************************/
void
usrp_block_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
  add_rpc_variable(
    rpcbasic_sptr(new rpcbasic_register_get<usrp_block, double>(
      alias(), "samp_rate",
      &usrp_block::get_samp_rate,
      pmt::mp(100000.0f), pmt::mp(25000000.0f), pmt::mp(1000000.0f),
      "sps", "Sample Rate", RPC_PRIVLVL_MIN,
      DISPTIME | DISPOPTSTRIP))
  );

  add_rpc_variable(
    rpcbasic_sptr(new rpcbasic_register_set<usrp_block, double>(
      alias(), "samp_rate",
      &usrp_block::set_samp_rate,
      pmt::mp(100000.0f), pmt::mp(25000000.0f), pmt::mp(1000000.0f),
      "sps", "Sample Rate",
      RPC_PRIVLVL_MIN, DISPNULL))
  );
#endif /* GR_CTRLPORT */
}

void usrp_block_impl::msg_handler_command(pmt::pmt_t msg)
{
  std::string command;
  pmt::pmt_t cmd_value;
  int chan = -1;
  if (not _unpack_chan_command(command, cmd_value, chan, msg)) {
    GR_LOG_ALERT(d_logger, boost::format("Error while unpacking command PMT: %s") % msg);
    return;
  }
  GR_LOG_DEBUG(d_debug_logger, boost::format("Received command: %s") % command);
  try {
    if (command == "freq") {
      _chans_to_tune = _update_vector_from_cmd_val<double>(
          _curr_freq, chan, pmt::to_double(cmd_value), true
      );
      _set_center_freq_from_internals_allchans();
    } else if (command == "lo_offset") {
      _chans_to_tune = _update_vector_from_cmd_val<double>(
          _curr_lo_offset, chan, pmt::to_double(cmd_value), true
      );
      _set_center_freq_from_internals_allchans();
    } else if (command == "gain") {
      boost::dynamic_bitset<> chans_to_change = _update_vector_from_cmd_val<double>(
          _curr_gain, chan, pmt::to_double(cmd_value), true
      );
      if (chans_to_change.any()) {
        for (size_t i = 0; i < chans_to_change.size(); i++) {
          if (chans_to_change[i]) {
            set_gain(_curr_gain[i], i);
          }
        }
      }
    } else {
      GR_LOG_ALERT(d_logger, boost::format("Received unknown command: %s") % command);
    }
  } catch (pmt::wrong_type &e) {
    GR_LOG_ALERT(d_logger, boost::format("Received command '%s' with invalid command value: %s") % command % cmd_value);
  }
}
