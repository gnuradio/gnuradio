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
import Utils
import Colors
from .. base import odict
from Constants import BORDER_PROXIMITY_SENSITIVITY
from Constants import \
	BLOCK_LABEL_PADDING, \
	PORT_SEPARATION, LABEL_SEPARATION, \
	PORT_BORDER_SEPARATION, POSSIBLE_ROTATIONS
import pygtk
pygtk.require('2.0')
import gtk
import pango

BLOCK_MARKUP_TMPL="""\
#set $foreground = $block.is_valid() and 'black' or 'red'
<span foreground="$foreground" font_desc="Sans 8"><b>$encode($block.get_name())</b></span>"""

class Block(Element):
	"""The graphical signal block."""

	def __init__(self):
		"""
		Block contructor.
		Add graphics related params to the block.
		"""
		#add the position param
		self.get_params().append(self.get_parent().get_parent().Param(
			block=self,
			n=odict({
				'name': 'GUI Coordinate',
				'key': '_coordinate',
				'type': 'raw',
				'value': '(0, 0)',
				'hide': 'all',
			})
		))
		self.get_params().append(self.get_parent().get_parent().Param(
			block=self,
			n=odict({
				'name': 'GUI Rotation',
				'key': '_rotation',
				'type': 'raw',
				'value': '0',
				'hide': 'all',
			})
		))
		Element.__init__(self)

	def get_coordinate(self):
		"""
		Get the coordinate from the position param.
		@return the coordinate tuple (x, y) or (0, 0) if failure
		"""
		try: #should evaluate to tuple
			coor = eval(self.get_param('_coordinate').get_value())
			x, y = map(int, coor)
			fgW,fgH = self.get_parent().get_size()
			if x <= 0:
				x = 0
			elif x >= fgW - BORDER_PROXIMITY_SENSITIVITY:
				x = fgW - BORDER_PROXIMITY_SENSITIVITY
			if y <= 0:
				y = 0
			elif y >= fgH - BORDER_PROXIMITY_SENSITIVITY:
				y = fgH - BORDER_PROXIMITY_SENSITIVITY
			return (x, y)
		except:
			self.set_coordinate((0, 0))
			return (0, 0)

	def set_coordinate(self, coor):
		"""
		Set the coordinate into the position param.
		@param coor the coordinate tuple (x, y)
		"""
		self.get_param('_coordinate').set_value(str(coor))

	def get_rotation(self):
		"""
		Get the rotation from the position param.
		@return the rotation in degrees or 0 if failure
		"""
		try: #should evaluate to dict
			rotation = eval(self.get_param('_rotation').get_value())
			return int(rotation)
		except:
			self.set_rotation(POSSIBLE_ROTATIONS[0])
			return POSSIBLE_ROTATIONS[0]

	def set_rotation(self, rot):
		"""
		Set the rotation into the position param.
		@param rot the rotation in degrees
		"""
		self.get_param('_rotation').set_value(str(rot))

	def create_shapes(self):
		"""Update the block, parameters, and ports when a change occurs."""
		Element.create_shapes(self)
		if self.is_horizontal(): self.add_area((0, 0), (self.W, self.H))
		elif self.is_vertical(): self.add_area((0, 0), (self.H, self.W))

	def create_labels(self):
		"""Create the labels for the signal block."""
		Element.create_labels(self)
		self._bg_color = self.get_enabled() and Colors.BLOCK_ENABLED_COLOR or Colors.BLOCK_DISABLED_COLOR
		layouts = list()
		#create the main layout
		layout = gtk.DrawingArea().create_pango_layout('')
		layouts.append(layout)
		layout.set_markup(Utils.parse_template(BLOCK_MARKUP_TMPL, block=self))
		self.label_width, self.label_height = layout.get_pixel_size()
		#display the params
		markups = [param.get_markup() for param in self.get_params() if param.get_hide() not in ('all', 'part')]
		if markups:
			layout = gtk.DrawingArea().create_pango_layout('')
			layout.set_spacing(LABEL_SEPARATION*pango.SCALE)
			layout.set_markup('\n'.join(markups))
			layouts.append(layout)
			w,h = layout.get_pixel_size()
			self.label_width = max(w, self.label_width)
			self.label_height += h + LABEL_SEPARATION
		width = self.label_width
		height = self.label_height
		#setup the pixmap
		pixmap = self.get_parent().new_pixmap(width, height)
		gc = pixmap.new_gc()
		gc.set_foreground(self._bg_color)
		pixmap.draw_rectangle(gc, True, 0, 0, width, height)
		#draw the layouts
		h_off = 0
		for i,layout in enumerate(layouts):
			w,h = layout.get_pixel_size()
			if i == 0: w_off = (width-w)/2
			else: w_off = 0
			pixmap.draw_layout(gc, w_off, h_off, layout)
			h_off = h + h_off + LABEL_SEPARATION
		#create vertical and horizontal pixmaps
		self.horizontal_label = pixmap
		if self.is_vertical():
			self.vertical_label = self.get_parent().new_pixmap(height, width)
			Utils.rotate_pixmap(gc, self.horizontal_label, self.vertical_label)
		#calculate width and height needed
		self.W = self.label_width + 2*BLOCK_LABEL_PADDING
		self.H = max(*(
			[self.label_height+2*BLOCK_LABEL_PADDING] + [2*PORT_BORDER_SEPARATION + \
			sum([port.H + PORT_SEPARATION for port in ports]) - PORT_SEPARATION
			for ports in (self.get_sources(), self.get_sinks())]
		))

	def draw(self, gc, window):
		"""
		Draw the signal block with label and inputs/outputs.
		@param gc the graphics context
		@param window the gtk window to draw on
		"""
		x, y = self.get_coordinate()
		#draw main block
		Element.draw(
			self, gc, window, bg_color=self._bg_color,
			border_color=self.is_highlighted() and Colors.HIGHLIGHT_COLOR or Colors.BORDER_COLOR,
		)
		#draw label image
		if self.is_horizontal():
			window.draw_drawable(gc, self.horizontal_label, 0, 0, x+BLOCK_LABEL_PADDING, y+(self.H-self.label_height)/2, -1, -1)
		elif self.is_vertical():
			window.draw_drawable(gc, self.vertical_label, 0, 0, x+(self.H-self.label_height)/2, y+BLOCK_LABEL_PADDING, -1, -1)
		#draw ports
		for port in self.get_ports(): port.draw(gc, window)

	def what_is_selected(self, coor, coor_m=None):
		"""
		Get the element that is selected.
		@param coor the (x,y) tuple
		@param coor_m the (x_m, y_m) tuple
		@return this block, a port, or None
		"""
		for port in self.get_ports():
			port_selected = port.what_is_selected(coor, coor_m)
			if port_selected: return port_selected
		return Element.what_is_selected(self, coor, coor_m)
