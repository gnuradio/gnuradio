/* -*- c++ -*- */
/*
 * gr-soapy: Soapy SDR Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, 2019, 2020
 *  Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "source_impl.h"
#include <gnuradio/io_signature.h>
#include <SoapySDR/Formats.h>
#include <cmath>

const pmt::pmt_t CMD_CHAN_KEY = pmt::mp("chan");
const pmt::pmt_t CMD_FREQ_KEY = pmt::mp("freq");
const pmt::pmt_t CMD_GAIN_KEY = pmt::mp("gain");
const pmt::pmt_t CMD_ANTENNA_KEY = pmt::mp("antenna");
const pmt::pmt_t CMD_RATE_KEY = pmt::mp("samp_rate");
const pmt::pmt_t CMD_BW_KEY = pmt::mp("bw");

namespace gr {
namespace soapy {

source::sptr source::make(size_t nchan,
                          const std::string& device,
                          const std::string& dev_args,
                          const std::string& stream_args,
                          const std::vector<std::string>& tune_args,
                          const std::vector<std::string>& other_settings,
                          double sampling_rate,
                          const std::string& type)
{
    return gnuradio::make_block_sptr<source_impl>(nchan,
                                                  device,
                                                  dev_args,
                                                  stream_args,
                                                  tune_args,
                                                  other_settings,
                                                  sampling_rate,
                                                  type);
}


/*
 * The private constructor
 */
