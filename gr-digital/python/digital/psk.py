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
from generic_mod_demod import shared_mod_args, shared_demod_args

# Default number of points in constellation.
_def_constellation_points = 4
# The default encoding (e.g. gray-code, set-partition)
_def_mod_code = mod_codes.GRAY_CODE
# Default use of differential encoding
_def_differential = True

def create_encodings(mod_code, arity, differential):
    post_diff_code = None
    if mod_code not in mod_codes.codes:
        raise ValueError('That modulation code does not exist.')
    if mod_code == mod_codes.GRAY_CODE:
        if differential:
            pre_diff_code = gray_code.gray_code(arity)
            post_diff_code = None
        else:
            pre_diff_code = []
            post_diff_code = gray_code.gray_code(arity)
    elif mod_code == mod_codes.NO_CODE:
        pre_diff_code = []
        post_diff_code = None
    else:
        raise ValueError('That modulation code is not implemented for this constellation.')
    return (pre_diff_code, post_diff_code)
    
# /////////////////////////////////////////////////////////////////////////////
#                           PSK constellation
# /////////////////////////////////////////////////////////////////////////////

def psk_constellation(m=_def_constellation_points, mod_code=_def_mod_code,
                      differential=_def_differential):
    """
    Creates a PSK constellation object.
    """
    k = log(m) / log(2.0)
    if (k != int(k)):
        raise StandardError('Number of constellation points must be a power of two.')
    points = [exp(2*pi*(0+1j)*i/m) for i in range(0,m)]
    pre_diff_code, post_diff_code = create_encodings(mod_code, m, differential)
    if post_diff_code is not None:
        inverse_post_diff_code = mod_codes.invert_code(post_diff_code)
        points = [points[x] for x in inverse_post_diff_code]
    constellation = digital_swig.constellation_psk(points, pre_diff_code, m)
    return constellation

# /////////////////////////////////////////////////////////////////////////////
#                           PSK modulator
# /////////////////////////////////////////////////////////////////////////////

class psk_mod(generic_mod):
    """
    Hierarchical block for RRC-filtered PSK modulation.
    
    The input is a byte stream (unsigned char), treated as a series of packed
    symbols. Symbols are grouped from MSB to LSB.

    The output is the complex modulated signal at baseband, with a given number
    of samples per symbol.

    If "Samples/Symbol" is 2, and "Number of Constellation Points" is 4, a
    single byte contains four symbols, and will produce eight samples.

    Args:
        constellation_points: Number of constellation points (must be a power of two) (integer).
        mod_code: Whether to use a gray_code (digital.mod_codes.GRAY_CODE) or not (digital.mod_codes.NO_CODE).
        differential: Whether to use differential encoding (boolean).
    """
    # See generic_mod for additional arguments
    __doc__ += shared_mod_args

    def __init__(self, constellation_points=_def_constellation_points,
                 mod_code=_def_mod_code,
                 differential=_def_differential,
                 *args, **kwargs):
        constellation = psk_constellation(constellation_points, mod_code, differential)
        super(psk_mod, self).__init__(constellation, differential, *args, **kwargs)

# /////////////////////////////////////////////////////////////////////////////
#                           PSK demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class psk_demod(generic_demod):

    """
    Hierarchical block for RRC-filtered PSK modulation.
    
    The input is a complex modulated signal at baseband.

    The output is a stream of bytes, each representing a recovered bit.
    The most significant bit is reported first.

    Args:
        constellation_points: Number of constellation points (must be a power of two) (integer).
        mod_code: Whether to use a gray_code (digital.mod_codes.GRAY_CODE) or not (digital.mod_codes.NO_CODE).
        differential: Whether to use differential encoding (boolean).
    """
    # See generic_mod for additional arguments
    __doc__ += shared_mod_args
    def __init__(self, constellation_points=_def_constellation_points,
                 mod_code=_def_mod_code,
                 differential=_def_differential,
                 *args, **kwargs):
        constellation = psk_constellation(constellation_points, mod_code, differential)
        super(psk_demod, self).__init__(constellation, differential, *args, **kwargs)

#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_mod('psk', psk_mod)
modulation_utils.add_type_1_demod('psk', psk_demod)
modulation_utils.add_type_1_constellation('psk', psk_constellation)
