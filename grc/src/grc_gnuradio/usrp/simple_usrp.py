# Copyright 2008 Free Software Foundation, Inc.
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

import sys
from gnuradio import usrp, gr

####################################################################
# Helper Functions
####################################################################

def _set_frequency(u, which, subdev, frequency, verbose=False):
	"""
	Set the carrier frequency for the given subdevice.
	@param u the usrp source/sink
	@param which specifies the DDC/DUC number
	@param frequency the carrier frequency in Hz
	@param verbose if true, print usrp tuning information
	"""
	r = u.tune(which, subdev, frequency)
	if not verbose: return
	print subdev.side_and_name()
	if r:
		print "  r.baseband_frequency =", r.baseband_freq
		print "  r.dxc_frequency      =", r.dxc_freq
		print "  r.residual_frequency =", r.residual_freq
		print "  r.inverted      =", r.inverted
	else: print >> sys.stderr, 'Error calling tune on subdevice.'

def _setup_rx_subdev(u, subdev_spec, ddc, gain, frequency, auto_tr=None, rx_ant=None):
	"""
	Setup a usrp receive subdevice by setting gain and frequency.
	Add the gain and frequency callbacks to the flow graph.
	FlexRF: Handle auto transmit/receive and set the receive antenna.
	@param u the usrp object
	@param subdev_spec the sub-device specification
	@param ddc which ddc to use: 0 or 1
	@param gain the gain to set
	@param frequency the frequency to tune
	@param auto_tr auto transmit/receive True, False, or None
	@param rx_ant the receive antenna: 'TX/RX', 'RX2', or None
	@return the subdevice
	"""
	subdev = usrp.selected_subdev(u, subdev_spec)#get the subdev
	subdev.set_gain(gain)
	_set_frequency(u, ddc, subdev, frequency, verbose=True)
	if auto_tr is not None: subdev.set_auto_tr(auto_tr)
	if rx_ant is not None: subdev.select_rx_antenna(rx_ant)
	return subdev

def _setup_tx_subdev(u, subdev_spec, gain, frequency, auto_tr=None, tx_enb=None):
	"""
	Setup a usrp receive subdevice by setting gain and frequency.
	Add the gain and frequency callbacks to the flow graph.
	FlexRF: Handle auto transmit/receive and enable the transmitter.
	@param u the usrp object
	@param subdev_spec the sub-device specification
	@param gain the gain to set
	@param frequency the frequency to tune
	@param auto_tr auto transmit/receive True, False, or None
	@param tx_enb the transmit enable: True, False, or None
	@return the subdevice
	"""
	subdev = usrp.selected_subdev(u, subdev_spec)#get the subdev
	subdev.set_gain(gain)
	_set_frequency(u, subdev.which(), subdev, frequency, verbose=True)
	if auto_tr is not None: subdev.set_auto_tr(auto_tr)
	if tx_enb is not None: subdev.set_enable(tx_enb)
	return subdev

##map the usrp contructors to IO sizes
constructor_to_size = {
	usrp.source_c: gr.sizeof_gr_complex,
	usrp.sink_c: gr.sizeof_gr_complex,
	usrp.source_s: gr.sizeof_short,
	usrp.sink_s: gr.sizeof_short,
}

####################################################################
####################################################################
# Simple USRP Base Classes
####################################################################
####################################################################

class _simple_usrp(object):
	"""A single usrp source/sink base class."""

	def __init__(self, u, subdev, which):
		"""
		Create a simple usrp base class.
		@param u the usrp object
		@param subdev the subdevice object
		@param which specifies the DDC/DUC number when tuning
		"""
		self._u = u
		self._subdev = subdev
		self._which = which

	def get_u(self):
		"""
		Get the underlying usrp object.
		@return the usrp source/sink object.
		"""
		return self._u

	def get_subdev(self):
		"""
		Get the underlying subdevice.
		@return the subdev object.
		"""
		return self._subdev

	def set_frequency(self, frequency):
		"""
		Set the frequency of the subdevice.
		@param frequency the frequency to tune
		"""
		_set_frequency(self.get_u(), self._which, self.get_subdev(), frequency)

	def set_gain(self, gain):
		"""
		Set the gain of the subdevice.
		@param gain the gain to set
		"""
		self.get_subdev().set_gain(gain)

