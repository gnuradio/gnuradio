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

from __future__ import absolute_import, division

from math import pi

from . import colors
from .drawable import Drawable
from .. import Utils
from ..Constants import (
    CONNECTOR_ARROW_BASE,
    CONNECTOR_ARROW_HEIGHT,
    GR_MESSAGE_DOMAIN,
    LINE_SELECT_SENSITIVITY,
)
from ...core.Connection import Connection as CoreConnection
from ...core.Element import nop_write


class Connection(CoreConnection, Drawable):
    """
    A graphical connection for ports.
    The connection has 2 parts, the arrow and the wire.
    The coloring of the arrow and wire exposes the status of 3 states:
        enabled/disabled, valid/invalid, highlighted/non-highlighted.
    The wire coloring exposes the enabled and highlighted states.
    The arrow coloring exposes the enabled and valid states.
    """

    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)
        Drawable.__init__(self)

        self._line = []
        self._line_width_factor = 1.0
        self._color = self._color2 = self._arrow_color = None

        self._current_port_rotations = self._current_coordinates = None

        self._rel_points = None  # connection coordinates relative to sink/source
        self._arrow_rotation = 0.0  # rotation of the arrow in radians
        self._current_cr = None  # for what_is_selected() of curved line
        self._line_path = None

    @nop_write
    @property
    def coordinate(self):
        return self.source_port.connector_coordinate_absolute

    @nop_write
    @property
    def rotation(self):
        """
        Get the 0 degree rotation.
        Rotations are irrelevant in connection.

        Returns:
            0
        """
        return 0

    def create_shapes(self):
        """Pre-calculate relative coordinates."""
        source = self.source_port
        sink = self.sink_port
        rotate = Utils.get_rotated_coordinate

        # first two components relative to source connector, rest relative to sink connector
        self._rel_points = [
            rotate((15, 0), source.rotation),  # line from 0,0 to here, bezier curve start
            rotate((50, 0), source.rotation),  # bezier curve control point 1
            rotate((-50, 0), sink.rotation),  # bezier curve control point 2
            rotate((-15, 0), sink.rotation),  # bezier curve end
            rotate((-CONNECTOR_ARROW_HEIGHT, 0), sink.rotation),  # line to arrow head
        ]
        self._current_coordinates = None  # triggers _make_path()

        def get_domain_color(domain_name):
            domain = self.parent_platform.domains.get(domain_name, {})
            color_spec = domain.get('color')
            return colors.get_color(color_spec) if color_spec else colors.DEFAULT_DOMAIN_COLOR

        if source.domain == GR_MESSAGE_DOMAIN:
            self._line_width_factor = 1.0
            self._color = None
            self._color2 = colors.CONNECTION_ENABLED_COLOR
        else:
            if source.domain != sink.domain:
                self._line_width_factor = 2.0
            self._color = get_domain_color(source.domain)
            self._color2 = get_domain_color(sink.domain)

        self._arrow_color = self._color2 if self.is_valid() else colors.CONNECTION_ERROR_COLOR
        self._arrow_rotation = -sink.rotation / 180 * pi

        if not self._bounding_points:
            self._make_path()  # no cr set --> only sets bounding_points for extent

    def _make_path(self, cr=None):
        x_pos, y_pos = self.coordinate  # is source connector coordinate
        # x_start, y_start = self.source_port.get_connector_coordinate()
        x_end, y_end = self.sink_port.connector_coordinate_absolute

        # sink connector relative to sink connector
        x_e, y_e = x_end - x_pos, y_end - y_pos

        # make rel_point all relative to source connector
        p0 = 0, 0  # x_start - x_pos, y_start - y_pos
        p1, p2, (dx_e1, dy_e1), (dx_e2, dy_e2), (dx_e3, dy_e3) = self._rel_points
        p3 = x_e + dx_e1, y_e + dy_e1
        p4 = x_e + dx_e2, y_e + dy_e2
        p5 = x_e + dx_e3, y_e + dy_e3
        self._bounding_points = p0, p1, p4, p5  # ignores curved part =(

        if cr:
            cr.move_to(*p0)
            cr.line_to(*p1)
            cr.curve_to(*(p2 + p3 + p4))
            cr.line_to(*p5)
            self._line_path = cr.copy_path()

    def draw(self, cr):
        """
        Draw the connection.
        """
        self._current_cr = cr
        sink = self.sink_port
        source = self.source_port

        # check for changes
        port_rotations = (source.rotation, sink.rotation)
        if self._current_port_rotations != port_rotations:
            self.create_shapes()  # triggers _make_path() call below
            self._current_port_rotations = port_rotations

        new_coordinates = (source.parent_block.coordinate, sink.parent_block.coordinate)
        if self._current_coordinates != new_coordinates:
            self._make_path(cr)
            self._current_coordinates = new_coordinates

        color1, color2, arrow_color = (
            None if color is None else
            colors.HIGHLIGHT_COLOR if self.highlighted else
            colors.CONNECTION_DISABLED_COLOR if not self.enabled else color
            for color in (self._color, self._color2, self._arrow_color)
        )

        cr.translate(*self.coordinate)
        cr.set_line_width(self._line_width_factor * cr.get_line_width())
        cr.new_path()
        cr.append_path(self._line_path)

        arrow_pos = cr.get_current_point()

        if color1:  # not a message connection
            cr.set_source_rgba(*color1)
            cr.stroke_preserve()

        if color1 != color2:
            cr.save()
            cr.set_dash([5.0, 5.0], 5.0 if color1 else 0.0)
            cr.set_source_rgba(*color2)
            cr.stroke()
            cr.restore()
        else:
            cr.new_path()

        cr.move_to(*arrow_pos)
        cr.set_source_rgba(*arrow_color)
        cr.rotate(self._arrow_rotation)
        cr.rel_move_to(CONNECTOR_ARROW_HEIGHT, 0)
        cr.rel_line_to(-CONNECTOR_ARROW_HEIGHT, -CONNECTOR_ARROW_BASE/2)
        cr.rel_line_to(0, CONNECTOR_ARROW_BASE)
        cr.close_path()
        cr.fill()

    def what_is_selected(self, coor, coor_m=None):
        """
        Returns:
            self if one of the areas/lines encompasses coor, else None.
        """
        if coor_m:
            return Drawable.what_is_selected(self, coor, coor_m)

        x, y = [a - b for a, b in zip(coor, self.coordinate)]

        cr = self._current_cr
        cr.save()
        cr.new_path()
        cr.append_path(self._line_path)
        cr.set_line_width(cr.get_line_width() * LINE_SELECT_SENSITIVITY)
        hit = cr.in_stroke(x, y)
        cr.restore()

        if hit:
            return self
