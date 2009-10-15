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
DEFAULT_WIN_SIZE = (600, 300)

##################################################
# histo window control panel
##################################################
class control_panel(wx.Panel):
	"""
	A control panel with wx widgits to control the plotter and histo sink.
	"""

	def __init__(self, parent):
		"""
		Create a new control panel.
		@param parent the wx parent window
		"""
		self.parent = parent
		wx.Panel.__init__(self, parent, style=wx.SUNKEN_BORDER)
		parent[SHOW_CONTROL_PANEL_KEY] = True
		parent.subscribe(SHOW_CONTROL_PANEL_KEY, self.Show)
		control_box = wx.BoxSizer(wx.VERTICAL)
		SIZE = (100, -1)
		control_box = forms.static_box_sizer(
			parent=self, label='Options',
			bold=True, orient=wx.VERTICAL,
		)
		#num bins
		control_box.AddStretchSpacer()
		forms.text_box(
			sizer=control_box, parent=self, label='Num Bins',
			converter=forms.int_converter(),
			ps=parent, key=NUM_BINS_KEY,
		)
		#frame size
		control_box.AddStretchSpacer()
		forms.text_box(
			sizer=control_box, parent=self, label='Frame Size',
			converter=forms.int_converter(),
			ps=parent, key=FRAME_SIZE_KEY,
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
# histo window with plotter and control panel
##################################################
class histo_window(wx.Panel, pubsub.pubsub):
	def __init__(
		self,
		parent,
		controller,
		size,
		title,
		maximum_key,
		minimum_key,
		num_bins_key,
		frame_size_key,
		msg_key,
	):
		pubsub.pubsub.__init__(self)
		#setup
		self.samples = list()
		#proxy the keys
		self.proxy(MAXIMUM_KEY, controller, maximum_key)
		self.proxy(MINIMUM_KEY, controller, minimum_key)
		self.proxy(NUM_BINS_KEY, controller, num_bins_key)
		self.proxy(FRAME_SIZE_KEY, controller, frame_size_key)
		self.proxy(MSG_KEY, controller, msg_key)
		#initialize values
		self[RUNNING_KEY] = True
		self[X_DIVS_KEY] = 8
		self[Y_DIVS_KEY] = 4
		#init panel and plot
		wx.Panel.__init__(self, parent, style=wx.SIMPLE_BORDER)
		self.plotter = plotter.bar_plotter(self)
		self.plotter.SetSize(wx.Size(*size))
		self.plotter.set_title(title)
		self.plotter.enable_point_label(True)
		self.plotter.enable_grid_lines(False)
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#register events
		self.subscribe(MSG_KEY, self.handle_msg)
		self.subscribe(X_DIVS_KEY, self.update_grid)
		self.subscribe(Y_DIVS_KEY, self.update_grid)

	def handle_msg(self, msg):
		"""
		Handle the message from the fft sink message queue.
		@param msg the frame as a character array
		"""
		if not self[RUNNING_KEY]: return
		#convert to floating point numbers
		self.samples = 100*numpy.fromstring(msg, numpy.float32)[:self[NUM_BINS_KEY]] #only take first frame
		self.plotter.set_bars(
			bars=self.samples,
			bar_width=0.6,
			color_spec=(0, 0, 1),
		)
		self.update_grid()

	def update_grid(self):
		if not len(self.samples): return
		#calculate the maximum y value
		y_off = math.ceil(numpy.max(self.samples))
		y_off = min(max(y_off, 1.0), 100.0) #between 1% and 100%
		#update the x grid
		self.plotter.set_x_grid(
			self[MINIMUM_KEY], self[MAXIMUM_KEY],
			common.get_clean_num((self[MAXIMUM_KEY] - self[MINIMUM_KEY])/self[X_DIVS_KEY]),
		)
		self.plotter.set_x_label('Counts')
		#update the y grid
		self.plotter.set_y_grid(0, y_off, y_off/self[Y_DIVS_KEY])
		self.plotter.set_y_label('Frequency', '%')
		self.plotter.update()
