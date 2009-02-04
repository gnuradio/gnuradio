/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP2_SINK_BASE_H
#define INCLUDED_USRP2_SINK_BASE_H

#include <usrp2_base.h>

/*!
 * Base class for all USRP2 transmit blocks
 */
class usrp2_sink_base : public usrp2_base 
{
protected:
  usrp2_sink_base(const char *name,
		  gr_io_signature_sptr input_signature,
		  const std::string &ifc,
		  const std::string &mac)
    throw (std::runtime_error);

public:
  ~usrp2_sink_base();

  /*!
   * \brief Set transmitter gain
   */
  bool set_gain(double gain);

  /*!
   * \brief Set transmitter LO offset frequency
   */
  bool set_lo_offset(double frequency);

  /*!
   * \brief Set transmitter center frequency
   */
  bool set_center_freq(double frequency, usrp2::tune_result *tr);
   
  /*!
   * \brief Set transmit interpolation rate
   */
  bool set_interp(int interp_factor);

  /*!
   * \brief Calculate default scale_iq for given interpolation factor
   */
  void default_scale_iq(int interpolation_factor, int *scale_i, int *scale_q);

  /*!
   * \brief Set transmit IQ scale factors
   */
  bool set_scale_iq(int scale_i, int scale_q);

  /*!
   * \brief Get transmit interpolation rate
   */
  int interp();

  /*!
   * \brief Get DAC sample rate in Hz 
   */
  bool dac_rate(long *rate);

  /*!
   * \brief Returns minimum Tx gain 
   */
  double gain_min();

  /*!
   * \brief Returns maximum Tx gain 
   */
  double gain_max();
  
  /*!
   * \brief Returns Tx gain db_per_step
   */
  double gain_db_per_step();

  /*!
   * \brief Returns minimum Tx center frequency
   */
  double freq_min();

  /*!
   * \brief Returns maximum Tx center frequency
   */
  double freq_max();

  /*!
   * \brief Get Tx daughterboard ID
   *
   * \param[out] dbid returns the daughterboard id.
   *
   * daughterboard id >= 0 if successful, -1 if no daugherboard installed,
   * -2 if invalid EEPROM on daughterboard.
   */
  bool daughterboard_id(int *dbid);
};

#endif /* INCLUDED_USRP2_SINK_BASE_H */
