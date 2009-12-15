# Copyright 2009 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import common
from gnuradio import gr

####################################################################
# Simple USRP Source
####################################################################
class _simple_source(gr.hier_block2):
	"""A single usrp source of IO type short or complex."""

	def __init__(self, which, side='A', rx_ant='RXA', no_hb=False):
		"""
		USRP simple source contructor.
		@param which the unit number
		@param side the usrp side A or B
		@param rx_ant the antenna choice
		@param no_hb disable half band filters
		"""
		self._no_hb = no_hb
		#initialize hier2 block
		gr.hier_block2.__init__(
			self, 'usrp_simple_source',
			gr.io_signature(0, 0, 0),
			gr.io_signature(1, 1, self._get_io_size()),
		)
		#create usrp object
		if self._no_hb: self._make_usrp(which=which, nchan=1, fpga_filename="std_4rx_0tx.rbf")
		else: self._make_usrp(which=which, nchan=1)
		subdev_spec = common.to_spec(side, rx_ant)
		self._get_u().set_mux(self._get_u().determine_rx_mux_value(subdev_spec))
		self._subdev = self._get_u().selected_subdev(subdev_spec)
		if common.is_flex(rx_ant): self._subdev.select_rx_antenna(rx_ant)
		#connect
		self.connect(self._get_u(), self)

	def set_decim_rate(self, decim):
		self._get_u().set_decim_rate(int(decim))
		if self._no_hb: #set the BW to half the sample rate
			self._subdev.set_bw(self._get_u().converter_rate()/decim/2)
	def set_frequency(self, frequency, verbose=False, lo_offset=None):
		if lo_offset is not None: self._subdev.set_lo_offset(lo_offset)
		self._set_frequency(
			chan=0, #ddc0
			subdev=self._subdev,
			frequency=frequency,
			verbose=verbose,
		)
	def set_gain(self, gain): self._subdev.set_gain(gain)

class simple_source_c(_simple_source, common.usrp_source_c): pass
class simple_source_s(_simple_source, common.usrp_source_s): pass

####################################################################
# Simple USRP Sink
####################################################################
class _simple_sink(gr.hier_block2):
	"""A single usrp sink of IO type short or complex."""

	def __init__(self, which, side='A'):
		"""
		USRP simple sink contructor.
		@param which the unit number
		@param side the usrp side A or B
		"""
		#initialize hier2 block
		gr.hier_block2.__init__(
			self, 'usrp_simple_sink',
			gr.io_signature(1, 1, self._get_io_size()),
			gr.io_signature(0, 0, 0),
		)
		#create usrp object
		self._make_usrp(which=which, nchan=1)
		subdev_spec = common.to_spec(side)
		self._get_u().set_mux(self._get_u().determine_tx_mux_value(subdev_spec))
		self._subdev = self._get_u().selected_subdev(subdev_spec)
		#connect
		self.connect(self, self._get_u())

	def set_interp_rate(self, interp): self._get_u().set_interp_rate(int(interp))
	def set_frequency(self, frequency, verbose=False, lo_offset=None):
		if lo_offset is not None: self._subdev.set_lo_offset(lo_offset)
		self._set_frequency(
			chan=self._subdev.which(),
			subdev=self._subdev,
			frequency=frequency,
			verbose=verbose,
		)
	def set_gain(self, gain): self._subdev.set_gain(gain)
	def set_enable(self, enable): self._subdev.set_enable(enable)
	def set_auto_tr(self, auto_tr): self._subdev.set_auto_tr(auto_tr)

class simple_sink_c(_simple_sink, common.usrp_sink_c): pass
class simple_sink_s(_simple_sink, common.usrp_sink_s): pass

