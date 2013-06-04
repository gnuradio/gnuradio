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
import plotter
import common
import wx
import numpy
import math
import pubsub
from constants import *
from gnuradio import gr #for gr.prefs
import forms

##################################################
# Constants
##################################################
SLIDER_STEPS = 200
LOOP_BW_MIN_EXP, LOOP_BW_MAX_EXP = -6, 0.0
GAIN_MU_MIN_EXP, GAIN_MU_MAX_EXP = -6, -0.301
DEFAULT_FRAME_RATE = gr.prefs().get_long('wxgui', 'const_rate', 5)
DEFAULT_WIN_SIZE = (500, 400)
DEFAULT_CONST_SIZE = gr.prefs().get_long('wxgui', 'const_size', 2048)
CONST_PLOT_COLOR_SPEC = (0, 0, 1)
MARKER_TYPES = (
	('Dot Small', 1.0),
	('Dot Medium', 2.0),
	('Dot Large', 3.0),
	('Line Link', None),
)
DEFAULT_MARKER_TYPE = 2.0

##################################################
# Constellation window control panel
##################################################
class control_panel(wx.Panel):
	"""
	A control panel with wx widgits to control the plotter.
	"""
	def __init__(self, parent):
		"""
		Create a new control panel.

                Args:
		    parent: the wx parent window
		"""
		self.parent = parent
		wx.Panel.__init__(self, parent, style=wx.SUNKEN_BORDER)
		parent[SHOW_CONTROL_PANEL_KEY] = True
		parent.subscribe(SHOW_CONTROL_PANEL_KEY, self.Show)
		control_box = forms.static_box_sizer(
			parent=self, label='Options',
			bold=True, orient=wx.VERTICAL,
		)
		#loop_bw
		control_box.AddStretchSpacer()
		forms.text_box(
			sizer=control_box, parent=self, label='Loop Bandwidth',
			converter=forms.float_converter(),
			ps=parent, key=LOOP_BW_KEY,
		)
		forms.log_slider(
			sizer=control_box, parent=self,
			min_exp=LOOP_BW_MIN_EXP,
			max_exp=LOOP_BW_MAX_EXP,
			num_steps=SLIDER_STEPS,
			ps=parent, key=LOOP_BW_KEY,
		)
		#gain_mu
		control_box.AddStretchSpacer()
		forms.text_box(
			sizer=control_box, parent=self, label='Gain Mu',
			converter=forms.float_converter(),
			ps=parent, key=GAIN_MU_KEY,
		)
		forms.log_slider(
			sizer=control_box, parent=self,
			min_exp=GAIN_MU_MIN_EXP,
			max_exp=GAIN_MU_MAX_EXP,
			num_steps=SLIDER_STEPS,
			ps=parent, key=GAIN_MU_KEY,
		)
		#marker
		control_box.AddStretchSpacer()
		forms.drop_down(
			sizer=control_box, parent=self,
			ps=parent, key=MARKER_KEY, label='Marker',
			choices=map(lambda x: x[1], MARKER_TYPES),
			labels=map(lambda x: x[0], MARKER_TYPES),
		)
		#run/stop
		control_box.AddStretchSpacer()
		forms.toggle_button(
			sizer=control_box, parent=self,
			true_label='Stop', false_label='Run',
			ps=parent, key=RUNNING_KEY,
		)
		#set sizer
		self.SetSizerAndFit(control_box)

##################################################
# Constellation window with plotter and control panel
##################################################
class const_window(wx.Panel, pubsub.pubsub):
	def __init__(
		self,
		parent,
		controller,
		size,
		title,
		msg_key,
		loop_bw_key,
		gain_mu_key,
		gain_omega_key,
		omega_key,
		sample_rate_key,
	):
		pubsub.pubsub.__init__(self)
		#proxy the keys
		self.proxy(MSG_KEY, controller, msg_key)
		self.proxy(LOOP_BW_KEY, controller, loop_bw_key)
		self.proxy(GAIN_MU_KEY, controller, gain_mu_key)
		self.proxy(GAIN_OMEGA_KEY, controller, gain_omega_key)
		self.proxy(OMEGA_KEY, controller, omega_key)
		self.proxy(SAMPLE_RATE_KEY, controller, sample_rate_key)
		#initialize values
		self[RUNNING_KEY] = True
		self[X_DIVS_KEY] = 8
		self[Y_DIVS_KEY] = 8
		self[MARKER_KEY] = DEFAULT_MARKER_TYPE
		#init panel and plot
		wx.Panel.__init__(self, parent, style=wx.SIMPLE_BORDER)
		self.plotter = plotter.channel_plotter(self)
		self.plotter.SetSize(wx.Size(*size))
		self.plotter.SetSizeHints(*size)
		self.plotter.set_title(title)
		self.plotter.set_x_label('Inphase')
		self.plotter.set_y_label('Quadrature')
		self.plotter.enable_point_label(True)
		self.plotter.enable_grid_lines(True)
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#alpha and gain mu 2nd orders
		def set_gain_omega(gain_mu): self[GAIN_OMEGA_KEY] = .25*gain_mu**2
		self.subscribe(GAIN_MU_KEY, set_gain_omega)
		#register events
		self.subscribe(MSG_KEY, self.handle_msg)
		self.subscribe(X_DIVS_KEY, self.update_grid)
		self.subscribe(Y_DIVS_KEY, self.update_grid)
		#initial update
		self.update_grid()

	def handle_msg(self, msg):
		"""
		Plot the samples onto the complex grid.

                Args:
		    msg: the array of complex samples
		"""
		if not self[RUNNING_KEY]: return
		#convert to complex floating point numbers
		samples = numpy.fromstring(msg, numpy.complex64)
		real = numpy.real(samples)
		imag = numpy.imag(samples)
		#plot
		self.plotter.set_waveform(
			channel=0,
			samples=(real, imag),
			color_spec=CONST_PLOT_COLOR_SPEC,
			marker=self[MARKER_KEY],
		)
		#update the plotter
		self.plotter.update()

	def update_grid(self):
		#update the x axis
		x_max = 2.0
		self.plotter.set_x_grid(-x_max, x_max, common.get_clean_num(2.0*x_max/self[X_DIVS_KEY]))
		#update the y axis
		y_max = 2.0
		self.plotter.set_y_grid(-y_max, y_max, common.get_clean_num(2.0*y_max/self[Y_DIVS_KEY]))
		#update plotter
		self.plotter.update()




