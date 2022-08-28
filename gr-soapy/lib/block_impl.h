/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_SOAPY_BLOCK_IMPL_H
#define INCLUDED_GR_SOAPY_BLOCK_IMPL_H

#include <mutex>

#include <gnuradio/soapy/block.h>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Version.hpp>

namespace gr {
namespace soapy {

using cmd_handler_t = std::function<void(pmt::pmt_t, size_t)>;
struct device_deleter {
    void operator()(SoapySDR::Device* d) { SoapySDR::Device::unmake(d); }
};
using device_ptr_t = std::unique_ptr<SoapySDR::Device, device_deleter>;

/*!
 * \brief Base block implementation for SDR devices.
 */

class block_impl : virtual public block
{
private:
    const int d_direction;
    const std::string d_dev_str;
    const std::string d_args;

    size_t d_mtu = 0;
    std::string d_stream_args;
    std::vector<size_t> d_channels;
    std::string d_soapy_type;
    std::map<pmt::pmt_t, cmd_handler_t> d_cmd_handlers;
    kwargs_list_t d_tune_args;

    void register_msg_cmd_handler(const pmt::pmt_t& cmd, cmd_handler_t handler);

    /*!
     * \brief Raise std::invalid_argument if channel is invalid
     */
    void validate_channel(size_t channel) const;

protected:
    block_impl(int direction,
               const std::string& device,
               const std::string& type,
               size_t nchan,
               const std::string& dev_args,
               const std::string& stream_args,
               const std::vector<std::string>& tune_args,
               const std::vector<std::string>& other_settings);
    block_impl(const block_impl&) = delete;
    block_impl(block_impl&&) = delete;
    block_impl& operator=(const block_impl&) = delete;
    block_impl& operator=(block_impl&&) = delete;
    virtual ~block_impl();

    size_t d_nchan;
    std::mutex d_device_mutex;
    device_ptr_t d_device;
    SoapySDR::Stream* d_stream = nullptr;

    static io_signature::sptr args_to_io_sig(const std::string& type, size_t nchan)
    {
        size_t size = 0;
        if (type == "fc32") {
            size = 8;
        } else if (type == "sc16") {
            size = 4;
        } else if (type == "sc8") {
            size = 2;
        } else {
            size = 1; /* TODO: this is an error */
        }
        return io_signature::make(nchan, nchan, size);
    }

public:
    bool start() override;
    bool stop() override;

    /*** Begin public API implementation ***/

    std::string get_driver_key() const override;
    std::string get_hardware_key() const override;
    kwargs_t get_hardware_info() const override;

    void set_frontend_mapping(const std::string& frontend_mapping) override;
    std::string get_frontend_mapping() const override;

    kwargs_t get_channel_info(size_t channel) const override;

    void set_sample_rate(size_t channel, double sample_rate) override;
    double get_sample_rate(size_t channel) const override;
    range_list_t get_sample_rate_range(size_t channel) const override;

    void set_frequency(size_t channel, double frequency) override;
    void
    set_frequency(size_t channel, const std::string& name, double frequency) override;
    double get_frequency(size_t channel) const override;
    double get_frequency(size_t channel, const std::string& name) const override;
    std::vector<std::string> list_frequencies(size_t channel) const override;
    range_list_t get_frequency_range(size_t channel) const override;
    range_list_t get_frequency_range(size_t channel,
                                     const std::string& name) const override;
    arginfo_list_t get_frequency_args_info(size_t channel) const override;

    void set_bandwidth(size_t channel, double bandwidth) override;
    double get_bandwidth(size_t channel) const override;
    range_list_t get_bandwidth_range(size_t channel) const override;

    std::vector<std::string> list_antennas(int channel) const override;
    void set_antenna(size_t channel, const std::string& name) override;
    std::string get_antenna(size_t channel) const override;

    bool has_gain_mode(size_t channel) const override;
    void set_gain_mode(size_t channel, bool enable) override;
    bool get_gain_mode(size_t channel) const override;

    std::vector<std::string> list_gains(size_t channel) const override;
    void set_gain(size_t channel, double gain) override;
    void set_gain(size_t channel, const std::string& name, double gain) override;
    double get_gain(size_t channel) const override;
    double get_gain(size_t channel, const std::string& name) const override;
    range_t get_gain_range(size_t channel) const override;
    range_t get_gain_range(size_t channel, const std::string& name) const override;

    bool has_frequency_correction(size_t channel) const override;
    void set_frequency_correction(size_t channel, double freq_correction) override;
    double get_frequency_correction(size_t channel) const override;

    bool has_dc_offset_mode(size_t channel) const override;
    void set_dc_offset_mode(size_t channel, bool automatic) override;
    bool get_dc_offset_mode(size_t channel) const override;

    bool has_dc_offset(size_t channel) const override;
    void set_dc_offset(size_t channel, const gr_complexd& dc_offset) override;
    gr_complexd get_dc_offset(size_t channel) const override;

    bool has_iq_balance(size_t channel) const override;
    void set_iq_balance(size_t channel, const gr_complexd& iq_balance) override;
    gr_complexd get_iq_balance(size_t channel) const override;

    bool has_iq_balance_mode(size_t channel) const override;
    void set_iq_balance_mode(size_t channel, bool automatic) override;
    bool get_iq_balance_mode(size_t channel) const override;

    void set_master_clock_rate(double clock_rate) override;
    double get_master_clock_rate() const override;
    range_list_t get_master_clock_rates() const override;

    void set_reference_clock_rate(double rate) override;
    double get_reference_clock_rate() const override;
    range_list_t get_reference_clock_rates() const override;

