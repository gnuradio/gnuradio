#
# Copyright 2005,2006 Free Software Foundation, Inc.
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
doesn't work for non-differential encodings.
"""

from gnuradio import gr, modulation_utils2
from gnuradio.blks2impl.generic_mod_demod import generic_mod


# Default number of points in constellation.
_def_constellation_points = 4
# Whether differential coding is used.
_def_differential = False
_def_gray_coded = True

# /////////////////////////////////////////////////////////////////////////////
#                           QPSK constellation
# /////////////////////////////////////////////////////////////////////////////

def qpsk_constellation(m=_def_constellation_points): 
    if m != _def_constellation_points:
        raise ValueError("QPSK can only have 4 constellation points.")
    return gr.constellation_qpsk()

# /////////////////////////////////////////////////////////////////////////////
#                           QPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class qpsk_mod(generic_mod):

    def __init__(self, constellation_points=_def_constellation_points,
                 differential=_def_differential,
                 gray_coded=_def_gray_coded,
                 *args, **kwargs):

        """
	Hierarchical block for RRC-filtered QPSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        See generic_mod block for list of parameters.
	"""

        constellation = gr.constellation_qpsk()
        if constellation_points != 4:
            raise ValueError("QPSK can only have 4 constellation points.")
        if differential or not gray_coded:
            raise ValueError("This QPSK mod/demod works only for gray-coded, non-differential.")
        super(qpsk_mod, self).__init__(constellation, differential, gray_coded, *args, **kwargs)

#
# Add these to the mod/demod registry
#
modulation_utils2.add_type_1_mod('qpsk', qpsk_mod)
modulation_utils2.add_type_1_constellation('qpsk', qpsk_constellation)
