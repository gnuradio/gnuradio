/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space/>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_SOAPY_BLOCK_H
#define INCLUDED_GR_SOAPY_BLOCK_H

#include <gnuradio/block.h>
#include <gnuradio/soapy/api.h>
#include <gnuradio/soapy/soapy_types.h>
#include <cstdint>
#include <string>
#include <vector>

namespace gr {
namespace soapy {

class SOAPY_API block : virtual public gr::block
{
public:
    /*!
     * A key that uniquely identifies the device driver.
     * This key identifies the underlying implementation.
     * Several variants of a product may share a driver.
     */
    virtual std::string get_driver_key() const = 0;

    /*!
     * A key that uniquely identifies the hardware.
     * This key should be meaningful to the user
     * to optimize for the underlying hardware.
     */
    virtual std::string get_hardware_key() const = 0;

    /*!
     * Query a dictionary of available device information.
     * This dictionary can any number of values like
     * vendor name, product name, revisions, serials...
     * This information can be displayed to the user
     * to help identify the instantiated device.
     */
    virtual kwargs_t get_hardware_info() const = 0;

    /*!
     * Set the frontend mapping of available DSP units to RF frontends.
     * This mapping controls channel mapping and channel availability.
     * \param frontend_mapping a vendor-specific mapping string
     */
    virtual void set_frontend_mapping(const std::string& frontend_mapping) = 0;

    /*!
     * Get the frontend mapping of available DSP units to RF frontends.
     * This mapping describes channel mapping and channel availability.
     * \return a vendor-specific mapping string
     */
    virtual std::string get_frontend_mapping() const = 0;

    /*!
     * Query a dictionary of available channel information.
     * This dictionary can any number of values like
     * decoder type, version, available functions...
     * This information can be displayed to the user
     * to help identify the instantiated channel.
     * \param channel an available channel on the device
     * \return channel information
     */
    virtual kwargs_t get_channel_info(size_t channel) const = 0;

    /*!
     * Set sample rate
     * \param channel an available channel
     * \param sample_rate samples per second
     */
    virtual void set_sample_rate(size_t channel, double sample_rate) = 0;

    /*!
     * Get the baseband sample rate of the RX chain.
     * \param channel an available channel on the device
     * \return the sample rate in samples per second
     */
    virtual double get_sample_rate(size_t channel) const = 0;

    /*!
     * Get the range of possible baseband sample rates.
     * \param channel an available channel on the device
     * \return a list of sample rate ranges in samples per second
     */
    virtual range_list_t get_sample_rate_range(size_t channel) const = 0;

    /*!
     * Set device center frequency
     * \param channel an available channel
     * \param freq frequency in Hz
     */
    virtual void set_frequency(size_t channel, double freq) = 0;

    /*!
     * Set center frequency of a tunable element
     * \param channel an available channel
     * \param name an available element name
     * \param freq frequency in Hz
     */
    virtual void set_frequency(size_t channel, const std::string& name, double freq) = 0;

    /*!
     * Get the down conversion frequency of the chain.
     * \param channel an available channel on the device
     * \return the center frequency in Hz
     */
    virtual double get_frequency(size_t channel) const = 0;

    /*!
     * Get the frequency of a tunable element in the chain.
     * \param channel an available channel on the device
     * \param name the name of a tunable element
     * \return the tunable element's frequency in Hz
     */
    virtual double get_frequency(size_t channel, const std::string& name) const = 0;

    /*!
     * List available tunable elements in the chain.
     * Elements should be in order RF to baseband.
     * \param channel an available channel
     * \return a list of tunable elements by name
     */
    virtual std::vector<std::string> list_frequencies(size_t channel) const = 0;

    /*!
     * Get the range of overall frequency values.
     * \param channel an available channel on the device
     * \return a list of frequency ranges in Hz
     */
    virtual range_list_t get_frequency_range(size_t channel) const = 0;

    /*!
     * Get the range of tunable values for the specified element.
     * \param channel an available channel on the device
     * \param name the name of a tunable element
     * \return a list of frequency ranges in Hz
     */
    virtual range_list_t get_frequency_range(size_t channel,
                                             const std::string& name) const = 0;

