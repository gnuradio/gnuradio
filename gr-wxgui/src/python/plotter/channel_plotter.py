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

import wx
from plotter_base import grid_plotter_base
from OpenGL.GL import *
from gnuradio.wxgui import common
import numpy
import gltext
import math

LEGEND_TEXT_FONT_SIZE = 8
LEGEND_BOX_PADDING = 3
PADDING = 35, 15, 40, 60 #top, right, bottom, left
#constants for the waveform storage
SAMPLES_KEY = 'samples'
COLOR_SPEC_KEY = 'color_spec'
MARKERY_KEY = 'marker'

##################################################
# Channel Plotter for X Y Waveforms
##################################################
class channel_plotter(grid_plotter_base):

	def __init__(self, parent):
		"""!
		Create a new channel plotter.
		"""
		#init
		grid_plotter_base.__init__(self, parent, PADDING)
		self._channels = dict()
		self.enable_legend(False)

	def _gl_init(self):
		"""!
		Run gl initialization tasks.
		"""
		glEnableClientState(GL_VERTEX_ARRAY)
		self._grid_compiled_list_id = glGenLists(1)

	def enable_legend(self, enable=None):
		"""!
		Enable/disable the legend.
		@param enable true to enable
		@return the enable state when None
		"""
		if enable is None: return self._enable_legend
		self.lock()
		self._enable_legend = enable
		self.changed(True)
		self.unlock()

	def draw(self):
		"""!
		Draw the grid and waveforms.
		"""
		self.lock()
		self.clear()
		#store the grid drawing operations
		if self.changed():
			glNewList(self._grid_compiled_list_id, GL_COMPILE)
			self._draw_grid()
			self._draw_legend()
			glEndList()
			self.changed(False)
		#draw the grid
		glCallList(self._grid_compiled_list_id)
		#use scissor to prevent drawing outside grid
		glEnable(GL_SCISSOR_TEST)
		glScissor(
			self.padding_left+1,
			self.padding_bottom+1,
			self.width-self.padding_left-self.padding_right-1,
			self.height-self.padding_top-self.padding_bottom-1,
		)
		#draw the waveforms
		self._draw_waveforms()
		glDisable(GL_SCISSOR_TEST)
		self._draw_point_label()
		#swap buffer into display
		self.SwapBuffers()
		self.unlock()

	def _draw_waveforms(self):
		"""!
		Draw the waveforms for each channel.
		Scale the waveform data to the grid using gl matrix operations.
		"""
		for channel in reversed(sorted(self._channels.keys())):
			samples = self._channels[channel][SAMPLES_KEY]
			num_samps = len(samples)
			if not num_samps: continue
			#use opengl to scale the waveform
			glPushMatrix()
			glTranslatef(self.padding_left, self.padding_top, 0)
			glScalef(
				(self.width-self.padding_left-self.padding_right),
				(self.height-self.padding_top-self.padding_bottom),
				1,
			)
			glTranslatef(0, 1, 0)
			if isinstance(samples, tuple):
				x_scale, x_trans = 1.0/(self.x_max-self.x_min), -self.x_min
				points = zip(*samples)
			else:
				x_scale, x_trans = 1.0/(num_samps-1), 0
				points = zip(numpy.arange(0, num_samps), samples)
			glScalef(x_scale, -1.0/(self.y_max-self.y_min), 1)
			glTranslatef(x_trans, -self.y_min, 0)
			#draw the points/lines
			glColor3f(*self._channels[channel][COLOR_SPEC_KEY])
			marker = self._channels[channel][MARKERY_KEY]
			if marker: glPointSize(marker)
			glVertexPointerf(points)
			glDrawArrays(marker is None and GL_LINE_STRIP or GL_POINTS, 0, len(points))
			glPopMatrix()

	def _populate_point_label(self, x_val, y_val):
		"""!
		Get the text the will populate the point label.
		Give X and Y values for the current point.
		Give values for the channel at the X coordinate.
		@param x_val the current x value
		@param y_val the current y value
		@return a string with newlines
		"""
		#create text
		label_str = '%s: %s %s\n%s: %s %s'%(
			self.x_label,
			common.label_format(x_val),
			self.x_units, self.y_label,
			common.label_format(y_val),
			self.y_units,
		)
		for channel in sorted(self._channels.keys()):
			samples = self._channels[channel][SAMPLES_KEY]
			num_samps = len(samples)
			if not num_samps: continue
			if isinstance(samples, tuple): continue
			#linear interpolation
			x_index = (num_samps-1)*(x_val/self.x_scalar-self.x_min)/(self.x_max-self.x_min)
			x_index_low = int(math.floor(x_index))
			x_index_high = int(math.ceil(x_index))
			y_value = (samples[x_index_high] - samples[x_index_low])*(x_index - x_index_low) + samples[x_index_low]
			label_str += '\n%s: %s %s'%(channel, common.label_format(y_value), self.y_units)
		return label_str

	def _draw_legend(self):
		"""!
		Draw the legend in the upper right corner.
		For each channel, draw a rectangle out of the channel color,
		and overlay the channel text on top of the rectangle.
		"""
		if not self.enable_legend(): return
		x_off = self.width - self.padding_right - LEGEND_BOX_PADDING
		for i, channel in enumerate(reversed(sorted(self._channels.keys()))):
			samples = self._channels[channel][SAMPLES_KEY]
			if not len(samples): continue
			color_spec = self._channels[channel][COLOR_SPEC_KEY]
			txt = gltext.Text(channel, font_size=LEGEND_TEXT_FONT_SIZE)
			w, h = txt.get_size()
			#draw rect + text
			glColor3f(*color_spec)
			self._draw_rect(
				x_off - w - LEGEND_BOX_PADDING,
				self.padding_top/2 - h/2 - LEGEND_BOX_PADDING,
				w+2*LEGEND_BOX_PADDING,
				h+2*LEGEND_BOX_PADDING,
			)
			txt.draw_text(wx.Point(x_off - w, self.padding_top/2 - h/2))
			x_off -= w + 4*LEGEND_BOX_PADDING

	def set_waveform(self, channel, samples, color_spec, marker=None):
		"""!
		Set the waveform for a given channel.
		@param channel the channel key
		@param samples the waveform samples
		@param color_spec the 3-tuple for line color
		@param marker None for line
		"""
		self.lock()
		if channel not in self._channels.keys(): self.changed(True)
		self._channels[channel] = {
			SAMPLES_KEY: samples,
			COLOR_SPEC_KEY: color_spec,
			MARKERY_KEY: marker,
		}
		self.unlock()

if __name__ == '__main__':
	app = wx.PySimpleApp()
	frame = wx.Frame(None, -1, 'Demo', wx.DefaultPosition)
	vbox = wx.BoxSizer(wx.VERTICAL)

	plotter = channel_plotter(frame)
	plotter.set_x_grid(-1, 1, .2)
	plotter.set_y_grid(-1, 1, .4)
	vbox.Add(plotter, 1, wx.EXPAND)

	plotter = channel_plotter(frame)
	plotter.set_x_grid(-1, 1, .2)
	plotter.set_y_grid(-1, 1, .4)
	vbox.Add(plotter, 1, wx.EXPAND)

	frame.SetSizerAndFit(vbox)
	frame.SetSize(wx.Size(800, 600))
	frame.Show()
	app.MainLoop()
