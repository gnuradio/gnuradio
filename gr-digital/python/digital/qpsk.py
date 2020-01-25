#
# Copyright 2005,2006,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
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
