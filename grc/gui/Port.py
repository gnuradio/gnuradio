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
import math
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, PangoCairo

from . import Actions, Colors, Utils, Constants
from .Element import Element

from ..core.Constants import DEFAULT_DOMAIN, GR_MESSAGE_DOMAIN
from ..core.Port import Port as _Port


class Port(_Port, Element):
    """The graphical port."""

    def __init__(self, block, n, dir):
        """
        Port contructor.
        Create list of connector coordinates.
        """
        _Port.__init__(self, block, n, dir)
        Element.__init__(self)
        self._connector_coordinate = (0, 0)
        self._hovering = True
        self._force_label_unhidden = False
        self._bg_color = (0, 0, 0)

        self.W = self.w = self.h = 0
        self.H = 20  # todo: fix
        self.connector_length = 0

        self.layout = Gtk.DrawingArea().create_pango_layout('')

    def _get_color(self):
        """
        Get the color that represents this port's type.
        Codes differ for ports where the vec length is 1 or greater than 1.

        Returns:
            a hex color code.
        """
        color = Colors.PORT_TYPE_TO_COLOR[self.get_type()]
        vlen = self.get_vlen()
        if vlen > 1:
            dark = (0, 0, 30 / 255.0, 50 / 255.0, 70 / 255.0)[min(4, vlen)]
            color = tuple(max(c - dark, 0) for c in color)
        return color

    def create_shapes(self):
        """Create new areas and labels for the port."""
        Element.create_shapes(self)
        self._bg_color = self._get_color()

        if self.get_hide():
            return  # this port is hidden, no need to create shapes
        if self.get_domain() == Constants.GR_MESSAGE_DOMAIN:
            pass
        elif self.get_domain() != Constants.DEFAULT_DOMAIN:
            self.line_attributes[0] = 2
        #get current rotation
        rotation = self.get_rotation()
        #get all sibling ports
        ports = self.parent.get_sources_gui() \
            if self.is_source else self.parent.get_sinks_gui()
        ports = [p for p in ports if not p.get_hide()]
        #get the max width
        self.W = max([port.W for port in ports] + [Constants.PORT_MIN_WIDTH])
        W = self.W if not self._label_hidden() else Constants.PORT_LABEL_HIDDEN_WIDTH
        #get a numeric index for this port relative to its sibling ports
        try:
            index = ports.index(self)
        except:
            if hasattr(self, '_connector_length'):
                del self.connector_length
            return
        #reverse the order of ports for these rotations
        if rotation in (180, 270):
            index = len(ports)-index-1

        port_separation = Constants.PORT_SEPARATION \
            if not self.parent.has_busses[self.is_source] \
            else max([port.H for port in ports]) + Constants.PORT_SPACING

        offset = (self.parent.H - (len(ports)-1)*port_separation - self.H)/2
        #create areas and connector coordinates
        if (self.is_sink and rotation == 0) or (self.is_source and rotation == 180):
            x = -W
            y = port_separation*index+offset
            self.add_area(x, y, W, self.H)
            self._connector_coordinate = (x-1, y+self.H/2)
        elif (self.is_source and rotation == 0) or (self.is_sink and rotation == 180):
            x = self.parent.W
            y = port_separation*index+offset
            self.add_area(x, y, W, self.H)
            self._connector_coordinate = (x+1+W, y+self.H/2)
        elif (self.is_source and rotation == 90) or (self.is_sink and rotation == 270):
            y = -W
            x = port_separation*index+offset
            self.add_area(x, y, self.H, W)
            self._connector_coordinate = (x+self.H/2, y-1)
        elif (self.is_sink and rotation == 90) or (self.is_source and rotation == 270):
            y = self.parent.W
            x = port_separation*index+offset
            self.add_area(x, y, self.H, W)
            self._connector_coordinate = (x+self.H/2, y+1+W)
        #the connector length
        self.connector_length = Constants.CONNECTOR_EXTENSION_MINIMAL + Constants.CONNECTOR_EXTENSION_INCREMENT * index

    def create_labels(self):
        """Create the labels for the socket."""
        self.layout.set_markup("""<span foreground="black" font_desc="{font}">{name}</span>""".format(
            name=Utils.encode(self.get_name()), font=Constants.PORT_FONT
        ))

    def draw(self, widget, cr, border_color, bg_color):
        """
        Draw the socket with a label.
        """
        Element.draw(self, widget, cr, border_color, self._bg_color)

        if not self._areas_list or self._label_hidden():
            return  # this port is either hidden (no areas) or folded (no label)

        x, y, _, __ = self._areas_list[0]

        cr.set_source_rgb(*self._bg_color)
        if self.is_horizontal():
            cr.translate(x + (self.W - self.w) / 2, y + (self.H - self.h) / 2)
        elif self.is_vertical():
            cr.translate(x + (self.H - self.h) / 2, y + (self.W - self.w) / 2)
            cr.rotate(-90 * math.pi / 180.)
            cr.translate(-self.w, 0)
        PangoCairo.update_layout(cr, self.layout)
        PangoCairo.show_layout(cr, self.layout)

    def get_connector_coordinate(self):
        """
        Get the coordinate where connections may attach to.

        Returns:
            the connector coordinate (x, y) tuple
        """
        x, y = self._connector_coordinate
        x_pos, y_pos = self.get_coordinate()
        return x + x_pos, y + y_pos

    def get_connector_direction(self):
        """
        Get the direction that the socket points: 0,90,180,270.
        This is the rotation degree if the socket is an output or
        the rotation degree + 180 if the socket is an input.

        Returns:
            the direction in degrees
        """
        if self.is_source:
            return self.get_rotation()
        elif self.is_sink:
            return (self.get_rotation() + 180) % 360

    def get_rotation(self):
        """
        Get the parent's rotation rather than self.

        Returns:
            the parent's rotation
        """
        return self.parent.get_rotation()

    def move(self, delta_coor):
        """
        Move the parent rather than self.

        Args:
            delta_corr: the (delta_x, delta_y) tuple
        """
        self.parent.move(delta_coor)

    def rotate(self, direction):
        """
        Rotate the parent rather than self.

        Args:
            direction: degrees to rotate
        """
        self.parent.rotate(direction)

    def get_coordinate(self):
        """
        Get the parent's coordinate rather than self.

        Returns:
            the parents coordinate
        """
        return self.parent.get_coordinate()

    def set_highlighted(self, highlight):
        """
        Set the parent highlight rather than self.

        Args:
            highlight: true to enable highlighting
        """
        self.parent.set_highlighted(highlight)

    def is_highlighted(self):
        """
        Get the parent's is highlight rather than self.

        Returns:
            the parent's highlighting status
        """
        return self.parent.is_highlighted()

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
