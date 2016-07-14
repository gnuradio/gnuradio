"""
Copyright 2008-2011,2015 Free Software Foundation, Inc.
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

from gi.repository import GLib

from .Constants import POSSIBLE_ROTATIONS, CANVAS_GRID_SIZE


def get_rotated_coordinate(coor, rotation):
    """
    Rotate the coordinate by the given rotation.

    Args:
        coor: the coordinate x, y tuple
        rotation: the angle in degrees

    Returns:
        the rotated coordinates
    """
    # handles negative angles
    rotation = (rotation + 360) % 360
    if rotation not in POSSIBLE_ROTATIONS:
        raise ValueError('unusable rotation angle "%s"'%str(rotation))
    # determine the number of degrees to rotate
    cos_r, sin_r = {
        0: (1, 0), 90: (0, 1), 180: (-1, 0), 270: (0, -1),
    }[rotation]
    x, y = coor
    return x * cos_r + y * sin_r, -x * sin_r + y * cos_r


def get_angle_from_coordinates(p1, p2):
    """
    Given two points, calculate the vector direction from point1 to point2, directions are multiples of 90 degrees.

    Args:
        (x1,y1): the coordinate of point 1
        (x2,y2): the coordinate of point 2

    Returns:
        the direction in degrees
    """
    (x1, y1) = p1
    (x2, y2) = p2
    if y1 == y2:  # 0 or 180
        return 0 if x2 > x1 else 180
    else:  # 90 or 270
        return 270 if y2 > y1 else 90


def encode(value):
    """Make sure that we pass only valid utf-8 strings into markup_escape_text.

    Older versions of glib seg fault if the last byte starts a multi-byte
    character.
    """
    valid_utf8 = value.decode('utf-8', errors='replace').encode('utf-8')
    return GLib.markup_escape_text(valid_utf8)


def align_to_grid(coor, mode=round):
    def align(value):
        return int(mode(value / (1.0 * CANVAS_GRID_SIZE)) * CANVAS_GRID_SIZE)
    try:
        return [align(c) for c in coor]
    except TypeError:
        x = coor
        return align(coor)
