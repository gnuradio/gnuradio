#
# Copyright 2005,2006,2011,2013 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

"""
QAM modulation and demodulation.
"""

from math import pi, sqrt, log

from gnuradio import gr
from generic_mod_demod import generic_mod, generic_demod
from generic_mod_demod import shared_mod_args, shared_demod_args
from utils.gray_code import gray_code
from utils import mod_codes
import modulation_utils
import digital_swig as digital

# Default number of points in constellation.
_def_constellation_points = 16
# Whether the quadrant bits are coded differentially.
_def_differential = True
# Whether gray coding is used.  If differential is True then gray
# coding is used within but not between each quadrant.
_def_mod_code = mod_codes.NO_CODE

def is_power_of_four(x):
    v = log(x)/log(4)
    return int(v) == v

def get_bit(x, n):
    """ Get the n'th bit of integer x (from little end)."""
    return (x&(0x01 << n)) >> n

def get_bits(x, n, k):
    """ Get the k bits of integer x starting at bit n(from little end)."""
    # Remove the n smallest bits
    v = x >> n 
    # Remove all bits bigger than n+k-1
    return v % pow(2, k)

def make_differential_constellation(m, gray_coded):
    """
    Create a constellation with m possible symbols where m must be
    a power of 4.

    Points are laid out in a square grid.

    Bits referring to the quadrant are differentilly encoded,
    remaining bits are gray coded.

    """
    sqrtm = pow(m, 0.5)
    if (not isinstance(m, int) or m < 4 or not is_power_of_four(m)):
        raise ValueError("m must be a power of 4 integer.")
    # Each symbol holds k bits.
    k = int(log(m) / log(2.0))
    # First create a constellation for one quadrant containing m/4 points.
    # The quadrant has 'side' points along each side of a quadrant.
    side = int(sqrtm/2)
    if gray_coded:
        # Number rows and columns using gray codes.
        gcs = gray_code(side)
        # Get inverse gray codes.
        i_gcs = dict([(v, key) for key, v in enumerate(gcs)])
    else:
        i_gcs = dict([(i, i) for i in range(0, side)])
    # The distance between points is found.
    step = 1/(side-0.5)

    gc_to_x = [(i_gcs[gc]+0.5)*step for gc in range(0, side)]

    # Takes the (x, y) location of the point with the quadrant along
    # with the quadrant number. (x, y) are integers referring to which
    # point within the quadrant it is.
    # A complex number representing this location of this point is returned.
    def get_c(gc_x, gc_y, quad):
        if quad == 0:
            return complex(gc_to_x[gc_x], gc_to_x[gc_y])
        if quad == 1:
            return complex(-gc_to_x[gc_y], gc_to_x[gc_x])
        if quad == 2:
            return complex(-gc_to_x[gc_x], -gc_to_x[gc_y])
        if quad == 3:
            return complex(gc_to_x[gc_y], -gc_to_x[gc_x])
        raise StandardError("Impossible!")

    # First two bits determine quadrant.
    # Next (k-2)/2 bits determine x position.
    # Following (k-2)/2 bits determine y position.
    # How x and y relate to real and imag depends on quadrant (see get_c function).
    const_map = []
    for i in range(m):
        y = get_bits(i, 0, (k-2)/2)
        x = get_bits(i, (k-2)/2, (k-2)/2)
        quad = get_bits(i, k-2, 2)
        const_map.append(get_c(x, y, quad))

    return const_map

def make_non_differential_constellation(m, gray_coded):
    side = int(pow(m, 0.5))
    if (not isinstance(m, int) or m < 4 or not is_power_of_four(m)):
        raise ValueError("m must be a power of 4 integer.")
    # Each symbol holds k bits.
    k = int(log(m) / log(2.0))
    if gray_coded:
        # Number rows and columns using gray codes.
        gcs = gray_code(side)
        # Get inverse gray codes.
        i_gcs = mod_codes.invert_code(gcs)
    else:
        i_gcs = range(0, side)
    # The distance between points is found.
    step = 2.0/(side-1)

    gc_to_x = [-1 + i_gcs[gc]*step for gc in range(0, side)]
    # First k/2 bits determine x position.
    # Following k/2 bits determine y position.
    const_map = []
    for i in range(m):
        y = gc_to_x[get_bits(i, 0, k/2)]
        x = gc_to_x[get_bits(i, k/2, k/2)]
        const_map.append(complex(x,y))
    return const_map

# /////////////////////////////////////////////////////////////////////////////
#                           QAM constellation
# /////////////////////////////////////////////////////////////////////////////

def qam_constellation(constellation_points=_def_constellation_points,
                      differential=_def_differential,
                      mod_code=_def_mod_code,
                      large_ampls_to_corners=False):
    """
    Creates a QAM constellation object.

    If large_ampls_to_corners=True then sectors that are probably
    occupied due to a phase offset, are not mapped to the closest
    constellation point.  Rather we take into account the fact that a
    phase offset is probably the problem and map them to the closest
    corner point.  It's a bit hackish but it seems to improve
    frequency locking.
    """
    if mod_code == mod_codes.GRAY_CODE:
        gray_coded = True
    elif mod_code == mod_codes.NO_CODE:
        gray_coded = False
    else:
        raise ValueError("Mod code is not implemented for QAM")
    if differential:
        points = make_differential_constellation(constellation_points, gray_coded=False)
    else:
        points = make_non_differential_constellation(constellation_points, gray_coded)
    side = int(sqrt(constellation_points))
    width = 2.0/(side-1)

    # No pre-diff code
    # Should add one so that we can gray-code the quadrant bits too.
    pre_diff_code = []
    if not large_ampls_to_corners:
        constellation = digital.constellation_rect(points, pre_diff_code, 4,
                                                        side, side, width, width)
    else:
        sector_values = large_ampls_to_corners_mapping(side, points, width)
        constellation = digital.constellation_expl_rect(
            points, pre_diff_code, 4, side, side, width, width, sector_values)

    return constellation

