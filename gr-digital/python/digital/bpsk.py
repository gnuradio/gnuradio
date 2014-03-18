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

from math import pi, log
from cmath import exp

from gnuradio import gr
from gnuradio.digital.generic_mod_demod import generic_mod, generic_demod
from gnuradio.digital.generic_mod_demod import shared_mod_args, shared_demod_args
import digital_swig
import modulation_utils

# /////////////////////////////////////////////////////////////////////////////
#                           BPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def bpsk_constellation():
    return digital_swig.constellation_bpsk()

# /////////////////////////////////////////////////////////////////////////////
#                           BPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class bpsk_mod(generic_mod):
    """
    Hierarchical block for RRC-filtered BPSK modulation.

    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.

    Args:
        mod_code: Argument is not used.  It exists purely to simplify generation of the block in grc.
        differential: Whether to use differential encoding (boolean).
    """
    # See generic_mod for additional arguments
    __doc__ += shared_mod_args

    def __init__(self, mod_code=None, differential=False, *args, **kwargs):

        constellation = digital_swig.constellation_bpsk()
        super(bpsk_mod, self).__init__(constellation=constellation,
                                       differential=differential, *args, **kwargs)


# /////////////////////////////////////////////////////////////////////////////
#                           BPSK demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class bpsk_demod(generic_demod):
    """
    Hierarchical block for RRC-filtered BPSK demodulation.

    The input is the complex modulated signal at baseband and the
    output is a stream of bits packed 1 bit per byte (LSB)

    Args:
        mod_code: Argument is not used.  It exists purely to simplify generation of the block in grc.
        differential: whether to use differential encoding (boolean)
    """
    # See generic_demod for additional arguments
    __doc__ += shared_demod_args

    def __init__(self, mod_code=None, differential=False, *args, **kwargs):
        constellation = digital_swig.constellation_bpsk()
        super(bpsk_demod, self).__init__(constellation=constellation,
                                         differential=differential, *args, **kwargs)
#bpsk_demod.__doc__ += shared_demod_args


# /////////////////////////////////////////////////////////////////////////////
#                           DBPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def dbpsk_constellation():
    return digital_swig.constellation_dbpsk()

# /////////////////////////////////////////////////////////////////////////////
#                           DBPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class dbpsk_mod(bpsk_mod):
    """
    Hierarchical block for RRC-filtered DBPSK modulation.

    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.

    Args:
        mod_code: Argument is not used.  It exists purely to simplify generation of the block in grc.
    """
    # See generic_mod for additional arguments
    __doc__ += shared_mod_args

    def __init__(self, mod_code=None, *args, **kwargs):

        super(dbpsk_mod, self).__init__(differential=True, *args, **kwargs)

# /////////////////////////////////////////////////////////////////////////////
#                           DBPSK demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class dbpsk_demod(bpsk_demod):
    """
    Hierarchical block for RRC-filtered DBPSK demodulation.

    The input is the complex modulated signal at baseband and the
    output is a stream of bits packed 1 bit per byte (LSB)

    Args:
        mod_code: Argument is not used.  It exists purely to simplify generation of the block in grc.
    """
    # See generic_demod for additional arguments
    __doc__ += shared_demod_args

    def __init__(self, mod_code=None, *args, **kwargs):

        super(dbpsk_demod, self).__init__(differential=True, *args, **kwargs)

#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_mod('bpsk', bpsk_mod)
modulation_utils.add_type_1_demod('bpsk', bpsk_demod)
modulation_utils.add_type_1_constellation('bpsk', bpsk_constellation)
modulation_utils.add_type_1_mod('dbpsk', dbpsk_mod)
modulation_utils.add_type_1_demod('dbpsk', dbpsk_demod)
modulation_utils.add_type_1_constellation('dbpsk', dbpsk_constellation)