####################################################################
# Simple USRP Source
####################################################################
class _simple_source(gr.hier_block2, _simple_usrp):
	"""A single usrp source of IO type short or complex."""

	def __init__(self, number, subdev_spec, frequency, decimation, gain, mux=0x0, auto_tr=None, rx_ant=None):
		"""
		USRP simple source contructor.
		@param number the unit number
		@param subdev_spec the sub-device specification tuple
		@param frequency the frequency to tune
		@param decimation the device decimation
		@param gain the gain to set
		@param mux the mux in hex or None
		@param auto_tr auto transmit/receive True, False, or None
		@param rx_ant the receive antenna: 'TX/RX', 'RX2', or None
		"""
		#initialize hier2 block
		gr.hier_block2.__init__(
			self, 'usrp_simple_source',
			gr.io_signature(0, 0, 0),
			gr.io_signature(1, 1, constructor_to_size[self.constructor[0]]),
		)
		#create usrp object
		u = self.constructor[0](number, nchan=1)
		if subdev_spec is None: subdev_spec = usrp.pick_rx_subdevice(u)
		u.set_decim_rate(decimation)
		if not mux: mux = usrp.determine_rx_mux_value(u, subdev_spec)
		u.set_mux(mux)
		subdev = _setup_rx_subdev(u, subdev_spec, 0, gain, frequency, auto_tr, rx_ant)
		_simple_usrp.__init__(self, u, subdev, 0)
		#connect
		self.connect(u, self)

	def set_decim_rate(self, decim): self.get_u().set_decim_rate(int(decim))

class simple_source_c(_simple_source): constructor = (usrp.source_c, )
class simple_source_s(_simple_source): constructor = (usrp.source_s, )

####################################################################
# Simple USRP Sink
####################################################################
class _simple_sink(gr.hier_block2, _simple_usrp):
	"""A single usrp sink of IO type short or complex."""

	def __init__(self, number, subdev_spec, frequency, interpolation, gain, mux=0x0, auto_tr=None, tx_enb=None):
		"""
		USRP simple sink contructor.
		@param number the unit number
		@param subdev_spec the sub-device specification tuple
		@param frequency the frequency to tune
		@param interpolation the device interpolation
		@param gain the gain to set
		@param mux the mux in hex or None
		@param auto_tr auto transmit/receive True, False, or None
		@param tx_enb the transmit enable: True, False, or None
		"""
		#initialize hier2 block
		gr.hier_block2.__init__(
			self, 'usrp_simple_sink',
			gr.io_signature(1, 1, constructor_to_size[self.constructor[0]]),
			gr.io_signature(0, 0, 0),
		)
		#create usrp object
		u = self.constructor[0](number, nchan=1)
		if subdev_spec is None: subdev_spec = usrp.pick_tx_subdevice(u)
		u.set_interp_rate(interpolation)
		if not mux: mux = usrp.determine_tx_mux_value(u, subdev_spec)
		u.set_mux(mux)
		subdev = _setup_tx_subdev(u, subdev_spec, gain, frequency, auto_tr, tx_enb)
		_simple_usrp.__init__(self, u, subdev, subdev.which())
		#connect
		self.connect(self, u)

	def set_interp_rate(self, interp): self.get_u().set_interp_rate(int(interp))

class simple_sink_c(_simple_sink): constructor = (usrp.sink_c, )
class simple_sink_s(_simple_sink): constructor = (usrp.sink_s, )

####################################################################
####################################################################
# Dual USRP Base Classes
####################################################################
####################################################################

class _dual_usrp(object):
	"""A dual usrp source/sink base class."""

	def __init__(self, u, subdev_a, subdev_b, which_a, which_b):
		"""
		Create a dual usrp base class.
		@param u the usrp object
		@param subdev_a the subdevice object side a
		@param subdev_b the subdevice object side b
		@param which_a specifies the DDC/DUC number when tuning side a
		@param which_b specifies the DDC/DUC number when tuning side b
		"""
		self._u = u
		self._subdev_a = subdev_a
		self._subdev_b = subdev_b
		self._which_a = which_a
		self._which_b = which_b

	def get_u(self):
		"""
		Get the underlying usrp object.
		@return the usrp source/sink object.
		"""
		return self._u

	def get_subdev_a(self):
		"""
		Get the underlying subdevice.
		@return the subdev object.
		"""
		return self._subdev_a

	def get_subdev_b(self):
		"""
		Get the underlying subdevice.
		@return the subdev object.
		"""
		return self._subdev_b

	def set_frequency_a(self, frequency):
		"""
		Set the frequency of the subdevice.
		@param frequency the frequency to tune
		"""
		_set_frequency(self.get_u(), self._which_a, self.get_subdev_a(), frequency)

	def set_frequency_b(self, frequency):
		"""
		Set the frequency of the subdevice.
		@param frequency the frequency to tune
		"""
		_set_frequency(self.get_u(), self._which_b, self.get_subdev_b(), frequency)

	def set_gain_a(self, gain):
		"""
		Set the gain of the subdevice.
		@param gain the gain to set
		"""
		self.get_subdev_a().set_gain(gain)

	def set_gain_b(self, gain):
		"""
		Set the gain of the subdevice.
		@param gain the gain to set
		"""
		self.get_subdev_b().set_gain(gain)

