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

from gnuradio import gr, packet_utils
import gnuradio.gr.gr_threading as _threading

##payload length in bytes
DEFAULT_PAYLOAD_LEN = 512

##how many messages in a queue
DEFAULT_MSGQ_LIMIT = 2

##threshold for unmaking packets
DEFAULT_THRESHOLD = 12

#######################################################################################
##	Packet Encoder
#######################################################################################

class _packet_encoder_thread(_threading.Thread):
	
	def __init__(self, msgq, payload_length, send):
		self._msgq = msgq
		self._payload_length = payload_length
		self._send = send
		_threading.Thread.__init__(self)
		self.setDaemon(1)
		self.keep_running = True
		self.start()
		
	def run(self):
		sample = '' #residual sample
		while self.keep_running:
			msg = self._msgq.delete_head() #blocking read of message queue
			sample = sample + msg.to_string() #get the body of the msg as a string	
			while len(sample) >= self._payload_length:		
				payload = sample[0:self._payload_length]
				sample = sample[self._payload_length:]			
				self._send(payload)	

class packet_encoder(gr.hier_block2):
	"""
	Hierarchical block for wrapping packet-based modulators.
	"""
	
	def __init__(self, item_size_in, samples_per_symbol, bits_per_symbol, access_code='', pad_for_usrp=True, payload_length=-1):
		"""!
		packet_mod constructor.
		@param item_size_in the size of the input data stream in bytes
		@param samples_per_symbol number of samples per symbol
		@param bits_per_symbol number of bits per symbol
		@param access_code AKA sync vector
		@param pad_for_usrp If true, packets are padded such that they end up a multiple of 128 samples
		@param payload_length number of bytes in a data-stream slice
		"""
		#setup parameters
		self._item_size_in = item_size_in
		self._samples_per_symbol = samples_per_symbol
		self._bits_per_symbol = bits_per_symbol
		self._pad_for_usrp = pad_for_usrp
		if not access_code: #get access code
			access_code = packet_utils.default_access_code
		if not packet_utils.is_1_0_string(access_code):
			raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)
		self._access_code = access_code		
		self._pad_for_usrp = pad_for_usrp		
		if payload_length < 0: #get payload length
			payload_length = DEFAULT_PAYLOAD_LEN
		if payload_length%self._item_size_in != 0:	#verify that packet length is a multiple of the stream size
			raise ValueError, 'The packet length: "%d" is not a mutiple of the stream size: "%d".'%(payload_length, self._item_size_in)
		self._payload_length = payload_length
		#create blocks
		msg_source = gr.message_source(gr.sizeof_char, DEFAULT_MSGQ_LIMIT)
		self._msgq_out = msg_source.msgq()		
		self._msgq_in = gr.msg_queue(DEFAULT_MSGQ_LIMIT)
		msg_sink = gr.message_sink(self._item_size_in, self._msgq_in, False) #False -> blocking		
		#initialize hier2
		gr.hier_block2.__init__(
			self, 
			"packet_encoder",
			gr.io_signature(1, 1, self._item_size_in), # Input signature
			gr.io_signature(1, 1, gr.sizeof_char) # Output signature
		)
		#connect
		self.connect(self, msg_sink)
		self.connect(msg_source, self)
		#start thread
		_packet_encoder_thread(self._msgq_in, self._payload_length, self._send_packet)
		
	def _send_packet(self, payload):
		"""!
		Wrap the payload in a packet and push onto the message queue.
		@param payload string, data to send
		"""	
		packet = packet_utils.make_packet(
			payload,
			self._samples_per_symbol,
			self._bits_per_symbol,
			self._access_code,
			self._pad_for_usrp
		)
		msg = gr.message_from_string(packet)
		self._msgq_out.insert_tail(msg)
	
#######################################################################################
##	Packet Decoder
#######################################################################################

class _packet_decoder_thread(_threading.Thread):
    
    def __init__(self, msgq, callback):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self._msgq = msgq
        self.callback = callback
        self.keep_running = True
        self.start()

    def run(self):
        while self.keep_running:
            msg = self._msgq.delete_head()
            ok, payload = packet_utils.unmake_packet(msg.to_string(), int(msg.arg1()))
            if self.callback:
                self.callback(ok, payload)

class packet_decoder(gr.hier_block2):
	"""
	Hierarchical block for wrapping packet-based demodulators.
	"""
	
	def __init__(self, item_size_out, access_code='', threshold=-1):
		"""!
		packet_demod constructor.
		@param item_size_out the size of the output data stream in bytes
		@param access_code AKA sync vector
		@param threshold detect access_code with up to threshold bits wrong (-1 -> use default)
		"""
		#setup
		self._item_size_out = item_size_out
		#access code
		if not access_code: #get access code
			access_code = packet_utils.default_access_code
		if not packet_utils.is_1_0_string(access_code):
			raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)
		self._access_code = access_code		
		#threshold
		if threshold < 0: threshold = DEFAULT_THRESHOLD 
		self._threshold = threshold  
		#blocks
		self._msgq_in = gr.msg_queue(DEFAULT_MSGQ_LIMIT) #holds packets from the PHY
		correlator = gr.correlate_access_code_bb(self._access_code, self._threshold)
		framer_sink = gr.framer_sink_1(self._msgq_in)		
		msg_source = gr.message_source(self._item_size_out, DEFAULT_MSGQ_LIMIT)
		self._msgq_out = msg_source.msgq()		
		#initialize hier2
		gr.hier_block2.__init__(
			self, 
			"packet_decoder",
			gr.io_signature(1, 1, gr.sizeof_char), # Input signature
			gr.io_signature(1, 1, self._item_size_out) # Output signature
		)
		#connect
		self.connect(self, correlator, framer_sink)
		self.connect(msg_source, self)
		#start thread
		_packet_decoder_thread(self._msgq_in, self._recv_packet)

	def _recv_packet(self, ok, payload):
		"""!
		Extract the payload from the packet and push onto message queue.
		@param ok boolean ok
		@param payload data received
		"""
		msg = gr.message_from_string(payload, 0, self._item_size_out, len(payload)/self._item_size_out)
		if ok: self._msgq_out.insert_tail(msg)			


