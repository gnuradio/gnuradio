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
    size_t d_nchan;
    std::string d_stream_args;
    std::vector<size_t> d_channels;
    std::string d_soapy_type;
    std::map<pmt::pmt_t, cmd_handler_t> d_cmd_handlers;
    std::vector<SoapySDR::Kwargs> d_tune_args;

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

    void set_sample_rate(size_t channel, double sample_rate) override;

    void set_frequency(size_t channel, double frequency) override;
    void
    set_frequency(size_t channel, const std::string& name, double frequency) override;

    void set_bandwidth(size_t channel, double bandwidth) override;

    std::vector<std::string> list_antennas(int channel) const override;
    void set_antenna(size_t channel, const std::string& name) override;

    bool has_gain_mode(size_t channel) const override;
    void set_gain_mode(size_t channel, bool enable) override;

    void set_gain(size_t channel, float gain) override;
    void set_gain(size_t channel, const std::string& name, float gain) override;

    bool has_frequency_correction(size_t channel) const override;
    void set_frequency_correction(size_t channel, double freq_correction) override;

    bool has_dc_offset_mode(size_t channel) const override;
    void set_dc_offset_mode(size_t channel, bool automatic) override;

    bool has_dc_offset(size_t channel) const override;
    void set_dc_offset(size_t channel, gr_complexd dc_offset) override;

    bool has_iq_balance(size_t channel) const override;
    void set_iq_balance(size_t channel, gr_complexd iq_balance) override;

    void set_master_clock_rate(double clock_rate) override;
    void set_clock_source(const std::string& clock_source) override;

    /*** End public API implementation ***/

protected:
    /*!
     * Set the frontend mapping of available DSP units to RF frontends.
     * This mapping controls channel mapping and channel availability.
     * \param frontend_mapping a vendor-specific mapping string
     */
    void set_frontend_mapping(const std::string& frontend_mapping);

    /**
     * Checks if the specified gain type for the given channel is
     * available
     * @param channel an available channel on the device
     * @param name an available gain on the device
     * @return true if the gain setting exists, false otherwise
     */
    bool gain_available(size_t channel, const std::string& name) const;

    /*!
     * Get the down conversion frequency of the chain.
     * \param channel an available channel on the device
     * \return the center frequency in Hz
     */
    double get_frequency(size_t channel) const;

    /*!
     * Get the overall value of the gain elements in a chain
     * \param channel an available channel on the device
     * \return the value of the gain in dB
     */
    double get_gain(size_t channel) const;

    /*!
     * Get the automatic gain mode on the RX chain.
     * \param channel an available channel on the device
     * \return true for automatic gain setting
     */
    bool get_gain_mode(size_t channel) const;

    /*!
     * Get the baseband sample rate of the RX chain.
     * \param channel an available channel on the device
     * \return the sample rate in samples per second
     */
    double get_sample_rate(size_t channel) const;

    /*!
     * Get baseband filter width of the RX chain.
     * \param channel an available channel on the device
     * \return the baseband filter width in Hz
     */
    double get_bandwidth(size_t channel) const;

    /*!
     * Get the selected antenna on RX chain.
     * \param channel an available channel on the device
     * \return the name of the selected antenna
     */
    std::string get_antenna(size_t channel) const;

    /*!
     * Get the DC offset correction.
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    std::complex<double> get_dc_offset(size_t channel) const;

    /*!
     * Get the automatic DC offset correction mode.
     * \param channel an available channel on the device
     * \return true for automatic offset correction
     */
    bool get_dc_offset_mode(size_t channel) const;

    /*!
     * Get the frequency correction value.
     * \param channel an available channel on the device
     * \return the correction value in PPM
     */
    double get_frequency_correction(size_t channel) const;

    /*!
     * Get the IQ balance correction.
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    std::complex<double> get_iq_balance(size_t channel) const;

    /*!
     * Get the master clock rate of the device.
     * \return the clock rate in Hz
     */
    double get_master_clock_rate() const;

    /*!
     * Get the clock source of the device
     * \return the name of the clock source
     */
    std::string get_clock_source() const;

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
     * @param channel an avalaible channel on the device
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

    /*** End message handlers ***/
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_BLOCK_IMPL_H */
