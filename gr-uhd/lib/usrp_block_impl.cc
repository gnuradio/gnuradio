/* -*- c++ -*- */
/*
 * Copyright 2015-2016,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "usrp_block_impl.h"
#include <chrono>
#include <thread>

using namespace gr::uhd;
using namespace std::chrono_literals;

namespace {
constexpr auto LOCK_TIMEOUT = 1.5s;
}

/**********************************************************************
 * Structors
 *********************************************************************/
usrp_block::usrp_block(const std::string& name,
                       gr::io_signature::sptr input_signature,
                       gr::io_signature::sptr output_signature)
    : sync_block(name, input_signature, output_signature)
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
const pmt::pmt_t gr::uhd::cmd_power_key()
{
    static const pmt::pmt_t val = pmt::mp("power_dbm");
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
const pmt::pmt_t gr::uhd::cmd_mtune_key()
{
    static const pmt::pmt_t val = pmt::mp("mtune");
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
const pmt::pmt_t gr::uhd::cmd_pc_clock_resync_key()
{
    static const pmt::pmt_t val = pmt::mp("pc_clock_resync");
    return val;
}

const pmt::pmt_t gr::uhd::cmd_gpio_key()
{
    static const pmt::pmt_t val = pmt::mp("gpio");
    return val;
}

const pmt::pmt_t gr::uhd::direction_rx()
{
    static const pmt::pmt_t val = pmt::mp("RX");
    return val;
}
const pmt::pmt_t gr::uhd::direction_tx()
{
    static const pmt::pmt_t val = pmt::mp("TX");
    return val;
}

usrp_block_impl::usrp_block_impl(const ::uhd::device_addr_t& device_addr,
                                 const ::uhd::stream_args_t& stream_args,
                                 const std::string& ts_tag_name)
    : _stream_args(stream_args),
      _nchan(stream_args.channels.size()),
      _stream_now(_nchan == 1 and ts_tag_name.empty()),
      _start_time_set(false),
      _force_tune(false),
      _curr_tx_tune_req(stream_args.channels.size(), ::uhd::tune_request_t()),
      _curr_rx_tune_req(stream_args.channels.size(), ::uhd::tune_request_t()),
      _tx_chans_to_tune(stream_args.channels.size()),
      _rx_chans_to_tune(stream_args.channels.size())
{
    _dev = ::uhd::usrp::multi_usrp::make(device_addr);

    _check_mboard_sensors_locked();

    // Set up message ports:
    message_port_register_in(pmt::mp("command"));
    set_msg_handler(pmt::mp("command"),
                    [this](pmt::pmt_t msg) { this->msg_handler_command(msg); });

// cuz we lazy:
#define REGISTER_CMD_HANDLER(key, _handler)                                   \
    register_msg_cmd_handler(                                                 \
        key, [this](const pmt::pmt_t& var, int chan, const pmt::pmt_t& msg) { \
            this->_handler(var, chan, msg);                                   \
        })
    // Register default command handlers:
    REGISTER_CMD_HANDLER(cmd_freq_key(), _cmd_handler_freq);
    REGISTER_CMD_HANDLER(cmd_gain_key(), _cmd_handler_gain);
    REGISTER_CMD_HANDLER(cmd_power_key(), _cmd_handler_power);
    REGISTER_CMD_HANDLER(cmd_lo_offset_key(), _cmd_handler_looffset);
    REGISTER_CMD_HANDLER(cmd_tune_key(), _cmd_handler_tune);
    REGISTER_CMD_HANDLER(cmd_mtune_key(), _cmd_handler_mtune);
    REGISTER_CMD_HANDLER(cmd_lo_freq_key(), _cmd_handler_lofreq);
    REGISTER_CMD_HANDLER(cmd_dsp_freq_key(), _cmd_handler_dspfreq);
    REGISTER_CMD_HANDLER(cmd_rate_key(), _cmd_handler_rate);
    REGISTER_CMD_HANDLER(cmd_bandwidth_key(), _cmd_handler_bw);
    REGISTER_CMD_HANDLER(cmd_antenna_key(), _cmd_handler_antenna);
    REGISTER_CMD_HANDLER(cmd_gpio_key(), _cmd_handler_gpio);
    REGISTER_CMD_HANDLER(cmd_pc_clock_resync_key(), _cmd_handler_pc_clock_resync);
}

usrp_block_impl::~usrp_block_impl()
{
    // nop
}

/**********************************************************************
 * Helpers
 *********************************************************************/
void usrp_block_impl::_update_stream_args(const ::uhd::stream_args_t& stream_args_)
{
    ::uhd::stream_args_t stream_args(stream_args_);
    if (stream_args.channels.empty()) {
        stream_args.channels = _stream_args.channels;
    }
    if (stream_args.cpu_format != _stream_args.cpu_format ||
        stream_args.channels.size() != _stream_args.channels.size()) {
        throw std::runtime_error(
            "Cannot change I/O signatures while updating stream args!");
    }
    _stream_args = stream_args;
}

bool usrp_block_impl::_wait_for_locked_sensor(std::vector<std::string> sensor_names,
                                              const std::string& sensor_name,
                                              get_sensor_fn_t get_sensor_fn)
{
    if (std::find(sensor_names.begin(), sensor_names.end(), sensor_name) ==
        sensor_names.end()) {
        return true;
    }

    const auto start = std::chrono::steady_clock::now();
    const auto timeout = start + LOCK_TIMEOUT;

    while (std::chrono::steady_clock::now() < timeout) {
        if (get_sensor_fn(sensor_name).to_bool()) {
            return true;
        }

        std::this_thread::sleep_for(100ms);
    }

    // One last try:
    return get_sensor_fn(sensor_name).to_bool();
}

bool usrp_block_impl::_unpack_chan_command(std::string& command,
                                           pmt::pmt_t& cmd_val,
                                           int& chan,
                                           const pmt::pmt_t& cmd_pmt)
{
    try {
        chan = -1; // Default value
        if (pmt::is_tuple(cmd_pmt) and
            (pmt::length(cmd_pmt) == 2 or pmt::length(cmd_pmt) == 3)) {
            command = pmt::symbol_to_string(pmt::tuple_ref(cmd_pmt, 0));
            cmd_val = pmt::tuple_ref(cmd_pmt, 1);
            if (pmt::length(cmd_pmt) == 3) {
                chan = pmt::to_long(pmt::tuple_ref(cmd_pmt, 2));
            }
        } else if (pmt::is_pair(cmd_pmt)) {
            command = pmt::symbol_to_string(pmt::car(cmd_pmt));
            cmd_val = pmt::cdr(cmd_pmt);
            if (pmt::is_pair(cmd_val)) {
                chan = pmt::to_long(pmt::car(cmd_val));
                cmd_val = pmt::cdr(cmd_val);
            }
        } else {
            return false;
        }
    } catch (pmt::wrong_type& w) {
        return false;
    }
    return true;
}

bool usrp_block_impl::_check_mboard_sensors_locked()
{
    bool clocks_locked = true;

    // Check ref lock for all mboards
    for (size_t mboard_index = 0; mboard_index < _dev->get_num_mboards();
         mboard_index++) {
        std::string sensor_name = "ref_locked";
        if (_dev->get_clock_source(mboard_index) == "internal") {
            continue;
        } else if (_dev->get_clock_source(mboard_index) == "mimo") {
            sensor_name = "mimo_locked";
        }
        if (not _wait_for_locked_sensor(get_mboard_sensor_names(mboard_index),
                                        sensor_name,
                                        [this, mboard_index](const std::string& name) {
                                            return static_cast<::uhd::sensor_value_t>(
                                                this->get_mboard_sensor(name,
                                                                        mboard_index));
                                        })) {
            GR_LOG_WARN(
                d_logger,
                boost::format(
                    "Sensor '%s' failed to lock within timeout on motherboard %d.") %
                    sensor_name % mboard_index);
            clocks_locked = false;
        }
    }

    return clocks_locked;
}

void usrp_block_impl::_set_center_freq_from_internals_allchans()
{
    unsigned int chan;
    while (_rx_chans_to_tune.any()) {
        // This resets() bits, so this loop should not run indefinitely
        chan = _rx_chans_to_tune.find_first();
        _set_center_freq_from_internals(chan, direction_rx());
        _rx_chans_to_tune.reset(chan);
    }

    while (_tx_chans_to_tune.any()) {
        // This resets() bits, so this loop should not run indefinitely
        chan = _tx_chans_to_tune.find_first();
        _set_center_freq_from_internals(chan, direction_tx());
        _tx_chans_to_tune.reset(chan);
    }
}


/**********************************************************************
 * Public API calls
 *********************************************************************/
::uhd::sensor_value_t usrp_block_impl::get_mboard_sensor(const std::string& name,
                                                         size_t mboard)
{
    return _dev->get_mboard_sensor(name, mboard);
}

std::vector<std::string> usrp_block_impl::get_mboard_sensor_names(size_t mboard)
{
    return _dev->get_mboard_sensor_names(mboard);
}

void usrp_block_impl::set_time_source(const std::string& source, const size_t mboard)
{
    return _dev->set_time_source(source, mboard);
}

std::string usrp_block_impl::get_time_source(const size_t mboard)
{
    return _dev->get_time_source(mboard);
}

std::vector<std::string> usrp_block_impl::get_time_sources(const size_t mboard)
{
    return _dev->get_time_sources(mboard);
}

void usrp_block_impl::set_clock_source(const std::string& source, const size_t mboard)
{
    return _dev->set_clock_source(source, mboard);
}

std::string usrp_block_impl::get_clock_source(const size_t mboard)
{
    return _dev->get_clock_source(mboard);
}

std::vector<std::string> usrp_block_impl::get_clock_sources(const size_t mboard)
{
    return _dev->get_clock_sources(mboard);
}

double usrp_block_impl::get_clock_rate(size_t mboard)
{
    return _dev->get_master_clock_rate(mboard);
}

void usrp_block_impl::set_clock_rate(double rate, size_t mboard)
{
    return _dev->set_master_clock_rate(rate, mboard);
}

::uhd::time_spec_t usrp_block_impl::get_time_now(size_t mboard)
{
    return _dev->get_time_now(mboard);
}

::uhd::time_spec_t usrp_block_impl::get_time_last_pps(size_t mboard)
{
    return _dev->get_time_last_pps(mboard);
}

std::vector<std::string> usrp_block_impl::get_gpio_banks(const size_t mboard)
{
    return _dev->get_gpio_banks(mboard);
}

uint32_t usrp_block_impl::get_gpio_attr(const std::string& bank,
                                        const std::string& attr,
                                        const size_t mboard)
{
    throw std::runtime_error("not implemented in this version");
}

void usrp_block_impl::set_time_now(const ::uhd::time_spec_t& time_spec, size_t mboard)
{
    return _dev->set_time_now(time_spec, mboard);
}

void usrp_block_impl::set_time_next_pps(const ::uhd::time_spec_t& time_spec)
{
    return _dev->set_time_next_pps(time_spec);
}

void usrp_block_impl::set_time_unknown_pps(const ::uhd::time_spec_t& time_spec)
{
    return _dev->set_time_unknown_pps(time_spec);
}

void usrp_block_impl::set_command_time(const ::uhd::time_spec_t& time_spec, size_t mboard)
{
    return _dev->set_command_time(time_spec, mboard);
}

void usrp_block_impl::clear_command_time(size_t mboard)
{
    return _dev->clear_command_time(mboard);
}

void usrp_block_impl::set_user_register(const uint8_t addr,
                                        const uint32_t data,
                                        size_t mboard)
{
    _dev->set_user_register(addr, data, mboard);
}

void usrp_block_impl::set_gpio_attr(const std::string& bank,
                                    const std::string& attr,
                                    const uint32_t value,
                                    const uint32_t mask,
                                    const size_t mboard)
{
    return _dev->set_gpio_attr(bank, attr, value, mask, mboard);
}

::uhd::usrp::multi_usrp::sptr usrp_block_impl::get_device(void) { return _dev; }

size_t usrp_block_impl::get_num_mboards() { return _dev->get_num_mboards(); }

/**********************************************************************
 * External Interfaces
 *********************************************************************/
void usrp_block_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<usrp_block, double>(alias(),
                                                      "samp_rate",
                                                      &usrp_block::get_samp_rate,
                                                      pmt::mp(100000.0f),
                                                      pmt::mp(25000000.0f),
                                                      pmt::mp(1000000.0f),
                                                      "sps",
                                                      "Sample Rate",
                                                      RPC_PRIVLVL_MIN,
                                                      DISPTIME | DISPOPTSTRIP)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_set<usrp_block, double>(alias(),
                                                      "samp_rate",
                                                      &usrp_block::set_samp_rate,
                                                      pmt::mp(100000.0f),
                                                      pmt::mp(25000000.0f),
                                                      pmt::mp(1000000.0f),
                                                      "sps",
                                                      "Sample Rate",
                                                      RPC_PRIVLVL_MIN,
                                                      DISPNULL)));
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
            GR_LOG_ALERT(d_logger,
                         boost::format("Error while unpacking command PMT: %s") % msg);
            return;
        }
        pmt::pmt_t new_msg = pmt::make_dict();
        new_msg = pmt::dict_add(new_msg, pmt::tuple_ref(msg, 0), pmt::tuple_ref(msg, 1));
        if (pmt::length(msg) == 3) {
            new_msg = pmt::dict_add(new_msg, cmd_chan_key(), pmt::tuple_ref(msg, 2));
        }
        GR_LOG_WARN(d_debug_logger,
                    boost::format("Using legacy message format (tuples): %s") % msg);
        return msg_handler_command(new_msg);
    }
    // End of legacy backward compat code.

    // pmt_dict is a subclass of pmt_pair. Make sure we use pmt_pair!
    // Old behavior was that these checks were interchangeable. Be aware of this change!
    if (!(pmt::is_dict(msg)) && pmt::is_pair(msg)) {
        GR_LOG_DEBUG(
            d_logger,
            boost::format(
                "Command message is pair, converting to dict: '%s': car(%s), cdr(%s)") %
                msg % pmt::car(msg) % pmt::cdr(msg));
        msg = pmt::dict_add(pmt::make_dict(), pmt::car(msg), pmt::cdr(msg));
    }

    // Make sure, we use dicts!
    if (!pmt::is_dict(msg)) {
        GR_LOG_ERROR(d_logger,
                     boost::format("Command message is neither dict nor pair: %s") % msg);
        return;
    }

    /*** Start the actual message processing *************************/
    /// 1) Check if there's a time stamp
    if (pmt::dict_has_key(msg, cmd_time_key())) {
        size_t mboard_index = pmt::to_long(pmt::dict_ref(
            msg,
            cmd_mboard_key(),
            pmt::from_long(::uhd::usrp::multi_usrp::ALL_MBOARDS) // Default to all mboards
            ));
        pmt::pmt_t timespec_p = pmt::dict_ref(msg, cmd_time_key(), pmt::PMT_NIL);
        if (timespec_p == pmt::PMT_NIL) {
            clear_command_time(mboard_index);
        } else {
            ::uhd::time_spec_t timespec(
                time_t(pmt::to_uint64(pmt::car(timespec_p))), // Full secs
                pmt::to_double(pmt::cdr(timespec_p))          // Frac secs
            );
            GR_LOG_DEBUG(d_debug_logger,
                         boost::format("Setting command time on mboard %d") %
                             mboard_index);
            set_command_time(timespec, mboard_index);
        }
    }

    /// 2) Read chan value
    int chan = int(pmt::to_long(pmt::dict_ref(msg,
                                              cmd_chan_key(),
                                              pmt::from_long(-1) // Default to all chans
                                              )));

    /// 3) If a direction key was specified, force the block to tune - see issue #1814
    _force_tune = pmt::dict_has_key(msg, cmd_direction_key());

    /// 4) Loop through all the values
    GR_LOG_DEBUG(d_debug_logger, boost::format("Processing command message %s") % msg);
    pmt::pmt_t msg_items = pmt::dict_items(msg);
    for (size_t i = 0; i < pmt::length(msg_items); i++) {
        try {
            dispatch_msg_cmd_handler(pmt::car(pmt::nth(i, msg_items)),
                                     pmt::cdr(pmt::nth(i, msg_items)),
                                     chan,
                                     msg);
        } catch (pmt::wrong_type& e) {
            GR_LOG_ALERT(d_logger,
                         boost::format("Invalid command value for key %s: %s") %
                             pmt::car(pmt::nth(i, msg_items)) %
                             pmt::cdr(pmt::nth(i, msg_items)));
            break;
        }
    }

    /// 5) Check if we need to re-tune
    _set_center_freq_from_internals_allchans();
    _force_tune = false;
}


