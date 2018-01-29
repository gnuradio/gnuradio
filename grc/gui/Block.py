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
import pango

from . import Actions, Colors, Utils, Constants

from . Element import Element
from ..core.Param import num_to_str
from ..core.utils import odict
from ..core.utils.complexity import calculate_flowgraph_complexity
from ..core.Block import Block as _Block

BLOCK_MARKUP_TMPL="""\
#set $foreground = $block.is_valid() and 'black' or 'red'
<span foreground="$foreground" font_desc="$font"><b>$encode($block.get_name())</b></span>"""

# Includes the additional complexity markup if enabled
COMMENT_COMPLEXITY_MARKUP_TMPL="""\
#set $foreground = $block.get_enabled() and '#444' or '#888'
#if $complexity
<span foreground="#444" size="medium" font_desc="$font"><b>$encode($complexity)</b></span>#slurp
#end if
#if $complexity and $comment
<span></span>
#end if
#if $comment
<span foreground="$foreground" font_desc="$font">$encode($comment)</span>#slurp
#end if
"""


class Block(Element, _Block):
    """The graphical signal block."""

    def __init__(self, flow_graph, n):
        """
        Block constructor.
        Add graphics related params to the block.
        """
        _Block.__init__(self, flow_graph, n)

        self.W = 0
        self.H = 0
        #add the position param
        self.get_params().append(self.get_parent().get_parent().Param(
            block=self,
            n=odict({
                'name': 'GUI Coordinate',
                'key': '_coordinate',
                'type': 'raw',
                'value': '(0, 0)',
                'hide': 'all',
            })
        ))
        self.get_params().append(self.get_parent().get_parent().Param(
            block=self,
            n=odict({
                'name': 'GUI Rotation',
                'key': '_rotation',
                'type': 'raw',
                'value': '0',
                'hide': 'all',
            })
        ))
        Element.__init__(self)
        self._comment_pixmap = None
        self.has_busses = [False, False]  # source, sink

    def get_coordinate(self):
        """
        Get the coordinate from the position param.

        Returns:
            the coordinate tuple (x, y) or (0, 0) if failure
        """
        proximity = Constants.BORDER_PROXIMITY_SENSITIVITY
        try: #should evaluate to tuple
            x, y = Utils.scale(eval(self.get_param('_coordinate').get_value()))
            fgW, fgH = self.get_parent().get_size()
            if x <= 0:
                x = 0
            elif x >= fgW - proximity:
                x = fgW - proximity
            if y <= 0:
                y = 0
            elif y >= fgH - proximity:
                y = fgH - proximity
            return (x, y)
        except:
            self.set_coordinate((0, 0))
            return (0, 0)

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
        self.get_param('_coordinate').set_value(str(Utils.scale(coor, reverse=True)))

    def bound_move_delta(self, delta_coor):
        """
        Limit potential moves from exceeding the bounds of the canvas

        Args:
            delta_coor: requested delta coordinate (dX, dY) to move

        Returns:
            The delta coordinate possible to move while keeping the block on the canvas
            or the input (dX, dY) on failure
        """
        dX, dY = delta_coor

        try:
            fgW, fgH = self.get_parent().get_size()
            x, y = Utils.scale(eval(self.get_param('_coordinate').get_value()))
            if self.is_horizontal():
                sW, sH = self.W, self.H
            else:
                sW, sH = self.H, self.W

            if x + dX < 0:
                dX = -x
            elif dX + x + sW >= fgW:
                dX = fgW - x - sW
            if y + dY < 0:
                dY = -y
            elif dY + y + sH >= fgH:
                dY = fgH - y - sH
        except:
            pass

        return ( dX, dY )

    def get_rotation(self):
        """
        Get the rotation from the position param.

        Returns:
            the rotation in degrees or 0 if failure
        """
        try: #should evaluate to dict
            rotation = eval(self.get_param('_rotation').get_value())
            return int(rotation)
        except:
            self.set_rotation(Constants.POSSIBLE_ROTATIONS[0])
            return Constants.POSSIBLE_ROTATIONS[0]

    def set_rotation(self, rot):
        """
        Set the rotation into the position param.

        Args:
            rot: the rotation in degrees
        """
        self.get_param('_rotation').set_value(str(rot))

    def create_shapes(self):
        """Update the block, parameters, and ports when a change occurs."""
        Element.create_shapes(self)
        if self.is_horizontal(): self.add_area((0, 0), (self.W, self.H))
        elif self.is_vertical(): self.add_area((0, 0), (self.H, self.W))

    def create_labels(self):
        """Create the labels for the signal block."""
        Element.create_labels(self)
        self._bg_color = self.is_dummy_block and Colors.MISSING_BLOCK_BACKGROUND_COLOR or \
                         self.get_bypassed() and Colors.BLOCK_BYPASSED_COLOR or \
                         self.get_enabled() and Colors.BLOCK_ENABLED_COLOR or Colors.BLOCK_DISABLED_COLOR

        layouts = list()
        #create the main layout
        layout = gtk.DrawingArea().create_pango_layout('')
        layouts.append(layout)
        layout.set_markup(Utils.parse_template(BLOCK_MARKUP_TMPL, block=self, font=Constants.BLOCK_FONT))
        self.label_width, self.label_height = layout.get_pixel_size()
        #display the params
        if self.is_dummy_block:
            markups = [
                '<span foreground="black" font_desc="{font}"><b>key: </b>{key}</span>'
                ''.format(font=Constants.PARAM_FONT, key=self._key)
            ]
        else:
            markups = [param.get_markup() for param in self.get_params() if param.get_hide() not in ('all', 'part')]
        if markups:
            layout = gtk.DrawingArea().create_pango_layout('')
            layout.set_spacing(Constants.LABEL_SEPARATION * pango.SCALE)
            layout.set_markup('\n'.join(markups))
            layouts.append(layout)
            w, h = layout.get_pixel_size()
            self.label_width = max(w, self.label_width)
            self.label_height += h + Constants.LABEL_SEPARATION
        width = self.label_width
        height = self.label_height
        #setup the pixmap
        pixmap = self.get_parent().new_pixmap(width, height)
        gc = pixmap.new_gc()
        gc.set_foreground(self._bg_color)
        pixmap.draw_rectangle(gc, True, 0, 0, width, height)
        #draw the layouts
        h_off = 0
        for i,layout in enumerate(layouts):
            w,h = layout.get_pixel_size()
            if i == 0: w_off = (width-w)/2
            else: w_off = 0
            pixmap.draw_layout(gc, w_off, h_off, layout)
            h_off = h + h_off + Constants.LABEL_SEPARATION
        #create vertical and horizontal pixmaps
        self.horizontal_label = pixmap
        if self.is_vertical():
            self.vertical_label = self.get_parent().new_pixmap(height, width)
            Utils.rotate_pixmap(gc, self.horizontal_label, self.vertical_label)
        #calculate width and height needed
        W = self.label_width + 2 * Constants.BLOCK_LABEL_PADDING

        def get_min_height_for_ports():
            visible_ports = filter(lambda p: not p.get_hide(), ports)
            min_height = 2*Constants.PORT_BORDER_SEPARATION + len(visible_ports) * Constants.PORT_SEPARATION
            if visible_ports:
                min_height -= ports[0].H
            return min_height
        H = max(
            [  # labels
                self.label_height + 2 * Constants.BLOCK_LABEL_PADDING
            ] +
            [  # ports
                get_min_height_for_ports() for ports in (self.get_sources_gui(), self.get_sinks_gui())
            ] +
            [  # bus ports only
                2 * Constants.PORT_BORDER_SEPARATION +
                sum([port.H + Constants.PORT_SPACING for port in ports if port.get_type() == 'bus']) - Constants.PORT_SPACING
                for ports in (self.get_sources_gui(), self.get_sinks_gui())
            ]
        )
        self.W, self.H = Utils.align_to_grid((W, H))
        self.has_busses = [
            any(port.get_type() == 'bus' for port in ports)
            for ports in (self.get_sources_gui(), self.get_sinks_gui())
        ]
        self.create_comment_label()

    def create_comment_label(self):
        comment = self.get_comment()    # Returns None if there are no comments
        complexity = None

        # Show the flowgraph complexity on the top block if enabled
        if Actions.TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY.get_active() and self.get_key() == "options":
            complexity = calculate_flowgraph_complexity(self.get_parent())
            complexity = "Complexity: {0}bal".format(num_to_str(complexity))

        layout = gtk.DrawingArea().create_pango_layout('')
        layout.set_markup(Utils.parse_template(COMMENT_COMPLEXITY_MARKUP_TMPL,
                                               block=self,
                                               comment=comment,
                                               complexity=complexity,
                                               font=Constants.BLOCK_FONT))

        # Setup the pixel map. Make sure that layout not empty
        width, height = layout.get_pixel_size()
        if width and height:
            padding = Constants.BLOCK_LABEL_PADDING
            pixmap = self.get_parent().new_pixmap(width + 2 * padding,
                                                  height + 2 * padding)
            gc = pixmap.new_gc()
            gc.set_foreground(Colors.COMMENT_BACKGROUND_COLOR)
            pixmap.draw_rectangle(
                gc, True, 0, 0, width + 2 * padding, height + 2 * padding)
            pixmap.draw_layout(gc, padding, padding, layout)
            self._comment_pixmap = pixmap
        else:
            self._comment_pixmap = None

    def draw(self, gc, window):
        """
        Draw the signal block with label and inputs/outputs.

        Args:
            gc: the graphics context
            window: the gtk window to draw on
        """
        # draw ports
        for port in self.get_ports_gui():
            port.draw(gc, window)
        # draw main block
        x, y = self.get_coordinate()
        Element.draw(
            self, gc, window, bg_color=self._bg_color,
            border_color=self.is_highlighted() and Colors.HIGHLIGHT_COLOR or
                         self.is_dummy_block and Colors.MISSING_BLOCK_BORDER_COLOR or Colors.BORDER_COLOR,
        )
        #draw label image
        if self.is_horizontal():
            window.draw_drawable(gc, self.horizontal_label, 0, 0, x+Constants.BLOCK_LABEL_PADDING, y+(self.H-self.label_height)/2, -1, -1)
        elif self.is_vertical():
            window.draw_drawable(gc, self.vertical_label, 0, 0, x+(self.H-self.label_height)/2, y+Constants.BLOCK_LABEL_PADDING, -1, -1)

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

    def draw_comment(self, gc, window):
        if not self._comment_pixmap:
            return
        x, y = self.get_coordinate()

        if self.is_horizontal():
            y += self.H + Constants.BLOCK_LABEL_PADDING
        else:
            x += self.H + Constants.BLOCK_LABEL_PADDING

        window.draw_drawable(gc, self._comment_pixmap, 0, 0, x, y, -1, -1)
