/* -*- c++ -*- */
/*
 * gr-soapy: Soapy SDR Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018
 *  Libre Space Foundation <http://librespacefoundation.org/>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SOAPY_SOURCE_H
#define INCLUDED_SOAPY_SOURCE_H

#include <gnuradio/sync_block.h>
#include <soapy/api.h>
#include <cstdint>
#include <vector>
#include <string>

namespace gr {
namespace soapy {

/*!
 * \addtogroup block
 * \brief <b>Source</b> block implements SoapySDR functionality for RX.
 * \ingroup soapy
 * \section source Soapy Source
 * The soapy source block receives samples and writes to a stream.
 * The source block also provides Soapy API calls for receiver settings.
 * Includes all parameters for full RX implementation.
 * Device is a string containing the driver and type name of the
 * device the user wants to use according to the Soapy* module
 * documentation.
 * Make parameters are passed through the xml block.
 * Some of the available parameters can be seen at Figure 2
 * Antenna and clock source can be left empty and default values
 * will be used.
 * This block has a message port, which consumes PMT messages.
 * For a description of the command syntax , see \ref cmd_handler.
 * \image html source_params.png "Figure 2"
 */
class SOAPY_API source : virtual public gr::sync_block {
public:
  typedef std::shared_ptr<source> sptr;

  /*!
   * \brief Return a shared_ptr to a new instance of soapy::source.
   *
   * To avoid accidental use of raw pointers, soapy::source's
   * constructor is in a private implementation
   * class. soapy::source::make is the public interface for
   * creating new instances.
   * \param nchan number of channels
   * \param device the device driver and type
   * \param dev_args device specific arguments
   * \param stream_args stream arguments. Same for all enabled channels
   * \param tune_args list with tuning specific arguments, one entry for every
   * enabled channel
   * \param other_settings list with general settings, one entry for every
   * enabled channel. Supports also specific gain settings.
   * \param sampling_rate the sampling rate of the device
   * \param type output stream format
   *
   * Driver name can be any of "uhd", "lime", "airspy",
   * "rtlsdr" or others
   */
  static sptr make(size_t nchan,
                   const std::string &device,
                   const std::string &dev_args,
                   const std::string &stream_args,
                   const std::vector<std::string> &tune_args,
                   const std::vector<std::string> &other_settings,
                   double sampling_rate,
                   const std::string &type);

  /*!
   * Returns a list with the available antennas for a specific channel
   * @param channel the channel index
   * @return the available antenna names
   */
  virtual std::vector<std::string> get_antennas(int channel) = 0;

  /*!
   * Callback to set overall gain
   * \param channel an available channel of the device
   * \param gain the overall gain value
   */
  virtual void set_gain(size_t channel, float gain) = 0;

  /*!
   * Callback to set specific gain value
   * \param channel an available channel on the device
   * \param name the gain name to set value
   * \param gain the gain value
   */
  virtual void set_gain(size_t channel, const std::string name, float gain) = 0;

  /*!
   * Callback to change the RF frequency of the device
   * \param channel an available channel of the device
   * \param freq the frequency to be set in Hz
   */
  virtual void set_frequency(size_t channel, double freq) = 0;

  /*!
   * Callback to change center frequency of a tunable element
   * \param channel an available channel of the device
   * \param name an available element name
   * \param frequency the frequency to be set in Hz
   */
  virtual void set_frequency(size_t channel, const std::string &name,
                             double frequency) = 0;

  /*!
   * Callback to set automatic gain control (AGC)
   * \param channel an available channel on the device
   * \param enable true to enable AGC
   */
  virtual void set_agc(size_t channel, bool enable) = 0;

  /*!
   * Callback to set sample rate
   * \param channel an available channel of the device
   * \param sample_rate number of samples in samples per second
   */
  virtual void set_sample_rate(size_t channel, double sample_rate) = 0;

  /*!
   * Callback to set digital filter bandwidth
   * \param channel an available channel on the device
   * \param bandwidth filter width in Hz
   */
  virtual void set_bandwidth(size_t channel, double bandwidth) = 0;

  /*!
   * Callback to set antenna for RF chain
   * \param channel an available channel of the device
   * \param name an available antenna string name
   */
  virtual void set_antenna(size_t channel, const std::string &name) = 0;

  /*!
   * Callback to set dc offset correction and mode
   * \param channel an available channel of the device
   * \param dc_offset complex for dc offset correction
   */
  virtual void set_dc_offset(size_t channel, gr_complexd dc_offset) = 0;

  /*!
   * Callback to set automatic DC removal
   * \param channel an available channel of the device
   * \param automatic true to set the automatic DC removal
   */
  virtual void set_dc_removal(size_t channel, bool automatic) = 0;

  /*!
   * Callback to set frequency correction
   * \param channel an available channel of the device
   * \param freq_correction relative value for frequency correction (1.0 max)
   */
  virtual void set_frequency_correction(size_t channel,
                                        double freq_correction) = 0;

  /*!
   * Callback to set iq balance correction
   * \param channel an available channel of the device
   * \param iq_balance complex value for iq balance correction
   */
  virtual void set_iq_balance(size_t channel, gr_complexd iq_balance) = 0;

  /*!
   * Callback to change master clock rate
   * \param clock_rate the clock rate in Hz
   */
  virtual void set_master_clock_rate(double clock_rate) = 0;

  /*!
   * Callback to set the clock source
   * \param clock_source an available clock source
   */
  virtual void set_clock_source(const std::string &clock_source) = 0;

};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_SOAPY_SOURCE_H */