void usrp_block_impl::dispatch_msg_cmd_handler(const pmt::pmt_t& cmd,
                                               const pmt::pmt_t& val,
                                               int chan,
                                               pmt::pmt_t& msg)
{
    if (_msg_cmd_handlers.has_key(cmd)) {
        _msg_cmd_handlers[cmd](val, chan, msg);
    }
}

void usrp_block_impl::register_msg_cmd_handler(const pmt::pmt_t& cmd,
                                               cmd_handler_t handler)
{
    _msg_cmd_handlers[cmd] = handler;
}

void usrp_block_impl::_update_curr_tune_req(::uhd::tune_request_t& tune_req,
                                            int chan,
                                            pmt::pmt_t direction)
{
    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            _update_curr_tune_req(tune_req, int(i), direction);
        }
        return;
    }

    if (pmt::eqv(direction, direction_rx())) {
        if (tune_req.target_freq != _curr_rx_tune_req[chan].target_freq ||
            tune_req.rf_freq_policy != _curr_rx_tune_req[chan].rf_freq_policy ||
            tune_req.rf_freq != _curr_rx_tune_req[chan].rf_freq ||
            tune_req.dsp_freq != _curr_rx_tune_req[chan].dsp_freq ||
            tune_req.dsp_freq_policy != _curr_rx_tune_req[chan].dsp_freq_policy ||
            _force_tune) {
            _curr_rx_tune_req[chan] = tune_req;
            _rx_chans_to_tune.set(chan);
        }
    } else {
        if (tune_req.target_freq != _curr_tx_tune_req[chan].target_freq ||
            tune_req.rf_freq_policy != _curr_tx_tune_req[chan].rf_freq_policy ||
            tune_req.rf_freq != _curr_tx_tune_req[chan].rf_freq ||
            tune_req.dsp_freq != _curr_tx_tune_req[chan].dsp_freq ||
            tune_req.dsp_freq_policy != _curr_tx_tune_req[chan].dsp_freq_policy ||
            _force_tune) {
            _curr_tx_tune_req[chan] = tune_req;
            _tx_chans_to_tune.set(chan);
        }
    }
}

