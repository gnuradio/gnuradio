# Copyright 2009, 2010 Free Software Foundation, Inc.
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
# Dual USRP Source
####################################################################
class _dual_source(gr.hier_block2):
	"""A dual usrp source of IO type short or complex."""

	def __init__(self, which, rx_ant_a='RXA', rx_ant_b='RXA', rx_source_a='A', rx_source_b='B'):
		"""
		USRP dual source contructor.
		@param which the unit number
		@param rx_ant_a the antenna choice
		@param rx_ant_b the antenna choice
		"""
		#initialize hier2 block
		gr.hier_block2.__init__(
			self, 'usrp_dual_source',
			gr.io_signature(0, 0, 0),
			gr.io_signature(2, 2, self._get_io_size()),
		)
		#create usrp object
		self._make_usrp(which=which, nchan=2)
		subdev_spec_a = common.to_spec(rx_source_a, rx_ant_a)
		subdev_spec_b = common.to_spec(rx_source_b, rx_ant_b)
		self._get_u().set_mux(self._get_u().determine_rx_mux_value(subdev_spec_a, subdev_spec_b))
		self._subdev_a = self._get_u().selected_subdev(subdev_spec_a)
		self._subdev_b = self._get_u().selected_subdev(subdev_spec_b)
		#connect
		deinter = gr.deinterleave(self._get_io_size())
		self.connect(self._get_u(), deinter)
		for i in range(2): self.connect((deinter, i), (self, i))

	def set_decim_rate(self, decim): self._get_u().set_decim_rate(int(decim))
	def set_frequency_a(self, frequency, verbose=False, lo_offset=None):
		if lo_offset is not None: self._subdev_a.set_lo_offset(lo_offset)
		self._set_frequency(
			chan=0, #ddc0
			subdev=self._subdev_a,
			frequency=frequency,
			verbose=verbose,
		)
	def set_frequency_b(self, frequency, verbose=False, lo_offset=None):
		if lo_offset is not None: self._subdev_b.set_lo_offset(lo_offset)
		self._set_frequency(
			chan=1, #ddc1
			subdev=self._subdev_b,
			frequency=frequency,
			verbose=verbose,
		)
	def set_gain_a(self, gain): self._subdev_a.set_gain(gain)
	def set_gain_b(self, gain): self._subdev_b.set_gain(gain)

class dual_source_c(_dual_source, common.usrp_source_c): pass
class dual_source_s(_dual_source, common.usrp_source_s): pass

####################################################################
# Dual USRP Sink
####################################################################
class _dual_sink(gr.hier_block2):
	"""A dual usrp sink of IO type short or complex."""

	def __init__(self, which):
		"""
		USRP simple sink contructor.
		@param which the unit number
		"""
		#initialize hier2 block
		gr.hier_block2.__init__(
			self, 'usrp_dual_sink',
			gr.io_signature(2, 2, self._get_io_size()),
			gr.io_signature(0, 0, 0),
		)
		#create usrp object
		self._make_usrp(which=which, nchan=2)
		subdev_spec_a = common.to_spec('A')
		subdev_spec_b = common.to_spec('B')
		self._get_u().set_mux(self._get_u().determine_tx_mux_value(subdev_spec_a, subdev_spec_b))
		self._subdev_a = self._get_u().selected_subdev(subdev_spec_a)
		self._subdev_b = self._get_u().selected_subdev(subdev_spec_b)
		#connect
		inter = gr.interleave(self._get_io_size())
		self.connect(inter, self._get_u())
		for i in range(2): self.connect((self, i), (inter, i))

	def set_interp_rate(self, interp): self._get_u().set_interp_rate(int(interp))
	def set_frequency_a(self, frequency, verbose=False, lo_offset=None):
		if lo_offset is not None: self._subdev_a.set_lo_offset(lo_offset)
		self._set_frequency(
			chan=self._subdev_a.which(),
			subdev=self._subdev_a,
			frequency=frequency,
			verbose=verbose,
		)
	def set_frequency_b(self, frequency, verbose=False, lo_offset=None):
		if lo_offset is not None: self._subdev_b.set_lo_offset(lo_offset)
		self._set_frequency(
			chan=self._subdev_b.which(),
			subdev=self._subdev_b,
			frequency=frequency,
			verbose=verbose,
		)
	def set_gain_a(self, gain): self._subdev_a.set_gain(gain)
	def set_gain_b(self, gain): self._subdev_b.set_gain(gain)
	def set_enable_a(self, enable): self._subdev_a.set_enable(enable)
	def set_enable_b(self, enable): self._subdev_b.set_enable(enable)
	def set_auto_tr_a(self, auto_tr): self._subdev_a.set_auto_tr(auto_tr)
	def set_auto_tr_b(self, auto_tr): self._subdev_b.set_auto_tr(auto_tr)

class dual_sink_c(_dual_sink, common.usrp_sink_c): pass
class dual_sink_s(_dual_sink, common.usrp_sink_s): pass
