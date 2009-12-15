"""
Copyright 2007, 2008, 2009 Free Software Foundation, Inc.
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

from Element import Element
from Constants import \
	PORT_SEPARATION, CONNECTOR_EXTENSION_MINIMAL, \
	CONNECTOR_EXTENSION_INCREMENT, \
	PORT_LABEL_PADDING, PORT_MIN_WIDTH
import Utils
import Colors
import pygtk
pygtk.require('2.0')
import gtk

PORT_MARKUP_TMPL="""\
<span foreground="black" font_desc="Sans 7.5">$encode($port.get_name())</span>"""

class Port(Element):
	"""The graphical port."""

	def __init__(self):
		"""
		Port contructor.
		Create list of connector coordinates.
		"""
		Element.__init__(self)
		self.connector_coordinates = dict()

	def create_shapes(self):
		"""Create new areas and labels for the port."""
		Element.create_shapes(self)
		#get current rotation
		rotation = self.get_rotation()
		#get all sibling ports
		if self.is_source(): ports = self.get_parent().get_sources()
		elif self.is_sink(): ports = self.get_parent().get_sinks()
		#get the max width
		self.W = max([port.W for port in ports] + [PORT_MIN_WIDTH])
		#get a numeric index for this port relative to its sibling ports
		index = ports.index(self)
		length = len(ports)
		#reverse the order of ports	for these rotations
		if rotation in (180, 270): index = length-index-1
		offset = (self.get_parent().H - length*self.H - (length-1)*PORT_SEPARATION)/2
		#create areas and connector coordinates
		if (self.is_sink() and rotation == 0) or (self.is_source() and rotation == 180):
			x = -1*self.W
			y = (PORT_SEPARATION+self.H)*index+offset
			self.add_area((x, y), (self.W, self.H))
			self._connector_coordinate = (x-1, y+self.H/2)
		elif (self.is_source() and rotation == 0) or (self.is_sink() and rotation == 180):
			x = self.get_parent().W
			y = (PORT_SEPARATION+self.H)*index+offset
			self.add_area((x, y), (self.W, self.H))
			self._connector_coordinate = (x+1+self.W, y+self.H/2)
		elif (self.is_source() and rotation == 90) or (self.is_sink() and rotation == 270):
			y = -1*self.W
			x = (PORT_SEPARATION+self.H)*index+offset
			self.add_area((x, y), (self.H, self.W))
			self._connector_coordinate = (x+self.H/2, y-1)
		elif (self.is_sink() and rotation == 90) or (self.is_source() and rotation == 270):
			y = self.get_parent().W
			x = (PORT_SEPARATION+self.H)*index+offset
			self.add_area((x, y), (self.H, self.W))
			self._connector_coordinate = (x+self.H/2, y+1+self.W)
		#the connector length
		self._connector_length = CONNECTOR_EXTENSION_MINIMAL + CONNECTOR_EXTENSION_INCREMENT*index

	def create_labels(self):
		"""Create the labels for the socket."""
		Element.create_labels(self)
		self._bg_color = Colors.get_color(self.get_color())
		#create the layout
		layout = gtk.DrawingArea().create_pango_layout('')
		layout.set_markup(Utils.parse_template(PORT_MARKUP_TMPL, port=self))
		self.w, self.h = layout.get_pixel_size()
		self.W, self.H = 2*PORT_LABEL_PADDING+self.w, 2*PORT_LABEL_PADDING+self.h
		#create the pixmap
		pixmap = self.get_parent().get_parent().new_pixmap(self.w, self.h)
		gc = pixmap.new_gc()
		gc.set_foreground(self._bg_color)
		pixmap.draw_rectangle(gc, True, 0, 0, self.w, self.h)
		pixmap.draw_layout(gc, 0, 0, layout)
		#create vertical and horizontal pixmaps
		self.horizontal_label = pixmap
		if self.is_vertical():
			self.vertical_label = self.get_parent().get_parent().new_pixmap(self.h, self.w)
			Utils.rotate_pixmap(gc, self.horizontal_label, self.vertical_label)

	def draw(self, gc, window):
		"""
		Draw the socket with a label.
		@param gc the graphics context
		@param window the gtk window to draw on
		"""
		Element.draw(
			self, gc, window, bg_color=self._bg_color,
			border_color=self.is_highlighted() and Colors.HIGHLIGHT_COLOR or Colors.BORDER_COLOR,
		)
		X,Y = self.get_coordinate()
		(x,y),(w,h) = self._areas_list[0] #use the first area's sizes to place the labels
		if self.is_horizontal():
			window.draw_drawable(gc, self.horizontal_label, 0, 0, x+X+(self.W-self.w)/2, y+Y+(self.H-self.h)/2, -1, -1)
		elif self.is_vertical():
			window.draw_drawable(gc, self.vertical_label, 0, 0, x+X+(self.H-self.h)/2, y+Y+(self.W-self.w)/2, -1, -1)

	def get_connector_coordinate(self):
		"""
		Get the coordinate where connections may attach to.
		@return the connector coordinate (x, y) tuple
		"""
		x,y = self._connector_coordinate
		X,Y = self.get_coordinate()
		return (x+X, y+Y)

	def get_connector_direction(self):
		"""
		Get the direction that the socket points: 0,90,180,270.
		This is the rotation degree if the socket is an output or
		the rotation degree + 180 if the socket is an input.
		@return the direction in degrees
		"""
		if self.is_source(): return self.get_rotation()
		elif self.is_sink(): return (self.get_rotation() + 180)%360

	def get_connector_length(self):
		"""
		Get the length of the connector.
		The connector length increases as the port index changes.
		@return the length in pixels
		"""
		return self._connector_length

	def get_rotation(self):
		"""
		Get the parent's rotation rather than self.
		@return the parent's rotation
		"""
		return self.get_parent().get_rotation()

	def move(self, delta_coor):
		"""
		Move the parent rather than self.
		@param delta_corr the (delta_x, delta_y) tuple
		"""
		self.get_parent().move(delta_coor)

	def rotate(self, direction):
		"""
		Rotate the parent rather than self.
		@param direction degrees to rotate
		"""
		self.get_parent().rotate(direction)

	def get_coordinate(self):
		"""
		Get the parent's coordinate rather than self.
		@return the parents coordinate
		"""
		return self.get_parent().get_coordinate()

	def set_highlighted(self, highlight):
		"""
		Set the parent highlight rather than self.
		@param highlight true to enable highlighting
		"""
		self.get_parent().set_highlighted(highlight)

	def is_highlighted(self):
		"""
		Get the parent's is highlight rather than self.
		@return the parent's highlighting status
		"""
		return self.get_parent().is_highlighted()
