#
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

from gnuradio import gr
import threading
import numpy
import time

class _probe_base(gr.hier_block2, threading.Thread):
	"""
	A hier2 block with float output and probe input.
	The thread polls the prope for values and writes to a message source.
	"""

	def __init__(self, probe_block, probe_callback, probe_rate):
		#init hier block
		gr.hier_block2.__init__(
			self, 'probe',
			gr.io_signature(1, 1, probe_block.input_signature().sizeof_stream_items()[0]),
			gr.io_signature(1, 1, gr.sizeof_float),
		)
		self._probe_callback = probe_callback
		self.set_probe_rate(probe_rate)
		#create message source
		message_source = gr.message_source(gr.sizeof_float, 1)
		self._msgq = message_source.msgq()
		#connect
		self.connect(self, probe_block)
		self.connect(message_source, self)
		#setup thread
		threading.Thread.__init__(self)
		self.setDaemon(True)
		self.start()

	def run(self):
		"""
		Infinite polling loop.
		"""
		while True:
			time.sleep(1.0/self._probe_rate)
			arr = numpy.array(self._probe_callback(), numpy.float32)
			msg = gr.message_from_string(arr.tostring(), 0, gr.sizeof_float, 1)
			self._msgq.insert_tail(msg)

	def set_probe_rate(self, probe_rate):
		self._probe_rate = probe_rate

#######################################################################################
## Probe: Average Magnitude Squared
#######################################################################################
class _probe_avg_mag_sqrd_base(_probe_base):
	def __init__(self, threshold, alpha, probe_rate):
		#create block
		probe_block = self._probe_block_contructor[0](threshold, alpha)
		#forward callbacks
		self.set_alpha = probe_block.set_alpha
		self.set_threshold = probe_block.set_threshold
		#init
		_probe_base.__init__(self, probe_block, probe_block.level, probe_rate)

class probe_avg_mag_sqrd_c(_probe_avg_mag_sqrd_base): _probe_block_contructor = (gr.probe_avg_mag_sqrd_c,)
class probe_avg_mag_sqrd_f(_probe_avg_mag_sqrd_base): _probe_block_contructor = (gr.probe_avg_mag_sqrd_f,)

#######################################################################################
## Probe: Density
#######################################################################################
class probe_density_b(_probe_base):
	def __init__(self, alpha, probe_rate):
		#create block
		probe_block = gr.probe_density_b(alpha)
		#forward callbacks
		self.set_alpha = probe_block.set_alpha
		#init
		_probe_base.__init__(self, probe_block, probe_block.density, probe_rate)

#######################################################################################
## Probe: MPSK SNR
#######################################################################################
class probe_mpsk_snr_c(_probe_base):
	def __init__(self, type, alpha, probe_rate):
		"""
		Type can be "snr", "signal_mean", or "noise_variance" 
		"""
		#create block
		probe_block = gr.probe_mpsk_snr_c(alpha)
		#forward callbacks
		self.set_alpha = probe_block.set_alpha
		#init
		_probe_base.__init__(self, probe_block, getattr(probe_block, type), probe_rate)
