/* -*- c++ -*- */
/*
 * gr-soapy: Soapy SDR Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, 2019
 *  Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sink_impl.h"
#include <gnuradio/io_signature.h>
#include <SoapySDR/Formats.h>

const pmt::pmt_t CMD_CHAN_KEY = pmt::mp("chan");
const pmt::pmt_t CMD_FREQ_KEY = pmt::mp("freq");
const pmt::pmt_t CMD_GAIN_KEY = pmt::mp("gain");
const pmt::pmt_t CMD_ANTENNA_KEY = pmt::mp("antenna");
const pmt::pmt_t CMD_RATE_KEY = pmt::mp("samp_rate");
const pmt::pmt_t CMD_BW_KEY = pmt::mp("bw");

namespace gr {
namespace soapy {

sink::sptr sink::make(size_t nchan,
                      const std::string &device,
                      const std::string &args,
                      double sampling_rate,
                      const std::string &type,
                      const std::string &length_tag_name)
{
  return gnuradio::make_block_sptr<sink_impl>(
           nchan, device, args, sampling_rate, type, length_tag_name);
}


/*
 * The private constructor
 */
sink_impl::sink_impl(size_t nchan,
                     const std::string &device,
                     const std::string &args,
                     double sampling_rate,
                     const std::string &type,
                     const std::string &length_tag_name)
  : gr::sync_block("sink",
                   args_to_io_sig(type, nchan),
                   gr::io_signature::make(0, 0, 0)),
    d_stopped(true),
    d_message_port(pmt::mp("command")),
    d_sampling_rate(sampling_rate),
    d_nchan(nchan),
    d_type(type),
    d_length_tag_key(length_tag_name.empty() ? pmt::PMT_NIL
                     : pmt::string_to_symbol(length_tag_name)),
    d_burst_remaining(0)
{
  if (type == "fc32") {
    d_type_size = 8;
    d_type = SOAPY_SDR_CF32;
  }
  else if (type == "sc16") {
    d_type_size = 4;
    d_type = SOAPY_SDR_CS16;
  }
  else if (type == "sc8") {
    d_type_size = 2;
    d_type = SOAPY_SDR_CS8;
  }

  /* "serial" device arguement needs special handling. */
  std::string dev_str = device;
  SoapySDR::Kwargs dev_args = SoapySDR::KwargsFromString(args);
  const SoapySDR::Kwargs::const_iterator iter = dev_args.find("serial");
  if (iter != dev_args.end()) {
    dev_str += ",serial=" + iter->second;
    dev_args.erase(iter->first);
  }

  d_device = SoapySDR::Device::make(dev_str);
  d_stopped = false;
  if (d_nchan > d_device->getNumChannels(SOAPY_SDR_TX)) {
    std::string msg = name() +  ": Unsupported number of channels. Only  "
                      + std::to_string(d_device->getNumChannels(SOAPY_SDR_TX))
                      + " channels available.";
    throw std::invalid_argument(msg);
  }

  std::vector<size_t> channs;
  channs.resize(d_nchan);
  for (size_t i = 0; i < d_nchan; i++) {
    channs[i] = i;

    SoapySDR::RangeList sampRange = d_device->getSampleRateRange(SOAPY_SDR_TX, i);

    double minRate = sampRange.front().minimum();
    double maxRate = sampRange.back().maximum();

    if ((d_sampling_rate < minRate)
        || (d_sampling_rate > maxRate)) {
      std::string msg = name() + ": Unsupported sample rate. Rate must be between "
                        + std::to_string(sampRange.front().minimum()) + " and "
                        + std::to_string(sampRange.back().maximum());
      throw std::invalid_argument(msg);
    }
    set_sample_rate(i, d_sampling_rate);
  }


  /* Apply to all streams the supported args */
  SoapySDR::ArgInfoList supported_args =
    d_device->getStreamArgsInfo(SOAPY_SDR_TX, 0);
  SoapySDR::Kwargs stream_args;
  for (const SoapySDR::ArgInfo &i : supported_args) {
    const SoapySDR::Kwargs::const_iterator iter = dev_args.find(i.key);
    if (iter != dev_args.end()) {
      stream_args[iter->first] = iter->second;
      /*
       * This was a stream argument. Erase it so it cannot be applied later
       * as device argument
       */
      dev_args.erase(i.key);
    }
  }

  d_stream = d_device->setupStream(SOAPY_SDR_TX, d_type, channs, stream_args);
  size_t mtu = d_device->getStreamMTU(d_stream);

  /*
   * Apply device settings to all enabled channels.
   * These should be any settings that were not a stream argument
   */
  SoapySDR::ArgInfoList supported_dev_args = d_device->getSettingInfo();
  for (const SoapySDR::ArgInfo &i : supported_args) {
    std::cout << i.description << std::endl;
  }

  for (size_t chan : channs) {
    for (const std::pair<std::string, std::string> &iter : dev_args) {
      d_device->writeSetting(SOAPY_SDR_TX, chan, iter.first, iter.second);
    }
  }

  message_port_register_in(d_message_port);
  set_msg_handler(d_message_port,
                  boost::bind(&sink_impl::msg_handler_command, this, _1));

  register_msg_cmd_handler(
    CMD_FREQ_KEY,
    boost::bind(&sink_impl::cmd_handler_frequency, this, _1, _2));
  register_msg_cmd_handler(
    CMD_GAIN_KEY,
    boost::bind(&sink_impl::cmd_handler_gain, this, _1, _2));
  register_msg_cmd_handler(
    CMD_RATE_KEY,
    boost::bind(&sink_impl::cmd_handler_samp_rate, this, _1, _2));
  register_msg_cmd_handler(
    CMD_BW_KEY, boost::bind(&sink_impl::cmd_handler_bw, this, _1, _2));
  register_msg_cmd_handler(
    CMD_ANTENNA_KEY,
    boost::bind(&sink_impl::cmd_handler_antenna, this, _1, _2));

  /*
   * Limit max samples per call to MTU, however some devices like PlutoSDR
   * does not support this. So we rely on these module to properly handle
   * a larger data transfer
   */
  if (mtu > 0) {
    set_max_noutput_items(mtu);
  }
}

