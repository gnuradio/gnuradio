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

##################################################
# Imports
##################################################
import const_window
import common
from gnuradio import gr, blks2
from pubsub import pubsub
from constants import *

##################################################
# Constellation sink block (wrapper for old wxgui)
##################################################
class const_sink_c(gr.hier_block2, common.wxgui_hb):
	"""
	A constellation block with a gui window.
	"""

	def __init__(
		self,
		parent,
		title='',
		sample_rate=1,
		size=const_window.DEFAULT_WIN_SIZE,
		frame_rate=const_window.DEFAULT_FRAME_RATE,
		const_size=const_window.DEFAULT_CONST_SIZE,
		#mpsk recv params
		M=4,
		theta=0,
		alpha=0.005,
		fmax=0.06,
		mu=0.5,
		gain_mu=0.005,
		symbol_rate=1,
		omega_limit=0.005,
	):
		#init
		gr.hier_block2.__init__(
			self,
			"const_sink",
			gr.io_signature(1, 1, gr.sizeof_gr_complex),
			gr.io_signature(0, 0, 0),
		)
		#blocks
		sd = blks2.stream_to_vector_decimator(
			item_size=gr.sizeof_gr_complex,
			sample_rate=sample_rate,
			vec_rate=frame_rate,
			vec_len=const_size,
		)
		beta = .25*alpha**2 #redundant, will be updated
		fmin = -fmax
		gain_omega = .25*gain_mu**2 #redundant, will be updated
		omega = 1 #set_sample_rate will update this
		# Costas frequency/phase recovery loop
		# Critically damped 2nd order PLL
		self._costas = gr.costas_loop_cc(alpha, beta, fmax, fmin, M)
		# Timing recovery loop
		# Critically damped 2nd order DLL
		self._retime = gr.clock_recovery_mm_cc(omega, gain_omega, mu, gain_mu, omega_limit)
		#sync = gr.mpsk_receiver_cc(
		#	M, #psk order
		#	theta,
		#	alpha,
		#	beta,
		#	fmin,
		#	fmax,
		#	mu,
		#	gain_mu,
		#	omega,
		#	gain_omega,
		#	omega_limit,
		#)
		agc = gr.feedforward_agc_cc(16, 1)
		msgq = gr.msg_queue(2)
		sink = gr.message_sink(gr.sizeof_gr_complex*const_size, msgq, True)
		#controller
		def setter(p, k, x): p[k] = x
		self.controller = pubsub()
		self.controller.subscribe(ALPHA_KEY, self._costas.set_alpha)
		self.controller.publish(ALPHA_KEY, self._costas.alpha)
		self.controller.subscribe(BETA_KEY, self._costas.set_beta)
		self.controller.publish(BETA_KEY, self._costas.beta)
		self.controller.subscribe(GAIN_MU_KEY, self._retime.set_gain_mu)
		self.controller.publish(GAIN_MU_KEY, self._retime.gain_mu)
		self.controller.subscribe(OMEGA_KEY, self._retime.set_omega)
		self.controller.publish(OMEGA_KEY, self._retime.omega)
		self.controller.subscribe(GAIN_OMEGA_KEY, self._retime.set_gain_omega)
		self.controller.publish(GAIN_OMEGA_KEY, self._retime.gain_omega)
		self.controller.subscribe(SAMPLE_RATE_KEY, sd.set_sample_rate)
		self.controller.subscribe(SAMPLE_RATE_KEY, lambda x: setter(self.controller, OMEGA_KEY, float(x)/symbol_rate))
		self.controller.publish(SAMPLE_RATE_KEY, sd.sample_rate)
		#initial update
		self.controller[SAMPLE_RATE_KEY] = sample_rate
		#start input watcher
		common.input_watcher(msgq, self.controller, MSG_KEY)
		#create window
		self.win = const_window.const_window(
			parent=parent,
			controller=self.controller,
			size=size,
			title=title,
			msg_key=MSG_KEY,
			alpha_key=ALPHA_KEY,
			beta_key=BETA_KEY,
			gain_mu_key=GAIN_MU_KEY,
			gain_omega_key=GAIN_OMEGA_KEY,
			omega_key=OMEGA_KEY,
			sample_rate_key=SAMPLE_RATE_KEY,
		)
		common.register_access_methods(self, self.win)
		#connect
		self.wxgui_connect(self, self._costas, self._retime, agc, sd, sink)


