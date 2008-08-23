"""
Copyright 2007, 2008 Free Software Foundation, Inc.
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
##@package grc.gui.elements.Connection
#The graphical connection for input/output ports.

import Utils
from Element import Element
import Colors
from grc.Constants import CONNECTOR_ARROW_BASE,CONNECTOR_ARROW_HEIGHT

class Connection(Element):
	"""A graphical connection for ports."""

	def get_coordinate(self):
		"""!
		Get the 0,0 coordinate.
		Coordinates are irrelevant in connection.
		@return 0, 0
		"""
		return (0, 0)

	def get_rotation(self):
		"""!
		Get the 0 degree rotation.
		Rotations are irrelevant in connection.
		@return 0
		"""
		return 0

	def update(self):
		"""Precalculate relative coordinates."""
		self._sink_rot = None
		self._source_rot = None
		self._sink_coor = None
		self._source_coor = None

		sink = self.get_sink()
		source = self.get_source()

		#get the source coordinate
		x1, y1 = 0, 0
		connector_length = source.get_connector_length()
		if source.get_rotation() == 0:
			x1 = 0 + connector_length
		elif source.get_rotation() == 90:
			y1 = 0 - connector_length
		elif source.get_rotation() == 180:
			x1 = 0 - connector_length
		elif source.get_rotation() == 270:
			y1 = 0 + connector_length
		self.x1, self.y1 = x1, y1

		#get the sink coordinate
		x2, y2 = 0, 0
		connector_length = sink.get_connector_length() + CONNECTOR_ARROW_HEIGHT
		if sink.get_rotation() == 0:
			x2 = 0 - connector_length
		elif sink.get_rotation() == 90:
			y2 = 0 + connector_length
		elif sink.get_rotation() == 180:
			x2 = 0 + connector_length
		elif sink.get_rotation() == 270:
			y2 = 0 - connector_length
		self.x2, self.y2 = x2, y2

		#build the arrow
		if sink.get_rotation() == 0:
			self.arrow = [(0, 0), (0-CONNECTOR_ARROW_HEIGHT, 0-CONNECTOR_ARROW_BASE/2), (0-CONNECTOR_ARROW_HEIGHT, 0+CONNECTOR_ARROW_BASE/2)]
		elif sink.get_rotation() == 90:
			self.arrow = [(0, 0), (0-CONNECTOR_ARROW_BASE/2, 0+CONNECTOR_ARROW_HEIGHT), (0+CONNECTOR_ARROW_BASE/2, 0+CONNECTOR_ARROW_HEIGHT)]
		elif sink.get_rotation() == 180:
			self.arrow = [(0, 0), (0+CONNECTOR_ARROW_HEIGHT, 0-CONNECTOR_ARROW_BASE/2), (0+CONNECTOR_ARROW_HEIGHT, 0+CONNECTOR_ARROW_BASE/2)]
		elif sink.get_rotation() == 270:
			self.arrow = [(0, 0), (0-CONNECTOR_ARROW_BASE/2, 0-CONNECTOR_ARROW_HEIGHT), (0+CONNECTOR_ARROW_BASE/2, 0-CONNECTOR_ARROW_HEIGHT)]

		self._update_after_move()

	def _update_after_move(self):
		"""Calculate coordinates."""

		self.clear()

		#source connector
		source = self.get_source()
		X, Y = source.get_connector_coordinate()
		x1, y1 = self.x1 + X, self.y1 + Y
		self.add_line((x1, y1), (X, Y))

		#sink connector
		sink = self.get_sink()
		X, Y = sink.get_connector_coordinate()
		x2, y2 = self.x2 + X, self.y2 + Y
		self.add_line((x2, y2), (X, Y))

		#adjust arrow
		self._arrow = [(x+X, y+Y) for x,y in self.arrow]

		#add the horizontal and vertical lines in this connection
		if abs(source.get_connector_direction() - sink.get_connector_direction()) == 180:
			#2 possible point sets to create a 3-line connector
			mid_x, mid_y = (x1 + x2)/2, (y1 + y2)/2
			points = [((mid_x, y1), (mid_x, y2)), ((x1, mid_y), (x2, mid_y))]
			#source connector -> points[0][0] should be in the direction of source (if possible)
			if Utils.get_angle_from_coordinates((x1, y1), points[0][0]) != source.get_connector_direction(): points.reverse()
			#points[0][0] -> sink connector should not be in the direction of sink
			if Utils.get_angle_from_coordinates(points[0][0], (x2, y2)) == sink.get_connector_direction(): points.reverse()
			#points[0][0] -> source connector should not be in the direction of source
			if Utils.get_angle_from_coordinates(points[0][0], (x1, y1)) == source.get_connector_direction(): points.reverse()
			#create 3-line connector
			self.add_line((x1, y1), points[0][0])
			self.add_line(points[0][0], points[0][1])
			self.add_line((x2, y2), points[0][1])
		else:
			#2 possible points to create a right-angled connector
			points = [(x1, y2), (x2, y1)]
			#source connector -> points[0] should be in the direction of source (if possible)
			if Utils.get_angle_from_coordinates((x1, y1), points[0]) != source.get_connector_direction(): points.reverse()
			#points[0] -> sink connector should not be in the direction of sink
			if Utils.get_angle_from_coordinates(points[0], (x2, y2)) == sink.get_connector_direction(): points.reverse()
			#points[0] -> source connector should not be in the direction of source
			if Utils.get_angle_from_coordinates(points[0], (x1, y1)) == source.get_connector_direction(): points.reverse()
			#create right-angled connector
			self.add_line((x1, y1), points[0])
			self.add_line((x2, y2), points[0])

	def draw(self, window):
		"""!
		Draw the connection.
		@param window the gtk window to draw on
		"""
		sink = self.get_sink()
		source = self.get_source()
		#check for changes
		if self._sink_rot != sink.get_rotation() or self._source_rot != source.get_rotation(): self.update()
		elif self._sink_coor != sink.get_coordinate() or self._source_coor != source.get_coordinate(): self._update_after_move()
		#cache values
		self._sink_rot = sink.get_rotation()
		self._source_rot = source.get_rotation()
		self._sink_coor = sink.get_coordinate()
		self._source_coor = source.get_coordinate()
		#draw
		fg_color = self.get_enabled() and Colors.FG_COLOR or Colors.DISABLED_FG_COLOR
		Element.draw(self, window, FG_color=fg_color)
		gc = self.get_gc()
		if self.is_valid(): gc.foreground = Colors.FG_COLOR
		else: gc.foreground = Colors.ERROR_COLOR
		#draw arrow on sink port
		window.draw_polygon(gc, True, self._arrow)
