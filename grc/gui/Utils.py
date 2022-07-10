"""
Copyright 2008-2011,2015 Free Software Foundation, Inc.
Copyright 2022 Marcus Müller
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from sys import platform
import os

import numpy
from gi.repository import GLib
import cairo

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
        raise ValueError('unusable rotation angle "%s"' % str(rotation))
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


def num_to_str(num) -> str:
    """ Display logic for numbers """

    def clean_e(unclean: str) -> str:
        """
        Removes the + from 1.2e+09, and the 0 from both 1.2e-09 and 1.2e09
        """
        return unclean\
            .replace("e-0", "e-")\
            .replace("e+0", "e+")\
            .replace("e+", "e")\
            .strip()

    def thousands_sep(inputstr: str,
                      replacement: str = "\N{SPACE}",
                      original_sep: str = "_") -> str:
        """
        Python allows us to group thousands with _ , but not with arbitrary
        characters.

        This function is just a simple str.replace wrapper to keep the default
        somewhere visible
        """
        return inputstr.replace(original_sep, replacement)

    if numpy.iscomplex(num):
        intreal = int(numpy.real(num))
        intimag = int(numpy.imag(num))
        if intreal == numpy.real(num) and \
           intimag == numpy.imag(num) and \
           abs(num) < 2**0.5 * 10**6:
            if intreal < 10**6 and intimag < 10**6:
                return f"{intreal:d}{intimag:+d}j"
            return thousands_sep(f"{intreal:_d}{intimag:+_d}j")
        return clean_e(f"{numpy.real(num):5.4g}{numpy.imag(num):+5.4g}j")

    if int(num) == num and abs(num) < 10**9:
        if abs(num) < 10**6:
            return f"{int(num):d}"
        return thousands_sep(f"{int(num):_d}")
    return clean_e(f"{num:9g}")


def encode(value):
    return GLib.markup_escape_text(value)


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


_nproc = None


def get_cmake_nproc():
    """ Get number of cmake processes for C++ flowgraphs """
    global _nproc  # Cached result
    if _nproc:
        return _nproc
    try:
        # See https://docs.python.org/3.8/library/os.html#os.cpu_count
        _nproc = len(os.sched_getaffinity(0))
    except:
        _nproc = os.cpu_count()
    if not _nproc:
        _nproc = 1

    _nproc = max(_nproc // 2 - 1, 1)
    return _nproc
