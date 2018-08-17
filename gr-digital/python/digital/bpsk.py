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