    /*!
     * Query the argument info description for stream args.
     * \param channel an available channel on the device
     * \return a list of argument info structures
     */
    virtual arginfo_list_t get_frequency_args_info(size_t channel) const = 0;

    /*!
     * Set filter bandwidth
     * \param channel an available channel
     * \param bandwidth filter width in Hz
     */
    virtual void set_bandwidth(size_t channel, double bandwidth) = 0;

    /*!
     * Get baseband filter width of the RX chain.
     * \param channel an available channel on the device
     * \return the baseband filter width in Hz
     */
    virtual double get_bandwidth(size_t channel) const = 0;

    /*!
     * Get the range of possible baseband filter widths.
     * \param channel an available channel on the device
     * \return a list of bandwidth ranges in Hz
     */
    virtual range_list_t get_bandwidth_range(size_t channel) const = 0;

    /*!
     * List available antennas for a channel
     * @param channel channel index
     * @return available antenna names
     */
    virtual std::vector<std::string> list_antennas(int channel) const = 0;

    /*!
     * Set antenna for channel
     * \param channel an available channel
     * \param name an available antenna string name
     */
    virtual void set_antenna(size_t channel, const std::string& name) = 0;

    /*!
     * Get the selected antenna on RX chain.
     * \param channel an available channel on the device
     * \return the name of the selected antenna
     */
    virtual std::string get_antenna(size_t channel) const = 0;

    /*!
     * Return whether automatic gain control (AGC) is supported
     * \param channel an available channel
     */
    virtual bool has_gain_mode(size_t channel) const = 0;

    /*!
     * Set automatic gain control (AGC)
     * \param channel an available channel
     * \param enable true to enable AGC
     */
    virtual void set_gain_mode(size_t channel, bool enable) = 0;

    /*!
     * Get the automatic gain mode on the RX chain.
     * \param channel an available channel on the device
     * \return true for automatic gain setting
     */
    virtual bool get_gain_mode(size_t channel) const = 0;

    /*!
     * List available amplification elements.
     * Elements should be in order RF to baseband.
     * \param channel an available channel
     * \return a list of gain string names
     */
    virtual std::vector<std::string> list_gains(size_t channel) const = 0;

    /*!
     * Set overall gain
     * The gain will be distributed automatically across available
     * elements according to Soapy API.
     * \param channel an available channel
     * \param gain overall gain value
     */
    virtual void set_gain(size_t channel, double gain) = 0;

    /*!
     * Set specific gain value
     * \param channel an available channel
     * \param name gain name to set
     * \param gain gain value
     */
    virtual void set_gain(size_t channel, const std::string& name, double gain) = 0;

    /*!
     * Get the overall value of the gain elements in a chain
     * \param channel an available channel on the device
     * \return the value of the gain in dB
     */
    virtual double get_gain(size_t channel) const = 0;

    /*!
     * Get the value of an individual amplification element in a chain.
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \return the value of the gain in dB
     */
    virtual double get_gain(size_t channel, const std::string& name) const = 0;

    /*!
     * Get the overall range of possible gain values.
     * \param channel an available channel on the device
     * \return a list of gain ranges in dB
     */
    virtual range_t get_gain_range(size_t channel) const = 0;

    /*!
     * Get the range of possible gain values for a specific element.
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \return a list of gain ranges in dB
     */
    virtual range_t get_gain_range(size_t channel, const std::string& name) const = 0;

    /*!
     * Return whether frequency correction is supported
     * \param channel an available channel
     */
    virtual bool has_frequency_correction(size_t channel) const = 0;

    /*!
     * Set frequency correction
     * \param channel an available channel
     * \param freq_correction in PPM
     */
    virtual void set_frequency_correction(size_t channel, double freq_correction) = 0;

    /*!
     * Get the frequency correction value.
     * \param channel an available channel on the device
     * \return the correction value in PPM
     */
    virtual double get_frequency_correction(size_t channel) const = 0;

    /*!
     * Return whether DC offset mode can be set
     * \param channel an available channel
     */
    virtual bool has_dc_offset_mode(size_t channel) const = 0;

    /*!
     * Set DC offset mode
     * \param channel an available channel
     * \param automatic true to set automatic DC removal
     */
    virtual void set_dc_offset_mode(size_t channel, bool automatic) = 0;