// Default handlers:
void usrp_block_impl::_cmd_handler_freq(const pmt::pmt_t& freq_,
                                        int chan,
                                        const pmt::pmt_t& msg)
{
    // Get the direction key
    const pmt::pmt_t direction = get_cmd_or_default_direction(msg);

    double freq = pmt::to_double(freq_);
    ::uhd::tune_request_t new_tune_request(freq);
    if (pmt::dict_has_key(msg, cmd_lo_offset_key())) {
        double lo_offset =
            pmt::to_double(pmt::dict_ref(msg, cmd_lo_offset_key(), pmt::PMT_NIL));
        new_tune_request = ::uhd::tune_request_t(freq, lo_offset);
    }

    _update_curr_tune_req(new_tune_request, chan, direction);
}

void usrp_block_impl::_cmd_handler_looffset(const pmt::pmt_t& lo_offset,
                                            int chan,
                                            const pmt::pmt_t& msg)
{
    // Get the direction key
    const pmt::pmt_t direction = get_cmd_or_default_direction(msg);

    if (pmt::dict_has_key(msg, cmd_freq_key())) {
        // Then it's already taken care of
        return;
    }

    double lo_offs = pmt::to_double(lo_offset);
    ::uhd::tune_request_t new_tune_request;
    if (pmt::eqv(direction, direction_rx())) {
        new_tune_request = _curr_rx_tune_req[chan];
    } else {
        new_tune_request = _curr_tx_tune_req[chan];
    }

    new_tune_request.rf_freq = new_tune_request.target_freq + lo_offs;
    new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
    new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_AUTO;

    _update_curr_tune_req(new_tune_request, chan, direction);
}

