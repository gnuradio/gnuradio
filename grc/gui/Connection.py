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

import gtk

import Colors
import Utils
from Constants import CONNECTOR_ARROW_BASE, CONNECTOR_ARROW_HEIGHT
from Element import Element

from ..core.Constants import GR_MESSAGE_DOMAIN
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
        # can't use Colors.CONNECTION_ENABLED_COLOR here, might not be defined (grcc)
        self._bg_color = self._arrow_color = self._color = None

    def get_coordinate(self):
        """
        Get the 0,0 coordinate.
        Coordinates are irrelevant in connection.

        Returns:
            0, 0
        """
        return 0, 0

    def get_rotation(self):
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
            connector_length = self.get_source().get_connector_length()
        except:
            return
        self.x1, self.y1 = Utils.get_rotated_coordinate((connector_length, 0), self.get_source().get_rotation())
        #get the sink coordinate
        connector_length = self.get_sink().get_connector_length() + CONNECTOR_ARROW_HEIGHT
        self.x2, self.y2 = Utils.get_rotated_coordinate((-connector_length, 0), self.get_sink().get_rotation())
        #build the arrow
        self.arrow = [(0, 0),
            Utils.get_rotated_coordinate((-CONNECTOR_ARROW_HEIGHT, -CONNECTOR_ARROW_BASE/2), self.get_sink().get_rotation()),
            Utils.get_rotated_coordinate((-CONNECTOR_ARROW_HEIGHT, CONNECTOR_ARROW_BASE/2), self.get_sink().get_rotation()),
        ]
        source_domain = self.get_source().get_domain()
        sink_domain = self.get_sink().get_domain()
        self.line_attributes[0] = 2 if source_domain != sink_domain else 0
        self.line_attributes[1] = gtk.gdk.LINE_DOUBLE_DASH \
            if not source_domain == sink_domain == GR_MESSAGE_DOMAIN \
            else gtk.gdk.LINE_ON_OFF_DASH
        get_domain_color = lambda d: Colors.get_color((
            self.get_parent().get_parent().domains.get(d, {})
        ).get('color') or Colors.DEFAULT_DOMAIN_COLOR_CODE)
        self._color = get_domain_color(source_domain)
        self._bg_color = get_domain_color(sink_domain)
        self._arrow_color = self._bg_color if self.is_valid() else Colors.CONNECTION_ERROR_COLOR
        self._update_after_move()

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

        Args:
            gc: the graphics context
            window: the gtk window to draw on
        """
        sink = self.get_sink()
        source = self.get_source()
        #check for changes
        if self._sink_rot != sink.get_rotation() or self._source_rot != source.get_rotation(): self.create_shapes()
        elif self._sink_coor != sink.get_coordinate() or self._source_coor != source.get_coordinate():
            try:
                self._update_after_move()
            except:
                return
        #cache values
        self._sink_rot = sink.get_rotation()
        self._source_rot = source.get_rotation()
        self._sink_coor = sink.get_coordinate()
        self._source_coor = source.get_coordinate()
        #draw
        mod_color = lambda color: (
            Colors.HIGHLIGHT_COLOR if self.is_highlighted() else
            Colors.CONNECTION_DISABLED_COLOR if not self.get_enabled() else
            color
        )
        Element.draw(self, gc, window, mod_color(self._color), mod_color(self._bg_color))
        # draw arrow on sink port
        try:
            gc.set_foreground(mod_color(self._arrow_color))
            gc.set_line_attributes(0, gtk.gdk.LINE_SOLID, gtk.gdk.CAP_BUTT, gtk.gdk.JOIN_MITER)
            window.draw_polygon(gc, True, self._arrow)
        except:
            pass
