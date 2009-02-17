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

%{
#include "usrp_sink_base.h"
%}

class usrp_sink_base : public usrp_base 
{
private:
  usrp_sink_base() throw (std::runtime_error);

public:
  bool set_interp_rate (unsigned int rate);
  bool set_nchannels (int nchan);
  bool set_mux (int mux);
  %rename(_real_determine_tx_mux_value) determine_tx_mux_value;
  int determine_tx_mux_value(usrp_subdev_spec ss);
  int determine_tx_mux_value(usrp_subdev_spec ss_a, usrp_subdev_spec ss_b);
  bool set_tx_freq (int channel, double freq);
  long dac_rate() const { return converter_rate(); }
  long dac_freq() const { return converter_rate(); }
  unsigned int interp_rate () const;
  int nchannels () const;
  int mux () const;
  double tx_freq (int channel) const;
  int nunderruns () const { return d_nunderruns; }
  bool has_rx_halfband();
  bool has_tx_halfband();
  int nddcs();
  int nducs();
  %rename(_real_tune) tune;
  bool tune(int chan, db_base_sptr db, double target_freq, usrp_tune_result *result);
  %rename(_real_pick_tx_subdevice) pick_tx_subdevice();
  usrp_subdev_spec pick_tx_subdevice();
};
