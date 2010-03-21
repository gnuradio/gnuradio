"""
Copyright 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import pygtk
pygtk.require('2.0')
import gtk
from Constants import MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT, DND_TARGETS

class DrawingArea(gtk.DrawingArea):
	"""
	DrawingArea is the gtk pixel map that graphical elements may draw themselves on.
	The drawing area also responds to mouse and key events.
	"""

	def __init__(self, flow_graph):
		"""
		DrawingArea contructor.
		Connect event handlers.
		@param main_window the main_window containing all flow graphs
		"""
		self.ctrl_mask = False
		self._flow_graph = flow_graph
		gtk.DrawingArea.__init__(self)
		self.set_size_request(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT)
		self.connect('realize', self._handle_window_realize)
		self.connect('configure-event', self._handle_window_configure)
		self.connect('expose-event', self._handle_window_expose)
		self.connect('motion-notify-event', self._handle_mouse_motion)
		self.connect('button-press-event', self._handle_mouse_button_press)
		self.connect('button-release-event', self._handle_mouse_button_release)
		self.add_events(
			gtk.gdk.BUTTON_PRESS_MASK | \
			gtk.gdk.POINTER_MOTION_MASK | \
			gtk.gdk.BUTTON_RELEASE_MASK | \
			gtk.gdk.LEAVE_NOTIFY_MASK | \
			gtk.gdk.ENTER_NOTIFY_MASK
		)
		#setup drag and drop
		self.drag_dest_set(gtk.DEST_DEFAULT_ALL, DND_TARGETS, gtk.gdk.ACTION_COPY)
		self.connect('drag-data-received', self._handle_drag_data_received)
		#setup the focus flag
		self._focus_flag = False
		self.get_focus_flag = lambda: self._focus_flag
		def _handle_focus_event(widget, event, focus_flag): self._focus_flag = focus_flag
		self.connect('leave-notify-event', _handle_focus_event, False)
		self.connect('enter-notify-event', _handle_focus_event, True)

	def new_pixmap(self, width, height): return gtk.gdk.Pixmap(self.window, width, height, -1)
	def get_pixbuf(self):
		width, height = self._pixmap.get_size()
		pixbuf = gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, 0, 8, width, height)
		pixbuf.get_from_drawable(self._pixmap, self._pixmap.get_colormap(), 0, 0, 0, 0, width, height)
		return pixbuf

	##########################################################################
	## Handlers
	##########################################################################
	def _handle_drag_data_received(self, widget, drag_context, x, y, selection_data, info, time):
		"""
		Handle a drag and drop by adding a block at the given coordinate.
		"""
		self._flow_graph.add_new_block(selection_data.data, (x, y))

	def _handle_mouse_button_press(self, widget, event):
		"""
		Forward button click information to the flow graph.
		"""
		self.ctrl_mask = event.state & gtk.gdk.CONTROL_MASK
		if event.button == 1: self._flow_graph.handle_mouse_selector_press(
			double_click=(event.type == gtk.gdk._2BUTTON_PRESS),
			coordinate=(event.x, event.y),
		)
		if event.button == 3: self._flow_graph.handle_mouse_context_press(
			coordinate=(event.x, event.y),
			event=event,
		)

	def _handle_mouse_button_release(self, widget, event):
		"""
		Forward button release information to the flow graph.
		"""
		self.ctrl_mask = event.state & gtk.gdk.CONTROL_MASK
		if event.button == 1: self._flow_graph.handle_mouse_selector_release(
			coordinate=(event.x, event.y),
		)

	def _handle_mouse_motion(self, widget, event):
		"""
		Forward mouse motion information to the flow graph.
		"""
		self.ctrl_mask = event.state & gtk.gdk.CONTROL_MASK
		self._flow_graph.handle_mouse_motion(
			coordinate=(event.x, event.y),
		)

	def _handle_window_realize(self, widget):
		"""
		Called when the window is realized.
		Update the flowgraph, which calls new pixmap.
		"""
		self._flow_graph.update()

	def _handle_window_configure(self, widget, event):
		"""
		Called when the window is resized.
		Create a new pixmap for background buffer.
		"""
		self._pixmap = self.new_pixmap(*self.get_size_request())

	def _handle_window_expose(self, widget, event):
		"""
		Called when window is exposed, or queue_draw is called.
		Double buffering: draw to pixmap, then draw pixmap to window.
		"""
		gc = self.window.new_gc()
		self._flow_graph.draw(gc, self._pixmap)
		self.window.draw_drawable(gc, self._pixmap, 0, 0, 0, 0, -1, -1)