void usrp_block_impl::_cmd_handler_gain(const pmt::pmt_t& gain_,
                                        int chan,
                                        const pmt::pmt_t& msg)
{
    // Get the direction key
    const pmt::pmt_t direction = get_cmd_or_default_direction(msg);

    double gain = pmt::to_double(gain_);
    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            set_gain(gain, i, direction);
        }
        return;
    }

    set_gain(gain, chan, direction);
}

void usrp_block_impl::_cmd_handler_power(const pmt::pmt_t& power_dbm_,
                                         int chan,
                                         const pmt::pmt_t& msg)
{
    double power_dbm = pmt::to_double(power_dbm_);
    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            set_power_reference(power_dbm, i);
        }
        return;
    }

    set_power_reference(power_dbm, chan);
}

void usrp_block_impl::_cmd_handler_antenna(const pmt::pmt_t& ant,
                                           int chan,
                                           const pmt::pmt_t& msg)
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

void usrp_block_impl::_cmd_handler_gpio(const pmt::pmt_t& gpio_attr,
                                        int chan,
                                        const pmt::pmt_t& msg)
{
    size_t mboard = pmt::to_long(pmt::dict_ref(
        msg,
        cmd_mboard_key(),
        // pmt::from_long(::uhd::usrp::multi_usrp::ALL_MBOARDS) // Default to all mboards
        pmt::from_long(0) // default to first mboard
        ));

    if (!pmt::is_dict(gpio_attr)) {
        GR_LOG_ERROR(d_logger,
                     boost::format("gpio_attr in  message is neither dict nor pair: %s") %
                         gpio_attr);
        return;
    }
    if (!pmt::dict_has_key(gpio_attr, pmt::mp("bank")) ||
        !pmt::dict_has_key(gpio_attr, pmt::mp("attr")) ||
        !pmt::dict_has_key(gpio_attr, pmt::mp("value")) ||
        !pmt::dict_has_key(gpio_attr, pmt::mp("mask"))) {
        GR_LOG_ERROR(
            d_logger,
            boost::format("gpio_attr message must include bank, attr, value and mask"));
        return;
    }
    std::string bank =
        pmt::symbol_to_string(pmt::dict_ref(gpio_attr, pmt::mp("bank"), pmt::mp("")));
    std::string attr =
        pmt::symbol_to_string(pmt::dict_ref(gpio_attr, pmt::mp("attr"), pmt::mp("")));
    uint32_t value = pmt::to_double(pmt::dict_ref(gpio_attr, pmt::mp("value"), 0));
    uint32_t mask = pmt::to_double(pmt::dict_ref(gpio_attr, pmt::mp("mask"), 0));

    set_gpio_attr(bank, attr, value, mask, mboard);
}

