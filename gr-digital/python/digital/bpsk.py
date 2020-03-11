#
# Copyright 2005,2006,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

"""
BPSK modulation and demodulation.
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from math import pi, log
from cmath import exp

from gnuradio import gr
from gnuradio.digital.generic_mod_demod import generic_mod, generic_demod
from gnuradio.digital.generic_mod_demod import shared_mod_args, shared_demod_args
from . import digital_swig
from . import modulation_utils

# /////////////////////////////////////////////////////////////////////////////
#                           BPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def bpsk_constellation():
    return digital_swig.constellation_bpsk()

# /////////////////////////////////////////////////////////////////////////////
#                           DBPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def dbpsk_constellation():
    return digital_swig.constellation_dbpsk()


#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_constellation('bpsk', bpsk_constellation)
modulation_utils.add_type_1_constellation('dbpsk', dbpsk_constellation)
