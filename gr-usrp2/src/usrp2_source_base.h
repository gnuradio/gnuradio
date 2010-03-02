/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_USRP2_SOURCE_BASE_H
#define INCLUDED_USRP2_SOURCE_BASE_H

#include <usrp2_base.h>

/*!
 * Base class for all USRP2 source blocks
 */
class usrp2_source_base : public usrp2_base 
{
protected:
  usrp2_source_base(const char *name,
		    gr_io_signature_sptr output_signature,
		    const std::string &ifc,
		    const std::string &mac)
    throw (std::runtime_error);

public:
  ~usrp2_source_base();

  /*!
   * \brief Set antenna
   */
  bool set_antenna(int ant);

  /*!
   * \brief Set receiver gain
   */
  bool set_gain(double gain);

  /*!
   * \brief Set receive LO offset frequency
   */
  bool set_lo_offset(double frequency);

  /*!
   * \brief Set receiver center frequency
   */
  bool set_center_freq(double frequency, usrp2::tune_result *tr);
   
  /*!
   * \brief Set receive decimation rate
   */
  bool set_decim(int decimation_factor);

  /*!
   * \brief Set receive IQ scale factors
   */
  bool set_scale_iq(int scale_i, int scale_q);

  /*!
   * \brief Get receive decimation rate
   */
  int decim();

  /*!
   * \brief Get the ADC sample rate
   */
  bool adc_rate(long *rate);

  /*!
   * \brief Returns minimum Rx gain 
   */
  double gain_min();

  /*!
   * \brief Returns maximum Rx gain 
   */
  double gain_max();
  
  /*!
   * \brief Returns Rx gain db_per_step
   */
  double gain_db_per_step();
  
  /*!
   * \brief Returns minimum Rx center frequency
   */
  double freq_min();

  /*!
   * \brief Returns maximum Rx center frequency
   */
  double freq_max();
  
  /*!
   * \brief Get Rx daughterboard ID
   *
   * \param[out] dbid returns the daughterboard id.
   *
   * daughterboard id >= 0 if successful, -1 if no daugherboard installed,
   * -2 if invalid EEPROM on daughterboard.
   */
  bool daughterboard_id(int *dbid);

  /*!
   * \brief Returns number of receiver overruns
   */
  unsigned int overruns();

  /*!
   * \brief Returns number of missing sequence numbers
   */
  unsigned int missing();

  /*!
   * \brief Called by scheduler when starting flowgraph
   */
  virtual bool start();
  
  /*!
   * \brief Called by scheduler when stopping flowgraph
   */
  virtual bool stop();

  /*!
   * \brief Set daughterboard GPIO data direction register.
   */
  bool set_gpio_ddr(uint16_t value, uint16_t mask);

  /*!
   * \brief Set daughterboard GPIO output selection register.
   */
  bool set_gpio_sels(std::string sels);

  /*!
   * \brief Set daughterboard GPIO pin values.
   */
  bool write_gpio(uint16_t value, uint16_t mask);

  /*!
   * \brief Read daughterboard GPIO pin values
   */
  bool read_gpio(uint16_t *value);

  /*!
   * \brief Enable streaming GPIO in sample LSBs
   */
  bool enable_gpio_streaming(int enable);
};

#endif /* INCLUDED_USRP2_SOURCE_BASE_H */