####################################################################
# Dual USRP Source
####################################################################
class _dual_source(gr.hier_block2, _dual_usrp):
	"""A dual usrp source of IO type short or complex."""

	def __init__(self, number, frequency_a, frequency_b, decimation, gain_a, gain_b, mux=0x0, auto_tr=None, rx_ant_a=None, rx_ant_b=None):
		"""
		USRP dual source contructor.
		@param number the unit number
		@param frequency_a the frequency to tune side a
		@param frequency_b the frequency to tune side b
		@param decimation the device decimation
		@param gain_a the gain to set side a
		@param gain_b the gain to set side b
		@param mux the mux in hex
		@param auto_tr auto transmit/receive True, False, or None
		@param rx_ant_a the receive antenna side a: 'TX/RX', 'RX2', or None
		@param rx_ant_b the receive antenna side b: 'TX/RX', 'RX2', or None
		"""
		#initialize hier2 block
		gr.hier_block2.__init__(
			self, 'usrp_dual_source',
			gr.io_signature(0, 0, 0),
			gr.io_signature(2, 2, constructor_to_size[self.constructor[0]]),
		)
		#create usrp object
		u = self.constructor[0](number, nchan=2)
		u.set_decim_rate(decimation)
		if not mux: mux = 0x3210
		u.set_mux(mux)
		subdev_a = _setup_rx_subdev(u, (0, 0), 0, gain_a, frequency_a, auto_tr, rx_ant_a)
		subdev_b = _setup_rx_subdev(u, (1, 0), 1, gain_b, frequency_b, auto_tr, rx_ant_b)
		_dual_usrp.__init__(self, u, subdev_a, subdev_b, 0, 1)
		#connect
		deinter = gr.deinterleave(constructor_to_size[self.constructor[0]])
		self.connect(u, deinter)
		for i in range(2): self.connect((deinter, i), (self, i))

	def set_decim_rate(self, decim): self.get_u().set_decim_rate(int(decim))

class dual_source_c(_dual_source): constructor = usrp.source_c
class dual_source_s(_dual_source): constructor = usrp.source_s

####################################################################
# Dual USRP Sink
####################################################################
class _dual_sink(gr.hier_block2, _dual_usrp):
	"""A dual usrp sink of IO type short or complex."""

	def __init__(self, number, frequency_a, frequency_b, interpolation, gain_a, gain_b, mux=0x0, auto_tr=None, tx_enb_a=None, tx_enb_b=None):
		"""
		USRP dual sink contructor.
		@param number the unit number
		@param subdev_spec the sub-device specification tuple
		@param frequency the frequency to tune
		@param interpolation the device interpolation
		@param gain the gain to set
		@param mux the mux in hex or None
		@param auto_tr auto transmit/receive True, False, or None
		@param tx_enb the transmit enable: True, False, or None
		"""
		#initialize hier2 block
		gr.hier_block2.__init__(
			self, 'usrp_dual_sink',
			gr.io_signature(2, 2, constructor_to_size[self.constructor[0]]),
			gr.io_signature(0, 0, 0),
		)
		#create usrp object
		u = self.constructor[0](number, nchan=2)
		u.set_interp_rate(interpolation)
		if not mux: mux = 0xba98
		u.set_mux(mux)
		subdev_a = _setup_tx_subdev(u, (0, 0), gain_a, frequency_a, auto_tr, tx_enb_a)
		subdev_b = _setup_tx_subdev(u, (1, 0), gain_b, frequency_b, auto_tr, tx_enb_b)
		_dual_usrp.__init__(self, u, subdev_a, subdev_b, subdev_a.which(), subdev_b.which())
		#connect
		inter = gr.interleave(constructor_to_size[self.constructor[0]])
		self.connect(inter, u)
		for i in range(2): self.connect((self, i), (inter, i))

	def set_interp_rate(self, interp): self.get_u().set_interp_rate(int(interp))

class dual_sink_c(_dual_sink): constructor = usrp.sink_c
class dual_sink_s(_dual_sink): constructor = usrp.sink_s