def find_closest_point(p, qs):
    """
    Return in index of the closest point in 'qs' to 'p'.
    """
    min_dist = None
    min_i = None
    for i, q in enumerate(qs):
        dist = abs(q-p)
        if min_dist is None or dist < min_dist:
            min_dist = dist
            min_i = i
    return min_i

def large_ampls_to_corners_mapping(side, points, width):
    """
    We have a grid that we use for decision making.  One additional row/column
    is placed on each side of the grid.  Points in these additional rows/columns
    are mapped to the corners rather than the closest constellation points.

    Args:
        side: The number of rows/columns in the grid that we use to do
              decision making.
        points: The list of constellation points.
        width: The width of the rows/columns.

    Returns:
        sector_values maps the sector index to the constellation
        point index.
    """
    # First find the indices of the corner points.
    # Assume the corner points are the 4 points with the largest magnitudes.
    corner_indices = []
    corner_points = []
    max_mag = 0
    for i, p in enumerate(points):
        if abs(p) > max_mag:
            corner_indices = [i]
            corner_points = [p]
            max_mag = abs(p)
        elif abs(p) == max_mag:
            corner_indices.append(i)
            corner_points.append(p)
    if len(corner_indices) != 4:
        raise ValueError("Found {0} corner indices.  Expected 4."
                         .format(len(corner_indices)))
    # We want an additional layer around the constellation
    # Value in this extra layer will be mapped to the closest corner rather
    # than the closest constellation point.
    extra_layers = 1
    side = side + extra_layers*2
    # Calculate sector values
    sector_values = []
    for real_x in range(side):
        for imag_x in range(side):
            sector = real_x * side + imag_x
            # If this sector is a normal constellation sector then
            # use the center point.
            c = ((real_x-side/2.0+0.5)*width +
                 (imag_x-side/2.0+0.5)*width*1j)
            if (real_x >= extra_layers and real_x < side-extra_layers
                and imag_x >= extra_layers and imag_x < side-extra_layers):
                # This is not an edge row/column.  Find closest point.
                index = find_closest_point(c, points)
            else:
                # This is an edge. Find closest corner point.
                index = corner_indices[find_closest_point(c, corner_points)]
            sector_values.append(index)
    return sector_values

 
# /////////////////////////////////////////////////////////////////////////////
#                           QAM modulator
# /////////////////////////////////////////////////////////////////////////////

class qam_mod(generic_mod):
    """
    Hierarchical block for RRC-filtered QAM modulation.
    
    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.
    
    Args:
        constellation_points: Number of constellation points (must be a power of four) (integer).
        mod_code: Whether to use a gray_code (digital.mod_codes.GRAY_CODE) or not (digital.mod_codes.NO_CODE).
        differential: Whether to use differential encoding (boolean).
    """
    # See generic_mod for additional arguments
    __doc__ += shared_mod_args

    def __init__(self, constellation_points=_def_constellation_points,
                 differential=_def_differential,
                 mod_code=_def_mod_code,
                 *args, **kwargs):

        """
	Hierarchical block for RRC-filtered QAM modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        Args:
            constellation_points: Number of constellation points.
                Must be a power of 4.
            mod_code: Specifies an encoding to use (typically used to indicated
                      if we want gray coding, see digital.utils.mod_codes)

        See generic_mod block for list of additional parameters.
	"""

        constellation = qam_constellation(constellation_points, differential,
                                          mod_code)
        # We take care of the gray coding in the constellation
        # generation so it doesn't need to be done in the block.
        super(qam_mod, self).__init__(constellation, differential=differential,
                                      *args, **kwargs)

# /////////////////////////////////////////////////////////////////////////////
#                           QAM demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class qam_demod(generic_demod):
    """
    Hierarchical block for RRC-filtered QAM modulation.
    
    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.
    
    Args:
        constellation_points: Number of constellation points (must be a power of four) (integer).
        mod_code: Whether to use a gray_code (digital.mod_codes.GRAY_CODE) or not (digital.mod_codes.NO_CODE).
        differential: Whether to use differential encoding (boolean).
    """
    # See generic_demod for additional arguments
    __doc__ += shared_mod_args

    def __init__(self, constellation_points=_def_constellation_points,
                 differential=_def_differential,
                 mod_code=_def_mod_code,
                 large_ampls_to_corner = False,
                 *args, **kwargs):
        """
	Hierarchical block for RRC-filtered QAM modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        Args:
            constellation_points: Number of constellation points.
                Must be a power of 4.
            mod_code: Specifies an encoding to use (typically used to indicated
                      if we want gray coding, see digital.utils.mod_codes)
            large_ampls_to_corners:  If this is set to True then when the
                constellation is making decisions, points that are far outside
                the constellation are mapped to the closest corner rather than
                the closet constellation point.  This can help with phase
                locking.

        See generic_demod block for list of additional parameters.
        """
        constellation = qam_constellation(constellation_points, differential,
                                          mod_code)
        # We take care of the gray coding in the constellation
        # generation so it doesn't need to be done in the block.
        super(qam_demod, self).__init__(constellation, differential=differential,
                                        *args, **kwargs)

#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_mod('qam', qam_mod)
modulation_utils.add_type_1_demod('qam', qam_demod)
modulation_utils.add_type_1_constellation('qam', qam_constellation)
