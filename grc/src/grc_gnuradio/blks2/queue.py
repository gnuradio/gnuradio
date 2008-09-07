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
import gnuradio.gr.gr_threading as _threading
import numpy

#######################################################################################
## Queue Sink Thread
#######################################################################################
class queue_sink_thread(_threading.Thread):
	"""
	Read samples from the queue sink and execute the callback.
	"""

	def __init__(self, queue_sink, callback):
		"""
		Queue sink thread contructor.
		@param queue_sink the queue to pop messages from
		@param callback the function of one argument
		"""
		self._queue_sink = queue_sink
		self._callback = callback
		_threading.Thread.__init__(self)
		self.setDaemon(1)
		self.keep_running = True
		self.start()

	def run(self):
		while self.keep_running:
			self._callback(self._queue_sink.pop())

#######################################################################################
## Queue Sink
#######################################################################################
class _queue_sink_base(gr.hier_block2):
	"""
	Queue sink base, a queue sink for any size queue.
	Easy read access to a gnuradio data stream from python.
	Call pop to read a sample from a gnuradio data stream.
	Samples are cast as python data types, complex, float, or int.
	"""

	def __init__(self, vlen=1):
		"""
		Queue sink base contructor.
		@param vlen the vector length
		"""
		self._vlen = vlen
		#initialize hier2
		gr.hier_block2.__init__(
			self,
			"queue_sink",
			gr.io_signature(1, 1, self._item_size*self._vlen), # Input signature
			gr.io_signature(0, 0, 0) # Output signature
		)
		#create message sink
		self._msgq = gr.msg_queue(1)
		message_sink = gr.message_sink(self._item_size*self._vlen, self._msgq, False) #False -> blocking
		#connect
		self.connect(self, message_sink)
		self.arr = ''

	def pop(self):
		"""
		Pop a new sample off the front of the queue.
		@return a new sample
		"""
		while len(self.arr) < self._item_size*self._vlen:
			msg = self._msgq.delete_head()
			self.arr = self.arr + msg.to_string()
		sample = self.arr[:self._item_size*self._vlen]
		self.arr = self.arr[self._item_size*self._vlen:]
		sample = map(self._cast, numpy.fromstring(sample, self._numpy))
		if self._vlen == 1: return sample[0]
		return sample

class queue_sink_c(_queue_sink_base):
	_item_size = gr.sizeof_gr_complex
	_numpy = numpy.complex64
	def _cast(self, arg): return complex(arg.real, arg.imag)

class queue_sink_f(_queue_sink_base):
	_item_size = gr.sizeof_float
	_numpy = numpy.float32
	_cast = float

class queue_sink_i(_queue_sink_base):
	_item_size = gr.sizeof_int
	_numpy = numpy.int32
	_cast = int

class queue_sink_s(_queue_sink_base):
	_item_size = gr.sizeof_short
	_numpy = numpy.int16
	_cast = int

class queue_sink_b(_queue_sink_base):
	_item_size = gr.sizeof_char
	_numpy = numpy.int8
	_cast = int

#######################################################################################
## Queue Source
#######################################################################################
class _queue_source_base(gr.hier_block2):
	"""
	Queue source base, a queue source for any size queue.
	Easy write access to a gnuradio data stream from python.
	Call push to to write a sample into the gnuradio data stream.
	"""

	def __init__(self, vlen=1):
		"""
		Queue source base contructor.
		@param vlen the vector length
		"""
		self._vlen = vlen
		#initialize hier2
		gr.hier_block2.__init__(
			self,
			"queue_source",
			gr.io_signature(0, 0, 0), # Input signature
			gr.io_signature(1, 1, self._item_size*self._vlen) # Output signature
		)
		#create message sink
		message_source = gr.message_source(self._item_size*self._vlen, 1)
		self._msgq = message_source.msgq()
		#connect
		self.connect(message_source, self)

	def push(self, item):
		"""
		Push an item into the back of the queue.
		@param item the item
		"""
		if self._vlen == 1: item = [item]
		arr = numpy.array(item, self._numpy)
		msg = gr.message_from_string(arr.tostring(), 0, self._item_size, self._vlen)
		self._msgq.insert_tail(msg)

class queue_source_c(_queue_source_base):
	_item_size = gr.sizeof_gr_complex
	_numpy = numpy.complex64

class queue_source_f(_queue_source_base):
	_item_size = gr.sizeof_float
	_numpy = numpy.float32

class queue_source_i(_queue_source_base):
	_item_size = gr.sizeof_int
	_numpy = numpy.int32

class queue_source_s(_queue_source_base):
	_item_size = gr.sizeof_short
	_numpy = numpy.int16

class queue_source_b(_queue_source_base):
	_item_size = gr.sizeof_char
	_numpy = numpy.int8
