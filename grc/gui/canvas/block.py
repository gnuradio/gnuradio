"""
Copyright 2007, 2008, 2009 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""

from .drawable import Drawable
from .. import Actions, Utils
from ..Theme import Theme, State, get_hexcolor
from ..Constants import (
    PORT_SPACING, PORT_SEPARATION, LABEL_SEPARATION,
    PORT_BORDER_SEPARATION
)
from ...core import utils
from ...core.blocks import Block as CoreBlock

import math

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('PangoCairo', '1.0')
from gi.repository import Gtk, Pango, PangoCairo


class Block(CoreBlock, Drawable):
    """The graphical signal block."""

    def __init__(self, parent, **n):
        """
        Block constructor.
        Add graphics related params to the block.
        """
        super(self.__class__, self).__init__(parent, **n)

        self.states.update(coordinate=(0, 0), rotation=0)
        self.width = self.height = 0
        Drawable.__init__(self)  # needs the states and initial sizes

        self._surface_layouts = [
            None,  # title
            None,  # params
        ]
        self._surface_layouts_offsets = 0, 0
        self._comment_layout = None

        self._area = []

        def theme(prop, *args, **kwargs):
            return Theme.get_property(prop, self, *args, **kwargs)

        self._theme = theme
        self._update_theming()

    @property
    def coordinate(self):
        """
        Get the coordinate from the position param.

        Returns:
            the coordinate tuple (x, y) or (0, 0) if failure
        """
        return Utils.scale(self.states['coordinate'])

    @coordinate.setter
    def coordinate(self, coor):
        """
        Set the coordinate into the position param.

        Args:
            coor: the coordinate tuple (x, y)
        """
        coor = Utils.scale(coor, reverse=True)
        if Actions.TOGGLE_SNAP_TO_GRID.get_active():
            offset_x, offset_y = (0, self.height / 2) if self.is_horizontal() else (self.height / 2, 0)
            coor = (
                Utils.align_to_grid(coor[0] + offset_x) - offset_x,
                Utils.align_to_grid(coor[1] + offset_y) - offset_y
            )
        self.states['coordinate'] = coor

    @property
    def rotation(self):
        """
        Get the rotation from the position param.

        Returns:
            the rotation in degrees or 0 if failure
        """
        return self.states['rotation']

    @rotation.setter
    def rotation(self, rot):
        """
        Set the rotation into the position param.

        Args:
            rot: the rotation in degrees
        """
        self.states['rotation'] = rot

    def notify(self):
        self._update_theming()

    def _update_theming(self):
        statecomb = []
        if self.is_dummy_block:
            statecomb.append(State.MISSING)
        if self.state == "bypassed":
            statecomb.append(State.BYPASSED)
        elif self.state != "enabled":
            statecomb.append(State.DISABLED)
        if self.highlighted:
            statecomb.append(State.SELECTED)
        state = State.reduce(statecomb)

        self._bg_color = self._theme("background-color", state)
        self._font_color = self._theme("color", state)

        self._border_color = self._theme("border-color", state)

        self._font_family = self._theme("font-family", state)
        self._font_size = int(float(self._theme("font-size", state)) * 1000)

        self._comment_color = Theme.get_property("color", "Comment", state)
        self._comment_family = Theme.get_property("font-family", "Comment",
                                                  state)
        self._comment_size = int(
            float(Theme.get_property("font-size", "Comment", state)) * 1000)

        self._padding = int(self._theme("padding", state))

    def create_shapes(self):
        """Update the block, parameters, and ports when a change occurs."""
        if self.is_horizontal():
            self._area = (0, 0, self.width, self.height)
        elif self.is_vertical():
            self._area = (0, 0, self.height, self.width)
        self.bounds_from_area(self._area)

        bussified = self.current_bus_structure['source'], self.current_bus_structure['sink']
        for ports, has_busses in zip((self.active_sources, self.active_sinks), bussified):
            if not ports:
                continue
            port_separation = PORT_SEPARATION if not has_busses else ports[0].height + PORT_SPACING
            offset = (self.height - (len(ports) - 1) * port_separation - ports[0].height) / 2
            for port in ports:
                port.create_shapes()
                port.coordinate = {
                    0: (+self.width, offset),
                    90: (offset, -port.width),
                    180: (-port.width, offset),
                    270: (offset, +self.width),
                }[port.connector_direction]

                offset += PORT_SEPARATION if not has_busses else port.height + PORT_SPACING

    def create_labels(self, cr=None):
        """Create the labels for the signal block."""

        # (Re-)creating layouts here, because layout.context_changed() doesn't seems to work (after zoom)
        title_layout, params_layout = self._surface_layouts = [
            Gtk.DrawingArea().create_pango_layout(''),  # title
            Gtk.DrawingArea().create_pango_layout(''),  # params
        ]

        if cr:  # to fix up extents after zooming
            PangoCairo.update_layout(cr, title_layout)
            PangoCairo.update_layout(cr, params_layout)

        title_layout.set_markup(
            f'<span weight="bold" '
            f'foreground="{get_hexcolor(self._font_color[:3])}" '
            f'font_family="{self._font_family}" '
            f'size="{self._font_size}" >'
            f'{Utils.encode(self.label)}</span>')
        title_width, title_height = title_layout.get_size()

        force_show_id = Actions.TOGGLE_SHOW_BLOCK_IDS.get_active()

        # update the params layout
        if not self.is_dummy_block:
            markups = [param.format_block_surface_markup()
                       for param in self.params.values() if (param.hide not in ('all', 'part') or (param.dtype == 'id' and force_show_id))]
        else:
            markups = [
                f'<span font_family="{self._font_family}" '
                f'size="{self._font_size}"><b>key: </b>{self.key}</span>'
            ]

        params_layout.set_spacing(LABEL_SEPARATION * Pango.SCALE)
        params_layout.set_markup('\n'.join(markups))
        params_width, params_height = params_layout.get_size() if markups else (0, 0)

        label_width = max(title_width, params_width) / Pango.SCALE
        label_height = title_height / Pango.SCALE
        if markups:
            label_height += LABEL_SEPARATION + params_height / Pango.SCALE

        # calculate width and height needed
        width = label_width + 2 * self._padding
        height = label_height + 2 * self._padding

        self._update_theming()
        self.create_port_labels()

        def get_min_height_for_ports(ports):
            min_height = 2 * PORT_BORDER_SEPARATION + len(ports) * PORT_SEPARATION
            # If any of the ports are bus ports - make the min height larger
            if any([p.dtype == 'bus' for p in ports]):
                min_height = 2 * PORT_BORDER_SEPARATION + sum(
                    port.height + PORT_SPACING for port in ports if port.dtype == 'bus'
                    ) - PORT_SPACING

            else:
                if ports:
                    min_height -= ports[-1].height
            return min_height

        height = max(height,
                     get_min_height_for_ports(self.active_sinks),
                     get_min_height_for_ports(self.active_sources))

        self.width, self.height = width, height = Utils.align_to_grid((width, height))

        self._surface_layouts_offsets = [
            (0, (height - label_height) / 2.0),
            (0, (height - label_height) / 2.0 + LABEL_SEPARATION + title_height / Pango.SCALE)
        ]

        title_layout.set_width(width * Pango.SCALE)
        title_layout.set_alignment(Pango.Alignment.CENTER)
        params_layout.set_indent((width - label_width) / 2.0 * Pango.SCALE)

        self.create_comment_layout()

    def create_port_labels(self):
        for ports in (self.active_sinks, self.active_sources):
            max_width = 0
            for port in ports:
                port.create_labels()
                max_width = max(max_width, port.width_with_label)
            for port in ports:
                port.width = max_width

    def create_comment_layout(self):
        markups = []

        # Show the flow graph complexity on the top block if enabled
        show_complexity = Actions.TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY.get_active()
        if show_complexity and self.key == "options":
            complexity = utils.flow_graph_complexity.calculate(self.parent)
            markups.append(
                f'<span foreground="{get_hexcolor(self._comment_color)}" '
                f'font_family="{self._comment_family}" '
                f'size="{self._comment_size}">'
                f'<b>Complexity: {Utils.num_to_str(complexity)}bal</b></span>')
        comment = self.comment  # Returns None if there are no comments
        if comment:
            if markups:
                markups.append('<span></span>')

            markups.append(
                f'<span foreground="{get_hexcolor(self._comment_color)}" '
                f'font_family="{self._comment_family}" '
                f'size="{self._comment_size}">'
                f'{Utils.encode(comment)}</span>'
            )
        if markups:
            layout = self._comment_layout = Gtk.DrawingArea().create_pango_layout('')
            layout.set_markup(''.join(markups))
        else:
            self._comment_layout = None

    def draw(self, cr):
        """
        Draw the signal block with label and inputs/outputs.
        """
        cr.translate(*self.coordinate)

        for port in self.active_ports():  # ports first
            cr.save()
            port.draw(cr)
            cr.restore()

        cr.rectangle(*self._area)
        cr.set_source_rgba(*self._bg_color)
        cr.fill_preserve()
        cr.set_source_rgba(*self._border_color)
        cr.stroke()

        # title and params label
        if self.is_vertical():
            cr.rotate(-math.pi / 2)
            cr.translate(-self.width, 0)
        cr.set_source_rgba(*self._font_color)
        for layout, offset in zip(self._surface_layouts, self._surface_layouts_offsets):
            cr.save()
            cr.translate(*offset)
            PangoCairo.update_layout(cr, layout)
            PangoCairo.show_layout(cr, layout)
            cr.restore()

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
        return Drawable.what_is_selected(self, coor, coor_m)

    def draw_comment(self, cr):
        if not self._comment_layout:
            return
        x, y = self.coordinate

        if self.is_horizontal():
            y += self.height + self._padding
        else:
            x += self.height + self._padding

        cr.save()
        cr.translate(x, y)
        PangoCairo.update_layout(cr, self._comment_layout)
        PangoCairo.show_layout(cr, self._comment_layout)
        cr.restore()

    def get_extents(self):
        extent = Drawable.get_extents(self)
        x, y = self.coordinate
        for port in self.active_ports():
            extent = (min_or_max(xy, offset + p_xy) for offset, min_or_max, xy, p_xy in zip(
                (x, y, x, y), (min, min, max, max), extent, port.get_extents()
            ))
        return tuple(extent)

    def get_extents_comment(self):
        x, y = self.coordinate
        if not self._comment_layout:
            return x, y, x, y
        if self.is_horizontal():
            y += self.height + self._padding
        else:
            x += self.height + self._padding
        w, h = self._comment_layout.get_pixel_size()
        return x, y, x + w, y + h

    ##############################################
    # Controller Modify
    ##############################################
    def type_controller_modify(self, direction):
        """
        Change the type controller.

        Args:
            direction: +1 or -1

        Returns:
            true for change
        """
        type_templates = ' '.join(p.dtype for p in self.params.values()) + ' '
        type_templates += ' '.join(p.get_raw('dtype') for p in (self.sinks + self.sources))
        type_param = None
        for key, param in self.params.items():
            if not param.is_enum():
                continue
            # Priority to the type controller
            if param.key in type_templates:
                type_param = param
                break
            # Use param if type param is unset
            if not type_param:
                type_param = param
        if not type_param:
            return False

        # Try to increment the enum by direction
        try:
            values = list(type_param.options)
            old_index = values.index(type_param.get_value())
            new_index = (old_index + direction + len(values)) % len(values)
            type_param.set_value(values[new_index])
            return True
        except IndexError:
            return False

    def port_controller_modify(self, direction):
        """
        Change the port controller.

        Args:
            direction: +1 or -1

        Returns:
            true for change
        """
        changed = False
        # Concat the nports string from the private nports settings of all ports
        nports_str = ' '.join(str(port.get_raw('multiplicity')) for port in self.ports())
        # Modify all params whose keys appear in the nports string
        for key, param in self.params.items():
            if param.is_enum() or param.key not in nports_str:
                continue
            # Try to increment the port controller by direction
            try:
                value = param.get_evaluated() + direction
                if value > 0:
                    param.set_value(value)
                    changed = True
            except ValueError:
                # Should we be logging something here
                pass
        return changed