    /*!
     * Get the automatic DC offset correction mode.
     * \param channel an available channel on the device
     * \return true for automatic offset correction
     */
    virtual bool get_dc_offset_mode(size_t channel) const = 0;

    /*!
     * Return whether manual dc offset correction is supported
     * \param channel an available channel
     */
    virtual bool has_dc_offset(size_t channel) const = 0;

    /*!
     * Set dc offset correction
     * \param channel an available channel
     * \param dc_offset complex dc offset correction
     */
    virtual void set_dc_offset(size_t channel, const gr_complexd& dc_offset) = 0;

    /*!
     * Get the DC offset correction.
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual gr_complexd get_dc_offset(size_t channel) const = 0;

    /*!
     * Return whether manual IQ balance correction is supported
     * \param channel an available channel
     */
    virtual bool has_iq_balance(size_t channel) const = 0;

    /*!
     * Set IQ balance correction
     * \param channel an available channel
     * \param iq_balance complex iq balance correction
     */
    virtual void set_iq_balance(size_t channel, const gr_complexd& iq_balance) = 0;

    /*!
     * Get the IQ balance correction.
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual gr_complexd get_iq_balance(size_t channel) const = 0;

    /*!
     * Does the device support automatic frontend IQ balance correction?
     * \param channel an available channel on the device
     * \return true if IQ balance corrections are supported
     */
    virtual bool has_iq_balance_mode(size_t channel) const = 0;

    /*!
     * Set the automatic frontend IQ balance correction.
     * \param channel an available channel on the device
     * \param automatic true for automatic correction
     */
    virtual void set_iq_balance_mode(size_t channel, bool automatic) = 0;

    /*!
     * Get the automatic IQ balance corrections mode.
     * \param channel an available channel on the device
     * \return true for automatic correction
     */
    virtual bool get_iq_balance_mode(size_t channel) const = 0;

    /*!
     * Set master clock rate
     * \param clock_rate clock rate in Hz
     */
    virtual void set_master_clock_rate(double clock_rate) = 0;

    /*!
     * Get the master clock rate of the device.
     * \return the clock rate in Hz
     */
    virtual double get_master_clock_rate() const = 0;

    /*!
     * Get the range of available master clock rates.
     * \return a list of clock rate ranges in Hz
     */
    virtual range_list_t get_master_clock_rates() const = 0;

    /*!
     * Set the reference clock rate of the device.
     * \param rate the clock rate in Hz
     */
    virtual void set_reference_clock_rate(double rate) = 0;

    /*!
     * Get the reference clock rate of the device.
     * \return the clock rate in Hz
     */
    virtual double get_reference_clock_rate() const = 0;

    /*!
     * Get the range of available reference clock rates.
     * \return a list of clock rate ranges in Hz
     */
    virtual range_list_t get_reference_clock_rates() const = 0;

    /*!
     * Get the list of available clock sources.
     * \return a list of clock source names
     */
    virtual std::vector<std::string> list_clock_sources() const = 0;

    /*!
     * Set the clock source
     * \param clock_source an available clock source
     */
    virtual void set_clock_source(const std::string& clock_source) = 0;

    /*!
     * Get the clock source of the device
     * \return the name of the clock source
     */
    virtual std::string get_clock_source() const = 0;

    /*!
     * Get the list of available time sources.
     * \return a list of time source names
     */
    virtual std::vector<std::string> list_time_sources() const = 0;

    /*!
     * Set the time source on the device
     * \param source the name of a time source
     */
    virtual void set_time_source(const std::string& source) = 0;

    /*!
     * Get the time source of the device
     * \return the name of a time source
     */
    virtual std::string get_time_source() const = 0;

    /*!
     * Does this device have a hardware clock?
     * \param what optional argument
     * \return true if the hardware clock exists
     */
    virtual bool has_hardware_time(const std::string& what = "") const = 0;

    /*!
     * Read the time from the hardware clock on the device.
     * The what argument can refer to a specific time counter.
     * \param what optional argument
     * \return the time in nanoseconds
     */
    virtual long long get_hardware_time(const std::string& what = "") const = 0;

