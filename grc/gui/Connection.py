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

from __future__ import absolute_import

from . import Colors, Utils
from .Constants import CONNECTOR_ARROW_BASE, CONNECTOR_ARROW_HEIGHT
from .Element import Element

from ..core.Element import property_nop_write
from ..core.Connection import Connection as _Connection


class Connection(Element, _Connection):
    """
    A graphical connection for ports.
    The connection has 2 parts, the arrow and the wire.
    The coloring of the arrow and wire exposes the status of 3 states:
        enabled/disabled, valid/invalid, highlighted/non-highlighted.
    The wire coloring exposes the enabled and highlighted states.
    The arrow coloring exposes the enabled and valid states.
    """

    def __init__(self, **kwargs):
        Element.__init__(self)
        _Connection.__init__(self, **kwargs)

        self._color = self._color2 = self._arrow_color = None

        self._sink_rot = self._source_rot = None
        self._sink_coor = self._source_coor = None

    @property_nop_write
    def coordinate(self):
        return self.source_port.get_connector_coordinate()

    @property_nop_write
    def rotation(self):
        """
        Get the 0 degree rotation.
        Rotations are irrelevant in connection.

        Returns:
            0
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
        try:
            connector_length = self.source_port.connector_length
        except:
            return  # todo: why?
        self.x1, self.y1 = Utils.get_rotated_coordinate((connector_length, 0), self.source_port.rotation)
        #get the sink coordinate
        connector_length = self.sink_port.connector_length + CONNECTOR_ARROW_HEIGHT
        self.x2, self.y2 = Utils.get_rotated_coordinate((-connector_length, 0), self.sink_port.rotation)
        #build the arrow
        self.arrow = [(0, 0),
            Utils.get_rotated_coordinate((-CONNECTOR_ARROW_HEIGHT, -CONNECTOR_ARROW_BASE/2), self.sink_port.rotation),
            Utils.get_rotated_coordinate((-CONNECTOR_ARROW_HEIGHT, CONNECTOR_ARROW_BASE/2), self.sink_port.rotation),
        ]
        source_domain = self.source_port.domain
        sink_domain = self.sink_port.domain
        # self.line_attributes[0] = 2 if source_domain != sink_domain else 0
        # self.line_attributes[1] = Gdk.LINE_DOUBLE_DASH \
        #     if not source_domain == sink_domain == GR_MESSAGE_DOMAIN \
        #     else Gdk.LINE_ON_OFF_DASH

        def get_domain_color(domain_name):
            domain = self.parent_platform.domains.get(domain_name, {})
            color_spec = domain.get('color')
            return Colors.get_color(color_spec) if color_spec else \
                Colors.DEFAULT_DOMAIN_COLOR

        self._color = get_domain_color(source_domain)
        self._color2 = get_domain_color(sink_domain)
        self._arrow_color = self._color2 if self.is_valid() else Colors.CONNECTION_ERROR_COLOR
        self._update_after_move()

    def _update_after_move(self):
        """Calculate coordinates."""
        self.clear()
        source = self.source_port
        sink = self.sink_port
        source_dir = source.get_connector_direction()
        sink_dir = sink.get_connector_direction()

        x_pos, y_pos = self.coordinate
        x_start, y_start = source.get_connector_coordinate()
        x_end, y_end = sink.get_connector_coordinate()

        p3 = x3, y3 = x_end - x_pos, y_end - y_pos
        p2 = x2, y2 = self.x2 + x3, self.y2 + y3
        p1 = x1, y1 = self.x1, self.y1
        p0 = x_start - x_pos, y_start - y_pos
        self._arrow = [(x + x3, y + y3) for x, y in self.arrow]

        if abs(source_dir - sink.get_connector_direction()) == 180:
            # 2 possible point sets to create a 3-line connector
            mid_x, mid_y = (x1 + x2) / 2.0, (y1 + y2) / 2.0
            points = ((mid_x, y1), (mid_x, y2))
            alt = ((x1, mid_y), (x2, mid_y))
            # source connector -> points[0][0] should be in the direction of source (if possible)
            if Utils.get_angle_from_coordinates(p1, points[0]) != source_dir:
                points, alt = alt, points
            # points[0] -> sink connector should not be in the direction of sink
            if Utils.get_angle_from_coordinates(points[0], p2) == sink_dir:
                points, alt = alt, points
            # points[0] -> source connector should not be in the direction of source
            if Utils.get_angle_from_coordinates(points[0], p1) == source_dir:
                points, alt = alt, points
            # create 3-line connector
            i1, i2 = points
            self.lines.append([p0, p1, i1, i2, p2, p3])
        else:
            # 2 possible points to create a right-angled connector
            point, alt = [(x1, y2), (x2, y1)]
            # source connector -> point should be in the direction of source (if possible)
            if Utils.get_angle_from_coordinates(p1, point) != source_dir:
                point, alt = alt, point
            # point -> sink connector should not be in the direction of sink
            if Utils.get_angle_from_coordinates(point, p2) == sink_dir:
                point, alt = alt, point
            # point -> source connector should not be in the direction of source
            if Utils.get_angle_from_coordinates(point, p1) == source_dir:
                point, alt = alt, point
            # create right-angled connector
            self.lines.append([p0, p1, point, p2, p3])

    def draw(self, widget, cr):
        """
        Draw the connection.
        """
        sink = self.sink_port
        source = self.source_port
        # check for changes
        if self._sink_rot != sink.rotation or self._source_rot != source.rotation:
            self.create_shapes()
            self._sink_rot = sink.rotation
            self._source_rot = source.rotation

        elif self._sink_coor != sink.parent_block.coordinate or self._source_coor != source.parent_block.coordinate:
            self._update_after_move()
            self._sink_coor = sink.parent_block.coordinate
            self._source_coor = source.parent_block.coordinate
        # draw
        color1, color2 = (
            Colors.HIGHLIGHT_COLOR if self.highlighted else
            Colors.CONNECTION_DISABLED_COLOR if not self.get_enabled() else
            color for color in (self._color, self._color2))

        Element.draw(self, widget, cr, color1, Colors.FLOWGRAPH_BACKGROUND_COLOR)

        if color1 != color2:
            cr.save()
            x_pos, y_pos = self.coordinate
            cr.translate(-x_pos, -y_pos)
            cr.set_dash([5.0, 5.0], 5.0)
            Element.draw(self, widget, cr, color2, Colors.FLOWGRAPH_BACKGROUND_COLOR)
            cr.restore()
        # draw arrow on sink port
        cr.set_source_rgb(*self._arrow_color)
        cr.move_to(*self._arrow[0])
        cr.line_to(*self._arrow[1])
        cr.line_to(*self._arrow[2])
        cr.close_path()
        cr.fill()
