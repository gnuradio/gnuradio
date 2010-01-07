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
import struct

LEGEND_LEFT_PAD = 7
LEGEND_NUM_BLOCKS = 256
LEGEND_NUM_LABELS = 9
LEGEND_WIDTH = 8
LEGEND_FONT_SIZE = 8
LEGEND_BORDER_COLOR_SPEC = (0, 0, 0) #black
MIN_PADDING = 0, 60, 0, 0 #top, right, bottom, left

ceil_log2 = lambda x: 2**int(math.ceil(math.log(x)/math.log(2)))

pack_color   = lambda x: struct.unpack('I', struct.pack('BBBB', *x))[0]
unpack_color = lambda x: struct.unpack('BBBB', struct.pack('I', int(x)))

def _get_rbga(red_pts, green_pts, blue_pts, alpha_pts=[(0, 0), (1, 0)]):
	"""
	Get an array of 256 rgba values where each index maps to a color.
	The scaling for red, green, blue, alpha are specified in piece-wise functions.
	The piece-wise functions consist of a set of x, y coordinates.
	The x and y values of the coordinates range from 0 to 1.
	The coordinates must be specified so that x increases with the index value.
	Resulting values are calculated along the line formed between 2 coordinates.
	@param *_pts an array of x,y coordinates for each color element
	@return array of rbga values (4 bytes) each
	"""
	def _fcn(x, pw):
		for (x1, y1), (x2, y2) in zip(pw, pw[1:]):
			#linear interpolation
			if x <= x2: return float(y1 - y2)/(x1 - x2)*(x - x1) + y1
		raise Exception
	return numpy.array([pack_color(map(
		lambda pw: int(255*_fcn(i/255.0, pw)),
		(red_pts, green_pts, blue_pts, alpha_pts),
	)) for i in range(0, 256)], numpy.uint32)

COLORS = {
	'rgb1': _get_rbga( #http://www.ks.uiuc.edu/Research/vmd/vmd-1.7.1/ug/img47.gif
		red_pts = [(0, 0), (.5, 0), (1, 1)],
		green_pts = [(0, 0), (.5, 1), (1, 0)],
		blue_pts = [(0, 1), (.5, 0), (1, 0)],
	),
	'rgb2': _get_rbga( #http://xtide.ldeo.columbia.edu/~krahmann/coledit/screen.jpg
		red_pts = [(0, 0), (3.0/8, 0), (5.0/8, 1), (7.0/8, 1), (1, .5)],
		green_pts = [(0, 0), (1.0/8, 0), (3.0/8, 1), (5.0/8, 1), (7.0/8, 0), (1, 0)],
		blue_pts = [(0, .5), (1.0/8, 1), (3.0/8, 1), (5.0/8, 0), (1, 0)],
	),
	'rgb3': _get_rbga(
		red_pts = [(0, 0), (1.0/3.0, 0), (2.0/3.0, 0), (1, 1)],
		green_pts = [(0, 0), (1.0/3.0, 0), (2.0/3.0, 1), (1, 0)],
		blue_pts = [(0, 0), (1.0/3.0, 1), (2.0/3.0, 0), (1, 0)],
	),
	'gray': _get_rbga(
		red_pts = [(0, 0), (1, 1)],
		green_pts = [(0, 0), (1, 1)],
		blue_pts = [(0, 0), (1, 1)],
	),
}

