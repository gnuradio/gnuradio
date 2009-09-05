"""
Copyright 2007 Free Software Foundation, Inc.
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

import Colors
import pygtk
pygtk.require('2.0')
import gtk
import pango
from Constants import LINE_SELECT_SENSITIVITY
from Constants import POSSIBLE_ROTATIONS

class Element(object):
	"""
	GraphicalElement is the base class for all graphical elements.
	It contains an X,Y coordinate, a list of rectangular areas that the element occupies,
	and methods to detect selection of those areas.
	"""

	def __init__(self):
		"""
		Make a new list of rectangular areas and lines, and set the coordinate and the rotation.
		"""
		self.set_rotation(POSSIBLE_ROTATIONS[0])
		self.set_coordinate((0, 0))
		self.clear()
		self.set_highlighted(False)

	def is_horizontal(self, rotation=None):
		"""
		Is this element horizontal?
		If rotation is None, use this element's rotation.
		@param rotation the optional rotation
		@return true if rotation is horizontal
		"""
		rotation = rotation or self.get_rotation()
		return rotation in (0, 180)

	def is_vertical(self, rotation=None):
		"""
		Is this element vertical?
		If rotation is None, use this element's rotation.
		@param rotation the optional rotation
		@return true if rotation is vertical
		"""
		rotation = rotation or self.get_rotation()
		return rotation in (90, 270)

	def draw(self, gc, window, border_color, bg_color):
		"""
		Draw in the given window.
		@param gc the graphics context
		@param window the gtk window to draw on
		@param border_color the color for lines and rectangle borders
		@param bg_color the color for the inside of the rectangle
		"""
		X,Y = self.get_coordinate()
		for (rX,rY),(W,H) in self.areas_dict[self.get_rotation()]:
			aX = X + rX
			aY = Y + rY
			gc.set_foreground(bg_color)
			window.draw_rectangle(gc, True, aX, aY, W, H)
			gc.set_foreground(border_color)
			window.draw_rectangle(gc, False, aX, aY, W, H)
		for (x1, y1),(x2, y2) in self.lines_dict[self.get_rotation()]:
			gc.set_foreground(border_color)
			window.draw_line(gc, X+x1, Y+y1, X+x2, Y+y2)

	def rotate(self, rotation):
		"""
		Rotate all of the areas by 90 degrees.
		@param rotation multiple of 90 degrees
		"""
		self.set_rotation((self.get_rotation() + rotation)%360)

	def clear(self):
		"""Empty the lines and areas."""
		self.areas_dict = dict((rotation, list()) for rotation in POSSIBLE_ROTATIONS)
		self.lines_dict = dict((rotation, list()) for rotation in POSSIBLE_ROTATIONS)

	def set_coordinate(self, coor):
		"""
		Set the reference coordinate.
		@param coor the coordinate tuple (x,y)
		"""
		self.coor = coor

	def get_parent(self):
		"""
		Get the parent of this element.
		@return the parent
		"""
		return self.parent

	def set_highlighted(self, highlighted):
		"""
		Set the highlight status.
		@param highlighted true to enable highlighting
		"""
		self.highlighted = highlighted

	def is_highlighted(self):
		"""
		Get the highlight status.
		@return true if highlighted
		"""
		return self.highlighted

	def get_coordinate(self):
		"""Get the coordinate.
		@return the coordinate tuple (x,y)
		"""
		return self.coor

	def move(self, delta_coor):
		"""
		Move the element by adding the delta_coor to the current coordinate.
		@param delta_coor (delta_x,delta_y) tuple
		"""
		deltaX, deltaY = delta_coor
		X, Y = self.get_coordinate()
		self.set_coordinate((X+deltaX, Y+deltaY))

	def add_area(self, rel_coor, area, rotation=None):
		"""
		Add an area to the area list.
		An area is actually a coordinate relative to the main coordinate
		with a width/height pair relative to the area coordinate.
		A positive width is to the right of the coordinate.
		A positive height is above the coordinate.
		The area is associated with a rotation.
		If rotation is not specified, the element's current rotation is used.
		@param rel_coor (x,y) offset from this element's coordinate
		@param area (width,height) tuple
		@param rotation rotation in degrees
		"""
		self.areas_dict[rotation or self.get_rotation()].append((rel_coor, area))

	def add_line(self, rel_coor1, rel_coor2, rotation=None):
		"""
		Add a line to the line list.
		A line is defined by 2 relative coordinates.
		Lines must be horizontal or vertical.
		The line is associated with a rotation.
		If rotation is not specified, the element's current rotation is used.
		@param rel_coor1 relative (x1,y1) tuple
		@param rel_coor2 relative (x2,y2) tuple
		@param rotation rotation in degrees
		"""
		self.lines_dict[rotation or self.get_rotation()].append((rel_coor1, rel_coor2))

	def what_is_selected(self, coor, coor_m=None):
		"""
		One coordinate specified:
			Is this element selected at given coordinate?
			ie: is the coordinate encompassed by one of the areas or lines?
		Both coordinates specified:
			Is this element within the rectangular region defined by both coordinates?
			ie: do any area corners or line endpoints fall within the region?
		@param coor the selection coordinate, tuple x, y
		@param coor_m an additional selection coordinate.
		@return self if one of the areas/lines encompasses coor, else None.
		"""
		#function to test if p is between a and b (inclusive)
		in_between = lambda p, a, b: p >= min(a, b) and p <= max(a, b)
		#relative coordinate
		x, y = [a-b for a,b in zip(coor, self.get_coordinate())]
		if coor_m:
			x_m, y_m = [a-b for a,b in zip(coor_m, self.get_coordinate())]
			#handle rectangular areas
			for (x1,y1), (w,h) in self.areas_dict[self.get_rotation()]:
				if in_between(x1, x, x_m) and in_between(y1, y, y_m) or \
					in_between(x1+w, x, x_m) and in_between(y1, y, y_m) or \
					in_between(x1, x, x_m) and in_between(y1+h, y, y_m) or \
					in_between(x1+w, x, x_m) and in_between(y1+h, y, y_m):
					return self
			#handle horizontal or vertical lines
			for (x1, y1), (x2, y2) in self.lines_dict[self.get_rotation()]:
				if in_between(x1, x, x_m) and in_between(y1, y, y_m) or \
					in_between(x2, x, x_m) and in_between(y2, y, y_m):
					return self
			return None
		else:
			#handle rectangular areas
			for (x1,y1), (w,h) in self.areas_dict[self.get_rotation()]:
				if in_between(x, x1, x1+w) and in_between(y, y1, y1+h): return self
			#handle horizontal or vertical lines
			for (x1, y1), (x2, y2) in self.lines_dict[self.get_rotation()]:
				if x1 == x2: x1, x2 = x1-LINE_SELECT_SENSITIVITY, x2+LINE_SELECT_SENSITIVITY
				if y1 == y2: y1, y2 = y1-LINE_SELECT_SENSITIVITY, y2+LINE_SELECT_SENSITIVITY
				if in_between(x, x1, x2) and in_between(y, y1, y2): return self
			return None

	def get_rotation(self):
		"""
		Get the rotation in degrees.
		@return the rotation
		"""
		return self.rotation

	def set_rotation(self, rotation):
		"""
		Set the rotation in degrees.
		@param rotation the rotation"""
		if rotation not in POSSIBLE_ROTATIONS:
			raise Exception('"%s" is not one of the possible rotations: (%s)'%(rotation, POSSIBLE_ROTATIONS))
		self.rotation = rotation

	def update(self):
		"""Do nothing for the update. Dummy method."""
		pass
