/* -*- c++ -*- */
/*
 * Copyright 2010-2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "usrp_block_impl.h"
#include <gnuradio/uhd/usrp_source.h>
#include <uhd/convert.hpp>
#include <algorithm>
#include <chrono>
#include <complex>
#include <mutex>
#include <string>
#include <vector>

static const pmt::pmt_t TIME_KEY = pmt::string_to_symbol("rx_time");
static const pmt::pmt_t RATE_KEY = pmt::string_to_symbol("rx_rate");
static const pmt::pmt_t FREQ_KEY = pmt::string_to_symbol("rx_freq");

namespace gr {
namespace uhd {

inline io_signature::sptr args_to_io_sig(const ::uhd::stream_args_t& args)
{
    const size_t nchan = std::max<size_t>(args.channels.size(), 1);
    const size_t size = ::uhd::convert::get_bytes_per_item(args.cpu_format);
    return io_signature::make(nchan, nchan, size);
}

/***********************************************************************
 * UHD Multi USRP Source Impl
 **********************************************************************/
class usrp_source_impl : public usrp_source, public usrp_block_impl
{
public:
    usrp_source_impl(const ::uhd::device_addr_t& device_addr,
                     const ::uhd::stream_args_t& stream_args,
                     const bool issue_stream_cmd_on_start = true);
    ~usrp_source_impl() override;

    // Get Commands
    ::uhd::dict<std::string, std::string> get_usrp_info(size_t chan) override;
    std::string get_subdev_spec(size_t mboard) override;
    double get_samp_rate(void) override;
    ::uhd::meta_range_t get_samp_rates(void) override;
    double get_center_freq(size_t chan) override;
    ::uhd::freq_range_t get_freq_range(size_t chan) override;
    double get_gain(size_t chan) override;
    double get_gain(const std::string& name, size_t chan) override;
    double get_normalized_gain(size_t chan) override;
    std::vector<std::string> get_gain_names(size_t chan) override;
    ::uhd::gain_range_t get_gain_range(size_t chan) override;
    ::uhd::gain_range_t get_gain_range(const std::string& name, size_t chan) override;
    bool has_power_reference(size_t chan) override;
    double get_power_reference(size_t chan) override;
    ::uhd::meta_range_t get_power_range(size_t chan) override;
    std::string get_antenna(size_t chan) override;
    std::vector<std::string> get_antennas(size_t chan) override;
    ::uhd::sensor_value_t get_sensor(const std::string& name, size_t chan) override;
    std::vector<std::string> get_sensor_names(size_t chan) override;
    ::uhd::usrp::dboard_iface::sptr get_dboard_iface(size_t chan) override;
    std::vector<std::string> get_lo_names(size_t chan) override;
    const std::string get_lo_source(const std::string& name, size_t chan) override;
    std::vector<std::string> get_lo_sources(const std::string& name,
                                            size_t chan) override;
    bool get_lo_export_enabled(const std::string& name, size_t chan) override;
    double get_lo_freq(const std::string& name, size_t chan) override;
    ::uhd::freq_range_t get_lo_freq_range(const std::string& name, size_t chan) override;
    std::vector<std::string> get_filter_names(const size_t chan) override;
    ::uhd::filter_info_base::sptr get_filter(const std::string& path,
                                             const size_t chan) override;

    // Set Commands
    void set_subdev_spec(const std::string& spec, size_t mboard) override;
    void set_samp_rate(double rate) override;
    ::uhd::tune_result_t set_center_freq(const ::uhd::tune_request_t tune_request,
                                         size_t chan) override;
    void
    set_gain(double gain, size_t chan = 0, pmt::pmt_t direction = pmt::PMT_NIL) override;
    void set_gain(double gain, const std::string& name, size_t chan) override;
    void set_rx_agc(const bool enable, size_t chan) override;
    void set_normalized_gain(double gain, size_t chan) override;
    void set_power_reference(double power_dbm, size_t chan) override;
    void set_antenna(const std::string& ant, size_t chan) override;
    void set_bandwidth(double bandwidth, size_t chan) override;
    double get_bandwidth(size_t chan) override;
    ::uhd::freq_range_t get_bandwidth_range(size_t chan) override;
    void set_auto_dc_offset(const bool enable, size_t chan) override;
    void set_dc_offset(const std::complex<double>& offset, size_t chan) override;
    void set_auto_iq_balance(const bool enable, size_t chan) override;
    void set_iq_balance(const std::complex<double>& correction, size_t chan) override;
    void set_stream_args(const ::uhd::stream_args_t& stream_args) override;
    void set_start_time(const ::uhd::time_spec_t& time) override;
    void set_lo_source(const std::string& src,
                       const std::string& name = ALL_LOS,
                       size_t chan = 0) override;
    void set_lo_export_enabled(bool enabled,
                               const std::string& name = ALL_LOS,
                               size_t chan = 0) override;
    double set_lo_freq(double freq, const std::string& name, size_t chan) override;
    void set_filter(const std::string& path,
                    ::uhd::filter_info_base::sptr filter,
                    const size_t chan) override;

    void issue_stream_cmd(const ::uhd::stream_cmd_t& cmd) override;
    void set_recv_timeout(const double timeout, const bool one_packet) override;
    void flush(void);
    bool start(void) override;
    bool stop(void) override;
    std::vector<std::complex<float>> finite_acquisition(const size_t nsamps) override;
    std::vector<std::vector<std::complex<float>>>
    finite_acquisition_v(const size_t nsamps) override;
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    void setup_rpc() override;

private:
    //! Like set_center_freq(), but uses _curr_freq and _curr_lo_offset
    ::uhd::tune_result_t _set_center_freq_from_internals(size_t chan,
                                                         pmt::pmt_t direction) override;
    void _cmd_handler_tag(const pmt::pmt_t& tag);

    ::uhd::rx_streamer::sptr _rx_stream;
    size_t _samps_per_packet;
    //! Timeout value for UHD's recv() call. Lower values mean lower latency.
    double _recv_timeout;
    //! one_packet value for UHD's recv() call. 'true' is lower latency.
    bool _recv_one_packet;

    bool _tag_now;
    ::uhd::rx_metadata_t _metadata;
    pmt::pmt_t _id;
    bool _issue_stream_cmd_on_start;
    std::chrono::time_point<std::chrono::steady_clock> _last_log;
    unsigned int _overflow_count;
    std::chrono::milliseconds _overflow_log_interval;

    // tag shadows
    double _samp_rate;

    std::recursive_mutex d_mutex;

    const pmt::pmt_t _direction() const override { return direction_rx(); };
};

} /* namespace uhd */
} /* namespace gr */