##################################################
# Waterfall Plotter
##################################################
class waterfall_plotter(grid_plotter_base):
	def __init__(self, parent):
		"""
		Create a new channel plotter.
		"""
		#init
		grid_plotter_base.__init__(self, parent, MIN_PADDING)
		#setup legend cache
		self._legend_cache = self.new_gl_cache(self._draw_legend)
		#setup waterfall cache
		self._waterfall_cache = self.new_gl_cache(self._draw_waterfall, 50)
		#setup waterfall plotter
		self.register_init(self._init_waterfall)
		self._resize_texture(False)
		self._minimum = 0
		self._maximum = 0
		self._fft_size = 1
		self._buffer = list()
		self._pointer = 0
		self._counter = 0
		self.set_num_lines(0)
		self.set_color_mode(COLORS.keys()[0])

	def _init_waterfall(self):
		"""
		Run gl initialization tasks.
		"""
		self._waterfall_texture = GL.glGenTextures(1)

	def _draw_waterfall(self):
		"""
		Draw the waterfall from the texture.
		The texture is circularly filled and will wrap around.
		Use matrix modeling to shift and scale the texture onto the coordinate plane.
		"""
		#resize texture
		self._resize_texture()
		#setup texture
		GL.glBindTexture(GL.GL_TEXTURE_2D, self._waterfall_texture)
		GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR)
		GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR)
		GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T, GL.GL_REPEAT)
		GL.glTexEnvi(GL.GL_TEXTURE_ENV, GL.GL_TEXTURE_ENV_MODE, GL.GL_REPLACE)
		#write the buffer to the texture
		while self._buffer:
			GL.glTexSubImage2D(GL.GL_TEXTURE_2D, 0, 0, self._pointer, self._fft_size, 1, GL.GL_RGBA, GL.GL_UNSIGNED_BYTE, self._buffer.pop(0))
			self._pointer = (self._pointer + 1)%self._num_lines
		#begin drawing
		GL.glEnable(GL.GL_TEXTURE_2D)
		GL.glPushMatrix()
		#matrix scaling
		GL.glTranslatef(self.padding_left, self.padding_top, 0)
		GL.glScalef(
			float(self.width-self.padding_left-self.padding_right),
			float(self.height-self.padding_top-self.padding_bottom),
			1.0,
		)
		#draw texture with wrapping
		GL.glBegin(GL.GL_QUADS)
		prop_y = float(self._pointer)/(self._num_lines-1)
		prop_x = float(self._fft_size)/ceil_log2(self._fft_size)
		off = 1.0/(self._num_lines-1)
		GL.glTexCoord2f(0, prop_y+1-off)
		GL.glVertex2f(0, 1)
		GL.glTexCoord2f(prop_x, prop_y+1-off)
		GL.glVertex2f(1, 1)
		GL.glTexCoord2f(prop_x, prop_y)
		GL.glVertex2f(1, 0)
		GL.glTexCoord2f(0, prop_y)
		GL.glVertex2f(0, 0)
		GL.glEnd()
		GL.glPopMatrix()
		GL.glDisable(GL.GL_TEXTURE_2D)

	def _populate_point_label(self, x_val, y_val):
		"""
		Get the text the will populate the point label.
		Give the X value for the current point.
		@param x_val the current x value
		@param y_val the current y value
		@return a value string with units
		"""
		return '%s: %s'%(self.x_label, common.eng_format(x_val, self.x_units))

	def _draw_legend(self):
		"""
		Draw the color scale legend.
		"""
		if not self._color_mode: return
		legend_height = self.height-self.padding_top-self.padding_bottom
		#draw each legend block
		block_height = float(legend_height)/LEGEND_NUM_BLOCKS
		x = self.width - self.padding_right + LEGEND_LEFT_PAD
		for i in range(LEGEND_NUM_BLOCKS):
			color = unpack_color(COLORS[self._color_mode][int(255*i/float(LEGEND_NUM_BLOCKS-1))])
			GL.glColor4f(*numpy.array(color)/255.0)
			y = self.height - (i+1)*block_height - self.padding_bottom
			self._draw_rect(x, y, LEGEND_WIDTH, block_height)
		#draw rectangle around color scale border
		GL.glColor3f(*LEGEND_BORDER_COLOR_SPEC)
		self._draw_rect(x, self.padding_top, LEGEND_WIDTH, legend_height, fill=False)
		#draw each legend label
		label_spacing = float(legend_height)/(LEGEND_NUM_LABELS-1)
		x = self.width - (self.padding_right - LEGEND_LEFT_PAD - LEGEND_WIDTH)/2
		for i in range(LEGEND_NUM_LABELS):
			proportion = i/float(LEGEND_NUM_LABELS-1)
			dB = proportion*(self._maximum - self._minimum) + self._minimum
			y = self.height - i*label_spacing - self.padding_bottom
			txt = gltext.Text('%ddB'%int(dB), font_size=LEGEND_FONT_SIZE, centered=True)
			txt.draw_text(wx.Point(x, y))

	def _resize_texture(self, flag=None):
		"""
		Create the texture to fit the fft_size X num_lines.
		@param flag the set/unset or update flag
		"""
		if flag is not None: 
			self._resize_texture_flag = flag
			return
		if not self._resize_texture_flag: return
		self._buffer = list()
		self._pointer = 0
		if self._num_lines and self._fft_size:
			GL.glBindTexture(GL.GL_TEXTURE_2D, self._waterfall_texture)
			data = numpy.zeros(self._num_lines*ceil_log2(self._fft_size)*4, numpy.uint8).tostring()
			GL.glTexImage2D(GL.GL_TEXTURE_2D, 0, GL.GL_RGBA, ceil_log2(self._fft_size), self._num_lines, 0, GL.GL_RGBA, GL.GL_UNSIGNED_BYTE, data)
		self._resize_texture_flag = False

	def set_color_mode(self, color_mode):
		"""
		Set the color mode.
		New samples will be converted to the new color mode.
		Old samples will not be recolorized.
		@param color_mode the new color mode string
		"""
		self.lock()
		if color_mode in COLORS.keys():
			self._color_mode = color_mode
			self._legend_cache.changed(True)
		self.update()
		self.unlock()

	def set_num_lines(self, num_lines):
		"""
		Set number of lines.
		Powers of two only.
		@param num_lines the new number of lines
		"""
		self.lock()
		self._num_lines = num_lines
		self._resize_texture(True)
		self.update()
		self.unlock()

	def set_samples(self, samples, minimum, maximum):
		"""
		Set the samples to the waterfall.
		Convert the samples to color data.
		@param samples the array of floats
		@param minimum the minimum value to scale
		@param maximum the maximum value to scale
		"""
		self.lock()
		#set the min, max values
		if self._minimum != minimum or self._maximum != maximum:
			self._minimum = minimum
			self._maximum = maximum
			self._legend_cache.changed(True)
		if self._fft_size != len(samples):
			self._fft_size = len(samples)
			self._resize_texture(True)
		#normalize the samples to min/max
		samples = (samples - minimum)*float(255/(maximum-minimum))
		samples = numpy.clip(samples, 0, 255) #clip
		samples = numpy.array(samples, numpy.uint8)
		#convert the samples to RGBA data
		data = COLORS[self._color_mode][samples].tostring()
		self._buffer.append(data)
		self._waterfall_cache.changed(True)
		self.unlock()
