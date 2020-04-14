"""
Copyright 2008-2011,2015 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""

from __future__ import absolute_import

from sys import platform
import numbers

from gi.repository import GLib
import cairo
import six

from .canvas.colors import FLOWGRAPH_BACKGROUND_COLOR
from . import Constants


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
    if rotation not in Constants.POSSIBLE_ROTATIONS:
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


def align_to_grid(coor, mode=round):
    def align(value):
        return int(mode(value / (1.0 * Constants.CANVAS_GRID_SIZE)) * Constants.CANVAS_GRID_SIZE)
    try:
        return [align(c) for c in coor]
    except TypeError:
        x = coor
        return align(coor)


def num_to_str(num):
    """ Display logic for numbers """
    def eng_notation(value, fmt='g'):
        """Convert a number to a string in engineering notation.  E.g., 5e-9 -> 5n"""
        template = '{:' + fmt + '}{}'
        magnitude = abs(value)
        for exp, symbol in zip(range(9, -15-1, -3), 'GMk munpf'):
            factor = 10 ** exp
            if magnitude >= factor:
                return template.format(value / factor, symbol.strip())
        return template.format(value, '')

    if isinstance(num, numbers.Complex):
        num = complex(num)  # Cast to python complex
        if num == 0:
            return '0'
        output = eng_notation(num.real) if num.real else ''
        output += eng_notation(num.imag, '+g' if output else 'g') + 'j' if num.imag else ''
        return output
    else:
        return str(num)


def encode(value):
    """Make sure that we pass only valid utf-8 strings into markup_escape_text.

    Older versions of glib seg fault if the last byte starts a multi-byte
    character.
    """
    if six.PY2:
        valid_utf8 = value.decode('utf-8', errors='replace').encode('utf-8')
    else:
        valid_utf8 = value
    return GLib.markup_escape_text(valid_utf8)


def make_screenshot(flow_graph, file_path, transparent_bg=False):
    if not file_path:
        return

    x_min, y_min, x_max, y_max = flow_graph.get_extents()
    padding = Constants.CANVAS_GRID_SIZE
    width = x_max - x_min + 2 * padding
    height = y_max - y_min + 2 * padding

    if file_path.endswith('.png'):
        psurf = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
    elif file_path.endswith('.pdf'):
        psurf = cairo.PDFSurface(file_path, width, height)
    elif file_path.endswith('.svg'):
        psurf = cairo.SVGSurface(file_path, width, height)
    else:
        raise ValueError('Unknown file format')

    cr = cairo.Context(psurf)

    if not transparent_bg:
        cr.set_source_rgba(*FLOWGRAPH_BACKGROUND_COLOR)
        cr.rectangle(0, 0, width, height)
        cr.fill()

    cr.translate(padding - x_min, padding - y_min)

    flow_graph.create_labels(cr)
    flow_graph.create_shapes()
    flow_graph.draw(cr)

    if file_path.endswith('.png'):
        psurf.write_to_png(file_path)
    if file_path.endswith('.pdf') or file_path.endswith('.svg'):
        cr.show_page()
    psurf.finish()


def scale(coor, reverse=False):
    factor = Constants.DPI_SCALING if not reverse else 1 / Constants.DPI_SCALING
    return tuple(int(x * factor) for x in coor)


def scale_scalar(coor, reverse=False):
    factor = Constants.DPI_SCALING if not reverse else 1 / Constants.DPI_SCALING
    return int(coor * factor)

def get_modifier_key(angle_brackets=False):
    """
    Get the modifier key based on platform.

    Args:
        angle_brackets: if return the modifier key with <> or not

    Returns:
        return the string with the modifier key
    """
    if platform == "darwin":
        if angle_brackets:
            return "<Meta>"
        else:
            return "Meta"
    else:
        if angle_brackets:
            return "<Ctrl>"
        else:
            return "Ctrl"
