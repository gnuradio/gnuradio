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
from .Constants import LINE_SELECT_SENSITIVITY

from six.moves import zip


class Element(object):
    """
    GraphicalElement is the base class for all graphical elements.
    It contains an X,Y coordinate, a list of rectangular areas that the element occupies,
    and methods to detect selection of those areas.
    """

    def __init__(self):
        """
        Make a new list of rectangular areas and lines, and set the coordinate and the rotation.
        """
        self.coordinate = (0, 0)
        self.rotation = 0
        self.highlighted = False

        self.areas = []
        self.lines = []

    def clear(self):
        """Empty the lines and areas."""
        del self.areas[:]
        del self.lines[:]

    def is_horizontal(self, rotation=None):
        """
        Is this element horizontal?
        If rotation is None, use this element's rotation.

        Args:
            rotation: the optional rotation

        Returns:
            true if rotation is horizontal
        """
        rotation = rotation or self.rotation
        return rotation in (0, 180)

    def is_vertical(self, rotation=None):
        """
        Is this element vertical?
        If rotation is None, use this element's rotation.

        Args:
            rotation: the optional rotation

        Returns:
            true if rotation is vertical
        """
        rotation = rotation or self.rotation
        return rotation in (90, 270)

    def create_labels(self):
        """
        Create labels (if applicable) and call on all children.
        Call this base method before creating labels in the element.
        """
        for child in self.get_children():
            child.create_labels()

    def create_shapes(self):
        """
        Create shapes (if applicable) and call on all children.
        Call this base method before creating shapes in the element.
        """
        self.clear()
        for child in self.get_children():
            child.create_shapes()

    def draw(self, widget, cr, border_color, bg_color):
        """
        Draw in the given window.

        Args:
            widget:
            cr:
            border_color: the color for lines and rectangle borders
            bg_color: the color for the inside of the rectangle
        """
        cr.translate(*self.coordinate)
        for area in self.areas:
            cr.set_source_rgb(*bg_color)
            cr.rectangle(*area)
            cr.fill()
            cr.set_source_rgb(*border_color)
            cr.rectangle(*area)
            cr.stroke()

        cr.set_source_rgb(*border_color)
        for line in self.lines:
            for point in line:
                cr.line_to(*point)
            cr.stroke()

    def rotate(self, rotation):
        """
        Rotate all of the areas by 90 degrees.

        Args:
            rotation: multiple of 90 degrees
        """
        self.rotation = (self.rotation + rotation) % 360

    def move(self, delta_coor):
        """
        Move the element by adding the delta_coor to the current coordinate.

        Args:
            delta_coor: (delta_x,delta_y) tuple
        """
        x, y = self.coordinate
        dx, dy = delta_coor
        self.coordinate = (x + dx, y + dy)

    def what_is_selected(self, coor, coor_m=None):
        """
        One coordinate specified:
            Is this element selected at given coordinate?
            ie: is the coordinate encompassed by one of the areas or lines?
        Both coordinates specified:
            Is this element within the rectangular region defined by both coordinates?
            ie: do any area corners or line endpoints fall within the region?

        Args:
            coor: the selection coordinate, tuple x, y
            coor_m: an additional selection coordinate.

        Returns:
            self if one of the areas/lines encompasses coor, else None.
        """
        # function to test if p is between a and b (inclusive)
        def in_between(p, a, b):
            # return min(a, b) <= p <= max(a, b)
            return a <= p <= b or b <= p <= a
        # relative coordinate
        x, y = [a - b for a, b in zip(coor, self.coordinate)]
        if coor_m:
            x_m, y_m = [a - b for a, b in zip(coor_m, self.coordinate)]
            # handle rectangular areas
            for x1, y1, w, h in self.areas:
                if (
                    in_between(x1,     x, x_m) and in_between(y1,     y, y_m) or
                    in_between(x1 + w, x, x_m) and in_between(y1,     y, y_m) or
                    in_between(x1,     x, x_m) and in_between(y1 + h, y, y_m) or
                    in_between(x1 + w, x, x_m) and in_between(y1 + h, y, y_m)
                ):
                    return self
            # handle horizontal or vertical lines
            for line in self.lines:
                last_point = line[0]
                for x2, y2 in line[1:]:
                    (x1, y1), last_point = last_point, (x2, y2)
                    if (
                        in_between(x1, x, x_m) and in_between(y1, y, y_m) or
                        in_between(x2, x, x_m) and in_between(y2, y, y_m)
                    ):
                        return self
            return None
        else:
            # handle rectangular areas
            for x1, y1, w, h in self.areas:
                if in_between(x, x1, x1+w) and in_between(y, y1, y1+h):
                    return self
            # handle horizontal or vertical lines
            for line in self.lines:
                last_point = line[0]
                for x2, y2 in line[1:]:
                    (x1, y1), last_point = last_point, (x2, y2)
                    if x1 == x2:
                        x1, x2 = x1 - LINE_SELECT_SENSITIVITY, x2 + LINE_SELECT_SENSITIVITY
                    if y1 == y2:
                        y1, y2 = y1 - LINE_SELECT_SENSITIVITY, y2 + LINE_SELECT_SENSITIVITY
                    if in_between(x, x1, x2) and in_between(y, y1, y2):
                        return self
            return None

    def mouse_over(self):
        pass

    def mouse_out(self):
        pass