void usrp_block_impl::_cmd_handler_rate(const pmt::pmt_t& rate_, int, const pmt::pmt_t&)
{
    const double rate = pmt::to_double(rate_);
    set_samp_rate(rate);
}

void usrp_block_impl::_cmd_handler_pc_clock_resync(const pmt::pmt_t&,
                                                   int,
                                                   const pmt::pmt_t&)
{
    const uint64_t ticks =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    const ::uhd::time_spec_t& time_spec = ::uhd::time_spec_t::from_ticks(ticks, 1.0e9);
    set_time_now(time_spec, ::uhd::usrp::multi_usrp::ALL_MBOARDS);
}

void usrp_block_impl::_cmd_handler_tune(const pmt::pmt_t& tune,
                                        int chan,
                                        const pmt::pmt_t& msg)
{
    // Get the direction key
    const pmt::pmt_t direction = get_cmd_or_default_direction(msg);

    double freq = pmt::to_double(pmt::car(tune));
    double lo_offset = pmt::to_double(pmt::cdr(tune));
    ::uhd::tune_request_t new_tune_request(freq, lo_offset);
    _update_curr_tune_req(new_tune_request, chan, direction);
}

void usrp_block_impl::_cmd_handler_mtune(const pmt::pmt_t& tune,
                                         int chan,
                                         const pmt::pmt_t& msg)
{
    // Get the direction key
    const pmt::pmt_t direction = get_cmd_or_default_direction(msg);

    ::uhd::tune_request_t new_tune_request;
    if (pmt::dict_has_key(tune, pmt::mp("dsp_freq"))) {
        new_tune_request.dsp_freq =
            pmt::to_double(pmt::dict_ref(tune, pmt::mp("dsp_freq"), 0));
    }
    if (pmt::dict_has_key(tune, pmt::mp("rf_freq"))) {
        new_tune_request.rf_freq =
            pmt::to_double(pmt::dict_ref(tune, pmt::mp("rf_freq"), 0));
    }
    if (pmt::dict_has_key(tune, pmt::mp("target_freq"))) {
        new_tune_request.target_freq =
            pmt::to_double(pmt::dict_ref(tune, pmt::mp("target_freq"), 0));
    }
    if (pmt::dict_has_key(tune, pmt::mp("dsp_freq_policy"))) {
        std::string policy = pmt::symbol_to_string(
            pmt::dict_ref(tune, pmt::mp("dsp_freq_policy"), pmt::mp("A")));
        if (policy == "M") {
            new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
        } else if (policy == "A") {
            new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_AUTO;
        } else {
            new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_NONE;
        }
    }
    if (pmt::dict_has_key(tune, pmt::mp("rf_freq_policy"))) {
        std::string policy = pmt::symbol_to_string(
            pmt::dict_ref(tune, pmt::mp("rf_freq_policy"), pmt::mp("A")));
        if (policy == "M") {
            new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
        } else if (policy == "A") {
            new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_AUTO;
        } else {
            new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_NONE;
        }
    }
    if (pmt::dict_has_key(tune, pmt::mp("args"))) {
        new_tune_request.args = ::uhd::device_addr_t(
            pmt::symbol_to_string(pmt::dict_ref(tune, pmt::mp("args"), pmt::mp(""))));
    }

    _update_curr_tune_req(new_tune_request, chan, direction);
}