    /*!
     * Write the time to the hardware clock on the device.
     * The what argument can refer to a specific time counter.
     * \param timeNs time in nanoseconds
     * \param what optional argument
     */
    virtual void set_hardware_time(long long timeNs, const std::string& what = "") = 0;

    /*!
     * List the available global readback sensors.
     * A sensor can represent a reference lock, RSSI, temperature.
     * \return a list of available sensor string names
     */
    virtual std::vector<std::string> list_sensors() const = 0;

    /*!
     * Get meta-information about a sensor.
     * Example: displayable name, type, range.
     * \param key the ID name of an available sensor
     * \return meta-information about a sensor
     */
    virtual arginfo_t get_sensor_info(const std::string& key) const = 0;

    /*!
     * Readback a global sensor given the name.
     * The value returned is a string which can represent
     * a boolean ("true"/"false"), an integer, or float.
     * \param key the ID name of an available sensor
     * \return the current value of the sensor
     */
    virtual std::string read_sensor(const std::string& key) const = 0;

    /*!
     * List the available channel readback sensors.
     * A sensor can represent a reference lock, RSSI, temperature.
     * \param channel an available channel on the device
     * \return a list of available sensor string names
     */
    virtual std::vector<std::string> list_sensors(size_t channel) const = 0;

    /*!
     * Get meta-information about a channel sensor.
     * Example: displayable name, type, range.
     * \param channel an available channel on the device
     * \param key the ID name of an available sensor
     * \return meta-information about a sensor
     */
    virtual arginfo_t get_sensor_info(size_t channel, const std::string& key) const = 0;

    /*!
     * Readback a channel sensor given the name.
     * The value returned is a string which can represent
     * a boolean ("true"/"false"), an integer, or float.
     * \param channel an available channel on the device
     * \param key the ID name of an available sensor
     * \return the current value of the sensor
     */
    virtual std::string read_sensor(size_t channel, const std::string& key) const = 0;

    /*!
     * Get a list of available register interfaces by name.
     * \return a list of available register interfaces
     */
    virtual std::vector<std::string> list_register_interfaces() const = 0;

    /*!
     * Write a register on the device given the interface name.
     * This can represent a register on a soft CPU, FPGA, IC;
     * the interpretation is up the implementation to decide.
     * \param name the name of a available register interface
     * \param addr the register address
     * \param value the register value
     */
    virtual void
    write_register(const std::string& name, unsigned addr, unsigned value) = 0;

    /*!
     * Read a register on the device given the interface name.
     * \param name the name of a available register interface
     * \param addr the register address
     * \return the register value
     */
    virtual unsigned read_register(const std::string& name, unsigned addr) const = 0;

    /*!
     * Write a memory block on the device given the interface name.
     * This can represent a memory block on a soft CPU, FPGA, IC;
     * the interpretation is up the implementation to decide.
     * \param name the name of a available memory block interface
     * \param addr the memory block start address
     * \param value the memory block content
     */
    virtual void write_registers(const std::string& name,
                                 unsigned addr,
                                 const std::vector<unsigned>& value) = 0;

    /*!
     * Read a memory block on the device given the interface name.
     * \param name the name of a available memory block interface
     * \param addr the memory block start address
     * \param length number of words to be read from memory block
     * \return the memory block content
     */
    virtual std::vector<unsigned>
    read_registers(const std::string& name, unsigned addr, size_t length) const = 0;

    /*!
     * Describe the allowed keys and values used for settings.
     * \return a list of argument info structures
     */
    virtual arginfo_list_t get_setting_info() const = 0;

    /*!
     * Write an arbitrary setting on the device.
     * The interpretation is up the implementation.
     * \param key the setting identifier
     * \param value the setting value
     */
    virtual void write_setting(const std::string& key, const std::string& value) = 0;

    /*!
     * Read an arbitrary setting on the device.
     * \param key the setting identifier
     * \return the setting value
     */
    virtual std::string read_setting(const std::string& key) const = 0;

    /*!
     * Describe the allowed keys and values used for channel settings.
     * \param channel an available channel on the device
     * \return a list of argument info structures
     */
    virtual arginfo_list_t get_setting_info(size_t channel) const = 0;