bool
sink_impl::start()
{
  d_device->activateStream(d_stream);
  return true;
}

bool
sink_impl::stop()
{
  if (!d_stopped) {
    SoapySDR::Device::unmake(d_device);
    d_stopped = true;
  }
  return true;
}

/*
 * Our virtual destructor.
 */
sink_impl::~sink_impl()
{
  stop();
}

void
sink_impl::register_msg_cmd_handler(const pmt::pmt_t &cmd,
                                    cmd_handler_t handler)
{
  d_cmd_handlers[cmd] = handler;
}

bool
sink_impl::DC_offset_support(int channel)
{
  return d_device->hasDCOffset(SOAPY_SDR_TX, channel);
}

bool
sink_impl::IQ_balance_support(int channel)
{
  return d_device->hasIQBalance(SOAPY_SDR_TX, channel);
}

bool
sink_impl::freq_correction_support(int channel)
{
  return d_device->hasFrequencyCorrection(SOAPY_SDR_TX, channel);
}

void
sink_impl::set_frequency(size_t channel, double frequency)
{
  if (channel >= d_nchan) {
    return;
  }

  d_device->setFrequency(SOAPY_SDR_TX, channel, frequency);
  d_frequency = d_device->getFrequency(SOAPY_SDR_TX, channel);
}

void
sink_impl::set_frequency(size_t channel, const std::string &name,
                         double frequency)
{
  if (channel >= d_nchan) {
    return;
  }

  d_device->setFrequency(SOAPY_SDR_TX, channel, name, frequency);
}

bool
sink_impl::is_gain_valid(size_t channel, std::string gainType)
{
  std::vector<std::string> gainList = d_device->listGains(SOAPY_SDR_TX, channel);

  if (std::find(gainList.begin(), gainList.end(), gainType) != gainList.end()) {
    return true;
  }
  else {
    return false;
  }
}

void
sink_impl::set_gain(size_t channel, const std::string name, float gain)
{
  if (channel >= d_nchan) {
    return;
  }

  if (!is_gain_valid(channel, name)) {
    return;
  }

  SoapySDR::Range rGain = d_device->getGainRange(SOAPY_SDR_TX, channel, name);

  if (gain < rGain.minimum() || gain > rGain.maximum()) {
    GR_LOG_WARN(d_logger,
                boost::format("Gain %s out of range: %d <= gain <= %d") %
                name % rGain.minimum() % rGain.maximum());
  }

  d_device->setGain(SOAPY_SDR_TX, channel, name, gain);
  d_gain = d_device->getGain(SOAPY_SDR_TX, channel);
}

void
sink_impl::set_gain(size_t channel, float gain)
{
  if (channel >= d_nchan) {
    return;
  }

  SoapySDR::Range rGain = d_device->getGainRange(SOAPY_SDR_TX, channel);

  if (gain < rGain.minimum() || gain > rGain.maximum()) {
    GR_LOG_WARN(d_logger,
                boost::format("Gain out of range: %d <= gain <= %d") %
                rGain.minimum() % rGain.maximum());
    return;
  }

  d_device->setGain(SOAPY_SDR_TX, channel, gain);
  d_gain = d_device->getGain(SOAPY_SDR_TX, channel);
}

