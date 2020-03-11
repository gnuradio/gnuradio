# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

"""
This file contains constellations that are similar to QAM, but are not perfect squares.
"""
from __future__ import absolute_import
from __future__ import unicode_literals

from . import digital_swig
from .qam import large_ampls_to_corners_mapping

def qam32_holeinside_constellation(large_ampls_to_corners=False):
    # First make constellation for one quadrant.
    #      0   1   2
    # 2 - 010 111 110
    # 1 - 011 101 100
    # 0 - 000 001

    # Have put hole in the side rather than corner.
    # Corner point is helpful for frequency locking.

    # It has an attempt at some gray-coding, but not
    # a very good one.

    # Indices are (horizontal, vertical).
    indices_and_numbers = (
        ((0, 0), 0b000),
        ((0, 1), 0b011),
        ((0, 2), 0b010),
        ((1, 0), 0b001),
        ((1, 1), 0b101),
        ((1, 2), 0b111),
        ((2, 1), 0b100),
        ((2, 2), 0b110),
        )
    points = [None]*32
    for indices, number in indices_and_numbers:
        p_in_quadrant = 0.5+indices[0] + 1j*(0.5+indices[1])
        for quadrant in range(4):
            index = number + 8 * quadrant
            rotation = pow(1j, quadrant)
            p = p_in_quadrant * rotation
            points[index] = p
    side = 6
    width = 1
    # Double number of boxes on side
    # This is so that points in the 'hole' get assigned correctly.
    side = 12
    width = 0.5
    pre_diff_code = []
    if not large_ampls_to_corners:
        constellation = digital_swig.constellation_rect(points, pre_diff_code, 4,
                                                        side, side, width, width)
    else:
        sector_values = large_ampls_to_corners_mapping(side, points, width)
        constellation = digital_swig.constellation_expl_rect(
            points, pre_diff_code, 4, side, side, width, width, sector_values)
    return constellation
