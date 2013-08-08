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
import wx.glcanvas
from OpenGL import GL
import common

BACKGROUND_COLOR_SPEC = (1, 0.976, 1, 1) #creamy white

##################################################
# GL caching interface
##################################################
class gl_cache(object):
	"""
	Cache a set of gl drawing routines in a compiled list.
	"""

	def __init__(self, draw):
		"""
		Create a new cache.

                Args:
		    draw: a function to draw gl stuff
		"""
		self.changed(True)
		self._draw = draw

	def init(self):
		"""
		To be called when gl initializes.
		Create a new compiled list.
		"""
		self._grid_compiled_list_id = GL.glGenLists(1)

	def draw(self):
		"""
		Draw the gl stuff using a compiled list.
		If changed, reload the compiled list.
		"""
		if self.changed():
			GL.glNewList(self._grid_compiled_list_id, GL.GL_COMPILE)
			self._draw()
			GL.glEndList()
			self.changed(False)
		#draw the grid
		GL.glCallList(self._grid_compiled_list_id)

	def changed(self, state=None):
		"""
		Set the changed flag if state is not None.
		Otherwise return the changed flag.
		"""
		if state is None: return self._changed
		self._changed = state

##################################################
# OpenGL WX Plotter Canvas
##################################################
class plotter_base(wx.glcanvas.GLCanvas, common.mutex):
	"""
	Plotter base class for all plot types.
	"""

	def __init__(self, parent):
		"""
		Create a new plotter base.
		Initialize the GLCanvas with double buffering.
		Initialize various plotter flags.
		Bind the paint and size events.

                Args:
		    parent: the parent widgit
		"""
		attribList = (wx.glcanvas.WX_GL_DOUBLEBUFFER, wx.glcanvas.WX_GL_RGBA)
		wx.glcanvas.GLCanvas.__init__(self, parent, wx.ID_ANY, attribList=attribList);	# Specifically use the CTOR which does NOT create an implicit GL context
		self._gl_ctx = wx.glcanvas.GLContext(self)	# Create the explicit GL context
		self.use_persistence=False
		self.persist_alpha=2.0/15
		self.clear_accum=True
		self._gl_init_flag = False
		self._resized_flag = True
		self._init_fcns = list()
		self._draw_fcns = list()
		self._gl_caches = list()
		self.Bind(wx.EVT_PAINT, self._on_paint)
		self.Bind(wx.EVT_SIZE, self._on_size)
		self.Bind(wx.EVT_ERASE_BACKGROUND, lambda e: None)

	def set_use_persistence(self,enable):
		self.use_persistence=enable
		self.clear_accum=True

	def set_persist_alpha(self,analog_alpha):
		self.persist_alpha=analog_alpha

	def new_gl_cache(self, draw_fcn, draw_pri=50):
		"""
		Create a new gl cache.
		Register its draw and init function.

                Returns:
		    the new cache object
		"""
		cache = gl_cache(draw_fcn)
		self.register_init(cache.init)
		self.register_draw(cache.draw, draw_pri)
		self._gl_caches.append(cache)
		return cache

	def register_init(self, init_fcn):
		self._init_fcns.append(init_fcn)

	def register_draw(self, draw_fcn, draw_pri=50):
		"""
		Register a draw function with a layer priority.
		Large pri values are drawn last.
		Small pri values are drawn first.
		"""
		for i in range(len(self._draw_fcns)):
			if draw_pri < self._draw_fcns[i][0]:
				self._draw_fcns.insert(i, (draw_pri, draw_fcn))
				return
		self._draw_fcns.append((draw_pri, draw_fcn))

	def _on_size(self, event):
		"""
		Flag the resize event.
		The paint event will handle the actual resizing.
		"""
		self.lock()
		self._resized_flag = True
		self.clear_accum=True
		self.unlock()

	def _on_paint(self, event):
		"""
		Respond to paint events.
		Initialize GL if this is the first paint event.
		Resize the view port if the width or height changed.
		Redraw the screen, calling the draw functions.
		"""
		if not self.IsShownOnScreen():	# Cannot realise a GL context on OS X if window is not yet shown
			return
		# create device context (needed on Windows, noop on X)
		dc = None
		if event.GetEventObject():	# Only create DC if paint triggered by WM message (for OS X)
			dc = wx.PaintDC(self)
		self.lock()
		self.SetCurrent(self._gl_ctx)	# Real the explicit GL context

		# check if gl was initialized
		if not self._gl_init_flag:
			GL.glClearColor(*BACKGROUND_COLOR_SPEC)
			for fcn in self._init_fcns: fcn()
			self._gl_init_flag = True

		# check for a change in window size
		if self._resized_flag:
			self.width, self.height = self.GetSize()
			GL.glMatrixMode(GL.GL_PROJECTION)
			GL.glLoadIdentity()
			GL.glOrtho(0, self.width, self.height, 0, 1, 0)
			GL.glMatrixMode(GL.GL_MODELVIEW)
			GL.glLoadIdentity()
			GL.glViewport(0, 0, self.width, self.height)
			for cache in self._gl_caches: cache.changed(True)
			self._resized_flag = False

		# clear buffer if needed
		if self.clear_accum or not self.use_persistence:
			GL.glClear(GL.GL_COLOR_BUFFER_BIT)
			self.clear_accum=False

		# apply fading
		if self.use_persistence:
			GL.glEnable(GL.GL_BLEND)
			GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)

			GL.glBegin(GL.GL_QUADS)
			GL.glColor4f(1,1,1,self.persist_alpha)
			GL.glVertex2f(0, self.height)
			GL.glVertex2f(self.width, self.height)
			GL.glVertex2f(self.width, 0)
			GL.glVertex2f(0, 0)
			GL.glEnd()

			GL.glDisable(GL.GL_BLEND)

		# draw functions
		for fcn in self._draw_fcns: fcn[1]()

		# show result
		self.SwapBuffers()
		self.unlock()

	def update(self):
		"""
		Force a paint event.
		"""
		if not self._gl_init_flag: return
		wx.PostEvent(self, wx.PaintEvent())
