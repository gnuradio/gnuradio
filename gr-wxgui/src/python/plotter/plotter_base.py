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
import wx.glcanvas
from OpenGL.GL import *
from gnuradio.wxgui import common
import threading
import gltext
import math
import time

BACKGROUND_COLOR_SPEC = (1, 0.976, 1, 1) #creamy white
GRID_LINE_COLOR_SPEC = (0, 0, 0) #black
TICK_TEXT_FONT_SIZE = 9
TITLE_TEXT_FONT_SIZE = 13
UNITS_TEXT_FONT_SIZE = 9
TICK_LABEL_PADDING = 5
POINT_LABEL_FONT_SIZE = 8
POINT_LABEL_COLOR_SPEC = (1, 1, .5)
POINT_LABEL_PADDING = 3

##################################################
# OpenGL WX Plotter Canvas
##################################################
class _plotter_base(wx.glcanvas.GLCanvas):
	"""!
	Plotter base class for all plot types.
	"""

	def __init__(self, parent):
		"""!
		Create a new plotter base.
		Initialize GL and register events.
		@param parent the parent widgit
		"""
		self._semaphore = threading.Semaphore(1)
		wx.glcanvas.GLCanvas.__init__(self, parent, -1)
		self.changed(False)
		self._gl_init_flag = False
		self._resized_flag = True
		self._update_ts = 0
		self.Bind(wx.EVT_PAINT, self._on_paint)
		self.Bind(wx.EVT_SIZE, self._on_size)

	def lock(self): self._semaphore.acquire(True)
	def unlock(self): self._semaphore.release()

	def _on_size(self, event):
		"""!
		Flag the resize event.
		The paint event will handle the actual resizing.
		"""
		self._resized_flag = True

	def _on_paint(self, event):
		"""!
		Respond to paint events, call update.
		Initialize GL if this is the first paint event.
		"""
		self.SetCurrent()
		#check if gl was initialized
		if not self._gl_init_flag:
			glClearColor(*BACKGROUND_COLOR_SPEC)
			self._gl_init()
			self._gl_init_flag = True
		#check for a change in window size
		if self._resized_flag:
			self.lock()
			self.width, self.height = self.GetSize()
			glViewport(0, 0, self.width, self.height)
			glMatrixMode(GL_PROJECTION)
			glLoadIdentity()
			glOrtho(0, self.width, self.height, 0, 1, 0)
			glMatrixMode(GL_MODELVIEW)
			glLoadIdentity()
			glViewport(0, 0, self.width, self.height)
			self._resized_flag = False
			self.changed(True)
			self.unlock()
		self.draw()

	def update(self):
		"""!
		Force a paint event.
		Record the timestamp.
		"""
		wx.PostEvent(self, wx.PaintEvent())
		self._update_ts = time.time()

	def clear(self): glClear(GL_COLOR_BUFFER_BIT)

	def changed(self, state=None):
		"""!
		Set the changed flag if state is not None.
		Otherwise return the changed flag.
		"""
		if state is not None: self._changed = state
		else: return self._changed

