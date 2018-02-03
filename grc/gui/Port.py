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

import pygtk
pygtk.require('2.0')
import gtk

from . import Actions, Colors, Utils
from .Constants import (
    PORT_SEPARATION, PORT_SPACING, CONNECTOR_EXTENSION_MINIMAL,
    CONNECTOR_EXTENSION_INCREMENT, PORT_LABEL_PADDING, PORT_MIN_WIDTH, PORT_LABEL_HIDDEN_WIDTH, PORT_FONT
)
from .Element import Element
from ..core.Constants import DEFAULT_DOMAIN, GR_MESSAGE_DOMAIN

from ..core.Port import Port as _Port

PORT_MARKUP_TMPL="""\
<span foreground="black" font_desc="$font">$encode($port.get_name())</span>"""


class Port(_Port, Element):
    """The graphical port."""

    def __init__(self, block, n, dir):
        """
        Port constructor.
        Create list of connector coordinates.
        """
        _Port.__init__(self, block, n, dir)
        Element.__init__(self)
        self.W = self.H = self.w = self.h = 0
        self._connector_coordinate = (0, 0)
        self._connector_length = 0
        self._hovering = True
        self._force_label_unhidden = False

    def create_shapes(self):
        """Create new areas and labels for the port."""
        Element.create_shapes(self)
        if self.get_hide():
            return  # this port is hidden, no need to create shapes
        if self.get_domain() == GR_MESSAGE_DOMAIN:
            pass
        elif self.get_domain() != DEFAULT_DOMAIN:
            self.line_attributes[0] = 2
        #get current rotation
        rotation = self.get_rotation()
        #get all sibling ports
        ports = self.get_parent().get_sources_gui() \
            if self.is_source else self.get_parent().get_sinks_gui()
        ports = filter(lambda p: not p.get_hide(), ports)
        #get the max width
        self.W = max([port.W for port in ports] + [PORT_MIN_WIDTH])
        W = self.W if not self._label_hidden() else PORT_LABEL_HIDDEN_WIDTH
        #get a numeric index for this port relative to its sibling ports
        try:
            index = ports.index(self)
        except:
            if hasattr(self, '_connector_length'):
                del self._connector_length
            return
        length = len(filter(lambda p: not p.get_hide(), ports))
        #reverse the order of ports for these rotations
        if rotation in (180, 270):
            index = length-index-1

        port_separation = PORT_SEPARATION \
            if not self.get_parent().has_busses[self.is_source] \
            else max([port.H for port in ports]) + PORT_SPACING

        offset = (self.get_parent().H - (length-1)*port_separation - self.H)/2
        #create areas and connector coordinates
        if (self.is_sink and rotation == 0) or (self.is_source and rotation == 180):
            x = -W
            y = port_separation*index+offset
            self.add_area((x, y), (W, self.H))
            self._connector_coordinate = (x-1, y+self.H/2)
        elif (self.is_source and rotation == 0) or (self.is_sink and rotation == 180):
            x = self.get_parent().W
            y = port_separation*index+offset
            self.add_area((x, y), (W, self.H))
            self._connector_coordinate = (x+1+W, y+self.H/2)
        elif (self.is_source and rotation == 90) or (self.is_sink and rotation == 270):
            y = -W
            x = port_separation*index+offset
            self.add_area((x, y), (self.H, W))
            self._connector_coordinate = (x+self.H/2, y-1)
        elif (self.is_sink and rotation == 90) or (self.is_source and rotation == 270):
            y = self.get_parent().W
            x = port_separation*index+offset
            self.add_area((x, y), (self.H, W))
            self._connector_coordinate = (x+self.H/2, y+1+W)
        #the connector length
        self._connector_length = CONNECTOR_EXTENSION_MINIMAL + CONNECTOR_EXTENSION_INCREMENT*index

    def create_labels(self):
        """Create the labels for the socket."""
        Element.create_labels(self)
        self._bg_color = Colors.get_color(self.get_color())
        # create the layout
        layout = gtk.DrawingArea().create_pango_layout('')
        layout.set_markup(Utils.parse_template(PORT_MARKUP_TMPL, port=self, font=PORT_FONT))
        self.w, self.h = layout.get_pixel_size()
        self.W = 2 * PORT_LABEL_PADDING + self.w
        self.H = 2 * PORT_LABEL_PADDING + self.h * (
            3 if self.get_type() == 'bus' else 1)
        self.H += self.H % 2
        # create the pixmap
        pixmap = self.get_parent().get_parent().new_pixmap(self.w, self.h)
        gc = pixmap.new_gc()
        gc.set_foreground(self._bg_color)
        pixmap.draw_rectangle(gc, True, 0, 0, self.w, self.h)
        pixmap.draw_layout(gc, 0, 0, layout)
        # create vertical and horizontal pixmaps
        self.horizontal_label = pixmap
        if self.is_vertical():
            self.vertical_label = self.get_parent().get_parent().new_pixmap(self.h, self.w)
            Utils.rotate_pixmap(gc, self.horizontal_label, self.vertical_label)

    def draw(self, gc, window):
        """
        Draw the socket with a label.

        Args:
            gc: the graphics context
            window: the gtk window to draw on
        """
        Element.draw(
            self, gc, window, bg_color=self._bg_color,
            border_color=self.is_highlighted() and Colors.HIGHLIGHT_COLOR or
                         self.get_parent().is_dummy_block and Colors.MISSING_BLOCK_BORDER_COLOR or
                         Colors.BORDER_COLOR,
        )
        if not self._areas_list or self._label_hidden():
            return  # this port is either hidden (no areas) or folded (no label)
        X, Y = self.get_coordinate()
        (x, y), (w, h) = self._areas_list[0]  # use the first area's sizes to place the labels
        if self.is_horizontal():
            window.draw_drawable(gc, self.horizontal_label, 0, 0, x+X+(self.W-self.w)/2, y+Y+(self.H-self.h)/2, -1, -1)
        elif self.is_vertical():
            window.draw_drawable(gc, self.vertical_label, 0, 0, x+X+(self.H-self.h)/2, y+Y+(self.W-self.w)/2, -1, -1)

    def get_connector_coordinate(self):
        """
        Get the coordinate where connections may attach to.

        Returns:
            the connector coordinate (x, y) tuple
        """
        x, y = self._connector_coordinate
        X, Y = self.get_coordinate()
        return (x + X, y + Y)

    def get_connector_direction(self):
        """
        Get the direction that the socket points: 0,90,180,270.
        This is the rotation degree if the socket is an output or
        the rotation degree + 180 if the socket is an input.

        Returns:
            the direction in degrees
        """
        if self.is_source: return self.get_rotation()
        elif self.is_sink: return (self.get_rotation() + 180)%360

    def get_connector_length(self):
        """
        Get the length of the connector.
        The connector length increases as the port index changes.

        Returns:
            the length in pixels
        """
        return self._connector_length

    def get_rotation(self):
        """
        Get the parent's rotation rather than self.

        Returns:
            the parent's rotation
        """
        return self.get_parent().get_rotation()

    def move(self, delta_coor):
        """
        Move the parent rather than self.

        Args:
            delta_corr: the (delta_x, delta_y) tuple
        """
        self.get_parent().move(delta_coor)

    def rotate(self, direction):
        """
        Rotate the parent rather than self.

        Args:
            direction: degrees to rotate
        """
        self.get_parent().rotate(direction)

    def get_coordinate(self):
        """
        Get the parent's coordinate rather than self.

        Returns:
            the parents coordinate
        """
        return self.get_parent().get_coordinate()

    def set_highlighted(self, highlight):
        """
        Set the parent highlight rather than self.

        Args:
            highlight: true to enable highlighting
        """
        self.get_parent().set_highlighted(highlight)

    def is_highlighted(self):
        """
        Get the parent's is highlight rather than self.

        Returns:
            the parent's highlighting status
        """
        return self.get_parent().is_highlighted()

    def _label_hidden(self):
        """
        Figure out if the label should be hidden

        Returns:
            true if the label should not be shown
        """
        return self._hovering and not self._force_label_unhidden and Actions.TOGGLE_AUTO_HIDE_PORT_LABELS.get_active()

    def force_label_unhidden(self, enable=True):
        """
        Disable showing the label on mouse-over for this port

        Args:
            enable: true to override the mouse-over behaviour
        """
        self._force_label_unhidden = enable

    def mouse_over(self):
        """
        Called from flow graph on mouse-over
        """
        self._hovering = False
        return Actions.TOGGLE_AUTO_HIDE_PORT_LABELS.get_active()  # only redraw if necessary

    def mouse_out(self):
        """
        Called from flow graph on mouse-out
        """
        self._hovering = True
        return Actions.TOGGLE_AUTO_HIDE_PORT_LABELS.get_active()  # only redraw if necessary
