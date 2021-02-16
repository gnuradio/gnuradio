/* -*- c++ -*- */
/*
 * gr-soapy: Soapy SDR Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, 2019, 2020
 *  Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SOAPY_SOURCE_IMPL_H
#define INCLUDED_SOAPY_SOURCE_IMPL_H

#include <soapy/source.h>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Device.hpp>

typedef boost::function<void(pmt::pmt_t, size_t)> cmd_handler_t;

namespace gr {
namespace soapy {

/*!
 * \brief Source block implementation for SDR devices.
 */

class source_impl : public source {
private:
  const std::string d_dev_str;
  const std::string d_args;

  SoapySDR::Device *d_device;
  SoapySDR::Stream *d_stream;

  bool d_stopped;
  size_t d_mtu;
  size_t d_nchan;
  std::map<pmt::pmt_t, cmd_handler_t> d_cmd_handlers;
  std::vector<SoapySDR::Kwargs> d_tune_args;

  void register_msg_cmd_handler(const pmt::pmt_t &cmd, cmd_handler_t handler);

  inline io_signature::sptr
  args_to_io_sig(const std::string type, size_t nchan)
  {
    size_t size = 0;
    if (type == "fc32") {
      size = 8;
    }
    else if (type == "sc16") {
      size = 4;
    }
    else if (type == "sc8") {
      size = 2;
    }
    else if (type == "s16") {
      size = 2;
    }
    else if (type == "s8") {
      size = 1;
    }
    return io_signature::make(nchan, nchan, size);
  }

public:
  source_impl(size_t nchan,
              const std::string &device,
              const std::string &dev_args,
              const std::string &stream_args,
              const std::vector<std::string> &tune_args,
              const std::vector<std::string> &other_settings,
              double sampling_rate,
              const std::string &type);
  ~source_impl();

  virtual bool start();
  virtual bool stop();