source_impl::source_impl(size_t nchan,
                         const std::string& device,
                         const std::string& dev_args,
                         const std::string& stream_args,
                         const std::vector<std::string>& tune_args,
                         const std::vector<std::string>& other_settings,
                         double sampling_rate,
                         const std::string& type)
    : gr::sync_block(
          "source", gr::io_signature::make(0, 0, 0), args_to_io_sig(type, nchan)),
      d_stopped(true),
      d_mtu(0),
      d_nchan(nchan)
{
    if (nchan != tune_args.size()) {
        std::string msg = name() + ": Wrong number of channels and tune arguments";
        throw std::invalid_argument(msg);
    }

    if (nchan != other_settings.size()) {
        std::string msg = name() + ": Wrong number of channels and settings";
        throw std::invalid_argument(msg);
    }

    std::string stype;
    if (type == "fc32") {
        stype = SOAPY_SDR_CF32;
    } else if (type == "sc16") {
        stype = SOAPY_SDR_CS16;
    } else if (type == "sc8") {
        stype = SOAPY_SDR_CS8;
    } else {
        std::string msg = name() + ": Invalid IO type";
        throw std::invalid_argument(msg);
    }

    SoapySDR::Kwargs dev_kwargs = SoapySDR::KwargsFromString(device + ", " + dev_args);
    d_device = SoapySDR::Device::make(dev_kwargs);
    d_stopped = false;

    /* Some of the arguments maybe device settings, so we need to re-apply them*/
    SoapySDR::ArgInfoList supported_settings = d_device->getSettingInfo();
    for (const std::pair<std::string, std::string>& i : dev_kwargs) {
        for (SoapySDR::ArgInfo& arg : supported_settings) {
            if (arg.key == i.first) {
                d_device->writeSetting(i.first, i.second);
            }
        }
    }

    if (d_nchan > d_device->getNumChannels(SOAPY_SDR_RX)) {
        std::string msg = name() + ": Unsupported number of channels. Only  " +
                          std::to_string(d_device->getNumChannels(SOAPY_SDR_RX)) +
                          " channels available.";
        throw std::invalid_argument(msg);
    }

    std::vector<size_t> channs;
    channs.resize(d_nchan);
    bool accept_samp_rate = false;
    for (size_t i = 0; i < d_nchan; i++) {
        channs[i] = i;

        SoapySDR::RangeList sps_range = d_device->getSampleRateRange(SOAPY_SDR_RX, i);

        for (SoapySDR::Range& range : sps_range) {
            double min_sps = range.minimum();
            double max_sps = range.maximum();

            if ((sampling_rate < min_sps) || (sampling_rate > max_sps)) {
                continue;
            } else {
                accept_samp_rate = true;
                break;
            }
        }
        if (!accept_samp_rate) {
            std::string msg = name() + ": Unsupported sample rate (" +
                              std::to_string(sampling_rate) +
                              ").  Rate must be in the range";
            for (SoapySDR::Range& range : sps_range) {
                double min_sps = range.minimum();
                double max_sps = range.maximum();
                msg +=
                    " [" + std::to_string(min_sps) + ", " + std::to_string(max_sps) + "]";
            }
            throw std::invalid_argument(msg);
        }
        set_sample_rate(i, sampling_rate);
    }

    /*
     * Check for the validity of the specified stream arguments on all enabled
     * channels
     */
    for (size_t chan : channs) {
        SoapySDR::ArgInfoList supported_args =
            d_device->getStreamArgsInfo(SOAPY_SDR_RX, chan);
        SoapySDR::Kwargs stream_kwargs = SoapySDR::KwargsFromString(stream_args);

        for (const std::pair<std::string, std::string>& i : stream_kwargs) {
            bool found = false;
            for (SoapySDR::ArgInfo& arg : supported_args) {
                if (arg.key == i.first) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::string msg = name() + ": Unsupported stream argument " + i.first +
                                  " for channel " + std::to_string(chan);
                throw std::invalid_argument(msg);
            }
        }
    }
    d_stream = d_device->setupStream(
        SOAPY_SDR_RX, stype, channs, SoapySDR::KwargsFromString(stream_args));
    d_mtu = d_device->getStreamMTU(d_stream);

    /* Apply tuning arguments on the enabled channels */
    for (size_t chan : channs) {
        SoapySDR::ArgInfoList supported_args =
            d_device->getFrequencyArgsInfo(SOAPY_SDR_RX, chan);
        SoapySDR::Kwargs tune_kwargs = SoapySDR::KwargsFromString(tune_args[chan]);

        for (const std::pair<std::string, std::string>& i : tune_kwargs) {
            bool found = false;
            for (SoapySDR::ArgInfo& arg : supported_args) {
                if (arg.key == i.first) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::string msg = name() + ": Unsupported tune argument " + i.first +
                                  " for channel " + std::to_string(chan);
                throw std::invalid_argument(msg);
            }
        }
        d_tune_args.push_back(tune_kwargs);
    }

    /* Apply any other channel specific setting */
    for (size_t chan : channs) {
        SoapySDR::ArgInfoList supported_settings =
            d_device->getSettingInfo(SOAPY_SDR_RX, chan);
        SoapySDR::Kwargs settings_kwargs =
            SoapySDR::KwargsFromString(other_settings[chan]);

        /*
         * Before applying the setting, make a last check if the setting
         * is a valid gain
         */
        std::vector<std::string> gains = d_device->listGains(SOAPY_SDR_RX, chan);
        for (const std::string& i : gains) {
            SoapySDR::Kwargs::iterator iter = settings_kwargs.find(i);
            if (iter != settings_kwargs.end()) {
                d_device->setGain(SOAPY_SDR_RX, chan, i, std::stod(iter->second));
                /*
                 * As only valid settings are applied, we remove it so it does not
                 * recognized as an invalid setting
                 */
                settings_kwargs.erase(iter);
            }
        }

        for (const std::pair<std::string, std::string>& i : settings_kwargs) {
            bool found = false;
            for (SoapySDR::ArgInfo& arg : supported_settings) {
                if (arg.key == i.first) {
                    found = true;
                    break;
                }
            }
            if (found) {
                d_device->writeSetting(SOAPY_SDR_RX, chan, i.first, i.second);
            } else {
                std::string msg = name() + ": Unsupported setting " + i.first +
                                  " for channel " + std::to_string(chan);
                throw std::invalid_argument(msg);
            }
        }
    }

    message_port_register_in(pmt::mp("command"));
    set_msg_handler(pmt::mp("command"),
                    boost::bind(&source_impl::msg_handler_command, this, _1));

    register_msg_cmd_handler(
        CMD_FREQ_KEY, boost::bind(&source_impl::cmd_handler_frequency, this, _1, _2));
    register_msg_cmd_handler(CMD_GAIN_KEY,
                             boost::bind(&source_impl::cmd_handler_gain, this, _1, _2));
    register_msg_cmd_handler(
        CMD_RATE_KEY, boost::bind(&source_impl::cmd_handler_samp_rate, this, _1, _2));
    register_msg_cmd_handler(CMD_BW_KEY,
                             boost::bind(&source_impl::cmd_handler_bw, this, _1, _2));
    register_msg_cmd_handler(
        CMD_ANTENNA_KEY, boost::bind(&source_impl::cmd_handler_antenna, this, _1, _2));

    /* This limits each work invocation to MTU transfers */
    if (d_mtu > 0) {
        set_max_noutput_items(d_mtu);
    } else {
        set_max_noutput_items(1024);
    }
}