    std::vector<std::string> list_clock_sources() const override;
    void set_clock_source(const std::string& clock_source) override;
    std::string get_clock_source() const override;

    std::vector<std::string> list_time_sources() const override;
    void set_time_source(const std::string& source) override;
    std::string get_time_source() const override;
    bool has_hardware_time(const std::string& what) const override;
    long long get_hardware_time(const std::string& what) const override;
    void set_hardware_time(long long timeNs, const std::string& what) override;

    std::vector<std::string> list_sensors() const override;
    arginfo_t get_sensor_info(const std::string& key) const override;
    std::string read_sensor(const std::string& key) const override;
    std::vector<std::string> list_sensors(size_t channel) const override;
    arginfo_t get_sensor_info(size_t channel, const std::string& key) const override;
    std::string read_sensor(size_t channel, const std::string& key) const override;

    std::vector<std::string> list_register_interfaces() const override;
    void write_register(const std::string& name, unsigned addr, unsigned value) override;
    unsigned read_register(const std::string& name, unsigned addr) const override;
    void write_registers(const std::string& name,
                         unsigned addr,
                         const std::vector<unsigned>& value) override;
    std::vector<unsigned>
    read_registers(const std::string& name, unsigned addr, size_t length) const override;

    virtual arginfo_list_t get_setting_info() const override;
    virtual void write_setting(const std::string& key, const std::string& value) override;
    virtual std::string read_setting(const std::string& key) const override;
    virtual arginfo_list_t get_setting_info(size_t channel) const override;
    virtual void write_setting(size_t channel,
                               const std::string& key,
                               const std::string& value) override;
    virtual std::string read_setting(size_t channel,
                                     const std::string& key) const override;

    std::vector<std::string> list_gpio_banks() const override;
    void write_gpio(const std::string& bank, unsigned value) override;
    void write_gpio(const std::string& bank, unsigned value, unsigned mask) override;
    unsigned read_gpio(const std::string& bank) const override;
    void write_gpio_dir(const std::string& bank, unsigned dir) override;
    void write_gpio_dir(const std::string& bank, unsigned dir, unsigned mask) override;
    unsigned read_gpio_dir(const std::string& bank) const override;

    void write_i2c(int addr, const std::string& data) override;
    std::string read_i2c(int addr, size_t num_bytes) override;

    unsigned transact_spi(int addr, unsigned data, size_t num_bits) override;

    std::vector<std::string> list_uarts() const override;
    void write_uart(const std::string& which, const std::string& data) override;
    std::string read_uart(const std::string& which, long timeout_us) const override;

    /*** End public API implementation ***/

protected:
    /*** Begin message handlers ***/

    /*!
     * Calls the correct message handler according to the received message symbol.
     * A dictionary with key the handler name is used in order to call the
     * corresponding handler.
     * \param msg a PMT dictionary
     */
    void msg_handler_cmd(pmt::pmt_t msg);

    /*!
     * Set the center frequency of the RX chain.
     * @param val center frequency in Hz
     * @param channel an available channel on the device
     */
    void cmd_handler_frequency(pmt::pmt_t val, size_t channel);

    /*!
     * Set the overall gain for the specified chain.
     * The gain will be distributed automatically across available
     * elements according to Soapy API.
     * @param val the new amplification value in dB
     * @param channel an available channel on the device
     */
    void cmd_handler_gain(pmt::pmt_t val, size_t channel);

    /*!
     * Set the baseband sample rate for the RX chain.
     * @param val the sample rate samples per second
     * @param channel an available channel on the device
     */
    void cmd_handler_samp_rate(pmt::pmt_t val, size_t channel);

    /*!
     * Set the baseband filter width for the RX chain.
     * @param val baseband filter width in Hz
     * @param channel an available channel on the device
     */
    void cmd_handler_bw(pmt::pmt_t val, size_t channel);

    /*!
     * Set the anntena element for the RX chain.
     * @param val name of the anntena
     * @param channel an available channel on the device
     */
    void cmd_handler_antenna(pmt::pmt_t val, size_t channel);

    void cmd_handler_gain_mode(pmt::pmt_t val, size_t channel);
    void cmd_handler_frequency_correction(pmt::pmt_t val, size_t channel);
    void cmd_handler_dc_offset_mode(pmt::pmt_t val, size_t channel);
    void cmd_handler_dc_offset(pmt::pmt_t val, size_t channel);
    void cmd_handler_iq_balance(pmt::pmt_t val, size_t channel);
    void cmd_handler_iq_balance_mode(pmt::pmt_t val, size_t channel);
    void cmd_handler_master_clock_rate(pmt::pmt_t val, size_t);
    void cmd_handler_reference_clock_rate(pmt::pmt_t val, size_t);
    void cmd_handler_clock_source(pmt::pmt_t val, size_t);
    void cmd_handler_time_source(pmt::pmt_t val, size_t);
    void cmd_handler_hardware_time(pmt::pmt_t val, size_t);
    void cmd_handler_register(pmt::pmt_t val, size_t);
    void cmd_handler_registers(pmt::pmt_t val, size_t);
    void cmd_handler_setting(pmt::pmt_t val, size_t channel);
    void cmd_handler_gpio(pmt::pmt_t val, size_t);
    void cmd_handler_gpio_dir(pmt::pmt_t val, size_t);
    void cmd_handler_i2c(pmt::pmt_t val, size_t);
    void cmd_handler_uart(pmt::pmt_t val, size_t);

    /*** End message handlers ***/
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_BLOCK_IMPL_H */
