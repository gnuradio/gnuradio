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

default_win_size = 1000

from gnuradio import gr
import gnuradio.gr.gr_threading as _threading
import numpy

#generate 1s counts array
_1s_counts = [sum([1&(i>>j) for j in range(8)]) for i in range(2**8)]

class input_watcher(_threading.Thread):
	"""
	Read samples from the message queue and hand them to the callback.
	"""

	def __init__(self, msgq, callback):
		self._msgq = msgq
		self._callback = callback
		_threading.Thread.__init__(self)
		self.setDaemon(1)
		self.keep_running = True
		self.start()

	def run(self):
		r = ''
		while True:
			msg = self._msgq.delete_head()
			itemsize = int(msg.arg1())
			nitems = int(msg.arg2())
			s = r + msg.to_string()
			i = (nitems-nitems%2)*itemsize
			r = s[i:]
			s = s[:i]
			samples = numpy.fromstring(s, numpy.int8)
			self._callback(samples)

class error_rate(gr.hier_block2):
	"""
	Sample the incoming data streams (byte) and calculate the bit or symbol error rate.
	Write the running rate to the output data stream (float).
	"""

	def __init__(self, type='BER', win_size=default_win_size, bits_per_symbol=2):
		"""
		Error rate constructor.
		@param type a string 'BER' or 'SER'
		@param win_size the number of samples to calculate over
		@param bits_per_symbol the number of information bits per symbol (BER only)
		"""
		#init
		gr.hier_block2.__init__(
			self, 'error_rate',
			gr.io_signature(2, 2, gr.sizeof_char),
			gr.io_signature(1, 1, gr.sizeof_float),
		)
		assert type in ('BER', 'SER')
		self._max_samples = win_size
		self._bits_per_symbol = bits_per_symbol
		#setup message queue
		msg_source = gr.message_source(gr.sizeof_float, 1)
		self._msgq_source = msg_source.msgq()
		msgq_sink = gr.msg_queue(2)
		msg_sink = gr.message_sink(gr.sizeof_char, msgq_sink, False) #False -> blocking
		inter = gr.interleave(gr.sizeof_char)
		#start thread
		self._num_errs = 0
		self._err_index = 0
		self._num_samps = 0
		self._err_array = numpy.zeros(self._max_samples, numpy.int8)
		if type == 'BER':
			input_watcher(msgq_sink, self._handler_ber)
		elif type == 'SER':
			input_watcher(msgq_sink, self._handler_ser)
		#connect
		self.connect(msg_source, self)
		self.connect((self, 0), (inter, 0))
		self.connect((self, 1), (inter, 1))
		self.connect(inter, msg_sink)

	def _handler_ber(self, samples):
		num = len(samples)/2
		arr = numpy.zeros(num, numpy.float32)
		for i in range(num):
			old_err = self._err_array[self._err_index]
			#record error
			self._err_array[self._err_index] = _1s_counts[samples[i*2] ^ samples[i*2 + 1]]
			self._num_errs = self._num_errs + self._err_array[self._err_index] - old_err
			#increment index
			self._err_index = (self._err_index + 1)%self._max_samples
			self._num_samps = min(self._num_samps + 1, self._max_samples)
			#write sample
			arr[i] = float(self._num_errs)/float(self._num_samps*self._bits_per_symbol)
		#write message
		msg = gr.message_from_string(arr.tostring(), 0, gr.sizeof_float, num)
		self._msgq_source.insert_tail(msg)

	def _handler_ser(self, samples):
		num = len(samples)/2
		arr = numpy.zeros(num, numpy.float32)
		for i in range(num):
			old_err = self._err_array[self._err_index]
			#record error
			ref = samples[i*2]
			res = samples[i*2 + 1]
			if ref == res:
				self._err_array[self._err_index] = 0
			else:
				self._err_array[self._err_index] = 1
			#update number of errors
			self._num_errs = self._num_errs + self._err_array[self._err_index] - old_err
			#increment index
			self._err_index = (self._err_index + 1)%self._max_samples
			self._num_samps = min(self._num_samps + 1, self._max_samples)
			#write sample
			arr[i] = float(self._num_errs)/float(self._num_samps)
		#write message
		msg = gr.message_from_string(arr.tostring(), 0, gr.sizeof_float, num)
		self._msgq_source.insert_tail(msg)