##################################################
# Grid Plotter Base Class
##################################################
class grid_plotter_base(_plotter_base):

	def __init__(self, parent, padding):
		_plotter_base.__init__(self, parent)
		self.padding_top, self.padding_right, self.padding_bottom, self.padding_left = padding
		#store title and unit strings
		self.set_title('Title')
		self.set_x_label('X Label')
		self.set_y_label('Y Label')
		#init the grid to some value
		self.set_x_grid(-1, 1, 1)
		self.set_y_grid(-1, 1, 1)
		#setup point label
		self.enable_point_label(False)
		self._mouse_coordinate = None
		self.Bind(wx.EVT_MOTION, self._on_motion)
		self.Bind(wx.EVT_LEAVE_WINDOW, self._on_leave_window)

	def _on_motion(self, event):
		"""!
		Mouse motion, record the position X, Y.
		"""
		self.lock()
		self._mouse_coordinate = event.GetPosition()
		#update based on last known update time
		if time.time() - self._update_ts > 0.03: self.update()
		self.unlock()

	def _on_leave_window(self, event):
		"""!
		Mouse leave window, set the position to None.
		"""
		self.lock()
		self._mouse_coordinate = None
		self.update()
		self.unlock()

	def enable_point_label(self, enable=None):
		"""!
		Enable/disable the point label.
		@param enable true to enable
		@return the enable state when None
		"""
		if enable is None: return self._enable_point_label
		self.lock()
		self._enable_point_label = enable
		self.changed(True)
		self.unlock()

	def set_title(self, title):
		"""!
		Set the title.
		@param title the title string
		"""
		self.lock()
		self.title = title
		self.changed(True)
		self.unlock()

	def set_x_label(self, x_label, x_units=''):
		"""!
		Set the x label and units.
		@param x_label the x label string
		@param x_units the x units string
		"""
		self.lock()
		self.x_label = x_label
		self.x_units = x_units
		self.changed(True)
		self.unlock()

	def set_y_label(self, y_label, y_units=''):
		"""!
		Set the y label and units.
		@param y_label the y label string
		@param y_units the y units string
		"""
		self.lock()
		self.y_label = y_label
		self.y_units = y_units
		self.changed(True)
		self.unlock()

	def set_x_grid(self, x_min, x_max, x_step, x_scalar=1.0):
		"""!
		Set the x grid parameters.
		@param x_min the left-most value
		@param x_max the right-most value
		@param x_step the grid spacing
		@param x_scalar the scalar factor
		"""
		self.lock()
		self.x_min = float(x_min)
		self.x_max = float(x_max)
		self.x_step = float(x_step)
		self.x_scalar = float(x_scalar)
		self.changed(True)
		self.unlock()

	def set_y_grid(self, y_min, y_max, y_step, y_scalar=1.0):
		"""!
		Set the y grid parameters.
		@param y_min the bottom-most value
		@param y_max the top-most value
		@param y_step the grid spacing
		@param y_scalar the scalar factor
		"""
		self.lock()
		self.y_min = float(y_min)
		self.y_max = float(y_max)
		self.y_step = float(y_step)
		self.y_scalar = float(y_scalar)
		self.changed(True)
		self.unlock()

	def _draw_grid(self):
		"""!
		Draw the border, grid, title, and units.
		"""
		##################################################
		# Draw Border
		##################################################
		glColor3f(*GRID_LINE_COLOR_SPEC)
		glBegin(GL_LINE_LOOP)
		glVertex3f(self.padding_left, self.padding_top, 0)
		glVertex3f(self.width - self.padding_right, self.padding_top, 0)
		glVertex3f(self.width - self.padding_right, self.height - self.padding_bottom, 0)
		glVertex3f(self.padding_left, self.height - self.padding_bottom, 0)
		glEnd()
		##################################################
		# Draw Grid X
		##################################################
		for tick in self._get_ticks(self.x_min, self.x_max, self.x_step, self.x_scalar):
			scaled_tick = (self.width-self.padding_left-self.padding_right)*\
				(tick/self.x_scalar-self.x_min)/(self.x_max-self.x_min) + self.padding_left
			glColor3f(*GRID_LINE_COLOR_SPEC)
			self._draw_line(
				(scaled_tick, self.padding_top, 0),
				(scaled_tick, self.height-self.padding_bottom, 0),
			)
			txt = self._get_tick_label(tick)
			w, h = txt.get_size()
			txt.draw_text(wx.Point(scaled_tick-w/2, self.height-self.padding_bottom+TICK_LABEL_PADDING))
		##################################################
		# Draw Grid Y
		##################################################
		for tick in self._get_ticks(self.y_min, self.y_max, self.y_step, self.y_scalar):
			scaled_tick = (self.height-self.padding_top-self.padding_bottom)*\
				(1 - (tick/self.y_scalar-self.y_min)/(self.y_max-self.y_min)) + self.padding_top
			glColor3f(*GRID_LINE_COLOR_SPEC)
			self._draw_line(
				(self.padding_left, scaled_tick, 0),
				(self.width-self.padding_right, scaled_tick, 0),
			)
			txt = self._get_tick_label(tick)
			w, h = txt.get_size()
			txt.draw_text(wx.Point(self.padding_left-w-TICK_LABEL_PADDING, scaled_tick-h/2))
		##################################################
		# Draw Title
		##################################################
		#draw x units
		txt = gltext.Text(self.title, bold=True, font_size=TITLE_TEXT_FONT_SIZE, centered=True)
		txt.draw_text(wx.Point(self.width/2.0, .5*self.padding_top))
		##################################################
		# Draw Labels
		##################################################
		#draw x labels
		x_label_str = self.x_units and "%s (%s)"%(self.x_label, self.x_units) or self.x_label
		txt = gltext.Text(x_label_str, bold=True, font_size=UNITS_TEXT_FONT_SIZE, centered=True)
		txt.draw_text(wx.Point(
				(self.width-self.padding_left-self.padding_right)/2.0 + self.padding_left,
				self.height-.25*self.padding_bottom,
				)
		)
		#draw y labels
		y_label_str = self.y_units and "%s (%s)"%(self.y_label, self.y_units) or self.y_label
		txt = gltext.Text(y_label_str, bold=True, font_size=UNITS_TEXT_FONT_SIZE, centered=True)
		txt.draw_text(wx.Point(
				.25*self.padding_left,
				(self.height-self.padding_top-self.padding_bottom)/2.0 + self.padding_top,
			), rotation=90,
		)

	def _get_tick_label(self, tick):
		"""!
		Format the tick value and create a gl text.
		@param tick the floating point tick value
		@return the tick label text
		"""
		tick_str = common.label_format(tick)
		return gltext.Text(tick_str, font_size=TICK_TEXT_FONT_SIZE)

	def _get_ticks(self, min, max, step, scalar):
		"""!
		Determine the positions for the ticks.
		@param min the lower bound
		@param max the upper bound
		@param step the grid spacing
		@param scalar the grid scaling
		@return a list of tick positions between min and max
		"""
		#cast to float
		min = float(min)
		max = float(max)
		step = float(step)
		#check for valid numbers
		assert step > 0
		assert max > min
		assert max - min > step
		#determine the start and stop value
		start = int(math.ceil(min/step))
		stop = int(math.floor(max/step))
		return [i*step*scalar for i in range(start, stop+1)]

	def _draw_line(self, coor1, coor2):
		"""!
		Draw a line from coor1 to coor2.
		@param corr1 a tuple of x, y, z
		@param corr2 a tuple of x, y, z
		"""
		glBegin(GL_LINES)
		glVertex3f(*coor1)
		glVertex3f(*coor2)
		glEnd()

	def _draw_rect(self, x, y, width, height, fill=True):
		"""!
		Draw a rectangle on the x, y plane.
		X and Y are the top-left corner.
		@param x the left position of the rectangle
		@param y the top position of the rectangle
		@param width the width of the rectangle
		@param height the height of the rectangle
		@param fill true to color inside of rectangle
		"""
		glBegin(fill and GL_QUADS or GL_LINE_LOOP)
		glVertex2f(x, y)
		glVertex2f(x+width, y)
		glVertex2f(x+width, y+height)
		glVertex2f(x, y+height)
		glEnd()

	def _draw_point_label(self):
		"""!
		Draw the point label for the last mouse motion coordinate.
		The mouse coordinate must be an X, Y tuple.
		The label will be drawn at the X, Y coordinate.
		The values of the X, Y coordinate will be scaled to the current X, Y bounds.
		"""
		if not self.enable_point_label(): return
		if not self._mouse_coordinate: return
		x, y = self._mouse_coordinate
		if x < self.padding_left or x > self.width-self.padding_right: return
		if y < self.padding_top or y > self.height-self.padding_bottom: return
		#scale to window bounds
		x_win_scalar = float(x - self.padding_left)/(self.width-self.padding_left-self.padding_right)
		y_win_scalar = float((self.height - y) - self.padding_bottom)/(self.height-self.padding_top-self.padding_bottom)
		#scale to grid bounds
		x_val = self.x_scalar*(x_win_scalar*(self.x_max-self.x_min) + self.x_min)
		y_val = self.y_scalar*(y_win_scalar*(self.y_max-self.y_min) + self.y_min)
		#create text
		label_str = self._populate_point_label(x_val, y_val)
		txt = gltext.Text(label_str, font_size=POINT_LABEL_FONT_SIZE)
		w, h = txt.get_size()
		#draw rect + text
		glColor3f(*POINT_LABEL_COLOR_SPEC)
		if x > self.width/2: x -= w+2*POINT_LABEL_PADDING
		self._draw_rect(x, y-h-2*POINT_LABEL_PADDING, w+2*POINT_LABEL_PADDING, h+2*POINT_LABEL_PADDING)
		txt.draw_text(wx.Point(x+POINT_LABEL_PADDING, y-h-POINT_LABEL_PADDING))
