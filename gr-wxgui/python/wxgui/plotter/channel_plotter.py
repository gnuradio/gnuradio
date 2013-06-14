#
# Copyright 2008, 2009, 2010 Free Software Foundation, Inc.
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
from grid_plotter_base import grid_plotter_base
from OpenGL import GL
import common
import numpy
import gltext
import math

LEGEND_TEXT_FONT_SIZE = 8
LEGEND_BOX_PADDING = 3
MIN_PADDING = 35, 10, 0, 0 #top, right, bottom, left
#constants for the waveform storage
SAMPLES_KEY = 'samples'
COLOR_SPEC_KEY = 'color_spec'
MARKERY_KEY = 'marker'
TRIG_OFF_KEY = 'trig_off'

##################################################
# Channel Plotter for X Y Waveforms
##################################################
class channel_plotter(grid_plotter_base):

	def __init__(self, parent):
		"""
		Create a new channel plotter.
		"""
		#init
		grid_plotter_base.__init__(self, parent, MIN_PADDING)
                self.set_use_persistence(False)
		#setup legend cache
		self._legend_cache = self.new_gl_cache(self._draw_legend, 50)
		self.enable_legend(False)
		#setup waveform cache
		self._waveform_cache = self.new_gl_cache(self._draw_waveforms, 50)
		self._channels = dict()
		#init channel plotter
		self.register_init(self._init_channel_plotter)
		self.callback = None

	def _init_channel_plotter(self):
		"""
		Run gl initialization tasks.
		"""
		GL.glEnableClientState(GL.GL_VERTEX_ARRAY)

	def enable_legend(self, enable=None):
		"""
		Enable/disable the legend.

                Args:
		    enable: true to enable

                Returns:
		    the enable state when None
		"""
		if enable is None: return self._enable_legend
		self.lock()
		self._enable_legend = enable
		self._legend_cache.changed(True)
		self.unlock()

	def _draw_waveforms(self):
		"""
		Draw the waveforms for each channel.
		Scale the waveform data to the grid using gl matrix operations.
		"""
		#use scissor to prevent drawing outside grid
		GL.glEnable(GL.GL_SCISSOR_TEST)
		GL.glScissor(
			self.padding_left+1,
			self.padding_bottom+1,
			self.width-self.padding_left-self.padding_right-1,
			self.height-self.padding_top-self.padding_bottom-1,
		)
		for channel in reversed(sorted(self._channels.keys())):
			samples = self._channels[channel][SAMPLES_KEY]
			num_samps = len(samples)
			if not num_samps: continue
			#use opengl to scale the waveform
			GL.glPushMatrix()
			GL.glTranslatef(self.padding_left, self.padding_top, 0)
			GL.glScalef(
				(self.width-self.padding_left-self.padding_right),
				(self.height-self.padding_top-self.padding_bottom),
				1,
			)
			GL.glTranslatef(0, 1, 0)
			if isinstance(samples, tuple):
				x_scale, x_trans = 1.0/(self.x_max-self.x_min), -self.x_min
				points = zip(*samples)
			else:
				x_scale, x_trans = 1.0/(num_samps-1), -self._channels[channel][TRIG_OFF_KEY]
				points = zip(numpy.arange(0, num_samps), samples)
			GL.glScalef(x_scale, -1.0/(self.y_max-self.y_min), 1)
			GL.glTranslatef(x_trans, -self.y_min, 0)
			#draw the points/lines
			GL.glColor3f(*self._channels[channel][COLOR_SPEC_KEY])
			marker = self._channels[channel][MARKERY_KEY]
			if marker is None:
				GL.glVertexPointerf(points)
				GL.glDrawArrays(GL.GL_LINE_STRIP, 0, len(points))
			elif isinstance(marker, (int, float)) and marker > 0:
				GL.glPointSize(marker)
				GL.glVertexPointerf(points)
				GL.glDrawArrays(GL.GL_POINTS, 0, len(points))
			GL.glPopMatrix()
		GL.glDisable(GL.GL_SCISSOR_TEST)

	def _populate_point_label(self, x_val, y_val):
		"""
		Get the text the will populate the point label.
		Give X and Y values for the current point.
		Give values for the channel at the X coordinate.

                Args:
		    x_val: the current x value
		    y_val: the current y value

                Returns:
		    a string with newlines
		"""
		#create text
		label_str = '%s: %s\n%s: %s'%(
			self.x_label, common.eng_format(x_val, self.x_units),
			self.y_label, common.eng_format(y_val, self.y_units),
		)
		for channel in sorted(self._channels.keys()):
			samples = self._channels[channel][SAMPLES_KEY]
			num_samps = len(samples)
			if not num_samps: continue
			if isinstance(samples, tuple): continue
			#linear interpolation
			x_index = (num_samps-1)*(x_val-self.x_min)/(self.x_max-self.x_min)
			x_index_low = int(math.floor(x_index))
			x_index_high = int(math.ceil(x_index))
			scale = x_index - x_index_low + self._channels[channel][TRIG_OFF_KEY]
			y_value = (samples[x_index_high] - samples[x_index_low])*scale + samples[x_index_low]
			if math.isnan(y_value): continue
			label_str += '\n%s: %s'%(channel, common.eng_format(y_value, self.y_units))
		return label_str

	def _call_callback (self, x_val, y_val):
		if self.callback != None:
			self.callback(x_val, y_val)

	def set_callback (self, callback):
		self.callback = callback

	def _draw_legend(self):
		"""
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
			GL.glColor3f(*color_spec)
			self._draw_rect(
				x_off - w - LEGEND_BOX_PADDING,
				self.padding_top/2 - h/2 - LEGEND_BOX_PADDING,
				w+2*LEGEND_BOX_PADDING,
				h+2*LEGEND_BOX_PADDING,
			)
			txt.draw_text(wx.Point(x_off - w, self.padding_top/2 - h/2))
			x_off -= w + 4*LEGEND_BOX_PADDING

	def clear_waveform(self, channel):
		"""
		Remove a waveform from the list of waveforms.

                Args:
		    channel: the channel key
		"""
		self.lock()
		if channel in self._channels.keys():
			self._channels.pop(channel)
			self._legend_cache.changed(True)
			self._waveform_cache.changed(True)
		self.unlock()

	def set_waveform(self, channel, samples=[], color_spec=(0, 0, 0), marker=None, trig_off=0):
		"""
		Set the waveform for a given channel.

                Args:
		    channel: the channel key
		    samples: the waveform samples
		    color_spec: the 3-tuple for line color
		    marker: None for line
		    trig_off: fraction of sample for trigger offset
		"""
		self.lock()
		if channel not in self._channels.keys(): self._legend_cache.changed(True)
		self._channels[channel] = {
			SAMPLES_KEY: samples,
			COLOR_SPEC_KEY: color_spec,
			MARKERY_KEY: marker,
			TRIG_OFF_KEY: trig_off,
		}
		self._waveform_cache.changed(True)
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