    /*!
     * Write an arbitrary channel setting on the device.
     * The interpretation is up the implementation.
     * \param channel an available channel on the device
     * \param key the setting identifier
     * \param value the setting value
     */
    virtual void
    write_setting(size_t channel, const std::string& key, const std::string& value) = 0;

    /*!
     * Read an arbitrary channel setting on the device.
     * \param channel an available channel on the device
     * \param key the setting identifier
     * \return the setting value
     */
    virtual std::string read_setting(size_t channel, const std::string& key) const = 0;

    /*!
     * Get a list of available GPIO banks by name.
     */
    virtual std::vector<std::string> list_gpio_banks() const = 0;

    /*!
     * Write the value of a GPIO bank.
     * \param bank the name of an available bank
     * \param value an integer representing GPIO bits
     */
    virtual void write_gpio(const std::string& bank, unsigned value) = 0;

    /*!
     * Write the value of a GPIO bank with modification mask.
     * \param bank the name of an available bank
     * \param value an integer representing GPIO bits
     * \param mask a modification mask where 1 = modify
     */
    virtual void write_gpio(const std::string& bank, unsigned value, unsigned mask) = 0;

    /*!
     * Readback the value of a GPIO bank.
     * \param bank the name of an available bank
     * \return an integer representing GPIO bits
     */
    virtual unsigned read_gpio(const std::string& bank) const = 0;

    /*!
     * Write the data direction of a GPIO bank.
     * 1 bits represent outputs, 0 bits represent inputs.
     * \param bank the name of an available bank
     * \param dir an integer representing data direction bits
     */
    virtual void write_gpio_dir(const std::string& bank, unsigned dir) = 0;

    /*!
     * Write the data direction of a GPIO bank with modification mask.
     * 1 bits represent outputs, 0 bits represent inputs.
     * \param bank the name of an available bank
     * \param dir an integer representing data direction bits
     * \param mask a modification mask where 1 = modify
     */
    virtual void write_gpio_dir(const std::string& bank, unsigned dir, unsigned mask) = 0;

    /*!
     * Read the data direction of a GPIO bank.
     * 1 bits represent outputs, 0 bits represent inputs.
     * \param bank the name of an available bank
     * \return an integer representing data direction bits
     */
    virtual unsigned read_gpio_dir(const std::string& bank) const = 0;

    /*!
     * Write to an available I2C slave.
     * If the device contains multiple I2C masters,
     * the address bits can encode which master.
     * \param addr the address of the slave
     * \param data an array of bytes write out
     */
    virtual void write_i2c(int addr, const std::string& data) = 0;

    /*!
     * Read from an available I2C slave.
     * If the device contains multiple I2C masters,
     * the address bits can encode which master.
     * \param addr the address of the slave
     * \param num_bytes the number of bytes to read
     * \return an array of bytes read from the slave
     */
    virtual std::string read_i2c(int addr, size_t num_bytes) = 0;

    /*!
     * Perform a SPI transaction and return the result.
     * Its up to the implementation to set the clock rate,
     * and read edge, and the write edge of the SPI core.
     * SPI slaves without a readback pin will return 0.
     *
     * If the device contains multiple SPI masters,
     * the address bits can encode which master.
     *
     * \param addr an address of an available SPI slave
     * \param data the SPI data, num_bits-1 is first out
     * \param num_bits the number of bits to clock out
     * \return the readback data, num_bits-1 is first in
     */
    virtual unsigned transact_spi(int addr, unsigned data, size_t num_bits) = 0;

    /*!
     * Enumerate the available UART devices.
     * \return a list of names of available UARTs
     */
    virtual std::vector<std::string> list_uarts() const = 0;

    /*!
     * Write data to a UART device.
     * Its up to the implementation to set the baud rate,
     * carriage return settings, flushing on newline.
     * \param which the name of an available UART
     * \param data an array of bytes to write out
     */
    virtual void write_uart(const std::string& which, const std::string& data) = 0;

    /*!
     * Read bytes from a UART until timeout or newline.
     * Its up to the implementation to set the baud rate,
     * carriage return settings, flushing on newline.
     * \param which the name of an available UART
     * \param timeout_us a timeout in microseconds
     * \return an array of bytes read from the UART
     */
    virtual std::string read_uart(const std::string& which,
                                  long timeout_us = 100000) const = 0;
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_BLOCK_H */
