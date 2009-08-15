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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vrt_sink_base.h>
#include <gr_io_signature.h>
#include <iostream>

vrt_sink_base::vrt_sink_base(const char *name,
				 gr_io_signature_sptr input_signature,
				 const std::string &ifc,
				 const std::string &mac) 
  throw (std::runtime_error)
  : vrt_base(name,
               input_signature,
	       gr_make_io_signature(0, 0, 0),
	       ifc, mac)
{
  // NOP
}

vrt_sink_base::~vrt_sink_base ()
{
  // NOP
}

bool
vrt_sink_base::set_gain(double gain)
{
  return d_u2->set_tx_gain(gain);
}

bool
vrt_sink_base::set_lo_offset(double frequency)
{
  return d_u2->set_tx_lo_offset(frequency);
}

bool
vrt_sink_base::set_center_freq(double frequency, vrt::tune_result *tr)
{
  return d_u2->set_tx_center_freq(frequency, tr);
}

bool
vrt_sink_base::set_interp(int interp_factor)
{
  return d_u2->set_tx_interp(interp_factor);
}

void
vrt_sink_base::default_scale_iq(int interp_factor, int *scale_i, int *scale_q)
{
  return d_u2->default_tx_scale_iq(interp_factor, scale_i, scale_q);
}

bool 
vrt_sink_base::set_scale_iq(int scale_i, int scale_q)
{
  return d_u2->set_tx_scale_iq(scale_i, scale_q);
}

int
vrt_sink_base::interp()
{
  return d_u2->tx_interp();
}

bool
vrt_sink_base::dac_rate(long *rate)
{
  return d_u2->dac_rate(rate);
}

double
vrt_sink_base::gain_min()
{
  return d_u2->tx_gain_min();
}

double
vrt_sink_base::gain_max()
{
  return d_u2->tx_gain_max();
}

double
vrt_sink_base::gain_db_per_step()
{
  return d_u2->tx_gain_db_per_step();
}
  
double
vrt_sink_base::freq_min()
{
  return d_u2->tx_freq_min();
}

double
vrt_sink_base::freq_max()
{
  return d_u2->tx_freq_max();
}

bool
vrt_sink_base::daughterboard_id(int *dbid)
{
  return d_u2->tx_daughterboard_id(dbid);
}

bool vrt_sink_base::set_gpio_ddr(uint16_t value, uint16_t mask)
{
  return d_u2->set_gpio_ddr(vrt::GPIO_TX_BANK, value, mask);
}

bool vrt_sink_base::set_gpio_sels(std::string sels)
{
  return d_u2->set_gpio_sels(vrt::GPIO_TX_BANK, sels);
}

bool vrt_sink_base::write_gpio(uint16_t value, uint16_t mask)
{
  return d_u2->write_gpio(vrt::GPIO_TX_BANK, value, mask);
}

bool vrt_sink_base::read_gpio(uint16_t *value)
{
  return d_u2->read_gpio(vrt::GPIO_TX_BANK, value);
}
