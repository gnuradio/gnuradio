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

const pmt::pmt_t gr::uhd::cmd_chan_key()
{
  static const pmt::pmt_t val = pmt::mp("chan");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_gain_key()
{
  static const pmt::pmt_t val = pmt::mp("gain");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_freq_key()
{
  static const pmt::pmt_t val = pmt::mp("freq");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_lo_offset_key()
{
  static const pmt::pmt_t val = pmt::mp("lo_offset");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_tune_key()
{
  static const pmt::pmt_t val = pmt::mp("tune");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_lo_freq_key()
{
  static const pmt::pmt_t val = pmt::mp("lo_freq");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_dsp_freq_key()
{
  static const pmt::pmt_t val = pmt::mp("dsp_freq");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_rate_key()
{
  static const pmt::pmt_t val = pmt::mp("rate");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_bandwidth_key()
{
  static const pmt::pmt_t val = pmt::mp("bandwidth");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_time_key()
{
  static const pmt::pmt_t val = pmt::mp("time");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_mboard_key()
{
  static const pmt::pmt_t val = pmt::mp("mboard");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_antenna_key()
{
  static const pmt::pmt_t val = pmt::mp("antenna");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_direction_key()
{
  static const pmt::pmt_t val = pmt::mp("direction");
  return val;
}
const pmt::pmt_t gr::uhd::cmd_tag_key()
{
  static const pmt::pmt_t val = pmt::mp("tag");
  return val;
}

const pmt::pmt_t gr::uhd::ant_direction_rx()
{
  static const pmt::pmt_t val = pmt::mp("RX");
  return val;
}
const pmt::pmt_t gr::uhd::ant_direction_tx()
{
  static const pmt::pmt_t val = pmt::mp("TX");
  return val;
}

usrp_block_impl::usrp_block_impl(
  const ::uhd::device_addr_t &device_addr,
  const ::uhd::stream_args_t &stream_args,
  const std::string &ts_tag_name
) : _stream_args(stream_args),
    _nchan(stream_args.channels.size()),
    _stream_now(_nchan == 1 and ts_tag_name.empty()),
    _start_time_set(false),
    _curr_tune_req(stream_args.channels.size(), ::uhd::tune_request_t()),
    _chans_to_tune(stream_args.channels.size())
{
  // TODO remove this when we update UHD
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

// cuz we lazy:
#define REGISTER_CMD_HANDLER(key, _handler) register_msg_cmd_handler(key, boost::bind(&usrp_block_impl::_handler, this, _1, _2, _3))
  // Register default command handlers:
  REGISTER_CMD_HANDLER(cmd_freq_key(), _cmd_handler_freq);
  REGISTER_CMD_HANDLER(cmd_gain_key(), _cmd_handler_gain);
  REGISTER_CMD_HANDLER(cmd_lo_offset_key(), _cmd_handler_looffset);
  REGISTER_CMD_HANDLER(cmd_tune_key(), _cmd_handler_tune);
  REGISTER_CMD_HANDLER(cmd_lo_freq_key(), _cmd_handler_lofreq);
  REGISTER_CMD_HANDLER(cmd_dsp_freq_key(), _cmd_handler_dspfreq);
  REGISTER_CMD_HANDLER(cmd_rate_key(), _cmd_handler_rate);
  REGISTER_CMD_HANDLER(cmd_bandwidth_key(), _cmd_handler_bw);
  REGISTER_CMD_HANDLER(cmd_antenna_key(), _cmd_handler_antenna);
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
        (boost::get_system_time() > (first_lock_time + boost::posix_time::seconds(static_cast<long>(LOCK_TIMEOUT))))) {
      break;
    }

    if (get_sensor_fn(sensor_name).to_bool()) {
      if (first_lock_time.is_not_a_date_time())
        first_lock_time = boost::get_system_time();
    }
    else {
      first_lock_time = boost::system_time(); //reset to 'not a date time'

      if (boost::get_system_time() > (start + boost::posix_time::seconds(static_cast<long>(LOCK_TIMEOUT)))){
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
usrp_block_impl::_set_center_freq_from_internals_allchans(pmt::pmt_t direction)
{
  while (_chans_to_tune.any()) {
    // This resets() bits, so this loop should not run indefinitely
    _set_center_freq_from_internals(_chans_to_tune.find_first(), direction);
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

std::vector<std::string>
usrp_block_impl::get_gpio_banks(const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_GPIO_API
  return _dev->get_gpio_banks(mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

boost::uint32_t
usrp_block_impl::get_gpio_attr(
    const std::string &bank,
    const std::string &attr,
    const size_t mboard
) {
#ifdef UHD_USRP_MULTI_USRP_GPIO_API
  return _dev->get_gpio_attr(bank, attr, mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
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

void
usrp_block_impl::set_gpio_attr(
    const std::string &bank,
    const std::string &attr,
    const boost::uint32_t value,
    const boost::uint32_t mask,
    const size_t mboard
) {
#ifdef UHD_USRP_MULTI_USRP_GPIO_API
  return _dev->set_gpio_attr(bank, attr, value, mask, mboard);
#else
  throw std::runtime_error("not implemented in this version");
#endif
}

::uhd::usrp::multi_usrp::sptr
usrp_block_impl::get_device(void)
{
  return _dev;
}

size_t
usrp_block_impl::get_num_mboards()
{
  return _dev->get_num_mboards();
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
  // Legacy code back compat: If we receive a tuple, we convert
  // it to a dict, and call the function again. Yep, this comes
  // at a slight performance hit. Sometime in the future, we can
  // hopefully remove this:
  if (pmt::is_tuple(msg)) {
    if (pmt::length(msg) != 2 && pmt::length(msg) != 3) {
      GR_LOG_ALERT(d_logger, boost::format("Error while unpacking command PMT: %s") % msg);
      return;
    }
    pmt::pmt_t new_msg = pmt::make_dict();
    new_msg = pmt::dict_add(new_msg, pmt::tuple_ref(msg, 0), pmt::tuple_ref(msg, 1));
    if (pmt::length(msg) == 3) {
      new_msg = pmt::dict_add(new_msg, cmd_chan_key(), pmt::tuple_ref(msg, 2));
    }
    GR_LOG_WARN(d_debug_logger, boost::format("Using legacy message format (tuples): %s") % msg);
    return msg_handler_command(new_msg);
  }
  // End of legacy backward compat code.

  // Turn pair into dict
  if (!pmt::is_dict(msg)) {
    GR_LOG_ERROR(d_logger, boost::format("Command message is neither dict nor pair: %s") % msg);
    return;
  }

  // OK, here comes the horrible part. Pairs pass is_dict(), but they're not dicts. Such dicks.
  try {
    // This will fail if msg is a pair:
    pmt::pmt_t keys = pmt::dict_keys(msg);
  } catch (const pmt::wrong_type &e) {
    // So we fix it:
    GR_LOG_DEBUG(d_debug_logger, boost::format("Converting pair to dict: %s") % msg);
    msg = pmt::dict_add(pmt::make_dict(), pmt::car(msg), pmt::cdr(msg));
  }

  /*** Start the actual message processing *************************/
  /// 1) Check if there's a time stamp
  if (pmt::dict_has_key(msg, cmd_time_key())) {
    size_t mboard_index = pmt::to_long(
        pmt::dict_ref(
          msg, cmd_mboard_key(),
          pmt::from_long( ::uhd::usrp::multi_usrp::ALL_MBOARDS ) // Default to all mboards
        )
    );
    pmt::pmt_t timespec_p = pmt::dict_ref(msg, cmd_time_key(), pmt::PMT_NIL);
    if (timespec_p == pmt::PMT_NIL) {
      clear_command_time(mboard_index);
    } else {
      ::uhd::time_spec_t timespec(
          time_t(pmt::to_uint64(pmt::car(timespec_p))), // Full secs
          pmt::to_double(pmt::cdr(timespec_p)) // Frac secs
      );
      GR_LOG_DEBUG(d_debug_logger, boost::format("Setting command time on mboard %d") % mboard_index);
      set_command_time(timespec, mboard_index);
    }
  }

  /// 2) Read chan value
  int chan = int(pmt::to_long(
      pmt::dict_ref(
        msg, cmd_chan_key(),
        pmt::from_long(-1) // Default to all chans
      )
  ));

  /// 3) Loop through all the values
  GR_LOG_DEBUG(d_debug_logger, boost::format("Processing command message %s") % msg);
  pmt::pmt_t msg_items = pmt::dict_items(msg);
  for (size_t i = 0; i < pmt::length(msg_items); i++) {
    try {
      dispatch_msg_cmd_handler(
          pmt::car(pmt::nth(i, msg_items)),
          pmt::cdr(pmt::nth(i, msg_items)),
          chan, msg
      );
    } catch (pmt::wrong_type &e) {
      GR_LOG_ALERT(d_logger, boost::format("Invalid command value for key %s: %s") % pmt::car(pmt::nth(i, msg_items)) % pmt::cdr(pmt::nth(i, msg_items)));
      break;
    }
  }

  /// 4) See if a direction was specified
  pmt::pmt_t direction = pmt::dict_ref(
      msg, cmd_direction_key(),
      pmt::PMT_NIL // Anything except "TX" or "RX will default to the messaged block direction"
  );

  /// 5) Check if we need to re-tune
  _set_center_freq_from_internals_allchans(direction);
}


void usrp_block_impl::dispatch_msg_cmd_handler(const pmt::pmt_t &cmd, const pmt::pmt_t &val, int chan, pmt::pmt_t &msg)
{
  if (_msg_cmd_handlers.has_key(cmd)) {
    _msg_cmd_handlers[cmd](val, chan, msg);
  }
}

void usrp_block_impl::register_msg_cmd_handler(const pmt::pmt_t &cmd, cmd_handler_t handler)
{
  _msg_cmd_handlers[cmd] = handler;
}

void usrp_block_impl::_update_curr_tune_req(::uhd::tune_request_t &tune_req, int chan)
{
  if (chan == -1) {
    for (size_t i = 0; i < _nchan; i++) {
      _update_curr_tune_req(tune_req, int(i));
    }
    return;
  }

  if (tune_req.target_freq != _curr_tune_req[chan].target_freq ||
      tune_req.rf_freq_policy != _curr_tune_req[chan].rf_freq_policy ||
      tune_req.rf_freq != _curr_tune_req[chan].rf_freq ||
      tune_req.dsp_freq != _curr_tune_req[chan].dsp_freq ||
      tune_req.dsp_freq_policy != _curr_tune_req[chan].dsp_freq_policy
      ) {
    _curr_tune_req[chan] = tune_req;
    _chans_to_tune.set(chan);
  }
}

// Default handlers:
void usrp_block_impl::_cmd_handler_freq(const pmt::pmt_t &freq_, int chan, const pmt::pmt_t &msg)
{
  double freq = pmt::to_double(freq_);
  ::uhd::tune_request_t new_tune_reqest(freq);
  if (pmt::dict_has_key(msg, cmd_lo_offset_key())) {
    double lo_offset = pmt::to_double(pmt::dict_ref(msg, cmd_lo_offset_key(), pmt::PMT_NIL));
    new_tune_reqest = ::uhd::tune_request_t(freq, lo_offset);
  }

  _update_curr_tune_req(new_tune_reqest, chan);
}

void usrp_block_impl::_cmd_handler_looffset(const pmt::pmt_t &lo_offset, int chan, const pmt::pmt_t &msg)
{
  if (pmt::dict_has_key(msg, cmd_freq_key())) {
    // Then it's already taken care of
    return;
  }

  double lo_offs = pmt::to_double(lo_offset);
  ::uhd::tune_request_t new_tune_request = _curr_tune_req[chan];
  new_tune_request.rf_freq = new_tune_request.target_freq + lo_offs;
  new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
  new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_AUTO;

  _update_curr_tune_req(new_tune_request, chan);
}

void usrp_block_impl::_cmd_handler_gain(const pmt::pmt_t &gain_, int chan, const pmt::pmt_t &msg)
{
  double gain = pmt::to_double(gain_);
  if (chan == -1) {
    for (size_t i = 0; i < _nchan; i++) {
      set_gain(gain, i);
    }
    return;
  }

  set_gain(gain, chan);
}

void usrp_block_impl::_cmd_handler_antenna(const pmt::pmt_t &ant, int chan, const pmt::pmt_t &msg)
{
  const std::string antenna(pmt::symbol_to_string(ant));
  if (chan == -1) {
    for (size_t i = 0; i < _nchan; i++) {
      set_antenna(antenna, i);
    }
    return;
  }

  set_antenna(antenna, chan);
}

void usrp_block_impl::_cmd_handler_rate(const pmt::pmt_t &rate_, int, const pmt::pmt_t &)
{
  const double rate = pmt::to_double(rate_);
  set_samp_rate(rate);
}

void usrp_block_impl::_cmd_handler_tune(const pmt::pmt_t &tune, int chan, const pmt::pmt_t &msg)
{
  double freq = pmt::to_double(pmt::car(tune));
  double lo_offset = pmt::to_double(pmt::cdr(tune));
  ::uhd::tune_request_t new_tune_reqest(freq, lo_offset);
  _update_curr_tune_req(new_tune_reqest, chan);
}

void usrp_block_impl::_cmd_handler_bw(const pmt::pmt_t &bw, int chan, const pmt::pmt_t &msg)
{
  double bandwidth = pmt::to_double(bw);
  if (chan == -1) {
    for (size_t i = 0; i < _nchan; i++) {
      set_bandwidth(bandwidth, i);
    }
    return;
  }

  set_bandwidth(bandwidth, chan);
}

void usrp_block_impl::_cmd_handler_lofreq(const pmt::pmt_t &lofreq, int chan, const pmt::pmt_t &msg)
{
  if (chan == -1) {
    for (size_t i = 0; i < _nchan; i++) {
      _cmd_handler_lofreq(lofreq, int(i), msg);
    }
    return;
  }

  ::uhd::tune_request_t new_tune_request = _curr_tune_req[chan];
  new_tune_request.rf_freq = pmt::to_double(lofreq);
  if (pmt::dict_has_key(msg, cmd_dsp_freq_key())) {
    new_tune_request.dsp_freq = pmt::to_double(pmt::dict_ref(msg, cmd_dsp_freq_key(), pmt::PMT_NIL));
  }
  new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
  new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;

  _update_curr_tune_req(new_tune_request, chan);
}

void usrp_block_impl::_cmd_handler_dspfreq(const pmt::pmt_t &dspfreq, int chan, const pmt::pmt_t &msg)
{
  if (pmt::dict_has_key(msg, cmd_lo_freq_key())) {
    // Then it's already dealt with
    return;
  }

  if (chan == -1) {
    for (size_t i = 0; i < _nchan; i++) {
      _cmd_handler_dspfreq(dspfreq, int(i), msg);
    }
    return;
  }

  ::uhd::tune_request_t new_tune_request = _curr_tune_req[chan];
  new_tune_request.dsp_freq = pmt::to_double(dspfreq);
  new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
  new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;

  _update_curr_tune_req(new_tune_request, chan);
}
