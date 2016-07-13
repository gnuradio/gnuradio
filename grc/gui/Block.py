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

from gi.repository import Gtk, Pango, PangoCairo

from . import Actions, Colors, Utils, Constants

from .Constants import (
    BLOCK_LABEL_PADDING, PORT_SPACING, PORT_SEPARATION, LABEL_SEPARATION,
    PORT_BORDER_SEPARATION, BLOCK_FONT, PARAM_FONT
)
from . Element import Element
from ..core.Param import num_to_str
from ..core.utils.complexity import calculate_flowgraph_complexity
from ..core.Block import Block as _Block


class Block(_Block, Element):
    """The graphical signal block."""

    def __init__(self, flow_graph, n):
        """
        Block constructor.
        Add graphics related params to the block.
        """
        _Block.__init__(self, flow_graph, n)

        self.states.update(_coordinate=(0, 0), _rotation=0)
        self.width = self.height = 0
        Element.__init__(self)  # needs the states and initial sizes

        self._surface_layouts = [
            Gtk.DrawingArea().create_pango_layout(''),  # title
            Gtk.DrawingArea().create_pango_layout(''),  # params
        ]
        self._surface_layout_offsets = 0, 0
        self._comment_layout = None

        self._bg_color = Colors.BLOCK_ENABLED_COLOR
        self.has_busses = [False, False]  # source, sink

    @property
    def coordinate(self):
        """
        Get the coordinate from the position param.

        Returns:
            the coordinate tuple (x, y) or (0, 0) if failure
        """
        return self.states['_coordinate']

    @coordinate.setter
    def coordinate(self, coor):
        """
        Set the coordinate into the position param.

        Args:
            coor: the coordinate tuple (x, y)
        """
        if Actions.TOGGLE_SNAP_TO_GRID.get_active():
            offset_x, offset_y = (0, self.height / 2) if self.is_horizontal() else (self.height / 2, 0)
            coor = (
                Utils.align_to_grid(coor[0] + offset_x) - offset_x,
                Utils.align_to_grid(coor[1] + offset_y) - offset_y
            )
        self.states['_coordinate'] = coor

    @property
    def rotation(self):
        """
        Get the rotation from the position param.

        Returns:
            the rotation in degrees or 0 if failure
        """
        return self.states['_rotation']

    @rotation.setter
    def rotation(self, rot):
        """
        Set the rotation into the position param.

        Args:
            rot: the rotation in degrees
        """
        self.states['_rotation'] = rot

    def create_shapes(self):
        """Update the block, parameters, and ports when a change occurs."""
        self.clear()

        if self.is_horizontal():
            self.areas.append([0, 0, self.width, self.height])
        elif self.is_vertical():
            self.areas.append([0, 0, self.height, self.width])

        for ports, has_busses in zip((self.active_sources, self.active_sinks), self.has_busses):
            if not ports:
                continue
            port_separation = PORT_SEPARATION if not has_busses else ports[0].height + PORT_SPACING
            offset = (self.height - (len(ports) - 1) * port_separation - ports[0].height) / 2
            for index, port in enumerate(ports):
                port.create_shapes()

                port.coordinate = {
                    0: (+self.width, offset),
                    90: (offset, -port.width),
                    180: (-port.width, offset),
                    270: (offset, +self.width),
                }[port.get_connector_direction()]
                offset += PORT_SEPARATION if not has_busses else port.height + PORT_SPACING

                port.connector_length = Constants.CONNECTOR_EXTENSION_MINIMAL + \
                                        Constants.CONNECTOR_EXTENSION_INCREMENT * index

    def create_labels(self):
        """Create the labels for the signal block."""
        self._bg_color = Colors.MISSING_BLOCK_BACKGROUND_COLOR if self.is_dummy_block else \
                         Colors.BLOCK_BYPASSED_COLOR if self.get_bypassed() else \
                         Colors.BLOCK_ENABLED_COLOR if self.get_enabled() else \
                         Colors.BLOCK_DISABLED_COLOR

        # update the title layout
        title_layout, params_layout = self._surface_layouts

        title_layout.set_markup(
            '<span foreground="{foreground}" font_desc="{font}"><b>{name}</b></span>'.format(
                foreground='black' if self.is_valid() else 'red', font=BLOCK_FONT,
                name=Utils.encode(self.name)
            )
        )
        title_width, title_height = title_layout.get_pixel_size()

        # update the params layout
        if not self.is_dummy_block:
            markups = [param.format_block_surface_markup()
                       for param in self.params.values() if param.get_hide() not in ('all', 'part')]
        else:
            markups = ['<span foreground="black" font_desc="{font}"><b>key: </b>{key}</span>'.format(
                font=PARAM_FONT, key=self.key
            )]

        params_layout.set_spacing(LABEL_SEPARATION * Pango.SCALE)
        params_layout.set_markup('\n'.join(markups))
        params_width, params_height = params_layout.get_pixel_size() if markups else (0, 0)

        label_width = max(title_width, params_width)
        label_height = title_height + LABEL_SEPARATION + params_height

        title_layout.set_width(label_width * Pango.SCALE)
        title_layout.set_alignment(Pango.Alignment.CENTER)

        # calculate width and height needed
        width = label_width + 2 * BLOCK_LABEL_PADDING
        height = label_height + 2 * BLOCK_LABEL_PADDING

        self.create_port_labels()

        def get_min_height_for_ports():
            min_height = 2 * PORT_BORDER_SEPARATION + len(ports) * PORT_SEPARATION
            if ports:
                min_height -= ports[-1].height
            return min_height
        height = max(
            [  # labels
                height
            ] +
            [  # ports
                get_min_height_for_ports() for ports in (self.active_sources, self.active_sinks)
            ] +
            [  # bus ports only
                2 * PORT_BORDER_SEPARATION +
                sum([port.height + PORT_SPACING for port in ports if port.get_type() == 'bus']) - PORT_SPACING
                for ports in (self.get_sources_gui(), self.get_sinks_gui())
            ]
        )
        self.width, self.height = width, height = Utils.align_to_grid((width, height))

        self._surface_layout_offsets = [
            (width - label_width) / 2.0,
            (height - label_height) / 2.0
        ]

        self.has_busses = [
            any(port.get_type() == 'bus' for port in ports)
            for ports in (self.get_sources_gui(), self.get_sinks_gui())
        ]
        self.create_comment_layout()

    def create_port_labels(self):
        for ports in (self.active_sinks, self.active_sources):
            max_width = 0
            for port in ports:
                port.create_labels()
                max_width = max(max_width, port.width)
            for port in ports:
                port.width = max_width

    def create_comment_layout(self):
        markups = []

        # Show the flow graph complexity on the top block if enabled
        if Actions.TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY.get_active() and self.key == "options":
            complexity = calculate_flowgraph_complexity(self.parent)
            markups.append(
                '<span foreground="#444" size="medium" font_desc="{font}">'
                '<b>Complexity: {num}bal</b></span>'.format(num=num_to_str(complexity), font=BLOCK_FONT)
            )
        comment = self.get_comment()  # Returns None if there are no comments
        if comment:
            if markups:
                markups.append('<span></span>')

            markups.append('<span foreground="{foreground}" font_desc="{font}">{comment}</span>'.format(
                foreground='#444' if self.get_enabled() else '#888', font=BLOCK_FONT, comment=Utils.encode(comment)
            ))
        if markups:
            layout = self._comment_layout = Gtk.DrawingArea().create_pango_layout('')
            layout.set_markup(''.join(markups))
        else:
            self._comment_layout = None

    def draw(self, widget, cr):
        """
        Draw the signal block with label and inputs/outputs.
        """
        bg_color = self._bg_color
        border_color = (
            Colors.HIGHLIGHT_COLOR if self.highlighted else
            Colors.MISSING_BLOCK_BORDER_COLOR if self.is_dummy_block else
            Colors.BORDER_COLOR
        )
        # draw main block
        Element.draw(self, widget, cr, border_color, bg_color)
        for port in self.active_ports():
            cr.save()
            port.draw(widget, cr, border_color, bg_color)
            cr.restore()

        # title and params label
        if self.is_vertical():
            cr.rotate(-math.pi / 2)
            cr.translate(-self.width, 0)
        cr.translate(*self._surface_layout_offsets)

        for layout in self._surface_layouts:
            PangoCairo.update_layout(cr, layout)
            PangoCairo.show_layout(cr, layout)
            _, h = layout.get_pixel_size()
            cr.translate(0, h + LABEL_SEPARATION)

    def what_is_selected(self, coor, coor_m=None):
        """
        Get the element that is selected.

        Args:
            coor: the (x,y) tuple
            coor_m: the (x_m, y_m) tuple

        Returns:
            this block, a port, or None
        """
        for port in self.active_ports():
            port_selected = port.what_is_selected(
                coor=[a - b for a, b in zip(coor, self.coordinate)],
                coor_m=[a - b for a, b in zip(coor, self.coordinate)] if coor_m is not None else None
            )
            if port_selected:
                return port_selected
        return Element.what_is_selected(self, coor, coor_m)

    def draw_comment(self, widget, cr):
        if not self._comment_layout:
            return
        x, y = self.coordinate

        if self.is_horizontal():
            y += self.height + BLOCK_LABEL_PADDING
        else:
            x += self.height + BLOCK_LABEL_PADDING

        cr.save()
        cr.translate(x, y)
        PangoCairo.update_layout(cr, self._comment_layout)
        PangoCairo.show_layout(cr, self._comment_layout)
        cr.restore()
