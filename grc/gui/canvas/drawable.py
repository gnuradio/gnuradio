"""
Copyright 2007, 2008, 2009, 2016 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""

from ..Constants import LINE_SELECT_SENSITIVITY


class Drawable(object):
    """
    GraphicalElement is the base class for all graphical elements.
    It contains an X,Y coordinate, a list of rectangular areas that the element occupies,
    and methods to detect selection of those areas.
    """

    @classmethod
    def make_cls_with_base(cls, super_cls):
        name = super_cls.__name__
        bases = (super_cls,) + cls.__bases__[1:]
        namespace = cls.__dict__.copy()
        return type(name, bases, namespace)

    def __init__(self):
        """
        Make a new list of rectangular areas and lines, and set the coordinate and the rotation.
        """
        self.coordinate = (0, 0)
        self.rotation = 0
        self.highlighted = False

        self._bounding_rects = []
        self._bounding_points = []

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

    def create_labels(self, cr=None):
        """
        Create labels (if applicable) and call on all children.
        Call this base method before creating labels in the element.
        """

    def create_shapes(self):
        """
        Create shapes (if applicable) and call on all children.
        Call this base method before creating shapes in the element.
        """

    def draw(self, cr):
        raise NotImplementedError()

    def bounds_from_area(self, area):
        x1, y1, w, h = area
        x2 = x1 + w
        y2 = y1 + h
        self._bounding_rects = [(x1, y1, x2, y2)]
        self._bounding_points = [(x1, y1), (x2, y1), (x1, y2), (x2, y2)]

    def bounds_from_line(self, line):
        self._bounding_rects = rects = []
        self._bounding_points = list(line)
        last_point = line[0]
        for x2, y2 in line[1:]:
            (x1, y1), last_point = last_point, (x2, y2)
            if x1 == x2:
                x1, x2 = x1 - LINE_SELECT_SENSITIVITY, x2 + LINE_SELECT_SENSITIVITY
                if y2 < y1:
                    y1, y2 = y2, y1
            elif y1 == y2:
                y1, y2 = y1 - LINE_SELECT_SENSITIVITY, y2 + LINE_SELECT_SENSITIVITY
                if x2 < x1:
                    x1, x2 = x2, x1

            rects.append((x1, y1, x2, y2))

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
        x, y = [a - b for a, b in zip(coor, self.coordinate)]

        if not coor_m:
            for x1, y1, x2, y2 in self._bounding_rects:
                if x1 <= x <= x2 and y1 <= y <= y2:
                    return self
        else:
            x_m, y_m = [a - b for a, b in zip(coor_m, self.coordinate)]
            if y_m < y:
                y, y_m = y_m, y
            if x_m < x:
                x, x_m = x_m, x

            for x1, y1 in self._bounding_points:
                if x <= x1 <= x_m and y <= y1 <= y_m:
                    return self

    def get_extents(self):
        x_min, y_min = x_max, y_max = self.coordinate
        x_min += min(x for x, y in self._bounding_points)
        y_min += min(y for x, y in self._bounding_points)
        x_max += max(x for x, y in self._bounding_points)
        y_max += max(y for x, y in self._bounding_points)
        return x_min, y_min, x_max, y_max

    def mouse_over(self):
        pass

    def mouse_out(self):
        pass