void usrp_block_impl::_cmd_handler_bw(const pmt::pmt_t& bw,
                                      int chan,
                                      const pmt::pmt_t& msg)
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

void usrp_block_impl::_cmd_handler_lofreq(const pmt::pmt_t& lofreq,
                                          int chan,
                                          const pmt::pmt_t& msg)
{
    // Get the direction key
    const pmt::pmt_t direction = get_cmd_or_default_direction(msg);

    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            _cmd_handler_lofreq(lofreq, int(i), msg);
        }
        return;
    }

    ::uhd::tune_request_t new_tune_request;
    if (pmt::eqv(direction, direction_rx())) {
        new_tune_request = _curr_rx_tune_req[chan];
    } else {
        new_tune_request = _curr_tx_tune_req[chan];
    }

    new_tune_request.rf_freq = pmt::to_double(lofreq);
    if (pmt::dict_has_key(msg, cmd_dsp_freq_key())) {
        new_tune_request.dsp_freq =
            pmt::to_double(pmt::dict_ref(msg, cmd_dsp_freq_key(), pmt::PMT_NIL));
    }
    new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
    new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;

    _update_curr_tune_req(new_tune_request, chan, direction);
}

void usrp_block_impl::_cmd_handler_dspfreq(const pmt::pmt_t& dspfreq,
                                           int chan,
                                           const pmt::pmt_t& msg)
{
    // Get the direction key
    const pmt::pmt_t direction = get_cmd_or_default_direction(msg);

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

    ::uhd::tune_request_t new_tune_request;
    if (pmt::eqv(direction, direction_rx())) {
        new_tune_request = _curr_rx_tune_req[chan];
    } else {
        new_tune_request = _curr_tx_tune_req[chan];
    }

    new_tune_request.dsp_freq = pmt::to_double(dspfreq);
    new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
    new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;

    _update_curr_tune_req(new_tune_request, chan, direction);
}

const pmt::pmt_t
usrp_block_impl::get_cmd_or_default_direction(const pmt::pmt_t& cmd) const
{
    const pmt::pmt_t dir = pmt::dict_ref(cmd, cmd_direction_key(), pmt::PMT_NIL);

    // if the direction key exists and is either "TX" or "RX", return that
    if (pmt::is_symbol(dir) &&
        (pmt::eqv(dir, direction_rx()) || pmt::eqv(dir, direction_tx()))) {
        return dir;
    }
    // otherwise return the direction key for the block that received the cmd
    return _direction();
}