void
sink_impl::set_gain_mode(size_t channel, bool gain_auto_mode)
{
  if (channel >= d_nchan) {
    return;
  }

  d_device->setGainMode(SOAPY_SDR_TX, channel, gain_auto_mode);
}

void
sink_impl::set_sample_rate(size_t channel, double sample_rate)
{
  if (channel >= d_nchan) {
    return;
  }

  d_device->setSampleRate(SOAPY_SDR_TX, channel, sample_rate);
}

void
sink_impl::set_bandwidth(size_t channel, double bandwidth)
{
  if (channel >= d_nchan) {
    return;
  }

  d_device->setBandwidth(SOAPY_SDR_TX, channel, bandwidth);
}

std::vector<std::string> sink_impl::listAntennas(int channel)
{
  if ((size_t)channel >= d_nchan) {
    return std::vector<std::string>();
  }
  return d_device->listAntennas(SOAPY_SDR_TX, channel);
}

void
sink_impl::set_antenna(const size_t channel, const std::string &name)
{
  if (channel >= d_nchan) {
    return;
  }

  std::vector<std::string> antennaList = d_device->listAntennas(SOAPY_SDR_TX,
                                         channel);

  if (antennaList.size() > 0) {
    if (std::find(antennaList.begin(), antennaList.end(),
                  name) == antennaList.end()) {
      GR_LOG_WARN(d_logger,
                  boost::format("Antenna name %s not supported.") % name);
      return;
    }
  }

  d_device->setAntenna(SOAPY_SDR_TX, channel, name);
  d_antenna = name;
}

void
sink_impl::set_dc_offset(size_t channel, gr_complexd dc_offset,
                         bool dc_offset_auto_mode)
{
  if (channel >= d_nchan) {
    return;
  }

  if (!DC_offset_support(channel)) {
    return;
  }

  /* If DC Correction is supported but automatic mode is not set DC correction */
  if (!dc_offset_auto_mode
      && d_device->hasDCOffset(SOAPY_SDR_TX, channel)) {
    d_device->setDCOffset(SOAPY_SDR_TX, channel, dc_offset);
    d_dc_offset = dc_offset;
  }
}

void
sink_impl::set_dc_offset_mode(size_t channel, bool dc_offset_auto_mode)
{
  if (channel >= d_nchan) {
    return;
  }

  if (!DC_offset_support(channel)) {
    return;
  }

  /* If user specifies automatic DC Correction and is supported activate it */
  if (dc_offset_auto_mode
      && d_device->hasDCOffsetMode(SOAPY_SDR_TX, channel)) {
    d_device->setDCOffsetMode(SOAPY_SDR_TX, channel, dc_offset_auto_mode);
    d_dc_offset_auto_mode = dc_offset_auto_mode;
  }
}

void
sink_impl::set_frequency_correction(size_t channel, double freq_correction)
{
  if (channel >= d_nchan) {
    return;
  }

  if (!freq_correction_support(channel)) {
    return;
  }

  /* If the device supports Frequency correction set value */
  if (d_device->hasFrequencyCorrection(SOAPY_SDR_TX, channel)) {
    d_device->setFrequencyCorrection(SOAPY_SDR_TX, channel,
                                     freq_correction);
    d_frequency_correction = freq_correction;
  }
}

void
sink_impl::set_iq_balance(size_t channel, gr_complexd iq_balance)
{
  if (channel >= d_nchan) {
    return;
  }

  if (!IQ_balance_support(channel)) {
    return;
  }

  /* If the device supports IQ blance correction set value */
  if (d_device->hasIQBalance(SOAPY_SDR_TX, channel)) {
    d_device->setIQBalance(SOAPY_SDR_TX, channel, iq_balance);
    d_iq_balance = iq_balance;
  }
}

void
sink_impl::set_master_clock_rate(double clock_rate)
{
  d_device->setMasterClockRate(clock_rate);
  d_clock_rate = clock_rate;
}

void
sink_impl::set_clock_source(const std::string &clock_source)
{
  d_device->setClockSource(clock_source);
  d_clock_source = clock_source;
}

void
sink_impl::set_frontend_mapping(const std::string &mapping)
{
  d_device->setFrontendMapping(SOAPY_SDR_TX, mapping);
}

double
sink_impl::get_frequency(size_t channel)
{
  return d_device->getFrequency(SOAPY_SDR_TX, channel);
}

double
sink_impl::get_gain(size_t channel)
{
  return d_device->getGain(SOAPY_SDR_TX, channel);
}

bool
sink_impl::get_gain_mode(size_t channel)
{
  return d_device->getGainMode(SOAPY_SDR_TX, channel);
}

