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
#include <gnuradio/uhd/usrp_sink.h>
#include <uhd/convert.hpp>

static const pmt::pmt_t SOB_KEY = pmt::string_to_symbol("tx_sob");
static const pmt::pmt_t EOB_KEY = pmt::string_to_symbol("tx_eob");
static const pmt::pmt_t TIME_KEY = pmt::string_to_symbol("tx_time");
static const pmt::pmt_t FREQ_KEY = pmt::string_to_symbol("tx_freq");
static const pmt::pmt_t COMMAND_KEY = pmt::string_to_symbol("tx_command");

// Asynchronous message handling related PMTs
static const pmt::pmt_t ASYNC_MSG_KEY = pmt::string_to_symbol("uhd_async_msg");
static const pmt::pmt_t CHANNEL_KEY = pmt::string_to_symbol("channel");
static const pmt::pmt_t TIME_SPEC_KEY = pmt::string_to_symbol("time_spec");
static const pmt::pmt_t EVENT_CODE_KEY = pmt::string_to_symbol("event_code");
static const pmt::pmt_t BURST_ACK_KEY = pmt::string_to_symbol("burst_ack");
static const pmt::pmt_t UNDERFLOW_KEY = pmt::string_to_symbol("underflow");
static const pmt::pmt_t UNDERFLOW_IN_PACKET_KEY =
    pmt::string_to_symbol("underflow_in_packet");
static const pmt::pmt_t SEQ_ERROR_KEY = pmt::string_to_symbol("seq_error");
static const pmt::pmt_t SEQ_ERROR_IN_BURST_KEY =
    pmt::string_to_symbol("seq_error_in_burst");
static const pmt::pmt_t TIME_ERROR_KEY = pmt::string_to_symbol("time_error");
static const pmt::pmt_t ASYNC_MSGS_PORT_KEY = pmt::string_to_symbol("async_msgs");


namespace gr {
namespace uhd {

inline io_signature::sptr args_to_io_sig(const ::uhd::stream_args_t& args)
{
    const size_t nchan = std::max<size_t>(args.channels.size(), 1);
    const size_t size = ::uhd::convert::get_bytes_per_item(args.cpu_format);
    return io_signature::make(nchan, nchan, size);
}

/***********************************************************************
 * UHD Multi USRP Sink Impl
 **********************************************************************/
class usrp_sink_impl : public usrp_sink, public usrp_block_impl
{
public:
    usrp_sink_impl(const ::uhd::device_addr_t& device_addr,
                   const ::uhd::stream_args_t& stream_args,
                   const std::string& length_tag_name);
    ~usrp_sink_impl() override;

    ::uhd::dict<std::string, std::string> get_usrp_info(size_t chan) override;
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

    void set_subdev_spec(const std::string& spec, size_t mboard) override;
    std::string get_subdev_spec(size_t mboard) override;
    void set_samp_rate(double rate) override;
    ::uhd::tune_result_t set_center_freq(const ::uhd::tune_request_t tune_request,
                                         size_t chan) override;
    void set_gain(double gain, size_t chan, pmt::pmt_t direction) override;
    void set_gain(double gain, const std::string& name, size_t chan) override;
    void set_normalized_gain(double gain, size_t chan) override;
    void set_power_reference(double power_dbm, size_t chan) override;
    void set_antenna(const std::string& ant, size_t chan) override;
    void set_bandwidth(double bandwidth, size_t chan) override;
    double get_bandwidth(size_t chan) override;
    ::uhd::freq_range_t get_bandwidth_range(size_t chan) override;
    void set_dc_offset(const std::complex<double>& offset, size_t chan) override;
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

    bool start(void) override;
    bool stop(void) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    inline void tag_work(int& ninput_items);

    void setup_rpc() override;

private:
    //! Like set_center_freq(), but uses _curr_freq and _curr_lo_offset
    ::uhd::tune_result_t _set_center_freq_from_internals(size_t chan,
                                                         pmt::pmt_t direction) override;

    ::uhd::tx_streamer::sptr _tx_stream;
    ::uhd::tx_metadata_t _metadata;
    double _sample_rate;

    // stream tags related stuff
    std::vector<tag_t> _tags;
    const pmt::pmt_t _length_tag_key;
    long _nitems_to_send;

    // asynchronous messages related stuff
    bool _async_event_loop_running;
    void async_event_loop();
    gr::thread::thread _async_event_thread;

    const pmt::pmt_t _direction() const override { return direction_tx(); };
};

} /* namespace uhd */
} /* namespace gr */
