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
from __future__ import absolute_import
from __future__ import unicode_literals

from gnuradio import gr
from gnuradio.digital.generic_mod_demod import generic_mod, generic_demod
from gnuradio.digital.generic_mod_demod import shared_mod_args, shared_demod_args
from .utils import mod_codes
from . import digital_swig as digital
from . import modulation_utils

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
#                           DQPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def dqpsk_constellation(mod_code=_def_mod_code):
    if mod_code != mod_codes.GRAY_CODE:
        raise ValueError("The DQPSK constellation is only generated for gray_coding.  But it can be used for non-grayed coded modulation if one doesn't use the pre-differential code.")
    return digital.constellation_dqpsk()

#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_constellation('qpsk', qpsk_constellation)
modulation_utils.add_type_1_constellation('dqpsk', dqpsk_constellation)
