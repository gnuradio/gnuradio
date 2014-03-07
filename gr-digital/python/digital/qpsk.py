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
QPSK modulation.

Demodulation is not included since the generic_mod_demod
"""

from gnuradio import gr
from gnuradio.digital.generic_mod_demod import generic_mod, generic_demod
from gnuradio.digital.generic_mod_demod import shared_mod_args, shared_demod_args
from utils import mod_codes
import digital_swig as digital
import modulation_utils

# The default encoding (e.g. gray-code, set-partition)
_def_mod_code = mod_codes.GRAY_CODE

# /////////////////////////////////////////////////////////////////////////////
#                           QPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def qpsk_constellation(mod_code=_def_mod_code):
    """
    Creates a QPSK constellation.
    """
    if mod_code != mod_codes.GRAY_CODE:
        raise ValueError("This QPSK mod/demod works only for gray-coded constellations.")
    return digital.constellation_qpsk()

# /////////////////////////////////////////////////////////////////////////////
#                           QPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class qpsk_mod(generic_mod):
    """
    Hierarchical block for RRC-filtered QPSK modulation.

    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.

    Args:
        mod_code: Whether to use a gray_code (digital.mod_codes.GRAY_CODE) or not (digital.mod_codes.NO_CODE).
        differential: Whether to use differential encoding (boolean).
    """
    # See generic_mod for additional arguments
    __doc__ += shared_mod_args

    def __init__(self, mod_code=_def_mod_code, differential=False, *args, **kwargs):
        pre_diff_code = True
        if not differential:
            constellation = digital.constellation_qpsk()
            if mod_code != mod_codes.GRAY_CODE:
                raise ValueError("This QPSK mod/demod works only for gray-coded constellations.")
        else:
            constellation = digital.constellation_dqpsk()
            if mod_code not in set([mod_codes.GRAY_CODE, mod_codes.NO_CODE]):
                raise ValueError("That mod_code is not supported for DQPSK mod/demod.")
            if mod_code == mod_codes.NO_CODE:
                pre_diff_code = False

        super(qpsk_mod, self).__init__(constellation=constellation,
                                       differential=differential,
                                       pre_diff_code=pre_diff_code,
                                       *args, **kwargs)


# /////////////////////////////////////////////////////////////////////////////
#                           QPSK demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class qpsk_demod(generic_demod):
    """
    Hierarchical block for RRC-filtered QPSK demodulation.

    The input is the complex modulated signal at baseband and the
    output is a stream of bits packed 1 bit per byte (LSB)

    Args:
        mod_code: Whether to use a gray_code (digital.mod_codes.GRAY_CODE) or not (digital.mod_codes.NO_CODE).
        differential: Whether to use differential encoding (boolean).
    """
    # See generic_mod for additional arguments
    __doc__ += shared_demod_args

    def __init__(self, mod_code=_def_mod_code, differential=False,
                 *args, **kwargs):
        pre_diff_code = True
        if not differential:
            constellation = digital.constellation_qpsk()
            if mod_code != mod_codes.GRAY_CODE:
                raise ValueError("This QPSK mod/demod works only for gray-coded constellations.")
        else:
            constellation = digital.constellation_dqpsk()
            if mod_code not in set([mod_codes.GRAY_CODE, mod_codes.NO_CODE]):
                raise ValueError("That mod_code is not supported for DQPSK mod/demod.")
            if mod_code == mod_codes.NO_CODE:
                pre_diff_code = False

        super(qpsk_demod, self).__init__(constellation=constellation,
                                         differential=differential,
                                         pre_diff_code=pre_diff_code,
                                         *args, **kwargs)



# /////////////////////////////////////////////////////////////////////////////
#                           DQPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def dqpsk_constellation(mod_code=_def_mod_code):
    if mod_code != mod_codes.GRAY_CODE:
        raise ValueError("The DQPSK constellation is only generated for gray_coding.  But it can be used for non-grayed coded modulation if one doesn't use the pre-differential code.")
    return digital.constellation_dqpsk()

# /////////////////////////////////////////////////////////////////////////////
#                           DQPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class dqpsk_mod(qpsk_mod):
    """
    Hierarchical block for RRC-filtered DQPSK modulation.

    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.

    Args:
        mod_code: Whether to use a gray_code (digital.mod_codes.GRAY_CODE) or not (digital.mod_codes.NO_CODE).
    """
    # See generic_mod for additional arguments
    __doc__ += shared_mod_args

    def __init__(self, mod_code=_def_mod_code, *args, **kwargs):
        super(dqpsk_mod, self).__init__(mod_code=mod_code,
                                        differential=True,
                                        *args, **kwargs)

# /////////////////////////////////////////////////////////////////////////////
#                           DQPSK demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class dqpsk_demod(qpsk_demod):
    """
    Hierarchical block for RRC-filtered DQPSK demodulation.

    The input is the complex modulated signal at baseband and the
    output is a stream of bits packed 1 bit per byte (LSB)

    Args:
        mod_code: Whether to use a gray_code (digital.mod_codes.GRAY_CODE) or not (digital.mod_codes.NO_CODE).
    """
    # See generic_mod for additional arguments
    __doc__ += shared_demod_args

    def __init__(self, mod_code=_def_mod_code, *args, **kwargs):
        super(dqpsk_demod, self).__init__(mod_code=mod_code,
                                          differential=True,
                                          *args, **kwargs)

#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_mod('qpsk', qpsk_mod)
modulation_utils.add_type_1_demod('qpsk', qpsk_demod)
modulation_utils.add_type_1_constellation('qpsk', qpsk_constellation)
modulation_utils.add_type_1_mod('dqpsk', dqpsk_mod)
modulation_utils.add_type_1_demod('dqpsk', dqpsk_demod)
modulation_utils.add_type_1_constellation('dqpsk', dqpsk_constellation)