double
sink_impl::get_sampling_rate(size_t channel)
{
  return d_device->getSampleRate(SOAPY_SDR_TX, channel);
}

double
sink_impl::get_bandwidth(size_t channel)
{
  return d_device->getBandwidth(SOAPY_SDR_TX, channel);
}

std::string
sink_impl::get_antenna(size_t channel)
{
  return d_device->getAntenna(SOAPY_SDR_TX, channel);
}

std::complex<double>
sink_impl::get_dc_offset(size_t channel)
{
  return d_device->getDCOffset(SOAPY_SDR_TX, channel);
}

bool
sink_impl::get_dc_offset_mode(size_t channel)
{
  return d_device->getDCOffsetMode(SOAPY_SDR_TX, channel);
}

double
sink_impl::get_frequency_correction(size_t channel)
{
  return d_device->getFrequencyCorrection(SOAPY_SDR_TX, channel);
}

std::complex<double>
sink_impl::get_iq_balance(size_t channel)
{
  return d_device->getIQBalance(SOAPY_SDR_TX, channel);
}

double
sink_impl::get_master_clock_rate()
{
  return d_device->getMasterClockRate();
}

std::string
sink_impl::get_clock_source()
{
  return d_device->getClockSource();
}

void
sink_impl::cmd_handler_frequency(pmt::pmt_t val, size_t chann)
{
  set_frequency(chann, pmt::to_float(val));
}

void
sink_impl::cmd_handler_gain(pmt::pmt_t val, size_t chann)
{
  set_gain(chann, pmt::to_float(val));
}

void
sink_impl::cmd_handler_samp_rate(pmt::pmt_t val, size_t chann)
{
  set_sample_rate(chann, pmt::to_float(val));
}

void
sink_impl::cmd_handler_bw(pmt::pmt_t val, size_t chann)
{
  set_bandwidth(chann, pmt::to_float(val));
}

void
sink_impl::cmd_handler_antenna(pmt::pmt_t val, size_t chann)
{
  set_antenna(chann, pmt::symbol_to_string(val));
}

int sink_impl::work(int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
{
  int ninput_items = noutput_items;
  int flags = 0;
  long long timeNs = 0;

  // Handle tags
  if (!pmt::is_null(d_length_tag_key)) {
    // Here we update d_burst_remaining when we find a length tag
    this->tag_work(noutput_items);
    // If a burst is active, check if we finish it on this call
    if (d_burst_remaining > 0) {
      ninput_items = std::min<long>(d_burst_remaining, ninput_items);
      if (d_burst_remaining == long(ninput_items)) {
        flags |= SOAPY_SDR_END_BURST;
      }
    }
    else {
      // No new length tag was found immediately following previous burst
      // Drop samples until next length tag is found, and notify of tag gap
      std::cerr << "tG" << std::flush;
      return ninput_items;
    }
  }

  boost::this_thread::disable_interruption disable_interrupt;
  int write = d_device->writeStream(d_stream, &input_items[0], ninput_items,
                                    flags, timeNs);
  boost::this_thread::restore_interruption restore_interrupt(disable_interrupt);

  // Tell runtime system how many output items we produced.
  if (write < 0) {
    return 0;
  }
  if (d_burst_remaining > 0) {
    d_burst_remaining -= write;
  }
  return write;
}

void
sink_impl::tag_work(int noutput_items)
{
  std::vector<tag_t> length_tags;
  get_tags_in_window(length_tags, 0, 0, noutput_items, d_length_tag_key);
  for (const tag_t &tag : length_tags) {
    // If there are still items left to send, the current burst has been preempted.
    // Set the items remaining counter to the new burst length. Notify the user of
    // the tag preemption.
    if (d_burst_remaining > 0) {
      std::cerr << "tP" << std::flush; // tag preempted
    }
    d_burst_remaining = pmt::to_long(tag.value);
    break;
  }
}

void
sink_impl::msg_handler_command(pmt::pmt_t msg)
{
  if (!pmt::is_dict(msg)) {
    return;
  }
  size_t chann = 0;
  if (pmt::dict_has_key(msg, CMD_CHAN_KEY)) {
    chann = pmt::to_long(
              pmt::dict_ref(msg, CMD_CHAN_KEY, pmt::from_long(0)));
    pmt::dict_delete(msg, CMD_CHAN_KEY);
  }
  for (size_t i = 0; i < pmt::length(msg); i++) {
    d_cmd_handlers[pmt::car(pmt::nth(i, msg))](
      pmt::cdr(pmt::nth(i, msg)), chann);
  }
}
} /* namespace soapy */
} /* namespace gr */
