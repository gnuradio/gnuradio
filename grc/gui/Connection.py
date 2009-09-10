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

import Utils
from Element import Element
import Colors
from Constants import CONNECTOR_ARROW_BASE, CONNECTOR_ARROW_HEIGHT

class Connection(Element):
	"""
	A graphical connection for ports.
	The connection has 2 parts, the arrow and the wire.
	The coloring of the arrow and wire exposes the status of 3 states:
		enabled/disabled, valid/invalid, highlighted/non-highlighted.
	The wire coloring exposes the enabled and highlighted states.
	The arrow coloring exposes the enabled and valid states.
	"""

	def __init__(self): Element.__init__(self)

	def get_coordinate(self):
		"""
		Get the 0,0 coordinate.
		Coordinates are irrelevant in connection.
		@return 0, 0
		"""
		return (0, 0)

	def get_rotation(self):
		"""
		Get the 0 degree rotation.
		Rotations are irrelevant in connection.
		@return 0
		"""
		return 0

	def create_shapes(self):
		"""Precalculate relative coordinates."""
		Element.create_shapes(self)
		self._sink_rot = None
		self._source_rot = None
		self._sink_coor = None
		self._source_coor = None
		#get the source coordinate
		connector_length = self.get_source().get_connector_length()
		self.x1, self.y1 = Utils.get_rotated_coordinate((connector_length, 0), self.get_source().get_rotation())
		#get the sink coordinate
		connector_length = self.get_sink().get_connector_length() + CONNECTOR_ARROW_HEIGHT
		self.x2, self.y2 = Utils.get_rotated_coordinate((-connector_length, 0), self.get_sink().get_rotation())
		#build the arrow
		self.arrow = [(0, 0),
			Utils.get_rotated_coordinate((-CONNECTOR_ARROW_HEIGHT, -CONNECTOR_ARROW_BASE/2), self.get_sink().get_rotation()),
			Utils.get_rotated_coordinate((-CONNECTOR_ARROW_HEIGHT, CONNECTOR_ARROW_BASE/2), self.get_sink().get_rotation()),
		]
		self._update_after_move()
		if not self.get_enabled(): self._arrow_color = Colors.CONNECTION_DISABLED_COLOR
		elif not self.is_valid(): self._arrow_color = Colors.CONNECTION_ERROR_COLOR
		else: self._arrow_color = Colors.CONNECTION_ENABLED_COLOR

	def _update_after_move(self):
		"""Calculate coordinates."""
		self.clear() #FIXME do i want this here?
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
			mid_x, mid_y = (x1 + x2)/2.0, (y1 + y2)/2.0
			points = [((mid_x, y1), (mid_x, y2)), ((x1, mid_y), (x2, mid_y))]
			#source connector -> points[0][0] should be in the direction of source (if possible)
			if Utils.get_angle_from_coordinates((x1, y1), points[0][0]) != source.get_connector_direction(): points.reverse()
			#points[0][0] -> sink connector should not be in the direction of sink
			if Utils.get_angle_from_coordinates(points[0][0], (x2, y2)) == sink.get_connector_direction(): points.reverse()
			#points[0][0] -> source connector should not be in the direction of source
			if Utils.get_angle_from_coordinates(points[0][0], (x1, y1)) == source.get_connector_direction(): points.reverse()
			#create 3-line connector
			p1, p2 = map(int, points[0][0]), map(int, points[0][1])
			self.add_line((x1, y1), p1)
			self.add_line(p1, p2)
			self.add_line((x2, y2), p2)
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

	def draw(self, gc, window):
		"""
		Draw the connection.
		@param gc the graphics context
		@param window the gtk window to draw on
		"""
		sink = self.get_sink()
		source = self.get_source()
		#check for changes
		if self._sink_rot != sink.get_rotation() or self._source_rot != source.get_rotation(): self.create_shapes()
		elif self._sink_coor != sink.get_coordinate() or self._source_coor != source.get_coordinate(): self._update_after_move()
		#cache values
		self._sink_rot = sink.get_rotation()
		self._source_rot = source.get_rotation()
		self._sink_coor = sink.get_coordinate()
		self._source_coor = source.get_coordinate()
		#draw
		if self.is_highlighted(): border_color = Colors.HIGHLIGHT_COLOR
		elif self.get_enabled(): border_color = Colors.CONNECTION_ENABLED_COLOR
		else: border_color = Colors.CONNECTION_DISABLED_COLOR
		Element.draw(self, gc, window, bg_color=None, border_color=border_color)
		#draw arrow on sink port
		gc.set_foreground(self._arrow_color)
		window.draw_polygon(gc, True, self._arrow)
