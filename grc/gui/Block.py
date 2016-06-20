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
gi.require_version('PangoCairo', '1.0')
from gi.repository import Gtk, Pango, PangoCairo

from . import Actions, Colors, Utils

from .Constants import (
    BLOCK_LABEL_PADDING, PORT_SPACING, PORT_SEPARATION, LABEL_SEPARATION,
    PORT_BORDER_SEPARATION, POSSIBLE_ROTATIONS, BLOCK_FONT, PARAM_FONT
)
from . Element import Element
from ..core.Param import num_to_str
from ..core.utils.complexity import calculate_flowgraph_complexity
from ..core.Block import Block as _Block


class Block(Element, _Block):
    """The graphical signal block."""

    def __init__(self, flow_graph, n):
        """
        Block constructor.
        Add graphics related params to the block.
        """
        _Block.__init__(self, flow_graph, n)

        self.states.update(_coordinate=(0, 0), _rotation=0)
        Element.__init__(self)  # needs the states

        self.W = self.H = 0
        self._param_layouts = []
        self._comment_layout = None
        self._bg_color = Colors.BLOCK_ENABLED_COLOR
        self.has_busses = [False, False]  # source, sink

    def get_coordinate(self):
        """
        Get the coordinate from the position param.

        Returns:
            the coordinate tuple (x, y) or (0, 0) if failure
        """
        return self.states['_coordinate']

    def set_coordinate(self, coor):
        """
        Set the coordinate into the position param.

        Args:
            coor: the coordinate tuple (x, y)
        """
        if Actions.TOGGLE_SNAP_TO_GRID.get_active():
            offset_x, offset_y = (0, self.H/2) if self.is_horizontal() else (self.H/2, 0)
            coor = (
                Utils.align_to_grid(coor[0] + offset_x) - offset_x,
                Utils.align_to_grid(coor[1] + offset_y) - offset_y
            )
        self.states['_coordinate'] = coor

    def get_rotation(self):
        """
        Get the rotation from the position param.

        Returns:
            the rotation in degrees or 0 if failure
        """
        return self.states['_rotation']

    def set_rotation(self, rot):
        """
        Set the rotation into the position param.
q
        Args:
            rot: the rotation in degrees
        """
        self.states['_rotation'] = rot

    def create_shapes(self):
        """Update the block, parameters, and ports when a change occurs."""
        Element.create_shapes(self)
        if self.is_horizontal():
            self.add_area(0, 0, self.W, self.H)
        elif self.is_vertical():
            self.add_area(0, 0, self.H, self.W)

    def create_labels(self):
        """Create the labels for the signal block."""
        Element.create_labels(self)
        self._bg_color = Colors.MISSING_BLOCK_BACKGROUND_COLOR if self.is_dummy_block else \
                         Colors.BLOCK_BYPASSED_COLOR if self.get_bypassed() else \
                         Colors.BLOCK_ENABLED_COLOR if self.get_enabled() else \
                         Colors.BLOCK_DISABLED_COLOR
        del self._param_layouts[:]
        #create the main layout
        layout = Gtk.DrawingArea().create_pango_layout('')
        layout.set_markup('<span foreground="{foreground}" font_desc="{font}"><b>{name}</b></span>'.format(
            foreground='black' if self.is_valid() else 'red', font=BLOCK_FONT, name=Utils.encode(self.name)
        ))
        self.label_width, self.label_height = layout.get_pixel_size()
        self._param_layouts.append(layout)
        #display the params
        if self.is_dummy_block:
            markups = ['<span foreground="black" font_desc="{font}"><b>key: </b>{key}</span>'.format(
                font=PARAM_FONT, key=self.key
            )]
        else:
            markups = [param.format_block_surface_markup()
                       for param in self.params.values() if param.get_hide() not in ('all', 'part')]
        if markups:
            layout = Gtk.DrawingArea().create_pango_layout('')
            layout.set_spacing(LABEL_SEPARATION*Pango.SCALE)
            layout.set_markup('\n'.join(markups))
            self._param_layouts.append(layout)
            w, h = layout.get_pixel_size()
            self.label_width = max(w, self.label_width)
            self.label_height += h + LABEL_SEPARATION

        #calculate width and height needed
        W = self.label_width + 2 * BLOCK_LABEL_PADDING

        def get_min_height_for_ports():
            visible_ports = [p for p in ports if not p.get_hide()]
            min_height = 2*PORT_BORDER_SEPARATION + len(visible_ports) * PORT_SEPARATION
            if visible_ports:
                min_height -= ports[0].H
            return min_height
        H = max(
            [  # labels
                self.label_height + 2 * BLOCK_LABEL_PADDING
            ] +
            [  # ports
                get_min_height_for_ports() for ports in (self.get_sources_gui(), self.get_sinks_gui())
            ] +
            [  # bus ports only
                2 * PORT_BORDER_SEPARATION +
                sum([port.H + PORT_SPACING for port in ports if port.get_type() == 'bus']) - PORT_SPACING
                for ports in (self.get_sources_gui(), self.get_sinks_gui())
            ]
        )
        self.W, self.H = Utils.align_to_grid((W, H))
        self.has_busses = [
            any(port.get_type() == 'bus' for port in ports)
            for ports in (self.get_sources_gui(), self.get_sinks_gui())
        ]
        self.create_comment_layout()

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
        # draw main block
        bg_color = self._bg_color
        border_color = (
            Colors.HIGHLIGHT_COLOR if self.is_highlighted() else
            Colors.MISSING_BLOCK_BORDER_COLOR if self.is_dummy_block else
            Colors.BORDER_COLOR
        )

        for port in self.get_ports_gui():
            cr.save()
            port.draw(widget, cr, border_color, bg_color)
            cr.restore()
        Element.draw(self, widget, cr, border_color, bg_color)

        # create the param label
        width = self.label_width
        cr.set_source_rgb(*self._bg_color)
        if self.is_horizontal():
            cr.translate(BLOCK_LABEL_PADDING, (self.H - self.label_height) / 2)
        elif self.is_vertical():
            cr.translate((self.H - self.label_height) / 2, BLOCK_LABEL_PADDING)
            cr.rotate(-90 * math.pi / 180.)
            cr.translate(-width, 0)

        # draw the layouts
        h_off = 0
        for i, layout in enumerate(self._param_layouts):
            w, h = layout.get_pixel_size()
            if i == 0:
                w_off = (width - w) / 2
            else:
                w_off = 0
            cr.translate(w_off, h_off)
            PangoCairo.update_layout(cr, layout)
            PangoCairo.show_layout(cr, layout)
            cr.translate(-w_off, -h_off)
            h_off += h + LABEL_SEPARATION

    def what_is_selected(self, coor, coor_m=None):
        """
        Get the element that is selected.

        Args:
            coor: the (x,y) tuple
            coor_m: the (x_m, y_m) tuple

        Returns:
            this block, a port, or None
        """
        for port in self.get_ports_gui():
            port_selected = port.what_is_selected(coor, coor_m)
            if port_selected: return port_selected
        return Element.what_is_selected(self, coor, coor_m)

    def draw_comment(self, widget, cr):
        if not self._comment_layout:
            return
        x, y = self.get_coordinate()

        if self.is_horizontal():
            y += self.H + BLOCK_LABEL_PADDING
        else:
            x += self.H + BLOCK_LABEL_PADDING

        cr.save()
        cr.translate(x, y)
        PangoCairo.update_layout(cr, self._comment_layout)
        PangoCairo.show_layout(cr, self._comment_layout)
        cr.restore()
