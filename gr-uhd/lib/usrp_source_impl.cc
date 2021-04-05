/* -*- c++ -*- */
/*
 * Copyright 2010-2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "gr_uhd_common.h"
#include "usrp_source_impl.h"
#include <gnuradio/prefs.h>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>
#include <chrono>
#include <mutex>
#include <stdexcept>

namespace gr {
namespace uhd {


usrp_source::sptr usrp_source::make(const ::uhd::device_addr_t& device_addr,
                                    const ::uhd::stream_args_t& stream_args,
                                    const bool issue_stream_cmd_on_start)
{
    check_abi();
    return usrp_source::sptr(new usrp_source_impl(
        device_addr, stream_args_ensure(stream_args), issue_stream_cmd_on_start));
}

usrp_source_impl::usrp_source_impl(const ::uhd::device_addr_t& device_addr,
                                   const ::uhd::stream_args_t& stream_args,
                                   const bool issue_stream_cmd_on_start)
    : usrp_block("usrp_source", io_signature::make(0, 0, 0), args_to_io_sig(stream_args)),
      usrp_block_impl(device_addr, stream_args, ""),
      _recv_timeout(0.1), // seconds
      _recv_one_packet(true),
      _tag_now(false),
      _issue_stream_cmd_on_start(issue_stream_cmd_on_start),
      _last_log(std::chrono::steady_clock::now()),
      _overflow_log_interval(
          gr::prefs::singleton()->get_long("uhd", "logging_interval_ms", 750))
{
    std::stringstream str;
    str << name() << unique_id();
    _id = pmt::string_to_symbol(str.str());

    _samp_rate = this->get_samp_rate();
    _samps_per_packet = 1;
    register_msg_cmd_handler(cmd_tag_key(),
                             [this](const pmt::pmt_t& tag, const int, const pmt::pmt_t&) {
                                 this->_cmd_handler_tag(tag);
                             });
}

usrp_source_impl::~usrp_source_impl() {}

::uhd::dict<std::string, std::string> usrp_source_impl::get_usrp_info(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_usrp_rx_info(chan);
}

void usrp_source_impl::set_subdev_spec(const std::string& spec, size_t mboard)
{
    return _dev->set_rx_subdev_spec(spec, mboard);
}

std::string usrp_source_impl::get_subdev_spec(size_t mboard)
{
    return _dev->get_rx_subdev_spec(mboard).to_string();
}

void usrp_source_impl::set_samp_rate(double rate)
{
    for (const auto& chan : _stream_args.channels) {
        _dev->set_rx_rate(rate, chan);
    }
    _samp_rate = this->get_samp_rate();
    _tag_now = true;
    auto is_should_ratio = _samp_rate / rate;
    if (is_should_ratio < 0.99 || is_should_ratio > 1.01) {
        GR_LOG_WARN(
            d_logger,
            boost::format("Requested sample rate %g Hz not set; instead, %g Hz used.") %
                rate % _samp_rate);
    }
}

double usrp_source_impl::get_samp_rate(void)
{
    return _dev->get_rx_rate(_stream_args.channels[0]);
}

::uhd::meta_range_t usrp_source_impl::get_samp_rates(void)
{
    return _dev->get_rx_rates(_stream_args.channels[0]);
}

::uhd::tune_result_t
usrp_source_impl::set_center_freq(const ::uhd::tune_request_t tune_request, size_t chan)
{
    chan = _stream_args.channels[chan];
    const ::uhd::tune_result_t res = _dev->set_rx_freq(tune_request, chan);
    _tag_now = true;
    return res;
}

::uhd::tune_result_t
usrp_source_impl::_set_center_freq_from_internals(size_t chan, pmt::pmt_t direction)
{
    if (pmt::eqv(direction, direction_tx())) {
        // TODO: what happens if the TX device is not instantiated? Catch error?
        _tx_chans_to_tune.reset(chan);
        return _dev->set_tx_freq(_curr_tx_tune_req[chan], _stream_args.channels[chan]);
    } else {
        _rx_chans_to_tune.reset(chan);
        return _dev->set_rx_freq(_curr_rx_tune_req[chan], _stream_args.channels[chan]);
    }
}

double usrp_source_impl::get_center_freq(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_freq(chan);
}

::uhd::freq_range_t usrp_source_impl::get_freq_range(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_freq_range(chan);
}

void usrp_source_impl::set_gain(double gain, size_t chan, pmt::pmt_t direction)
{
    chan = _stream_args.channels[chan];
    if (pmt::eqv(direction, direction_tx())) {
        return _dev->set_tx_gain(gain, chan);
    } else {
        return _dev->set_rx_gain(gain, chan);
    }
}

void usrp_source_impl::set_gain(double gain, const std::string& name, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_rx_gain(gain, name, chan);
}

void usrp_source_impl::set_rx_agc(const bool enable, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_rx_agc(enable, chan);
}

void usrp_source_impl::set_normalized_gain(double norm_gain, size_t chan)
{
    _dev->set_normalized_rx_gain(norm_gain, chan);
}

double usrp_source_impl::get_gain(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_gain(chan);
}

double usrp_source_impl::get_gain(const std::string& name, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_gain(name, chan);
}

double usrp_source_impl::get_normalized_gain(size_t chan)
{
    return _dev->get_normalized_rx_gain(chan);
}

std::vector<std::string> usrp_source_impl::get_gain_names(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_gain_names(chan);
}

::uhd::gain_range_t usrp_source_impl::get_gain_range(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_gain_range(chan);
}

::uhd::gain_range_t usrp_source_impl::get_gain_range(const std::string& name, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_gain_range(name, chan);
}

bool usrp_source_impl::has_power_reference(size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_POWER_LEVEL
    if (chan >= _stream_args.channels.size()) {
        throw std::out_of_range("Invalid channel: " + std::to_string(chan));
    }
    const size_t dev_chan = _stream_args.channels[chan];
    return _dev->has_rx_power_reference(dev_chan);
#else
    GR_LOG_WARN(d_logger, "UHD version 4.0 or greater required for power reference API.");
    return false;
#endif
}

void usrp_source_impl::set_power_reference(double power_dbm, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_POWER_LEVEL
    if (chan >= _stream_args.channels.size()) {
        throw std::out_of_range("Invalid channel: " + std::to_string(chan));
    }
    const size_t dev_chan = _stream_args.channels[chan];
    _dev->set_rx_power_reference(power_dbm, dev_chan);
#else
    GR_LOG_ERROR(d_logger,
                 "UHD version 4.0 or greater required for power reference API.");
    throw std::runtime_error("not implemented in this version");
#endif
}

double usrp_source_impl::get_power_reference(size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_POWER_LEVEL
    if (chan >= _stream_args.channels.size()) {
        throw std::out_of_range("Invalid channel: " + std::to_string(chan));
    }
    const size_t dev_chan = _stream_args.channels[chan];
    return _dev->get_rx_power_reference(dev_chan);
#else
    GR_LOG_ERROR(d_logger,
                 "UHD version 4.0 or greater required for power reference API.");
    throw std::runtime_error("not implemented in this version");
#endif
}

::uhd::meta_range_t usrp_source_impl::get_power_range(size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_POWER_LEVEL
    if (chan >= _stream_args.channels.size()) {
        throw std::out_of_range("Invalid channel: " + std::to_string(chan));
    }
    const size_t dev_chan = _stream_args.channels[chan];
    return _dev->get_rx_power_range(dev_chan);
#else
    GR_LOG_ERROR(d_logger,
                 "UHD version 4.0 or greater required for power reference API.");
    throw std::runtime_error("not implemented in this version");
#endif
}

void usrp_source_impl::set_antenna(const std::string& ant, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_rx_antenna(ant, chan);
}

std::string usrp_source_impl::get_antenna(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_antenna(chan);
}

std::vector<std::string> usrp_source_impl::get_antennas(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_antennas(chan);
}

void usrp_source_impl::set_bandwidth(double bandwidth, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_rx_bandwidth(bandwidth, chan);
}

double usrp_source_impl::get_bandwidth(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_bandwidth(chan);
}

::uhd::freq_range_t usrp_source_impl::get_bandwidth_range(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_bandwidth_range(chan);
}

std::vector<std::string> usrp_source_impl::get_lo_names(size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_rx_lo_names(chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

const std::string usrp_source_impl::get_lo_source(const std::string& name, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_rx_lo_source(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

std::vector<std::string> usrp_source_impl::get_lo_sources(const std::string& name,
                                                          size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_rx_lo_sources(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

void usrp_source_impl::set_lo_source(const std::string& src,
                                     const std::string& name,
                                     size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->set_rx_lo_source(src, name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

bool usrp_source_impl::get_lo_export_enabled(const std::string& name, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_rx_lo_export_enabled(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

void usrp_source_impl::set_lo_export_enabled(bool enabled,
                                             const std::string& name,
                                             size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->set_rx_lo_export_enabled(enabled, name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

::uhd::freq_range_t usrp_source_impl::get_lo_freq_range(const std::string& name,
                                                        size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_rx_lo_freq_range(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

double usrp_source_impl::get_lo_freq(const std::string& name, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_rx_lo_freq(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

double usrp_source_impl::set_lo_freq(double freq, const std::string& name, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->set_rx_lo_freq(freq, name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

void usrp_source_impl::set_auto_dc_offset(const bool enable, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_rx_dc_offset(enable, chan);
}

void usrp_source_impl::set_dc_offset(const std::complex<double>& offset, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_rx_dc_offset(offset, chan);
}

void usrp_source_impl::set_auto_iq_balance(const bool enable, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_rx_iq_balance(enable, chan);
}


void usrp_source_impl::set_iq_balance(const std::complex<double>& correction, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_rx_iq_balance(correction, chan);
}

::uhd::sensor_value_t usrp_source_impl::get_sensor(const std::string& name, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_sensor(name, chan);
}

std::vector<std::string> usrp_source_impl::get_sensor_names(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_sensor_names(chan);
}

::uhd::usrp::dboard_iface::sptr usrp_source_impl::get_dboard_iface(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_rx_dboard_iface(chan);
}

#if UHD_VERSION >= 4000000
std::vector<std::string> usrp_source_impl::get_filter_names(const size_t chan)
{
    return _dev->get_rx_filter_names(chan);
}

::uhd::filter_info_base::sptr usrp_source_impl::get_filter(const std::string& path,
                                                           const size_t chan)
{
    return _dev->get_rx_filter(path, chan);
}

void usrp_source_impl::set_filter(const std::string& path,
                                  ::uhd::filter_info_base::sptr filter,
                                  const size_t chan)
{
    _dev->set_rx_filter(path, filter, chan);
}
#else
std::vector<std::string> usrp_source_impl::get_filter_names(const size_t /*chan*/)
{
    return _dev->get_filter_names("rx");
}