  // Where all the action really happens
  int work(int noutput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

  std::vector<std::string> get_antennas(int channel);

  /*!
   * Set the center frequency for the specified RX chain.
   * Default implementation tunes RF component frequency as close as
   * possible to the requested frequency. See specific device module
   * for more information.
   * \param channel an available channel on the device
   * \param frequency center frequency in Hz
   */
  void set_frequency(size_t channel, double frequency);

  /*!
   * Set the center frequency for the specified RX chain of the element.
   * Default implementation tunes RF component frequency as close as
   * possible to the requested frequency. See specific device module
   * for more information
   * \param channel an available channel on the device
   * \param name an available element name
   * \param frequency center frequency in Hz
   */
  void set_frequency(size_t channel, const std::string &name, double frequency);

  /*!
    * Set the overall gain for the specified RX chain.
    * The gain will be distributed automatically across available
    * elements according to Soapy API.
    * \param channel an available channel on the device
    * \param gain the new amplification value in dB
    */
  void set_gain(size_t channel, float gain);

  /*!
   * Set the value for the specified gain for the specified TX chain.
   * \param channel an available channel on the device
   * \param name an available gain on the device
   * \param gain gain the new amplification value in dB
   */
  void set_gain(size_t channel, const std::string name, float gain);


  /**
   * Checks if the specified gain type for the given channel is
   * available
   * @param channel an available channel on the device
   * @param nane an available gain on the device
   * @return true if the gain setting exists, false otherwise
   */
  bool gain_available(size_t channel, const std::string &name);

  /*!
   * Set the automatic gain mode for the specified chain if supported.
   * If not supported set gain value manually.
   * \param channel an available channel on the device
   * \param enable true for automatic gain mode
   */
  void set_agc(size_t channel, bool enable);

  /*!
   * Set the baseband sample rate for the RX chain.
   * \param channel an available channel on the device
   * \param sample_rate the sample rate samples per second
   */
  void set_sample_rate(size_t channel, double sample_rate);

  /*!
   * Set the baseband filter width of the RX chain
   * \param channel an available channel on the device
   * \param bandwidth the baseband filter width in Hz
   */
  void set_bandwidth(size_t channel, double bandwidth);

  /*!
   * Set the antenna element for the RX chain.
   * \param channel an available channel on the device
   * \param name the name of an available antenna
   */
  void set_antenna(size_t channel, const std::string &name);

  /*!
   * Set the dc offset correction for the RX chain.
   * If the dc offset correction automatic mode is on
   * the value is omitted and the device sets the dc offset
   * correction automatically.
   * \param channel an available channel on the device
   * \param dc_offset the relative correction (1.0 max)
   */
  void set_dc_offset(size_t channel, gr_complexd dc_offset);

  /*!
   * Set automatic DC removal to the RX chain
   * if supported by the device.
   * \param channel an available channel on the device
   * \param automatic true for automatic DC offset correction
   */
  void set_dc_removal(size_t channel, bool automatic);

  /*!
   * Set the frequency correction to the RX chain.
   * @param channel an available channel on the device
   * @param freq_correction the correction value in PPM
   */
  void set_frequency_correction(size_t channel, double freq_correction);

  /*!
   * Set iq balance correction to the RX chain
   * \param channel an available channel on the device
   * \param iq_balance the relative correction (1.0 max)
   */
  void set_iq_balance(size_t channel, gr_complexd iq_balance);

  /*!
   * Set the master clock rate of the device
   * \param clock_rate the clock rate in Hz
   */
  void set_master_clock_rate(double clock_rate);

  /*!
   * Set the clock source of the device
   * \param clock_source the name of clock source
   */
  void set_clock_source(const std::string &clock_source);

  /*!
   * Set the frontend mapping of available DSP units to RF frontends.
   * This mapping controls channel mapping and channel availability.
   * \param frontend_mapping a vendor-specific mapping string
   */
  void set_frontend_mapping(const std::string &frontend_mapping);

  /*!
   * Get the down conversion frequency of the chain.
   * \param channel an available channel on the device
   * \return the center frequency in Hz
   */
  double get_frequency(size_t channel);

  /*!
   * Get the overall value of the gain elements in a chain
   * \param channel an available channel on the device
   * \return the value of the gain in dB
   */
  double get_gain(size_t channel);

  /*!
   * Get the automatic gain mode on the RX chain.
   * \param channel an available channel on the device
   * \return true for automatic gain setting
   */
  bool get_gain_mode(size_t channel);

  /*!
   * Get the baseband sample rate of the RX chain.
   * \param channel an available channel on the device
   * \return the sample rate in samples per second
   */
  double get_sampling_rate(size_t channel);

  /*!
   * Get baseband filter width of the RX chain.
   * \param channel an available channel on the device
   * \return the baseband filter width in Hz
   */
  double get_bandwidth(size_t channel);

  /*!
   * Get the selected antenna on RX chain.
   * \param channel an available channel on the device
   * \return the name of the selected antenna
   */
  std::string get_antenna(size_t channel);

  /*!
   * Get the DC offset correction.
   * \param channel an available channel on the device
   * \return the relative correction (1.0 max)
   */
  std::complex<double> get_dc_offset(size_t channel);

  /*!
   * Get the automatic DC offset correction mode.
   * \param channel an available channel on the device
   * \return true for automatic offset correction
   */
  bool get_dc_offset_mode(size_t channel);

  /*!
   * Get the frequency correction value.
   * \param channel an available channel on the device
   * \return the correction value in PPM
   */
  double get_frequency_correction(size_t channel);

  /*!
   * Get the IQ balance correction.
   * \param channel an available channel on the device
   * \return the relative correction (1.0 max)
   */
  std::complex<double> get_iq_balance(size_t channel);

  /*!
   * Get the master clock rate of the device.
   * \return the clock rate in Hz
   */
  double get_master_clock_rate();

  /*!
   * Get the clock source of the device
   * \return the name of the clock source
   */
  std::string get_clock_source();

  /*!
   * Calls the correct message handler according to the received message symbol.
   * A dictionary with key the handler name is used in order to call the
   * corresponding handler.
   * \param msg a PMT dictionary
   */
  void msg_handler_command(pmt::pmt_t msg);

  /*!
   * Set the center frequency of the RX chain.
   * @param val center frequency in Hz
   * @param chann an available channel on the device
   */
  void cmd_handler_frequency(pmt::pmt_t val, size_t chann);

  /*!
   * Set the overall gain for the specified chain.
   * The gain will be distributed automatically across available
   * elements according to Soapy API.
   * @param val the new amplification value in dB
   * @param chann an avalaible channel on the device
   */
  void cmd_handler_gain(pmt::pmt_t val, size_t chann);

  /*!
   * Set the baseband sample rate for the RX chain.
   * @param val the sample rate samples per second
   * @param chann an available channel on the device
   */
  void cmd_handler_samp_rate(pmt::pmt_t val, size_t chann);

  /*!
   * Set the baseband filter width for the RX chain.
   * @param val baseband filter width in Hz
   * @param chann an available channel on the device
   */
  void cmd_handler_bw(pmt::pmt_t val, size_t chann);

  /*!
   * Set the anntena element for the RX chain.
   * @param val name of the anntena
   * @param chann an available channel on the device
   */
  void cmd_handler_antenna(pmt::pmt_t val, size_t chann);
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_SOAPY_SOURCE_IMPL_H */