bool source_impl::start()
{
    d_device->activateStream(d_stream);
    return true;
}

bool source_impl::stop()
{
    if (!d_stopped) {
        d_device->closeStream(d_stream);
        SoapySDR::Device::unmake(d_device);
        d_stopped = true;
    }
    return true;
}

source_impl::~source_impl()
{
    /*
     * Guarded with the d_stopped flag.
     * if \ref stop() has not yet called for releasing hardware etc,
     * it will be called by this destructor.
     *
     * If the scheduler has already called \ref stop() calling it again should have
     * no impact.
     */
    stop();
}

void source_impl::register_msg_cmd_handler(const pmt::pmt_t& cmd, cmd_handler_t handler)
{
    d_cmd_handlers[cmd] = handler;
}

void source_impl::set_frequency(size_t channel, double frequency)
{
    if (channel >= d_nchan) {
        std::string msg =
            name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    d_device->setFrequency(SOAPY_SDR_RX, channel, frequency, d_tune_args[channel]);
}

void source_impl::set_frequency(size_t channel, const std::string& name, double frequency)
{
    if (channel >= d_nchan) {
        std::string msg =
            this->name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    std::vector<std::string> freqs = d_device->listFrequencies(SOAPY_SDR_RX, channel);

    if (std::find(freqs.begin(), freqs.end(), name) == freqs.end()) {
        if (std::fpclassify(std::abs(frequency)) == FP_ZERO) {
            return;
        }
        std::string msg = this->name() + ": Channel " + std::to_string(channel) +
                          " does not support frequency " + name +
                          ". Consider setting this frequency to 0 to bypass this error";
        throw std::invalid_argument(msg);
    }

    d_device->setFrequency(SOAPY_SDR_RX, channel, name, frequency);
}

bool source_impl::gain_available(size_t channel, const std::string& name)
{
    std::vector<std::string> gains = d_device->listGains(SOAPY_SDR_RX, channel);

    if (std::find(gains.begin(), gains.end(), name) != gains.end()) {
        return true;
    }
    return false;
}

void source_impl::set_gain(size_t channel, float gain)
{
    if (channel >= d_nchan) {
        std::string msg =
            name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    SoapySDR::Range rGain = d_device->getGainRange(SOAPY_SDR_RX, channel);

    if (gain < rGain.minimum() || gain > rGain.maximum()) {
        GR_LOG_WARN(d_logger,
                    boost::format("Gain out of range: %d <= gain <= %d") %
                        rGain.minimum() % rGain.maximum());
        return;
    }

    d_device->setGain(SOAPY_SDR_RX, channel, gain);
}

void source_impl::set_gain(size_t channel, const std::string name, float gain)
{
    if (channel >= d_nchan) {
        return;
    }

    if (!gain_available(channel, name)) {
        std::string msg = this->name() + ": Unknown gain " + name + " for channel " +
                          std::to_string(channel);
        throw std::invalid_argument(msg);
    }

    SoapySDR::Range rGain = d_device->getGainRange(SOAPY_SDR_RX, channel, name);

    if (gain < rGain.minimum() || gain > rGain.maximum()) {
        GR_LOG_WARN(d_logger,
                    boost::format("Gain %s out of range: %d <= gain <= %d") % name %
                        rGain.minimum() % rGain.maximum());
    }

    d_device->setGain(SOAPY_SDR_RX, channel, name, gain);
}

void source_impl::set_agc(size_t channel, bool enable)
{
    if (channel >= d_nchan) {
        std::string msg =
            name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    if (!d_device->hasGainMode(SOAPY_SDR_RX, channel) && enable) {
        std::string msg =
            name() + ": Channel " + std::to_string(channel) + " does not support AGC";
        throw std::invalid_argument(msg);
    }
    d_device->setGainMode(SOAPY_SDR_RX, channel, enable);
}

void source_impl::set_sample_rate(size_t channel, double sample_rate)
{
    if (channel >= d_nchan) {
        std::string msg =
            name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    d_device->setSampleRate(SOAPY_SDR_RX, channel, sample_rate);
}

std::vector<std::string> source_impl::get_antennas(int channel)
{
    if ((size_t)channel >= d_nchan) {
        return std::vector<std::string>();
    }

    return d_device->listAntennas(SOAPY_SDR_RX, channel);
}

void source_impl::set_bandwidth(size_t channel, double bandwidth)
{
    if (channel >= d_nchan) {
        std::string msg =
            name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    d_device->setBandwidth(SOAPY_SDR_RX, channel, bandwidth);
}

void source_impl::set_antenna(const size_t channel, const std::string& name)
{
    if (channel >= d_nchan) {
        std::string msg =
            this->name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    std::vector<std::string> antennas = d_device->listAntennas(SOAPY_SDR_RX, channel);

    if (antennas.size() > 0) {
        if (std::find(antennas.begin(), antennas.end(), name) == antennas.end()) {
            std::string msg = this->name() + ": Antenna " + name + " at channel " +
                              std::to_string(channel) + " is not supported. " +
                              "Available antennas are: [";
            for (std::string i : antennas) {
                msg += i + ", ";
            }
            msg.erase(msg.length() - 2, 2);
            msg += "]";
            throw std::invalid_argument(msg);
            return;
        }
    }

    d_device->setAntenna(SOAPY_SDR_RX, channel, name);
}

void source_impl::set_dc_offset(size_t channel, gr_complexd dc_offset)
{
    if (channel >= d_nchan) {
        std::string msg =
            this->name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    const bool supported = d_device->hasDCOffset(SOAPY_SDR_RX, channel);
    /*
     * In case of 0 DC offset, the method exits to avoid failing in case of the
     * device does not supports DC offset
     */
    if (std::fpclassify(std::norm(dc_offset)) == FP_ZERO) {
        if (supported && !d_device->getDCOffsetMode(SOAPY_SDR_RX, channel)) {
            d_device->setDCOffset(SOAPY_SDR_RX, channel, dc_offset);
        }
        return;
    }

    if (!supported) {
        std::string msg = this->name() + ": Channel " + std::to_string(channel) +
                          " does not support DC offset setting";
        throw std::invalid_argument(msg);
    }
    d_device->setDCOffset(SOAPY_SDR_RX, channel, dc_offset);
}

void source_impl::set_dc_removal(size_t channel, bool automatic)
{
    if (channel >= d_nchan) {
        std::string msg =
            this->name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    if (!d_device->hasDCOffsetMode(SOAPY_SDR_RX, channel) && automatic) {
        std::string msg = this->name() + ": Channel " + std::to_string(channel) +
                          " does not support automatic DC removal setting";
        throw std::invalid_argument(msg);
    }
    d_device->setDCOffsetMode(SOAPY_SDR_RX, channel, automatic);
}

void source_impl::set_frequency_correction(size_t channel, double freq_correction)
{
    if (channel >= d_nchan) {
        std::string msg =
            this->name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    const bool supported = d_device->hasFrequencyCorrection(SOAPY_SDR_RX, channel);
    /*
     * In case of 0 frequency correction, the method exits to avoid failing
     * in case of the device does not supports DC offset
     */
    if (std::fpclassify(std::abs(freq_correction)) == FP_ZERO) {
        if (supported) {
            d_device->setFrequencyCorrection(SOAPY_SDR_RX, channel, freq_correction);
        }
        return;
    }

    if (!supported) {
        std::string msg = this->name() + ": Channel " + std::to_string(channel) +
                          " does not support frequency correction setting";
        throw std::invalid_argument(msg);
    }
    d_device->setFrequencyCorrection(SOAPY_SDR_RX, channel, freq_correction);
}

void source_impl::set_iq_balance(size_t channel, gr_complexd iq_balance)
{
    if (channel >= d_nchan) {
        std::string msg =
            this->name() + ": Channel " + std::to_string(channel) + " is not activated.";
        throw std::invalid_argument(msg);
    }

    const bool supported = d_device->hasIQBalance(SOAPY_SDR_RX, channel);
    /*
     * In case of 0 frequency correction, the method exits to avoid failing
     * in case of the device does not supports DC offset
     */
    if (std::fpclassify(std::norm(iq_balance)) == FP_ZERO) {
        if (supported) {
            d_device->setIQBalance(SOAPY_SDR_RX, channel, iq_balance);
        }
        return;
    }

    if (!supported) {
        std::string msg = this->name() + ": Channel " + std::to_string(channel) +
                          " does not support IQ imbalance correction setting";
        throw std::invalid_argument(msg);
    }
    d_device->setIQBalance(SOAPY_SDR_RX, channel, iq_balance);
}

void source_impl::set_master_clock_rate(double clock_rate)
{
    d_device->setMasterClockRate(clock_rate);
}

void source_impl::set_clock_source(const std::string& clock_source)
{
    d_device->setClockSource(clock_source);
}

void source_impl::set_frontend_mapping(const std::string& mapping)
{
    d_device->setFrontendMapping(SOAPY_SDR_RX, mapping);
}

double source_impl::get_frequency(size_t channel)
{
    return d_device->getFrequency(SOAPY_SDR_RX, channel);
}

double source_impl::get_gain(size_t channel)
{
    return d_device->getGain(SOAPY_SDR_RX, channel);
}

bool source_impl::get_gain_mode(size_t channel)
{
    return d_device->getGainMode(SOAPY_SDR_RX, channel);
}

double source_impl::get_sampling_rate(size_t channel)
{
    return d_device->getSampleRate(SOAPY_SDR_RX, channel);
}

double source_impl::get_bandwidth(size_t channel)
{
    return d_device->getBandwidth(SOAPY_SDR_RX, channel);
}

std::string source_impl::get_antenna(size_t channel)
{
    return d_device->getAntenna(SOAPY_SDR_RX, channel);
}

std::complex<double> source_impl::get_dc_offset(size_t channel)
{
    return d_device->getDCOffset(SOAPY_SDR_RX, channel);
}

bool source_impl::get_dc_offset_mode(size_t channel)
{
    return d_device->getDCOffsetMode(SOAPY_SDR_RX, channel);
}

double source_impl::get_frequency_correction(size_t channel)
{
    return d_device->getFrequencyCorrection(SOAPY_SDR_RX, channel);
}

std::complex<double> source_impl::get_iq_balance(size_t channel)
{
    return d_device->getIQBalance(SOAPY_SDR_RX, channel);
}

double source_impl::get_master_clock_rate() { return d_device->getMasterClockRate(); }

std::string source_impl::get_clock_source() { return d_device->getClockSource(); }

void source_impl::cmd_handler_frequency(pmt::pmt_t val, size_t chann)
{
    set_frequency(chann, pmt::to_float(val));
}

void source_impl::cmd_handler_gain(pmt::pmt_t val, size_t chann)
{
    set_gain(chann, pmt::to_float(val));
}

void source_impl::cmd_handler_samp_rate(pmt::pmt_t val, size_t chann)
{
    set_sample_rate(chann, pmt::to_float(val));
}

void source_impl::cmd_handler_bw(pmt::pmt_t val, size_t chann)
{
    set_bandwidth(chann, pmt::to_float(val));
}

void source_impl::cmd_handler_antenna(pmt::pmt_t val, size_t chann)
{
    set_antenna(chann, pmt::symbol_to_string(val));
}

int source_impl::work(int noutput_items,
                      gr_vector_const_void_star& input_items __attribute__((unused)),
                      gr_vector_void_star& output_items)
{
    long long int time_ns = 0;
    int flags = 0;
    while (1) {
        boost::this_thread::disable_interruption disable_interrupt;
        int read = d_device->readStream(
            d_stream, output_items.data(), noutput_items, flags, time_ns);
        boost::this_thread::restore_interruption restore_interrupt(disable_interrupt);
        if (read >= 0) {
            return read;
        }

        if (read < 0) {
            // Added some error handling
            switch (read) {
            case SOAPY_SDR_OVERFLOW:
                std::cout << "sO";
                break;
            case SOAPY_SDR_UNDERFLOW:
                std::cout << "sU";
                break;
            case SOAPY_SDR_STREAM_ERROR:
                GR_LOG_WARN(d_logger, boost::format("Block stream error."));
                return 0;
                ;
            case SOAPY_SDR_TIMEOUT:
                break;
            case SOAPY_SDR_CORRUPTION:
                GR_LOG_WARN(d_logger, boost::format("Block corruption."));
                return 0;
            default:
                GR_LOG_WARN(d_logger,
                            boost::format("Block caught RX error code: %d") % read);
                return 0;
            }
        }
    }
    return 0;
}

void source_impl::msg_handler_command(pmt::pmt_t msg)
{
    if (!pmt::is_dict(msg)) {
        return;
    }
    size_t chann = 0;
    if (pmt::dict_has_key(msg, CMD_CHAN_KEY)) {
        chann = pmt::to_long(pmt::dict_ref(msg, CMD_CHAN_KEY, pmt::from_long(0)));
        pmt::dict_delete(msg, CMD_CHAN_KEY);
    }
    for (size_t i = 0; i < pmt::length(msg); i++) {
        d_cmd_handlers[pmt::car(pmt::nth(i, msg))](pmt::cdr(pmt::nth(i, msg)), chann);
    }
}
} /* namespace soapy */
} /* namespace gr */
