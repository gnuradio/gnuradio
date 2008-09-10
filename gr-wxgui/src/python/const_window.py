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

##################################################
# Constants
##################################################
SLIDER_STEPS = 200
ALPHA_MIN_EXP, ALPHA_MAX_EXP = -6, -0.301
GAIN_MU_MIN_EXP, GAIN_MU_MAX_EXP = -6, -0.301
DEFAULT_FRAME_RATE = 5
DEFAULT_WIN_SIZE = (500, 400)
DEFAULT_CONST_SIZE = 2048
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
		@param parent the wx parent window
		"""
		self.parent = parent
		wx.Panel.__init__(self, parent, -1, style=wx.SUNKEN_BORDER)
		control_box = wx.BoxSizer(wx.VERTICAL)
		self.marker_index = 2
		#begin control box
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Options'), 0, wx.ALIGN_CENTER)
		#marker
		control_box.AddStretchSpacer()
		self.marker_chooser = common.DropDownController(self, 'Marker', MARKER_TYPES, parent, MARKER_KEY)
		control_box.Add(self.marker_chooser, 0, wx.EXPAND)
		#alpha
		control_box.AddStretchSpacer()
		self.alpha_slider = common.LogSliderController(
			self, 'Alpha',
			ALPHA_MIN_EXP, ALPHA_MAX_EXP, SLIDER_STEPS,
			parent.ext_controller, parent.alpha_key,
		)
		control_box.Add(self.alpha_slider, 0, wx.EXPAND)
		#gain_mu
		control_box.AddStretchSpacer()
		self.gain_mu_slider = common.LogSliderController(
			self, 'Gain Mu',
			GAIN_MU_MIN_EXP, GAIN_MU_MAX_EXP, SLIDER_STEPS,
			parent.ext_controller, parent.gain_mu_key,
		)
		control_box.Add(self.gain_mu_slider, 0, wx.EXPAND)
		#run/stop
		control_box.AddStretchSpacer()
		self.run_button = common.ToggleButtonController(self, parent, RUNNING_KEY, 'Stop', 'Run')
		control_box.Add(self.run_button, 0, wx.EXPAND)
		#set sizer
		self.SetSizerAndFit(control_box)

##################################################
# Constellation window with plotter and control panel
##################################################
class const_window(wx.Panel, pubsub.pubsub, common.prop_setter):
	def __init__(
		self,
		parent,
		controller,
		size,
		title,
		msg_key,
		alpha_key,
		beta_key,
		gain_mu_key,
		gain_omega_key,
	):
		pubsub.pubsub.__init__(self)
		#setup
		self.ext_controller = controller
		self.alpha_key = alpha_key
		self.beta_key = beta_key
		self.gain_mu_key = gain_mu_key
		self.gain_omega_key = gain_omega_key
		#init panel and plot
		wx.Panel.__init__(self, parent, -1, style=wx.SIMPLE_BORDER)
		self.plotter = plotter.channel_plotter(self)
		self.plotter.SetSize(wx.Size(*size))
		self.plotter.set_title(title)
		self.plotter.set_x_label('Inphase')
		self.plotter.set_y_label('Quadrature')
		self.plotter.enable_point_label(True)
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#alpha and gain mu 2nd orders
		def set_beta(alpha): self.ext_controller[self.beta_key] = .25*alpha**2
		self.ext_controller.subscribe(self.alpha_key, set_beta)
		def set_gain_omega(gain_mu): self.ext_controller[self.gain_omega_key] = .25*gain_mu**2
		self.ext_controller.subscribe(self.gain_mu_key, set_gain_omega)
		#initial setup
		self.ext_controller[self.alpha_key] = self.ext_controller[self.alpha_key]
		self.ext_controller[self.gain_mu_key] = self.ext_controller[self.gain_mu_key]
		self._register_set_prop(self, RUNNING_KEY, True)
		self._register_set_prop(self, X_DIVS_KEY, 8)
		self._register_set_prop(self, Y_DIVS_KEY, 8)
		self._register_set_prop(self, MARKER_KEY, DEFAULT_MARKER_TYPE)
		#register events
		self.ext_controller.subscribe(msg_key, self.handle_msg)
		for key in (
			X_DIVS_KEY, Y_DIVS_KEY,
		): self.subscribe(key, self.update_grid)
		#initial update
		self.update_grid()

	def handle_msg(self, msg):
		"""
		Plot the samples onto the complex grid.
		@param msg the array of complex samples
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
		#grid parameters
		x_divs = self[X_DIVS_KEY]
		y_divs = self[Y_DIVS_KEY]
		#update the x axis
		x_max = 2.0
		self.plotter.set_x_grid(-x_max, x_max, common.get_clean_num(2.0*x_max/x_divs))
		#update the y axis
		y_max = 2.0
		self.plotter.set_y_grid(-y_max, y_max, common.get_clean_num(2.0*y_max/y_divs))
		#update plotter
		self.plotter.update()




