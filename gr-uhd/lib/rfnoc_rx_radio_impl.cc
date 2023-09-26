/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rfnoc_rx_radio_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/uhd/cmd_keys.h>

namespace gr {
namespace uhd {

constexpr size_t MAX_RADIO_REFS = 2; // One for RX, one for TX

const size_t rfnoc_rx_radio::ALL_CHANS = ::uhd::rfnoc::radio_control::ALL_CHANS;

rfnoc_rx_radio::sptr rfnoc_rx_radio::make(rfnoc_graph::sptr graph,
                                          const ::uhd::device_addr_t& block_args,
                                          const int device_select,
                                          const int instance)
{
    return gnuradio::make_block_sptr<rfnoc_rx_radio_impl>(rfnoc_block::make_block_ref(
        graph, block_args, "Radio", device_select, instance, MAX_RADIO_REFS));
}


rfnoc_rx_radio_impl::rfnoc_rx_radio_impl(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref), d_radio_ref(get_block_ref<::uhd::rfnoc::radio_control>())
{
#define REGISTER_CMD_HANDLER(key, _handler)                                   \
    register_msg_cmd_handler(                                                 \
        key, [this](const pmt::pmt_t& var, int chan, const pmt::pmt_t& msg) { \
            this->_handler(var, chan, msg);                                   \
        })
    // Register default command handlers:
    REGISTER_CMD_HANDLER(pmt::symbol_to_string(cmd_stream_cmd_key()),
                         _cmd_handler_stream_cmd);
}

rfnoc_rx_radio_impl::~rfnoc_rx_radio_impl() {}

/******************************************************************************
 * rfnoc_rx_radio API
 *****************************************************************************/
double rfnoc_rx_radio_impl::set_rate(const double rate)
{
    return d_radio_ref->set_rate(rate);
}

void rfnoc_rx_radio_impl::set_antenna(const std::string& antenna, const size_t chan)
{
    return d_radio_ref->set_rx_antenna(antenna, chan);
}

double rfnoc_rx_radio_impl::set_frequency(const double frequency, const size_t chan)
{
    return d_radio_ref->set_rx_frequency(frequency, chan);
}

void rfnoc_rx_radio_impl::set_tune_args(const ::uhd::device_addr_t& args,
                                        const size_t chan)
{
    return d_radio_ref->set_rx_tune_args(args, chan);
}

double rfnoc_rx_radio_impl::set_gain(const double gain, const size_t chan)
{
    return d_radio_ref->set_rx_gain(gain, chan);
}

double rfnoc_rx_radio_impl::set_gain(const double gain,
                                     const std::string& name,
                                     const size_t chan)
{
    return d_radio_ref->set_rx_gain(gain, name, chan);
}

void rfnoc_rx_radio_impl::set_agc(const bool enable, const size_t chan)
{
    return d_radio_ref->set_rx_agc(enable, chan);
}

void rfnoc_rx_radio_impl::set_gain_profile(const std::string& profile, const size_t chan)
{
    return d_radio_ref->set_rx_gain_profile(profile, chan);
}

double rfnoc_rx_radio_impl::set_bandwidth(const double bandwidth, const size_t chan)
{
    return d_radio_ref->set_rx_bandwidth(bandwidth, chan);
}

void rfnoc_rx_radio_impl::set_lo_source(const std::string& source,
                                        const std::string& name,
                                        const size_t chan)
{
    return d_radio_ref->set_rx_lo_source(source, name, chan);
}

void rfnoc_rx_radio_impl::set_lo_export_enabled(const bool enabled,
                                                const std::string& name,
                                                const size_t chan)
{
    return d_radio_ref->set_rx_lo_export_enabled(enabled, name, chan);
}

double rfnoc_rx_radio_impl::set_lo_freq(const double freq,
                                        const std::string& name,
                                        const size_t chan)
{
    return d_radio_ref->set_rx_lo_freq(freq, name, chan);
}

void rfnoc_rx_radio_impl::set_dc_offset(const bool enable, const size_t chan)
{
    return d_radio_ref->set_rx_dc_offset(enable, chan);
}

void rfnoc_rx_radio_impl::set_dc_offset(const std::complex<double>& offset,
                                        const size_t chan)
{
    return d_radio_ref->set_rx_dc_offset(offset, chan);
}

void rfnoc_rx_radio_impl::set_iq_balance(const bool enable, const size_t chan)
{
    return d_radio_ref->set_rx_iq_balance(enable, chan);
}

void rfnoc_rx_radio_impl::set_iq_balance(const std::complex<double>& correction,
                                         const size_t chan)
{
    return d_radio_ref->set_rx_iq_balance(correction, chan);
}

void rfnoc_rx_radio_impl::issue_stream_cmd(const ::uhd::stream_cmd_t& cmd,
                                           const size_t chan)
{
    if (chan == ALL_CHANS) {
        for (size_t ch = 0; ch < d_radio_ref->get_num_output_ports(); ch++) {
            d_radio_ref->issue_stream_cmd(cmd, ch);
        }
        return;
    }
    d_radio_ref->issue_stream_cmd(cmd, chan);
}

void rfnoc_rx_radio_impl::enable_rx_timestamps(const bool enable, const size_t chan)
{
    return d_radio_ref->enable_rx_timestamps(enable, chan);
}


/******************************************************************************
 * Command handlers
 *****************************************************************************/
void rfnoc_rx_radio_impl::_cmd_handler_stream_cmd(const pmt::pmt_t& cmd_p,
                                                  int chan_i,
                                                  const pmt::pmt_t& msg)
{
    ::uhd::stream_cmd_t cmd(::uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    const std::string stream_mode_str = pmt::write_string(
        pmt::dict_ref(cmd_p, pmt::mp("stream_mode"), pmt::mp("start_cont")));
    if (stream_mode_str == "start_cont") {
        // All set already
    } else if (stream_mode_str == "stop_cont") {
        cmd.stream_mode = ::uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    } else if (stream_mode_str == "num_done") {
        cmd.stream_mode = ::uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE;
    } else if (stream_mode_str == "num_more") {
        cmd.stream_mode = ::uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE;
    } else {
        d_logger->warn("Invalid stream command in command message: {}", stream_mode_str);
        return;
    }

    cmd.stream_now =
        pmt::to_bool(pmt::dict_ref(cmd_p, pmt::mp("stream_now"), pmt::mp(false)));
    cmd.num_samps = pmt::to_long(pmt::dict_ref(cmd_p, pmt::mp("num_samps"), pmt::mp(0)));

    if (pmt::dict_has_key(cmd_p, cmd_time_key())) {
        pmt::pmt_t timespec_p = pmt::dict_ref(msg, cmd_time_key(), pmt::PMT_NIL);
        if (timespec_p == pmt::PMT_NIL) {
            cmd.time_spec = ::uhd::time_spec_t(0.0);
        } else {
            ::uhd::time_spec_t timespec(
                time_t(pmt::to_uint64(pmt::car(timespec_p))), // Full secs
                pmt::to_double(pmt::cdr(timespec_p))          // Frac secs
            );
            cmd.time_spec = timespec;
        }
    } else if (!cmd.stream_now) {
        // If the user requests don't stream now, but doesn't provide a time
        // stamp, we'll automatically add a timestamp a bit into the future.
        // However, we can only read back the current time from the radio block
        // starting with UHD 4.2. For older versions, we therefore fall back to
        // stream-now, because stream-not-now without a timespec would guarantee
        // a late command error.
#if UHD_VERSION >= 4020000
        static const double reasonable_delay = 0.1; // s
        cmd.time_spec = d_radio_ref->get_time_now() + reasonable_delay;
#else
        d_logger->warn("Stream command set stream_now to false, but did not provide a "
                       "time spec! Setting stream_now to true.");
        cmd.stream_now = true;
#endif
    }

    d_logger->debug("Received command message: issue stream command ({})",
                    stream_mode_str);

    const size_t chan = (chan_i == -1) ? ALL_CHANS : static_cast<size_t>(chan_i);

    issue_stream_cmd(cmd, chan);
}

} /* namespace uhd */
} /* namespace gr */
