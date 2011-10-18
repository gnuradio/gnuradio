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
import digital_swig
import modulation_utils

# Default number of points in constellation.
_def_constellation_points = 4
# Whether gray coding is used.
_def_gray_coded = True

# /////////////////////////////////////////////////////////////////////////////
#                           QPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def qpsk_constellation(m=_def_constellation_points): 
    if m != _def_constellation_points:
        raise ValueError("QPSK can only have 4 constellation points.")
    return digital_swig.constellation_qpsk()

# /////////////////////////////////////////////////////////////////////////////
#                           QPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class qpsk_mod(generic_mod):

    def __init__(self, constellation_points=_def_constellation_points,
                 gray_coded=_def_gray_coded,
                 *args, **kwargs):

        """
	Hierarchical block for RRC-filtered QPSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        See generic_mod block for list of parameters.
	"""

        constellation_points = _def_constellation_points
        constellation = digital_swig.constellation_qpsk()
        if constellation_points != 4:
            raise ValueError("QPSK can only have 4 constellation points.")
        if not gray_coded:
            raise ValueError("This QPSK mod/demod works only for gray-coded constellations.")
        super(qpsk_mod, self).__init__(constellation=constellation,
                                       gray_coded=gray_coded, 
                                       *args, **kwargs)


# /////////////////////////////////////////////////////////////////////////////
#                           QPSK demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class qpsk_demod(generic_demod):

    def __init__(self, constellation_points=_def_constellation_points,
                 *args, **kwargs):

        """
	Hierarchical block for RRC-filtered QPSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        See generic_demod block for list of parameters.
        """

        constellation_points = _def_constellation_points
        constellation = digital_swig.constellation_qpsk()
        if constellation_points != 4:
            raise ValueError('Number of constellation points must be 4 for QPSK.')
        super(qpsk_demod, self).__init__(constellation=constellation,
                                         *args, **kwargs)



# /////////////////////////////////////////////////////////////////////////////
#                           DQPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def dqpsk_constellation(m=_def_constellation_points):
    if m != _def_constellation_points:
        raise ValueError("DQPSK can only have 4 constellation points.")
    return digital_swig.constellation_dqpsk()

# /////////////////////////////////////////////////////////////////////////////
#                           DQPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class dqpsk_mod(generic_mod):

    def __init__(self, constellation_points=_def_constellation_points,
                 gray_coded=_def_gray_coded,
                 differential=True, *args, **kwargs):
        """
	Hierarchical block for RRC-filtered DQPSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        See generic_mod block for list of parameters.
	"""

        constellation_points = _def_constellation_points
        constellation = digital_swig.constellation_dqpsk()
        if constellation_points != 4:
            raise ValueError('Number of constellation points must be 4 for DQPSK.')
        super(dqpsk_mod, self).__init__(constellation=constellation,
                                        gray_coded=gray_coded,
                                        differential=True,
                                        *args, **kwargs)

# /////////////////////////////////////////////////////////////////////////////
#                           DQPSK demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class dqpsk_demod(generic_demod):

    def __init__(self, constellation_points=_def_constellation_points,
                 differential=True, *args, **kwargs):

        """
	Hierarchical block for RRC-filtered DQPSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        See generic_demod block for list of parameters.
        """
        constellation_points = _def_constellation_points
        constellation = digital_swig.constellation_dqpsk()
        if constellation_points != 4:
            raise ValueError('Number of constellation points must be 4 for DQPSK.')
        super(dqpsk_demod, self).__init__(constellation=constellation,
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