::uhd::filter_info_base::sptr usrp_source_impl::get_filter(const std::string& path,
                                                           const size_t /*chan*/)
{
    return _dev->get_filter(path);
}

void usrp_source_impl::set_filter(const std::string& path,
                                  ::uhd::filter_info_base::sptr filter,
                                  const size_t /*chan*/)
{
    _dev->set_filter(path, filter);
}
#endif

void usrp_source_impl::set_stream_args(const ::uhd::stream_args_t& stream_args)
{
    _update_stream_args(stream_args);
    if (_rx_stream) {
        _rx_stream.reset();
    }
}

void usrp_source_impl::_cmd_handler_tag(const pmt::pmt_t& tag) { _tag_now = true; }

void usrp_source_impl::set_start_time(const ::uhd::time_spec_t& time)
{
    _start_time = time;
    _start_time_set = true;
    _stream_now = false;
}

void usrp_source_impl::issue_stream_cmd(const ::uhd::stream_cmd_t& cmd)
{
    _rx_stream->issue_stream_cmd(cmd);
    _tag_now = true;
}

void usrp_source_impl::set_recv_timeout(const double timeout, const bool one_packet)
{
    _recv_timeout = timeout;
    _recv_one_packet = one_packet;
}

bool usrp_source_impl::start(void)
{
    std::lock_guard<std::recursive_mutex> lock(d_mutex);
    if (not _rx_stream) {
        _rx_stream = _dev->get_rx_stream(_stream_args);
        _samps_per_packet = _rx_stream->get_max_num_samps();
    }
    if (_issue_stream_cmd_on_start) {
        // setup a stream command that starts streaming slightly in the future
        static const double reasonable_delay = 0.1; // order of magnitude over RTT
        ::uhd::stream_cmd_t stream_cmd(::uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        stream_cmd.stream_now = _stream_now;
        if (_start_time_set) {
            _start_time_set = false; // cleared for next run
            stream_cmd.time_spec = _start_time;
        } else {
            stream_cmd.time_spec = get_time_now() + ::uhd::time_spec_t(reasonable_delay);
        }
        this->issue_stream_cmd(stream_cmd);
    }
    _tag_now = true;
    return true;
}

void usrp_source_impl::flush(void)
{
    const size_t nbytes = 4096;
    gr_vector_void_star outputs;
    std::vector<std::vector<char>> buffs(_nchan, std::vector<char>(nbytes));
    for (size_t i = 0; i < _nchan; i++) {
        outputs.push_back(&buffs[i].front());
    }
    while (true) {
        const size_t bpi = ::uhd::convert::get_bytes_per_item(_stream_args.cpu_format);
        if (_rx_stream)
            // get the remaining samples out of the buffers
            _rx_stream->recv(outputs, nbytes / bpi, _metadata, 0.0);
        else
            // no rx streamer -- nothing to flush
            break;

        if (_metadata.error_code == ::uhd::rx_metadata_t::ERROR_CODE_TIMEOUT)
            break;
    }
}

bool usrp_source_impl::stop(void)
{
    std::lock_guard<std::recursive_mutex> lock(d_mutex);
    this->issue_stream_cmd(::uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
    this->flush();

    return true;
}

std::vector<std::complex<float>> usrp_source_impl::finite_acquisition(const size_t nsamps)
{
    if (_nchan != 1)
        throw std::runtime_error("finite_acquisition: usrp source has multiple channels, "
                                 "call finite_acquisition_v");
    return finite_acquisition_v(nsamps).front();
}

std::vector<std::vector<std::complex<float>>>
usrp_source_impl::finite_acquisition_v(const size_t nsamps)
{
    // kludgy way to ensure rx streamer exists
    if (!_rx_stream) {
        this->start();
        this->stop();
    }

    // flush so there is no queued-up data
    this->flush();

    // create a multi-dimensional container to hold an array of sample buffers
    std::vector<std::vector<std::complex<float>>> samps(
        _nchan, std::vector<std::complex<float>>(nsamps));

    // load the void* vector of buffer pointers
    std::vector<void*> buffs(_nchan);
    for (size_t i = 0; i < _nchan; i++) {
        buffs[i] = &samps[i].front();
    }

    // tell the device to stream a finite amount
    ::uhd::stream_cmd_t cmd(::uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
    cmd.num_samps = nsamps;
    cmd.stream_now = _stream_now;
    static const double reasonable_delay = 0.1; // order of magnitude over RTT
    cmd.time_spec = get_time_now() + ::uhd::time_spec_t(reasonable_delay);
    this->issue_stream_cmd(cmd);

    // receive samples until timeout
    const size_t actual_num_samps = _rx_stream->recv(buffs, nsamps, _metadata, 1.0);

    // resize the resulting sample buffers
    for (size_t i = 0; i < _nchan; i++) {
        samps[i].resize(actual_num_samps);
    }

    return samps;
}

int usrp_source_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    std::lock_guard<std::recursive_mutex> lock(d_mutex);
    boost::this_thread::disable_interruption disable_interrupt;
    // In order to allow for low-latency:
    // We receive all available packets without timeout.
    // This call can timeout under regular operation...
    size_t num_samps = _rx_stream->recv(
        output_items, noutput_items, _metadata, _recv_timeout, _recv_one_packet);
    boost::this_thread::restore_interruption restore_interrupt(disable_interrupt);

    // handle possible errors conditions
    switch (_metadata.error_code) {
    case ::uhd::rx_metadata_t::ERROR_CODE_NONE:
        if (_tag_now) {
            _tag_now = false;
            // create a timestamp pmt for the first sample
            const pmt::pmt_t val =
                pmt::make_tuple(pmt::from_uint64(_metadata.time_spec.get_full_secs()),
                                pmt::from_double(_metadata.time_spec.get_frac_secs()));
            // create a tag set for each channel
            for (size_t i = 0; i < _nchan; i++) {
                this->add_item_tag(i, nitems_written(0), TIME_KEY, val, _id);
                this->add_item_tag(
                    i, nitems_written(0), RATE_KEY, pmt::from_double(_samp_rate), _id);
                this->add_item_tag(i,
                                   nitems_written(0),
                                   FREQ_KEY,
                                   pmt::from_double(this->get_center_freq(i)),
                                   _id);
            }
        }
        break;

    case ::uhd::rx_metadata_t::ERROR_CODE_TIMEOUT:
        // its ok to timeout, perhaps the user is doing finite streaming
        return 0;

    case ::uhd::rx_metadata_t::ERROR_CODE_OVERFLOW: {
        static auto oflow_msg =
            boost::format("In the last %d ms, %d overflows occurred.");
        _tag_now = true;
        ++_overflow_count;
        auto now = std::chrono::steady_clock::now();
        auto delta = now - _last_log;
        if (delta > _overflow_log_interval) {
            _last_log = now;
            auto ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
            GR_LOG_ERROR(d_logger, oflow_msg % ms % _overflow_count);
            _overflow_count = 0;
        }
        // ignore overflows and try work again
        return work(noutput_items, input_items, output_items);
    }

    default:
        GR_LOG_WARN(d_logger,
                    "USRP Source Block caught rx error: " + _metadata.strerror());
        return num_samps;
    }

    return num_samps;
}

void usrp_source_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_handler<usrp_block>(
        alias(), "command", "", "UHD Commands", RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
}

} /* namespace uhd */
} /* namespace gr */
