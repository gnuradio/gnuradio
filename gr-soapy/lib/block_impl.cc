/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "block_impl.h"
#include <gnuradio/io_signature.h>
#include <SoapySDR/Formats.h>
#include <SoapySDR/Version.hpp>
#include <boost/format.hpp>
#include <cmath>
#include <numeric>

namespace gr {
namespace soapy {

static bool arg_info_has_key(const arginfo_list_t& info_list, const std::string& key)
{
    return std::any_of(info_list.begin(), info_list.end(), [key](const arginfo_t& info) {
        return info.key == key;
    });
}

template <typename T>
static inline bool vector_contains(const std::vector<T>& vec, const std::string& elem)
{
    return (std::find(vec.begin(), vec.end(), elem) != vec.end());
}

static inline bool value_in_range(const range_t& range, double value)
{
    return (value >= range.minimum()) && (value <= range.maximum());
}

static inline bool value_in_ranges(const range_list_t& ranges, double value)
{
    return std::any_of(ranges.begin(), ranges.end(), [value](const range_t& range) {
        return value_in_range(range, value);
    });
}

static std::string string_vector_to_string(const std::vector<std::string>& vec)
{
    if (vec.empty())
        return "[]";

    std::string str = "[";
    for (const auto& vecStr : vec) {
        str += vecStr;
        str += ", ";
    }
    str.erase(str.length() - 2, 2);

    str += "]";

    return str;
}

static std::string range_to_string(const range_t& range)
{
    const auto min = range.minimum();
    const auto max = range.maximum();

    if (min == max)
        return std::to_string(min);
    else
        return "[" + std::to_string(min) + ", " + std::to_string(max) + "]";
}

static std::string ranges_to_string(const range_list_t& ranges)
{
    if (ranges.empty())
        return "[]";

    std::string str;
    for (const auto& range : ranges) {
        str += range_to_string(range);
        str += ", ";
    }
    str.erase(str.length() - 2, 2);

    return str;
}

static void check_abi(void)
{
    const std::string buildtime_abi = SOAPY_SDR_ABI_VERSION;
    const std::string runtime_abi = SoapySDR::getABIVersion();

    if (buildtime_abi != runtime_abi) {
        throw std::runtime_error(str(
            boost::format(
                "\nGR-Soapy detected ABI compatibility mismatch with SoapySDR library.\n"
                "GR-Soapy was built against ABI: %s,\n"
                "but the SoapySDR library reports ABI: %s\n"
                "Suggestion: install an ABI compatible version of SoapySDR,\n"
                "or rebuild GR-Soapy component against this ABI version.\n") %
            buildtime_abi % runtime_abi));
    }
}

const pmt::pmt_t CMD_CHAN_KEY = pmt::mp("chan");
const pmt::pmt_t CMD_FREQ_KEY = pmt::mp("freq");
const pmt::pmt_t CMD_GAIN_KEY = pmt::mp("gain");
const pmt::pmt_t CMD_ANTENNA_KEY = pmt::mp("ant");
const pmt::pmt_t CMD_RATE_KEY = pmt::mp("rate");
const pmt::pmt_t CMD_BW_KEY = pmt::mp("bw");

/*
 * The private constructor
 */
block_impl::block_impl(int direction,
                       const std::string& device,
                       const std::string& type,
                       size_t nchan,
                       const std::string& dev_args,
                       const std::string& stream_args,
                       const std::vector<std::string>& tune_args,
                       const std::vector<std::string>& other_settings)
    : d_direction(direction),
      d_nchan(nchan),
      d_stream_args(stream_args),
      d_channels(nchan)
{
    check_abi();

    // Channel list
    std::iota(std::begin(d_channels), std::end(d_channels), 0);

    // Must be 1 or nchan.
    if (nchan != tune_args.size() && tune_args.size() != 1) {
        throw std::invalid_argument(name() +
                                    ": Wrong number of channels and tune arguments");
    }

    // Must be 1 or nchan.
    if (nchan != other_settings.size() && other_settings.size() != 1) {
        throw std::invalid_argument(name() + ": Wrong number of channels and settings");
    }

    // Convert type to Soapy type.
    if (type == "fc32") {
        d_soapy_type = SOAPY_SDR_CF32;
    } else if (type == "sc16") {
        d_soapy_type = SOAPY_SDR_CS16;
    } else if (type == "sc8") {
        d_soapy_type = SOAPY_SDR_CS8;
    } else {
        throw std::invalid_argument(name() + ": Invalid IO type");
    }

    kwargs_t dev_kwargs = SoapySDR::KwargsFromString(device + ", " + dev_args);
    d_device = device_ptr_t(SoapySDR::Device::make(dev_kwargs));

    /* Some of the arguments maybe device settings, so we need to re-apply them */
    arginfo_list_t supported_settings = d_device->getSettingInfo();
    for (const auto& arg : dev_kwargs) {
        for (const auto& supported : supported_settings) {
            if (supported.key == arg.first) {
                d_device->writeSetting(arg.first, arg.second);
            }
        }
    }

    if (d_nchan > d_device->getNumChannels(d_direction)) {
        throw std::invalid_argument(
            name() + ": Unsupported number of channels. Only  " +
            std::to_string(d_device->getNumChannels(d_direction)) +
            " channels available.");
    }

    /*
     * Validate stream arguments for all channels
     */
    for (const auto& channel : d_channels) {
        arginfo_list_t supported_args = d_device->getStreamArgsInfo(d_direction, channel);
        kwargs_t stream_kwargs = SoapySDR::KwargsFromString(stream_args);

        for (const auto& arg : stream_kwargs) {
            if (!arg_info_has_key(supported_args, arg.first)) {
                throw std::invalid_argument(name() + ": Unsupported stream argument " +
                                            arg.first + " for channel " +
                                            std::to_string(channel));
            }
        }
    }

    /* Validate tuning arguments for each channel */
    for (const auto& channel : d_channels) {
        arginfo_list_t supported_args =
            d_device->getFrequencyArgsInfo(d_direction, channel);

        /* If single set of args, apply to all channels */
        const size_t arg_idx = tune_args.size() == 1 ? 0 : channel;
        kwargs_t tune_kwargs = SoapySDR::KwargsFromString(tune_args[arg_idx]);

        for (const auto& arg : tune_kwargs) {
            if (!arg_info_has_key(supported_args, arg.first)) {
                throw std::invalid_argument(name() + ": Unsupported tune argument " +
                                            arg.first + " for channel " +
                                            std::to_string(channel));
            }
        }
        d_tune_args.push_back(tune_kwargs);
    }

    /* Validate and apply other settings to each channel */
    for (const auto& channel : d_channels) {
        arginfo_list_t supported_settings =
            d_device->getSettingInfo(d_direction, channel);

        /* If single set of args, apply to all channels */
        const size_t arg_idx = other_settings.size() == 1 ? 0 : channel;
        kwargs_t settings_kwargs = SoapySDR::KwargsFromString(other_settings[arg_idx]);

        /*
         * Before applying the setting, make a last check if the setting
         * is a valid gain
         */
        std::vector<std::string> gain_names = d_device->listGains(d_direction, channel);
        for (const auto& gain_name : gain_names) {
            SoapySDR::Kwargs::iterator iter = settings_kwargs.find(gain_name);
            if (iter != settings_kwargs.end()) {
                d_device->setGain(
                    d_direction, channel, gain_name, std::stod(iter->second));
                /*
                 * As only valid settings are applied, we remove it so it does not
                 * recognized as an invalid setting
                 */
                settings_kwargs.erase(iter);
            }
        }

        for (const auto& arg : settings_kwargs) {
            if (!arg_info_has_key(supported_settings, arg.first)) {
                throw std::invalid_argument(name() + ": Unsupported setting " +
                                            arg.first + " for channel " +
                                            std::to_string(channel));
            }
            d_device->writeSetting(d_direction, channel, arg.first, arg.second);
        }
    }

    message_port_register_in(pmt::mp("cmd"));
    set_msg_handler(pmt::mp("cmd"),
                    [this](pmt::pmt_t pmt) { this->msg_handler_cmd(pmt); });
    register_msg_cmd_handler(CMD_FREQ_KEY, [this](pmt::pmt_t val, size_t channel) {
        this->cmd_handler_frequency(val, channel);
    });
    register_msg_cmd_handler(CMD_GAIN_KEY, [this](pmt::pmt_t val, size_t channel) {
        this->cmd_handler_gain(val, channel);
    });

    register_msg_cmd_handler(CMD_RATE_KEY, [this](pmt::pmt_t val, size_t channel) {
        this->cmd_handler_samp_rate(val, channel);
    });
    register_msg_cmd_handler(CMD_BW_KEY, [this](pmt::pmt_t val, size_t channel) {
        this->cmd_handler_bw(val, channel);
    });
    register_msg_cmd_handler(CMD_ANTENNA_KEY, [this](pmt::pmt_t val, size_t channel) {
        this->cmd_handler_antenna(val, channel);
    });
}

bool block_impl::start()
{
    std::lock_guard<std::mutex> l(d_device_mutex);

    d_stream = d_device->setupStream(
        d_direction, d_soapy_type, d_channels, SoapySDR::KwargsFromString(d_stream_args));
    d_mtu = d_device->getStreamMTU(d_stream);

    /* This limits each work invocation to MTU transfers */
    if (d_mtu > 0) {
        set_max_noutput_items(d_mtu);
    } else {
        set_max_noutput_items(1024);
    }

    d_device->activateStream(d_stream);

    return true;
}

bool block_impl::stop()
{
    if (d_stream) {
        std::lock_guard<std::mutex> l(d_device_mutex);
        d_device->closeStream(d_stream);
        d_stream = nullptr;
    }

    return true;
}

block_impl::~block_impl() {}

void block_impl::register_msg_cmd_handler(const pmt::pmt_t& cmd, cmd_handler_t handler)
{
    d_cmd_handlers[cmd] = handler;
}

void block_impl::validate_channel(size_t channel) const
{
    if (channel >= d_nchan) {
        throw std::invalid_argument(name() + ": Channel " + std::to_string(channel) +
                                    " is not activated.");
    }
}

void block_impl::set_frontend_mapping(const std::string& mapping)
{
    d_device->setFrontendMapping(d_direction, mapping);
}

std::string block_impl::get_frontend_mapping() const
{
    return d_device->getFrontendMapping(d_direction);
}

kwargs_t block_impl::get_channel_info(size_t channel) const
{
    validate_channel(channel);
    return d_device->getChannelInfo(d_direction, channel);
}

void block_impl::set_sample_rate(size_t channel, double sample_rate)
{
    validate_channel(channel);

    range_list_t sps_range = d_device->getSampleRateRange(d_direction, channel);

    if (!value_in_ranges(sps_range, sample_rate)) {
        std::string msg = name() + ": Unsupported sample rate (" +
                          std::to_string(sample_rate) + ").  Rate must be in the range ";
        msg += ranges_to_string(sps_range);

        throw std::invalid_argument(msg);
    }

    d_device->setSampleRate(d_direction, channel, sample_rate);
}

double block_impl::get_sample_rate(size_t channel) const
{
    validate_channel(channel);
    return d_device->getSampleRate(d_direction, channel);
}

range_list_t block_impl::get_sample_rate_range(size_t channel) const
{
    validate_channel(channel);
    return d_device->getSampleRateRange(d_direction, channel);
}

void block_impl::set_frequency(size_t channel, double frequency)
{
    validate_channel(channel);
    d_device->setFrequency(d_direction, channel, frequency, d_tune_args[channel]);
}

void block_impl::set_frequency(size_t channel, const std::string& name, double frequency)
{
    validate_channel(channel);
    std::vector<std::string> freqs = d_device->listFrequencies(d_direction, channel);

    // Set frequency for specified element, silently ignoring a non-existant
    // element if frequency is zero.
    if (vector_contains(freqs, name)) {
        d_device->setFrequency(d_direction, channel, name, frequency);
    } else if (std::fpclassify(std::abs(frequency)) != FP_ZERO) {
        throw std::invalid_argument(
            this->name() + ": Channel " + std::to_string(channel) +
            " does not support frequency " + name +
            ". Consider setting this frequency to 0 to bypass this error");
    }
}

double block_impl::get_frequency(size_t channel) const
{
    validate_channel(channel);
    return d_device->getFrequency(d_direction, channel);
}

double block_impl::get_frequency(size_t channel, const std::string& name) const
{
    validate_channel(channel);
    return d_device->getFrequency(d_direction, channel, name);
}

std::vector<std::string> block_impl::list_frequencies(size_t channel) const
{
    validate_channel(channel);
    return d_device->listFrequencies(d_direction, channel);
}

range_list_t block_impl::get_frequency_range(size_t channel) const
{
    validate_channel(channel);
    return d_device->getFrequencyRange(d_direction, channel);
}

range_list_t block_impl::get_frequency_range(size_t channel,
                                             const std::string& name) const
{
    validate_channel(channel);
    return d_device->getFrequencyRange(d_direction, channel, name);
}

void block_impl::set_bandwidth(size_t channel, double bandwidth)
{
    validate_channel(channel);
    d_device->setBandwidth(d_direction, channel, bandwidth);
}

double block_impl::get_bandwidth(size_t channel) const
{
    validate_channel(channel);
    return d_device->getBandwidth(d_direction, channel);
}

range_list_t block_impl::get_bandwidth_range(size_t channel) const
{
    validate_channel(channel);
    return d_device->getBandwidthRange(d_direction, channel);
}

std::vector<std::string> block_impl::list_antennas(int channel) const
{
    validate_channel(channel);
    return d_device->listAntennas(d_direction, channel);
}

void block_impl::set_antenna(const size_t channel, const std::string& name)
{
    validate_channel(channel);
    std::vector<std::string> antennas = d_device->listAntennas(d_direction, channel);

    // Ignore call if there are no antennas.
    if (antennas.size() == 0) {
        return;
    }

    if (!vector_contains(antennas, name)) {
        std::string msg = this->name() + ": Antenna " + name + " at channel " +
                          std::to_string(channel) + " is not supported. " +
                          "Available antennas are: ";
        msg += string_vector_to_string(antennas);

        throw std::invalid_argument(msg);
    }

    d_device->setAntenna(d_direction, channel, name);
}

std::string block_impl::get_antenna(size_t channel) const
{
    validate_channel(channel);
    return d_device->getAntenna(d_direction, channel);
}

bool block_impl::has_gain_mode(size_t channel) const
{
    validate_channel(channel);
    return d_device->hasGainMode(d_direction, channel);
}

void block_impl::set_gain_mode(size_t channel, bool enable)
{
    validate_channel(channel);
    // TODO: ignoring error if disabling AGC ... is this a good idea?
    if (!has_gain_mode(channel) && enable) {
        throw std::invalid_argument(name() + ": Channel " + std::to_string(channel) +
                                    " does not support AGC");
    }
    d_device->setGainMode(d_direction, channel, enable);
}

bool block_impl::get_gain_mode(size_t channel) const
{
    validate_channel(channel);
    return d_device->getGainMode(d_direction, channel);
}

std::vector<std::string> block_impl::list_gains(size_t channel) const
{
    validate_channel(channel);
    return d_device->listGains(d_direction, channel);
}

void block_impl::set_gain(size_t channel, double gain)
{
    validate_channel(channel);
    range_t rGain = d_device->getGainRange(d_direction, channel);

    if (!value_in_range(rGain, gain)) {
        GR_LOG_WARN(d_logger,
                    boost::format("Gain out of range: %d <= gain <= %d") %
                        rGain.minimum() % rGain.maximum());
        return;
    }

    d_device->setGain(d_direction, channel, gain);
}

void block_impl::set_gain(size_t channel, const std::string& name, double gain)
{
    validate_channel(channel);

    /* Validate gain name */
    std::vector<std::string> gains = d_device->listGains(d_direction, channel);
    if (!vector_contains(gains, name)) {
        throw std::invalid_argument(this->name() + ": Unknown gain " + name +
                                    " for channel " + std::to_string(channel));
    }

    /* Validate gain value */
    range_t rGain = d_device->getGainRange(d_direction, channel, name);
    if (!value_in_range(rGain, gain)) {
        GR_LOG_WARN(d_logger,
                    boost::format("Gain %s out of range: %d <= gain <= %d") % name %
                        rGain.minimum() % rGain.maximum());
    }

    d_device->setGain(d_direction, channel, name, gain);
}

double block_impl::get_gain(size_t channel) const
{
    validate_channel(channel);
    return d_device->getGain(d_direction, channel);
}

double block_impl::get_gain(size_t channel, const std::string& name) const
{
    validate_channel(channel);
    return d_device->getGain(d_direction, channel, name);
}

range_t block_impl::get_gain_range(size_t channel) const
{
    validate_channel(channel);
    return d_device->getGainRange(d_direction, channel);
}

range_t block_impl::get_gain_range(size_t channel, const std::string& name) const
{
    validate_channel(channel);
    return d_device->getGainRange(d_direction, channel, name);
}

bool block_impl::has_frequency_correction(size_t channel) const
{
    validate_channel(channel);
    return d_device->hasFrequencyCorrection(d_direction, channel);
}

void block_impl::set_frequency_correction(size_t channel, double freq_correction)
{
    validate_channel(channel);

    // Set frequency correction, ignoring missing functionality if correction
    // is zero.
    if (has_frequency_correction(channel)) {
        d_device->setFrequencyCorrection(d_direction, channel, freq_correction);
    } else if (std::fpclassify(std::abs(freq_correction)) != FP_ZERO) {
        throw std::invalid_argument(this->name() + ": Channel " +
                                    std::to_string(channel) +
                                    " does not support frequency correction setting");
    }
}

double block_impl::get_frequency_correction(size_t channel) const
{
    validate_channel(channel);
    return d_device->getFrequencyCorrection(d_direction, channel);
}

bool block_impl::has_dc_offset_mode(size_t channel) const
{
    validate_channel(channel);
    return d_device->hasDCOffsetMode(d_direction, channel);
}

void block_impl::set_dc_offset_mode(size_t channel, bool automatic)
{
    validate_channel(channel);

    // TODO: ignoring error if disabling automatic ... is this a good idea?
    if (!has_dc_offset_mode(channel) && automatic) {
        throw std::invalid_argument(this->name() + ": Channel " +
                                    std::to_string(channel) +
                                    " does not support automatic DC removal setting");
    }
    d_device->setDCOffsetMode(d_direction, channel, automatic);
}

bool block_impl::get_dc_offset_mode(size_t channel) const
{
    validate_channel(channel);
    return d_device->getDCOffsetMode(d_direction, channel);
}

bool block_impl::has_dc_offset(size_t channel) const
{
    validate_channel(channel);
    return d_device->hasDCOffset(d_direction, channel);
}

void block_impl::set_dc_offset(size_t channel, const gr_complexd& dc_offset)
{
    validate_channel(channel);

    const bool supported = has_dc_offset(channel);
    /*
     * In case of 0 DC offset, the method exits to avoid failing in case of the
     * device does not supports DC offset
     */
    if (std::fpclassify(std::norm(dc_offset)) == FP_ZERO) {
        if (supported && !d_device->getDCOffsetMode(d_direction, channel)) {
            d_device->setDCOffset(d_direction, channel, dc_offset);
        }
        return;
    }

    if (!supported) {
        throw std::invalid_argument(this->name() + ": Channel " +
                                    std::to_string(channel) +
                                    " does not support DC offset setting");
    }
    d_device->setDCOffset(d_direction, channel, dc_offset);
}

gr_complexd block_impl::get_dc_offset(size_t channel) const
{
    validate_channel(channel);
    return d_device->getDCOffset(d_direction, channel);
}

bool block_impl::has_iq_balance(size_t channel) const
{
    validate_channel(channel);
    return d_device->hasIQBalance(d_direction, channel);
}

void block_impl::set_iq_balance(size_t channel, const gr_complexd& iq_balance)
{
    validate_channel(channel);

    const bool supported = has_iq_balance(channel);
    /*
     * In case of 0 frequency correction, the method exits to avoid failing
     * in case of the device does not supports DC offset
     */
    if (std::fpclassify(std::norm(iq_balance)) == FP_ZERO) {
        if (supported) {
            d_device->setIQBalance(d_direction, channel, iq_balance);
        }
        return;
    }

    if (!supported) {
        throw std::invalid_argument(this->name() + ": Channel " +
                                    std::to_string(channel) +
                                    " does not support IQ imbalance correction setting");
    }
    d_device->setIQBalance(d_direction, channel, iq_balance);
}

gr_complexd block_impl::get_iq_balance(size_t channel) const
{
    validate_channel(channel);
    return d_device->getIQBalance(d_direction, channel);
}

void block_impl::set_master_clock_rate(double clock_rate)
{
    const auto clock_rates = d_device->getMasterClockRates();

    if (!value_in_ranges(clock_rates, clock_rate)) {
        std::string msg = "Unsupported clock rate (";
        msg += std::to_string(clock_rate);
        msg += "). Clock rate must be in the range ";
        msg += ranges_to_string(clock_rates);

        throw std::invalid_argument(msg);
    }
    d_device->setMasterClockRate(clock_rate);
}

double block_impl::get_master_clock_rate() const
{
    return d_device->getMasterClockRate();
}

range_list_t block_impl::get_master_clock_rates() const
{
    return d_device->getMasterClockRates();
}

std::vector<std::string> block_impl::list_clock_sources() const
{
    return d_device->listClockSources();
}

void block_impl::set_clock_source(const std::string& clock_source)
{
    const auto clock_sources = d_device->listClockSources();

    if (!vector_contains(clock_sources, clock_source)) {
        std::string msg = "Invalid clock source (" + clock_source + ").";
        msg += "Valid clock sources: ";
        msg += string_vector_to_string(clock_sources);

        throw std::invalid_argument(msg);
    }

    d_device->setClockSource(clock_source);
}

std::string block_impl::get_clock_source() const { return d_device->getClockSource(); }

/* End public API implementation */

void block_impl::cmd_handler_frequency(pmt::pmt_t val, size_t channel)
{
    if (!(val->is_number() && !val->is_complex())) {
        GR_LOG_WARN(d_logger, "soapy: freq must be float/int");
        return;
    }
    set_frequency(channel, pmt::to_double(val));
}

void block_impl::cmd_handler_gain(pmt::pmt_t val, size_t channel)
{
    if (!(val->is_number() && !val->is_complex())) {
        GR_LOG_WARN(d_logger, "soapy: gain must be float/int");
        return;
    }
    set_gain(channel, pmt::to_float(val));
}

void block_impl::cmd_handler_samp_rate(pmt::pmt_t val, size_t channel)
{
    if (!(val->is_number() && !val->is_complex())) {
        GR_LOG_WARN(d_logger, "soapy: rate must be float/int");
        return;
    }
    set_sample_rate(channel, pmt::to_double(val));
}

void block_impl::cmd_handler_bw(pmt::pmt_t val, size_t channel)
{
    if (!(val->is_number() && !val->is_complex())) {
        GR_LOG_WARN(d_logger, "soapy: bw must be float/int");
        return;
    }
    set_bandwidth(channel, pmt::to_double(val));
}

void block_impl::cmd_handler_antenna(pmt::pmt_t val, size_t channel)
{
    if (!val->is_symbol()) {
        GR_LOG_WARN(d_logger, "soapy: ant must be string");
        return;
    }
    set_antenna(channel, pmt::symbol_to_string(val));
}

void block_impl::msg_handler_cmd(pmt::pmt_t msg)
{
    const long CHANNEL_ALL = 0x12345678; // arbitrary

    if (!pmt::is_dict(msg)) {
        GR_LOG_WARN(d_logger, "soapy: commands must be pmt::dict");
        return;
    }

    // Channel not specified means apply to all
    const pmt::pmt_t channel_all_pmt = pmt::from_long(CHANNEL_ALL);
    size_t channel = pmt::to_long(pmt::dict_ref(msg, CMD_CHAN_KEY, channel_all_pmt));

    if (channel != CHANNEL_ALL && channel >= d_nchan) {
        GR_LOG_WARN(d_logger,
                    boost::format("soapy: ignoring command for invalid channel %d") %
                        channel);
        return;
    }

    for (size_t i = 0; i < pmt::length(msg); i++) {
        const pmt::pmt_t item = pmt::nth(i, msg);
        const pmt::pmt_t key = pmt::car(item);
        const pmt::pmt_t val = pmt::cdr(item);
        if (key == CMD_CHAN_KEY) {
            continue;
        }

        // Find command handler
        auto it = d_cmd_handlers.find(key);
        if (it == d_cmd_handlers.end()) {
            GR_LOG_WARN(d_logger,
                        boost::format("soapy: ignoring unknown command key '%s'") %
                            pmt::symbol_to_string(key));
            continue;
        }
        cmd_handler_t handler = it->second;

        if (channel != CHANNEL_ALL) {
            std::lock_guard<std::mutex> l(d_device_mutex);
            handler(val, channel);
        } else {
            for (size_t c = 0; c < d_nchan; c++) {
                std::lock_guard<std::mutex> l(d_device_mutex);
                handler(val, c);
            }
        }
    }
}
} /* namespace soapy */
} /* namespace gr */
