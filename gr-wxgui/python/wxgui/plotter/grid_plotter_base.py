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
import wx.glcanvas
from OpenGL import GL
import common
from plotter_base import plotter_base
import gltext
import math

GRID_LINE_COLOR_SPEC = (.7, .7, .7) #gray
GRID_BORDER_COLOR_SPEC = (0, 0, 0) #black
TICK_TEXT_FONT_SIZE = 9
TITLE_TEXT_FONT_SIZE = 13
UNITS_TEXT_FONT_SIZE = 9
AXIS_LABEL_PADDING = 5
TICK_LABEL_PADDING = 5
TITLE_LABEL_PADDING = 7
POINT_LABEL_FONT_SIZE = 8
POINT_LABEL_COLOR_SPEC = (1, 1, 0.5, 0.75)
POINT_LABEL_PADDING = 3
POINT_LABEL_OFFSET = 10
GRID_LINE_DASH_LEN = 4

##################################################
# Grid Plotter Base Class
##################################################
class grid_plotter_base(plotter_base):

	def __init__(self, parent, min_padding=(0, 0, 0, 0)):
		plotter_base.__init__(self, parent)
		#setup grid cache
		self._grid_cache = self.new_gl_cache(self._draw_grid, 25)
		self.enable_grid_lines(True)
		#setup padding
		self.padding_top_min, self.padding_right_min, self.padding_bottom_min, self.padding_left_min = min_padding
		#store title and unit strings
		self.set_title('Title')
		self.set_x_label('X Label')
		self.set_y_label('Y Label')
		#init the grid to some value
		self.set_x_grid(-1, 1, 1)
		self.set_y_grid(-1, 1, 1)
		#setup point label cache
		self._point_label_cache = self.new_gl_cache(self._draw_point_label, 75)
		self.enable_point_label(False)
		self.enable_grid_aspect_ratio(False)
		self.set_point_label_coordinate(None)
		common.point_label_thread(self)
		#init grid plotter
		self.register_init(self._init_grid_plotter)

	def _init_grid_plotter(self):
		"""
		Run gl initialization tasks.
		"""
		GL.glEnableClientState(GL.GL_VERTEX_ARRAY)

	def set_point_label_coordinate(self, coor):
		"""
		Set the point label coordinate.

                Args:
		    coor: the coordinate x, y tuple or None
		"""
		self.lock()
		self._point_label_coordinate = coor
		self._point_label_cache.changed(True)
		self.update()
		self.unlock()

	def call_freq_callback(self, coor):
		try:
			x, y = self._point_label_coordinate
		except:
			return
		if x < self.padding_left or x > self.width-self.padding_right: return
		if y < self.padding_top or y > self.height-self.padding_bottom: return
		#scale to window bounds
		x_win_scalar = float(x - self.padding_left)/(self.width-self.padding_left-self.padding_right)
		y_win_scalar = float((self.height - y) - self.padding_bottom)/(self.height-self.padding_top-self.padding_bottom)
		#scale to grid bounds
		x_val = x_win_scalar*(self.x_max-self.x_min) + self.x_min
		y_val = y_win_scalar*(self.y_max-self.y_min) + self.y_min
		self._call_callback(x_val, y_val)

	def enable_grid_aspect_ratio(self, enable=None):
		"""
		Enable/disable the grid aspect ratio.
		If enabled, enforce the aspect ratio on the padding:
			horizontal_padding:vertical_padding == width:height

                Args:
		    enable: true to enable

                Returns:
		    the enable state when None
		"""
		if enable is None: return self._enable_grid_aspect_ratio
		self.lock()
		self._enable_grid_aspect_ratio = enable
		for cache in self._gl_caches: cache.changed(True)
		self.unlock()

	def enable_point_label(self, enable=None):
		"""
		Enable/disable the point label.

                Args:
		    enable: true to enable

                Returns:
		    the enable state when None
		"""
		if enable is None: return self._enable_point_label
		self.lock()
		self._enable_point_label = enable
		self._point_label_cache.changed(True)
		self.unlock()

	def set_title(self, title):
		"""
		Set the title.

                Args:
		    title the title string
		"""
		self.lock()
		self.title = title
		self._grid_cache.changed(True)
		self.unlock()

	def set_x_label(self, x_label, x_units=''):
		"""
		Set the x label and units.

                Args:
		    x_label: the x label string
		    x_units: the x units string
		"""
		self.lock()
		self.x_label = x_label
		self.x_units = x_units
		self._grid_cache.changed(True)
		self.unlock()

	def set_y_label(self, y_label, y_units=''):
		"""
		Set the y label and units.

                Args:
		    y_label: the y label string
		    y_units: the y units string
		"""
		self.lock()
		self.y_label = y_label
		self.y_units = y_units
		self._grid_cache.changed(True)
		self.unlock()

	def set_x_grid(self, minimum, maximum, step, scale=False):
		"""
		Set the x grid parameters.

                Args:
		    minimum: the left-most value
		    maximum: the right-most value
		    step: the grid spacing
		    scale: true to scale the x grid
		"""
		self.lock()
		self.x_min = float(minimum)
		self.x_max = float(maximum)
		self.x_step = float(step)
		if scale:
			coeff, exp, prefix = common.get_si_components(max(abs(self.x_min), abs(self.x_max)))
			self.x_scalar = 10**(-exp)
			self.x_prefix = prefix
		else:
			self.x_scalar = 1.0
			self.x_prefix = ''
		for cache in self._gl_caches: cache.changed(True)
		self.unlock()

	def set_y_grid(self, minimum, maximum, step, scale=False):
		"""
		Set the y grid parameters.

                Args:
		    minimum: the bottom-most value
		    maximum: the top-most value
		    step: the grid spacing
		    scale: true to scale the y grid
		"""
		self.lock()
		self.y_min = float(minimum)
		self.y_max = float(maximum)
		self.y_step = float(step)
		if scale:
			coeff, exp, prefix = common.get_si_components(max(abs(self.y_min), abs(self.y_max)))
			self.y_scalar = 10**(-exp)
			self.y_prefix = prefix
		else:
			self.y_scalar = 1.0
			self.y_prefix = ''
		for cache in self._gl_caches: cache.changed(True)
		self.unlock()

	def _draw_grid(self):
		"""
		Create the x, y, tick, and title labels.
		Resize the padding for the labels.
		Draw the border, grid, title, and labels.
		"""
		##################################################
		# Create GL text labels
		##################################################
		#create x tick labels
		x_tick_labels = [(tick, self._get_tick_label(tick, self.x_units))
			for tick in self._get_ticks(self.x_min, self.x_max, self.x_step, self.x_scalar)]
		#create x tick labels
		y_tick_labels = [(tick, self._get_tick_label(tick, self.y_units))
			for tick in self._get_ticks(self.y_min, self.y_max, self.y_step, self.y_scalar)]
		#create x label
		x_label_str = self.x_units and "%s (%s%s)"%(self.x_label, self.x_prefix, self.x_units) or self.x_label
		x_label = gltext.Text(x_label_str, bold=True, font_size=UNITS_TEXT_FONT_SIZE, centered=True)
		#create y label
		y_label_str = self.y_units and "%s (%s%s)"%(self.y_label, self.y_prefix, self.y_units) or self.y_label
		y_label = gltext.Text(y_label_str, bold=True, font_size=UNITS_TEXT_FONT_SIZE, centered=True)
		#create title
		title_label = gltext.Text(self.title, bold=True, font_size=TITLE_TEXT_FONT_SIZE, centered=True)
		##################################################
		# Resize the padding
		##################################################
		self.padding_top = max(2*TITLE_LABEL_PADDING + title_label.get_size()[1], self.padding_top_min)
		self.padding_right = max(2*TICK_LABEL_PADDING, self.padding_right_min)
		self.padding_bottom = max(2*AXIS_LABEL_PADDING + TICK_LABEL_PADDING + x_label.get_size()[1] + max([label.get_size()[1] for tick, label in x_tick_labels]), self.padding_bottom_min)
		self.padding_left = max(2*AXIS_LABEL_PADDING + TICK_LABEL_PADDING + y_label.get_size()[1] + max([label.get_size()[0] for tick, label in y_tick_labels]), self.padding_left_min)
		#enforce padding aspect ratio if enabled
		if self.enable_grid_aspect_ratio():
			w_over_h_ratio = float(self.width)/float(self.height)
			horizontal_padding = float(self.padding_right + self.padding_left)
			veritical_padding = float(self.padding_top + self.padding_bottom)
			if w_over_h_ratio > horizontal_padding/veritical_padding:
				#increase the horizontal padding
				new_padding = veritical_padding*w_over_h_ratio - horizontal_padding
				#distribute the padding to left and right
				self.padding_left += int(round(new_padding/2))
				self.padding_right += int(round(new_padding/2))
			else:
				#increase the vertical padding
				new_padding = horizontal_padding/w_over_h_ratio - veritical_padding
				#distribute the padding to top and bottom
				self.padding_top += int(round(new_padding/2))
				self.padding_bottom += int(round(new_padding/2))
		##################################################
		# Draw Grid X
		##################################################
		for tick, label in x_tick_labels:
			scaled_tick = (self.width-self.padding_left-self.padding_right)*\
				(tick/self.x_scalar-self.x_min)/(self.x_max-self.x_min) + self.padding_left
			self._draw_grid_line(
				(scaled_tick, self.padding_top),
				(scaled_tick, self.height-self.padding_bottom),
			)
			w, h = label.get_size()
			label.draw_text(wx.Point(scaled_tick-w/2, self.height-self.padding_bottom+TICK_LABEL_PADDING))
		##################################################
		# Draw Grid Y
		##################################################
		for tick, label in y_tick_labels:
			scaled_tick = (self.height-self.padding_top-self.padding_bottom)*\
				(1 - (tick/self.y_scalar-self.y_min)/(self.y_max-self.y_min)) + self.padding_top
			self._draw_grid_line(
				(self.padding_left, scaled_tick),
				(self.width-self.padding_right, scaled_tick),
			)
			w, h = label.get_size()
			label.draw_text(wx.Point(self.padding_left-w-TICK_LABEL_PADDING, scaled_tick-h/2))
		##################################################
		# Draw Border
		##################################################
		GL.glColor3f(*GRID_BORDER_COLOR_SPEC)
		self._draw_rect(
			self.padding_left,
			self.padding_top,
			self.width - self.padding_right - self.padding_left,
			self.height - self.padding_top - self.padding_bottom,
			fill=False,
		)
		##################################################
		# Draw Labels
		##################################################
		#draw title label
		title_label.draw_text(wx.Point(self.width/2.0, TITLE_LABEL_PADDING + title_label.get_size()[1]/2))
		#draw x labels
		x_label.draw_text(wx.Point(
				(self.width-self.padding_left-self.padding_right)/2.0 + self.padding_left,
				self.height-(AXIS_LABEL_PADDING + x_label.get_size()[1]/2),
				)
		)
		#draw y labels
		y_label.draw_text(wx.Point(
				AXIS_LABEL_PADDING + y_label.get_size()[1]/2,
				(self.height-self.padding_top-self.padding_bottom)/2.0 + self.padding_top,
			), rotation=90,
		)

	def _get_tick_label(self, tick, unit):
		"""
		Format the tick value and create a gl text.

                Args:
		    tick: the floating point tick value
		    unit: the axis unit

                Returns:
		    the tick label text
		"""
		if unit: tick_str = common.sci_format(tick)
		else: tick_str = common.eng_format(tick)
		return gltext.Text(tick_str, font_size=TICK_TEXT_FONT_SIZE)

	def _get_ticks(self, min, max, step, scalar):
		"""
		Determine the positions for the ticks.

                Args:
		    min: the lower bound
		    max: the upper bound
		    step: the grid spacing
		    scalar: the grid scaling

                Returns:
		    a list of tick positions between min and max
		"""
		#cast to float
		min = float(min)
		max = float(max)
		step = float(step)
		#check for valid numbers
		try:
			assert step > 0
			assert max > min
			assert max - min > step
		except AssertionError: return [-1, 1]
		#determine the start and stop value
		start = int(math.ceil(min/step))
		stop = int(math.floor(max/step))
		return [i*step*scalar for i in range(start, stop+1)]

	def enable_grid_lines(self, enable=None):
		"""
		Enable/disable the grid lines.

                Args:
		    enable: true to enable

                Returns:
		    the enable state when None
		"""
		if enable is None: return self._enable_grid_lines
		self.lock()
		self._enable_grid_lines = enable
		self._grid_cache.changed(True)
		self.unlock()

	def _draw_grid_line(self, coor1, coor2):
		"""
		Draw a dashed line from coor1 to coor2.

                Args:
		    corr1: a tuple of x, y
		    corr2: a tuple of x, y
		"""
		if not self.enable_grid_lines(): return
		length = math.sqrt((coor1[0] - coor2[0])**2 + (coor1[1] - coor2[1])**2)
		num_points = int(length/GRID_LINE_DASH_LEN)
		#calculate points array
		points = [(
			coor1[0] + i*(coor2[0]-coor1[0])/(num_points - 1),
			coor1[1] + i*(coor2[1]-coor1[1])/(num_points - 1)
		) for i in range(num_points)]
		#set color and draw
		GL.glColor3f(*GRID_LINE_COLOR_SPEC)
		GL.glVertexPointerf(points)
		GL.glDrawArrays(GL.GL_LINES, 0, len(points))

	def _draw_rect(self, x, y, width, height, fill=True):
		"""
		Draw a rectangle on the x, y plane.
		X and Y are the top-left corner.

                Args:
		    x: the left position of the rectangle
		    y: the top position of the rectangle
		    width: the width of the rectangle
		    height: the height of the rectangle
		    fill: true to color inside of rectangle
		"""
		GL.glBegin(fill and GL.GL_QUADS or GL.GL_LINE_LOOP)
		GL.glVertex2f(x, y)
		GL.glVertex2f(x+width, y)
		GL.glVertex2f(x+width, y+height)
		GL.glVertex2f(x, y+height)
		GL.glEnd()

	def _draw_point_label(self):
		"""
		Draw the point label for the last mouse motion coordinate.
		The mouse coordinate must be an X, Y tuple.
		The label will be drawn at the X, Y coordinate.
		The values of the X, Y coordinate will be scaled to the current X, Y bounds.
		"""
		if not self.enable_point_label(): return
		if not self._point_label_coordinate: return
		x, y = self._point_label_coordinate
		if x < self.padding_left or x > self.width-self.padding_right: return
		if y < self.padding_top or y > self.height-self.padding_bottom: return
		#scale to window bounds
		x_win_scalar = float(x - self.padding_left)/(self.width-self.padding_left-self.padding_right)
		y_win_scalar = float((self.height - y) - self.padding_bottom)/(self.height-self.padding_top-self.padding_bottom)
		#scale to grid bounds
		x_val = x_win_scalar*(self.x_max-self.x_min) + self.x_min
		y_val = y_win_scalar*(self.y_max-self.y_min) + self.y_min
		#create text
		label_str = self._populate_point_label(x_val, y_val)
		if not label_str: return
		txt = gltext.Text(label_str, font_size=POINT_LABEL_FONT_SIZE)
		w, h = txt.get_size()
		#enable transparency
		GL.glEnable(GL.GL_BLEND)
		GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)
		#draw rect + text
		GL.glColor4f(*POINT_LABEL_COLOR_SPEC)
		if x > self.width/2: x -= w+2*POINT_LABEL_PADDING + POINT_LABEL_OFFSET
		else: x += POINT_LABEL_OFFSET
		self._draw_rect(x, y-h-2*POINT_LABEL_PADDING, w+2*POINT_LABEL_PADDING, h+2*POINT_LABEL_PADDING)
		txt.draw_text(wx.Point(x+POINT_LABEL_PADDING, y-h-POINT_LABEL_PADDING))
