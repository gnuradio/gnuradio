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

import wx
from grid_plotter_base import grid_plotter_base
from OpenGL import GL
import common
import numpy

LEGEND_TEXT_FONT_SIZE = 8
LEGEND_BOX_PADDING = 3
MIN_PADDING = 0, 0, 0, 70 #top, right, bottom, left
#constants for the waveform storage
SAMPLES_KEY = 'samples'
COLOR_SPEC_KEY = 'color_spec'
MARKERY_KEY = 'marker'
TRIG_OFF_KEY = 'trig_off'

##################################################
# Bar Plotter for histogram waveforms
##################################################
class bar_plotter(grid_plotter_base):

	def __init__(self, parent):
		"""
		Create a new bar plotter.
		"""
		#init
		grid_plotter_base.__init__(self, parent, MIN_PADDING)
		self._bars = list()
		self._bar_width = .5
		self._color_spec = (0, 0, 0)
		#setup bar cache
		self._bar_cache = self.new_gl_cache(self._draw_bars)
		#setup bar plotter
		self.register_init(self._init_bar_plotter)

	def _init_bar_plotter(self):
		"""
		Run gl initialization tasks.
		"""
		GL.glEnableClientState(GL.GL_VERTEX_ARRAY)

	def _draw_bars(self):
		"""
		Draw the vertical bars.
		"""
		bars = self._bars
		num_bars = len(bars)
		if num_bars == 0: return
		#use scissor to prevent drawing outside grid
		GL.glEnable(GL.GL_SCISSOR_TEST)
		GL.glScissor(
			self.padding_left,
			self.padding_bottom+1,
			self.width-self.padding_left-self.padding_right-1,
			self.height-self.padding_top-self.padding_bottom-1,
		)
		#load the points
		points = list()
		width = self._bar_width/2
		for i, bar in enumerate(bars):
			points.extend([
					(i-width, 0),
					(i+width, 0),
					(i+width, bar),
					(i-width, bar),
				]
			)
		GL.glColor3f(*self._color_spec)
		#matrix transforms
		GL.glPushMatrix()
		GL.glTranslatef(self.padding_left, self.padding_top, 0)
		GL.glScalef(
			(self.width-self.padding_left-self.padding_right),
			(self.height-self.padding_top-self.padding_bottom),
			1,
		)
		GL.glTranslatef(0, 1, 0)
		GL.glScalef(1.0/(num_bars-1), -1.0/(self.y_max-self.y_min), 1)
		GL.glTranslatef(0, -self.y_min, 0)
		#draw the bars
		GL.glVertexPointerf(points)
		GL.glDrawArrays(GL.GL_QUADS, 0, len(points))
		GL.glPopMatrix()
		GL.glDisable(GL.GL_SCISSOR_TEST)

	def _populate_point_label(self, x_val, y_val):
		"""
		Get the text the will populate the point label.
		Give X and Y values for the current point.
		Give values for the channel at the X coordinate.
		@param x_val the current x value
		@param y_val the current y value
		@return a string with newlines
		"""
		if len(self._bars) == 0: return ''
		scalar = float(len(self._bars)-1)/(self.x_max - self.x_min)
		#convert x val to bar #
		bar_index = scalar*(x_val - self.x_min)
		#if abs(bar_index - round(bar_index)) > self._bar_width/2: return ''
		bar_index = int(round(bar_index))
		bar_start = (bar_index - self._bar_width/2)/scalar + self.x_min
		bar_end = (bar_index + self._bar_width/2)/scalar + self.x_min
		bar_value = self._bars[bar_index]
		return '%s to %s\n%s: %s'%(
			common.eng_format(bar_start, self.x_units),
			common.eng_format(bar_end, self.x_units),
			self.y_label, common.eng_format(bar_value, self.y_units),
		)

	def set_bars(self, bars, bar_width, color_spec):
		"""
		Set the bars.
		@param bars a list of bars
		@param bar_width the fractional width of the bar, between 0 and 1
		@param color_spec the color tuple
		"""
		self.lock()
		self._bars = bars
		self._bar_width = float(bar_width)
		self._color_spec = color_spec
		self._bar_cache.changed(True)
		self.unlock()


