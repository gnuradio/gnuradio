#
# Copyright 2005,2006,2011 Free Software Foundation, Inc.
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
PSK modulation and demodulation.
"""

from math import pi, log
from cmath import exp

import digital_swig
import modulation_utils
from utils import mod_codes, gray_code
from generic_mod_demod import generic_mod, generic_demod

# Default number of points in constellation.
_def_constellation_points = 4
# The default encoding (e.g. gray-code, set-partition)
_def_mod_code = mod_codes.GRAY_CODE

def create_encodings(mod_code, arity):
    post_diff_code = None
    if mod_code not in mod_codes.codes:
        raise ValueError('That modulation code does not exist.')
    if mod_code == mod_codes.GRAY_CODE:
        pre_diff_code = gray_code.gray_code(arity)
    elif mod_code == mod_codes.SET_PARTITION_CODE:
        pre_diff_code = set_partition_code.set_partition_code(arity) 
    elif mod_code == mod_codes.NO_CODE:
        pre_diff_code = []
    else:
        raise ValueError('That modulation code is not implemented for this constellation.')
    return (pre_diff_code, post_diff_code)
    
# /////////////////////////////////////////////////////////////////////////////
#                           PSK constellation
# /////////////////////////////////////////////////////////////////////////////

def psk_constellation(m=_def_constellation_points, mod_code=_def_mod_code):
    """
    Creates a PSK constellation object.
    """
    k = log(m) / log(2.0)
    if (k != int(k)):
        raise StandardError('Number of constellation points must be a power of two.')
    points = [exp(2*pi*(0+1j)*i/m) for i in range(0,m)]
    pre_diff_code, post_diff_code = create_encodings(mod_code, m)
    if post_diff_code is not None:
        inverse_post_diff_code = mod_codes.invert_code(post_diff_code)
        points = [points[x] for x in inverse_post_diff_code]
    constellation = digital_swig.constellation_psk(points, pre_diff_code, m)
    return constellation

# /////////////////////////////////////////////////////////////////////////////
#                           PSK modulator
# /////////////////////////////////////////////////////////////////////////////

class psk_mod(generic_mod):

    def __init__(self, constellation_points=_def_constellation_points,
                 mod_code=_def_mod_code,
                 *args, **kwargs):

        """
	Hierarchical block for RRC-filtered PSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        See generic_mod block for list of parameters.
	"""

        constellation = psk_constellation(constellation_points, mod_code)
        super(psk_mod, self).__init__(constellation, *args, **kwargs)

# /////////////////////////////////////////////////////////////////////////////
#                           PSK demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class psk_demod(generic_demod):

    def __init__(self, constellation_points=_def_constellation_points,
                 mod_code=_def_mod_code,
                 *args, **kwargs):

        """
	Hierarchical block for RRC-filtered PSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        See generic_demod block for list of parameters.
        """

        constellation = psk_constellation(constellation_points, mod_code)
        super(psk_demod, self).__init__(constellation, *args, **kwargs)

#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_mod('psk', psk_mod)
modulation_utils.add_type_1_demod('psk', psk_demod)
modulation_utils.add_type_1_constellation('psk', psk_constellation)
