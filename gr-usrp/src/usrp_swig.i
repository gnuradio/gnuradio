/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

%include "gnuradio.i"	// the common stuff

%{
#include <vector>
%}

%include <usrp/usrp_subdev_spec.h>
%include <usrp/db_base.i>
%include <fpga_regs_common.h>
%include <fpga_regs_standard.h>
%include "usrp_standard.i"
%include "usrp_base.i"
%include "usrp_source_base.i"
%include "usrp_source_c.i"
%include "usrp_source_s.i"
%include "usrp_sink_base.i"
%include "usrp_sink_c.i"
%include "usrp_sink_s.i"

//---Allow a more Pythonic interface
%pythoncode %{

# Allow subdev_spec to be tuple
def __selected_subdev(self, subdev_spec):
  ss = usrp_subdev_spec(subdev_spec[0], subdev_spec[1])
  return self._real_selected_subdev(ss)

# Allow subdev_spec to be tuple
def __determine_tx_mux_value(self, subdev_spec, subdev_spec_=None):
    ss = usrp_subdev_spec(subdev_spec[0], subdev_spec[1])
    if subdev_spec_ is None: return self._real_determine_tx_mux_value(ss)
    ss_ = usrp_subdev_spec(subdev_spec_[0], subdev_spec_[1])
    return self._real_determine_tx_mux_value(ss, ss_)

# Allow subdev_spec to be tuple
def __determine_rx_mux_value(self, subdev_spec, subdev_spec_=None):
    ss = usrp_subdev_spec(subdev_spec[0], subdev_spec[1])
    if subdev_spec_ is None: return self._real_determine_rx_mux_value(ss)
    ss_ = usrp_subdev_spec(subdev_spec_[0], subdev_spec_[1])
    return self._real_determine_rx_mux_value(ss, ss_)

# Allow subdev_spec to be tuple
def __pick_subdev(self, candidates=[]):
    ss = self._real_pick_subdev(candidates)
    return (ss.side, ss.subdev)

# Allow subdev_spec to be tuple
def __pick_tx_subdevice(self):
    ss = self._real_pick_tx_subdevice()
    return (ss.side, ss.subdev)

# Allow subdev_spec to be tuple
def __pick_rx_subdevice(self):
    ss = self._real_pick_rx_subdevice()
    return (ss.side, ss.subdev)

# Make return tune_result or None on failure
def __tune(self, chan, db, target_freq):
    tr = usrp_tune_result()
    r = self._real_tune(chan, db, target_freq, tr)
    if r:
        return tr
    else:
        return None

# Allow to be called as a free function
def tune(u, chan, subdev, target_freq):
    return u.tune(chan, subdev, target_freq)

# Allow to be called as free function
def determine_tx_mux_value(u, subdev_spec, subdev_spec_=None):
    return u.determine_tx_mux_value(subdev_spec, subdev_spec_)

# Allow to be called as free function
def determine_rx_mux_value(u, subdev_spec, subdev_spec_=None):
    return u.determine_rx_mux_value(subdev_spec, subdev_spec_)

# Allow to be called as free function
def selected_subdev(u, subdev_spec):
    return u.selected_subdev(subdev_spec)

# Allow to be called as free function
def pick_subdev(u, candidates=[]):
    return u.pick_subdev(candidates);

# Allow to be called as free function
def pick_tx_subdevice(u):
    return u.pick_tx_subdevice();

# Allow to be called as free function
def pick_rx_subdevice(u):
    return u.pick_rx_subdevice();

# Jam into Python objects
usrp_sink_c_sptr.determine_tx_mux_value   = __determine_tx_mux_value
usrp_sink_s_sptr.determine_tx_mux_value   = __determine_tx_mux_value

usrp_source_c_sptr.determine_rx_mux_value = __determine_rx_mux_value
usrp_source_s_sptr.determine_rx_mux_value = __determine_rx_mux_value

usrp_sink_c_sptr.selected_subdev          = __selected_subdev
usrp_sink_s_sptr.selected_subdev          = __selected_subdev
usrp_source_c_sptr.selected_subdev        = __selected_subdev
usrp_source_s_sptr.selected_subdev        = __selected_subdev

usrp_sink_c_sptr.tune                     = __tune
usrp_sink_s_sptr.tune                     = __tune
usrp_source_c_sptr.tune                   = __tune
usrp_source_s_sptr.tune                   = __tune

usrp_sink_c_sptr.pick_subdev              = __pick_subdev
usrp_sink_s_sptr.pick_subdev              = __pick_subdev
usrp_source_c_sptr.pick_subdev            = __pick_subdev
usrp_source_s_sptr.pick_subdev            = __pick_subdev

usrp_sink_c_sptr.pick_tx_subdevice        = __pick_tx_subdevice
usrp_sink_s_sptr.pick_tx_subdevice        = __pick_tx_subdevice
usrp_source_c_sptr.pick_rx_subdevice      = __pick_rx_subdevice
usrp_source_s_sptr.pick_rx_subdevice      = __pick_rx_subdevice

%}
