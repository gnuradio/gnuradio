#
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

from gnuradio import gr
import threading
import numpy

class _variable_sink_base(gr.hier_block2, threading.Thread):
	"""
	The thread polls the message queue for values and writes to a callback.
	"""

	def __init__(self, vlen, decim, callback):
		self._vlen = vlen
		self._callback = callback
		self._item_size = self._size*self._vlen
		#init hier block
		gr.hier_block2.__init__(
			self, 'variable_sink',
			gr.io_signature(1, 1, self._item_size),
			gr.io_signature(0, 0, 0),
		)
		#create blocks
		self._decimator = gr.keep_one_in_n(self._item_size, decim)
		self._msgq = gr.msg_queue(2)
		message_sink = gr.message_sink(self._item_size, self._msgq, False)
		#connect
		self.connect(self, self._decimator, message_sink)
		#setup thread
		threading.Thread.__init__(self)
		self.setDaemon(True)
		self.start() 

	def set_decim(self, decim): self._decimator.set_n(decim)

	def run(self):
		while True: #truncate to item size, convert to array, callback
			msg = self._msgq.delete_head().to_string()[-self._item_size:]
			arr = map(self._cast, numpy.fromstring(msg, self._numpy))
			self._callback(self._vlen > 1 and arr or arr[0])

class variable_sink_b(_variable_sink_base): _numpy, _size, _cast = numpy.int8, gr.sizeof_char, int
class variable_sink_s(_variable_sink_base): _numpy, _size, _cast = numpy.int16, gr.sizeof_short, int
class variable_sink_i(_variable_sink_base): _numpy, _size, _cast = numpy.int32, gr.sizeof_int, int
class variable_sink_f(_variable_sink_base): _numpy, _size, _cast = numpy.float32, gr.sizeof_float, float
class variable_sink_c(_variable_sink_base): _numpy, _size, _cast = numpy.complex64, gr.sizeof_gr_complex, complex
